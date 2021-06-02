/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_recovery.c			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>

#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "date.h"
#include "fixed_asset_purchase.h"
#include "tax_recovery.h"

int tax_recovery_now_year( void )
{
	return atoi( date_now_yyyy_mm_dd( date_utc_offset() ) );
}

int tax_recovery_prior_recovery_year(
			char *asset_name,
			char *serial_label )
{
	char system_string[ 1024 ];
	char *where;

	if ( asset_name && serial_label )
	{
		where = 
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			fixed_asset_purchase_primary_where(
				asset_name,
				serial_label );
	}
	else
	{
		where = "1 = 1";
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" ''",
		"max( tax_year )",
		TAX_RECOVERY_TABLE,
		where );

	return atoi( string_pipe_fetch( system_string ) );
}

TAX_RECOVERY *tax_recovery_parse(
			char *input )
{
	char asset_name[ 128 ];
	char serial_label[ 128 ];
	char tax_year[ 128 ];
	char piece_buffer[ 128 ];
	TAX_RECOVERY *tax_recovery;

	if ( !input || !*input ) return (TAX_RECOVERY *)0;

	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );
	piece( tax_year, SQL_DELIMITER, input, 2 );

	tax_recovery =
		tax_recovery_new(
			strdup( asset_name ),
			strdup( serial_label ),
			atoi( tax_year ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	tax_recovery->recovery_percent = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	tax_recovery->tax_recovery_amount = atof( piece_buffer );

	return tax_recovery;
}

TAX_RECOVERY *tax_recovery_new(
			char *asset_name,
			char *serial_label,
			int tax_year )
{
	TAX_RECOVERY *tax_recovery;

	if ( ! ( tax_recovery = calloc( 1, sizeof( TAX_RECOVERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	tax_recovery->asset_name = asset_name;
	tax_recovery->serial_label = serial_label;
	tax_recovery->tax_year = tax_year;

	return tax_recovery;
}

double tax_recovery_period_years(
			char *recovery_period_years )
{
	if ( !recovery_period_years
	||   !*recovery_period_years )
	{
		return 0.0;
	}

	return atof( recovery_period_years );
}

TAX_RECOVERY *tax_recovery_evaluate(
			char *asset_name,
			char *serial_label,
			int tax_year,
			char *service_placement_date,
			char *disposal_date,
			double cost_basis,
			char *tax_recovery_period,
			double prior_accumulated_recovery )
{
	TAX_RECOVERY *tax_recovery;
	DATE *service_placement;
	DATE *disposal;

	if ( !tax_year ) return (TAX_RECOVERY *)0;

	if ( !service_placement_date || !*service_placement_date )
	{
		return (TAX_RECOVERY *)0;
	}

	if ( !tax_recovery_period || !*tax_recovery_period )
	{
		return (TAX_RECOVERY *)0;
	}

	tax_recovery =
		tax_recovery_new(
			asset_name,
			serial_label,
			tax_year );

	if ( ! ( service_placement =
			date_yyyy_mm_dd_new(
				service_placement_date ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid service_placement_date = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 service_placement_date );

		return (TAX_RECOVERY *)0;
	}

	tax_recovery->service_month = date_month( service_placement );
	tax_recovery->service_year = date_year( service_placement );

	if ( disposal_date && *disposal_date )
	{
		if ( ! ( disposal =
				date_yyyy_mm_dd_new(
					disposal_date ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: invalid disposal_date = (%s)\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	disposal_date );

			return (TAX_RECOVERY *)0;
		}

		tax_recovery->disposal_month = date_month( disposal );
		tax_recovery->disposal_year = date_year( disposal );
	}

	tax_recovery->cost_basis = cost_basis;

	tax_recovery->tax_recovery_period_years =
		tax_recovery_period_years(
			tax_recovery_period );

	tax_recovery->prior_accumulated_recovery = prior_accumulated_recovery;

	tax_recovery->tax_recovery_amount =
		tax_recovery_amount(
			&tax_recovery->recovery_percent,
			tax_recovery->tax_year,
			tax_recovery->service_month,
			tax_recovery->service_year,
			tax_recovery->disposal_month,
			tax_recovery->disposal_year,
			tax_recovery->cost_basis,
			tax_recovery->tax_recovery_period_years,
			tax_recovery->prior_accumulated_recovery );

	return tax_recovery;
}

char *tax_recovery_primary_where(
			char *asset_name,
			char *serial_label,
			int tax_year )
{
	static char where[ 256 ];

	sprintf(where,
		"asset_name = '%s' and		"
		"serial_label = '%s' and	"
		"tax_year = %d			",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_name_escape( asset_name ),
		serial_label,
		tax_year );

	return strdup( where );
}

TAX_RECOVERY *tax_recovery_fetch(
			char *asset_name,
			char *serial_label,
			int tax_year )
{
	char sys_string[ 1024 ];

	if ( !asset_name
	||   !serial_label )
	{
		return (TAX_RECOVERY *)0;
	}

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" ''",
		 TAX_RECOVERY_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 tax_recovery_primary_where(
			asset_name,
			serial_label,
			tax_year ) );

	return tax_recovery_parse( string_pipe_fetch( sys_string ) );
}

LIST *tax_recovery_system_list(
			char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *tax_recovery_list;

	tax_recovery_list = list_new();
	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			tax_recovery_list, 
			tax_recovery_parse( input ) );
	}

	pclose( input_pipe );
	return tax_recovery_list;
}

char *tax_recovery_system_string(
			char *where )
{
	char system_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf(system_string,
		"select.sh '*' %s \"%s\" tax_year",
		TAX_RECOVERY_TABLE,
		where );

	return strdup( system_string );
}

LIST *tax_recovery_list_fetch(
			char *asset_name,
			char *serial_label )
{
	return tax_recovery_system_list(
			tax_recovery_system_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				fixed_asset_purchase_primary_where(
					asset_name,
					serial_label ) ) );
}

FILE *tax_recovery_insert_open( void )
{
	char sys_string[ 1024 ];
	char *field;

	field =	"asset_name,"
		"serial_label,"
		"tax_year,"
		"recovery_percent,"
		"recovery_amount";

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^'	|"
		 "sql 2>&1						 ",
		 TAX_RECOVERY_TABLE,
		 field );

	return popen( sys_string, "w" );
}

void tax_recovery_insert(
			FILE *insert_pipe,
			char *asset_name,
			char *serial_label,
			int tax_year,
			double recovery_percent,
			double recovery_amount )
{
	if ( !asset_name
	||   !serial_label
	||   !tax_year
	||   !recovery_percent
	||   !recovery_amount )
	{
		return;
	}

	fprintf(insert_pipe,
		"%s^%s^%d^%.2lf^%.2lf\n",
		asset_name,
		serial_label,
		tax_year,
		recovery_percent,
		recovery_amount );
}

void tax_recovery_list_insert(
			LIST *tax_recovery_list )
{
	TAX_RECOVERY *tax_recovery;
	FILE *insert_pipe;

	if ( !list_rewind( tax_recovery_list ) ) return;

	insert_pipe = tax_recovery_insert_open();

	do {
		tax_recovery =
			list_get(
				tax_recovery_list );

		tax_recovery_insert(
			insert_pipe,
			tax_recovery->asset_name,
			tax_recovery->serial_label,
			tax_recovery->tax_year,
			tax_recovery->recovery_percent,
			tax_recovery->tax_recovery_amount );

	} while ( list_next( tax_recovery_list ) );

	pclose( insert_pipe );
}

double tax_recovery_period_months(
			double tax_recovery_period_years )
{
	return tax_recovery_period_years * 12.0;
}

double tax_recovery_period_semi_months(
			double tax_recovery_period_months )
{
	return tax_recovery_period_months * 2.0;
}

double tax_recovery_percent_per_year(
			double tax_recovery_period_years )
{
	return 1.0 / tax_recovery_period_years;
}

double tax_recovery_amount(
			double *recovery_percent,
			int tax_year,
			int service_month,
			int service_year,
			int disposal_month,
			int disposal_year,
			double cost_basis,
			double tax_recovery_period_years,
			double prior_accumulated_recovery )
{
	double recovery_amount;
	int recovery_period_months;
	int recovery_period_semi_months;
	double percent_per_year;
	double percent_per_semi_month;
	unsigned int recovery_months_as_of_december;
	unsigned int recovery_months_extra_year;
	double applicable_rate = 0.0;

	recovery_period_months = (int)(tax_recovery_period_years * 12.0);
	recovery_period_semi_months = recovery_period_months * 2;
	percent_per_year = 1.0 / tax_recovery_period_years;
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
		if ( timlib_double_is_integer( tax_recovery_period_years ) )
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

	if ( !applicable_rate ) return 0.0;

	recovery_amount = cost_basis * applicable_rate;

	if ( recovery_percent )
	{
		*recovery_percent = applicable_rate * 100.0;
	}

	if ( recovery_amount + prior_accumulated_recovery > cost_basis )
	{
		recovery_amount = cost_basis - prior_accumulated_recovery;
	}

	return recovery_amount;
}
