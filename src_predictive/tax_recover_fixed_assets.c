/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/tax_recover_fixed_assets.c		*/
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
#include "tax_recovery.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean tax_recover_fixed_assets(
			int tax_year,
			boolean execute );

boolean tax_recover_fixed_assets_undo(
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
		if ( tax_recover_fixed_assets( tax_year, execute ) )
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
		if ( tax_recover_fixed_assets_undo( execute ) )
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

boolean tax_recover_fixed_assets(
			int tax_year,
			boolean execute )
{
	LIST *fixed_asset_purchase_list;
	char *where;

	where = "tax_accumulated_recovery < cost_basis";

	fixed_asset_purchase_list =
		fixed_asset_purchase_list_tax_recover(
			fixed_asset_purchase_list_fetch(
				where,
				0 /* not fetch_last_depreciation */,
				0 /* not fetch_last_recovery */ ),
			tax_year );

	if ( !list_length( fixed_asset_purchase_list ) ) return 0;

	fixed_asset_purchase_recovery_display(
		fixed_asset_purchase_list );

	if ( execute )
	{
		LIST *tax_recovery_list;

		tax_recovery_list =
			fixed_asset_purchase_tax_recovery_list(
				fixed_asset_purchase_list );

		tax_recovery_list_insert( tax_recovery_list );

		fixed_asset_purchase_list_update(
			fixed_asset_purchase_list );
	}
	return 1;
}

boolean tax_recover_fixed_assets_undo(
			boolean execute )
{
	return 1;
}
