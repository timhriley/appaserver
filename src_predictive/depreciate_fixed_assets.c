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
char *depreciate_transaction_journal_delete(
			FILE *output_pipe,
			char *max_undo_date,
			char *input_folder_name,
			char *propagate_transaction_date_time );

void depreciate_fixed_assets_undo(
			char *max_undo_date,
			LIST *depreciation_fund_list );

void depreciate_fixed_assets(
			char *process_name,
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

	depreciate_fixed_assets(
		process_name,
		undo,
		execute );

	document_close();

	return 0;

} /* main() */

void depreciate_fixed_assets(
			char *process_name,
			boolean undo,
			boolean execute )
{
	char buffer[ 128 ];
	DEPRECIATION_STRUCTURE *depreciation_structure;
	ENTITY_SELF *entity_self;
	char *depreciation_date_string = {0};

	/* ----- */
	/* Input */
	/* ----- */

	/* Sets depreciation_date */
	/* ---------------------- */
	depreciation_structure =
		depreciation_structure_new(
			application_name );

	if ( !depreciation_structure->depreciation_date )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty depreciation_date.\n",
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

	if ( ! ( entity_self =
			entity_self_load(
				application_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load entity self.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !undo )
	{
		depreciation_date_string = pipe2string( "now.sh ymd" );

		if ( timlib_strcmp( depreciation_structure->
					depreciation_date->
					max_undo_date,
				    depreciation_date_string ) == 0 )
		{
			printf(
"<h3>Error: depreciation date for fixed assets exists for today.</h3>\n" );

			return;
		}
	}

	/* ------- */
	/* Process */
	/* ------- */
	if ( execute )
	{
		printf( "<h1>%s</h1>\n",
			format_initial_capital(
				buffer,
				process_name ) );

		if ( undo )
		{
			depreciate_fixed_assets_undo(
				application_name,
				depreciation_structure->
					depreciation_date->
					max_undo_date,
				depreciation_structure->
					depreciation_fund_list );

			printf(
"<h3>Depreciation of fixed assets posted on %s is now deleted.</h3>\n",
				depreciation_structure->
					depreciation_date->
					max_undo_date );
			return;
		}

		depreciation_fund_list_asset_list_build(
			depreciation_structure->depreciation_fund_list,
			application_name );

		depreciation_fund_list_asset_list_set(
			depreciation_structure->depreciation_fund_list,
			depreciation_date_string,
			depreciation_structure->
				depreciation_date->
				prior_fixed_asset_date,
			depreciation_structure->
				depreciation_date->
				prior_fixed_prior_date,
			depreciation_structure->
				depreciation_date->
				prior_property_date,
			depreciation_structure->
				depreciation_date->
				prior_property_prior_date );

		depreciation_fund_list_depreciation_amount_set(
			depreciation_structure->depreciation_fund_list );

		if ( !depreciation_fund_list_transaction_set(
			depreciation_structure->depreciation_fund_list,
			entity_self->entity->full_name,
			entity_self->entity->street_address ) )
		{
			printf(
			"<h3>Warning: no fixed assets to depreciate.</h3>\n" );
			return;
		}

		/* Sets the true transaction_date_time */
		/* ----------------------------------- */
		depreciation_fund_transaction_insert(
				depreciation_structure->
					depreciation_fund_list,
				application_name );

		if ( !depreciation_fund_asset_depreciation_insert(
			depreciation_structure->depreciation_fund_list,
			entity_self->entity->full_name,
			entity_self->entity->street_address ) )	
		{
			printf( "<h3>Error: check log.</h3>\n" );
		}
		else
		{
			printf(
				"<h3>Depreciation now posted on %s.</h3>\n",
				depreciation_date_string );
		}
	}
	else
	/* --------------- */
	/* Must be display */
	/* --------------- */
	{
		if ( undo )
		{
			printf(
	"<h3>Will delete the depreciation taken place on %s.</h3>\n",
				depreciation_structure->
					depreciation_date->
					max_undo_date );
			return;
		}

		depreciation_fund_list_asset_list_build(
			depreciation_structure->depreciation_fund_list,
			application_name );

		depreciation_fund_list_asset_list_set(
			depreciation_structure->depreciation_fund_list,
			depreciation_date_string,
			depreciation_structure->
				depreciation_date->
				prior_fixed_asset_date,
			depreciation_structure->
				depreciation_date->
				prior_fixed_prior_date,
			depreciation_structure->
				depreciation_date->
				prior_property_date,
			depreciation_structure->
				depreciation_date->
				prior_property_prior_date );

		depreciation_fund_list_table_display(
			process_name,
			depreciation_structure->
				depreciation_fund_list );
	}

} /* depreciate_fixed_assets() */

void depreciate_fixed_assets_undo(	char *application_name,
					char *max_undo_date,
					LIST *depreciation_fund_list )
{
	char sys_string[ 1024 ];
	char *propagate_transaction_date_time = {0};
	DEPRECIATION_FUND *depreciation_fund;
	FILE *output_pipe;

	if ( !max_undo_date || !*max_undo_date )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty max_undo_date.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	output_pipe = popen( "sql.e", "w" );

	/* FIXED_ASSET_PURCHASE */
	/* -------------------- */
	if ( folder_exists_folder(
		application_name,
		"fixed_asset_depreciation" ) )
	{
		propagate_transaction_date_time =
			depreciate_transaction_journal_ledger_delete(
				output_pipe,
				application_name,
				max_undo_date,
				"fixed_asset_depreciation"
					/* input_folder_name */,
				propagate_transaction_date_time );
	}

	/* PROPERTY_PURCHASE */
	/* ------------------ */
	if ( folder_exists_folder(
		application_name,
		"property_depreciation" ) )
	{
		propagate_transaction_date_time =
			depreciate_transaction_journal_ledger_delete(
				output_pipe,
				application_name,
				max_undo_date,
				"property_depreciation"
					/* input_folder_name */,
				propagate_transaction_date_time );
	}

	/* PRIOR_FIXED_ASSET */
	/* ------------------ */
	if ( folder_exists_folder(
		application_name,
		"prior_fixed_asset_depreciation" ) )
	{
		propagate_transaction_date_time =
			depreciate_transaction_journal_ledger_delete(
				output_pipe,
				application_name,
				max_undo_date,
				"prior_fixed_asset_depreciation"
					/* input_folder_name */,
				propagate_transaction_date_time );
	}

	/* PRIOR_PROPERTY */
	/* -------------- */
	if ( folder_exists_folder(
		application_name,
		"prior_property_depreciation" ) )
	{
		propagate_transaction_date_time =
			depreciate_transaction_journal_ledger_delete(
				output_pipe,
				application_name,
				max_undo_date,
				"prior_property_depreciation"
					/* input_folder_name */,
				propagate_transaction_date_time );
	}

	pclose( output_pipe );

	if ( propagate_transaction_date_time
	&&   list_rewind( depreciation_fund_list ) )
	{
		do {
			depreciation_fund =
				list_get_pointer(
					depreciation_fund_list );

			sprintf(sys_string,
		 		"ledger_propagate %s \"%s\" '' \"%s\" \"%s\"",
		 		application_name,
		 		propagate_transaction_date_time,
		 		depreciation_fund->
					depreciation_expense_account,
		 		depreciation_fund->
					accumulated_depreciation_account );

			system( sys_string );

		} while( list_next( depreciation_fund_list ) );

		sprintf(sys_string,
		 	"finance_accumulated_depreciation_reset.sh %s",
		 	application_name );

		system( sys_string );
	}

} /* depreciate_fixed_assets_undo() */

/* Returns propagate_transaction_date_time */
/* --------------------------------------- */
char *depreciate_transaction_journal_ledger_delete(
					FILE *output_pipe,
					char *application_name,
					char *max_undo_date,
					char *input_folder_name,
					char *propagate_transaction_date_time )
{
	char transaction_date_time[ 64 ];
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *select;
	FILE *input_pipe;

	if ( !max_undo_date || !*max_undo_date )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty max_undo_date.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	select = "transaction_date_time";

	sprintf(where,
		"depreciation_date = '%s'",
		max_undo_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=\"%s\"			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			|"
		 "sort -u						 ",
		 application_name,
		 select,
		 input_folder_name,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( transaction_date_time, input_pipe ) )
	{
		sprintf(where,
			"transaction_date_time = '%s'",
			transaction_date_time );

		fprintf( output_pipe,
		 	 "delete from journal_ledger where %s;\n",
		 	 where );

		fprintf( output_pipe,
		 	 "delete from transaction where %s;\n",
		 	 where );

		if ( !propagate_transaction_date_time )
		{
			propagate_transaction_date_time =
				strdup( transaction_date_time );
		}
		else
		{
			if ( transaction_date_time <
			     propagate_transaction_date_time )
			{
				propagate_transaction_date_time =
					strdup( transaction_date_time );
			}
		}
	}

	pclose( input_pipe );

	sprintf(where,
		"depreciation_date = '%s'",
		max_undo_date );

	fprintf( output_pipe,
	 	 "delete from %s where %s;\n",
		 input_folder_name,
	 	 where );

	return propagate_transaction_date_time;

} /* depreciate_transaction_journal_ledger_delete() */

