/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_recovery.c			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>

/* Include tax_recovery.h in fixed_asset.h */
/* --------------------------------------- */
#include "fixed_asset.h"

#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "appaserver_library.h"

boolean tax_recovery_parse(
				int *service_month,
				int *service_year,
				int *disposal_month,
				int *disposal_year,
				char *service_placement_date_string,
				char *disposal_date_string )
{
	DATE *service_placement_date;
	DATE *disposal_date = {0};

	*service_month = 0;
	*service_year = 0;
	*disposal_month = 0;
	*disposal_year = 0;

	if ( ! ( service_placement_date =
			date_yyyy_mm_dd_new(
				service_placement_date_string ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid service_placement_date = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 service_placement_date_string );

		return 0;
	}

	*service_month = date_get_month( service_placement_date );
	*service_year = date_get_year( service_placement_date );
	date_free( service_placement_date );

	if ( disposal_date_string && *disposal_date_string )
	{
		if ( ! ( disposal_date =
				date_yyyy_mm_dd_new(
					disposal_date_string ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid disposal_date = (%s)\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	disposal_date_string );

			return 0;
		}

		*disposal_month = date_get_month( disposal_date );
		*disposal_year = date_get_year( disposal_date );
		date_free( disposal_date );
	}

	return 1;

} /* tax_recovery_parse() */

double tax_recovery_calculate_recovery_amount(
				double *recovery_percent,
				double cost_basis,
				char *service_placement_date_string,
				char *disposal_date_string,
				double recovery_period_years,
				int tax_year )
{
	double recovery_amount = 0.0;
	int service_month = 0;
	int service_year = 0;
	int disposal_month = 0;
	int disposal_year = 0;
	int recovery_period_months;
	int recovery_period_semi_months;
	double percent_per_year;
	double percent_per_semi_month;
	unsigned int recovery_months_as_of_december;
	unsigned int recovery_months_extra_year;
	double applicable_rate = 0.0;

	if ( !tax_recovery_parse(
				&service_month,
				&service_year,
				&disposal_month,
				&disposal_year,
				service_placement_date_string,
				disposal_date_string ) )
	{
		return 0.0;
	}

	recovery_period_months = (int)(recovery_period_years * 12.0);
	recovery_period_semi_months = recovery_period_months * 2;
	percent_per_year = 1.0 / recovery_period_years;
	percent_per_semi_month = 1.0 / (double)recovery_period_semi_months;

	recovery_months_as_of_december =
		( tax_year * 12 - service_year * 12 ) +
		( ( 12 - service_month ) + 1 );

	recovery_months_extra_year =
		recovery_period_months + 12;

	if ( disposal_year == tax_year
	&&   recovery_months_as_of_december <= recovery_period_months )
	{
		applicable_rate =
			(double)( ( ( disposal_month - 1 ) * 2 ) + 1 ) *
			percent_per_semi_month;
	}
	else
	if ( disposal_year )
	{
		return 0.0;
	}
	else
	if ( recovery_months_as_of_december <= 12 )
	{
		applicable_rate =
			(double)( ( ( 12 - service_month ) * 2 ) + 1 ) *
			percent_per_semi_month;
	}
	else
	if ( recovery_months_as_of_december > 12
	&&   recovery_months_as_of_december <= recovery_period_months )
	{
		applicable_rate = percent_per_year;
	}
	else
	if ( recovery_months_as_of_december > recovery_period_months
	&&   recovery_months_as_of_december <= recovery_months_extra_year )
	{
		if ( timlib_double_is_integer( recovery_period_years ) )
		{
			applicable_rate =
				(double)
				( ( service_month * 2 ) - 1 ) *
				percent_per_semi_month;
		}
		else
		/* ------------------ */
		/* If extra half-year */
		/* ------------------ */
		if ( service_month <= 6 )
		{
			applicable_rate =
			(double)( 12 + ( service_month * 2 ) - 1 ) *
			percent_per_semi_month;
		}
		else
		{
			applicable_rate =
			(double)( ( service_month - 6 ) * 2 - 1 ) *
			percent_per_semi_month;
		}
	}

	recovery_amount = cost_basis * applicable_rate;

	if ( recovery_percent )
	{
		*recovery_percent = applicable_rate * 100.0;
	}

	return recovery_amount;

} /* tax_recovery_calculate_recovery_amount() */

int tax_recovery_fetch_max_tax_year(
			char *application_name,
			char *folder_name )
{
	char sys_string[ 1024 ];
	int max_tax_year;

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			",
		 application_name,
		 "max(tax_year)",
		 folder_name );

	max_tax_year = atoi( pipe2string( sys_string ) );

	return max_tax_year;

} /* tax_recovery_fetch_max_tax_year() */

TAX_RECOVERY *tax_recovery_new(	char *service_placement_date,
				double tax_cost_basis,
				char *tax_recovery_period,
				char *disposal_date,
				int tax_year )
{
	TAX_RECOVERY *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_RECOVERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->service_placement_date = service_placement_date;
	t->tax_cost_basis = tax_cost_basis;
	t->tax_recovery_period = tax_recovery_period;
	t->disposal_date = disposal_date;
	t->tax_year = tax_year;

	if ( t->tax_recovery_period )
	{
		t->tax_recovery_period_years =
			atof( t->tax_recovery_period );
	}

	return t;

} /* tax_recovery_new() */

void tax_recovery_depreciation_fund_list_set(
			LIST *depreciation_fund_list,
			int tax_year )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;
	LIST *fixed_asset_list;

	if ( !list_rewind( depreciation_fund_list ) ) return;

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
			tax_recovery_fixed_asset_list_set(
				fixed_asset_list,
				tax_year );
		}

		fixed_asset_list =
			depreciation_asset_list->
				prior_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_set(
				fixed_asset_list,
				tax_year );
		}

		fixed_asset_list =
			depreciation_asset_list->
				purchase_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_set(
				fixed_asset_list,
				tax_year );
		}

		fixed_asset_list =
			depreciation_asset_list->
				prior_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_set(
				fixed_asset_list,
				tax_year );
		}

	} while( list_next( depreciation_fund_list ) );

} /* tax_recovery_depreciation_fund_list_set() */

