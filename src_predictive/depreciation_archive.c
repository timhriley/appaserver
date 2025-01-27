/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.c			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver depreciation ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "appaserver_library.h"
#include "ledger.h"
#include "entity.h"
#include "folder.h"
#include "purchase.h"
#include "column.h"
#include "fixed_asset.h"
#include "depreciation.h"

DEPRECIATION_TRANSACTION *depreciation_transaction_new( void )
{
	DEPRECIATION_TRANSACTION *p =
		calloc( 1, sizeof( DEPRECIATION_TRANSACTION ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;
}

DEPRECIATION_AMOUNT *depreciation_amount_new( void )
{
	DEPRECIATION_AMOUNT *p =
		calloc( 1, sizeof( DEPRECIATION_AMOUNT ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* depreciation_amount_new() */

/* Sets depreciation_date and depreciation_tax_year (if not nonprofit) */
/* ------------------------------------------------------------------- */
DEPRECIATION_STRUCTURE *depreciation_structure_new(
				char *application_name )
{
	DEPRECIATION_STRUCTURE *p =
		calloc( 1, sizeof( DEPRECIATION_STRUCTURE ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	p->depreciation_date =
		depreciation_date_new(
			application_name );

	if ( ledger_tax_recovery_period_attribute_exists(
		application_name ) )
	{
		p->depreciation_tax_year =
			depreciation_tax_year_new(
				application_name );
	}

	return p;

} /* depreciation_structure_new() */

DEPRECIATION_ASSET_LIST *depreciation_asset_list_new(
					char *application_name,
					char *fund_name )
{
	DEPRECIATION_ASSET_LIST *d =
		calloc( 1, sizeof( DEPRECIATION_ASSET_LIST ) );

	if ( !d )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	if ( folder_exists_folder(
		application_name,
		"fixed_asset_purchase" ) )
	{
		d->purchase_fixed_asset_list =
			fixed_asset_depreciation_purchase_fetch_list(
				application_name,
				fund_name );
	}

	if ( folder_exists_folder(
		application_name,
		"prior_fixed_asset" ) )
	{
		d->prior_fixed_asset_list =
			fixed_asset_depreciation_prior_fetch_list(
				application_name,
				fund_name );
	}

	if ( folder_exists_folder(
		application_name,
		"property_purchase" ) )
	{
		d->purchase_property_list =
			fixed_property_depreciation_purchase_fetch_list(
				application_name,
				fund_name );
	}

	if ( folder_exists_folder(
		application_name,
		"prior_property" ) )
	{
		d->prior_property_list =
			fixed_property_depreciation_prior_fetch_list(
				application_name,
				fund_name );
	}

	return d;

} /* depreciation_asset_list_new() */

DEPRECIATION_TAX_YEAR *depreciation_tax_year_new(
				char *application_name )
{
	DEPRECIATION_TAX_YEAR *p;

	if ( ! ( p = calloc( 1, sizeof( DEPRECIATION_TAX_YEAR ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	/* FIXED_ASSET_PURCHASE */
	/* -------------------- */
	if ( folder_exists_folder(
		application_name,
		"tax_fixed_asset_recovery" ) )
	{
		p->undo_fixed_asset_year =
			depreciation_fetch_max_tax_year(
				application_name,
				"tax_fixed_asset_recovery" );

		p->max_undo_year = p->undo_fixed_asset_year;
	}

	/* PRIOR_FIXED_ASSET */
	/* ------------------ */
	if ( folder_exists_folder(
		application_name,
		"tax_prior_fixed_asset_recovery" ) )
	{
		p->undo_fixed_prior_year =
			depreciation_fetch_max_tax_year(
				application_name,
				"tax_prior_fixed_asset_recovery" );

		if ( !p->max_undo_year
		||   p->undo_fixed_prior_year > p->max_undo_year )
		{
			p->max_undo_year = p->undo_fixed_prior_year;
		}
	}

	/* PROPERTY_PURCHASE */
	/* ------------------ */
	if ( folder_exists_folder(
		application_name,
		"tax_property_recovery" ) )
	{
		p->undo_property_year =
			depreciation_fetch_max_tax_year(
				application_name,
				"tax_property_recovery" );

		if ( !p->max_undo_year
		||   p->undo_property_year > p->max_undo_year )
		{
			p->max_undo_year = p->undo_property_year;
		}
	}

	/* PRIOR_PROPERTY */
	/* -------------- */
	if ( folder_exists_folder(
		application_name,
		"tax_prior_property_recovery" ) )
	{
		p->undo_property_prior_year =
			depreciation_fetch_max_tax_year(
				application_name,
				"tax_prior_property_recovery" );

		if ( !p->max_undo_year
		||   p->undo_property_prior_year > p->max_undo_year )
		{
			p->max_undo_year = p->undo_property_prior_year;
		}
	}

	return p;

} /* depreciation_tax_year_new() */

DEPRECIATION_DATE *depreciation_date_new( char *application_name )
{
	DEPRECIATION_DATE *p;

	if ( ! ( p = calloc( 1, sizeof( DEPRECIATION_DATE ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	/* FIXED_ASSET_PURCHASE */
	/* -------------------- */
	if ( folder_exists_folder(
		application_name,
		"fixed_asset_purchase" ) )
	{
		p->undo_fixed_asset_date =
		p->prior_fixed_asset_date =
			depreciation_fetch_max_depreciation_date(
				application_name,
				FIXED_ASSET_DEPRECIATION_FOLDER );

		p->max_undo_date = p->undo_fixed_asset_date;
	}

	/* PRIOR_FIXED_ASSET */
	/* ------------------ */
	if ( folder_exists_folder(
		application_name,
		"prior_fixed_asset" ) )
	{
		p->undo_fixed_prior_date =
		p->prior_fixed_prior_date =
			depreciation_fetch_max_depreciation_date(
				application_name,
				PRIOR_FIXED_ASSET_DEPRECIATION_FOLDER );

		if ( !p->max_undo_date
		||   timlib_strcmp(
			p->undo_fixed_prior_date,
			p->max_undo_date ) > 0 )
		{
			p->max_undo_date = p->undo_fixed_prior_date;
		}
	}

	/* PROPERTY_PURCHASE */
	/* ------------------ */
	if ( folder_exists_folder(
		application_name,
		"property_purchase" ) )
	{
		p->undo_property_date =
		p->prior_property_date =
			depreciation_fetch_max_depreciation_date(
				application_name,
				PROPERTY_DEPRECIATION_FOLDER );

		if ( !p->max_undo_date
		||   timlib_strcmp(
			p->undo_property_date,
			p->max_undo_date ) > 0 )
		{
			p->max_undo_date = p->undo_property_date;
		}
	}

	/* PRIOR_PROPERTY */
	/* -------------- */
	if ( folder_exists_folder(
		application_name,
		"prior_property" ) )
	{
		p->undo_property_prior_date =
		p->prior_property_prior_date =
			depreciation_fetch_max_depreciation_date(
				application_name,
				PRIOR_PROPERTY_DEPRECIATION_FOLDER );

		if ( !p->max_undo_date
		||   timlib_strcmp(
			p->undo_property_prior_date,
			p->max_undo_date ) > 0 )
		{
			p->max_undo_date = p->undo_property_prior_date;
		}
	}

	return p;

} /* depreciation_date_new() */

DEPRECIATION *depreciation_calloc( void )
{
	DEPRECIATION *p = calloc( 1, sizeof( DEPRECIATION ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* depreciation_calloc() */

DEPRECIATION *depreciation_new( char *depreciation_date_string )
{
	DEPRECIATION *d = depreciation_calloc();

	d->depreciation_date_string = depreciation_date_string;

	return d;

} /* depreciation_new() */

char *depreciation_fetch_max_depreciation_date(
			char *application_name,
			char *folder_name )
{
	char sys_string[ 1024 ];
	char *select;

	select = "max(depreciation_date)";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			",
		 application_name,
		 select,
		 folder_name );

	return pipe2string( sys_string );

} /* depreciation_fetch_max_depreciation_date() */

double depreciation_fixed_asset_calculate_amount(
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			char *service_placement_date,
			int units_produced )
{
	if ( !depreciation_method || !*depreciation_method )
	{
		/* Land is not depreciated. */
		/* ------------------------ */
		return 0.0;
	}
	else
	if ( strcmp( depreciation_method, "straight_line" ) == 0 )
	{
		return depreciation_straight_line_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			finance_accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "units_of_production" ) == 0 )
	{
		return depreciation_units_of_production_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_units,
			units_produced,
			finance_accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "double_declining_balance" ) == 0 )
	{
		return depreciation_n_declining_balance_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			finance_accumulated_depreciation,
			2 /* n */ );
	}
	else
	if ( strcmp( depreciation_method, "n_declining_balance" ) == 0 )
	{
		return depreciation_n_declining_balance_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			finance_accumulated_depreciation,
			declining_balance_n );
	}
	else
	if ( strcmp( depreciation_method, "sum_of_years_digits" ) == 0 )
	{
		return depreciation_sum_of_years_digits_get_amount(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			finance_accumulated_depreciation,
			service_placement_date );
	}
	else
	{
		return 0.0;
	}

} /* depreciation_fixed_asset_calculate_amount() */

double depreciation_units_of_production_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double finance_accumulated_depreciation )
{
	double depreciation_rate_per_unit = 0.0;
	double depreciation_base;
	double depreciation_amount;

	depreciation_base = extension - (double)estimated_residual_value;

	if ( estimated_useful_life_units )
	{
		depreciation_rate_per_unit =
			depreciation_base /
			(double)estimated_useful_life_units;
	}

	depreciation_amount = 
		depreciation_rate_per_unit *
		(double)units_produced;


	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
	}

	return depreciation_amount;

} /* depreciation_units_of_production_get_amount() */

double depreciation_straight_line_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation )
{
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year = 0.0;
	double depreciation_amount;

	if ( prior_depreciation_date_string
	&&   *prior_depreciation_date_string )
	{
		fraction_of_year =
			ledger_get_fraction_of_year(
				prior_depreciation_date_string,
				depreciation_date_string );
	}

	depreciation_base = extension - (double)estimated_residual_value;

	if ( estimated_useful_life_years )
	{
		annual_depreciation_amount =
			depreciation_base /
			(double) estimated_useful_life_years;
	}
	else
	{
		annual_depreciation_amount = 0.0;
	}

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	/* If depreciation_date is past the useful life. */
	/* --------------------------------------------- */
	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
	}

	return depreciation_amount;

} /* depreciation_straight_line_get_amount() */

double depreciation_sum_of_years_digits_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			char *service_placement_date )
{
	double denominator;
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_fraction;
	double depreciation_amount;
	int current_age_years;
	int remaining_life_years;

	if ( estimated_useful_life_years <= 0 ) return 0.0;

	depreciation_base = extension - (double)estimated_residual_value;

	denominator =
		( (double)( estimated_useful_life_years *
			  ( estimated_useful_life_years + 1 ) ) )
			  / 2.0;

	current_age_years =
		date_years_between(
			depreciation_date_string,
			service_placement_date );

	remaining_life_years = estimated_useful_life_years - current_age_years;

	if ( remaining_life_years < 0 ) remaining_life_years = 0;

	depreciation_fraction =
		(double)remaining_life_years /
		(double)denominator;

	annual_depreciation_amount =
		depreciation_base *
		depreciation_fraction;

	fraction_of_year =
		ledger_get_fraction_of_year(
			prior_depreciation_date_string,
			depreciation_date_string );

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	/* If depreciation_date is past the useful life. */
	/* --------------------------------------------- */
	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
	}

	return depreciation_amount;

} /* depreciation_sum_of_years_digits_get_amount() */

/* Returns propagate_account_list */
/* ------------------------------ */
LIST *depreciation_journal_ledger_refresh(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double depreciation_amount )
{
	char *depreciation_expense_account = {0};
	char *accumulated_depreciation_account = {0};
	LIST *propagate_account_list = {0};
	ACCOUNT *account;
	JOURNAL_LEDGER *prior_ledger;

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&depreciation_expense_account,
		&accumulated_depreciation_account,
		application_name,
		fund_name );

	if ( depreciation_amount )
	{
		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			depreciation_expense_account,
			depreciation_amount,
			1 /* is_debit */ );

		ledger_journal_ledger_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			accumulated_depreciation_account,
			depreciation_amount,
			0 /* not is_debit */ );
	}

	propagate_account_list = list_new();

	account = ledger_account_new( depreciation_expense_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				depreciation_expense_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			depreciation_expense_account );

	list_append_pointer( propagate_account_list, account );

	account = ledger_account_new( accumulated_depreciation_account );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				accumulated_depreciation_account );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			accumulated_depreciation_account );

	list_append_pointer( propagate_account_list, account );

	return propagate_account_list;

} /* depreciation_journal_ledger_refresh() */

double depreciation_n_declining_balance_get_amount(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			int n )
{
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_amount;
	double book_value;
	double maximum_depreciation;

	book_value = extension - finance_accumulated_depreciation;

	fraction_of_year =
		ledger_get_fraction_of_year(
			prior_depreciation_date_string,
			depreciation_date_string );

	annual_depreciation_amount =
		( book_value * (double)n ) /
			       (double)estimated_useful_life_years;

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	maximum_depreciation = book_value - (double)estimated_residual_value;

	if ( depreciation_amount > maximum_depreciation )
		depreciation_amount = maximum_depreciation;

	return depreciation_amount;

} /* depreciation_n_declining_balance_get_amount() */

DEPRECIATION_TRANSACTION *depreciation_fund_get_transaction(
				char *full_name,
				char *street_address,
				char *depreciation_expense_account,
				char *accumulated_depreciation_account,
				double purchase_fixed_asset_depreciation_amount,
				double prior_fixed_asset_depreciation_amount,
				double purchase_property_depreciation_amount,
				double prior_property_depreciation_amount )
{
	DEPRECIATION_TRANSACTION *t;
	static DATE *transaction_date_time = {0};

	t = depreciation_transaction_new();

	if ( !transaction_date_time )
	{
		transaction_date_time =
			date_now_new( date_get_utc_offset() );
	}

	/* FIXED_ASSET_PURCHASE */
	/* -------------------- */
	if ( purchase_fixed_asset_depreciation_amount )
	{
		t->purchase_fixed_asset_transaction =
			ledger_transaction_new(
				full_name,
				street_address,
				date_display_yyyy_mm_dd_colon_hms(
					transaction_date_time ),
				DEPRECIATION_MEMO );

		t->purchase_fixed_asset_transaction->transaction_amount =
			purchase_fixed_asset_depreciation_amount;

		t->purchase_fixed_asset_transaction->journal_ledger_list =
			ledger_get_binary_ledger_list(
				t->purchase_fixed_asset_transaction->
					transaction_amount,
				depreciation_expense_account
					/* debit_account */,
				accumulated_depreciation_account
					/* credit_account */ );

		date_increment_seconds(
			transaction_date_time,
			1 );
	}

	/* PRIOR_FIXED_ASSET */
	/* ----------------- */
	if ( prior_fixed_asset_depreciation_amount )
	{
		t->prior_fixed_asset_transaction =
			ledger_transaction_new(
				full_name,
				street_address,
				date_display_yyyy_mm_dd_colon_hms(
					transaction_date_time ),
				DEPRECIATION_MEMO );

		t->prior_fixed_asset_transaction->transaction_amount =
			prior_fixed_asset_depreciation_amount;

		t->prior_fixed_asset_transaction->journal_ledger_list =
			ledger_get_binary_ledger_list(
				t->prior_fixed_asset_transaction->
					transaction_amount,
				depreciation_expense_account
					/* debit_account */,
				accumulated_depreciation_account
					/* credit_account */ );

		date_increment_seconds(
			transaction_date_time,
			1 );
	}

	/* PROPERTY_PURCHASE */
	/* ----------------- */
	if ( purchase_property_depreciation_amount )
	{
		t->purchase_property_transaction =
			ledger_transaction_new(
				full_name,
				street_address,
				date_display_yyyy_mm_dd_colon_hms(
					transaction_date_time ),
				DEPRECIATION_MEMO );

		t->purchase_property_transaction->transaction_amount =
			purchase_property_depreciation_amount;

		t->purchase_property_transaction->journal_ledger_list =
			ledger_get_binary_ledger_list(
				t->purchase_property_transaction->
					transaction_amount,
				depreciation_expense_account
					/* debit_account */,
				accumulated_depreciation_account
					/* credit_account */ );

		date_increment_seconds(
			transaction_date_time,
			1 );
	}

	/* PRIOR_PROPERTY */
	/* -------------- */
	if ( prior_property_depreciation_amount )
	{
		t->prior_property_transaction =
			ledger_transaction_new(
				full_name,
				street_address,
				date_display_yyyy_mm_dd_colon_hms(
					transaction_date_time ),
				DEPRECIATION_MEMO );

		t->prior_property_transaction->transaction_amount =
			prior_property_depreciation_amount;

		t->purchase_fixed_asset_transaction->journal_ledger_list =
			ledger_get_binary_ledger_list(
				t->prior_property_transaction->
					transaction_amount,
				depreciation_expense_account
					/* debit_account */,
				accumulated_depreciation_account
					/* credit_account */ );

		date_increment_seconds(
			transaction_date_time,
			1 );
	}

	return t;

} /* depreciation_fund_get_transaction() */

void depreciation_fund_list_table_display(
				char *process_name,
				LIST *depreciation_fund_list )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;
	LIST *fixed_asset_list;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;
	char *justification;
	char buffer[ 128 ];

	if ( !list_rewind( depreciation_fund_list ) ) return;

	heading =
"Asset,Serial,Extension,Prior Accumulated,Depreciation,Post Accumulated";

	justification = "left,left,right";

	sprintf( sys_string,
		 "html_table.e '%s' '%s' '^' '%s'",
		 format_initial_capital( buffer, process_name ),
		 heading,
		 justification );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_asset_list =
			depreciation_fund->
				depreciation_asset_list;

		if ( !depreciation_asset_list ) continue;

		fixed_asset_list =
			depreciation_asset_list->
				purchase_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			depreciation_fixed_asset_list_table_display(
				output_pipe,
				fixed_asset_list );
		}

		fixed_asset_list =
			depreciation_asset_list->
				prior_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			depreciation_fixed_asset_list_table_display(
				output_pipe,
				fixed_asset_list );
		}

		fixed_asset_list =
			depreciation_asset_list->
				purchase_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			depreciation_fixed_asset_list_table_display(
				output_pipe,
				fixed_asset_list );
		}

		fixed_asset_list =
			depreciation_asset_list->
				prior_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			depreciation_fixed_asset_list_table_display(
				output_pipe,
				fixed_asset_list );
		}

	} while( list_next( depreciation_fund_list ) );

	pclose( output_pipe );

} /* depreciation_fund_list_table_display() */

