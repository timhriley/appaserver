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

/* In fixed_asset.h, include tax_recovery.h and depreciation.h */
/* ----------------------------------------------------------- */
#include "fixed_asset.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void tax_recover_fixed_assets(		char *application_name,
					boolean undo,
					boolean execute );

void tax_recover_fixed_assets_undo(	char *application_name,
					int max_tax_year );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	boolean execute;
	char buffer[ 128 ];
	boolean undo;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;

	application_name = environ_get_application_name( argv[ 0 ] );

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

	document = document_new( process_name /* title */, application_name );
	document->output_content_type = 1;
	
	document_output_heading(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );
	
	document_output_body(	document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );

	tax_recover_fixed_assets(
		application_name,
		undo,
		execute );

	document_close();

	return 0;

} /* main() */

void tax_recover_fixed_assets(	char *application_name,
				boolean undo,
				boolean execute )
{
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

} /* tax_recover_fixed_assets() */

void tax_recover_fixed_assets_undo(	char *application_name,
					int max_tax_year )
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

} /* tax_recover_fixed_assets_undo() */

