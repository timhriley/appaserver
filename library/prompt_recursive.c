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
#include "environ.h"
#include "relation.h"
#include "query.h"
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
			boolean drillthru_skipped )
{
	PROMPT_RECURSIVE *prompt_recursive;

	if ( drillthru_skipped ) return (PROMPT_RECURSIVE *)0;

	prompt_recursive = prompt_recursive_calloc();

	prompt_recursive->folder_name = folder_name;
	prompt_recursive->drillthru_dictionary = drillthru_dictionary;

	prompt_recursive->prompt_recursive_folder_list =
		prompt_recursive_folder_list(
			relation_mto1_non_isa_list,
			drillthru_dictionary,
			login_name );

	if ( !list_length( prompt_recursive->prompt_recursive_folder_list ) )
	{
		return (PROMPT_RECURSIVE *)0;
	}

	prompt_recursive_set_javascript(
		prompt_recursive->
			prompt_recursive_folder_list );

	prompt_recursive->element_list =
		prompt_recursive_element_list(
			prompt_recursive->
				prompt_recursive_folder_list );

	return prompt_recursive;
}

LIST *prompt_recursive_folder_list(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name )
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
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ! ( prompt_recursive_folder =
				prompt_recursive_folder_new(
					relation->one_folder,
					relation->drop_down_multi_select,
					drillthru_dictionary,
					login_name ) ) )
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
			char *login_name )
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

	one_folder->relation_mto1_non_isa_list =
		relation_mto1_non_isa_list(
			one_folder->folder_name );

	one_folder->delimited_list =
		query_primary_delimited_fetch_list(
			one_folder->folder_name,
			one_folder->primary_key_list,
			one_folder->folder_attribute_primary_list,
			one_folder->relation_mto1_non_isa_list,
			drillthru_dictionary,
			login_name );

	prompt_recursive_folder = prompt_recursive_folder_calloc();

	prompt_recursive_folder->one_folder = one_folder;

	prompt_recursive_folder->drop_down_multi_select =
		drop_down_multi_select;

	prompt_recursive_folder->relation_mto1_primary_key_subset_list =
		relation_mto1_primary_key_subset_list(
			one_folder->folder_name,
			one_folder->primary_key_list );

	prompt_recursive_folder->prompt_recursive_mto1_folder_list =
		prompt_recursive_mto1_folder_list(
			prompt_recursive_folder->
				relation_mto1_primary_key_subset_list,
			drillthru_dictionary,
			login_name );

	return prompt_recursive_folder;
}

LIST *prompt_recursive_mto1_folder_list(
			LIST *relation_mto1_primary_key_subset_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name )
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
				login_name ) );

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
			char *login_name )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *recursive_mto1_folder =
		prompt_recursive_mto1_folder_calloc();

	one_folder->relation_mto1_non_isa_list =
		relation_mto1_non_isa_list(
			one_folder->folder_name );

	one_folder->delimited_list =
		query_primary_delimited_fetch_list(
			one_folder->folder_name,
			one_folder->primary_key_list,
			one_folder->folder_attribute_primary_list,
			one_folder->relation_mto1_non_isa_list,
			drillthru_dictionary,
			login_name );

	recursive_mto1_folder->one_folder = one_folder;
	recursive_mto1_folder->drop_down_multi_select = drop_down_multi_select;

	return recursive_mto1_folder;
}

