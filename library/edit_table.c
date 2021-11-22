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

EDIT_TABLE *edit_table_fetch(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			POST_DICTIONARY *post_dictionary )
{
	EDIT_TABLE *edit_table = edit_table_calloc();

	/* Input */
	/* ----- */
	edit_table->application_name = application_name;
	edit_table->login_name = login_name;
	edit_table->session = session;
	edit_table->folder_name = folder_name;
	edit_table->role_name = role_name;
	edit_table->insert_update_key = insert_update_key;
	edit_table->target_frame = target_frame;
	edit_table->post_dictionary = post_dictionary;

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
				role_exclude_lookup_attribute_name_list(
					role->attribute_exclude_list ),
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
				0 /* not fetch_role_folder_list */,
				0 /* not fetch_row_level_restriction */ ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );

		return (EDIT_TABLE *)0;
	}

	if ( ! ( edit_table->role_folder_list =
			role_folder_fetch_list(
				role_name,
				folder_name ) ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: role_folder_fetch_list(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name,
			folder_name );

		return (EDIT_TABLE_FORM *)0;
	}

	if ( ! ( edit_table->edit_table_state =
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			edit_table_state(
				edit_table_form->
					role_folder_list ) ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: edit_table_state(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name,
			folder_name );

		return (EDIT_TABLE_FORM *)0;
	}

	edit_table->dictionary_appaserver =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_appaserver_stream_new(
			edit_table->post_dictionary,
			edit_table->application_name,
			edit_table->login_name,
			attribute_name_list(
				edit_table->
					folder->
					attribute_list ),
			attribute_date_name_list(
				edit_table->
					folder->
					attribute_list ) );

	edit_table->primary_key_non_edit =
		edit_table_primary_keys_non_edit(
			list_length(
				edit_table->
					folder->
					relation_mto1_isa_list_length ) );

	edit_table->folder->join_one2m_relation_list =
		relation_join_one2m_list(
			edit_table->
				folder->
				relation_one2m_list,
			dictionary_appaserver->ignore_dictionary );

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
