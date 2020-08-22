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
#include "depreciation.h"
#include "equipment_purchase.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
LIST *depreciate_fetch_equipment_purchase_list(
			void );

DEPRECIATION *depreciate_equipment_depreciation(
			EQUIPMENT_PURCHASE *equipment_purchase,
			char *depreciation_date,
			char *prior_depreciation_date,
			int units_produced );

/* -------------------------------------------- */
/* Returns equipment_purchase_list with		*/
/* equipment_purchase->depreciation_list set.	*/
/* -------------------------------------------- */
LIST *depreciate_equipment_purchase_list(
			LIST *equipment_purchase_list,
			char *depreciation_date );

void depreciate_equipment_undo(
			LIST *equipment_purchase_list,
			char *max_undo_date,
			boolean execute );

void depreciate_fixed_assets(
			boolean undo,
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

	depreciate_fixed_assets(
		undo,
		execute );

	document_close();

	return 0;
}

LIST *depreciate_fetch_equipment_purchase_list( void )
{
	return equipment_purchase_list_fetch(
		"finance_accumulated_depreciation < purchase_price"
			/* where */ );
}

DEPRECIATION *depreciate_equipment_depreciation(
			EQUIPMENT_PURCHASE *equipment_purchase,
			char *depreciation_date,
			char *prior_depreciation_date,
			int units_produced )
{
	DEPRECIATION *depreciation;

	depreciation =
		depreciation_new(
			equipment_purchase->asset_name,
			equipment_purchase->serial_number,
			equipment_purchase->vendor_entity->full_name,
			equipment_purchase->vendor_entity->street_address,
			equipment_purchase->purchase_date_time,
			depreciation_date );

	depreciation->depreciation_amount =
		depreciation_amount(
			equipment_purchase->depreciation_method,
			equipment_purchase->equipment_cost,
			equipment_purchase->estimated_residual_value,
			equipment_purchase->estimated_useful_life_years,
			equipment_purchase->estimated_useful_life_units,
			equipment_purchase->declining_balance_n,
			prior_depreciation_date,
			depreciation_date,
			equipment_purchase->finance_accumulated_depreciation,
			equipment_purchase->service_placement_date,
			units_produced );

	if ( timlib_dollar_virtually_same(
		depreciation->depreciation_amount,
		0.0 ) )
	{
		return (DEPRECIATION *)0;
	}

	depreciation->depreciation_accumulated_depreciation =
		depreciation_accumulated_depreciation(
			depreciation->depreciation_accumulated_depreciation
				/* prior_accumulated_depreciation */,
			depreciation->depreciation_amount );

	return depreciation;
}

LIST *depreciate_equipment_purchase_list(
			LIST *equipment_purchase_list,
			char *depreciation_date )
{
	EQUIPMENT_PURCHASE *equipment_purchase;
	char *prior_depreciation_date;

	if ( !list_rewind( equipment_purchase_list ) ) return (LIST *)0;

	do {
		equipment_purchase =
			list_get(
				equipment_purchase_list );

		if ( list_length( equipment_purchase->depreciation_list ) )
		{
			DEPRECIATION *prior_depreciation;

			prior_depreciation =
				list_last(
					equipment_purchase->
						depreciation_list );

			prior_depreciation_date =
				prior_depreciation->depreciation_date;
		}
		else
		{
			prior_depreciation_date = (char *)0;
		}

		list_set(
			equipment_purchase->depreciation_list,
			depreciate_equipment_depreciation(
				equipment_purchase,
				depreciation_date,
				prior_depreciation_date,
				0 /* units_produced */ ) );

	} while ( list_next( equipment_purchase_list ) );

	return equipment_purchase_list;
}

/*
void depreciate_equipment_undo(
			LIST *equipment_purchase_list,
			char *max_undo_date,
			boolean execute )
{
}
*/

void depreciate_fixed_assets(
			boolean undo,
			boolean execute )
{
	LIST *equipment_purchase_list;

	if ( !undo )
	{
		equipment_purchase_list =
			depreciate_equipment_purchase_list(
				depreciate_fetch_equipment_purchase_list(),
				pipe2string( "now.sh ymd" )
					/* depreciation_date */ );

		if ( execute ) {};
		if ( equipment_purchase_list ) {};
	}
	else
	{
#ifdef NOT_DEFINED
		depreciate_equipment_undo(
			depreciate_fetch_equipment_purchase_list(),
			pipe2string( "now.sh ymd" )
				/* max_undo_date */,
			execute );
#endif
	}
}

