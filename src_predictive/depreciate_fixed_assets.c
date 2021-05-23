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

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
FILE *depreciate_fixed_asset_undo_html_open(
			void );

LIST *depreciate_undo_fixed_asset_purchase_list(
			char *max_depreciation_date );

boolean depreciate_fixed_asset_undo(
			boolean execute );

boolean depreciate_fixed_assets(
			boolean execute );

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

	if ( undo )
	{
		if ( depreciate_fixed_asset_undo(
			execute ) )
		{
			if ( execute )
				printf( "<h3>Undo complete</h3>\n" );
			else
				printf( "<h3>Undo not executed</h3>\n" );
		}
		else
		{
			printf( "<h3>No depreciation to undo</h3>\n" );
		}
	}
	else
	{
		if ( depreciate_fixed_assets(
			execute ) )
		{
			if ( execute )
				printf( "<h3>Posting complete</h3>\n" );
			else
				printf( "<h3>Posting not executed</h3>\n" );
		}
		else
		{
			printf( "<h3>No fixed assets to depreciate</h3>\n" );
		}
	}

	document_close();

	return 0;
}

LIST *depreciate_fetch_fixed_asset_purchase_list(
			char *asset_folder_name,
			char *depreciate_folder_name )
{
	/* -------------------------------------------- */
	/* Returns fixed_asset_purchase_list with	*/
	/* fixed_asset_purchase->depreciation set.	*/
	/* -------------------------------------------- */
	return equipment_purchase_list_fetch(
		asset_folder_name,
		depreciate_folder_name,
		"finance_accumulated_depreciation < purchase_price"
			/* where */ );
}

LIST *depreciate_undo_equipment_purchase_list(
			char *asset_folder_name,
			char *depreciate_folder_name,
			char *max_depreciation_date )
{
	char where[ 1024 ];

	sprintf( where,
"exists ( select 1						"
"	   from %s						"
"	   where						"
"		%s.asset_name =					"
"			%s.asset_name and			"
"		%s.serial_number =				"
"			%s.serial_number and			"
"		depreciation_date = '%s' )			",
		depreciate_folder_name,
		asset_folder_name,
		depreciate_folder_name,
		asset_folder_name,
		depreciate_folder_name,
		max_depreciation_date );

	/* -------------------------------------------- */
	/* Returns equipment_purchase_list with		*/
	/* equipment_purchase->depreciation_list set.	*/
	/* -------------------------------------------- */
	return equipment_purchase_list_fetch(
			asset_folder_name,
			depreciate_folder_name,
			where );
}

FILE *depreciate_fixed_asset_undo_html_open( void )
{
	char sys_string[ 1024 ];
	char *heading_list_string;
	char *justify_list_string;

	heading_list_string =
		"asset_name,"
		"serial_number,"
		"vendor,"
		"purchase,"
		"depreciation,"
		"depreciation_amount";

	justify_list_string = "left,left,left,left,left,right";

	sprintf( sys_string,
		 "html_table.e '^%s' '%s' '^' '%s'",
		 "UNDO Depreciate Equipment",
		 heading_list_string,
		 justify_list_string );

	return popen( sys_string, "w" );
}

boolean depreciate_fixed_asset_undo(
			char *folder_name,
			boolean execute )
{
	EQUIPMENT_PURCHASE *equipment_purchase;
	LIST *equipment_purchase_list;
	DEPRECIATION *depreciation;
	char *max_depreciation_date;
	FILE *html_output;
	FILE *delete_pipe = {0};

	if ( execute )
	{
		delete_pipe = depreciation_delete_open();
	}

	html_output = depreciate_fixed_asset_undo_html_open();

	equipment_purchase_list =
		depreciate_undo_equipment_purchase_list(
			folder_name,
			( max_depreciation_date =
				depreciation_max_date() ) );

	if ( !list_rewind( equipment_purchase_list ) ) return 0;

	do {
		equipment_purchase =
			list_get(
				equipment_purchase_list );

		if ( ! ( depreciation =
				depreciation_seek(
					equipment_purchase->
						depreciation_list,
					max_depreciation_date ) ) )
		{
			continue;
		}

		if ( !depreciation->depreciation_transaction )
		{
			fprintf(
			stderr,
	"ERROR in %s/%s()/%d: empty depreciation_transaction for %s/%s/%s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 equipment_purchase->asset_name,
			 equipment_purchase->serial_number,
			 equipment_purchase->vendor_entity->full_name );

			exit( 1 );
		}

		fprintf( html_output,
			 "%s^%s^%s/%s^%s^%s^%.2lf\n",
			 equipment_purchase->asset_name,
			 equipment_purchase->serial_number,
			 equipment_purchase->vendor_entity->full_name,
			 equipment_purchase->vendor_entity->street_address,
			 equipment_purchase->service_placement_date,
			 depreciation->depreciation_date,
			 depreciation->depreciation_amount );

		if ( execute )
		{
			/* Performs journal_propagate() */
			/* ---------------------------- */
			transaction_delete(
				depreciation->
					depreciation_transaction->
					full_name,
				depreciation->
					depreciation_transaction->
					street_address,
				depreciation->
					depreciation_transaction->
					transaction_date_time );

			fprintf(delete_pipe,
			 	"%s^%s^%s/%s^%s^%s\n",
			 	equipment_purchase->asset_name,
			 	equipment_purchase->serial_number,
			 	equipment_purchase->
					vendor_entity->
					full_name,
			 	equipment_purchase->
					vendor_entity->
					street_address,
			 	equipment_purchase->service_placement_date,
			 	depreciation->depreciation_date );

		}

	} while ( list_next( equipment_purchase_list ) );

	pclose( html_output );

	if ( delete_pipe ) pclose( delete_pipe );

	return 1;
}

boolean depreciate_fixed_assets(
			boolean execute )
{
	LIST *fixed_asset_purchase_list;

	equipment_purchase_list =
		equipment_purchase_list_depreciate(
			/* -------------------------------------------- */
			/* Returns equipment_purchase_list with		*/
			/* equipment_purchase->depreciation_list set.	*/
			/* -------------------------------------------- */
			depreciate_fetch_equipment_purchase_list(
				asset_folder_name,
				depreciate_folder_name ),
			pipe2string( "now.sh ymd" )
				/* depreciation_date */,
			execute /* set_depreciation_transaction */ );

	equipment_purchase_depreciation_table( equipment_purchase_list );

	if ( !list_length( equipment_purchase_list ) ) return 0;

	if ( execute )
	{
		/* Sets true transaction_date_time */
		/* ------------------------------- */
		transaction_list_insert(
			depreciation_transaction_list(
				equipment_purchase_depreciation_list(
					equipment_purchase_list ) ),
			0 /* not lock_transaciton */ );

		depreciation_list_insert(
			equipment_purchase_depreciation_list(
				equipment_purchase_list ) );
	}
	return 1;
}

