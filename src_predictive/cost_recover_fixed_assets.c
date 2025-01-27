/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/cost_recover_fixed_assets.c		*/
/* ---------------------------------------------------------------	*/
/* 									*/
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
#include "appaserver_parameter_file.h"
#include "fixed_asset_purchase.h"
#include "recovery.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean cost_recover_fixed_assets(
			int tax_year,
			boolean execute );

boolean cost_recover_fixed_assets_undo(
			int tax_year,
			boolean execute );

FILE *cost_recover_fixed_asset_undo_html_open(
			void );

void cost_recover_fixed_assets_undo_display(
			LIST *fixed_asset_purchase_list );

void cost_recover_fixed_assets_undo_execute(
			LIST *fixed_asset_purchase_list );

void cost_recover_period_of_record_display(
			void );

FILE *cost_recover_period_of_record_html_open(
			void );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	int tax_year;
	boolean undo;
	boolean execute;
	char buffer[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

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

	printf( "<h1>%s For Year %d</h1>\n",
		format_initial_capital(
			buffer,
			process_name ),
		( tax_year =
			date_year(
				date_subtract_year(
					date_now_new(
						date_utc_offset() ),
					1 ) ) ) );

	if ( !undo )
	{
		if ( cost_recover_fixed_assets( tax_year, execute ) )
		{
			if ( execute )
				printf( "<h3>Posting complete</h3>\n" );
			else
				printf( "<h3>Posting not executed</h3>\n" );
		}
		else
		{
			printf( "<h3>No fixed assets to recover</h3>\n" );

			cost_recover_period_of_record_display();
		}
	}
	else
	{
		if ( cost_recover_fixed_assets_undo( tax_year, execute ) )
		{
			if ( execute )
				printf( "<h3>Undo complete</h3>\n" );
			else
				printf( "<h3>Undo not executed</h3>\n" );
		}
		else
		{
			printf( "<h3>No recoveries to undo</h3>\n" );
		}
	}

	document_close();

	return 0;
}

boolean cost_recover_fixed_assets(
			int tax_year,
			boolean execute )
{
	LIST *fixed_asset_purchase_list;
	char where[ 1024 ];

	sprintf(where,
		"tax_adjusted_basis > 0 and not %s",
		recovery_subquery_where( tax_year ) );

	fixed_asset_purchase_list =
		fixed_asset_purchase_list_cost_recover(
			fixed_asset_purchase_list_fetch(
				where,
				0 /* not fetch_last_depreciation */,
				0 /* not fetch_last_recovery */ ),
			tax_year );

	if ( !list_length(
		fixed_asset_purchase_cost_recovery_list(
			fixed_asset_purchase_list ) ) )
	{
		return 0;
	}

	fixed_asset_purchase_recovery_display(
		fixed_asset_purchase_list );

	if ( execute )
	{
		recovery_list_insert(
			fixed_asset_purchase_cost_recovery_list(
				fixed_asset_purchase_list ) );

		fixed_asset_purchase_list_subtract_recovery_amount(
			fixed_asset_purchase_list );

		fixed_asset_purchase_list_update(
			fixed_asset_purchase_list );
	}
	return 1;
}

boolean cost_recover_fixed_assets_undo(
			int tax_year,
			boolean execute )
{
	LIST *fixed_asset_purchase_list;

	fixed_asset_purchase_list =
		fixed_asset_purchase_list_fetch(
			recovery_subquery_where( tax_year ),
			0 /* not fetch_last_depreciation */,
			1 /* fetch_last_recovery */ );

	if ( !list_length( fixed_asset_purchase_list ) ) return 0;

	cost_recover_fixed_assets_undo_display(
		fixed_asset_purchase_list );

	if ( execute )
	{
		cost_recover_fixed_assets_undo_execute(
			fixed_asset_purchase_list );
	}

	return 1;
}

