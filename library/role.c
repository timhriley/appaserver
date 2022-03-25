/* $APPASERVER_HOME/library/role.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "security.h"
#include "folder_attribute.h"
#include "application.h"
#include "process.h"
#include "role.h"

ROLE *role_calloc( void )
{
	ROLE *role;

	if ( ! ( role = (ROLE *)calloc( 1, sizeof( ROLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return role;
}

void role_free( ROLE *role )
{
	free( role );
}

char *role_select( void )
{
	char *select;

	if ( folder_attribute_exists(
		ROLE_TABLE,
		"grace_no_cycle_colors_yn" ) )
	{
		select =
	"folder_count_yn,override_row_restrictions_yn,grace_no_cycle_colors_yn";
	}
	else
	{
		select =
		"folder_count_yn,override_row_restrictions_yn,'n'";
	}

	return select;
}

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_new(
			char *role_name,
			char *attribute_name,
			char *permission )
{
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( ! ( role_attribute_exclude =
			(ROLE_ATTRIBUTE_EXCLUDE *)
				calloc( 1,
					sizeof( ROLE_ATTRIBUTE_EXCLUDE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	role_attribute_exclude->role_name = role_name;
	role_attribute_exclude->attribute_name = attribute_name;
	role_attribute_exclude->permission = permission;

	return role_attribute_exclude;
}

boolean role_exists_attribute_exclude_insert(
				LIST *attribute_exclude_list,
				char *attribute_name )
{
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( !list_rewind( attribute_exclude_list ) )
		return 0;

	do {
		role_attribute_exclude = list_get( attribute_exclude_list );

		if ( strcmp(	role_attribute_exclude->attribute_name,
				attribute_name ) == 0 )
		{
			if ( strcmp(	role_attribute_exclude->permission,
					"insert" ) == 0 )
			{
				return 1;
			}
		}
	} while( list_next( attribute_exclude_list ) );

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

	sprintf(where,
		"role = '%s'",
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		security_sql_injection_escape(
			role_name ) );

	return where;
}

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_parse(
			char *input )
{
	char role_name[ 128 ];
	char attribute_name[ 128 ];
	char permission[ 128 ];

	piece( role_name, SQL_DELIMITER, input, 0 );
	piece( attribute_name, SQL_DELIMITER, input, 1 );
	piece( permission, SQL_DELIMITER, input, 2 );

	return role_attribute_exclude_new(
			strdup( role_name ),
			strdup( attribute_name ),
			strdup( permission ) );
}

char *role_attribute_exclude_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' attribute_exclude \"%s\" none",
		where );

	return strdup( system_string );
}

LIST *role_attribute_exclude_system_list(
			char *system_string )
{
	FILE *pipe = popen( system_string, "r" );
	char input[ 256 ];
	LIST *list = {0};

	while( string_input( input, pipe, 256 ) )
	{
		if ( !list ) list = list_new();

		list_set(
			list,
			role_attribute_exclude_parse(
				input ) );
	}
	pclose( pipe );
	return list;
}

char *role_system_string(
			char *role_select,
			char *role_table,
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" none",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		role_select,
		role_table,
		where );

	return strdup( system_string );
}

ROLE *role_fetch(	char *role_name,
			boolean fetch_attribute_exclude_list )
{
	if ( !role_name || !*role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	return
		role_parse(
			string_pipe_fetch(
				role_system_string(
					role_select(),
					ROLE_TABLE,
					role_primary_where(
						role_name ) ) ),
			fetch_attribute_exclude_list );
}

ROLE *role_parse(	char *input,
			boolean fetch_attribute_exclude_list )
{
	ROLE *role = role_calloc();
	char piece_buffer[ 128 ];

	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	role->role_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	role->folder_count = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	role->override_row_restrictions = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	role->grace_no_cycle_colors = (*piece_buffer == 'y');

	if ( fetch_attribute_exclude_list )
	{
		role->attribute_exclude_list =
			role_attribute_exclude_system_list(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				role_attribute_exclude_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					role_primary_where(
						role->role_name ) ) );
	}

	return role;
}

char *role_appaserver_user_primary_where(
			char *login_name,
			char *role_name )
{
	static char where[ 256 ];

	sprintf(where,
		"login_name = '%s' and role = '%s'",
		login_name,
		role_name );

	return where;
}

LIST *role_exclude_lookup_attribute_name_list(
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

LIST *role_process_list( char *role_name )
{
	LIST *list = list_new();
	char input[ 256 ];
	FILE *input_pipe;

	input_pipe =
		popen(
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			role_process_system_string(
				/* -------------------------- */
				/* Safely returns heap memory */
				/* -------------------------- */
				role_process_where(
					role_name ) ),
			"r" );

	while( string_input( input, input_pipe, 256 ) )
	{
		list_set(
			list,
			role_process_parse( input ) );
	}

	pclose( input_pipe );
	return list;
}

