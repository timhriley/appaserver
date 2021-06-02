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
	char *where;

	where = "tax_adjusted_basis > 0";

	fixed_asset_purchase_list =
		fixed_asset_purchase_list_cost_recover(
			fixed_asset_purchase_list_fetch(
				where,
				0 /* not fetch_last_depreciation */ ),
			tax_year );

	if ( !list_length( fixed_asset_purchase_list ) ) return 0;

	fixed_asset_purchase_recovery_display(
		fixed_asset_purchase_list );

	if ( execute )
	{
		LIST *cost_recovery_list;

		cost_recovery_list =
			fixed_asset_purchase_cost_recovery_list(
				fixed_asset_purchase_list );

		recovery_list_insert( cost_recovery_list );

		fixed_asset_purchase_list_update(
			fixed_asset_purchase_list );
	}
	return 1;
}

boolean cost_recover_fixed_assets_undo(
			int tax_year,
			boolean execute )
{
	return 1;
}
