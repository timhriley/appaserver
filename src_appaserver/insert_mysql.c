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
#include "environ.h"

/* Prototypes */
/* ---------- */
int insert_mysql(	char *application_name,
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

	rows_to_insert =
		insert_mysql(
			application_name,
			rows_to_insert,
			folder_name,
			attribute_comma_string,
			carrot_delimited_row_filename );

	printf( "%d\n", rows_to_insert );

	return 0;
}

int insert_mysql(	char *application_name,
			int rows_to_insert,
			char *folder_name,
			char *attribute_comma_string,
			char *carrot_delimited_row_filename )
{
	FILE *p;
	char *table_name;
	char buffer[ 1024 ];

	table_name = get_table_name( application_name, folder_name );

	sprintf( buffer,
		 "cat %s			|"
		 "unescape_single_quotes.e	|"
		 "insert_statement %s %s '^'	|"
		 "sed 's/\\\\\\$/$/g'		|"
		 "tee_appaserver_error.sh	|"
		 "sql 2>&1			 ",
		 carrot_delimited_row_filename,
		 table_name,
		 attribute_comma_string );

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
}
