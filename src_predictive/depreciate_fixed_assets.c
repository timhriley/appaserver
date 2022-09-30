/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/depreciate_fixed_assets.c		*/
/* ---------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "environ.h"
#include "document.h"
#include "appaserver_library.h"
#include "application.h"
#include "appaserver_error.h"
#include "entity.h"
#include "appaserver_parameter_file.h"
#include "account.h"
#include "predictive.h"
#include "fixed_asset_purchase.h"
#include "transaction.h"
#include "depreciation.h"

boolean depreciate_fixed_assets(
			boolean execute );

FILE *depreciate_fixed_assets_undo_html_open(
			void );

boolean depreciate_fixed_assets_undo(
			boolean execute );

void depreciate_fixed_assets_undo_display(
			LIST *fixed_asset_purchase_list );

void depreciate_fixed_assets_undo_execute(
			LIST *fixed_asset_purchase_list );

void depreciate_period_of_record_display(
			void );

FILE *depreciate_period_of_record_html_open(
			void );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	boolean execute;
	boolean undo;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 256 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s process undo_yn execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	undo = (*argv[ 2 ]) == 'y';
	execute = (*argv[ 3 ]) == 'y';

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s</h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );

	if ( !undo )
	{
		if ( depreciate_fixed_assets( execute ) )
		{
			if ( execute )
				printf( "<h3>Posting complete</h3>\n" );
			else
				printf( "<h3>Posting not executed</h3>\n" );
		}
		else
		{
			printf( "<h3>No fixed assets to depreciate</h3>\n" );

			depreciate_period_of_record_display();
		}
	}
	else
	{
		if ( depreciate_fixed_assets_undo( execute ) )
		{
			if ( execute )
				printf( "<h3>Undo complete</h3>\n" );
			else
				printf( "<h3>Undo not executed</h3>\n" );
		}
		else
		{
			printf( "<h3>No depreciations to undo</h3>\n" );
		}
	}

	document_close();

	return 0;
}

boolean depreciate_fixed_assets( boolean execute )
{
	LIST *fixed_asset_purchase_list;
	char *depreciation_date;

	depreciation_date =
		/* Returns heap memory */
		/* ------------------- */
		fixed_asset_purchase_depreciation_date();

	fixed_asset_purchase_list =
		/* -------------------------------------------- */
		/* Returns fixed_asset_purchase_list with	*/
		/* fixed_asset_purchase->depreciation set.	*/
		/* -------------------------------------------- */
		fixed_asset_purchase_list_depreciate(
			fixed_asset_purchase_list_fetch(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				fixed_asset_purchase_depreciation_where(
					depreciation_date ),
				1 /* fetch_last_depreciation */,
				0 /* not fetch_last_recovery */ ),
			depreciation_date );

	if ( !list_length(
		fixed_asset_purchase_depreciation_list(
			fixed_asset_purchase_list ) ) )
	{
		return 0;
	}

	fixed_asset_purchase_depreciation_display(
		fixed_asset_purchase_list );

	if ( execute )
	{
		LIST *depreciation_list;

		depreciation_list_insert(
			( depreciation_list =
				fixed_asset_purchase_depreciation_list(
					fixed_asset_purchase_list ) ) );

		fixed_asset_purchase_list_add_depreciation_amount(
			fixed_asset_purchase_list );

		fixed_asset_purchase_list_update(
			fixed_asset_purchase_list );

		/* Sets true transaction_date_time */
		/* ------------------------------- */
		transaction_list_insert(
			depreciation_transaction_list(
				depreciation_list ),
			1 /* insert_journal_list_boolean */ );
	}
	return 1;
}

FILE *depreciate_fixed_assets_undo_html_open( void )
{
	char sys_string[ 1024 ];
	char *heading_list_string;
	char *justify_list_string;

	heading_list_string =
		"asset_name,"
		"serial_label,"
		"service_placement,"
		"depreciation,"
		"depreciation_amount";

	justify_list_string = "left,left,left,left,right";

	sprintf( sys_string,
		 "html_table.e '^^%s' '%s' '^' '%s'",
		 "UNDO Depreciate Fixed Assets",
		 heading_list_string,
		 justify_list_string );

	fflush( stdout );
	return popen( sys_string, "w" );
}

