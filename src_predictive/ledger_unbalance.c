/* ---------------------------------------------------------------	*/
/* src_predictive/ledger_unbalance.c					*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean report_error(			char *name_address_date );

boolean ledger_unbalance_input_buffer(	char *input_buffer );

void ledger_unbalance(			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s ignored\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	ledger_unbalance( application_name );

	return 0;

} /* main() */

void ledger_unbalance( char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	FILE *input_pipe;
	char input_buffer[ 1024 ];

	select =
"concat( full_name, '|', street_address, '|', transaction_date_time ), debit_amount, credit_amount";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=journal_ledger		"
		 "			order=transaction_date_time	",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 1024 ) )
	{
		if ( !*input_buffer )
		{
			pclose( input_pipe );
			return;
		}

		if ( !ledger_unbalance_input_buffer( input_buffer ) )
		{
			pclose( input_pipe );
			return;
		}
	}

	/* If no input, then skip this. */
	/* ---------------------------- */
	if ( *input_buffer )
	{
		ledger_unbalance_input_buffer( (char *)0 );
	}

	pclose( input_pipe );

} /* ledger_unbalance() */

boolean ledger_unbalance_input_buffer( char *input_buffer )
{
	static char prior_name_address_date[ 256 ] = {0};
	static double sum_debit_amount = 0.0;
	static double sum_credit_amount = 0.0;
	char name_address_date[ 256 ];
	char debit_string[ 32 ];
	char credit_string[ 32 ];
	double debit_amount;
	double credit_amount;

	/* If all done, need one more check. */
	/* --------------------------------- */
	if ( !input_buffer )
	{
		if ( !timlib_dollar_virtually_same(
			sum_debit_amount,
			sum_credit_amount ) )
		{
			return report_error( prior_name_address_date );
		}
	}

	piece( name_address_date, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	piece( debit_string, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	piece( credit_string, FOLDER_DATA_DELIMITER, input_buffer, 2 );

	debit_amount = atof( debit_string );
	credit_amount = atof( credit_string );

	/* If first time. */
	/* -------------- */
	if ( !*prior_name_address_date )
	{
		timlib_strcpy(
			prior_name_address_date,
			name_address_date,
			256 );

		sum_debit_amount = debit_amount;
		sum_credit_amount = credit_amount;

		return 1;
	}

	/* If change in transaction. */
	/* ------------------------- */
	if ( strcmp( name_address_date, prior_name_address_date ) != 0 )
	{
		if ( !timlib_dollar_virtually_same(
			sum_debit_amount,
			sum_credit_amount ) )
		{
			return report_error( prior_name_address_date );
		}
		timlib_strcpy(
			prior_name_address_date,
			name_address_date,
			256 );

		sum_debit_amount = debit_amount;
		sum_credit_amount = credit_amount;

		return 1;
	}

	/* If same transaction. */
	/* -------------------- */
	sum_debit_amount += debit_amount;
	sum_credit_amount += credit_amount;

	return 1;

} /* ledger_unbalance_input_buffer() */

boolean report_error( char *name_address_date )
{
	fprintf( stderr, "Unbalance at %s\n", name_address_date );
	return 0;
}