ROLE_PROCESS *role_process_calloc( void )
{
	ROLE_PROCESS *role_process;

	if ( ! ( role_process = calloc( 1, sizeof( ROLE_PROCESS ) ) ) )
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

char *role_process_select( void )
{
	static char select[ 256 ];

	sprintf(select,
		"%s.role,ifnull(%s.process_group,'process'),%s.process",
		ROLE_PROCESS_TABLE,
		PROCESS_TABLE,
		ROLE_PROCESS_TABLE );

	return select;
}

char *role_process_where( char *role_name )
{
	char where[ 256 ];

	if ( !role_name || !*role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"%s.role = '%s' and %s.process = %s.process",
		ROLE_PROCESS_TABLE,
		role_name,
		ROLE_PROCESS_TABLE,
		PROCESS_TABLE );

	return strdup( where );
}

char *role_process_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s,%s \"%s\"",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		role_process_select(),
		ROLE_PROCESS_TABLE,
		PROCESS_TABLE,
		where );

	return strdup( system_string );
}

ROLE_PROCESS *role_process_parse( char *input )
{
	ROLE_PROCESS *role_process = role_process_calloc();
	char piece_buffer[ 128 ];

	/* See role_process_select() */
	/* ------------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	role_process->role_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	role_process->process_group_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	role_process->process_name = strdup( piece_buffer );

	return role_process;
}

LIST *role_process_set_list( char *role_name )
{
	LIST *list = list_new();
	char input[ 256 ];
	FILE *input_pipe;

	input_pipe =
		popen(
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			role_process_set_system_string(
				/* -------------------------- */
				/* Safely returns heap memory */
				/* -------------------------- */
				role_process_set_where(
					role_name ) ),
			"r" );

	while( string_input( input, input_pipe, 256 ) )
	{
		list_set(
			list,
			role_process_set_parse( input ) );
	}

	pclose( input_pipe );
	return list;
}