void depreciation_fixed_asset_list_table_display(
				FILE *output_pipe,
				LIST *fixed_asset_list )
{
	FIXED_ASSET *fixed_asset;
	char buffer[ 128 ];

	if ( !list_rewind( fixed_asset_list ) ) return;

	do {
		fixed_asset = list_get_pointer( fixed_asset_list );

		fprintf(output_pipe,
			"%s^%s^%.2lf^%.2lf",
			format_initial_capital(
				buffer, fixed_asset->asset_name ),
			fixed_asset->serial_number,
			fixed_asset->extension,
			fixed_asset->
				database_finance_accumulated_depreciation );

		if ( !fixed_asset->depreciation )
		{
			fprintf( output_pipe, "^Error" );
		}
		else
		{
			fprintf(output_pipe,
				"^%.2lf",
				fixed_asset->
					depreciation->
					depreciation_amount );
		}

		fprintf(output_pipe,
			"^%.2lf\n",
			fixed_asset->
				finance_accumulated_depreciation );

	} while( list_next( fixed_asset_list ) );

} /* depreciation_fixed_asset_list_table_display() */

/* ------------------------------------- */
/* Sets depreciation_expense_account and */
/*      accumulated_depreciation_account */
/* ------------------------------------- */
LIST *depreciation_fetch_fund_list(
				char *application_name )
{
	DEPRECIATION_FUND *depreciation_fund;
	LIST *depreciation_fund_list = list_new();

	if ( ledger_fund_attribute_exists( application_name ) )
	{
		LIST *fund_name_list;
		char *fund_name;

		fund_name_list =
			ledger_fetch_fund_name_list(
				application_name );

		if ( !list_rewind( fund_name_list ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty fund_name_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		do {
			fund_name = list_get_pointer( fund_name_list );

			depreciation_fund =
				depreciation_fund_new(
					application_name,
					fund_name );

			list_append_pointer(
				depreciation_fund_list,
				depreciation_fund );

		} while( list_next( fund_name_list ) );
	}
	else
	{
		depreciation_fund =
			depreciation_fund_new(
				application_name,
				(char *)0 /* fund_name */ );

		list_append_pointer(
			depreciation_fund_list,
			depreciation_fund );
	}

	return depreciation_fund_list;

} /* depreciation_fetch_fund_list() */

DEPRECIATION_FUND *depreciation_fund_new(
					char *application_name,
					char *fund_name )
{
	DEPRECIATION_FUND *d = calloc( 1, sizeof( DEPRECIATION_FUND ) );

	if ( !d )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	d->fund_name = fund_name;

	/* Error with an exit if failure. */
	/* ------------------------------ */
	ledger_get_depreciation_account_names(
		&d->depreciation_expense_account,
		&d->accumulated_depreciation_account,
		application_name,
		fund_name );

	return d;

} /* depreciation_fund_new() */

void depreciation_fund_list_asset_list_build(
			LIST *depreciation_fund_list,
			char *application_name )
{
	DEPRECIATION_FUND *depreciation_fund;

	if ( !list_rewind( depreciation_fund_list ) ) return;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_fund->depreciation_asset_list =
			depreciation_asset_list_new(
				application_name,
				depreciation_fund->fund_name );

	} while( list_next( depreciation_fund_list ) );

} /* depreciation_fund_list_asset_list_build() */

boolean depreciation_fund_list_transaction_set(
				LIST *depreciation_fund_list,
				char *full_name,
				char *street_address )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_AMOUNT *depreciation_amount;
	boolean did_any = 0;

	if ( !list_rewind( depreciation_fund_list ) ) return 0;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_amount =
			depreciation_fund->
				depreciation_amount;

		if ( !depreciation_amount ) continue;

		depreciation_fund->depreciation_transaction =
			depreciation_fund_get_transaction(
				full_name,
				street_address,
				depreciation_fund->
					depreciation_expense_account,
				depreciation_fund->
					accumulated_depreciation_account,
				depreciation_amount->
				     purchase_fixed_asset_depreciation_amount,
				depreciation_amount->
				     prior_fixed_asset_depreciation_amount,
				depreciation_amount->
				     purchase_property_depreciation_amount,
				depreciation_amount->
				     prior_property_depreciation_amount );

		if ( depreciation_fund->depreciation_transaction )
		{
			did_any = 1;
		}

	} while( list_next( depreciation_fund_list ) );

	return did_any;

} /* depreciation_fund_list_transaction_set() */

