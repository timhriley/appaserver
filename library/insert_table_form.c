/* $APPASERVER_HOME/library/insert_table_form.c				*/
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
#include "insert_table_form.h"

INSERT_TABLE_FORM *insert_table_form_calloc( void )
{
	INSERT_TABLE_FORM *insert_table_form;

	if ( ! ( insert_table_form =
			calloc( 1, sizeof( INSERT_TABLE_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_table_form;
}

INSERT_TABLE_FORM *insert_table_form_fetch(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *target_frame,
			DICTIONARY *post_dictionary )
{
	INSERT_TABLE_FORM *insert_table_form = insert_table_form_calloc();

	/* Input */
	/* ----- */
	insert_table_form->application_name = application_name;
	insert_table_form->login_name = login_name;
	insert_table_form->session = session;
	insert_table_form->folder_name = folder_name;
	insert_table_form->role_name = role_name;
	insert_table_form->insert_update_key = insert_update_key;
	insert_table_form->target_frame = target_frame;
	insert_table_form->post_dictionary = post_dictionary;

	/* Process */
	/* ------- */
	if ( ! ( insert_table_form->role =
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
		return (INSERT_TABLE_FORM *)0;
	}

	if ( ! ( insert_table_form->folder =
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

		return (INSERT_TABLE_FORM *)0;
	}

	if ( ! ( insert_table_form->role_folder_list =
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

		return (INSERT_TABLE_FORM *)0;
	}

	if ( insert_table_form_forbid(
		role_folder_insert(
			insert_table_form->
				role_folder_list ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: insert_table_form_forbid() returned true.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (INSERT_TABLE_FORM *)0;
	}

	if ( ! ( edit_table_form->dictionary_appaserver =
			dictionary_appaserver_new(
				insert_table_form->post_dictionary,
				insert_table_form->application_name,
				attribute_name_list(
					insert_table_form->
						folder->
						attribute_list ),
				attribute_date_name_list(
					insert_table_form->
						folder->
						attribute_list ),
				(LIST *)0 /* operation_name_list */,
				insert_table_form->login_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: dictionary_appaserver_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		return (EDIT_TABLE_FORM *)0;
	}

	return insert_table_form;
}

boolean insert_table_form_forbid(
			boolean role_folder_insert )
{
	return 1 - role_folder_insert;
}

