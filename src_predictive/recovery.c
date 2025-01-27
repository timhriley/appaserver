/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/recovery.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "date.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "fixed_asset.h"
#include "fixed_asset_purchase.h"
#include "recovery.h"

RECOVERY_STRAIGHT_LINE *recovery_straight_line_calloc( void )
{
	RECOVERY_STRAIGHT_LINE *recovery_straight_line;

	if ( ! ( recovery_straight_line =
			calloc( 1, sizeof ( RECOVERY_STRAIGHT_LINE ) ) ) )
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
			calloc( 1, sizeof ( RECOVERY_ACCELERATED ) ) ) )
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

RECOVERY *recovery_parse( char *input )
{
	char asset_name[ 128 ];
	char serial_label[ 128 ];
	char tax_year[ 128 ];
	char piece_buffer[ 128 ];
	RECOVERY *recovery;

	if ( !input || !*input ) return (RECOVERY *)0;

	/* See RECOVERY_SELECT */
	/* ------------------- */
	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );
	piece( tax_year, SQL_DELIMITER, input, 2 );

	recovery =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recovery_new(
			strdup( asset_name ),
			strdup( serial_label ),
			atoi( tax_year ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	recovery->rate = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	recovery->amount = atof( piece_buffer );

	return recovery;
}

RECOVERY *recovery_calloc( void )
{
	RECOVERY *recovery;

	if ( ! ( recovery = calloc( 1, sizeof ( RECOVERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return recovery;
}

RECOVERY *recovery_new(
		char *asset_name,
		char *serial_label,
		int tax_year )
{
	RECOVERY *recovery;

	if ( !asset_name
	||   !serial_label
	||   !tax_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	recovery = recovery_calloc();

	recovery->asset_name = asset_name;
	recovery->serial_label = serial_label;
	recovery->tax_year = tax_year;

	return recovery;
}

DATE *recovery_service_placement_date(
		char *service_placement_date_string )
{
	return
	date_yyyy_mm_dd_new(
		service_placement_date_string );
}

DATE *recovery_disposal_date(
		char *disposal_date_string )
{
	return
	date_yyyy_mm_dd_new(
		disposal_date_string );
}

RECOVERY *recovery_evaluate(
		char *asset_name,
		char *serial_label,
		int tax_year,
		double cost_basis,
		char *service_placement_date_string,
		char *disposal_date_string,
		char *cost_recovery_class_year_string,
		char *cost_recovery_method,
		char *cost_recovery_convention,
		char *cost_recovery_system )
{
	RECOVERY *recovery;

	if ( !asset_name
	||   !serial_label
	||   !tax_year
	||   !cost_basis
	||   !service_placement_date_string
	||   !cost_recovery_class_year_string
	||   !cost_recovery_method
	||   !cost_recovery_convention
	||   !cost_recovery_system )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	recovery =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recovery_new(
			asset_name,
			serial_label,
			tax_year );

	recovery->cost_basis = cost_basis;

	if ( ! ( recovery->service_placement_date =
			recovery_service_placement_date(
				service_placement_date_string ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"service_placement_date=[%s] is invalid.",
			service_placement_date_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	recovery->service_placement_month =
		date_month(
			recovery->service_placement_date );

	recovery->service_placement_year =
		date_year(
			recovery->service_placement_date );

	if ( disposal_date_string && *disposal_date_string )
	{
		if ( ! ( recovery->disposal_date =
				recovery_disposal_date(
					disposal_date_string ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"disposal_placement_date=[%s] is invalid.",
				disposal_date_string );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		recovery->disposal_month =
			date_month(
				recovery->disposal_date );

		recovery->disposal_year =
			date_year(
				recovery->disposal_date );
	}

	recovery->year =
		recovery_year(
			tax_year,
			recovery->service_placement_year,
			recovery->disposal_year );

	if ( !recovery->year ) return recovery;

	recovery->class_year =
		recovery_class_year(
			cost_recovery_class_year_string );

	if ( string_strcmp(
		cost_recovery_method,
		RECOVERY_METHOD_STRAIGHT_LINE ) == 0 )
	{
		recovery->recovery_straight_line =
			recovery_straight_line_new(
				cost_basis,
				recovery->service_placement_month,
				recovery->disposal_month,
				cost_recovery_convention,
				cost_recovery_system,
				recovery->year,
				recovery->class_year );
	}
	else
	if ( strcmp(
		cost_recovery_method,
		RECOVERY_METHOD_ACCELERATED ) == 0 )
	{
		recovery->recovery_accelerated =
			recovery_accelerated_new(
				cost_basis,
				recovery->service_placement_month,
				recovery->disposal_month,
				cost_recovery_convention,
				cost_recovery_system,
				recovery->year,
				recovery->class_year );
	}

	recovery->rate =
		recovery_rate(
			recovery->recovery_straight_line,
			recovery->recovery_accelerated );

	recovery->amount =
		recovery_amount(
			recovery->recovery_straight_line,
			recovery->recovery_accelerated );

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
	if ( !asset_name
	||   !serial_label
	||   !tax_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	recovery_parse(
		string_pipe_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				RECOVERY_SELECT,
				RECOVERY_TABLE,
		 		/* --------------------- */
		 		/* Returns static memory */
		 		/* --------------------- */
				recovery_primary_where(
					asset_name,
					serial_label,
					tax_year ) ) ) );
}

LIST *recovery_system_list( char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *list;

	list = list_new();
	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list, 
			recovery_parse( input ) );
	}

	pclose( input_pipe );

	return list;
}

LIST *recovery_fetch_list(
			char *asset_name,
			char *serial_label )
{
	return
	recovery_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			RECOVERY_SELECT,
			RECOVERY_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			fixed_asset_purchase_primary_where(
				asset_name,
				serial_label ) ) );
}

FILE *recovery_insert_open( char *recovery_table )
{
	char system_string[ 1024 ];
	char *field;

	field =	"asset_name,"
		"serial_label,"
		"tax_year,"
		"recovery_rate,"
		"recovery_amount";

	sprintf( system_string,
		 "insert_statement table=%s field=%s delimiter='^'	|"
		 "tee_appaserver_error.sh				|"
		 "sql 2>&1						 ",
		 recovery_table,
		 field );

	return popen( system_string, "w" );
}

void recovery_insert(
		FILE *insert_open,
		char *asset_name,
		char *serial_label,
		int tax_year,
		double recovery_rate,
		double recovery_amount )
{
	if ( !insert_open
	||   !asset_name
	||   !serial_label
	||   !tax_year
	||   !recovery_rate
	||   !recovery_amount )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( insert_open ) pclose( insert_open );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(insert_open,
		"%s^%s^%d^%.5lf^%.2lf\n",
		asset_name,
		serial_label,
		tax_year,
		recovery_rate,
		recovery_amount );
}

void recovery_list_insert(
		char *recovery_table,
		LIST *recovery_list )
{
	RECOVERY *recovery;
	FILE *insert_open;

	if ( !list_rewind( recovery_list ) ) return;

	insert_open = recovery_insert_open( recovery_table );

	do {
		recovery =
			list_get(
				recovery_list );

		recovery_insert(
			insert_open,
			recovery->asset_name,
			recovery->serial_label,
			recovery->tax_year,
			recovery->rate,
			recovery->amount );

	} while ( list_next( recovery_list ) );

	pclose( insert_open );
}

double recovery_period_years( char *cost_recovery_period_string )
{
	return string_atof( cost_recovery_period_string );
}

int recovery_service_placement_years(
		int tax_year,
		int service_placement_year )
{
	return tax_year - service_placement_year + 1;
}

int recovery_prior_tax_year( char *recovery_table )
{
	static char *prior_tax_year = {0};

	if ( !prior_tax_year )
	{
		char system_string[ 1024 ];

		sprintf(system_string,
			"select.sh \"%s\" %s '' ''",
			"max( tax_year )",
			recovery_table );

		prior_tax_year = string_pipe_fetch( system_string );
	}

	return (prior_tax_year) ? atoi( prior_tax_year ) : 0;
}

FILE *recovery_delete_open( char *recovery_table )
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
		 recovery_table,
		 key,
		 SQL_DELIMITER );

	return popen( system_string, "w" );
}

char *recovery_subquery_where( int tax_year )
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
		RECOVERY_STRAIGHT_LINE *recovery_straight_line,
		RECOVERY_ACCELERATED *recovery_accelerated )
{
	if ( recovery_straight_line )
	{
		return recovery_straight_line->amount;
	}
	else
	if ( recovery_accelerated )
	{
		return recovery_accelerated->amount;
	}
	else
	{
		return 0.0;
	}
}

double recovery_rate(
		RECOVERY_STRAIGHT_LINE *recovery_straight_line,
		RECOVERY_ACCELERATED *recovery_accelerated )
{
	if ( recovery_straight_line )
	{
		return recovery_straight_line->rate;
	}
	else
	if ( recovery_accelerated )
	{
		return recovery_accelerated->rate;
	}
	else
	{
		return 0.0;
	}
}

FILE *recovery_update_open( char *recovery_table )
{
	char system_string[ 1024 ];
	char *key;

	key =	"asset_name,"
		"serial_label,"
		"tax_year";

	sprintf( system_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 recovery_table,
		 key );

	return popen( system_string, "w" );
}

void recovery_update(
		char *asset_name,
		char *serial_label,
		int tax_year,
		double recovery_rate,
		double recovery_amount )
{
	FILE *update_open;
	char *escape;

	if ( !asset_name
	||   !serial_label
	||   !tax_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_open = recovery_update_open( RECOVERY_TABLE );
	escape = fixed_asset_name_escape( asset_name );

	fprintf(update_open,
		"%s^%s^%d^recovery_rate^%.5lf\n",
		escape,
		serial_label,
		tax_year,
		recovery_rate );

	fprintf(update_open,
		"%s^%s^%d^recovery_amount^%.2lf\n",
		escape,
		serial_label,
		tax_year,
		recovery_amount );

	pclose( update_open );
}

double recovery_straight_line_amount(
		double cost_basis,
		double rate )
{
	return cost_basis * rate;
}

double recovery_accelerated_amount(
		double cost_basis,
		double rate )
{
	return cost_basis * rate;
}

int recovery_year(
		int tax_year,
		int service_placement_year,
		int disposal_year )
{
	int year;

	if ( disposal_year )
	{
		if ( tax_year > disposal_year )
		{
			year = 0;
		}
		else
		{
			year =
				tax_year -
				disposal_year +
				1;
		}
	}
	else
	{
		year =
			tax_year -
			service_placement_year +
			1;
	}

	return year;
}

double recovery_class_year( char *cost_recovery_class_year_string )
{
	return atof( cost_recovery_class_year_string );
}

RECOVERY_STRAIGHT_LINE *
	recovery_straight_line_new(
		double cost_basis,
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_convention,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year )
{
	RECOVERY_STRAIGHT_LINE *recovery_straight_line;

	if ( !cost_basis
	||   !service_placement_month
	||   !disposal_month
	||   !cost_recovery_convention
	||   !cost_recovery_system
	||   !recovery_year
	||   !recovery_class_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	recovery_straight_line = recovery_straight_line_calloc();

	if ( string_strcmp(
		cost_recovery_convention,
		RECOVERY_CONVENTION_HALF_YEAR ) == 0 )
	{
		recovery_straight_line->rate =
			recovery_straight_line_half_year_rate(
				cost_recovery_system,
				recovery_year,
				recovery_class_year );
	}
	else
	if ( string_strcmp(
		cost_recovery_convention,
		RECOVERY_CONVENTION_MID_MONTH ) == 0 )
	{
		recovery_straight_line->rate =
			recovery_straight_line_mid_month_rate(
				service_placement_month,
				disposal_month,
				cost_recovery_system,
				recovery_year,
				recovery_class_year );
	}
	else
	if ( string_strcmp(
		cost_recovery_convention,
		RECOVERY_CONVENTION_MID_QUARTER ) == 0 )
	{
		recovery_straight_line->rate =
			recovery_straight_line_mid_quarter_rate(
				service_placement_month,
				disposal_month,
				cost_recovery_system,
				recovery_year,
				recovery_class_year );
	}

	recovery_straight_line->amount =
		recovery_straight_line_amount(
			cost_basis,
			recovery_straight_line->rate );

	return recovery_straight_line;
}

double recovery_straight_line_half_year_rate(
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year )
{
	if ( !cost_recovery_system
	||   !recovery_year
	||   !recovery_class_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return 0.0;
}

double recovery_straight_line_mid_month_rate(
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year )
{
	if ( !service_placement_month
	||   !disposal_month
	||   !cost_recovery_system
	||   !recovery_year
	||   !recovery_class_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return 0.0;
}

double recovery_straight_line_mid_quarter_rate(
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year )
{
	if ( !service_placement_month
	||   !disposal_month
	||   !cost_recovery_system
	||   !recovery_year
	||   !recovery_class_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return 0.0;
}

RECOVERY_ACCELERATED *
	recovery_accelerated_new(
		double cost_basis,
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_convention,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year )
{
	RECOVERY_ACCELERATED *recovery_accelerated;

	if ( !cost_basis
	||   !service_placement_month
	||   !disposal_month
	||   !cost_recovery_convention
	||   !cost_recovery_system
	||   !recovery_year
	||   !recovery_class_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	recovery_accelerated = recovery_accelerated_calloc();

	if ( string_strcmp(
		cost_recovery_convention,
		RECOVERY_CONVENTION_HALF_YEAR ) == 0 )
	{
		recovery_accelerated->rate =
			recovery_accelerated_half_year_rate(
				cost_recovery_system,
				recovery_year,
				recovery_class_year );
	}
	else
	if ( string_strcmp(
		cost_recovery_convention,
		RECOVERY_CONVENTION_MID_MONTH ) == 0 )
	{
		recovery_accelerated->rate =
			recovery_accelerated_mid_month_rate(
				service_placement_month,
				disposal_month,
				cost_recovery_system,
				recovery_year,
				recovery_class_year );
	}
	else
	if ( string_strcmp(
		cost_recovery_convention,
		RECOVERY_CONVENTION_MID_QUARTER ) == 0 )
	{
		recovery_accelerated->rate =
			recovery_accelerated_mid_quarter_rate(
				service_placement_month,
				disposal_month,
				cost_recovery_system,
				recovery_year,
				recovery_class_year );
	}

	recovery_accelerated->amount =
		recovery_accelerated_amount(
			cost_basis,
			recovery_accelerated->rate );

	return recovery_accelerated;
}

double recovery_accelerated_half_year_rate(
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year )
{
	if ( !cost_recovery_system
	||   !recovery_year
	||   !recovery_class_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return 0.0;
}

double recovery_accelerated_mid_month_rate(
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year )
{
	if ( !service_placement_month
	||   !disposal_month
	||   !cost_recovery_system
	||   !recovery_year
	||   !recovery_class_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return 0.0;
}

double recovery_accelerated_mid_quarter_rate(
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year )
{
	if ( !service_placement_month
	||   !disposal_month
	||   !cost_recovery_system
	||   !recovery_year
	||   !recovery_class_year )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return 0.0;
}