/* Sets the true depreciation_transaction->transaction_date_time */
/* ------------------------------------------------------------- */
void depreciation_fund_transaction_insert(
				LIST *depreciation_fund_list,
				char *application_name )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_TRANSACTION *depreciation_transaction;
	TRANSACTION *transaction;

	if ( !list_rewind( depreciation_fund_list ) ) return;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_transaction =
			depreciation_fund->
				depreciation_transaction;

		if ( !depreciation_transaction ) continue;

		/* FIXED_ASSET_PURCHASE */
		/* -------------------- */
		transaction =
			depreciation_transaction->
				purchase_fixed_asset_transaction;

		if ( transaction )
		{
			transaction->
			transaction_date_time =
			     ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->
					transaction_date_time,
				transaction->
					transaction_amount,
				transaction->
					memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				transaction->
					journal_ledger_list );
		}

		/* PROPERTY_PURCHASE */
		/* ----------------- */
		transaction =
			depreciation_transaction->
				purchase_property_transaction;

		if ( transaction )
		{
			transaction->
			transaction_date_time =
			     ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->
					transaction_date_time,
				transaction->
					transaction_amount,
				transaction->
					memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				transaction->
					journal_ledger_list );
		}

		/* PRIOR_FIXED_ASSET */
		/* ----------------- */
		transaction =
			depreciation_transaction->
				prior_fixed_asset_transaction;

		if ( transaction )
		{
			transaction->
			transaction_date_time =
			     ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->
					transaction_date_time,
				transaction->
					transaction_amount,
				transaction->
					memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				transaction->
					journal_ledger_list );
		}

		/* PRIOR_PROPERTY */
		/* -------------- */
		transaction =
			depreciation_transaction->
				prior_property_transaction;

		if ( transaction )
		{
			transaction->
			transaction_date_time =
			     ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->
					transaction_date_time,
				transaction->
					transaction_amount,
				transaction->
					memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				transaction->
					journal_ledger_list );
		}

	} while( list_next( depreciation_fund_list ) );

} /* depreciation_fund_transaction_insert() */

