/* appaserver_user.c 							*/
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
#include "appaserver_user.h"
#include "appaserver_library.h"
#include "boolean.h"
#include "piece.h"
#include "folder.h"
#include "timlib.h"

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

} /* appaserver_user_calloc() */

APPASERVER_USER *appaserver_user_fetch(
				char *application_name,
				char *login_name )
{
	char sys_string[ 1024 ];
	char piece_buffer[ 1024 ];
	char *input_string;
	APPASERVER_USER *appaserver_user;
	char *table_name;
	char select_string[ 128 ];
	char where_string[ 128 ];
	char *select_frameset;
	char *select_person;

	table_name = get_table_name( application_name, "appaserver_user" );

	if ( folder_exists_attribute(	application_name,
					"appaserver_user",
					"person_full_name" ) )
	{
		select_person = "person_full_name";
	}
	else
	{
		select_person = "null";
	}

	if ( folder_exists_attribute(	application_name,
					"appaserver_user",
					"frameset_menu_horizontal_yn" ) )
	{
		select_frameset = "frameset_menu_horizontal_yn";
	}
	else
	{
		select_frameset = "'y'";
	}

	sprintf( select_string,
		 "%s,password,%s",
		 select_person,
		 select_frameset );

	sprintf( where_string, "login_name = '%s'", login_name );

	sprintf(	sys_string,
			"echo \"select %s from %s where %s;\" | sql.e '%c'",
			select_string,
			table_name,
			where_string,
			APPASERVER_USER_RECORD_DELIMITER );

	input_string = pipe2string( sys_string );

	if ( !input_string )
	{
		fprintf(stderr,
	"Warning in %s/%s/%d(): cannot get record for login_name = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name );
		return (APPASERVER_USER *)0;
	}

	if ( character_count( APPASERVER_USER_RECORD_DELIMITER,
			      input_string ) != 2 )
	{
		fprintf(stderr,
			"ERROR in %s/%s/%d(): invalid input = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			input_string );
		exit( 1 );
	}

	appaserver_user = appaserver_user_calloc();

	piece(	piece_buffer,
		APPASERVER_USER_RECORD_DELIMITER,
		input_string,
		0 );
	appaserver_user->person_full_name = strdup( piece_buffer );

	piece(	piece_buffer,
		APPASERVER_USER_RECORD_DELIMITER,
		input_string,
		1 );
	appaserver_user->database_password = strdup( piece_buffer );

	piece(	piece_buffer,
		APPASERVER_USER_RECORD_DELIMITER,
		input_string,
		2 );
	appaserver_user->frameset_menu_horizontal =
		( *piece_buffer == 'y' );

	return appaserver_user;

} /* appaserver_user_fetch() */

LIST *appaserver_user_session_list(	char *application_name,
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

} /* appaserver_user_session_list() */

char *appaserver_user_person_full_name(
			char *application_name, char *login_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				application_name, login_name );
	}

	if ( !global_appaserver_user )
		return "";
	else
		return global_appaserver_user->person_full_name;

} /* appaserver_user_person_full_name() */

char *appaserver_user_password_fetch(
				char *application_name,
				char *login_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				application_name, login_name );
	}

	if ( !global_appaserver_user )
		return "";
	else
		return global_appaserver_user->database_password;

} /* appaserver_user_password_fetch() */

boolean appaserver_user_frameset_menu_horizontal(
						char *application_name,
						char *login_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				application_name, login_name );
	}

	if ( !global_appaserver_user )
	{
		return 0;
	}

	return global_appaserver_user->frameset_menu_horizontal;

} /* appaserver_user_frameset_menu_horizontal() */

boolean appaserver_user_exists_session(	char *application_name,
					char *login_name,
					char *session )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				application_name, login_name );
	}

	if ( !global_appaserver_user->session_list )
	{
		global_appaserver_user->session_list =
			appaserver_user_session_list(
				application_name,
				login_name );
	}

	return list_exists_string(
		global_appaserver_user->session_list,
		session );

} /* appaserver_user_exists_session() */

boolean appaserver_user_exists_role(	char *application_name,
					char *login_name,
					char *role_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_fetch(
				application_name, login_name );
	}

	if ( !global_appaserver_user->role_list )
	{
		global_appaserver_user->role_list =
			appaserver_user_role_list(
				application_name,
				login_name );
	}

	return list_exists_string(
		global_appaserver_user->role_list,
		role_name );

} /* appaserver_user_exists_role() */

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

} /* appaserver_user_role_list() */

