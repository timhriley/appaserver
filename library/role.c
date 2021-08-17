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
#include "environ.h"
#include "security.h"
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

	if ( attribute_exists(
		environment_application_name(),
		ROLE_TABLE_NAME,
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


	role_table_name = get_table_name( application_name, "role" );

	role_appaserver_user_table_name =
		get_table_name(
			application_name,
			"role_appaserver_user" );

	sprintf( sys_string,
		 "echo \"select count(*)				 "
		 "	 from %s,%s					 "
		 "	 where %s.role = %s.role			 "
		 "	   and %s.login_name = '%s'			 "
		 "	   and folder_count_yn = 'y';\"			|"
		 "sql.e							 ",
		 role_table_name,
		 role_appaserver_user_table_name,
		 role_table_name,
		 role_appaserver_user_table_name,
		 role_appaserver_user_table_name,
		 login_name );

	results = pipe2string( sys_string );
	return atoi( results );

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

LIST *role_get_attribute_exclude_list(	char *application_name,
					char *role_name )
{
	char sys_string[ 1024 ];
	char attribute_name[ 128 ];
	char permission[ 128 ];
	char *select;
	char where[ 128 ];
	FILE *input_pipe;
	char input_buffer[ 256 ];
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;
	LIST *attribute_exclude_list = {0};

	select = "attribute,permission";

	sprintf( where,
		 "role = '%s'",
		 role_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=attribute_exclude	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	attribute_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	permission,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		if ( !attribute_exclude_list )
			attribute_exclude_list = list_new();

		role_attribute_exclude =
			role_attribute_exclude_new(
				role_name,
				strdup( attribute_name ),
				strdup( permission ) );

		list_append_pointer(	attribute_exclude_list,
					role_attribute_exclude );
	}

	return attribute_exclude_list;
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

	sprintf(where,
		"role = '%s'",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_sql_injection_escape(
			role_name );

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
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" role \"%s\" none",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		role_select(),
		where );

	return strdup( system_string );
}

LIST *role_system_list(	char *system_string,
			boolean fetch_attribute_exclude_list )
{
	FILE *pipe = popen( system_string, "r" );
	char input[ 1024 ];
	LIST *list = {0};

	while( string_input( input, pipe, 1024 ) )
	{
		if ( !list ) list = list_new();

		list_set(
			list,
			role_parse(
				input,
				fetch_attribute_exclude_list ) );
	}

	pclose( pipe );
	return list;
}

ROLE *role_fetch(	char *role_name,
			boolean fetch_attribute_exclude_list )
{
	return
		role_parse(
			string_pipe_fetch(
				role_system_string(
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
	role->grace_no_cycle_colors_yn = *piece_buffer;

	if ( fetch_attribute_exclude_list )
	{
		role->role_attribute_exclude_list =
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

LIST *role_list_fetch( char *login_name )
{
	char sys_string[ 256 ];

	sprintf( sys_string, 
		 "roles4appaserver_user.sh %s %s",
		 environment_application_name(),
		 login_name );

	return list_pipe_fetch( sys_string );
}

char *role_appaserver_user_primary_where(
			char *login_name,
			char *role_name )
{
}

char *role_appaserver_user_system_string( char *where )
{
}

ROLE_APPASERVER_USER *role_appaserver_user_calloc( void )
{
}

ROLE_APPASERVER_USER *role_appaserver_user_fetch(
			char *session_login_name,
			char *sql_injection_escape_role_name )
{
}


ROLE_APPASERVER_USER *role_appaserver_user_parse( char *input )
{
}