boolean depreciation_fund_asset_depreciation_insert(
				LIST *depreciation_fund_list,
				char *full_name,
				char *street_address )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;
	DEPRECIATION_TRANSACTION *depreciation_transaction;
	TRANSACTION *transaction;
	LIST *fixed_asset_list;
	boolean did_any = 0;

	if ( !list_rewind( depreciation_fund_list ) ) return 0;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_transaction =
			depreciation_fund->
				depreciation_transaction;

		depreciation_asset_list =
			depreciation_fund->
				depreciation_asset_list;

		if ( depreciation_asset_list
		&&   !depreciation_transaction )
		{
			fprintf( stderr,
				 "Warning in %s/%s/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			continue;
		}

		if ( depreciation_transaction
		&&   !depreciation_asset_list )
		{
			fprintf( stderr,
				 "Warning in %s/%s/%d: empty asset list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			continue;
		}

		/* FIXED_ASSET_PURCHASE */
		/* -------------------- */
		transaction =
			depreciation_transaction->
				purchase_fixed_asset_transaction;

		if ( transaction )
		{
			fixed_asset_list =
				depreciation_asset_list->
					purchase_fixed_asset_list;

			if ( !list_length( fixed_asset_list ) )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: empty fixed_asset_list.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			depreciation_asset_list_depreciation_insert(
				fixed_asset_list,
				"fixed_asset_depreciation",
				full_name,
				street_address,
				transaction->transaction_date_time );

			did_any = 1;
		}

		/* PROPERTY_PURCHASE */
		/* ----------------- */
		transaction =
			depreciation_transaction->
				purchase_property_transaction;

		if ( transaction )
		{
			fixed_asset_list =
				depreciation_asset_list->
					purchase_property_list;

			if ( !list_length( fixed_asset_list ) )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: empty fixed_asset_list.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			depreciation_asset_list_depreciation_insert(
				fixed_asset_list,
				"property_depreciation",
				full_name,
				street_address,
				transaction->transaction_date_time );

			did_any = 1;
		}

		/* PRIOR_FIXED_ASSET */
		/* ----------------- */
		transaction =
			depreciation_transaction->
				prior_fixed_asset_transaction;

		if ( transaction )
		{
			fixed_asset_list =
				depreciation_asset_list->
					prior_fixed_asset_list;

			if ( !list_length( fixed_asset_list ) )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: empty fixed_asset_list.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			depreciation_asset_list_depreciation_insert(
				fixed_asset_list,
				"prior_fixed_asset_depreciation",
				full_name,
				street_address,
				transaction->transaction_date_time );

			did_any = 1;
		}


		/* PRIOR_PROPERTY */
		/* -------------- */
		transaction =
			depreciation_transaction->
				prior_property_transaction;

		if ( transaction )
		{
			fixed_asset_list =
				depreciation_asset_list->
					prior_property_list;

			if ( !list_length( fixed_asset_list ) )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: empty fixed_asset_list.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			depreciation_asset_list_depreciation_insert(
				fixed_asset_list,
				"prior_property_depreciation",
				full_name,
				street_address,
				transaction->transaction_date_time );

			did_any = 1;
		}

	} while( list_next( depreciation_fund_list ) );

	return did_any;

} /* depreciation_fund_asset_depreciation_insert() */