enum password_function
	appaserver_user_password_function(
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
		return password_function;
	}
	if ( str_len == 56 )
	{
		return sha2_function;
	}
	else
	{
		return no_encryption;
	}

} /* appaserver_user_password_function() */

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_encryption_select(
			enum password_function
				appaserver_user_password_function,
			char *typed_in_password )
{
	char encryption_function[ 128 ];

	if ( appaserver_user_password_function == no_encryption )
	{
		/* Returns heap memory */
		/* ------------------- */
		return timlib_escape_sql_injection(
				typed_in_password );
	}
	else
	if ( appaserver_user_password_function == old_password_function )
	{
		sprintf( encryption_function,
			 "old_password( '%s' )",
			 timlib_escape_sql_injection(
				typed_in_password ) );
	}
	else
	if ( appaserver_user_password_function == password_function )
	{
		sprintf( encryption_function,
			 "password( '%s' )",
			 timlib_escape_sql_injection(
				typed_in_password ) );
	}
	else
	if ( appaserver_user_password_function == sha2_function )
	{
		sprintf( encryption_function,
			 "sha2( '%s', 224 )",
			 timlib_escape_sql_injection(
				typed_in_password ) );
	}
	else
	{
		/* Returns heap memory */
		/* ------------------- */
		return timlib_escape_sql_injection(
				typed_in_password );
	}

	return strdup( encryption_function );

} /* appaserver_user_encryption_select() */

/* Returns heap memory */
/* ------------------- */
char *appaserver_user_function_encrypted_password(
				char *application_name,
				char *typed_in_password,
				enum password_function
					password_function )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *select;
	char *results;

	sprintf( where, "application = '%s'", application_name );

	if ( ! ( select = 
			appaserver_user_encryption_select(
				password_function,
				typed_in_password ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot appaserver_user_encryption_select()\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=application	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 sys_string );
		exit( 1 );
	}

	return results;

} /* appaserver_user_function_encrypted_password() */

boolean appaserver_user_password_match(
					char *application_name,
					char *typed_in_password,
					char *database_password )
{
	char *encrypted_password;
	enum password_function password_function;

	password_function =
		appaserver_user_password_function(
			database_password );

	encrypted_password =
		appaserver_user_function_encrypted_password(
			application_name,
			typed_in_password,
			password_function );

	return ( timlib_strcmp( encrypted_password, database_password ) == 0 );

} /* appaserver_user_password_match() */

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

} /* appaserver_user_mysql_version() */

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_version_encrypted_password(
					char *application_name,
					char *typed_in_password,
					char *mysql_version )
{
	enum password_function password_function;

	password_function =
		appaserver_user_version_password_function(
			mysql_version );

	return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_user_function_encrypted_password(
				application_name,
				typed_in_password,
				password_function );

} /* appaserver_user_version_encrypted_password() */

enum password_function
	appaserver_user_version_password_function(
					char *mysql_version )
{
	int version;

	if ( !mysql_version || !isdigit( *mysql_version ) )
		return no_encryption;

	version = atoi( mysql_version );

	if ( version < 5 )
		return old_password_function;
	else
	if ( version == 5 )
		return password_function;
	else
		return sha2_function;

} /* appaserver_user_version_password_function() */

/* Returns 1 if no error. */
/* ---------------------- */
boolean appaserver_user_insert(		char *application_name,
					char *login_name,
					char *person_full_name,
					char *database_password,
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
			error_filename );

	appaserver_user_insert_stream(
		output_pipe,
		login_name,
		person_full_name,
		database_password,
		user_date_format );

	pclose( output_pipe );

	file_populated = timlib_file_populated( error_filename );

	timlib_remove_file( error_filename );

	if ( file_populated )
		return 0;
	else
		return 1;

} /* appaserver_user_insert() */

FILE *appaserver_user_insert_open(	char *application_name,
					char *error_filename )
{
	char sys_string[ 1024 ];
	char *field;
	char *table_name;
	FILE *output_pipe;

	field = "login_name,person_full_name,password,user_date_format";

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

} /* appaserver_user_insert_open() */

void appaserver_user_insert_stream(	FILE *output_pipe,
					char *login_name,
					char *person_full_name,
					char *database_password,
					char *user_date_format )
{
	char entity_buffer[ 128 ];

	fprintf(	output_pipe,
			"%s^%s^%s^%s\n",
			login_name,
	 		escape_character(	entity_buffer,
						person_full_name,
						'\'' ),
			(database_password) ? database_password : "",
			user_date_format );

} /* appaserver_user_insert_stream() */

APPASERVER_USER *appaserver_user_parse(
				char *input_buffer )
{
	APPASERVER_USER *appaserver_user;
	char piece_buffer[ 512 ];

	appaserver_user = appaserver_user_calloc();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	appaserver_user->login_name = strdup( piece_buffer );

	if ( !piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: not enough delimiters in [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_buffer );
		return (APPASERVER_USER *)0;
	}

	appaserver_user->person_full_name = strdup( piece_buffer );

	if ( !piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: not enough delimiters in [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_buffer );
		return (APPASERVER_USER *)0;
	}

	appaserver_user->typed_in_password = strdup( piece_buffer );

	if ( !piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: not enough delimiters in [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_buffer );
		return (APPASERVER_USER *)0;
	}

	appaserver_user->user_date_format = strdup( piece_buffer );

	return appaserver_user;

} /* appaserver_user_parse() */

