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
#include "dictionary_separate.h"
#include "query.h"
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

	prompt_recursive_folder_list_set_javascript(
		prompt_recursive->
			prompt_recursive_folder_list );

	prompt_recursive->element_list =
		prompt_recursive_folder_list_element_list(
			prompt_recursive->
				prompt_recursive_folder_list );

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

	if ( !list_length( one_folder->relation_mto1_nnon_isa_list ) )
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

	prompt_recursive_folder->mto1_folder_list =
		prompt_recursive_mto1_folder_list(
			prompt_recursive_folder->
				relation_mto1_primary_key_subset_list,
			drillthru_dictionary,
			login_name,
			security_entity );

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

LIST *prompt_recursive_folder_list_element_list(
			LIST *prompt_recursive_folder_list )
{
	LIST *element_list = {0};
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;

	if ( !list_rewind( prompt_recursive_folder_list ) )
		return (LIST *)0;

	do {
		prompt_recursive_folder =
			list_get(
				prompt_recursive_folder_list );

		prompt_recursive_folder->element_list =
			prompt_recursive_folder_element_list(
				prompt_recursive_folder->one_folder,
				prompt_recursive_folder->
					drop_down_multi_select,
				prompt_recursive_folder->javascript,
				prompt_recursive_folder->
					mto1_folder_list );

		if ( list_length( prompt_recursive_folder->element_list ) )
		{
			if ( !element_list ) element_list = list_new();

			list_set_list(
				element_list,
				prompt_recursive_folder->element_list );
		}
	} while ( list_next( prompt_recursive_folder_list ) );

	return element_list;
}

LIST *prompt_recursive_folder_element_list(
			FOLDER *one_folder,
			boolean drop_down_multi_select,
			char *javascript,
			LIST *mto1_folder_list )
{
	LIST *element_list;
	PROMPT_RECURSIVE_MTO1_FOLDER *mto1_folder;

	if ( !one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( mto1_folder_list ) ) return (LIST *)0;

	element_list = 
		prompt_recursive_one_folder_element_list(
			one_folder->folder_name,
			one_folder->primary_key_list,
			drop_down_multi_select,
			javascript );

	return
		prompt_recursive_mto1_folder_element_list(
			/* -------------------- */
			/* Returns element_list */
			/* -------------------- */
			element_list,
			javascript,
			mto1_folder_list );
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
		mto1_folder =
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
				mto1_folder->one_folder->primary_key_list,
				SQL_DELIMITER ),
			 0 /* element_index */ );

	} while( list_next( relation_mto1_primary_key_subset_list ) );

	sprintf( ptr, "');" );

	return strdup( javascript );
}

void prompt_recursive_folder_list_set_javascript(
			LIST *prompt_recursive_folder_list )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;

	if ( !list_rewind( prompt_recursive_folder_list ) ) return;

	do {
		prompt_recursive_folder =
			list_get(
				prompt_recursive_folder_list );

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

	} while ( list_next( prompt_recursive_folder_list ) );
}

LIST *prompt_recursive_one_folder_element_list(
			char *folder_name,
			LIST *primary_key_list,
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

	/* Create the line break */
	/* --------------------- */
	element = appaserver_element_new( linebreak );

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
	element = appaserver_element_new( prompt );

	sprintf(element_name,
	 	"*%s",
	 	one_folder->folder_name );

	element->prompt->html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		element_prompt_html(
			strdup(
				format_initial_capital_not_parens(
					buffer, 
					element_name ) ) );

	list_set( element_list, element );

	/* Create the drop-down element */
	/* ---------------------------- */
	sprintf(element_name,
	 	"%s",
	 	list_display_delimited(
		  	primary_key_list,
		  	SQL_DELIMITER ) );

	if ( drop_down_multi_select )
	{
here1
		char drop_down_element_name[ 128 ];

		sprintf(drop_down_element_name,
			"%s%s",
			QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL,
			element_name );

		element =
			element_appaserver_new(
				drop_down,
				strdup( drop_down_element_name ) );

		element->drop_down->multi_select = 1;

		element->drop_down->multi_select_element_name =
			strdup( element_name );
	}
	else
	{
		element =
			element_appaserver_new(
				drop_down,
				strdup( element_name ) );

		element->drop_down->output_null_option = 1;
		element->drop_down->output_not_null_option = 1;
		element->drop_down->output_select_option = 1;
	}

	element->drop_down->post_change_javascript = javascript;
	element->drop_down->no_initial_capital = one_folder->no_initial_capital;
	element->tab_index = 1;

	list_set( element_list, element );

	element_drop_down_set_option_data_option_label_list(
		&element->drop_down->option_data_list,
		&element->drop_down->option_label_list,
		one_folder->delimited_list );

	if ( !drop_down_multi_select
	&&   list_length( element->drop_down->option_data_list ) )
	{
		element->drop_down->initial_data =
			list_first(
				element->drop_down->option_data_list );
	}

	/* Create the hint message */
	/* ----------------------- */
	if ( *one_folder->notepad )
	{
		element = 
			element_appaserver_new(
				non_edit_text,
				one_folder->notepad );

		list_set( element_list, element );
	}

	/* Create the hidden equals operator */
	/* --------------------------------- */
	sprintf( relation_operator_equals,
		 "%s%s",
		 QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 element_name );

	element =
		element_appaserver_new(
			hidden,
			strdup( relation_operator_equals ) );

	element->hidden->data = "equals";

	list_set( element_list, element );

	return element_list;
}