void depreciation_asset_list_depreciation_insert(
				LIST *fixed_asset_list,
				char *folder_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	FIXED_ASSET *fixed_asset;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *field;

	if ( !list_rewind( fixed_asset_list ) ) return;

	field =
"asset_name,serial_number,full_name,street_address,transaction_date_time,depreciation_date,depreciation_amount";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e						 ",
		 folder_name,
		 field );
		 
	output_pipe = popen( sys_string, "w" );

	do {
		fixed_asset = list_get_pointer( fixed_asset_list );

		if ( fixed_asset->depreciation )
		{
			fprintf(output_pipe,
				"%s^%s^%s^%s^%s^%s^%.2lf\n",
				fixed_asset->asset_name,
				fixed_asset->serial_number,
				full_name,
				street_address,
				transaction_date_time,
				fixed_asset->
					depreciation->
					depreciation_date_string,
				fixed_asset->
					depreciation->
					depreciation_amount );
		}

	} while( list_next( fixed_asset_list ) );

	pclose( output_pipe );

} /* depreciation_asset_list_depreciation_insert() */

int depreciation_fetch_max_tax_year(
				char *application_name,
				char *folder_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *results;

	select = "max(tax_year)";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			",
		 application_name,
		 select,
		 folder_name );

	results = pipe2string( sys_string );

	if ( results && *results )
		return atoi( results );
	else
		return 0;

} /* depreciation_fetch_max_tax_year() */

