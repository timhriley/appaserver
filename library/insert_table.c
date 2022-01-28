/* $APPASERVER_HOME/library/insert_table.c				*/
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
#include "insert_table.h"

INSERT_TABLE *insert_table_calloc( void )
{
	INSERT_TABLE *insert_table;

	if ( ! ( insert_table = calloc( 1, sizeof( INSERT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_table;
}

INSERT_TABLE *insert_table_output_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *message,
			char *dictionary_string )
{
	INSERT_TABLE *insert_table = insert_table_calloc();

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
		exit( 1 );
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

	edit_table_form->dictionary_appaserver =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_appaserver_stream_new(
			insert_table_form->post_dictionary,
			insert_table_form->application_name,
			insert_table_form->login_name,
			attribute_name_list(
				insert_table_form->
					folder->
					attribute_list ),
			attribute_date_name_list(
				insert_table_form->
					folder->
					attribute_list ) );

	return insert_table_form;
}

boolean insert_table_form_forbid(
			boolean role_folder_insert )
{
	return 1 - role_folder_insert;
}