LIST *prompt_recursive_mto1_folder_element_list(
			/* -------------------- */
			/* Returns element_list */
			/* -------------------- */
			LIST *element_list,
			char *javascript,
			LIST *mto1_folder_list )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *mto1_folder;
	char buffer[ 256 ];
	char element_name[ 256 ];
	APPASERVER_ELEMENT *element;
	char relation_operator_equals[ 256 ];

	if ( !list_length( element_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( mto1_folder_list ) )
		return element_list;

	do {
		mto1_folder =
			list_get( mto1_folder_list );


		/* Create the line break */
		/* --------------------- */
		element = element_appaserver_new( linebreak, "" );

		list_set( element_list, element );

		sprintf(element_name,
	 		"%s%s",
			NO_DISPLAY_PUSH_BUTTON_PREFIX,
	 		list_display_delimited(
		  		mto1_folder->one_folder->primary_key_list,
		  		MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) );

		element =
			element_appaserver_new(
				toggle_button, 
				strdup( element_name ) );

		element_toggle_button_set_heading(
			element->toggle_button,
			NO_DISPLAY_PUSH_BUTTON_PREFIX );

		list_set( element_list, element );

		/* Create the prompt element */
		/* ------------------------- */
		sprintf(element_name,
		 	"*%s",
		 	strdup( mto1_folder->one_folder->folder_name ) );

		element =
			element_appaserver_new(
				prompt,
				strdup( format_initial_capital_not_parens(
						buffer, 
						element_name ) ) );

		list_set( element_list, element );

		sprintf(element_name, 
	 		"%s",
	 		list_display_delimited(
		  		mto1_folder->one_folder->primary_key_list,
		  		MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER));

		if ( mto1_folder->drop_down_multi_select )
		{
			char drop_down_element_name[ 128 ];

			sprintf(drop_down_element_name,
				"%s%s",
				QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL,
				element_name );

			element =
				element_appaserver_new(
					drop_down,
					strdup( drop_down_element_name ) );

			element->drop_down->multi_select = 1;

			element->drop_down->multi_select_element_name =
				strdup( element_name );
		}
		else
		{
			element =
				element_appaserver_new(
					drop_down,
					strdup( element_name ) );

			element->drop_down->output_null_option = 1;
			element->drop_down->output_not_null_option = 1;
			element->drop_down->output_select_option = 1;
		}

		element->drop_down->post_change_javascript = javascript;

		list_set( element_list, element );

		element_drop_down_set_option_data_option_label_list(
			&element->drop_down->option_data_list,
			&element->drop_down->option_label_list,
			mto1_folder->one_folder->delimited_list );

		if ( list_length( element->drop_down->option_data_list ) )
		{
			element->drop_down->initial_data =
				list_first(
					element->drop_down->option_data_list );
		}

		/* Create the hint message */
		/* ----------------------- */
		if ( *mto1_folder->one_folder->notepad )
		{
			element = 
				element_appaserver_new(
					non_edit_text,
					mto1_folder->one_folder->notepad );

			list_set( element_list, element );
		}

		/* Create the hidden equals operator */
		/* --------------------------------- */
		sprintf( relation_operator_equals,
		 	"%s%s",
		 	QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 	element_name );

		element =
			element_appaserver_new(
				hidden,
				strdup( relation_operator_equals ) );

		element->hidden->data = "equals";

		list_set( element_list, element );

	} while( list_next( mto1_folder_list ) );

	return element_list;
}