void depreciation_fund_list_depreciation_amount_set(
			LIST *depreciation_fund_list )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;

	if ( !list_rewind( depreciation_fund_list ) ) return;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_asset_list =
			depreciation_fund->
				depreciation_asset_list;

		if ( !depreciation_asset_list )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty depreciation_asset_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		depreciation_fund->depreciation_amount =
			depreciation_asset_list_depreciation_amount_set(
				depreciation_asset_list->
					purchase_fixed_asset_list,
				depreciation_asset_list->
					prior_fixed_asset_list,
				depreciation_asset_list->
					purchase_property_list,
				depreciation_asset_list->
					prior_property_list );

	} while( list_next( depreciation_fund_list ) );

} /* depreciation_fund_list_depreciation_amount_set() */

DEPRECIATION_AMOUNT *depreciation_asset_list_depreciation_amount_set(
				LIST *purchase_fixed_asset_list,
				LIST *prior_fixed_asset_list,
				LIST *purchase_property_list,
				LIST *prior_property_list )
{
	DEPRECIATION_AMOUNT *d;
	double check_sum;

	d = depreciation_amount_new();

	if ( list_length( purchase_fixed_asset_list ) )
	{
		d->purchase_fixed_asset_depreciation_amount =
			depreciation_asset_list_accumulate_depreciation_amount(
				purchase_fixed_asset_list );
	}

	if ( list_length( prior_fixed_asset_list ) )
	{
		d->prior_fixed_asset_depreciation_amount =
			depreciation_asset_list_accumulate_depreciation_amount(
				prior_fixed_asset_list );
	}

	if ( list_length( purchase_property_list ) )
	{
		d->purchase_property_depreciation_amount =
			depreciation_asset_list_accumulate_depreciation_amount(
				purchase_property_list );
	}

	if ( list_length( prior_property_list ) )
	{
		d->prior_property_depreciation_amount =
			depreciation_asset_list_accumulate_depreciation_amount(
				prior_property_list );
	}

	check_sum =
		d->purchase_fixed_asset_depreciation_amount +
		d->prior_fixed_asset_depreciation_amount +
		d->purchase_property_depreciation_amount +
		d->prior_property_depreciation_amount;

	if ( !check_sum )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: did not calculate any depreciation amount.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
	}

	return d;

} /* depreciation_asset_list_depreciation_amount_set() */

