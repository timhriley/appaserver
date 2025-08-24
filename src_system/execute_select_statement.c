/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_system/execute_select_statement.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "list.h"
#include "environ.h"
#include "execute_select.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *session_key;
	char *role_name;
	char *select_statement_title;
	char *login_name;
	char *statement;
	APPASERVER_PARAMETER *appaserver_parameter;
	EXECUTE_SELECT *execute_select;
	int row_count;

	application_name =
		environ_exit_application_name(
			argv[ 0 ] );

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s process session role select_statement_title login_name statement\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	session_key = argv[ 2 ];
	role_name = argv[ 3 ];
	select_statement_title = argv[ 4 ];
	login_name = argv[ 5 ];
	statement = argv[ 6 ];

	appaserver_error_argv_file(
		argc,
		argv,
		application_name,
		login_name );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	if ( ( !*statement
	||     strcmp( statement, "statement" ) == 0 )
	&&   ( !*select_statement_title
	||     strcmp(	select_statement_title,
			"select_statement_title" ) == 0 ) )
	{
		printf( "<h3>Insufficient input.</h3>\n" );
		document_close();
		exit( 0 );
	}

	execute_select =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		execute_select_new(
			application_name,
			session_key,
			role_name,
			select_statement_title,
			statement,
			appaserver_parameter->data_directory );

	if ( execute_select->error_message )
	{
		printf( "<h3>%s</h3>\n", execute_select->error_message );
		document_close();
		exit( 0 );
	}

	if ( !list_length( execute_select->column_name_list ) )
	{
		printf( "<h3>No columns present to select.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*statement
	||   strcmp( statement, "statement" ) == 0 )
	{
		printf( "<p>%s\n", execute_select->statement );
		fflush( stdout );
	}

	row_count =
		execute_select_generate(
			execute_select->html_system_string,
			execute_select->output_filename,
			execute_select->spreadsheet_heading_string,
			execute_select->spreadsheet_system_string,
			execute_select->error_filename,
			execute_select->input_system_string );

	printf( "<h3>Row count: %d</h3>\n", row_count );

	printf( "%s\n",
		execute_select->appaserver_link_anchor_html );

	/* Both </body> and </html> */
	/* ------------------------ */
	document_close();

	return 0;
}
