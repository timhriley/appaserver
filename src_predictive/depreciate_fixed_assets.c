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
#include "equipment_purchase.h"
#include "depreciation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
LIST *depreciate_undo_equipment_purchase_list(
			char *max_depreciation_date );

boolean depreciate_equipment_undo(
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
	DOCUMENT *document;
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
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );
	
	document_output_body(	document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );

	if ( undo )
	{
		if ( depreciate_equipment_undo( execute ) )
		{
			if ( execute )
				printf( "<h3>Undo complete.</h3>\n" );
			else
				printf( "<h3>Undo not executed.</h3>\n" );
		}
		else
		{
			printf( "<h3>No depreciation to undo.</h3>\n" );
		}
	}
	else
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
			printf( "<h3>No equipment to depreciate.</h3>\n" );
		}
	}

	document_close();

	return 0;
}

LIST *depreciate_fetch_equipment_purchase_list( void )
{
	return equipment_purchase_list_fetch(
		"finance_accumulated_depreciation < purchase_price"
			/* where */ );
}

LIST *depreciate_undo_equipment_purchase_list(
			char *max_depreciation_date )
{
	char where[ 128 ];

	sprintf( where,
		 "depreciation_date = '%s'",
		 max_depreciation_date );

	return equipment_purchase_list_fetch( where );
}

boolean depreciate_equipment_undo( boolean execute )
{
	EQUIPMENT_PURCHASE *equipment_purchase;
	LIST *equipment_purchase_list;

	equipment_purchase_list =
		depreciate_undo_equipment_purchase_list(
			depreciation_max_date() );

	if ( !list_rewind( equipment_purchase_list ) ) return 0;

	do {
		equipment_purchase =
			list_get(
				equipment_purchase_list );

	} while ( list_next( equipment_purchase_list ) );

	return 1;
}

boolean depreciate_fixed_assets( boolean execute )
{
	LIST *equipment_purchase_list;

	equipment_purchase_list =
		/* -------------------------------------------- */
		/* Returns equipment_purchase_list with		*/
		/* equipment_purchase->depreciation_list set.	*/
		/* -------------------------------------------- */
		equipment_purchase_list_depreciate(
			depreciate_fetch_equipment_purchase_list(),
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
					equipment_purchase_list ) ) );

		depreciation_list_insert(
			equipment_purchase_depreciation_list(
				equipment_purchase_list ) );
	}
	return 1;
}

