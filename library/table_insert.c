/* $APPASERVER_HOME/library/table_insert.c				*/
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
#include "folder_attribute.h"
#include "table_insert.h"

TABLE_INSERT *table_insert_calloc( void )
{
	TABLE_INSERT *table_insert;

	if ( ! ( table_insert = calloc( 1, sizeof( TABLE_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return table_insert;
}

TABLE_INSERT *table_insert_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *message,
			boolean menu_horizontal_boolean,
			DICTIONARY *original_post_dictionary )
{
	TABLE_INSERT *table_insert = table_insert_calloc();

	/* Process */
	/* ------- */
	if ( ! ( table_insert->role =
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
		return (TABLE_INSERT *)0;
	}

	if ( ! ( table_insert->folder =
			folder_fetch(
				folder_name,
				role_name /* fetching role_folder_list */,
				role_exclude_lookup_attribute_name_list(
					table_insert->
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
				1 /* fetch_relation_one2m_recursive_list */,
				1 /* fetch_process */,
				1 /* fetch_role_folder_list */,
				1 /* fetch_row_level_restriction */,
				0 /* not fetch_role_operation_list */ ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );
		return (TABLE_INSERT *)0;
	}

	if ( table_insert_forbid(
		role_folder_insert(
			table_insert->folder->role_folder_list ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: table_insert_forbid() returned true.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (TABLE_INSERT *)0;
	}

	table_insert->dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_table_insert_new(
			original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list(
				table_insert->
					folder->
					folder_attribute_append_isa_list ),
			table_insert->folder->folder_attribute_append_isa_list,
			table_insert->folder->folder_attribute_name_list );

	return table_insert;
}

boolean table_insert_forbid(
			boolean role_folder_insert )
{
	return 1 - role_folder_insert;
}

char *table_insert_output_system_string(
			char *executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !executable
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s %s %s \"%s\" 2>>%s",
		executable,
		session_key,
		login_name,
		role_name,
		folder_name,
		target_frame,
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		appaserver_error_filename );

	return strdup( system_string );
}

