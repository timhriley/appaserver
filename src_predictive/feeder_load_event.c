/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_load_event.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "float.h"
#include "appaserver.h"
#include "sql.h"
#include "appaserver_error.h"
#include "journal.h"
#include "feeder.h"
#include "feeder_load_event.h"

FEEDER_LOAD_EVENT *feeder_load_event_new(
		char *login_name,
		char *feeder_account_name,
		char *exchange_format_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !login_name
	||   !feeder_account_name
	||   !exchange_format_filename
	||   !feeder_row_account_end_date
	||   !feeder_load_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_load_event = feeder_load_event_calloc();

	feeder_load_event->feeder_account_name = feeder_account_name;
	feeder_load_event->exchange_format_filename = exchange_format_filename;

	feeder_load_event->feeder_row_account_end_date =
		feeder_row_account_end_date;

	feeder_load_event->feeder_row_account_end_balance =
		feeder_row_account_end_balance;

	feeder_load_event->feeder_load_date_time = feeder_load_date_time;

	feeder_load_event->appaserver_user =
		appaserver_user_login_fetch(
			login_name,
			0 /* not fetch_role_name_list */ );

	if ( !feeder_load_event->appaserver_user )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"appaserver_user_login_fetch(%s) returned empty.",
			login_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_load_event;
}

FEEDER_LOAD_EVENT *feeder_load_event_calloc( void )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( ! ( feeder_load_event =
			calloc( 1, sizeof ( FEEDER_LOAD_EVENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_load_event;
}

void feeder_load_event_insert(
		const char *feeder_load_event_table,
		const char *feeder_load_event_insert,
		char *feeder_account_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time,
		char *full_name,
		char *street_address )
{
	FILE *output_pipe;
	char *tmp;

	if ( !feeder_account_name
	||   !feeder_load_filename
	||   !feeder_row_account_end_date
	||   !feeder_load_date_time
	||   !full_name
	||   !street_address )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_pipe =
		appaserver_output_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = feeder_load_event_insert_system_string(
				feeder_load_event_table,
				feeder_load_event_insert,
				SQL_DELIMITER ) ) );

	feeder_load_event_insert_pipe(
		SQL_DELIMITER,
		feeder_account_name,
		feeder_load_filename,
		feeder_row_account_end_date,
		feeder_row_account_end_balance,
		feeder_load_date_time,
		full_name,
		street_address,
		output_pipe );

	pclose( output_pipe );
	free( tmp );
}

char *feeder_load_event_insert_system_string(
		const char *feeder_load_event_table,
		const char *feeder_load_event_insert,
		const char sql_delimiter )
{
	char system_string[ 1024 ];

	sprintf(system_string,
	 	"insert_statement table=%s field=%s del='%c' 		  |"
	 	"sql 2>&1						  |"
		"grep -vi duplicate					  |"
	 	"html_paragraph_wrapper.e				   ",
	 	feeder_load_event_table,
	 	feeder_load_event_insert,
	 	sql_delimiter );

	return strdup( system_string );
}

void feeder_load_event_insert_pipe(
		const char sql_delimiter,
		char *feeder_account_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time,
		char *full_name,
		char *street_address,
		FILE *output_pipe )
{
	if ( !feeder_account_name
	||   !feeder_load_filename
	||   !feeder_row_account_end_date
	||   !feeder_load_date_time
	||   !full_name
	||   !street_address
	||   !output_pipe )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( output_pipe ) pclose( output_pipe );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See FEEDER_LOAD_EVENT_INSERT */
	/* ---------------------------- */
	fprintf(output_pipe,
		"%s%c%s%c%s%c%.2lf%c%s%c%s%c%s\n",
		feeder_account_name,
		sql_delimiter,
		feeder_load_filename,
		sql_delimiter,
		feeder_row_account_end_date,
		sql_delimiter,
		feeder_row_account_end_balance,
		sql_delimiter,
	 	feeder_load_date_time,
		sql_delimiter,
		full_name,
		sql_delimiter,
		street_address );
}

FEEDER_LOAD_EVENT *feeder_load_event_fetch(
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	FILE *input_open;
	char *tmp;
	char input[ 1024 ];

	if ( !feeder_account_name
	||   !feeder_load_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input_open =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = feeder_load_event_system_string(
				FEEDER_LOAD_EVENT_SELECT,
				FEEDER_LOAD_EVENT_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_load_event_primary_where(
					feeder_account_name,
					feeder_load_date_time ) ) ) );

	feeder_load_event =
		feeder_load_event_parse(
			string_input(
				input,
				input_open,
				1024 ) );

	pclose( input_open );

	free( tmp );

	if ( feeder_load_event && !feeder_load_event->appaserver_user )
	{
		feeder_load_event = NULL;
	}

	return feeder_load_event;
}

char *feeder_load_event_primary_where(
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	static char where[ 128 ];

	if ( !feeder_account_name
	||   !feeder_load_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"feeder_account = '%s' and "
		"feeder_load_date_time = '%s'",
		feeder_account_name,
		feeder_load_date_time );

	return where;
}

