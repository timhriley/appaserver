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
#include "boolean.h"
#include "piece.h"
#include "folder.h"
#include "timlib.h"
#include "attribute.h"
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
			boolean fetch_role_list,
			boolean fetch_attribute_exclude_list,
			boolean fetch_session_list )
{
	return appaserver_user_parse(
			string_pipe_fetch(
				appaserver_user_system_string(
					appaserver_user_primary_where(
						login_name ) ) ),
			fetch_role_list,
			fetch_attribute_exclude_list,
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

enum password_function
	appaserver_user_database_password_function(
			char *database_password )
{
	int str_len;

	str_len = strlen( database_password );

	if ( str_len == 16 )
	{
		return old_password_function;
	}
	else
	if ( str_len == 41 && *database_password == '*' )
	{
		return regular_password_function;
	}
	if ( str_len == 56 )
	{
		return sha2_function;
	}
	else
	{
		return no_encryption;
	}
}

char *appaserver_user_encryption_select_clause(
			enum password_function password_function,
			char *password )
{
	char select_clause[ 128 ];

	if ( password_function == no_encryption )
	{
		sprintf(select_clause,
			"'%s'",
			password );
	}
	else
	if ( password_function == old_password_function )
	{
		sprintf(select_clause,
			"old_password('%s')",
			password );
	}
	else
	if ( password_function == regular_password_function )
	{
		sprintf(select_clause,
			"password('%s')",
			password );
	}
	else
	if ( password_function == sha2_function )
	{
		sprintf(select_clause,
			"sha2('%s',224 )",
			password );
	}
	else
	{
		sprintf(select_clause,
			"'%s'",
			password );
	}

	return strdup( select_clause );
}

char *appaserver_user_encrypted_password(
			char *application_name,
			char *password,
			enum password_function password_function )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *select_clause;
	char *results;
	char *table_name;

	table_name = get_table_name( application_name, "application" );

	sprintf( where, "application = '%s'", application_name );

	if ( ! ( select_clause = 
			appaserver_user_encryption_select_clause(
				password_function,
				password ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: appaserver_user_encryption_select_clause() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 select_clause,
		 table_name,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: password function not supported = %d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 password_function );
		exit( 1 );
	}

	return results;
}

boolean appaserver_user_password_match(
			char *database_password,
			char *injection_escaped_encrypted_password )
{
	if ( !database_password || !*database_password ) return 0;

	if ( string_strcmp( database_password, "null" ) == 0 ) return 0;

	if ( !injection_escaped_encrypted_password
	||   !*injection_escaped_encrypted_password )
	{
		return 0;
	}

	return ( strcmp(
			injection_escaped_encrypted_password,
			database_password ) == 0 );
}

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_mysql_version( void )
{
	static char *version = {0};

	if ( !version )
	{
		version = pipe2string( "mysql_version.sh" );
	}

	return version;
}

enum password_function
	appaserver_user_mysql_version_password_function(
			char *mysql_version )
{
	double version;

	if ( !mysql_version || !isdigit( *mysql_version ) )
		return no_encryption;

	version = atof( mysql_version );

	if ( version < 5.0 )
		return old_password_function;
	else
	if ( version <= 5.6 )
		return regular_password_function;
	else
		return sha2_function;
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
	if ( attribute_exists(
			environment_application_name(),
			"appaserver_user",
			"full_name" ) )
	{
		return "login_name,full_name,password,user_date_format";
	}
	else
	if ( attribute_exists(
			environment_application_name(),
			"appaserver_user",
			"person_full_name" ) )
	{
		return "login_name,person_full_name,password,user_date_format";
	}
	else
	{
		return "login_name,'',password,user_date_format";
	}
}

APPASERVER_USER *appaserver_user_parse(
			char *input,
			boolean fetch_role_list,
			boolean fetch_attribute_exclude_list,
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

	if ( fetch_role_list )
	{
		appaserver_user->role_list =
			role_system_list(
				role_system_string(
					appaserver_user_primary_where(
					      appaserver_user->login_name ) ),
				fetch_attribute_exclude_list );
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

LIST *appaserver_user_role_list(
			char *application_name,
			char *login_name )
{
	char where[ 128 ];
	char sys_string[ 1024 ];

	sprintf( where, "login_name = '%s'", login_name );
	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=role			"
		 "			folder=role_appaserver_user	"
		 "			where=\"%s\"			",
		 application_name,
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
				login_name, 0, 0, 0 );
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
				login_name, 0, 0, 0 );
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
				login_name, 0, 0, 0 );
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

boolean appaserver_user_exists_role(	char *application_name,
					char *login_name,
					char *role_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				login_name, 0, 0, 0 );
	}

	if ( !global_appaserver_user->role_list )
	{
		global_appaserver_user->role_list =
			appaserver_user_role_list(
				application_name,
				login_name );
	}

	return list_exists_string(
		role_name,
		global_appaserver_user->role_list );
}

boolean appaserver_user_password_encrypted(
			char *password )
{
	enum password_function password_function;

	password_function =
		appaserver_user_database_password_function(
			password );

	if ( password_function == no_encryption )
		return 0;
	else
		return 1;
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
