/* library/pair_one2m.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "related_folder.h"
#include "dictionary_appaserver.h"
#include "timlib.h"
#include "pair_one2m.h"

PAIR_ONE2M *pair_one2m_calloc( void )
{
	PAIR_ONE2M *pair_one2m;


	if ( ! ( pair_one2m =
			(PAIR_ONE2M *)calloc( 1, sizeof( PAIR_ONE2M ) ) ) )
	{
		fprintf( stderr, 
			 "%s/%s(): calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}
	return pair_one2m;
}

PAIR_ONE2M *pair_one2m_new(
			LIST *ignore_attribute_name_list	/* out only */,
			DICTIONARY *pair_one2m_dictionary	/* in/out   */,
			char *application_name			/* in only  */,
			LIST *pair_one2m_related_folder_list	/* in only  */,
			LIST *posted_attribute_name_list	/* in only  */,
			ROLE *role				/* in only  */,
			char *session				/* in only  */)
{
	PAIR_ONE2M *pair_one2m = pair_one2m_calloc();
	char *element_name;
	char *submit_button_pressed;

	pair_one2m->is_participating =
		!pair_one2m_omit( pair_one2m_dictionary )
		&& ( list_length( pair_one2m_related_folder_list ) > 0 );

	if ( !pair_one2m->is_participating )
	{
		return pair_one2m;
	}

	pair_one2m->pair_one2m_related_folder_list =
		pair_one2m_related_folder_list;

	pair_one2m->pair_one2m_folder_name_list =
		related_folder_get_one2m_related_folder_name_list(
			pair_one2m_related_folder_list );

	if ( !pair_one2m_dictionary ) return pair_one2m;

	if ( ! ( pair_one2m->pair_inserted_folder_name_list =
	 		pair_one2m_get_pair_inserted_folder_name_list(
				pair_one2m_dictionary ) ) )
	{
		pair_one2m->pair_inserted_folder_name_list = list_new();
	}

	pair_one2m->inserted_duplicate =
		pair_one2m_get_inserted_duplicate(
			pair_one2m_dictionary );

	/* Check if did them all. */
	/* ---------------------- */
	if  ( !( pair_one2m->next_folder_name =
		pair_one2m_get_next_folder_name(
			pair_one2m->pair_inserted_folder_name_list,
			pair_one2m_related_folder_list ) ) )
	{
		pair_one2m->insert_is_completed = 1;
	}

	/* Check if pressed the <Submit> button or a specified submit button. */
	/* ------------------------------------------------------------------ */
	element_name =
		pair_one2m_get_pair_one2m_submit_element_name(
			1 /* with_suffix_zero */ );

	if ( ( submit_button_pressed =
			dictionary_fetch(
				pair_one2m_dictionary,
				element_name ) ) )
	{
		/* If just pressed a submit button on the top frame. */
		/* ------------------------------------------------- */
		if ( strcmp(	submit_button_pressed,
				PAIR_ONE2M_OMIT ) == 0 )
		{
			pair_one2m->submit_button_on_top_frame = 1;
			return pair_one2m;
		}
		else
		/* -------------------------------- */
		/* Else pressed a specified button. */
		/* -------------------------------- */
		{
			pair_one2m->specified_folder_name =
				submit_button_pressed;

			/* ------------------------------------ */
			/* Only display "One row inserted" only	*/
			/* the first time.			*/
			/* ------------------------------------ */
			dictionary_delete_key(
				pair_one2m_dictionary,
				element_name );
		}
	}
	else
	{
		pair_one2m->continuing_series = 1;
	}

	if ( pair_one2m->specified_folder_name )
	{
		pair_one2m->pair_insert_folder =
			folder_new_folder(
				application_name,
				session,
				pair_one2m->specified_folder_name );
	}
	else
	if ( pair_one2m->next_folder_name )
	{
		pair_one2m->pair_insert_folder =
			folder_new_folder(
				application_name,
				session,
				pair_one2m->next_folder_name );
	}
	else
	{
		return pair_one2m;
	}

	pair_one2m->pair_insert_folder->mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new(),
			application_name,
			session,
			pair_one2m->pair_insert_folder->folder_name,
			role->role_name,
			0 /* isa_flag */,
			related_folder_no_recursive,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			(LIST *)0 /* root_primary_att..._name_list */,
			0 /* recursive_level */ );

	pair_one2m->pair_insert_folder->mto1_isa_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			session,
			pair_one2m->pair_insert_folder->folder_name,
			role->role_name,
			1 /* isa_flag */,
			related_folder_recursive_all,
			role_get_override_row_restrictions(
			role->override_row_restrictions_yn ),
			(LIST *)0 /* root_primary_att..._name_list */,
			0 /* recursive_level */ );
	
	pair_one2m->pair_insert_folder->attribute_list =
		attribute_get_attribute_list(
			application_name,
			pair_one2m->pair_insert_folder->folder_name,
			(char *)0 /* attribute_name */,
			pair_one2m->
				pair_insert_folder->
				mto1_isa_related_folder_list,
			role->role_name );

	folder_load(
		&pair_one2m->pair_insert_folder->insert_rows_number,
		&pair_one2m->pair_insert_folder->lookup_email_output,
		&pair_one2m->pair_insert_folder->row_level_non_owner_forbid,
		&pair_one2m->pair_insert_folder->row_level_non_owner_view_only,
		&pair_one2m->pair_insert_folder->populate_drop_down_process,
		&pair_one2m->pair_insert_folder->post_change_process,
		&pair_one2m->pair_insert_folder->folder_form,
		&pair_one2m->pair_insert_folder->notepad,
		&pair_one2m->pair_insert_folder->html_help_file_anchor,
		&pair_one2m->pair_insert_folder->post_change_javascript,
		&pair_one2m->pair_insert_folder->lookup_before_drop_down,
		&pair_one2m->pair_insert_folder->data_directory,
		&pair_one2m->pair_insert_folder->index_directory,
		&pair_one2m->pair_insert_folder->no_initial_capital,
		&pair_one2m->pair_insert_folder->subschema_name,
		&pair_one2m->pair_insert_folder->create_view_statement,
		application_name,
		session,
		pair_one2m->pair_insert_folder->folder_name,
		role_get_override_row_restrictions(
			role->
			override_row_restrictions_yn ),
		role->role_name,
		(LIST *)0 /* mto1_related_folder_list */ );

	pair_one2m->pair_insert_folder->attribute_name_list =
		folder_get_attribute_name_list(
			pair_one2m->pair_insert_folder->attribute_list );

	list_append_string_list(
		ignore_attribute_name_list,
		posted_attribute_name_list );

	/* Add this folder to the already done list. */
	/* ----------------------------------------- */
	list_append_pointer(
		pair_one2m->pair_inserted_folder_name_list,
		pair_one2m->pair_insert_folder->folder_name );

	dictionary_set_pointer(
		pair_one2m_dictionary,
		PAIR_ONE2M_FOLDER_LIST,
		list_display_delimited(
			pair_one2m->pair_inserted_folder_name_list,
			',' ) );

	return pair_one2m;

}