double depreciation_asset_list_accumulate_depreciation_amount(
				LIST *fixed_asset_list )
{
	FIXED_ASSET *fixed_asset;
	double amount;

	if ( !list_rewind( fixed_asset_list ) ) return 0.0;

	amount = 0.0;

	do {
		fixed_asset = list_get_pointer( fixed_asset_list );

		if ( !fixed_asset->depreciation )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty depreciation.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		amount += fixed_asset->depreciation->depreciation_amount;

	} while( list_next( fixed_asset_list ) );

	return amount;

} /* depreciation_asset_list_accumulate_depreciation_amount() */

void depreciation_fund_list_asset_list_set(
			LIST *depreciation_fund_list,
			char *depreciation_date_string,
			char *prior_fixed_asset_date,
			char *prior_fixed_prior_date,
			char *prior_property_date,
			char *prior_property_prior_date )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;
	LIST *fixed_asset_list;

	if ( !depreciation_date_string
	||   !*depreciation_date_string )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty depreciation_date_string.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( depreciation_fund_list ) ) return;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_asset_list =
			depreciation_fund->
				depreciation_asset_list;

		if ( !depreciation_asset_list )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty depreciation_asset_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		/* FIXED_ASSET_PURCHASE */
		/* -------------------- */
		fixed_asset_list =
			depreciation_asset_list->
				purchase_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			depreciation_fixed_asset_list_set(
				fixed_asset_list,
				depreciation_date_string,
				prior_fixed_asset_date );
		}

		/* PRIOR_FIXED_ASSET */
		/* ----------------- */
		fixed_asset_list =
			depreciation_asset_list->
				prior_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			depreciation_fixed_asset_list_set(
				fixed_asset_list,
				depreciation_date_string,
				prior_fixed_prior_date );
		}

		/* PROPERTY_PURCHASE */
		/* ----------------- */
		fixed_asset_list =
			depreciation_asset_list->
				purchase_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			depreciation_fixed_asset_list_set(
				fixed_asset_list,
				depreciation_date_string,
				prior_property_date );
		}

		/* PRIOR_PROPERTY */
		/* -------------- */
		fixed_asset_list =
			depreciation_asset_list->
				prior_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			depreciation_fixed_asset_list_set(
				fixed_asset_list,
				depreciation_date_string,
				prior_property_prior_date );
		}

	} while( list_next( depreciation_fund_list ) );

} /* depreciation_fund_list_asset_list_set() */