void tax_recovery_fixed_asset_list_set(	LIST *fixed_asset_list,
					int tax_year )
{
	FIXED_ASSET *fixed_asset;

	if ( !list_rewind( fixed_asset_list ) ) return;

	do {
		fixed_asset = list_get_pointer( fixed_asset_list );

		fixed_asset->tax_recovery =
			tax_recovery_new(
				fixed_asset->service_placement_date,
				fixed_asset->tax_cost_basis,
				fixed_asset->tax_recovery_period,
				fixed_asset->disposal_date,
				tax_year );

		/* Need to write the table lookup function. */
		/* ---------------------------------------- */
		if ( fixed_asset->
			tax_recovery->
			tax_recovery_period_years < 27.5 )
		{
			continue;
		}

		fixed_asset->tax_recovery->recovery_amount =
			tax_recovery_calculate_recovery_amount(
				&fixed_asset->tax_recovery->recovery_percent,
				fixed_asset->tax_recovery->tax_cost_basis,
				fixed_asset->
					tax_recovery->
					service_placement_date,
				fixed_asset->
					tax_recovery->
					disposal_date,
				fixed_asset->
					tax_recovery->
					tax_recovery_period_years,
				fixed_asset->
					tax_recovery->
					tax_year );

	} while( list_next( fixed_asset_list ) );

} /* tax_recovery_fixed_asset_list_set() */

boolean tax_recovery_fixed_assets_insert(
				LIST *depreciation_fund_list )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;
	LIST *fixed_asset_list;
	boolean did_any = 0;

	if ( !list_rewind( depreciation_fund_list ) ) return 0;

	do {
		depreciation_fund =
			list_get_pointer(
				depreciation_fund_list );

		depreciation_asset_list =
			depreciation_fund->
				depreciation_asset_list;

		if ( !depreciation_asset_list ) continue;

		/* FIXED_ASSET_PURCHASE */
		/* -------------------- */
		fixed_asset_list =
			depreciation_asset_list->
				purchase_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_insert(
				"tax_fixed_asset_recovery",
				fixed_asset_list );

			did_any = 1;
		}

		/* PROPERTY_PURCHASE */
		/* ----------------- */
		fixed_asset_list =
			depreciation_asset_list->
				purchase_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_insert(
				"tax_property_recovery",
				fixed_asset_list );

			did_any = 1;
		}

		/* PRIOR_FIXED_ASSET */
		/* ----------------- */
		fixed_asset_list =
			depreciation_asset_list->
				prior_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_insert(
				"tax_prior_fixed_asset_recovery",
				fixed_asset_list );

			did_any = 1;
		}

		/* PRIOR_PROPERTY */
		/* -------------- */
		fixed_asset_list =
			depreciation_asset_list->
				prior_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_insert(
				"tax_prior_property_recovery",
				fixed_asset_list );

			did_any = 1;
		}

	} while( list_next( depreciation_fund_list ) );

	return did_any;

} /* tax_recovery_fixed_assets_insert() */

