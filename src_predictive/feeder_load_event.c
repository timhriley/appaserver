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
#include "optional_column.h"
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
		const char *feeder_load_event_insert,
		const char *feeder_load_event_table,
		char *feeder_account_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time,
		char *full_name,
		char *contact_key,
		boolean contact_key_boolean )
{
	char *system_string;
	char *insert_string;
	FILE *output_pipe;

	if ( !feeder_account_name
	||   !feeder_load_filename
	||   !feeder_row_account_end_date
	||   !feeder_load_date_time
	||   !full_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_event_insert_system_string(
			feeder_load_event_insert,
			feeder_load_event_table,
			ENTITY_CONTACT_KEY_COLUMN,
			SQL_DELIMITER,
			contact_key_boolean );

	output_pipe =
		appaserver_output_pipe(
			system_string );

	free( system_string );

	insert_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_event_insert_string(
			SQL_DELIMITER,
			feeder_account_name,
			feeder_load_filename,
			feeder_row_account_end_date,
			feeder_row_account_end_balance,
			feeder_load_date_time,
			full_name,
			contact_key,
			contact_key_boolean );

	fprintf(output_pipe,
		"%s\n",
		insert_string );

	free( insert_string );
	pclose( output_pipe );
}

char *feeder_load_event_insert_system_string(
		const char *feeder_load_event_insert,
		const char *feeder_load_event_table,
		const char *entity_contact_key_column,
		const char sql_delimiter,
		boolean contact_key_boolean )
{
	char *column_string;
	char system_string[ 1024 ];

	column_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_insert_column_string(
			feeder_load_event_insert /* ENTITY_INSERT */,
			entity_contact_key_column,
			contact_key_boolean );

	snprintf(
		system_string,
		sizeof ( system_string ),
	 	"insert_statement table=%s field=%s del='%c' 		  |"
	 	"sql 2>&1						  |"
		"grep -vi duplicate					  |"
	 	"html_paragraph_wrapper.e				   ",
	 	feeder_load_event_table,
	 	column_string,
	 	sql_delimiter );

	return strdup( system_string );
}

char *feeder_load_event_insert_string(
		const char sql_delimiter,
		char *feeder_account_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time,
		char *full_name,
		char *contact_key,
		boolean contact_key_boolean )
{
	char insert_string[ 1024 ];
	OPTIONAL_COLUMN *optional_column;

	if ( !feeder_account_name
	||   !feeder_load_filename
	||   !feeder_row_account_end_date
	||   !feeder_load_date_time
	||   !full_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See FEEDER_LOAD_EVENT_INSERT */
	/* ---------------------------- */
	snprintf(
		insert_string,
		sizeof ( insert_string ),
		"%s^%s^%s^%.2lf^%s^%s\n",
		feeder_account_name,
		feeder_load_filename,
		feeder_row_account_end_date,
		feeder_row_account_end_balance,
	 	feeder_load_date_time,
		full_name );

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			sql_delimiter,
			insert_string /* base_string */,
			contact_key /* component column or datum */,
			1 /* not escape_boolean */,
			contact_key_boolean /* set_boolean */ );

	return optional_column->return_string /* heap memory */;
}

FEEDER_LOAD_EVENT *feeder_load_event_fetch(
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	boolean contact_key_boolean;
	char *system_string;
	char *input;

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

	contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_event_system_string(
			FEEDER_LOAD_EVENT_SELECT,
			FEEDER_LOAD_EVENT_TABLE,
			ENTITY_CONTACT_KEY_COLUMN,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_load_event_primary_where(
				feeder_account_name,
				feeder_load_date_time ),
			contact_key_boolean );

	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	free( system_string );

	feeder_load_event =
		feeder_load_event_parse(
			contact_key_boolean,
			input );

	free( input );

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
		const char *entity_contact_key_column,
		char *feeder_load_event_primary_where,
		boolean contact_key_boolean )
{
	char *select_string;
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

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_event_select_string(
			feeder_load_event_select,
			entity_contact_key_column,
			contact_key_boolean );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\"",
		select_string,
		feeder_load_event_table,
		feeder_load_event_primary_where );

	free( select_string );

	return strdup( system_string );
}

FEEDER_LOAD_EVENT *feeder_load_event_parse(
		boolean contact_key_boolean,
		char *input )
{
	char buffer[ 128 ];
	char full_name[ 128 ];
	char *contact_key = {0};
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !input || !*input ) return NULL;

	feeder_load_event = feeder_load_event_calloc();

	/* See feeder_load_event_select_string() */
	/* ------------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_load_event->feeder_account_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_load_event->feeder_load_date_time = strdup( buffer );

	piece( full_name, SQL_DELIMITER, input, 2 );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		feeder_load_event->exchange_format_filename =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
	{
		feeder_load_event->feeder_row_account_end_date =
			strdup( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
	{
		feeder_load_event->feeder_row_account_end_balance =
			atof( buffer );
	}

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 6 );
		contact_key = strdup( buffer );
	}

	if ( *full_name )
	{
		feeder_load_event->appaserver_user =
			appaserver_user_new(
				strdup( full_name ) );

		feeder_load_event->appaserver_user->contact_key = contact_key;
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
		char *feeder_account_name )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	double prior_account_end_balance = 0.0;

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

	return prior_account_end_balance;
}

double feeder_load_event_error_double(
		double exchange_journal_begin_amount,
		LIST *feeder_row_list,
		FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch )
{
	double exist_sum;
	double calculate_begin_amount;
	double error_double;

	/* If first time run, then this is the initial exchange file. */
	/* ---------------------------------------------------------- */
	if ( !feeder_load_event_latest_fetch ) return 0.0;

	if ( !list_length( feeder_row_list ) ) return 0.0;


	exist_sum = feeder_row_exist_sum( feeder_row_list );

	calculate_begin_amount =
		feeder_load_event_calculate_begin_amount(
			feeder_load_event_latest_fetch->
				feeder_row_account_end_balance,
			exist_sum );

	if ( float_money_virtually_same(
		exchange_journal_begin_amount,
		calculate_begin_amount ) )
	{
		return 0.0;
	}

	error_double =
		calculate_begin_amount -
		exchange_journal_begin_amount;

#ifdef DEBUG_MODE
{
char message[ 65536 ];

/* feeder_row_list_raw_display( stderr, feeder_row_list ); */

snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: exchange_journal_begin_amount=%.2lf; exist_sum=%.2lf; feeder_load_event->end_balance=%.2lf; calculate_begin_amount=%.2lf; error_double=%.2lf\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	exchange_journal_begin_amount,
	exist_sum,
	feeder_load_event_latest_fetch->
		feeder_row_account_end_balance,
	calculate_begin_amount,
	error_double );
msg( (char *)0, message );
}
#endif

	return error_double;
}

double feeder_load_event_calculate_begin_amount(
		double feeder_row_account_end_balance,
		double feeder_row_exist_sum )
{
	return
	feeder_row_account_end_balance -
	feeder_row_exist_sum;
}

char *feeder_load_event_select_string(
		const char *feeder_load_event_select,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			(char *)feeder_load_event_select /* base_string */,
			(char *)entity_contact_key_column
				/* component column or datum */,
			0 /* not escape_boolean */,
			entity_contact_key_boolean /* set_boolean */ );

	return optional_column->return_string /* heap memory */;
}