void depreciation_fixed_asset_list_set(
			LIST *fixed_asset_list,
			char *depreciation_date_string,
			char *prior_depreciation_date )
{
	FIXED_ASSET *fixed_asset;
	DEPRECIATION *depreciation;

	if ( !list_rewind( fixed_asset_list ) ) return;

	do {
		fixed_asset = list_get_pointer( fixed_asset_list );

		depreciation = depreciation_new( depreciation_date_string );

		depreciation->depreciation_amount =
			depreciation_fixed_asset_calculate_amount(
				fixed_asset->depreciation_method,
				fixed_asset->extension,
				fixed_asset->estimated_residual_value,
				fixed_asset->estimated_useful_life_years,
				fixed_asset->estimated_useful_life_units,
				fixed_asset->declining_balance_n,
				prior_depreciation_date,
				depreciation->
					depreciation_date_string,
				fixed_asset->finance_accumulated_depreciation,
				fixed_asset->service_placement_date,
				0 /* units_produced */ );

		if ( depreciation->depreciation_amount )
		{
			fixed_asset->finance_accumulated_depreciation +=
				depreciation->depreciation_amount;

			fixed_asset->depreciation = depreciation;
		}

	} while( list_next( fixed_asset_list ) );

} /* depreciation_fixed_asset_list_set() */