LIST *prompt_recursive_element_list(
			LIST *prompt_recursive_folder_list )
{
	LIST *element_list = {0};
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;

	if ( !list_rewind( prompt_recursive_folder ) ) return (LIST *)0;

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
			one_folder,
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

#ifdef NOT_DEFINED
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;
	char *no_display_push_button_prefix = {0};
	char *no_display_push_button_heading = {0};
	boolean set_first_initial_data;
	boolean output_null_option;
	boolean output_not_null_option;
	/* boolean output_select_option; */

	if ( !omit_push_buttons )
	{
		no_display_push_button_prefix = NO_DISPLAY_PUSH_BUTTON_PREFIX;
		no_display_push_button_heading = NO_DISPLAY_PUSH_BUTTON_HEADING;
	}

	recursive_element_list = list_new_list();

	if ( prompt_folder )
	{
		list_append_pointer(
			done_folder_name_list,
			strdup( get_done_folder_name(
				prompt_folder->folder_name,
				(char *)0 /* related_attribute_name */ ) ) );
	}

	*post_change_javascript =
		prompt_recursive_get_gray_drop_downs_javascript(
			prompt_folder,
			prompt_folder_drop_down_multi_select,
			prompt_recursive_mto1_folder_list );

	if ( prompt_folder
	&&   lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_not_root
	&&   lookup_before_drop_down_is_fulfilled_folder(
		lookup_before_drop_down->
			lookup_before_drop_down_folder_list,
		prompt_folder->folder_name ) )
	{
		set_first_initial_data = 1;
		output_null_option = 0;
		output_not_null_option = 0;
		/* output_select_option = 0; */
	}
	else
	if ( prompt_folder
	&&   lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_is_root_all_complete
	&&   lookup_before_drop_down_is_fulfilled_folder(
		lookup_before_drop_down->
			lookup_before_drop_down_folder_list,
		prompt_folder->folder_name ) )
	{
		set_first_initial_data = 1;
		output_null_option = 0;
		output_not_null_option = 0;
		/* output_select_option = 1; */
	}
	else
	{
		set_first_initial_data = 0;
		output_null_option = 1;
		output_not_null_option = 1;
		/* output_select_option = 1; */
	}

	if ( prompt_folder )
	{
		list_append_list(
			recursive_element_list,
			related_folder_prompt_element_list(
				(RELATED_FOLDER **)0,
				application_name,
				session,
				role_name,
				login_name,
				prompt_folder->folder_name,
				prompt_folder->populate_drop_down_process,
				prompt_folder->attribute_list,
				attribute_primary_key_list(
					prompt_folder->attribute_list ),
				0 /* dont omit_ignore_push_buttons */,
				preprompt_dictionary,
	  			no_display_push_button_prefix,
	  			no_display_push_button_heading,
				*post_change_javascript,
				prompt_folder->notepad /* hint_message */,
				0 /* max_drop_down_size */,
				(LIST *)0 /* common_non_primary_a... */,
				0 /* not is_primary_attribute */,
				prompt_folder->row_level_non_owner_view_only,
				prompt_folder->row_level_non_owner_forbid,
				(char *)0 /* related_attribute_name */,
				prompt_folder_drop_down_multi_select,
				prompt_folder->no_initial_capital,
				(char *)0 /* state */,
				(char *)0 /* one2m_folder_name_for_processes */,
				0 /* tab_index */,
				set_first_initial_data,
				output_null_option,
				output_not_null_option,
				1 /* output_select_option */,
				(char *)0
				/* appaserver_user_foreign_login_name */,
			        0 /* not omit_lookup_before_drop_down */
				) );
	
		list_append_string_list(
			exclude_attribute_name_list,
			attribute_primary_key_list(
				prompt_folder->attribute_list ) );
	}

	if ( !list_rewind( prompt_recursive_mto1_folder_list ) )
		return recursive_element_list;

	do {
		prompt_recursive_mto1_folder =
			list_get_pointer( prompt_recursive_mto1_folder_list );

		list_append_pointer(
			done_folder_name_list,
			strdup( get_done_folder_name(
				prompt_recursive_mto1_folder->
					folder->
					folder_name,
				(char *)0 /* related_attribute_name */ ) ) );

		list_append_list(
			recursive_element_list,
			related_folder_prompt_element_list(
				(RELATED_FOLDER **)0,
				application_name,
				session,
				role_name,
				login_name,
				prompt_recursive_mto1_folder->
					folder->
					folder_name,
				prompt_recursive_mto1_folder->
					folder->
					populate_drop_down_process,
				prompt_recursive_mto1_folder->
					folder->
					attribute_list,
				attribute_primary_key_list(
					prompt_recursive_mto1_folder->
						folder->
						attribute_list ),
				0 /* dont omit_ignore_push_buttons */,
				preprompt_dictionary,
	  			no_display_push_button_prefix,
	  			no_display_push_button_heading,
				*post_change_javascript,
				prompt_recursive_mto1_folder->
					folder->
					notepad,
				0 /* max_drop_down_size */,
				(LIST *)0 /* common_non_primary_a... */,
				0 /* not is_primary_attribute */,
				prompt_recursive_mto1_folder->
					folder->
					row_level_non_owner_view_only,
				prompt_recursive_mto1_folder->
					folder->
					row_level_non_owner_forbid,
				(char *)0 /* related_attribute_name */,
				0 /* drop_down_multi_select */,
				prompt_recursive_mto1_folder->
					folder->
					no_initial_capital,
				(char *)0 /* state */,
				(char *)0 /* one2m_folder_name_for_processes */,
				0 /* tab_index */,
				0 /* not set_first_initial_data */,
				1 /* output_null_option */,
				1 /* output_not_null_option */,
				1 /* output_select_option */,
				(char *)0
				/* appaserver_user_foreign_login_name */,
			        0 /* not omit_lookup_before_drop_down */
				) );

			list_append_string_list(
				exclude_attribute_name_list,
				attribute_primary_key_list(
					prompt_recursive_mto1_folder->
						folder->
						attribute_list ) );

	} while( list_next( prompt_recursive_mto1_folder_list ) );

	return element_list;
#endif

char *prompt_recursive_folder_javascript(
			FOLDER *one_folder,
			boolean drop_down_multi_select,
			LIST *prompt_recursive_mto1_folder_list )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *mto1_folder;
	char javascript[ 1024 ];
	char *ptr = javascript;
	int element_index;

	element_index = prompt_folder_drop_down_multi_select;

	ptr +=
		sprintf( ptr,
			 "timlib_gray_mutually_exclusive_drop_downs('" );

	ptr +=
		sprintf( ptr,
			 "%s_%d",
			 list_display_delimited(
				one_folder->primary_key_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ),
			 element_index );

	if ( !list_rewind( prompt_mto1_recursive_folder_list ) )
	{
		sprintf( ptr, "');" );
		return strdup( javascript );
	}

	do {
		mto1_folder =
			list_get(
				prompt_mto1_recursive_folder_list );

		if ( !list_at_first( prompt_mto1_recursive_folder_list ) )
		{
			ptr += sprintf( ptr, "," );
		}

		ptr +=
			sprintf( ptr,
			 "%s_%d",
			 list_display_delimited(
				mto1_folder->one_folder->primary_key_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ),
			 0 /* element_index */ );

	} while( list_next( prompt_mto1_recursive_folder_list ) );

	sprintf( ptr, "');" );

	return strdup( javascript );
}