char *feeder_load_event_system_string(
		const char *feeder_load_event_select,
		const char *feeder_load_event_table,
		char *feeder_load_event_primary_where )
{
	char system_string[ 1024 ];

	if ( !feeder_load_event_primary_where )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_load_event_primary_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\"",
		feeder_load_event_select,
		feeder_load_event_table,
		feeder_load_event_primary_where );

	return strdup( system_string );
}

FEEDER_LOAD_EVENT *feeder_load_event_parse( char *input )
{
	char buffer[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !input || !*input ) return NULL;

	feeder_load_event = feeder_load_event_calloc();

	/* See FEEDER_LOAD_EVENT_SELECT */
	/* ---------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_load_event->feeder_account_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_load_event->feeder_load_date_time = strdup( buffer );

	piece( full_name, SQL_DELIMITER, input, 2 );
	piece( street_address, SQL_DELIMITER, input, 3 );

	if ( *full_name && *street_address )
	{
		feeder_load_event->appaserver_user =
			appaserver_user_new(
				strdup( full_name ),
				strdup( street_address ) );
	}

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		feeder_load_event->exchange_format_filename =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
	{
		feeder_load_event->feeder_row_account_end_date =
			strdup( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer )
	{
		feeder_load_event->feeder_row_account_end_balance =
			atof( buffer );
	}

	return feeder_load_event;
}

FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch(
		const char *feeder_load_event_table,
		char *feeder_account_name )
{
	char *latest_date_time;

	if ( ! ( latest_date_time =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_event_latest_date_time(
				feeder_load_event_latest_system_string(
					feeder_load_event_table,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					feeder_load_event_account_where(
						feeder_account_name ) ) ) ) )
	{
		return NULL;
	}

	return
	feeder_load_event_fetch(
		feeder_account_name,
		latest_date_time );
}

char *feeder_load_event_account_where( char *feeder_account_name )
{
	static char where[ 128 ];

	if ( !feeder_account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"feeder_account = '%s'",
		feeder_account_name );

	return where;
}

char *feeder_load_event_latest_system_string(
		const char *table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !where )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\"",
		"max( feeder_load_date_time )",
		table,
		where );

	return strdup( system_string );
}

char *feeder_load_event_latest_date_time(
		char *feeder_load_event_latest_system_string )
{
	if ( !feeder_load_event_latest_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_pipe_fetch(
		feeder_load_event_latest_system_string );
}

double feeder_load_event_prior_account_end_balance(
		const char *feeder_load_event_table,
		char *feeder_account_name,
		boolean accumulate_debit_boolean )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	double prior_account_end_balance = {0};

	if ( !feeder_account_name )
	{
		char message[ 128 ];

		sprintf(message, "feeder_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ( feeder_load_event =
		feeder_load_event_latest_fetch(
			feeder_load_event_table,
			feeder_account_name ) ) )
	{
		prior_account_end_balance =
			feeder_load_event->feeder_row_account_end_balance;
	}
	else
	{
		prior_account_end_balance =
			journal_first_account_balance(
				JOURNAL_TABLE,
				(char *)0 /* fund_name */,
				feeder_account_name /* account_name */ );

		if ( !accumulate_debit_boolean )
		{
			prior_account_end_balance =
				-prior_account_end_balance;
		}
	}

	return prior_account_end_balance;
}

boolean feeder_load_event_match_boolean(
		double exchange_journal_begin_amount,
		LIST *feeder_row_list,
		FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch )
{
	double exist_sum;
	double calculate_begin_amount;

	/* If first time run, then this is the initial exchange file. */
	/* ---------------------------------------------------------- */
	if ( !feeder_load_event_latest_fetch ) return 1;

	if ( !list_length( feeder_row_list ) ) return 0;


	exist_sum = feeder_row_exist_sum( feeder_row_list );

	calculate_begin_amount =
		feeder_load_event_calculate_begin_amount(
			feeder_load_event_latest_fetch->
				feeder_row_account_end_balance,
			exist_sum );

#ifdef DEBUG_MODE
{
char message[ 65536 ];

/* feeder_row_list_raw_display( stderr, feeder_row_list ); */

snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: exchange_journal_begin_amount=%.2lf; exist_sum=%.2lf; feeder_load_event->end_balance=%.2lf; calculate_begin_amount=%.2lf\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	exchange_journal_begin_amount,
	exist_sum,
	feeder_load_event_latest_fetch->
		feeder_row_account_end_balance,
	calculate_begin_amount );
msg( (char *)0, message );
}
#endif

	return
	float_money_virtually_same(
		exchange_journal_begin_amount,
		calculate_begin_amount );
}

double feeder_load_event_calculate_begin_amount(
		double feeder_row_account_end_balance,
		double feeder_row_exist_sum )
{
	return
	feeder_row_account_end_balance -
	feeder_row_exist_sum;
}