boolean depreciate_fixed_assets_undo( boolean execute )
{
	LIST *fixed_asset_purchase_list;
	char *prior_depreciation_date;

	prior_depreciation_date =
		depreciation_prior_depreciation_date(
			DEPRECIATION_TABLE,
			(char *)0 /* asset_name */,
			(char *)0 /* serial_label */,
			(char *)0 /* current_depreciation_date */ );

	if ( !prior_depreciation_date ) return 0;

	fixed_asset_purchase_list =
		fixed_asset_purchase_list_fetch(
			depreciation_subquery_where(
				DEPRECIATION_TABLE,
				FIXED_ASSET_PURCHASE_TABLE,
				prior_depreciation_date ),
			1 /* fetch_last_depreciation */,
			0 /* not fetch_last_recovery */ );

	if ( !list_length( fixed_asset_purchase_list ) ) return 0;

	depreciate_fixed_assets_undo_display(
		fixed_asset_purchase_list );

	if ( execute )
	{
		depreciate_fixed_assets_undo_execute(
			fixed_asset_purchase_list );
	}

	return 1;
}

void depreciate_fixed_assets_undo_display(
			LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	FILE *html_output;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return;

	html_output = depreciate_fixed_assets_undo_html_open();

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( !fixed_asset_purchase->depreciation )
		{
			continue;
		}

		fprintf( html_output,
			 "%s^%s^%s^%s^%.2lf\n",
			 fixed_asset_purchase->fixed_asset->asset_name,
			 fixed_asset_purchase->serial_label,
			 fixed_asset_purchase->service_placement_date,
			 fixed_asset_purchase->
				depreciation->
				depreciation_date,
			 fixed_asset_purchase->
				depreciation->
				amount );

	} while ( list_next( fixed_asset_purchase_list ) );

	pclose( html_output );
}

void depreciate_fixed_assets_undo_execute(
			LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	FILE *delete_pipe = {0};

	if ( !list_rewind( fixed_asset_purchase_list ) ) return;

	delete_pipe = depreciation_delete_open();

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( !fixed_asset_purchase->depreciation )
		{
			continue;
		}

		fprintf(delete_pipe,
		 	"%s^%s^%s\n",
		 	fixed_asset_purchase->
				fixed_asset->
				asset_name,
		 	fixed_asset_purchase->
				serial_label,
		 	fixed_asset_purchase->
				depreciation->
				depreciation_date );

		if ( !fixed_asset_purchase->
				depreciation->
				depreciation_transaction )
		{
			fprintf(stderr,
		"Warning in %s/%s()/%d: empty depreciation_transaction.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );

			continue;
		}

		/* Performs journal_propagate() */
		/* ---------------------------- */
		transaction_delete(
			fixed_asset_purchase->
				depreciation->
				depreciation_transaction->
				full_name,
			fixed_asset_purchase->
				depreciation->
				depreciation_transaction->
				street_address,
			fixed_asset_purchase->
				depreciation->
				depreciation_transaction->
				transaction_date_time );

	} while ( list_next( fixed_asset_purchase_list ) );

	pclose( delete_pipe );

	fixed_asset_purchase_list_negate_depreciation_amount(
		fixed_asset_purchase_list );

	fixed_asset_purchase_list_update(
		fixed_asset_purchase_list );
}

FILE *depreciate_period_of_record_html_open( void )
{
	char sys_string[ 1024 ];
	char *heading_list_string;
	char *justify_list_string;

	heading_list_string =
		"depreciation_date,"
		"total_depreciation";

	justify_list_string = "left,right";

	sprintf( sys_string,
		 "html_table.e '^^%s' '%s' '^' '%s'",
		 "Period of Record",
		 heading_list_string,
		 justify_list_string );

	fflush( stdout );
	return popen( sys_string, "w" );
}

void depreciate_period_of_record_display( void )
{
	FILE *output_pipe = depreciate_period_of_record_html_open();
	FILE *input_pipe;
	char system_string[ 1024 ];
	char *select;
	char *group;
	char input[ 128 ];

	select = "depreciation_date, sum( depreciation_amount )";
	group = "depreciation_date";

	sprintf(system_string,
		"echo \"select %s from %s group by %s;\"	|"
		"sql.e						|"
		"sort -nr					|"
		"cat						 ",
		select,
		DEPRECIATION_TABLE,
		group );

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 128 ) )
	{
		fprintf( output_pipe, "%s\n", input );
	}

	pclose( input_pipe );
	pclose( output_pipe );
}

