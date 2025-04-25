/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/role.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "security.h"
#include "folder_attribute.h"
#include "application.h"
#include "appaserver.h"
#include "process.h"
#include "role.h"

ROLE *role_calloc( void )
{
	ROLE *role;

	if ( ! ( role = calloc( 1, sizeof ( ROLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return role;
}

char *role_select( boolean grace_no_cycle_colors_exists )
{
	char *select;

	if ( grace_no_cycle_colors_exists )
	{
		select =
	"table_count_yn,override_row_restrictions_yn,grace_no_cycle_colors_yn";
	}
	else
	{
		select =
		"table_count_yn,override_row_restrictions_yn,'n'";
	}

	return select;
}

boolean role_attribute_exclude_exists_permission(
		char *permission,
		char *attribute_name,
		LIST *role_attribute_exclude_list )
{
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( !permission
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( role_attribute_exclude_list ) )
	do {
		role_attribute_exclude =
			list_get(
				role_attribute_exclude_list );

		if ( strcmp(	role_attribute_exclude->attribute_name,
				attribute_name ) == 0
		&&   strcmp(	role_attribute_exclude->permission,
				permission ) == 0 )
		{
			return 1;
		}

	} while( list_next( role_attribute_exclude_list ) );

	return 0;
}

char *role_primary_where( char *role_name )
{
	static char where[ 128 ];

	if ( !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		where,
		sizeof ( where ),
		"role = '%s'",
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			role_name ) );

	return where;
}

ROLE *role_fetch(
		char *role_name,
		boolean fetch_role_attribute_exclude_list )
{
	boolean grace_no_cycle_colors_exists;

	if ( !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	grace_no_cycle_colors_exists =
		folder_attribute_exists(
			ROLE_TABLE /* folder_name */,
			"grace_no_cycle_colors_yn" /* attribute_name */ );

	return
	role_parse(
		role_name,
		fetch_role_attribute_exclude_list,
		string_pipe_input(
			appaserver_system_string(
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				role_select( grace_no_cycle_colors_exists ),
				ROLE_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				role_primary_where( role_name )
					/* where */ ) ) );
}

ROLE *role_parse(
		char *role_name,
		boolean fetch_role_attribute_exclude_list,
		char *input )
{
	ROLE *role;
	char piece_buffer[ 128 ];

	if ( !role_name
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	role = role_calloc();
	role->role_name = role_name;

	/* See role_select() */
	/* ----------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	role->folder_count = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	role->override_row_restrictions = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	role->grace_no_cycle_colors = (*piece_buffer == 'y');

	if ( fetch_role_attribute_exclude_list )
	{
		role->role_attribute_exclude_list =
			role_attribute_exclude_list(
				role_name );
	}

	return role;
}

LIST *role_exclude_lookup_attribute_name_list( LIST *attribute_exclude_list )
{
	LIST *exclude_attribute_name_list;
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( !list_rewind( attribute_exclude_list ) )
	{
		return (LIST *)0;
	}

	exclude_attribute_name_list = list_new();

	do {
		role_attribute_exclude =
			list_get(
				attribute_exclude_list );

		if ( strcmp(
			role_attribute_exclude->permission,
			"lookup" ) == 0 )
		{
			list_set(
				exclude_attribute_name_list,
				role_attribute_exclude->attribute_name );
		}
	} while ( list_next( attribute_exclude_list ) );

	return exclude_attribute_name_list;
}

LIST *role_exclude_insert_attribute_name_list(
			LIST *attribute_exclude_list )
{
	LIST *exclude_attribute_name_list;
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( !list_rewind( attribute_exclude_list ) )
	{
		return (LIST *)0;
	}

	exclude_attribute_name_list = list_new();

	do {
		role_attribute_exclude =
			list_get(
				attribute_exclude_list );

		if ( strcmp(
			role_attribute_exclude->permission,
			"insert" ) == 0 )
		{
			list_set(
				exclude_attribute_name_list,
				role_attribute_exclude->attribute_name );
		}
	} while ( list_next( attribute_exclude_list ) );

	return exclude_attribute_name_list;
}

LIST *role_exclude_update_attribute_name_list(
			LIST *attribute_exclude_list )
{
	LIST *exclude_attribute_name_list;
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( !list_rewind( attribute_exclude_list ) )
	{
		return (LIST *)0;
	}

	exclude_attribute_name_list = list_new();

	do {
		role_attribute_exclude =
			list_get(
				attribute_exclude_list );

		if ( strcmp(
			role_attribute_exclude->permission,
			"update" ) == 0 )
		{
			list_set(
				exclude_attribute_name_list,
				role_attribute_exclude->attribute_name );
		}
	} while ( list_next( attribute_exclude_list ) );

	return exclude_attribute_name_list;
}

LIST *role_process_list(
			char *role_name,
			boolean fetch_process )
{
	LIST *list;
	char input[ 256 ];
	FILE *input_pipe;

	if ( !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	input_pipe =
		role_process_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			role_process_system_string(
				ROLE_PROCESS_SELECT,
				ROLE_PROCESS_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				role_primary_where(
					role_name ) ) );

	while( string_input( input, input_pipe, 256 ) )
	{
		list_set(
			list,
			role_process_parse(
				role_name,
				fetch_process,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

char *role_process_system_string(
			char *role_process_select,
			char *role_process_table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !role_process_select
	||   !role_process_table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		role_process_select,
		role_process_table,
		where );

	return strdup( system_string );
}

FILE *role_process_input_pipe( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message,
			"system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( system_string, "r" );
}

ROLE_PROCESS *role_process_new(
			char *role_name,
			char *process_name )
{
	ROLE_PROCESS *role_process;

	if ( !role_name
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	role_process = role_process_calloc();
	role_process->role_name = role_name;
	role_process->process_name = process_name;

	return role_process;
}

ROLE_PROCESS *role_process_calloc( void )
{
	ROLE_PROCESS *role_process;

	if ( ! ( role_process = calloc( 1, sizeof ( ROLE_PROCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return role_process;
}

ROLE_PROCESS *role_process_parse(
			char *role_name,
			boolean fetch_process,
			char *input )
{
	ROLE_PROCESS *role_process;
	char process_name[ 128 ];

	if ( !role_name
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See ROLE_PROCESS_SELECT */
	/* ----------------------- */
	piece( process_name, SQL_DELIMITER, input, 0 );

	role_process =
		role_process_new(
			role_name,
			strdup( process_name ) );

	if ( fetch_process )
	{
		role_process->process =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_fetch(
				role_process->process_name,
			/* ----------------------- */
			/* Not fetching javascript */
			/* ----------------------- */
			(char *)0 /* document_root_directory */,
			(char *)0 /* relative_source_directory */,
			/* ------------------------ */
			/* Not preparing to execute */
			/* ------------------------ */
			0 /* not check_executable_inside_filesystem */,
			(char *)0 /* mount_point */ );
	}

	return role_process;
}

FILE *role_process_set_member_input_pipe( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( system_string, "r" );
}

LIST *role_process_set_member_list(
			char *role_name,
			boolean fetch_process_set )
{
	LIST *list = list_new();
	char input[ 256 ];
	FILE *input_pipe;

	input_pipe =
		role_process_set_member_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			role_process_set_member_system_string(
				ROLE_PROCESS_SET_MEMBER_SELECT,
				ROLE_PROCESS_SET_MEMBER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				role_primary_where( role_name ) ) );

	while( string_input( input, input_pipe, 256 ) )
	{
		list_set(
			list,
			role_process_set_member_parse(
				role_name,
				fetch_process_set,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

char *role_process_set_member_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !select
	||   !table
	||   !where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" select",
		select,
		table,
		where );

	return strdup( system_string );
}

ROLE_PROCESS_SET_MEMBER *role_process_set_member_parse(
			char *role_name,
			boolean fetch_process_set,
			char *input )
{
	ROLE_PROCESS_SET_MEMBER *role_process_set_member;
	char process_name[ 128 ];
	char process_set_name[ 128 ];

	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	/* See ROLE_PROCESS_SET_MEMBER_SELECT */
	/* ---------------------------------- */
	piece( process_name, SQL_DELIMITER, input, 0 );
	piece( process_set_name, SQL_DELIMITER, input, 1 );

	role_process_set_member =
		role_process_set_member_new(
			strdup( process_name ),
			strdup( process_set_name ),
			role_name );

	if ( fetch_process_set )
	{
		role_process_set_member->process_set =
			process_set_fetch(
				role_process_set_member->
					process_set_name,
				(char *)0 /* role_name */,
				(char *)0 /* document_root_directory */,
				(char *)0 /* relative_source_directory */,
				0 /* not fetch_process_set_..._name_list */ );
	}

	return role_process_set_member;
}

LIST *role_process_or_set_name_list(
			char *process_group,
			LIST *role_process_list,
			LIST *role_process_set_member_list )
{
	LIST *name_list = list_new();

	if ( !process_group )
	{
		char message[ 128 ];

		sprintf(message, "process_group is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( role_process_list ) )
	{
		ROLE_PROCESS *role_process;

		do {
			role_process =
				list_get(
					role_process_list );

			if ( !role_process->process )
			{
				char message[ 128 ];

				sprintf(message,
					"role_process->process is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( !role_process->process->process_group )
				continue;

			if ( string_strcmp(
				role_process->
					process->
					process_group,
				process_group ) == 0 )
			{
				list_set_string_in_order(
					name_list,
					role_process->process_name );
			}

		} while ( list_next( role_process_list ) );
	}

	if ( list_rewind( role_process_set_member_list ) )
	{
		ROLE_PROCESS_SET_MEMBER *role_process_set_member;

		do {
			role_process_set_member =
				list_get(
					role_process_set_member_list );

			if ( !role_process_set_member->process_set )
			{
				char message[ 128 ];

				sprintf(message, "process_set is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( !role_process_set_member->
				process_set->
				process_group )
			{
				continue;
			}

			if ( string_strcmp(
				role_process_set_member->
					process_set->
					process_group,
				process_group ) == 0 )
			{
				list_set_string_in_order(
					name_list,
					role_process_set_member->
						process_set_name );
			}

		} while ( list_next( role_process_set_member_list ) );
	}

	if ( !list_length( name_list ) )
		return (LIST *)0;
	else
		return name_list;
}

LIST *role_process_or_set_missing_group_name_list(
			LIST *role_process_list,
			LIST *role_process_set_member_list )
{
	LIST *name_list = {0};

	if ( list_rewind( role_process_list ) )
	{
		ROLE_PROCESS *role_process;

		do {
			role_process = list_get( role_process_list );

			if ( !role_process->process )
			{
				char message[ 128 ];

				sprintf(message,
					"role_process->process empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( role_process->process->process_group ) continue;

			if ( !name_list ) name_list = list_new();

			list_set(
				name_list,
				role_process->process_name );

		} while ( list_next( role_process_list ) );
	}

	if ( list_rewind( role_process_set_member_list ) )
	{
		ROLE_PROCESS_SET_MEMBER *role_process_set_member;

		do {
			role_process_set_member =
				list_get(
					role_process_set_member_list );

			if ( !role_process_set_member->process_set )
			{
				char message[ 128 ];

				sprintf(message,
			"role_process_set_member->process_set is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( role_process_set_member->
				process_set->
				process_group )
			{
				continue;
			}

			if ( !name_list ) name_list = list_new();

			list_set(
				name_list,
				role_process_set_member->process_set_name );

		} while ( list_next( role_process_set_member_list ) );
	}

	return name_list;
}

LIST *role_process_or_set_group_name_list(
			LIST *role_process_list,
			LIST *role_process_set_member_list )
{
	LIST *name_list = {0};

	if ( list_rewind( role_process_list ) )
	{
		ROLE_PROCESS *role_process;

		do {
			role_process = list_get( role_process_list );

			if ( !role_process->process )
			{
				char message[ 128 ];

				sprintf(message,
					"role_process->process is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( !role_process->process->process_group ) continue;

			if ( !name_list ) name_list = list_new();

			list_string_in_order(
				name_list,
				role_process->process->process_group );

		} while ( list_next( role_process_list ) );
	}

	if ( list_rewind( role_process_set_member_list ) )
	{
		ROLE_PROCESS_SET_MEMBER *role_process_set_member;

		do {
			role_process_set_member =
				list_get(
					role_process_set_member_list );

			if ( !role_process_set_member->process_set )
			{
				char message[ 128 ];

				sprintf(message, "process_set is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( !role_process_set_member->
				process_set->
				process_group )
			{
				continue;
			}

			if ( !name_list ) name_list = list_new();

			list_string_in_order(
				name_list,
				role_process_set_member->
					process_set->
					process_group );

		} while ( list_next( role_process_set_member_list ) );
	}

	return list_unique_list( (LIST *)0, name_list );
}

LIST *role_process_set_member_process_name_list(
			char *process_group,
			LIST *role_process_set_member_list )
{
	ROLE_PROCESS_SET_MEMBER *role_process_set_member;
	LIST *name_list = {0};

	if ( list_rewind( role_process_set_member_list ) )
	{
		do {
			role_process_set_member =
				list_get(
					role_process_set_member_list );

			if ( !role_process_set_member->process_set )
			{
				char message[ 128 ];

				sprintf(message, "process_set is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( !role_process_set_member->
				process_set->
				process_group )
			{
				continue;
			}

			if ( string_strcmp(
				process_group,
				role_process_set_member->
					process_set->
					process_group ) == 0 )
			{
				if ( !name_list ) name_list = list_new();

				list_string_in_order(
					name_list,
					role_process_set_member->
						process_name );
			}

		} while ( list_next( role_process_set_member_list ) );
	}

	return name_list;
}

LIST *role_attribute_exclude_list( char *role_name )
{
	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	role_attribute_exclude_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			ROLE_ATTRIBUTE_EXCLUDE_SELECT,
			ROLE_ATTRIBUTE_EXCLUDE_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			role_primary_where( role_name ) /* where */ ) );
}

LIST *role_attribute_exclude_system_list(
		char *system_string )
{
	FILE *input_pipe;
	char input[ 128 ];
	LIST *list;

	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input_pipe =
		appaserver_input_pipe(
			system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 128 ) )
	{
		list_set(
			list,
			role_attribute_exclude_parse(
				input ) );
	}

	pclose( input_pipe );

	return list;
}

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_parse(
		char *input )
{
	char attribute_name[ 128 ];
	char permission[ 128 ];
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( !input )
	{
		char message[ 128 ];

		sprintf(message, "input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See ROLE_ATTRIBUTE_EXCLUDE_SELECT */
	/* --------------------------------- */
	piece( attribute_name, SQL_DELIMITER, input, 0 );
	piece( permission, SQL_DELIMITER, input, 1 );

	role_attribute_exclude = role_attribute_exclude_calloc();

	role_attribute_exclude->attribute_name = strdup( attribute_name );
	role_attribute_exclude->permission = strdup( permission );

	return role_attribute_exclude;
}

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_calloc( void )
{
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( ! ( role_attribute_exclude =
			calloc( 1, sizeof ( ROLE_ATTRIBUTE_EXCLUDE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return role_attribute_exclude;
}

LIST *role_attribute_exclude_lookup_name_list(
		char *role_permission_lookup,
		char *role_permission_update,
		LIST *role_attribute_exclude_list )
{
	LIST *list = {0};
	LIST *name_list;

	name_list =
		role_attribute_exclude_name_list(
			role_permission_lookup,
			role_attribute_exclude_list );

	if ( list_length( name_list ) )
	{
		list = list_new();

		list_set_list(
			list,
			name_list );

		list_free_container( name_list );
	}

	name_list =
		role_attribute_exclude_name_list(
			role_permission_update,
			role_attribute_exclude_list );

	if ( list_length( name_list ) )
	{
		if ( !list ) list = list_new();

		list_set_list(
			list,
			name_list );

		list_free_container( name_list );
	}

	return list;
}

LIST *role_attribute_exclude_insert_name_list(
		char *role_permission_insert,
		LIST *role_attribute_exclude_list )
{
	return
	role_attribute_exclude_name_list(
		role_permission_insert,
		role_attribute_exclude_list );
}

LIST *role_attribute_exclude_name_list(
		char *permission,
		LIST *role_attribute_exclude_list )
{
	LIST *attribute_name_list = {0};
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( !permission )
	{
		char message[ 128 ];

		sprintf(message, "permission is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( role_attribute_exclude_list ) )
	do {
		role_attribute_exclude =
			list_get(
				role_attribute_exclude_list );

		if ( string_strcmp(
			role_attribute_exclude->permission,
			permission ) == 0 )
		{
			if ( !attribute_name_list )
				attribute_name_list =
					list_new();

			list_set(
				attribute_name_list,
				role_attribute_exclude->attribute_name );
		}

	} while ( list_next( role_attribute_exclude_list ) );

	return attribute_name_list;
}

boolean role_process_exists(
		char *process_name,
		LIST *role_process_list )
{
	ROLE_PROCESS *role_process;

	if ( !process_name )
	{
		char message[ 128 ];

		sprintf(message, "process_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( role_process_list ) )
	do {
		role_process = list_get( role_process_list );

		if ( strcmp(
			role_process->process_name,
			process_name ) == 0 )
		{
			return 1;
		}

	} while ( list_next( role_process_list ) );

	return 0;
}

ROLE_PROCESS_SET_MEMBER *role_process_set_member_new(
		char *process_name,
		char *process_set_name,
		char *role_name )
{
	ROLE_PROCESS_SET_MEMBER *role_process_set_member;

	if ( !process_name
	||   !process_set_name
	||   !role_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	role_process_set_member = role_process_set_member_calloc();

	role_process_set_member->process_name = process_name;
	role_process_set_member->process_set_name = process_set_name;
	role_process_set_member->role_name = role_name;

	return role_process_set_member;
}

ROLE_PROCESS_SET_MEMBER *role_process_set_member_calloc( void )
{
	ROLE_PROCESS_SET_MEMBER *role_process_set_member;

	if ( ! ( role_process_set_member =
			calloc( 1,
				sizeof ( ROLE_PROCESS_SET_MEMBER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return role_process_set_member;
}

LIST *role_name_list( const char *role_table )
{
	char system_string[ 128 ];

	sprintf(system_string,
		"select.sh role %s",
		role_table );

	return list_pipe( system_string );
}

