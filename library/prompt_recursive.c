/* $APPASERVER_HOME/library/prompt_recursive.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "sql.h"
#include "timlib.h"
#include "environ.h"
#include "relation.h"
#include "appaserver_library.h"
#include "application.h"
#include "dictionary_separate.h"
#include "query.h"
#include "appaserver.h"
#include "element.h"
#include "prompt_recursive.h"

PROMPT_RECURSIVE *prompt_recursive_calloc(
			void )
{
	PROMPT_RECURSIVE *prompt_recursive;

	if ( ! ( prompt_recursive =
			calloc( 1, sizeof( PROMPT_RECURSIVE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return prompt_recursive;
}

PROMPT_RECURSIVE *prompt_recursive_new(
			char *folder_name,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			boolean non_owner_forbid,
			boolean override_row_restrictions,
			boolean drillthru_skipped )
{
	PROMPT_RECURSIVE *prompt_recursive;

	if ( drillthru_skipped ) return (PROMPT_RECURSIVE *)0;

	prompt_recursive = prompt_recursive_calloc();

	prompt_recursive->folder_name = folder_name;

	prompt_recursive->security_entity =
		security_entity_new(
			login_name,
			non_owner_forbid,
			override_row_restrictions );

	prompt_recursive->prompt_recursive_folder_list =
		prompt_recursive_folder_list(
			relation_mto1_non_isa_list,
			drillthru_dictionary,
			login_name,
			prompt_recursive->security_entity );

	if ( !list_length( prompt_recursive->prompt_recursive_folder_list ) )
	{
		free( prompt_recursive );
		return (PROMPT_RECURSIVE *)0;
	}

	return prompt_recursive;
}

LIST *prompt_recursive_folder_list(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			SECURITY_ENTITY *security_entity )
{
	LIST *folder_list = {0};
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	RELATION *relation;

	if ( !list_rewind( relation_mto1_non_isa_list ) ) return (LIST *)0;

	do {
		relation = list_get( relation_mto1_non_isa_list );

		if ( !relation->prompt_mto1_recursive ) continue;

		if ( !relation->one_folder )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !list_length( relation->one_folder->primary_key_list ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: for one_folder = %s, primary_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				relation->one_folder->folder_name );
			exit( 1 );
		}

		if ( ! ( prompt_recursive_folder =
				prompt_recursive_folder_new(
					relation->one_folder,
					relation->drop_down_multi_select,
					drillthru_dictionary,
					login_name,
					security_entity ) ) )
		{
			continue;
		}

		if ( !folder_list ) folder_list = list_new();

		list_set(
			folder_list,
			prompt_recursive_folder );

	} while( list_next( relation_mto1_non_isa_list ) );

	return folder_list;
}

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_calloc( void )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;

	prompt_recursive_folder =
		(PROMPT_RECURSIVE_FOLDER *)
			calloc( 1, sizeof( PROMPT_RECURSIVE_FOLDER ) );

	if ( !prompt_recursive_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_recursive_folder;
}

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_new(
			FOLDER *one_folder,
			boolean drop_down_multi_select,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			SECURITY_ENTITY *security_entity )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;

	if ( !one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( one_folder->primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( one_folder->relation_mto1_non_isa_list )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: one_folder->relation_mto1_non_isa_list is set. The next instruction is unnecessary.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
	}
	else
	{
		one_folder->relation_mto1_non_isa_list =
			relation_mto1_non_isa_list(
				one_folder->folder_name );
	}

	if ( !list_length( one_folder->relation_mto1_non_isa_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: relation_mto1_non_isa_list() returned empty. Unset relation->prompt_mto1_recursive.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PROMPT_RECURSIVE_FOLDER *)0;
	}

	one_folder->delimited_list =
		query_primary_delimited_fetch_list(
			one_folder->folder_name,
			one_folder->primary_key_list,
			one_folder->folder_attribute_primary_list,
			one_folder->relation_mto1_non_isa_list,
			drillthru_dictionary,
			login_name,
			security_entity_where(
				security_entity,
				one_folder->folder_attribute_list ) );

	prompt_recursive_folder = prompt_recursive_folder_calloc();

	prompt_recursive_folder->one_folder = one_folder;

	prompt_recursive_folder->drop_down_multi_select =
		drop_down_multi_select;

	prompt_recursive_folder->relation_mto1_primary_key_subset_list =
		relation_mto1_primary_key_subset_list(
			one_folder->folder_name,
			one_folder->primary_key_list );

	prompt_recursive_folder->javascript =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		prompt_recursive_folder_javascript(
			prompt_recursive_folder->
				one_folder->
				primary_key_list,
			prompt_recursive_folder->
				drop_down_multi_select,
			prompt_recursive_folder->
				relation_mto1_primary_key_subset_list );

	prompt_recursive_folder->mto1_folder_list =
		prompt_recursive_mto1_folder_list(
			prompt_recursive_folder->
				relation_mto1_primary_key_subset_list,
			drillthru_dictionary,
			login_name,
			security_entity );

	prompt_recursive_folder->one_element_list =
		prompt_recursive_folder_one_element_list(
			prompt_recursive_folder->
				one_folder->
				folder_name,
			prompt_recursive_folder->
				one_folder->
				primary_key_list,
			prompt_recursive_folder->
				one_folder->
				delimited_list,
			prompt_recursive_folder->
				one_folder->
				no_initial_capital,
			prompt_recursive_folder->
				one_folder->
				notepad,
			drop_down_multi_select,
			prompt_recursive_folder->javascript );

	prompt_recursive_folder->element_list =
		prompt_recursive_mto1_folder_element_list(
			/* --------------------------- */
			/* Appends to one_element_list */
			/* --------------------------- */
			prompt_recursive_folder->one_element_list,
			prompt_recursive_folder->mto1_folder_list,
			prompt_recursive_folder->javascript );

	return prompt_recursive_folder;
}

