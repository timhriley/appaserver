/* $APPASERVER_HOME/library/appaserver_user.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "security.h"
#include "environ.h"
#include "sql.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "boolean.h"
#include "piece.h"
#include "folder.h"
#include "timlib.h"
#include "session.h"
#include "role.h"
#include "folder_attribute.h"
#include "appaserver_user.h"

APPASERVER_USER *appaserver_user_calloc( void )
{
	APPASERVER_USER *appaserver_user;

	if ( ! ( appaserver_user = calloc( 1, sizeof( APPASERVER_USER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return appaserver_user;
}

char *appaserver_user_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" none",
		select,
		table,
		where );

	return strdup( system_string );
}

APPASERVER_USER *appaserver_user_fetch(
			char *login_name,
			boolean fetch_role_name_list )
{
	boolean full_name_exists;
	boolean person_full_name_exists;
	boolean frameset_menu_horizontal_exists;

	full_name_exists =
		folder_attribute_exists(
			APPASERVER_USER_TABLE,
			"full_name" );

	person_full_name_exists =
		folder_attribute_exists(
			APPASERVER_USER_TABLE,
			"person_full_name" );

	frameset_menu_horizontal_exists =
		folder_attribute_exists(
			APPASERVER_USER_TABLE,
			"frameset_menu_horizontal_yn" );

	return
	appaserver_user_parse(
		string_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_user_system_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				appaserver_user_select(
					full_name_exists,
					person_full_name_exists,
					frameset_menu_horizontal_exists ),
				APPASERVER_USER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				appaserver_user_primary_where(
					login_name ) ) ),
		fetch_role_name_list,
		full_name_exists,
		frameset_menu_horizontal_exists );
}

char *appaserver_user_primary_where( char *login_name )
{
	static char where[ 128 ];
	char *tmp;

	if ( !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"login_name = '%s'",
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		( tmp = security_sql_injection_escape(
			login_name ) ) );

	free( tmp );
	return where;
}

LIST *appaserver_user_session_list(
			char *application_name,
			char *login_name )
{
	char sys_string[ 1024 ];
	char where_string[ 128 ];

	sprintf( where_string, "login_name = '%s'", login_name );

	sprintf(	sys_string,
			"get_folder_data	application=%s		  "
			"			folder=appaserver_sessions"
			"			select=appaserver_session "
			"			where=\"%s\"		  ",
			application_name,
			where_string );

	return pipe2list( sys_string );
}

char *appaserver_user_select(
			boolean full_name_exists,
			boolean person_full_name_exists,
			boolean frameset_menu_horizontal_exists )
{
	static char select[ 256 ];
	char *ptr = select;

	if ( full_name_exists && person_full_name_exists )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: both full_name_exists and person_full_name_exists are set.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "login_name" );

	if ( full_name_exists )
	{
		ptr += sprintf( ptr, ",full_name" );
	}

	if ( person_full_name_exists )
	{
		ptr += sprintf( ptr, ",person_full_name" );
	}

	if ( frameset_menu_horizontal_exists )
	{
		ptr += sprintf( ptr, ",frameset_menu_horizontal_yn" );
	}

	ptr += sprintf( ptr, ",password,user_date_format" );

	return select;
}

APPASERVER_USER *appaserver_user_parse(
			char *input,
			boolean fetch_role_name_list,
			boolean full_name_exists,
			boolean frameset_menu_horizontal_exists )
{
	APPASERVER_USER *appaserver_user;
	char piece_buffer[ 512 ];
	int piece_offset = 0;

	if ( !input || !*input )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: input is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	appaserver_user = appaserver_user_calloc();

	/* See appaserver_user_select() */
	/* ---------------------------- */
	piece_offset = 0;
	piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );
	appaserver_user->login_name = strdup( piece_buffer );

	if ( full_name_exists )
	{
		piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );
		appaserver_user->full_name = strdup( piece_buffer );
	}

	if ( frameset_menu_horizontal_exists )
	{
		appaserver_user->frameset_menu_horizontal_exists = 1;

		piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );

		appaserver_user->frameset_menu_horizontal =
			( *piece_buffer == 'y' );
	}

	piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );
	appaserver_user->database_password = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );
	appaserver_user->user_date_format = strdup( piece_buffer );

	if ( fetch_role_name_list )
	{
		appaserver_user->role_name_list =
			role_appaserver_user_role_name_list(
				appaserver_user->login_name );
	}

	return appaserver_user;
}

char *appaserver_user_person_full_name(
			char *login_name )
{
	APPASERVER_USER *appaserver_user;

	if ( !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( appaserver_user =
			appaserver_user_fetch(
				login_name,
				0 /* not fetch_role_name_ist */ ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: appaserver_user_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name );
		exit( 1 );
	}

	return appaserver_user->full_name;
}

void appaserver_user_update(
			char *database_password,
			char *login_name )
{
	FILE *update_open;

	update_open =
		appaserver_user_update_open(
			APPASERVER_USER_TABLE,
			APPASERVER_USER_PRIMARY_KEY );

	appaserver_user_update_pipe(
		update_open,
		security_encrypted_password(
			security_sql_injection_escape(
				database_password ),
			security_mysql_version_password_function(
				security_mysql_version() ) ),
		login_name );

	pclose( update_pipe() );
}

FILE *appaserver_user_update_open(
			char *appaserver_user_table,
			char *appaserver_user_primary_key )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"update_statement table=%s key=%s carrot=y	|"
		"tee_appaserver_error.sh 			|"
		"sql						 ",
		appaserver_user_table,
		appaserver_user_primary_key );

	return popen( system_string, "w" );
}

void appaserver_user_update_pipe(
			FILE *update_pipe,
			char *injection_escaped_encrypted_password,
			char *login_name )
{
	fprintf(update_pipe,
		"%s^password^%s\n",
		login_name,
		injection_escaped_encrypted_password );
}

boolean appaserver_user_frameset_menu_horizontal(
			char *login_name )
{
	APPASERVER_USER *appaserver_user;

	if ( ! ( appaserver_user =
			appaserver_user_fetch(
				login_name,
				0 /* not fetch_role_name_list */,
				0 /* not fetch_session_list */ ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: appaserver_user_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return appaserver_user->frameset_menu_horizontal;
}

char *appaserver_user_default_role_name(
			char *login_name )
{
	char system_string[ 1024 ];
	char where[ 128 ];

	sprintf(where,
		"login_name = '%s'",
		login_name );

	sprintf(system_string,
		"select.sh role login_default_role \"%s\"",
		where );


	return pipe2string( system_string );
}

