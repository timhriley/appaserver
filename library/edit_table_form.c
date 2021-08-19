/* $APPASERVER_HOME/library/edit_table_form.c				*/
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
#include "edit_table_form.h"

EDIT_TABLE_FORM *edit_table_form_calloc( void )
{
	EDIT_TABLE_FORM *edit_table_form;

	if ( ! ( edit_table_form =
			calloc( 1, sizeof( EDIT_TABLE_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return edit_table_form;
}

EDIT_TABLE_FORM *edit_table_form_fetch(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			DICTIONARY *post_dictionary )
{
	EDIT_TABLE_FORM *edit_table_form = edit_table_form_calloc();

	/* Input */
	/* ----- */
	edit_table_form->application_name = application_name;
	edit_table_form->login_name = login_name;
	edit_table_form->session = session;
	edit_table_form->folder_name = folder_name;
	edit_table_form->role_name = role_name;
	edit_table_form->insert_update_key = insert_update_key;
	edit_table_form->target_frame = target_frame;
	edit_table_form->post_dictionary = post_dictionary;

	/* Process */
	/* ------- */
	if ( ! ( edit_table_form->role =
			role_fetch(
				role_name,
				1 /* fetch_attribute_exclude_list */ ) ) )
 	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );
		return (EDIT_TABLE_FORM *)0;
	}

	if ( ! ( edit_table_form->folder =
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
	"ERROR in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );

		return (EDIT_TABLE_FORM *)0;
	}

	if ( ! ( edit_table_form->role_folder =
			role_folder_fetch(
				role_name,
				folder_name ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: role_folder_fetch(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name,
			folder_name );

		return (EDIT_TABLE_FORM *)0;
	}

	edit_table_form->edit_table_form_state =
		edit_table_form_state(
			edit_table_form->role_folder );

	if ( ! ( edit_table_form->dictionary_appaserver =
			dictionary_appaserver_new(
				edit_table_form->post_dictionary,
				edit_table_form->application_name,
				edit_table_form->folder->attribute_list,
				(LIST *)0 /* operation_name_list */,
				edit_table_form->login_name ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: dictionary_appaserver_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		return (EDIT_TABLE_FORM *)0;
	}

	edit_table_form->primary_key_non_edit =
		edit_table_form_primary_keys_non_edit(
			list_length(
				edit_table_form->
					folder->
					relation_mto1_isa_list_length ) );

	edit_table_form->folder->join_one2m_relation_list =
		relation_join_one2m_list(
			edit_table_form->
				folder->
				relation_one2m_list,
			dictionary_appaserver->ignore_dictionary );

	lookup_before_drop_down =
		lookup_before_drop_down_new(
			application_name,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			state );

	lookup_before_drop_down->lookup_before_drop_down_state =
		lookup_before_drop_down_get_state(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			dictionary_appaserver->preprompt_dictionary,
			folder->lookup_before_drop_down );

	return edit_table_form;
}

char *edit_table_form_state(
			ROLE_FOLDER *role_folder )
{
	if ( role_folder_view_only( role_folder ) )
		return "lookup";
	else
		return "update";
}

boolean edit_table_form_primary_keys_non_edit(
			int relation_mto1_isa_list_length )
{
	return ( relation_mto1_isa_list_length >= 1 );
}
