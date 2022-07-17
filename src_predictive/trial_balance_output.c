/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/trial_balance_output.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "trial_balance.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *subclassification_option_string;
	char *output_medium_string;
	char *as_of_date;
	int prior_year_count;
	TRIAL_BALANCE *trial_balance;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s session login_name role process as_of_date prior_year_count subclassification_option output_medium\n",
			 argv[ 0 ] );

		fprintf( stderr,
"Note: subclassification_option={display,omit}\n" );

		fprintf( stderr,
"Note: output_medium={table,spreadsheet,PDF,stdout}\n" );

		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];
	as_of_date = argv[ 5 ];
	prior_year_count = atoi( argv[ 6 ] );
	subclassification_option_string = argv[ 7 ];
	output_medium_string = argv[ 8 ];

	trial_balance =
		trial_balance_fetch(
			application_name,
			session_key,
			role_name,
			process_name,
			as_of_date,
			prior_year_count,
			subclassification_option_string,
			output_medium_string );

	return 0;
}
