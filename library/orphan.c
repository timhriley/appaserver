/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/orphan.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and Freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "sql.h"
#include "piece.h"
#include "String.h"
#include "role.h"
#include "role_folder.h"
#include "orphan.h"

LIST *orphan_insert_statement_list(
		char *one_table_name,
		LIST *folder_attribute_primary_key_list,
		LIST *orphan_data_list )
{
	LIST *list;
	LIST *existing_data_list;
	char *orphan_data;

	if ( !one_table_name
	||   !list_length( folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	} 

	if ( !list_rewind( orphan_data_list ) ) return (LIST *)0;

	list = list_new();
	existing_data_list = list_new();

	do {
		orphan_data = list_get( orphan_data_list );

		if ( list_string_exists(
			orphan_data,
			existing_data_list ) )
		{
			continue;
		}

		list_set(
			list,
			orphan_insert_statement_new(
				one_table_name,
				folder_attribute_primary_key_list,
				orphan_data ) );

		list_set(
			existing_data_list,
			orphan_data );

	} while ( list_next( orphan_data_list ) );

	return list;
}

ORPHAN_INSERT_STATEMENT *orphan_insert_statement_new(
		char *one_table_name,
		LIST *folder_attribute_primary_key_list,
		char *orphan_data )
{
	char statement[ 1024 ];
	char *ptr = statement;
	int p;
	char data[ 128 ];
	ORPHAN_INSERT_STATEMENT *orphan_insert_statement;

	if ( !one_table_name
	||   !list_length( folder_attribute_primary_key_list )
	||   !orphan_data )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	orphan_insert_statement = orphan_insert_statement_calloc();

	orphan_insert_statement->one_table_name = one_table_name;
	orphan_insert_statement->orphan_data = orphan_data;

	ptr += sprintf(
		ptr,
		"insert into %s (%s) values ( ",
		one_table_name,
		list_display_delimited(
			folder_attribute_primary_key_list,
			',' ) );

	for (	p = 0;
		/* ------------------------------------- */
		/* Returns null if not enough delimiters */
		/* ------------------------------------- */
		piece(	data,
			SQL_DELIMITER,
			orphan_data,
			p );
		p++ )
	{
		if ( p ) ptr += sprintf( ptr, ", " );

		ptr += sprintf(
			ptr,
			"'%s'",
			data );
	}

	ptr += sprintf( ptr, " );" );

	orphan_insert_statement->statement = strdup( statement );

	return orphan_insert_statement;
}

ORPHAN_INSERT_STATEMENT *orphan_insert_statement_calloc( void )
{
	ORPHAN_INSERT_STATEMENT *orphan_insert_statement;

	if ( ! ( orphan_insert_statement =
			calloc( 1,
				sizeof ( ORPHAN_INSERT_STATEMENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan_insert_statement;
}

LIST *orphan_insert_list( LIST *orphan_subquery_list )
{
	LIST *list;
	ORPHAN_SUBQUERY *orphan_subquery;

	if ( !list_rewind( orphan_subquery_list ) ) return (LIST *)0;

	list = list_new();

	do {
		orphan_subquery =
			list_get(
				orphan_subquery_list );

		if ( list_length( orphan_subquery->orphan_data_list ) )
		{
			list_set(
				list,
				orphan_insert_new(
					orphan_subquery ) );
		}

	} while ( list_next( orphan_subquery_list ) );

	return list;
}

ORPHAN_INSERT *orphan_insert_new( ORPHAN_SUBQUERY *orphan_subquery )
{
	ORPHAN_INSERT *orphan_insert;

	if ( !orphan_subquery
	||   !orphan_subquery->relation_mto1
	||   !orphan_subquery->relation_mto1->one_folder
	||   !list_length(
		orphan_subquery->
			relation_mto1->
			one_folder->
			folder_attribute_primary_key_list )
	||   !orphan_subquery->one_table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( orphan_subquery->orphan_data_list ) )
		return (ORPHAN_INSERT *)0;

	orphan_insert = orphan_insert_calloc();

	orphan_insert->orphan_insert_statement_list =
		orphan_insert_statement_list(
			orphan_subquery->one_table_name,
			orphan_subquery->
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list,
			orphan_subquery->orphan_data_list );

	return orphan_insert;
}

ORPHAN_INSERT *orphan_insert_calloc( void )
{
	ORPHAN_INSERT *orphan_insert;

	if ( ! ( orphan_insert = calloc( 1, sizeof ( ORPHAN_INSERT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan_insert;
}

LIST *orphan_delete_list(
		LIST *orphan_subquery_list )
{
	LIST *list = list_new();
	ORPHAN_SUBQUERY *orphan_subquery;

	if ( list_rewind( orphan_subquery_list ) )
	do {
		orphan_subquery =
			list_get(
				orphan_subquery_list );

		if ( list_length(
			orphan_subquery->
				orphan_data_list ) )
		{
			list_set(
				list,
				orphan_delete_new(
					orphan_subquery->many_table_name,
					orphan_subquery->one_table_name,
					orphan_subquery->
						relation_mto1->
						relation_foreign_key_list,
					orphan_subquery->orphan_data_list ) );
		}

	} while ( list_next( orphan_subquery_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

ORPHAN_DELETE *orphan_delete_new(
		char *many_table_name,
		char *one_table_name,
		LIST *relation_foreign_key_list,
		LIST *orphan_data_list )
{
	ORPHAN_DELETE *orphan_delete;

	if ( !many_table_name
	||   !one_table_name
	||   !list_length( relation_foreign_key_list )
	||   !list_length( orphan_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	orphan_delete = orphan_delete_calloc();

	orphan_delete->many_table_name = many_table_name;
	orphan_delete->one_table_name = one_table_name;
	orphan_delete->relation_foreign_key_list = relation_foreign_key_list;

	orphan_delete->orphan_delete_statement_list =
		orphan_delete_statement_list(
			many_table_name,
			relation_foreign_key_list,
			orphan_data_list );

	return orphan_delete;
}

ORPHAN_DELETE *orphan_delete_calloc( void )
{
	ORPHAN_DELETE *orphan_delete;

	if ( ! ( orphan_delete = calloc( 1, sizeof ( ORPHAN_DELETE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan_delete;
}

ORPHAN_SUBQUERY *orphan_subquery_new(
		char *application_name,
		char *folder_name,
		RELATION_MTO1 *relation_mto1 )
{
	ORPHAN_SUBQUERY *orphan_subquery;

	if ( !application_name
	||   !folder_name
	||   !relation_mto1
	||   !relation_mto1->one_folder
	||   !list_length( relation_mto1->relation_foreign_key_list )
	||   !list_length(
		relation_mto1->
			one_folder->
			folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length(
		relation_mto1->
			relation_foreign_key_list ) !=
	     list_length(
		relation_mto1->
			one_folder->
			folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"list_length=%d <> list_length=%d.",
			list_length(
				relation_mto1->
					relation_foreign_key_list ),
			list_length(
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	orphan_subquery = orphan_subquery_calloc();

	orphan_subquery->relation_mto1 = relation_mto1;

	orphan_subquery->many_table_name =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				folder_name ) );

	orphan_subquery->one_table_name =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				relation_mto1->one_folder->folder_name ) );
	

	orphan_subquery->string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		orphan_subquery_string(
			relation_mto1,
			orphan_subquery->many_table_name,
			orphan_subquery->one_table_name );

	orphan_subquery->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		orphan_subquery_system_string(
			orphan_subquery->string );

	orphan_subquery->orphan_data_list =
		list_pipe(
			orphan_subquery->system_string );
	return orphan_subquery;
}

ORPHAN_SUBQUERY *orphan_subquery_calloc( void )
{
	ORPHAN_SUBQUERY *orphan_subquery;

	if ( ! ( orphan_subquery = calloc( 1, sizeof ( ORPHAN_SUBQUERY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan_subquery;
}

char *orphan_subquery_system_string( char *subquery_string )
{
	char system_string[ 2048 ];

	if ( !subquery_string )
	{
		char message[ 128 ];

		sprintf(message, "subquery_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s\" | "
		"sql.e | "
		/* ------------------------------------- */
		/* subquery returns empty lines for null */
		/* ------------------------------------- */
		"grep -v '^$' | "
		/* ------------------------------------------- */
		/* empty lines for multi-select have leading ^ */
		/* ------------------------------------------- */
		"grep -v '^\\^'",
		subquery_string );

	return strdup( system_string );
}

ORPHAN_FOLDER *orphan_folder_new(
		char *application_name,
		char *role_name,
		char *folder_name,
		boolean delete_boolean )
{
	ORPHAN_FOLDER *orphan_folder;
	RELATION_MTO1 *relation_mto1;
	ORPHAN_SUBQUERY *orphan_subquery;

	if ( !application_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	orphan_folder = orphan_folder_calloc();

	orphan_folder->folder_name = folder_name;

	orphan_folder->orphan_folder_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		orphan_folder_input_new(
			role_name,
			folder_name );

	orphan_folder->orphan_subquery_list = list_new();

	if ( list_rewind(
			orphan_folder->
				orphan_folder_input->
				relation_mto1_list ) )
	do {
		relation_mto1 =
			list_get(
				orphan_folder->
					orphan_folder_input->
					relation_mto1_list );

		orphan_subquery =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			orphan_subquery_new(
				application_name,
				folder_name,
				relation_mto1 );

		list_set(
			orphan_folder->orphan_subquery_list,
			orphan_subquery );

	} while ( list_next(
			orphan_folder->
				orphan_folder_input->
				relation_mto1_list ) );

	if ( list_rewind(
			orphan_folder->
				orphan_folder_input->
				relation_mto1_isa_list ) )
	do {
		relation_mto1 =
			list_get(
				orphan_folder->
					orphan_folder_input->
					relation_mto1_isa_list );

		list_set(
			orphan_folder->orphan_subquery_list,
			orphan_subquery_new(
				application_name,
				folder_name,
				relation_mto1 ) );
	
	} while ( list_next(
			orphan_folder->
				orphan_folder_input->
				relation_mto1_isa_list ) );

/*
orphan_subquery_string_stdout( orphan_folder->orphan_subquery_list );
*/

	if ( !delete_boolean )
	{
		orphan_folder->orphan_insert_list =
			orphan_insert_list(
				orphan_folder->
					orphan_subquery_list );
	}
	else
	{
		orphan_folder->orphan_delete_list =
			orphan_delete_list(
				orphan_folder->
					orphan_subquery_list );
	}

	return orphan_folder;
}

ORPHAN_FOLDER *orphan_folder_calloc( void )
{
	ORPHAN_FOLDER *orphan_folder;

	if ( ! ( orphan_folder = calloc( 1, sizeof ( ORPHAN_FOLDER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan_folder;
}

LIST *orphan_delete_statement_list(
		char *many_table_name,
		LIST *relation_foreign_key_list,
		LIST *orphan_data_list )
{
	LIST *list;
	char *orphan_data;

	if ( !many_table_name
	||   !list_length( relation_foreign_key_list )
	||   !list_rewind( orphan_data_list ) )
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

	do {
		orphan_data = list_get( orphan_data_list );

		list_set(
			list,
			orphan_delete_statement_new(
				many_table_name,
				relation_foreign_key_list,
				orphan_data ) );

	} while ( list_next( orphan_data_list ) );

	return list;
}

ORPHAN_DELETE_STATEMENT *orphan_delete_statement_new(
		char *many_table_name,
		LIST *relation_foreign_key_list,
		char *orphan_data )
{
	char statement[ 1024 ];
	char *ptr = statement;
	int p = 0;
	boolean first_time = 1;
	ORPHAN_DELETE_STATEMENT *orphan_delete_statement;
	char *foreign_key;
	char datum[ 128 ];

	if ( !many_table_name
	||   !list_rewind( relation_foreign_key_list )
	||   !orphan_data )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	orphan_delete_statement = orphan_delete_statement_calloc();

	orphan_delete_statement->many_table_name = many_table_name;
	orphan_delete_statement->orphan_data = orphan_data;

	ptr += sprintf(
		ptr,
		"delete from %s where ",
		many_table_name );

	do {
		foreign_key = list_get( relation_foreign_key_list );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " and " );

		/* Returns null if not enough delimiters */
		/* ------------------------------------- */
		if ( !piece(	datum,
				SQL_DELIMITER,
				orphan_data,
				p ) )
		{
			char message[ 128 ];

			sprintf(message,
				"piece(%d,%s) returned empty.",
				p,
				orphan_data );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr +=
			sprintf(
				ptr,
				"%s = '%s'",
				foreign_key,
				datum );

	} while ( list_next( relation_foreign_key_list ) );

	ptr += sprintf( ptr, ";" );

	orphan_delete_statement->statement = strdup( statement );

	return orphan_delete_statement;
}

ORPHAN_DELETE_STATEMENT *orphan_delete_statement_calloc( void )
{
	ORPHAN_DELETE_STATEMENT *orphan_delete_statement;

	if ( ! ( orphan_delete_statement =
			calloc( 1,
				sizeof ( ORPHAN_DELETE_STATEMENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan_delete_statement;
}

LIST *orphan_role_list( char *folder_name )
{
	LIST *list = list_new();

	if ( folder_name )
	{
		ORPHAN_ROLE *orphan_role;

		orphan_role =
			orphan_role_new(
				(char *)0 /* role_name */ );

		list_set(
			orphan_role->folder_name_list,
			folder_name );

		list_set( list, orphan_role );
	}
	else
	{
		LIST *name_list;
		char *role_name;
		ORPHAN_ROLE *orphan_role;
		LIST *fetch_name_list;
		char *folder_name;

		name_list =
			role_name_list(
				ROLE_TABLE );

		if ( list_rewind( name_list ) )
		do {
			role_name = list_get( name_list );

			orphan_role =
				orphan_role_new(
					role_name );

			fetch_name_list =
				role_folder_fetch_name_list(
					ROLE_FOLDER_TABLE,
					FOLDER_PRIMARY_KEY,
					role_name );

			if ( list_rewind( fetch_name_list ) )
			do {
				folder_name =
					list_get(
						fetch_name_list );

				if ( !orphan_role_folder_name_boolean(
					list /* orphan_role_list */,
					folder_name ) )
				{
					list_set(
						orphan_role->
							folder_name_list,
						folder_name );
				}
			} while ( list_next( fetch_name_list ) );

			list_set( list, orphan_role );

		} while ( list_next( name_list ) );

	} /* if !folder_name */

	return list;
}

ORPHAN_ROLE *orphan_role_new( char *role_name )
{
	ORPHAN_ROLE *orphan_role = orphan_role_calloc();

	orphan_role->role_name = role_name;
	orphan_role->folder_name_list = list_new();

	return orphan_role;
}

ORPHAN_ROLE *orphan_role_calloc( void )
{
	ORPHAN_ROLE *orphan_role;

	if ( ! ( orphan_role = calloc( 1, sizeof ( ORPHAN_ROLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan_role;
}

boolean orphan_role_folder_name_boolean(
		LIST *orphan_role_list,
		char *folder_name )
{
	ORPHAN_ROLE *orphan_role;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( orphan_role_list ) )
	do {
		orphan_role = list_get( orphan_role_list );

		if ( list_string_exists(
			folder_name,
			orphan_role->folder_name_list) )
		{
			return 1;
		}

	} while ( list_next( orphan_role_list ) );

	return 0;
}

ORPHAN *orphan_new(
		char *application_name,
		char *folder_name,
		boolean delete_boolean )
{
	ORPHAN *orphan;
	ORPHAN_ROLE *orphan_role;
	ORPHAN_FOLDER *orphan_folder;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	orphan = orphan_calloc();

	orphan->folder_name =
		/* --------------------------- */
		/* Returns folder_name or null */
		/* --------------------------- */
		orphan_folder_name(
			folder_name );

	/* Per role to engage caching. */
	/* --------------------------- */
	orphan->orphan_role_list =
		orphan_role_list(
			orphan->folder_name );

	orphan->orphan_folder_list = list_new();

	if ( list_rewind( orphan->orphan_role_list ) )
	do {
		orphan_role = list_get( orphan->orphan_role_list );

		if ( list_rewind( orphan_role->folder_name_list ) )
		do {
			folder_name =
				list_get(
					orphan_role->folder_name_list );

			orphan_folder =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				orphan_folder_new(
					application_name,
					orphan_role->role_name,
					folder_name,
					delete_boolean );

			list_set(
				orphan->orphan_folder_list,
				orphan_folder );

		} while ( list_next( orphan_role->folder_name_list ) );

	} while ( list_next( orphan->orphan_role_list ) );

	return orphan;
}

ORPHAN *orphan_calloc( void )
{
	ORPHAN *orphan;

	if ( ! ( orphan = calloc( 1, sizeof ( ORPHAN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan;
}

void orphan_subquery_string_stdout( LIST *orphan_subquery_list )
{
	ORPHAN_SUBQUERY *orphan_subquery;

	if ( list_rewind( orphan_subquery_list ) )
	do {
		orphan_subquery = list_get( orphan_subquery_list );

		printf( "%s\n", orphan_subquery->string );

	} while ( list_next( orphan_subquery_list ) );
}

char *orphan_subquery_string(
		RELATION_MTO1 *relation_mto1,
		char *many_table_name,
		char *one_table_name )
{
	char string[ 2048 ];
	char *ptr = string;
	char *foreign_key;
	char *primary_key;
	boolean first_time = 1;

	if ( !relation_mto1
	||   !many_table_name
	||   !one_table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"select distinct %s from %s ",
		list_display_delimited(
			relation_mto1->relation_foreign_key_list,
			',' ),
		many_table_name );

	ptr += sprintf(
		ptr,
		"where not exists ( select 1 from %s where ",
		one_table_name );

	list_rewind(
		relation_mto1->
			one_folder->
			folder_attribute_primary_key_list );

	list_rewind( relation_mto1->relation_foreign_key_list );

	do {
		primary_key =
			list_get(
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list );

		foreign_key =
			list_get(
				relation_mto1->relation_foreign_key_list );


		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " and " );

		ptr +=
			sprintf(
				ptr,
				"%s.%s = %s.%s",
				one_table_name,
				primary_key,
				many_table_name,
				foreign_key );

		list_next(
			relation_mto1->
				one_folder->
				folder_attribute_primary_key_list );

	} while ( list_next( relation_mto1->relation_foreign_key_list ) );

	ptr += sprintf( ptr, " );" );

	return strdup( string );
}

FILE *orphan_insert_display_pipe(
		boolean stdout_boolean,
		char *folder_name )
{
	char system_string[ 256 ];
	char buffer[ 128 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( stdout_boolean )
	{
		sprintf(system_string,
			"echo \"%s\" && cat -",
			folder_name );
	}
	else
	{
		sprintf(system_string,
			"html_table.e '^^%s' 'Folder,New Data,Statement' '|'",
			string_initial_capital(
				buffer,
				folder_name ) );
	}

	return popen( system_string, "w" );
}

FILE *orphan_delete_display_pipe(
		boolean stdout_boolean,
		char *one_table_name )
{
	char system_string[ 256 ];
	char buffer[ 128 ];

	if ( !one_table_name )
	{
		char message[ 128 ];

		sprintf(message, "one_table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( stdout_boolean )
	{
		sprintf(system_string,
			"echo \"%s\" && cat -",
			one_table_name );
	}
	else
	{
		sprintf(system_string,
		"html_table.e '^^%s' 'Folder,Delete Data,Statement' '|'",
			string_initial_capital(
				buffer,
				one_table_name ) );
	}

	return popen( system_string, "w" );
}

void orphan_insert_list_display(
		boolean stdout_boolean,
		char *folder_name,
		LIST *orphan_insert_list )
{
	FILE *display_pipe;
	ORPHAN_INSERT *orphan_insert;
	ORPHAN_INSERT_STATEMENT *orphan_insert_statement;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( orphan_insert_list ) )
	do {
		orphan_insert = list_get( orphan_insert_list );

		if ( !list_rewind(
			orphan_insert->
				orphan_insert_statement_list ) )
		{
			continue;
		}

		display_pipe =
			orphan_insert_display_pipe(
				stdout_boolean,
				folder_name );

		do {
			orphan_insert_statement =
				list_get(
					orphan_insert->
						orphan_insert_statement_list );

			fprintf(display_pipe,
				"%s|%s|%s\n",
				orphan_insert_statement->one_table_name,
				orphan_insert_statement->orphan_data,
				orphan_insert_statement->statement );

		} while ( list_next(
				orphan_insert->
					orphan_insert_statement_list ) );

		pclose( display_pipe );

	} while ( list_next( orphan_insert_list ) );
}

void orphan_delete_execute( LIST *orphan_delete_list )
{
	FILE *output_pipe;
	ORPHAN_DELETE *orphan_delete;
	ORPHAN_DELETE_STATEMENT *orphan_delete_statement;

	output_pipe = popen( "tee_appaserver.sh | sql.e", "w" );

	if ( list_rewind( orphan_delete_list ) )
	do {
		orphan_delete = list_get( orphan_delete_list );

		if ( !list_rewind(
			orphan_delete->
				orphan_delete_statement_list ) )
		{
			continue;
		}

		do {
			orphan_delete_statement =
				list_get(
					orphan_delete->
						orphan_delete_statement_list );

			fprintf(output_pipe,
				"%s\n",
				orphan_delete_statement->statement );

		} while (
			list_next(
				orphan_delete->
					orphan_delete_statement_list ) );

	} while ( list_next( orphan_delete_list ) );

	pclose( output_pipe );
}

void orphan_insert_execute( LIST *orphan_insert_list )
{
	FILE *output_pipe;
	ORPHAN_INSERT *orphan_insert;
	ORPHAN_INSERT_STATEMENT *orphan_insert_statement;

	output_pipe = popen( "tee_appaserver.sh | sql.e", "w" );

	if ( list_rewind( orphan_insert_list ) )
	do {
		orphan_insert = list_get( orphan_insert_list );

		if ( !list_rewind(
			orphan_insert->
				orphan_insert_statement_list ) )
		{
			continue;
		}

		do {
			orphan_insert_statement =
				list_get(
					orphan_insert->
						orphan_insert_statement_list );

			fprintf(output_pipe,
				"%s\n",
				orphan_insert_statement->statement );

		} while (
			list_next(
				orphan_insert->
					orphan_insert_statement_list ) );

	} while ( list_next( orphan_insert_list ) );

	pclose( output_pipe );
}

char *orphan_folder_name(
		char *folder_name )
{
	if ( folder_name
	&&   *folder_name
	&&   strcmp( folder_name, "orphans_table" ) != 0 )
	{
		return folder_name;
	}

	return (char *)0;
}

boolean orphan_subquery_clean_boolean( LIST *orphan_subquery_list )
{
	ORPHAN_SUBQUERY *orphan_subquery;

	if ( list_rewind( orphan_subquery_list ) )
	do {
		orphan_subquery = list_get( orphan_subquery_list );

		if ( list_length( orphan_subquery->orphan_data_list ) )
		{
			return 0;
		}
	} while ( list_next( orphan_subquery_list ) );

	return 1;
}

boolean orphan_folder_clean_boolean( LIST *orphan_folder_list )
{
	ORPHAN_FOLDER *orphan_folder;

	if ( list_rewind( orphan_folder_list ) )
	do {
		orphan_folder = list_get( orphan_folder_list );

		if ( !orphan_subquery_clean_boolean(
			orphan_folder->orphan_subquery_list ) )
		{
			return 0;
		}
	} while ( list_next( orphan_folder_list ) );

	return 1;
}

void orphan_clean_output(
		boolean stdout_boolean,
		char *orphan_folder_name )
{
	if ( !stdout_boolean )
	{
		char buffer[ 128 ];

		printf(	"<h3>There are no orphans in %s.</h3>",
			(orphan_folder_name)
				? string_initial_capital(
					buffer,
					orphan_folder_name )
				: "the database" );
	}
}

void orphan_delete_list_display(
		boolean stdout_boolean,
		LIST *orphan_delete_list )
{
	FILE *display_pipe;
	ORPHAN_DELETE *orphan_delete;
	ORPHAN_DELETE_STATEMENT *orphan_delete_statement;

	if ( list_rewind( orphan_delete_list ) )
	do {
		orphan_delete = list_get( orphan_delete_list );

		if ( !list_rewind(
			orphan_delete->
				orphan_delete_statement_list ) )
		{
			continue;
		}

		display_pipe =
			orphan_delete_display_pipe(
				stdout_boolean,
				orphan_delete->one_table_name );

		do {
			orphan_delete_statement =
				list_get(
					orphan_delete->
						orphan_delete_statement_list );

			fprintf(display_pipe,
				"%s|%s|%s\n",
				orphan_delete_statement->many_table_name,
				orphan_delete_statement->orphan_data,
				orphan_delete_statement->statement );

		} while ( list_next(
				orphan_delete->
					orphan_delete_statement_list ) );

		pclose( display_pipe );

	} while ( list_next( orphan_delete_list ) );
}

ORPHAN_FOLDER_INPUT *orphan_folder_input_new(
		char *role_name,
		char *folder_name )
{
	ORPHAN_FOLDER_INPUT *orphan_folder_input;

	if ( !folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	orphan_folder_input = orphan_folder_input_calloc();

	orphan_folder_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			role_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_attribute */ );

	orphan_folder_input->relation_mto1_list =
		relation_mto1_list(
			role_name,
			folder_name
				/* many_folder_name */,
			orphan_folder_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );


	orphan_folder_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0,
			role_name,
			folder_name,
			orphan_folder_input->
				folder->
				folder_attribute_primary_key_list,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	return orphan_folder_input;
}

ORPHAN_FOLDER_INPUT *orphan_folder_input_calloc( void )
{
	ORPHAN_FOLDER_INPUT *orphan_folder_input;

	if ( ! ( orphan_folder_input =
			calloc( 1,
				sizeof ( ORPHAN_FOLDER_INPUT ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return orphan_folder_input;
}

