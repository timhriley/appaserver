/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/delete_primary.c	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org 	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "query.h"
#include "security.h"
#include "sql.h"
#include "delete_primary.h"

DELETE_PRIMARY *delete_primary_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *primary_data_list_string,
		boolean delete_omit_isa_boolean )
{
	DELETE_PRIMARY *delete_primary;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !primary_data_list_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_primary = delete_primary_calloc();

	delete_primary->session_fetch =
		/* ----------------------- */
		/* If error then exit( 1 ) */
		/* ----------------------- */
		session_fetch(
			application_name,
			session_key,
			login_name );

	delete_primary->folder_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			folder_name /* datum */ );

	delete_primary->folder_attribute_fetch_primary_key_list =
		folder_attribute_fetch_primary_key_list(
			delete_primary->folder_name );

	if ( !list_length(
		delete_primary->
			folder_attribute_fetch_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"folder_attribute_fetch_primary_key_list(%s) returned empty.",
			delete_primary->folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_primary->query_cell_list =
		delete_primary_query_cell_list(
			SQL_DELIMITER,
			primary_data_list_string,
			delete_primary->
				folder_attribute_fetch_primary_key_list );

	delete_primary->delete =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		delete_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			delete_primary->query_cell_list,
			!delete_omit_isa_boolean
				/* isa_boolean (need to unwind) */,
			1 /* update_null_boolean */ );

	return delete_primary;
}

DELETE_PRIMARY *delete_primary_calloc( void )
{
	DELETE_PRIMARY *delete_primary;

	if ( ! ( delete_primary = calloc( 1, sizeof ( DELETE_PRIMARY ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_primary;
}

LIST *delete_primary_query_cell_list(
		char sql_delimiter,
		char *primary_data_list_string,
		LIST *folder_attribute_fetch_primary_key_list )
{
	int p;
	LIST *list;
	QUERY_CELL *query_cell;
	char datum[ 128 ];

	if ( !sql_delimiter
	||   !primary_data_list_string
	||   !list_rewind( folder_attribute_fetch_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	for (	p = 0;
		piece(	datum,
			sql_delimiter,
			primary_data_list_string,
			p );
		p++ )
	{
		query_cell =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_cell_parse(
				(LIST *)0 /* folder_attribute_list */,
				0 /* date_convert_format_enum */,
				list_get(
				   folder_attribute_fetch_primary_key_list )
					/* attribute_name */,
				strdup( datum ) );

		query_cell->primary_key_index = p + 1;

		list_set( list, query_cell );
		list_next( folder_attribute_fetch_primary_key_list );
	}

	return list;
}