void prompt_recursive_set_javascript(
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
				prompt_recursive_folder->one_folder,
				prompt_recursive_folder->drop_down_multi_select,
				prompt_recursive_folder->mto1_folder_list );

	} while ( list_next( prompt_recursive_folder_list ) );
}

LIST *prompt_recursive_one_folder_element_list(
			FOLDER *one_folder,
			boolean drop_down_multi_select,
			char *javascript )
{
	char buffer[ 256 ];
	char element_name[ 256 ];
	LIST *element_list;
	APPASERVER_ELEMENT *element;
	char relation_operator_equals[ 256 ];
	boolean set_option_data_option_label_list;

	return_list = list_new_list();

	/* Create the line break */
	/* --------------------- */
	element = element_appaserver_new( linebreak, "" );

	list_set( element_list, element );

	sprintf(element_name,
	 	"%s%s",
		NO_DISPLAY_PUSH_BUTTON_PREFIX,
	 	list_display_delimited(
		  	one_folder->primary_key_list,
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
	sprintf( element_name,
	 	 "*%s",
	 	 one_folder->folder_name );

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
		  	one_folder->primary_key_list,
		  	MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER));

	if ( drop_down_multi_select )
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

	element->drop_down->post_change_javascript =
		post_change_javascript;

	element->drop_down->no_initial_capital = folder->no_initial_capital;
	element->tab_index = 1;

	list_set( element_list, element );

	set_option_data_option_label_list = 1;

	if ( !drop_down_multi_select
	&&   !omit_lookup_before_drop_down
	&&   ajax_fill_drop_down_related_folder
	&&   !*ajax_fill_drop_down_related_folder
			/* Can only execute this once.		     */
			/* Therefore, only one ajaxed mto1 relation. */ )
	{
		FOLDER *folder;

		folder = folder_with_load_new(
				application_name,
				session,
				folder_name,
				(ROLE *)0 );

		if ( ( *ajax_fill_drop_down_related_folder =
		       related_folder_get_ajax_fill_drop_down_related_folder(
			folder->mto1_related_folder_list ) ) )
		{
			char *onclick_function;

			/* Create the fill button element */
			/* ------------------------------ */
			element =
				element_appaserver_new(
					push_button, 
					(char *)0 /* element_name */ );

			element->push_button->label =
				RELATED_FOLDER_AJAX_FILL_LABEL;

			onclick_function =
				related_folder_ajax_onclick_function(
					foreign_attribute_name_list );

			element->push_button->onclick_function =
				onclick_function;

			list_append_pointer(
				return_list, 
				element );

			set_option_data_option_label_list = 0;
		}
	}

	if ( set_option_data_option_label_list )
	{
		element_drop_down_set_option_data_option_label_list(
			&element->drop_down->option_data_list,
			&element->drop_down->option_label_list,
			folder_prompt_primary_data_list(
				application_name,
				session,
				folder_name,
				login_name,
				preprompt_dictionary,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
				populate_drop_down_process,
				attribute_list,
				common_non_primary_attribute_name_list,
				( row_level_non_owner_view_only ||
				  row_level_non_owner_forbid )
					/* filter_only_login_name */,
				role_name,
				state,
				one2m_folder_name_for_processes,
				appaserver_user_foreign_login_name ) );
	}

	if ( set_first_initial_data
	&&   list_length( element->drop_down->option_data_list ) )
	{
		element->drop_down->initial_data =
			list_get_first_pointer(
				element->drop_down->option_data_list );
	}

	/* Create the hint message */
	/* ----------------------- */
	if ( hint_message && *hint_message )
	{
		element = 
			element_appaserver_new(
				non_edit_text,
				hint_message );

		list_append_pointer(
			return_list, 
			element );
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
	ELEMENT_APPASERVER *element;
	char relation_operator_equals[ 256 ];
	boolean set_option_data_option_label_list;

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
			mto1_folder->one_folder->primary_delimited_list );

		if ( list_length( element->drop_down->option_data_list ) )
		{
			element->drop_down->initial_data =
				list_first(
					element->drop_down->option_data_list );
		}

		/* Create the hint message */
		/* ----------------------- */
		if ( *mto1_folder->one_folder->hint_message )
		{
			element = 
				element_appaserver_new(
					non_edit_text,
					mto1_folder->one_folder->hint_message );

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