ROLE_PROCESS_SET *role_process_set_calloc( void )
{
	ROLE_PROCESS_SET *role_process_set;

	if ( ! ( role_process_set = calloc( 1, sizeof( ROLE_PROCESS_SET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return role_process_set;
}

char *role_process_set_select( void )
{
	static char select[ 256 ];

	sprintf(select,
		"%s.role,ifnull(%s.process_group,'process'),%s.process_set",
		ROLE_PROCESS_SET_MEMBER_TABLE,
		PROCESS_SET_TABLE,
		ROLE_PROCESS_SET_MEMBER_TABLE );

	return select;
}

char *role_process_set_where( char *role_name )
{
	char where[ 256 ];

	if ( !role_name || !*role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"%s.role = '%s' and %s.process_set = %s.process_set",
		ROLE_PROCESS_SET_MEMBER_TABLE,
		role_name,
		ROLE_PROCESS_SET_MEMBER_TABLE,
		PROCESS_SET_TABLE );

	return strdup( where );
}

char *role_process_set_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s,%s \"%s\" | sort -u",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		role_process_set_select(),
		ROLE_PROCESS_SET_MEMBER_TABLE,
		PROCESS_SET_TABLE,
		where );

	return strdup( system_string );
}

ROLE_PROCESS_SET *role_process_set_parse( char *input )
{
	ROLE_PROCESS_SET *role_process_set = role_process_set_calloc();
	char piece_buffer[ 128 ];

	/* See role_process_set_select() */
	/* ----------------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	role_process_set->role_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	role_process_set->process_group_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	role_process_set->process_set_name = strdup( piece_buffer );

	return role_process_set;
}

LIST *role_process_or_set_name_list(
			char *process_group_name,
			LIST *role_process_list,
			LIST *role_process_set_list )
{
	LIST *name_list = list_new();

	if ( list_rewind( role_process_list ) )
	{
		ROLE_PROCESS *role_process;

		do {
			role_process =
				list_get(
					role_process_list );

			if ( string_strcmp(
				role_process->process_group_name,
				process_group_name ) == 0 )
			{
				list_set_string_in_order(
					name_list,
					role_process->process_name );
			}
		} while ( list_next( role_process_list ) );
	}

	if ( list_rewind( role_process_set_list ) )
	{
		ROLE_PROCESS_SET *role_process_set;

		do {
			role_process_set =
				list_get(
					role_process_set_list );

			if ( string_strcmp(
				role_process_set->process_group_name,
				process_group_name ) == 0 )
			{
				list_set_string_in_order(
					name_list,
					role_process_set->process_set_name );
			}
		} while ( list_next( role_process_set_list ) );
	}

	return name_list;
}

LIST *role_process_group_missing_process_name_list(
			LIST *role_process_list,
			LIST *role_process_set_list )
{
	LIST *name_list = {0};
	ROLE_PROCESS *role_process;
	ROLE_PROCESS_SET *role_process_set;

	if ( list_rewind( role_process_list ) )
	{
		do {
			role_process = list_get( role_process_list );

			if ( role_process->process_group_name
			&&   *role_process->process_group_name )
			{
				continue;
			}

			if ( !name_list ) name_list = list_new();

			list_set(
				name_list,
				role_process->process_name );

		} while ( list_next( role_process_list ) );
	}

	if ( list_rewind( role_process_set_list ) )
	{
		do {
			role_process_set = list_get( role_process_set_list );

			if ( role_process_set->process_group_name
			&&   *role_process_set->process_group_name )
			{
				continue;
			}

			if ( !name_list ) name_list = list_new();

			list_set(
				name_list,
				role_process_set->process_set_name );

		} while ( list_next( role_process_set_list ) );
	}

	return name_list;
}

LIST *role_process_group_name_list(
			LIST *role_process_list,
			LIST *role_process_set_list )
{
	LIST *name_list = {0};
	ROLE_PROCESS *role_process;
	ROLE_PROCESS_SET *role_process_set;

	if ( list_rewind( role_process_list ) )
	{
		do {
			role_process = list_get( role_process_list );

			if ( !role_process->process_group_name
			||   !*role_process->process_group_name )
			{
				continue;
			}

			if ( !name_list ) name_list = list_new();

			list_set(
				name_list,
				role_process->process_group_name );

		} while ( list_next( role_process_list ) );
	}

	if ( list_rewind( role_process_set_list ) )
	{
		do {
			role_process_set = list_get( role_process_set_list );

			if ( !role_process_set->process_group_name
			||   !*role_process_set->process_group_name )
			{
				continue;
			}

			if ( !name_list ) name_list = list_new();

			list_set(
				name_list,
				role_process_set->process_group_name );

		} while ( list_next( role_process_set_list ) );
	}

	return name_list;
}

LIST *role_appaserver_user_role_name_list( char *login_name )
{
	if ( !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	list_pipe_fetch(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		role_appaserver_user_system_string(
			ROLE_NAME_COLUMN,
			ROLE_APPASERVER_USER_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			role_appaserver_user_where(
				ROLE_LOGIN_NAME_COLUMN,
				login_name ) ) );
}

char *role_appaserver_user_where(
			char *role_login_name_column,
			char *login_name )
{
	static char where[ 128 ];

	if ( !role_login_name_column
	||   !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"%s = '%s'",
		role_login_name_column,
		login_name );

	return where;
}

char *role_appaserver_user_system_string(
			char *role_login_name_column,
			char *role_appaserver_user_table,
			char *role_appaserver_user_where )
{
	char system_string[ 1024 ];

	if ( !role_login_name_column
	||   !role_appaserver_user_table
	||   !role_appaserver_user_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh %s %s \"%s\" select",
		role_login_name_column,
		role_appaserver_user_table,
		role_appaserver_user_where );

	return strdup( system_string );
}