void tax_recovery_fixed_asset_list_insert(
					char *folder_name,
					LIST *fixed_asset_list )
{
	FIXED_ASSET *fixed_asset;
	TAX_RECOVERY *tax_recovery;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *field;

	if ( !list_rewind( fixed_asset_list ) ) return;

	field =
"asset_name,serial_number,tax_year,recovery_amount,recovery_percent";

	sprintf( sys_string,
	 	 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e						 ",
		 folder_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	do {
		fixed_asset =
			list_get_pointer(
				fixed_asset_list );

		tax_recovery = fixed_asset->tax_recovery;

		if ( !tax_recovery )
		{
			fprintf( stderr,
				"ERROR in %s/%s()/%d: empty tax_recovery.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			pclose( output_pipe );
			exit( 1 );
		}

		fprintf(	output_pipe,
				"%s^%s^%d^%.2lf^%.3lf\n",
				fixed_asset->asset_name,
				fixed_asset->serial_number,
				tax_recovery->tax_year,
				tax_recovery->recovery_amount,
				tax_recovery->recovery_percent );

	} while( list_next( fixed_asset_list ) );

	pclose( output_pipe );

} /* tax_recovery_fixed_asset_list_insert() */

void tax_recovery_fixed_assets_display(	LIST *depreciation_fund_list )
{
	DEPRECIATION_FUND *depreciation_fund;
	DEPRECIATION_ASSET_LIST *depreciation_asset_list;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;
	char *justify;
	LIST *fixed_asset_list;

	if ( !list_rewind( depreciation_fund_list ) ) return;

	heading =
	"asset_name,serial_number,cost_basis,recovery_amount,recovery_percent";

	justify = "left,left,right,right,right";

	sprintf( sys_string,
		 "html_table.e '' '%s' '^' '%s'",
		 heading,
		 justify );

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

		/* FIXED_ASSET_PURCHASE */
		/* -------------------- */
		fixed_asset_list =
			depreciation_asset_list->
				purchase_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_display(
				output_pipe,
				fixed_asset_list );
		}

		/* PROPERTY_PURCHASE */
		/* ----------------- */
		fixed_asset_list =
			depreciation_asset_list->
				purchase_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_display(
				output_pipe,
				fixed_asset_list );
		}

		/* PRIOR_FIXED_ASSET */
		/* ----------------- */
		fixed_asset_list =
			depreciation_asset_list->
				prior_fixed_asset_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_display(
				output_pipe,
				fixed_asset_list );
		}

		/* PRIOR_PROPERTY */
		/* -------------- */
		fixed_asset_list =
			depreciation_asset_list->
				prior_property_list;

		if ( list_length( fixed_asset_list ) )
		{
			tax_recovery_fixed_asset_list_display(
				output_pipe,
				fixed_asset_list );
		}

	} while( list_next( depreciation_fund_list ) );

	pclose( output_pipe );

} /* tax_recovery_fixed_assets_display() */

void tax_recovery_fixed_asset_list_display(
					FILE *output_pipe,
					LIST *fixed_asset_list )
{
	FIXED_ASSET *fixed_asset;
	TAX_RECOVERY *tax_recovery;

	if ( !list_rewind( fixed_asset_list ) ) return;

	do {
		fixed_asset =
			list_get_pointer(
				fixed_asset_list );

		tax_recovery = fixed_asset->tax_recovery;

		if ( !tax_recovery )
		{
			fprintf( stderr,
				"ERROR in %s/%s()/%d: empty tax_recovery.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			pclose( output_pipe );
			exit( 1 );
		}

		fprintf(	output_pipe,
				"%s^%s^%.2lf^%.2lf^%.3lf\n",
				fixed_asset->asset_name,
				fixed_asset->serial_number,
				tax_recovery->tax_cost_basis,
				tax_recovery->recovery_amount,
				tax_recovery->recovery_percent );

	} while( list_next( fixed_asset_list ) );

} /* tax_recovery_fixed_assets_display() */

