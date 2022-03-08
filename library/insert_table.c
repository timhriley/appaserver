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
#include "folder_attribute.h"
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

INSERT_TABLE *insert_table_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *message,
			POST_DICTIONARY *post_dictionary )
{
	INSERT_TABLE *insert_table = insert_table_calloc();

	/* Process */
	/* ------- */
	if ( ! ( insert_table->role =
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
		return (INSERT_TABLE *)0;
	}

	if ( ! ( insert_table->folder =
			folder_fetch(
				folder_name,
				role_name /* fetching role_folder_list */,
				role_exclude_lookup_attribute_name_list(
					insert_table->
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
				0 /* not fetch_role_folder_list */,
				0 /* not fetch_row_level_restriction */,
				0 /* not fetch_role_operation_list */ ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );
		return (INSERT_TABLE *)0;
	}

	if ( insert_table_forbid(
		role_folder_insert(
			insert_table->folder->role_folder_list ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: insert_table_forbid() returned true.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (INSERT_TABLE *)0;
	}

	insert_table->dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_folder_new(
			post_dictionary->original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list(
				insert_table->
					folder->
					folder_attribute_append_isa_list ) );

	return insert_table;
}

boolean insert_table_forbid(
			boolean role_folder_insert )
{
	return 1 - role_folder_insert;
}

char *insert_table_output_system_string(
			char *executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !executable
	||   !login_name
	||   !session_key
	||   !folder_name
	||   !role_name
	||   !dictionary_separate_send_string
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
		"%s %s %s %s %s \"%s\" 2>>%s",
		executable,
		login_name,
		session_key,
		folder_name,
		role_name,
		dictionary_separate_send_string,
		appaserver_error_filename );

	return strdup( system_string );
}

