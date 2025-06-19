/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver_user.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
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
#include "appaserver_error.h"
#include "piece.h"
#include "role.h"
#include "appaserver.h"
#include "application.h"
#include "date_convert.h"
#include "role_appaserver_user.h"
#include "entity.h"
#include "appaserver_user.h"

APPASERVER_USER *appaserver_user_calloc( void )
{
	APPASERVER_USER *appaserver_user;

	if ( ! ( appaserver_user = calloc( 1, sizeof ( APPASERVER_USER ) ) ) )
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

APPASERVER_USER *appaserver_user_login_fetch(
		char *login_name,
		boolean fetch_role_name_list )
{
	char *where;
	char *system_string;
	char *input;

	if ( !login_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_user_login_where(
			APPASERVER_USER_LOGIN_NAME,
			login_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			APPASERVER_USER_SELECT,
			APPASERVER_USER_TABLE,
			where );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			system_string );

	if ( !input )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_pipe_fetch(%s) returned empty.",
			login_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( system_string );

	return
	appaserver_user_parse(
		fetch_role_name_list,
		input );
}

APPASERVER_USER *appaserver_user_parse(
		boolean fetch_role_name_list,
		char *input )
{
	APPASERVER_USER *appaserver_user;
	char full_name[ 128 ];
	char street_address[ 128 ];
	char buffer[ 128 ];

	if ( !input )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See APPASERVER_USER_SELECT */
	/* -------------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );

	appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_new(
			strdup( full_name ),
			strdup( street_address ) );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) appaserver_user->login_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) appaserver_user->password = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) appaserver_user->user_date_format = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) appaserver_user->deactivated_boolean = (*buffer == 'y');

	if ( fetch_role_name_list )
	{
		appaserver_user->role_appaserver_user_name_list =
			role_appaserver_user_name_list(
				ROLE_APPASERVER_USER_TABLE,
				appaserver_user->full_name,
				appaserver_user->street_address );
	}

	return appaserver_user;
}

void appaserver_user_update(
		const char *appaserver_user_table,
		char *password,
		char *login_name )
{
	FILE *update_open;

	update_open =
		appaserver_user_update_open(
			appaserver_user_table,
			APPASERVER_USER_LOGIN_NAME );

	appaserver_user_update_pipe(
		update_open,
		security_encrypt_password(
			password,
			security_mysql_version_password_function(
				security_mysql_version() ) ),
		login_name );

	pclose( update_open );
}

FILE *appaserver_user_update_open(
		const char *appaserver_user_table,
		const char *appaserver_user_login_name )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement table=%s key=%s carrot=y	|"
		"tee_appaserver.sh 				|"
		"sql.e						 ",
		appaserver_user_table,
		appaserver_user_login_name );

	return popen( system_string, "w" );
}

void appaserver_user_update_pipe(
		FILE *update_pipe,
		char *password,
		char *login_name )
{
	fprintf(update_pipe,
		"%s^password^%s\n",
		login_name,
		password );
}

char *appaserver_user_date_format_string(
		char *application_name,
		char *login_name )
{
	char *user_date_format_string = "international";
	APPASERVER_USER *appaserver_user;

	if ( !application_name
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_login_fetch(
			login_name,
			0 /* not fetch_role_name_list */ );

	if ( appaserver_user->user_date_format )
	{
		user_date_format_string =
			appaserver_user->
				user_date_format;
	}
	else
	{
		APPLICATION *application;

		if ( ! ( application =
				application_fetch(
					application_name ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"application_fetch(%s) returned empty.",
				application_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( application->user_date_format )
		{
			user_date_format_string =
				application->
					user_date_format;
		}
	}

	return user_date_format_string;
}

char *appaserver_user_date_convert_string(
		char *appaserver_user_date_format_string,
		char *international_date_string )
{
	DATE_CONVERT *date_convert;
	enum date_convert_format_enum date_convert_format_enum;

	if ( !international_date_string )
	{
		char message[ 128 ];

		sprintf(message, "international_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !appaserver_user_date_format_string )
	{
		return strdup( international_date_string );
	}

	date_convert_format_enum =
		date_convert_format_string_evaluate(
			appaserver_user_date_format_string );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		date_convert_new(
			date_convert_international
				/* source_enum */,
			date_convert_format_enum
				/* destination_enum */,
			international_date_string
				/* source_date_string */ );

	return date_convert->return_date_string;
}

APPASERVER_USER *appaserver_user_new(
		char *full_name,
		char *street_address )
{
	APPASERVER_USER *appaserver_user;

	if ( !full_name
	||   !street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	appaserver_user = appaserver_user_calloc();

	appaserver_user->full_name = full_name;
	appaserver_user->street_address = street_address;

	return appaserver_user;
}

LIST *appaserver_user_role_name_list( char *login_name )
{
	APPASERVER_USER *appaserver_user;

	if ( !login_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_login_fetch(
			login_name,
			1 /* fetch_role_name_list */ );

	return appaserver_user->role_appaserver_user_name_list;
}

APPASERVER_USER *appaserver_user_fetch(
		char *full_name,
		char *street_address,
		boolean fetch_role_name_list )
{
	char *input;

	if ( !full_name
	||   !street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				APPASERVER_USER_SELECT,
				APPASERVER_USER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				entity_primary_where(
					full_name,
					street_address ) ) );

	if ( !input ) return NULL;

	return
	appaserver_user_parse(
		fetch_role_name_list,
		input );
}

char *appaserver_user_login_where(
		const char *appaserver_user_login_name,
		char *login_name )
{
	static char where[ 128 ];
	char *tmp;

	if ( !login_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"%s = '%s'",
		appaserver_user_login_name,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tmp = security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			login_name ) ) );

	free( tmp );

	return where;
}

