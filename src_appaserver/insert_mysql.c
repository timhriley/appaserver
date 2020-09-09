/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/insert_mysql.c			*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "environ.h"

/* #define DEBUG_MODE		1 */

/* Prototypes */
/* ---------- */
int insert_mysql_appaserver_user(
				char *application_name,
				int rows_to_insert,
				char *carrot_delimited_row_filename );

int insert_mysql(		char *application_name,
				int rows_to_insert,
				char *folder_name,
				char *attribute_comma_string,
				char *carrot_delimited_row_filename );

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name;
	char *attribute_comma_string;
	char *carrot_delimited_row_filename;
	int rows_to_insert;
	char buffer[ 1024 ];
	char *results;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s ignored folder attribute_comma_list carrot_delimited_row_filename\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	folder_name = argv[ 2 ];
	attribute_comma_string = argv[ 3 ];
	carrot_delimited_row_filename = argv[ 4 ];

	/* Count rows expected */
	/* ------------------- */
	sprintf( buffer, 
		 "wc -l %s | awk '{print $1}'",
		 carrot_delimited_row_filename );

	results = pipe2string( buffer );

	if ( !results )
		rows_to_insert = 0;
	else
		rows_to_insert = atoi( results );

	if ( !rows_to_insert )
	{
		printf( "0\n" );
		exit( 0 );
	}

	if ( strcmp( folder_name, "appaserver_user" ) == 0 )
	{
		rows_to_insert =
			insert_mysql_appaserver_user(
				application_name,
				rows_to_insert,
				carrot_delimited_row_filename );
	}
	else
	{
		rows_to_insert =
			insert_mysql(
				application_name,
				rows_to_insert,
				folder_name,
				attribute_comma_string,
				carrot_delimited_row_filename );
	}

	printf( "%d\n", rows_to_insert );

	return 0;
}

int insert_mysql(		char *application_name,
				int rows_to_insert,
				char *folder_name,
				char *attribute_comma_string,
				char *carrot_delimited_row_filename )
{
	FILE *p;
	char *table_name;
	char *sql_executable;
	char buffer[ 1024 ];

	table_name = get_table_name( application_name, folder_name );

#ifdef DEBUG_MODE
	sql_executable = "html_paragraph_wrapper.e";
#else
	sql_executable = "sql";
#endif
	sprintf( buffer,
		 "cat %s			|"
		 "unescape_single_quotes.e	|"
		 "insert_statement %s %s '^'	|"
		 "sed 's/\\\\\\$/$/g'		|"
		 "tee_appaserver_error.sh	|"
		 "%s 2>&1			 ",
		 carrot_delimited_row_filename,
		 table_name,
		 attribute_comma_string,
		 sql_executable );

	p = popen( buffer, "r" );

	/* Each line of input is a row not inserted */
	/* ---------------------------------------- */
	while( get_line( buffer, p ) )
	{
		appaserver_output_error_message(
			application_name, buffer, (char *)0 );
		fprintf( stderr, "%s\n", buffer );
		rows_to_insert--;
	}

	pclose( p );

	return rows_to_insert;

} /* insert_mysql() */

int insert_mysql_appaserver_user(
				char *application_name,
				int rows_to_insert,
				char *carrot_delimited_row_filename )
{
	FILE *f;
	char input_buffer[ 1024 ];
	APPASERVER_USER *appaserver_user;

	if ( ! ( f = fopen( carrot_delimited_row_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 carrot_delimited_row_filename );
		exit( 1 );
	}

	while ( get_line( input_buffer, f ) )
	{
		if ( ! ( appaserver_user =
				appaserver_user_parse(
					input_buffer ) ) )
		{
			fclose( f );
			exit( 1 );
		}

		appaserver_user->database_password =
			/* -------------------- */
			/* Returns heap memory. */
			/* -------------------- */
			appaserver_user_version_encrypted_password(
					application_name,
					appaserver_user->typed_in_password,
					appaserver_user_mysql_version() );

		if ( !appaserver_user_insert(
					application_name,
					appaserver_user->login_name,
					appaserver_user->person_full_name,
					appaserver_user->database_password,
					appaserver_user->user_date_format ) )
		{
			rows_to_insert--;
		}
	}

	fclose( f );
	return rows_to_insert;

} /* insert_mysql_appaserver_user() */