LIST *pair_one2m_get_pair_inserted_folder_name_list(
					DICTIONARY *pair_one2m_dictionary )
{
	LIST *pair_inserted_folder_name_list = {0};
	char *pair_1tom_folder_name_list_string;

	if ( !pair_one2m_dictionary ) return (LIST *)0;

	if ( dictionary_length( pair_one2m_dictionary ) )
	{
		pair_1tom_folder_name_list_string =
			dictionary_get_string(
				pair_one2m_dictionary,
				PAIR_ONE2M_FOLDER_LIST );

		if ( pair_1tom_folder_name_list_string
		&&   *pair_1tom_folder_name_list_string )
		{
			search_replace_string(
					pair_1tom_folder_name_list_string,
					"\\",
					"" );

			pair_inserted_folder_name_list =
				list_string2list(
					pair_1tom_folder_name_list_string,
					',' );
		}
	}
	return pair_inserted_folder_name_list;
}

boolean pair_one2m_get_inserted_duplicate(
			DICTIONARY *pair_one2m_dictionary )
{
	char *data;

	if ( !pair_one2m_dictionary ) return 0;

	if ( ( data = dictionary_get_pointer(
			pair_one2m_dictionary,
			PAIR_ONE2M_DUPLICATE_STATE_KEY ) ) )
	{
		if ( strcmp( data, "yes" ) == 0 )
		{
			/* Only display the duplicate message the first time. */
			/* -------------------------------------------------- */
			dictionary_set_pointer(
				pair_one2m_dictionary,
				PAIR_ONE2M_DUPLICATE_STATE_KEY,
				"none" );

			return 1;
		}
	}
	return 0;
}

char *pair_one2m_get_pair_one2m_submit_element_name(
			boolean with_suffix_zero )
{
	static char element_name[ 128 ];

	if ( with_suffix_zero )
	{
		sprintf(element_name,
		 	"%s_0",
		 	PAIR_ONE2M_SUBMIT_FOLDER );
	}
	else
	{
		sprintf(element_name,
		 	"%s%s",
		 	PAIR_ONE2M_PREFIX,
		 	PAIR_ONE2M_SUBMIT_FOLDER );
	}
	return element_name;
}

char *pair_one2m_get_next_folder_name(	LIST *pair_inserted_folder_name_list,
					LIST *pair_one2m_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	/* ---------------- */
	/* Stay on sequence */
	/* ---------------- */
	boolean ok_start_checking = 0;

	if ( !list_rewind( pair_one2m_related_folder_list ) )
		return (char *)0;

	/* If pressed a button on the top frame. */
	/* ------------------------------------- */
	if ( !list_length( pair_inserted_folder_name_list ) )
		ok_start_checking = 1;

	do {
		related_folder =
			list_get_pointer(
				pair_one2m_related_folder_list );

		if ( !ok_start_checking
		&&   related_folder->pair_1tom_order
		&&   list_exists_string(
			pair_inserted_folder_name_list,
			related_folder->
				one2m_folder->
					folder_name ) )
		{
			ok_start_checking = 1;
			continue;
		}

		if ( related_folder->pair_1tom_order
		&&   !list_exists_string(
			pair_inserted_folder_name_list,
			related_folder->
				one2m_folder->
					folder_name )
		&&   ok_start_checking )
		{
			return related_folder->
				one2m_folder->
					folder_name;
		}

	} while( list_next( pair_one2m_related_folder_list ) );

	return (char *)0;
}

boolean pair_one2m_omit( DICTIONARY *pair_1tom_dictionary )
{
	char *data;
	char key[ 128 ];

	sprintf( key, "%s_0", PAIR_ONE2M_SUBMIT_FOLDER );

	data = dictionary_fetch( pair_1tom_dictionary, key );

	return ( timlib_strcmp( data, PAIR_ONE2M_OMIT ) == 0 );
}

