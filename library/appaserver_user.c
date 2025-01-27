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
#include "timlib.h"
#include "session.h"
#include "role.h"
#include "folder_attribute.h"
#include "appaserver.h"
#include "application.h"
#include "date_convert.h"
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

APPASERVER_USER *appaserver_user_fetch(
		char *login_name,
		boolean fetch_role_name_list )
{
	boolean full_name_exists;
	boolean person_full_name_exists;

	full_name_exists =
		folder_attribute_exists(
			APPASERVER_USER_TABLE,
			"full_name" );

	person_full_name_exists =
		folder_attribute_exists(
			APPASERVER_USER_TABLE,
			"person_full_name" );

	return
	/* ------------------------------------ */
	/* Returns null if login_name not found */
	/* ------------------------------------ */
	appaserver_user_parse(
		fetch_role_name_list,
		full_name_exists || person_full_name_exists,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				appaserver_user_select(
					full_name_exists,
					person_full_name_exists ),
				APPASERVER_USER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				appaserver_user_primary_where(
					login_name ) ) ) );
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

	snprintf(
		where,
		sizeof ( where ),
		"login_name = '%s'",
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		( tmp = security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				login_name ) ) );

	free( tmp );
	return where;
}

char *appaserver_user_select(
		boolean full_name_exists,
		boolean person_full_name_exists )
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
	else
	if ( person_full_name_exists )
	{
		ptr += sprintf( ptr, ",person_full_name" );
	}

	ptr += sprintf( ptr, ",password,user_date_format,deactivated_yn" );

	return select;
}

APPASERVER_USER *appaserver_user_parse(
		boolean fetch_role_name_list,
		boolean full_name_exists,
		char *input )
{
	APPASERVER_USER *appaserver_user;
	char piece_buffer[ 512 ];
	int piece_offset = 0;

	if ( !input || !*input ) return NULL;

	/* See appaserver_user_select() */
	/* ---------------------------- */
	piece_offset = 0;
	piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );

	appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_new(
			strdup( piece_buffer )
				/* login_name */ );

	if ( full_name_exists )
	{
		piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );

		if ( *piece_buffer )
			appaserver_user->full_name =
				strdup( piece_buffer );
	}

	piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );

	if ( *piece_buffer )
	{
		appaserver_user->database_password = strdup( piece_buffer );
	}

	piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );

	if ( *piece_buffer )
	{
		appaserver_user->user_date_format = strdup( piece_buffer );
	}

	piece( piece_buffer, SQL_DELIMITER, input, piece_offset++ );

	if ( *piece_buffer )
	{
		appaserver_user->deactivated_boolean = (*piece_buffer == 'y');
	}

	if ( fetch_role_name_list )
	{
		appaserver_user->role_name_list =
			role_appaserver_user_role_name_list(
				appaserver_user->login_name );
	}

	return appaserver_user;
}

char *appaserver_user_person_full_name( char *login_name )
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

	appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_fetch(
			login_name,
			0 /* not fetch_role_name_ist */ );

	return appaserver_user->full_name;
}

void appaserver_user_update(
		char *password,
		char *login_name )
{
	FILE *update_open;

	update_open =
		appaserver_user_update_open(
			APPASERVER_USER_TABLE,
			APPASERVER_USER_PRIMARY_KEY );

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
		char *appaserver_user_table,
		char *appaserver_user_primary_key )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement table=%s key=%s carrot=y	|"
		"tee_appaserver.sh 				|"
		"sql.e						 ",
		appaserver_user_table,
		appaserver_user_primary_key );

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

char *appaserver_user_default_role_name( char *login_name )
{
	char system_string[ 1024 ];
	char where[ 128 ];

	if ( !login_name )
	{
		char message[ 128 ];

		sprintf(message, "login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"login_name = '%s'",
		login_name );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh role login_default_role \"%s\"",
		where );

	/* Returns heap memory or null */
	/* --------------------------- */
	return string_pipe_input( system_string );
}

LIST *appaserver_user_role_name_list( char *login_name )
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

	appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_fetch(
			login_name,
			1 /* fetch_role_name_ist */ );

	return appaserver_user->role_name_list;
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
		appaserver_user_fetch(
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
		char *login_name )
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

	appaserver_user = appaserver_user_calloc();
	appaserver_user->login_name = login_name;

	return appaserver_user;
}
