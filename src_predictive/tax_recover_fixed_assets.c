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
			boolean execute );

boolean tax_recover_fixed_assets_undo(
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	boolean execute;
	char buffer[ 128 ];
	boolean undo;
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

	printf( "<h1>%s</h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );

	if ( !undo )
	{
		if ( tax_recover_fixed_assets( execute ) )
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
			boolean execute )
{
	LIST *fixed_asset_purchase_list;
	char where[ 512 ];

	sprintf(where,
		"tax_accumulated_recovery < cost_basis"
		" and disposal_date is null" );

	fixed_asset_purchase_list =
		fixed_asset_purchase_list_tax_recover(
			fixed_asset_purchase_list_fetch(
				where,
				0 /* not fetch_last_depreciation */,
				0 /* not fetch_last_recovery */ ),
			tax_recovery_now_year() );

	if ( !list_length( fixed_asset_purchase_list ) ) return 0;

	fixed_asset_purchase_depreciation_display(
		fixed_asset_purchase_list );

	if ( execute )
	{
		LIST *depreciation_list;

		depreciation_list =
			fixed_asset_purchase_depreciation_list(
				fixed_asset_purchase_list );

		depreciation_list_insert( depreciation_list );

		fixed_asset_purchase_list_update(
			fixed_asset_purchase_list );

		/* Sets true transaction_date_time */
		/* ------------------------------- */
		transaction_list_insert(
			depreciation_transaction_list(
				depreciation_list ),
			1 /* lock_transaciton */ );

	}
	return 1;
#ifdef NOT_DEFINED
	int max_tax_year;
	int now_year;
	int tax_year;
	DEPRECIATION_STRUCTURE *depreciation_structure;

	/* ----- */
	/* Input */
	/* ----- */

	/* Sets depreciation_date and depreciation_tax_year */
	/* ------------------------------------------------ */
	depreciation_structure =
		depreciation_structure_new(
			application_name );

	if ( !depreciation_structure->depreciation_tax_year )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty depreciation_tax_year.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* ------------------------------------- */
	/* Sets depreciation_expense_account and */
	/*      accumulated_depreciation_account */
	/* ------------------------------------- */
	depreciation_structure->depreciation_fund_list =
		depreciation_fetch_fund_list(
			application_name );

	now_year = atoi( date_get_now_yyyy_mm_dd( date_get_utc_offset() ) );

	if ( depreciation_structure->depreciation_tax_year->max_undo_year )
	{
		max_tax_year =
			depreciation_structure->
				depreciation_tax_year->
				max_undo_year;
	}
	else
	{
		max_tax_year = 0;
	}

	if ( undo )
	{
		if ( !max_tax_year )
		{
			printf(
			"<h3>Error: no tax recoveries posted.</h3>\n" );
			return;
		}
	}
	else
	{
		if ( max_tax_year == now_year )
		{
			printf(
			"<h3>Error: tax recovery posted already.</h3>\n" );
			return;
		}

		if ( max_tax_year )
			tax_year = max_tax_year + 1;
		else
			tax_year = now_year;
	}

	/* ------- */
	/* Process */
	/* ------- */
	if ( execute )
	{
		if ( undo )
		{
			tax_recover_fixed_assets_undo(
				application_name,
				max_tax_year );

			printf(
		"<h3>Tax Fixed Asset Recovery for %d is now deleted.</h3>\n",
				max_tax_year );

			return;
		}

		/* Insert */
		/* ------ */
		tax_recovery_depreciation_fund_list_set(
			depreciation_structure->
				depreciation_fund_list,
			tax_year );

		if ( tax_recovery_fixed_assets_insert(
			depreciation_structure->
				depreciation_fund_list ) )
		{
			printf( "<h3>Tax Recovery now posted for %d.</h3>\n",
				tax_year );
		}
		else
		{
			printf(
			"<h3>Error: No fixed assets to recover.</h3>\n" );
		}
	}
	else
	/* ------- */
	/* Display */
	/* ------- */
	{
		if ( undo )
		{
			printf(
			"<h3>Will undo tax recovery posted for %d.</h3>\n",
				tax_year );
			return;
		}

		tax_recovery_depreciation_fund_list_set(
			depreciation_structure->
				depreciation_fund_list,
			tax_year );

		tax_recovery_fixed_assets_display(
			depreciation_structure->
				depreciation_fund_list );
	} /* if Display */

	return 1;
#endif
}

boolean tax_recover_fixed_assets_undo(
			boolean execute )
{
	char sys_string[ 1024 ];
	char *folder_name;
	char where[ 128 ];

	sprintf( where, "tax_year = %d", max_tax_year );

	/* TAX_FIXED_ASSET_RECOVERY */
	/* ------------------------ */
	folder_name = "tax_fixed_asset_recovery";

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 folder_name,
		 where );

	system( sys_string );

	/* TAX_PROPERTY_RECOVERY */
	/* --------------------- */
	folder_name = "tax_property_recovery";

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 folder_name,
		 where );

	system( sys_string );

	/* TAX_PRIOR_FIXED_ASSET_RECOVERY */
	/* ------------------------------ */
	folder_name = "tax_prior_fixed_asset_recovery";

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 folder_name,
		 where );

	system( sys_string );

	/* TAX_PRIOR_PROPERTY_RECOVERY */
	/* --------------------------- */
	folder_name = "tax_prior_property_recovery";

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 folder_name,
		 where );

	system( sys_string );

	sprintf(sys_string,
		"tax_accumulated_depreciation_reset.sh %s",
		application_name );

	system( sys_string );

	return 1;
}