void cost_recover_fixed_assets_undo_display(
			LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	FILE *html_output;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return;

	html_output = cost_recover_fixed_asset_undo_html_open();

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( !fixed_asset_purchase->recovery )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: empty recovery for %s/%s\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				fixed_asset_purchase->
					fixed_asset->
					asset_name,
				fixed_asset_purchase->
					serial_label );
			continue;
		}

		fprintf(html_output,
			"%s^%s^%s^%s\n",
			fixed_asset_purchase->fixed_asset->asset_name,
			fixed_asset_purchase->serial_label,
			fixed_asset_purchase->service_placement_date,
			/* --------------------- */
			/* Returns static memory */
			/* Doesn't trim pennies  */
			/* --------------------- */
			commas_in_money(
			 	fixed_asset_purchase->
					recovery->
					recovery_amount ) );

	} while ( list_next( fixed_asset_purchase_list ) );

	pclose( html_output );
}

void cost_recover_fixed_assets_undo_execute(
			LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	FILE *delete_pipe = {0};

	if ( !list_rewind( fixed_asset_purchase_list ) ) return;

	delete_pipe = recovery_delete_open();

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( !fixed_asset_purchase->recovery )
		{
			continue;
		}

		fprintf(delete_pipe,
		 	"%s^%s^%d\n",
		 	fixed_asset_purchase->
				fixed_asset->
				asset_name,
		 	fixed_asset_purchase->
				serial_label,
		 	fixed_asset_purchase->
				recovery->
				tax_year );

	} while ( list_next( fixed_asset_purchase_list ) );

	pclose( delete_pipe );

	fixed_asset_purchase_list_negate_recovery_amount(
		fixed_asset_purchase_list );

	fixed_asset_purchase_list_update(
		fixed_asset_purchase_list );
}

FILE *cost_recover_fixed_asset_undo_html_open( void )
{
	char sys_string[ 1024 ];
	char *heading_list_string;
	char *justify_list_string;

	heading_list_string =
		"asset_name,"
		"serial_label,"
		"service_placement,"
		"recovery_amount";

	justify_list_string = "left,left,left,right";

	sprintf( sys_string,
		 "html_table.e '^^%s' '%s' '^' '%s'",
		 "UNDO Cost Recover Fixed Assets",
		 heading_list_string,
		 justify_list_string );

	fflush( stdout );
	return popen( sys_string, "w" );
}

FILE *cost_recover_period_of_record_html_open( void )
{
	char sys_string[ 1024 ];
	char *heading_list_string;
	char *justify_list_string;

	heading_list_string =
		"tax_year,"
		"total_recovery";

	justify_list_string = "right,right";

	sprintf( sys_string,
		 "html_table.e '^^%s' '%s' '%c' '%s'",
		 "Period of Record",
		 heading_list_string,
		 SQL_DELIMITER,
		 justify_list_string );

	fflush( stdout );
	return popen( sys_string, "w" );
}

void cost_recover_period_of_record_display( void )
{
	FILE *output_pipe = cost_recover_period_of_record_html_open();
	FILE *input_pipe;
	char system_string[ 1024 ];
	char *select;
	char *group;
	char input[ 128 ];
	char tax_year[ 128 ];
	char total_recovery[ 128 ];

	select = "tax_year, sum( recovery_amount )";
	group = "tax_year";

	sprintf(system_string,
		"echo \"select %s from %s group by %s;\"	|"
		"sql.e						|"
		"sort -nr					|"
		"cat						 ",
		select,
		COST_RECOVERY_TABLE,
		group );

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 128 ) )
	{
		piece( tax_year, SQL_DELIMITER, input, 0 );
		piece( total_recovery, SQL_DELIMITER, input, 1 );

		fprintf(output_pipe,
			"%s%c%s\n",
			tax_year,
			SQL_DELIMITER,
			/* --------------------- */
			/* Returns static memory */
			/* Doesn't trim pennies  */
			/* --------------------- */
			commas_in_money(
				atof( total_recovery ) ) );
	}

	pclose( input_pipe );
	pclose( output_pipe );
}

