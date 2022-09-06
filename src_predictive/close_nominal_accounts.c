/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/close_nominal_accounts.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "transaction.h"
#include "close_nominal.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *as_of_date;
	boolean execute;
	CLOSE_NOMINAL *close_nominal;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s process as_of_date execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	as_of_date = argv[ 2 ];
	execute = (*argv[ 3 ] == 'y');

	close_nominal =
		close_nominal_fetch(
			application_name,
			process_name,
			as_of_date );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	if ( !close_nominal )
	{
		printf( "<h3>Nothing to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	printf( "%s\n",
		close_nominal->
			statement_caption->
			frame_title );

	if ( close_nominal->transaction_date_time_exists )
	{
		printf(
		"<h3>Warning: a closing entry exists for the date.</h3>\n" );
	}
	else
	{
		if ( execute )
		{
			close_nominal_transaction_insert(
				close_nominal->
					close_nominal_transaction->
					transaction );

			printf( "<h3>Process complete.</h3>\n" );
		}
		else
		{
			journal_list_html_display(
				close_nominal->
					close_nominal_transaction->
					transaction->
					journal_list,
				close_nominal->
					close_nominal_transaction->
					transaction->
					transaction_date_time,
				close_nominal->
					close_nominal_transaction->
					transaction->
					memo );
		}
	}

	document_close();

	return 0;
}
