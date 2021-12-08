/* $APPASERVER_HOME/library/edit_table.c				*/
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
#include "appaserver_library.h"
#include "edit_table.h"

EDIT_TABLE *edit_table_calloc( void )
{
	EDIT_TABLE *edit_table;

	if ( ! ( edit_table =
			calloc( 1, sizeof( EDIT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return edit_table;
}

EDIT_TABLE *edit_table_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			boolean frameset_menu_horizontal,
			DICTIONARY *query_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *ignore_select_attribute_name_list )
{
	EDIT_TABLE *edit_table = edit_table_calloc();

	/* Process */
	/* ------- */
	if ( ! ( edit_table->role =
			role_fetch(
				role_name,
				1 /* fetch_attribute_exclude_list */ ) ) )
 	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );

		return (EDIT_TABLE_FORM *)0;
	}

	if ( ! ( edit_table->folder =
			folder_fetch(
				folder_name,
				/* ------------------------- */
				/* Fetching role_folder_list */
				/* ------------------------- */
				role_name,
				role_exclude_lookup_attribute_name_list(
					edit_table->
						role->
						attribute_exclude_list ),
				/* -------------------------- */
				/* Also sets primary_key_list */
				/* -------------------------- */
				1 /* fetch_folder_attribute_list */,
				0 /* not fetch_relation_mto1_non_isa_list */,
				/* ------------------------------------------ */
				/* Also sets folder_attribute_append_isa_list */
				/* ------------------------------------------ */
				1 /* fetch_relation_mto1_isa_list */,
				1 /* fetch_relation_one2m_list */,
				0 /* not fetch_relation_one2m_recursive_list */,
				0 /* not fetch_process */,
				1 /* fetch_role_folder_list */,
				1 /* fetch_row_level_restriction */,
				1 /* fetch_role_operation_list */ ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );

		return (EDIT_TABLE *)0;
	}

	edit_table->folder->join_one2m_relation_list =
		relation_join_one2m_list(
			edit_table->folder->relation_one2m_recursive_list,
			ignore_dictionary );

	if ( ! ( edit_table->query =
			query_edit_table(
				edit_table->folder,
				query_dictionary ) ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: query_edit_table(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );

		return (EDIT_TABLE *)0;
	}

	edit_table->state =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		edit_table_state(
			edit_table->
				folder->
				role_folder_list );

	edit_table->with_dynarch_menu =
		edit_table_with_dynarch_menu(
			target_frame );

	edit_table->row_insert_count =
		edit_table_row_insert_count(
			non_prefixed_dictionary );

	edit_table->cell_update_count =
		edit_table_cell_update_count(
			non_prefixed_dictionary );

	edit_table->results_string =
		edit_table_results_string(
			non_prefixed_dictionary );

	edit_table->primary_key_non_edit =
		edit_table_primary_keys_non_edit(
			list_length(
				edit_table->
					folder->
					relation_mto1_isa_list_length ) );

	edit_table->row_security =
		row_security_edit_table(
			folder_name,
			edit_table->folder->folder_attribute_append_isa_list,
			drillthru_dictionary,
			edit_table->primary_keys_non_edit,
			edit_table->folder->role_operation_list,
			edit_table->state );

	edit_table->document =
		document_edit_table(

	return edit_table;
}

char *edit_table_state( LIST *role_folder_list )
{
	if ( role_folder_update( role_folder_list ) )
		return APPASERVER_UPDATE_STATE;

	if ( role_folder_lookup( role_folder_list ) )
		return APPASERVER_VIEW_ONLY_STATE;

	return (char *)0;
}

boolean edit_table_primary_keys_non_edit(
			int relation_mto1_isa_list_length )
{
	return ( relation_mto1_isa_list_length >= 1 );
}

int edit_table_row_insert_count(
			DICTIONARY *non_prefixed_dictionary )
{
	char *row_insert_count;

	if ( ( row_insert_count =
		dictionary_get(
			ROWS_INSERTED_COUNT_KEY
			non_prefixed_dictionary ) ) )
	{
		return atoi( row_insert_count );
	}
	else
	{
		return 0;
	}
}

int edit_table_cell_update_count(
			DICTIONARY *non_prefixed_dictionary )
{
	char *cell_update_count;

	if ( ( cell_update_count =
		dictionary_get(
			COLUMNS_UPDATED_KEY,
			non_prefixed_dictionary ) ) )
	{
		return atoi( cell_update_count );
	}
	else
	{
		return 0;
	}
}

char *edit_table_cell_update_folder_list_string(
			DICTIONARY *non_prefixed_dictionary )
{
	return
		dictionary_get(
			COLUMNS_UPDATED_CHANGED_FOLDER_KEY,
			non_prefixed_dictionary );
}

char *edit_table_results_string(
			DICTIONARY *non_prefixed_dictionary )
{
	return
		dictionary_get(
			non_prefixed_dictionary,
			RESULTS_STRING_KEY );
}

boolean edit_table_content_type(
			boolean with_dynarch_menu )
{
	return with_dynarch_menu;
}

