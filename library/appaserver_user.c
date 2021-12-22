/* $APPASERVER_HOME/library/appaserver_user.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver user ADT.					*/
/*									*/
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
#include "folder_attribute.h"
#include "session.h"
#include "appaserver_user.h"

static APPASERVER_USER *global_appaserver_user = {0};

APPASERVER_USER *appaserver_user_calloc( void )
{
	APPASERVER_USER *a;

	if ( ! ( a = (APPASERVER_USER *)
			calloc(	1,
				sizeof( APPASERVER_USER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return a;

}

char *appaserver_user_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" appaserver_user \"%s\" none",
		appaserver_user_select(),
		where );

	return strdup( system_string );
}

APPASERVER_USER *appaserver_user_fetch(
			char *login_name,
			boolean fetch_role_name_list,
			boolean fetch_session_list )
{
	return appaserver_user_parse(
			string_pipe_fetch(
				appaserver_user_system_string(
					appaserver_user_primary_where(
						login_name ) ) ),
			fetch_role_name_list,
			fetch_session_list );
}

char *appaserver_user_primary_where(
			char *login_name )
{
	static char where[ 128 ];
	char *tmp;

	sprintf(where,
		"login_name = '%s'",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
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

/* Returns 1 if no error. */
/* ---------------------- */
boolean appaserver_user_insert(
			char *application_name,
			char *login_name,
			char *database_password,
			char *person_full_name,
			char *frameset_menu_horizontal_yn,
			char *user_date_format )
{
	FILE *output_pipe;
	char error_filename[ 128 ];
	boolean file_populated;

	sprintf( error_filename,
		 "/tmp/appaserver_user_insert_%d.err",
		 getpid() );

	output_pipe =
		appaserver_user_insert_open(
			application_name,
			error_filename,
			frameset_menu_horizontal_yn );

	appaserver_user_insert_stream(
		output_pipe,
		login_name,
		database_password,
		person_full_name,
		frameset_menu_horizontal_yn,
		user_date_format );

	pclose( output_pipe );

	file_populated = timlib_file_populated( error_filename );

	timlib_remove_file( error_filename );

	if ( file_populated )
		return 0;
	else
		return 1;
}

FILE *appaserver_user_insert_open(
			char *application_name,
			char *error_filename,
			char *frameset_menu_horizontal_yn )
{
	char sys_string[ 1024 ];
	char *field;
	char *table_name;
	FILE *output_pipe;

	if (	frameset_menu_horizontal_yn &&
		*frameset_menu_horizontal_yn )
	{
		field =	"login_name,"
			"password,"
			"person_full_name,"
			"frameset_menu_horizontal_yn,"
			"user_date_format";
	}
	else
	{
		field =	"login_name,"
			"password,"
			"person_full_name,"
			"user_date_format";
	}

	table_name =
		get_table_name(
			application_name,
			"appaserver_user" );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e 2>%s					 ",
		 table_name,
		 field,
		 error_filename );

	output_pipe = popen( sys_string, "w" );

	return output_pipe;
}

void appaserver_user_insert_stream(
			FILE *output_pipe,
			char *login_name,
			char *database_password,
			char *person_full_name,
			char *frameset_menu_horizontal_yn,
			char *user_date_format )
{
	char entity_buffer[ 128 ];

	if ( frameset_menu_horizontal_yn
	&&   *frameset_menu_horizontal_yn )
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s^%s\n",
			login_name,
			(database_password) ? database_password : "",
	 		escape_character(	entity_buffer,
						person_full_name,
						'\'' ),
			frameset_menu_horizontal_yn,
			user_date_format );
	}
	else
	{
		fprintf(output_pipe,
			"%s^%s^%s^%s\n",
			login_name,
			(database_password) ? database_password : "",
	 		escape_character(	entity_buffer,
						person_full_name,
						'\'' ),
			user_date_format );
	}
}

char *appaserver_user_select( void )
{
	static char select[ 256 ];

	if ( folder_attribute_exists(
			"appaserver_user",
			"full_name" ) )
	{
		strcpy(
			select,
			"login_name,full_name,password,user_date_format" );
	}
	else
	if ( folder_attribute_exists(
			"appaserver_user",
			"person_full_name" ) )
	{
		strcpy(
		   select,
		   "login_name,person_full_name,password,user_date_format" );
	}
	else
	{
		strcpy(
			select,
			"login_name,'',password,user_date_format" );
	}

	if ( folder_attribute_exists(
			"appaserver_user",
			"frameset_menu_horizontal_yn" ) )
	{
		sprintf(select + strlen( select ),
			",frameset_menu_horizontal_yn" );
	}
	return select;
}

APPASERVER_USER *appaserver_user_parse(
			char *input,
			boolean fetch_role_name_list,
			boolean fetch_session_list )
{
	APPASERVER_USER *appaserver_user;
	char piece_buffer[ 512 ];

	appaserver_user = appaserver_user_calloc();

	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	appaserver_user->login_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	appaserver_user->full_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	appaserver_user->database_password = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	appaserver_user->user_date_format = strdup( piece_buffer );

	if ( folder_attribute_exists(
			"appaserver_user",
			"frameset_menu_horizontal_yn" ) )
	{
		piece( piece_buffer, SQL_DELIMITER, input, 4 );
		appaserver_user->frameset_menu_horizontal =
			( *piece_buffer == 'y' );
	}

	if ( fetch_role_name_list )
	{
		appaserver_user->role_name_list =
			appaserver_user_role_name_list(
				appaserver_user->login_name );
	}

	if ( fetch_session_list )
	{
		appaserver_user->session_list =
			session_system_list(
				session_system_string(
					appaserver_user_primary_where(
					      appaserver_user->login_name ) ) );
	}

	return appaserver_user;
}

LIST *appaserver_user_role_name_list(
			char *login_name )
{
	char where[ 128 ];
	char system_string[ 1024 ];

	sprintf( where, "login_name = '%s'", login_name );

	sprintf(system_string,
		"select.sh role role_appaserver_user \"%s\" select",
		where );

	return pipe2list( sys_string );
}

char *appaserver_user_person_full_name(
			char *login_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				login_name, 0, 0 );
	}

	if ( !global_appaserver_user )
		return "";
	else
		return global_appaserver_user->full_name;
}

char *appaserver_user_password_fetch(
				char *login_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				login_name, 0, 0 );
	}

	if ( !global_appaserver_user )
		return "";
	else
		return global_appaserver_user->database_password;
}

boolean appaserver_user_exists_session(
			char *application_name,
			char *login_name,
			char *session )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				login_name, 0, 0 );
	}

	if ( !global_appaserver_user->session_list )
	{
		global_appaserver_user->session_list =
			appaserver_user_session_list(
				application_name,
				login_name );
	}

	return list_exists_string(
		session,
		global_appaserver_user->session_list );
}

boolean appaserver_user_exists_role(
			char *login_name,
			char *role_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				login_name,
				1 /* fetch_role_name_list */,
				0 /* not fetch_session_list */ );
	}

	return list_exists_string(
		role_name,
		global_appaserver_user->role_name_list );
}

FILE *appaserver_user_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 APPASERVER_USER_TABLE,
		 APPASERVER_USER_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void appaserver_user_update(
			FILE *update_pipe,
			char *injection_escaped_encrypted_password,
			char *login_name )
{
	fprintf( update_pipe,
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

