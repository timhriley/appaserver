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
#include <malloc.h>
#include "environ.h"
#include "relation.h"
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
			boolean drillthru_skipped )
{
	PROMPT_RECURSIVE *prompt_recursive;

	if ( drillthru_skipped ) return (PROMPT_RECURSIVE *)0;

	prompt_recursive->folder_name = folder_name;
	prompt_recursive->drillthru_dictionary = drillthru_dictionary;

	prompt_recursive->prompt_recursive_folder_list =
		prompt_recursive_folder_list(
			relation_mto1_non_isa_list,
			drillthru_dictionary );

	if ( !list_length( prompt_recursive->prompt_recursive_folder_list ) )
	{
		return (PROMPT_RECURSIVE *)0;
	}

	prompt_recursive->element_list =
		prompt_recursive_element_list(
			prompt_recursive->prompt_recursive_folder_list );

	return prompt_recursive;
}

LIST *prompt_recursive_folder_list(
			LIST *relation_mto1_non_isa_list )
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
					/* ------------------------------ */
					/* Exclude drop_down_multi_select */
					/* ------------------------------ */
					relation->drop_down_multi_select ) ) )
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
			boolean drop_down_multi_select )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	RELATION *relation;

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

	if ( drop_down_multi_select ) return (PROMPT_RECURSIVE_FOLDER *)0;

	prompt_recursive_folder = prompt_recursive_folder_calloc();

	prompt_recursive_folder->one_folder = one_folder;

	prompt_recursive_folder->relation_mto1_primary_key_subset_list =
		relation_mto1_primary_key_subset_list(
			one_folder->folder_name,
			one_folder->primary_key_list );

	if ( !list_rewind(
		prompt_recursive_folder->
			relation_mto1_primary_key_subset_list ) )
	{
		return (PROMPT_RECURSIVE_FOLDER *)0;
	}

	prompt_recursive_folder->prompt_recursive_mto1_folder_list =
		list_new();

	do {
		relation =
			list_get(
				prompt_recursive_folder->
					relation_mto1_primary_key_subset_list );

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
			prompt_recursive_folder->
				prompt_recursive_mto1_folder_list,
			prompt_recursive_mto1_folder_new(
				relation->one_folder ) );

	} while ( list_next( 
		prompt_recursive_folder->
			relation_mto1_primary_key_subset_list ) );

	return prompt_recursive_folder;
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
			FOLDER *one_folder )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *recursive_mto1_folder =
		prompt_recursive_mto1_folder_calloc();

	recursive_mto1_folder->one_folder = one_folder;
	return recursive_mto1_folder;
}

LIST *prompt_recursive_element_list(
			LIST *prompt_recursive_folder_list,
			DICTIONARY *drillthru_dictionary )
{
	LIST *element_list = list_new();
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;


	return element_list;

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
}