LIST *prompt_recursive_mto1_folder_list(
			LIST *relation_mto1_primary_key_subset_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			SECURITY_ENTITY *security_entity )
{
	RELATION *relation;
	LIST *mto1_folder_list;

	if ( !list_rewind( relation_mto1_primary_key_subset_list ) )
	{
		return (LIST *)0;
	}

	mto1_folder_list = list_new();

	do {
		relation = list_get( relation_mto1_primary_key_subset_list );

		if ( !relation->one_folder )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			mto1_folder_list,
			prompt_recursive_mto1_folder_new(
				relation->one_folder,
				relation->drop_down_multi_select,
				drillthru_dictionary,
				login_name,
				security_entity ) );

	} while ( list_next( relation_mto1_primary_key_subset_list ) );

	return mto1_folder_list;
}

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_calloc( void )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;

	if ( ! ( prompt_recursive_mto1_folder =
			calloc( 1, sizeof( PROMPT_RECURSIVE_MTO1_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_recursive_mto1_folder;
}

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_new(
			FOLDER *one_folder,
			boolean drop_down_multi_select,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			SECURITY_ENTITY *security_entity )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *recursive_mto1_folder =
		prompt_recursive_mto1_folder_calloc();

	recursive_mto1_folder->one_folder = one_folder;
	recursive_mto1_folder->drop_down_multi_select = drop_down_multi_select;

	if ( one_folder->relation_mto1_non_isa_list )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: relation_mto1_non_isa_list is set. The next instruction is unnecessary.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
	}
	else
	{
		one_folder->relation_mto1_non_isa_list =
			relation_mto1_non_isa_list(
				one_folder->folder_name );
	}

	one_folder->delimited_list =
		query_primary_delimited_fetch_list(
			one_folder->folder_name,
			one_folder->primary_key_list,
			one_folder->folder_attribute_primary_list,
			one_folder->relation_mto1_non_isa_list,
			drillthru_dictionary,
			login_name,
			security_entity_where(
				security_entity,
				one_folder->folder_attribute_list ) );

	return recursive_mto1_folder;
}

char *prompt_recursive_folder_javascript(
			LIST *primary_key_list,
			boolean drop_down_multi_select,
			LIST *relation_mto1_primary_key_subset_list )
{
	RELATION *relation_mto1;
	char javascript[ 1024 ];
	char *ptr = javascript;
	int element_index;

	if ( !list_rewind( relation_mto1_primary_key_subset_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: relation_mto1_primary_key_subset_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_index = (int)drop_down_multi_select;

	ptr +=
		sprintf( ptr,
			 "timlib_gray_mutually_exclusive_drop_downs('" );

	ptr +=
		sprintf( ptr,
			 "%s_%d",
			 list_display_delimited(
				primary_key_list,
				SQL_DELIMITER ),
			 element_index );

	do {
		relation_mto1 =
			list_get(
				relation_mto1_primary_key_subset_list );

		if ( !list_at_first( relation_mto1_primary_key_subset_list ) )
		{
			ptr += sprintf( ptr, "," );
		}

		ptr +=
			sprintf( ptr,
			 "%s_%d",
			 list_display_delimited(
				relation_mto1->one_folder->primary_key_list,
				SQL_DELIMITER ),
			 0 /* element_index */ );

	} while( list_next( relation_mto1_primary_key_subset_list ) );

	sprintf( ptr, "');" );

	return strdup( javascript );
}

LIST *prompt_recursive_mto1_folder_element_list(
			/* ------------------------ */
			/* Returns one_element_list */
			/* ------------------------ */
			LIST *one_element_list,
			LIST *mto1_folder_list,
			char *javascript )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *mto1_folder;

	if ( !list_length( one_element_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_element_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( mto1_folder_list ) )
		return one_element_list;

	do {
		mto1_folder = list_get( mto1_folder_list );

		list_set_list(
			one_element_list,
			prompt_recursive_folder_one_element_list(
				mto1_folder->one_folder->folder_name,
				mto1_folder->one_folder->primary_key_list,
				mto1_folder->one_folder->delimited_list,
				mto1_folder->one_folder->no_initial_capital,
				mto1_folder->one_folder->notepad,
				0 /* not drop_down_multi_select */,
				javascript ) );

	} while( list_next( mto1_folder_list ) );

	return one_element_list;
}

LIST *prompt_recursive_folder_one_element_list(
			char *folder_name,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *notepad,
			boolean drop_down_multi_select,
			char *javascript )
{
	char buffer[ 256 ];
	char prompt_display[ 256 ];
	char element_name[ 256 ];
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new();

	if ( !list_length( primary_key_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_name=%s has empty primary_key_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		exit( 1 );
	}

	/* Create the table row */
	/* -------------------- */
	element = appaserver_element_new( table_row );

	list_set( element_list, element );

	/* Create the no display checkbox */
	/* ------------------------------ */
	element = appaserver_element_new( checkbox ); 

	sprintf(element_name,
	 	"%s%s",
		NO_DISPLAY_PUSH_BUTTON_PREFIX,
	 	list_display_delimited(
		  	primary_key_list,
		  	SQL_DELIMITER ) );

	element->checkbox->name = strdup( element_name );

	element->checkbox->html =
		/* ---------------------------- */
		/* Returns heap memory or null. */
		/* ---------------------------- */
		element_checkbox_html(
			element->checkbox->name,
			string_initial_capital(
				prompt_display,
				NO_DISPLAY_PUSH_BUTTON_HEADING ),
			0 /* not checked */,
			(char *)0 /* action_string */,
			0 /* tab_order */,
			"yes" /* value */ );

	if ( !element->checkbox->html )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: element_checkbox_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set( element_list, element );

	/* Create the prompt element */
	/* ------------------------- */
	element = appaserver_element_new( non_edit_text );

	sprintf(element_name,
	 	"*%s",
	 	folder_name );

	element->non_edit_text->html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		element_non_edit_text_html(
			format_initial_capital_not_parens(
				buffer, 
				element_name ) );

	list_set( element_list, element );

	/* Create the drop-down element */
	/* ---------------------------- */
	if ( drop_down_multi_select )
	{
		element = appaserver_element_new( multi_drop_down );

		free( element->multi_drop_down );

		element->multi_drop_down =
			element_multi_drop_down_new(
				primary_key_list,
				delimited_list,
				no_initial_capital,
				javascript,
				APPASERVER_LOOKUP_STATE );
	}
	else
	{
		element = appaserver_element_new( drop_down );

		free( element->drop_down );

		element->drop_down =
			element_drop_down_new(
				primary_key_list /* attribute_name_list */,
				(char *)0 /* initial_data */,
				delimited_list,
				no_initial_capital,
				1 /* output_null_option */,
				1 /* output_not_null_option */,
				1 /* output_select_option */,
				1 /* column_span */,
				element_drop_down_size(
					list_length(
						delimited_list ) ),
				-1 /* tab_order */,
				0 /* not multi_select */,
				javascript,
				(char *)0 /* state */ );
	}

	list_set( element_list, element );

	/* Create the hint message */
	/* ----------------------- */
	if ( notepad )
	{
		element = appaserver_element_new( non_edit_text );

		element->non_edit_text->html =
			element_non_edit_text_html(
				notepad );

		list_set( element_list, element );
	}

	return element_list;
}

char *prompt_recursive_html(
			LIST *prompt_recursive_folder_list )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	char *folder_html;

	if ( !list_rewind( prompt_recursive_folder_list ) ) return (char *)0;

	do {
		prompt_recursive_folder =
			list_get(
				prompt_recursive_folder_list );

		if ( ( folder_html =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				prompt_recursive_folder_html(
					prompt_recursive_folder->
						element_list ) ) )
		{
			ptr += sprintf( ptr, "%s\n", folder_html );

			free( folder_html );
		}

	} while ( list_next( prompt_recursive_folder_list ) );

	return strdup( html );
}

char *prompt_recursive_folder_html(
			LIST *element_list )
{
	/* Returns heap memory or null */
	/* --------------------------- */
	return
	appaserver_element_list_html(
		application_background_color(
			environment_application_name() ),
		(char *)0 /* state */,
		0 /* row_number */,
		element_list );
}

