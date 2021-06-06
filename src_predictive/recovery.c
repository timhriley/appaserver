/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/recovery.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>

#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "date.h"
#include "fixed_asset.h"
#include "fixed_asset_purchase.h"
#include "recovery.h"

RECOVERY_STRAIGHT_LINE *recovery_straight_line_calloc( void )
{
	RECOVERY_STRAIGHT_LINE *recovery_straight_line;

	if ( ! ( recovery_straight_line =
			calloc( 1, sizeof( RECOVERY_STRAIGHT_LINE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty..\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return recovery_straight_line;
}

RECOVERY_ACCELERATED *recovery_accelerated_calloc( void )
{
	RECOVERY_ACCELERATED *recovery_accelerated;

	if ( ! ( recovery_accelerated =
			calloc( 1, sizeof( RECOVERY_ACCELERATED ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty..\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return recovery_accelerated;
}

RECOVERY *recovery_parse(
			char *input )
{
	char asset_name[ 128 ];
	char serial_label[ 128 ];
	char tax_year[ 128 ];
	char piece_buffer[ 128 ];
	RECOVERY *recovery;

	if ( !input || !*input ) return (RECOVERY *)0;

	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );
	piece( tax_year, SQL_DELIMITER, input, 2 );

	recovery =
		recovery_new(
			strdup( asset_name ),
			strdup( serial_label ),
			atoi( tax_year ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	recovery->recovery_amount = atof( piece_buffer );

	return recovery;
}

RECOVERY *recovery_new(
			char *asset_name,
			char *serial_label,
			int tax_year )
{
	RECOVERY *recovery;

	if ( ! ( recovery = calloc( 1, sizeof( RECOVERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	recovery->asset_name = asset_name;
	recovery->serial_label = serial_label;
	recovery->tax_year = tax_year;

	return recovery;
}

RECOVERY *recovery_evaluate(
			char *asset_name,
			char *serial_label,
			int tax_year,
			double cost_basis,
			char *service_placement_date,
			char *disposal_date,
			char *cost_recovery_period_string,
			char *cost_recovery_method,
			char *cost_recovery_conversion )
{
	RECOVERY *recovery;
	DATE *service_placement;
	DATE *disposal;

	if ( !tax_year ) return (RECOVERY *)0;
	if ( !cost_basis ) return (RECOVERY *)0;

	if ( !service_placement_date || !*service_placement_date )
	{
		return (RECOVERY *)0;
	}

	if ( !cost_recovery_period_string || !*cost_recovery_period_string )
	{
		return (RECOVERY *)0;
	}

	recovery =
		recovery_new(
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

		return (RECOVERY *)0;
	}

	recovery->service_placement_month = date_month( service_placement );
	recovery->service_placement_year = date_year( service_placement );

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

			return (RECOVERY *)0;
		}

		recovery->disposal_month = date_month( disposal );
		recovery->disposal_year = date_year( disposal );
	}

	recovery->cost_basis = cost_basis;

	recovery->recovery_period_years =
		recovery_period_years(
			cost_recovery_period_string );

	recovery->cost_recovery_method = cost_recovery_method;
	recovery->cost_recovery_conversion = cost_recovery_conversion;

	if ( strcmp(	recovery->cost_recovery_method,
			RECOVERY_STATUTORY_STRAIGHT_LINE ) == 0 )
	{
		recovery->straight_line =
			recovery_statutory_straight_line_evaluate(
				recovery->tax_year,
				recovery->cost_basis,
				recovery->service_placement_month,
				recovery->service_placement_year,
				recovery->disposal_month,
				recovery->disposal_year,
				recovery->recovery_period_years,
				recovery->cost_recovery_conversion );
	}
	else
	if ( strcmp(	recovery->cost_recovery_method,
			RECOVERY_STATUTORY_ACCELERATED ) == 0 )
	{
		recovery->accelerated =
			recovery_statutory_accelerated_evaluate(
				recovery->tax_year,
				recovery->cost_basis,
				recovery->service_placement_month,
				recovery->service_placement_year,
				recovery->disposal_month,
				recovery->disposal_year,
				recovery->recovery_period_years,
				recovery->cost_recovery_conversion );
	}

	recovery->recovery_amount =
		recovery_amount(
			recovery->straight_line,
			recovery->accelerated );

	recovery->recovery_rate =
		recovery_rate(
			recovery->straight_line,
			recovery->accelerated );

	return recovery;
}

char *recovery_primary_where(
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

RECOVERY *recovery_fetch(
			char *asset_name,
			char *serial_label,
			int tax_year )
{
	char sys_string[ 1024 ];

	if ( !asset_name
	||   !serial_label )
	{
		return (RECOVERY *)0;
	}

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" ''",
		 COST_RECOVERY_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 recovery_primary_where(
			asset_name,
			serial_label,
			tax_year ) );

	return recovery_parse( string_pipe_fetch( sys_string ) );
}

LIST *recovery_system_list(
			char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *recovery_list;

	recovery_list = list_new();
	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			recovery_list, 
			recovery_parse( input ) );
	}

	pclose( input_pipe );
	return recovery_list;
}

char *recovery_system_string(
			char *where )
{
	char system_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf(system_string,
		"select.sh '*' %s \"%s\" tax_year",
		COST_RECOVERY_TABLE,
		where );

	return strdup( system_string );
}

LIST *recovery_list_fetch(
			char *asset_name,
			char *serial_label )
{
	return recovery_system_list(
			recovery_system_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				fixed_asset_purchase_primary_where(
					asset_name,
					serial_label ) ) );
}

FILE *recovery_insert_open( void )
{
	char sys_string[ 1024 ];
	char *field;

	field =	"asset_name,"
		"serial_label,"
		"tax_year,"
		"recovery_amount";

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^'	|"
		 "tee_appaserver_error.sh				|"
		 "sql 2>&1						 ",
		 COST_RECOVERY_TABLE,
		 field );

	return popen( sys_string, "w" );
}

void recovery_insert(
			FILE *insert_pipe,
			char *asset_name,
			char *serial_label,
			int tax_year,
			double recovery_amount )
{
	if ( !asset_name
	||   !serial_label
	||   !tax_year
	||   !recovery_amount )
	{
		return;
	}

	fprintf(insert_pipe,
		"%s^%s^%d^%.2lf\n",
		asset_name,
		serial_label,
		tax_year,
		recovery_amount );
}

void recovery_list_insert(
			LIST *recovery_list )
{
	RECOVERY *recovery;
	FILE *insert_pipe;

	if ( !list_rewind( recovery_list ) ) return;

	insert_pipe = recovery_insert_open();

	do {
		recovery =
			list_get(
				recovery_list );

		recovery_insert(
			insert_pipe,
			recovery->asset_name,
			recovery->serial_label,
			recovery->tax_year,
			recovery->recovery_amount );

	} while ( list_next( recovery_list ) );

	pclose( insert_pipe );
}

double recovery_period_years(
			char *cost_recovery_period_string )
{
	if ( !cost_recovery_period_string
	||   !*cost_recovery_period_string )
	{
		return 0.0;
	}

	return atof( cost_recovery_period_string );
}

int recovery_service_placement_years(
			int tax_year,
			int service_placement_year )
{
	return tax_year - service_placement_year;
}

RECOVERY_STRAIGHT_LINE *recovery_statutory_straight_line_evaluate(
			int tax_year,
			double cost_basis,
			int service_placement_month,
			int service_placement_year,
			int disposal_month,
			int disposal_year,
			double recovery_period_years,
			char *cost_recovery_conversion )
{
	RECOVERY_STRAIGHT_LINE *straight_line = {0};
	double straight_line_rate;
	int service_placement_years;

	if ( string_strcmp(	cost_recovery_conversion,
				RECOVERY_HALF_YEAR ) == 0 )
	{
		service_placement_years =
			recovery_service_placement_years(
				tax_year,
				service_placement_year );

		if ( ( straight_line_rate =
			recovery_statutory_straight_line_half_year_rate(
				service_placement_years,
				tax_year,
				disposal_year,
				recovery_period_years ) ) )
		{
			straight_line = recovery_straight_line_calloc();

			straight_line->service_placement_years =
				service_placement_years;

			straight_line->straight_line_rate =
				straight_line_rate;

			straight_line->straight_line_amount =
				recovery_straight_line_amount(
					straight_line->straight_line_rate,
					cost_basis );
		}
	}

	return straight_line;
}

double recovery_statutory_straight_line_half_year_mid_year_rate(
			double recovery_period_years )
{
	if ( !recovery_period_years ) return 0.0;

	return 1.0 / recovery_period_years;
}

double recovery_statutory_straight_line_half_year_book_ends_rate(
			double half_year_mid_year_rate )
{
	return half_year_mid_year_rate / 2.0;
}

double recovery_statutory_straight_line_half_year_rate(
			int service_placement_years,
			int tax_year,
			int disposal_year,
			double recovery_period_years )
{
	double rate = 0.0;

	if ( !tax_year ) return 0.0;
	if ( !recovery_period_years ) return 0.0;

	if ( (double)service_placement_years > recovery_period_years )
	{
		rate = 0.0;
	}
	else
	if ( tax_year == disposal_year )
	{
		rate =
		  recovery_statutory_straight_line_half_year_book_ends_rate(
		     recovery_statutory_straight_line_half_year_mid_year_rate(
				recovery_period_years ) );
	}
	else
	if ( ( service_placement_years == 0 )
	||   ( (double)service_placement_years == recovery_period_years ) )
	{
		rate =
		  recovery_statutory_straight_line_half_year_book_ends_rate(
		       recovery_statutory_straight_line_half_year_mid_year_rate(
				recovery_period_years ) );
	}
	if ( service_placement_years > 0
	&&   (double)service_placement_years < recovery_period_years )
	{
		rate =
		  recovery_statutory_straight_line_half_year_mid_year_rate(
			recovery_period_years );
	}

	return rate;
}

int recovery_prior_tax_year( void )
{
	static char *prior_tax_year = {0};

	if ( !prior_tax_year )
	{
		char system_string[ 1024 ];

		sprintf(system_string,
			"select.sh \"%s\" %s '' ''",
			"max( tax_year )",
			COST_RECOVERY_TABLE );

		prior_tax_year = string_pipe_fetch( system_string );
	}
	return (prior_tax_year) ? atoi( prior_tax_year ) : 0;
}

FILE *recovery_delete_open( void )
{
	char system_string[ 1024 ];
	char *key;

	key =	"asset_name,"
		"serial_label,"
		"tax_year";

	sprintf( system_string,
		 "delete_statement table=%s field=%s delimiter='%c'	|"
		 "tee_appaserver_error.sh				|"
		 "sql							 ",
		 COST_RECOVERY_TABLE,
		 key,
		 '^' );

	return popen( system_string, "w" );
}

char *recovery_subquery_where(
			int tax_year )
{
	char where[ 1024 ];

	sprintf(where,
		"exists ( select 1					"
		"	  from cost_recovery				"
		"	  where fixed_asset_purchase.asset_name =	"
		"		cost_recovery.asset_name		"
		"	    and fixed_asset_purchase.serial_label =	"
		"		cost_recovery.serial_label		"
		"	    and tax_year = %d )				",
		tax_year );

	return strdup( where );
}

double recovery_amount(
			RECOVERY_STRAIGHT_LINE *straight_line,
			RECOVERY_ACCELERATED *accelerated )
{
	if ( straight_line )
	{
		return straight_line->straight_line_amount;
	}
	else
	if ( accelerated )
	{
		return accelerated->accelerated_amount;
	}

	return 0.0;
}

double recovery_rate(
			RECOVERY_STRAIGHT_LINE *straight_line,
			RECOVERY_ACCELERATED *accelerated )
{
	if ( straight_line )
	{
		return straight_line->straight_line_rate;
	}
	else
	if ( accelerated )
	{
		return accelerated->accelerated_rate;
	}

	return 0.0;
}

FILE *recovery_update_open( void )
{
	char system_string[ 1024 ];
	char *key;

	key =	"asset_name,"
		"serial_label,"
		"tax_year";

	sprintf( system_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 COST_RECOVERY_TABLE,
		 key );

	return popen( system_string, "w" );
}

void recovery_update(	double recovery_amount,
			char *asset_name,
			char *serial_label,
			int tax_year )
{
	FILE *update_pipe = recovery_update_open();
	char *escape = fixed_asset_name_escape( asset_name );

	fprintf(update_pipe,
		"%s^%s^%d^recovery_amount^%.2lf\n",
		escape,
		serial_label,
		tax_year,
		recovery_amount );

	pclose( update_pipe );
}

double recovery_straight_line_amount(
			double straight_line_rate,
			double cost_basis )
{
	return straight_line_rate * cost_basis;
}

RECOVERY_ACCELERATED *recovery_statutory_accelerated_evaluate(
			int tax_year,
			double cost_basis,
			int service_placement_month,
			int service_placement_year,
			int disposal_month,
			int disposal_year,
			double recovery_period_years,
			char *cost_recovery_conversion )
{
	RECOVERY_ACCELERATED *accelerated = {0};
	double accelerated_rate;

	if ( string_strcmp(	cost_recovery_conversion,
				RECOVERY_HALF_YEAR ) == 0 )
	{
		if ( ( accelerated_rate =
			recovery_statutory_accelerated_half_year_rate(
				tax_year,
				disposal_year,
				recovery_period_years ) ) )
		{
			accelerated = recovery_accelerated_calloc();

			accelerated->accelerated_rate =
				accelerated_rate;

			accelerated->accelerated_amount =
				recovery_accelerated_amount(
					accelerated->accelerated_rate,
					cost_basis );
		}
	}

	return straight_line;
}

double recovery_statutory_accelerated_half_year_rate(
			int tax_year,
			int disposal_year,
			double recovery_period_years )
{
	double rate;

	if ( tax_year > disposal_year ) return 0.0;

	rate =
		recovery_statutory_accelerated_half_year_evaluate(
			tax_year,
			recovery_period_years );

	if ( disposal_year == tax_year )
		rate *= 0.5;

	return rate;
}

double recovery_statutory_accelerated_half_year_evaluate(
			int tax_year,
			double recovery_period_years )
{
	char system_string[ 1024 ];
	char *results;

	sprintf(system_string,
		"recovery_statutory_accelerated_half_year.sh %.0lf %d",
		recovery_period_years,
		tax_year );

	results = string_pipe_fetch( system_string );

	if ( results && *results )
		return atof( results );
	else
		return 0.0;
}

double recovery_accelerated_amount(
			double accelerated_rate,
			double cost_basis )
{
	return accelerated_rate * cost_basis;
}

