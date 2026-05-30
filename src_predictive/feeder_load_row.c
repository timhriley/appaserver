/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_load_row.c			*/
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
#include "appaserver.h"
#include "column.h"
#include "float.h"
#include "sql.h"
#include "sed.h"
#include "environ.h"
#include "process.h"
#include "application_constant.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "date_convert.h"
#include "exchange.h"
#include "feeder_load_row.h"

char *feeder_load_row_trim_date( char *description_space_trim )
{
	static char sans_bank_date_description[ 256 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	regular_expression = "[ ][0-9][0-9]/[0-9][0-9]$";
	replace = "";

	sed = sed_new( regular_expression, replace );

	string_strcpy(	sans_bank_date_description,
			description_space_trim,
			512 );

	/* Why do I need to append the EOL character for sed() to work? */
	/* ------------------------------------------------------------ */
	sprintf( sans_bank_date_description +
		 strlen( sans_bank_date_description ),
		 "$" );

	if ( sed_will_replace( sans_bank_date_description, sed ) )
	{
		sed_search_replace( sans_bank_date_description, sed );
	}
	else
	{
		/* ---------------------- */
		/* Trim the EOL character */
		/* ---------------------- */

		/* Returns buffer with end shortened (maybe) */
		/* ----------------------------------------- */
		string_trim_right( sans_bank_date_description, 1 );
	}

	sed_free( sed );

	return
	/* ----------------------------------------- */
	/* Returns buffer with end shortened (maybe) */
	/* ----------------------------------------- */
	string_rtrim( sans_bank_date_description );
}

FEEDER_LOAD_ROW *feeder_load_row_new(
		double exchange_journal_amount,
		char *exchange_journal_description,
		char *transaction_date_time,
		double debit_amount,
		double credit_amount,
		double journal_balance )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( float_virtually_same( exchange_journal_amount, 0.0 )
	||   !exchange_journal_description
	||   !transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_load_row = feeder_load_row_calloc();

	feeder_load_row->exchange_journal_amount = exchange_journal_amount;
	feeder_load_row->transaction_date_time = transaction_date_time;
	feeder_load_row->debit_amount = debit_amount;
	feeder_load_row->credit_amount = credit_amount;
	feeder_load_row->journal_balance = journal_balance;

	feeder_load_row->international_date =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		feeder_load_row_international_date(
			transaction_date_time );

	feeder_load_row->description_space_trim =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_row_description_space_trim(
			exchange_journal_description );

	feeder_load_row->check_number =
		feeder_load_row_check_number(
			feeder_load_row->description_space_trim );

	feeder_load_row->description_embedded =
		/* --------------------------------------------- */
		/* Returns heap memory or description_space_trim */
		/* --------------------------------------------- */
		feeder_load_row_description_embedded(
			feeder_load_row->journal_balance,
			feeder_load_row->description_space_trim,
			feeder_load_row->check_number );

	return feeder_load_row;
}

FEEDER_LOAD_ROW *feeder_load_row_calloc( void )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( ! ( feeder_load_row =
			calloc( 1, sizeof ( FEEDER_LOAD_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_load_row;
}

char *feeder_load_row_description_embedded(
		double journal_balance,
		char *description_space_trim,
		int check_number )
{
	if ( check_number ) return description_space_trim;

	return
	/* ----------------------------------------- */
	/* Returns feeder_load_row_description_build */
	/* ----------------------------------------- */
	feeder_load_row_description_crop(
		FEEDER_LOAD_ROW_DESCRIPTION_SIZE,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_row_description_build(
			journal_balance,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_load_row_trim_date(
				description_space_trim ) ) );
}

char *feeder_load_row_description_build(
		double journal_balance,
		char *feeder_load_row_trim_date )
{
	char build[ 1024 ];

	if ( !feeder_load_row_trim_date )
	{
		char message[ 128 ];

		sprintf(message, "feeder_load_row_trim_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		build,
		sizeof ( build ),
		"%s %.2lf",
		feeder_load_row_trim_date,
		journal_balance );

	return strdup( build );
}

char *feeder_load_row_description_crop(
		const int description_size,
		char *description_build )
{
	if ( string_strlen( description_build ) > description_size )
	{
		*( description_build + description_size ) = '\0';
	}

	return description_build;
}

char *feeder_load_row_international_date( char *transaction_date_time )
{
	char international_date[ 128 ];

	if ( !transaction_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"transaction_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	column( international_date, 0, transaction_date_time );

	return strdup( international_date );
}

FILE *feeder_load_file_input_open( char *feeder_load_file_filename )
{
	FILE *input_open;

	if ( !feeder_load_file_filename )
	{
		char message[ 128 ];

		sprintf(message, "feeder_load_file_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( input_open = fopen( feeder_load_file_filename, "r" ) ) )
	{
		char message[ 256 ];

		sprintf(message,
			"fopen(%s) returned empty.",
			feeder_load_file_filename );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return input_open;
}

int feeder_load_row_position_check_number(
		char *description_space_trim,
		int position,
		int strlen_substr )
{
	if ( position > -1 )
	{
		return
		atoi( description_space_trim + position + strlen_substr );
	}
	else
	{
		return 0;
	}
}

int feeder_load_row_pound_colon_number( char *description_space_trim )
{
	char *substr = "#:";
	int position;

	position =
		/* ------------------------ */
		/* Returns -1 if not found. */
		/* ------------------------ */
		string_instr( 
			substr,
			description_space_trim /* string */,
			1 /* occurrence */ );

	return
	feeder_load_row_position_check_number(
		description_space_trim,
		position,
		strlen( substr ) );
}

int feeder_load_row_pound_number( char *description_space_trim )
{
	char *substr;
	int position;

	substr = "Confirmation#";

	position =
		/* ------------------------ */
		/* Returns -1 if not found. */
		/* ------------------------ */
		string_instr( 
			substr,
			description_space_trim /* string */,
			1 /* occurrence */ );

	if ( position > -1 ) return 0;

	substr = "#";

	position =
		/* ------------------------ */
		/* Returns -1 if not found. */
		/* ------------------------ */
		string_instr( 
			substr,
			description_space_trim /* string */,
			1 /* occurrence */ );

	if ( position < 0 ) return 0;

	if ( !feeder_load_row_check_number_boolean(
		description_space_trim + position + 1
			/* check_number_string */ ) )
	{
		return 0;
	}

	return
	feeder_load_row_position_check_number(
		description_space_trim,
		position,
		strlen( substr ) /* strlen_substr */ );
}

int feeder_load_row_check_text_number( char *description_space_trim )
{
	char *substr;
	int strlen_substr;
	int position;
	int check_number;

	substr = "check";
	strlen_substr = strlen( substr );

	position =
		/* ------------------------ */
		/* Returns -1 if not found. */
		/* ------------------------ */
		string_instr( 
			substr,
			description_space_trim /* string */,
			1 /* occurrence */ );

	if ( ( check_number =
		feeder_load_row_position_check_number(
			description_space_trim,
			position,
			strlen_substr ) ) )
	{
		return check_number;
	}

	substr = "CHECK";

	position =
		/* ------------------------ */
		/* Returns -1 if not found. */
		/* ------------------------ */
		string_instr( 
			substr,
			description_space_trim /* string */,
			1 /* occurrence */ );

	return
	feeder_load_row_position_check_number(
		description_space_trim,
		position,
		strlen_substr );
}

int feeder_load_row_check_number( char *description_space_trim )
{
	int check_number;
       
	if ( !description_space_trim )
	{
		char message[ 128 ];

		sprintf(message, "description_space_trim is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ( check_number = feeder_load_row_pound_colon_number(
		description_space_trim ) ) )
	{
		return check_number;
	}

	if ( ( check_number = feeder_load_row_pound_number(
		description_space_trim ) ) )
	{
		return check_number;
	}

	return
	feeder_load_row_check_text_number(
		description_space_trim );
}

void feeder_load_row_file_row_balance_set(
		double exchange_balance_amount,
		LIST *feeder_load_row_list )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( list_tail( feeder_load_row_list ) )
	do {
		feeder_load_row =
			list_get(
				feeder_load_row_list );

		feeder_load_row->file_row_balance =
			/* ------------------------------- */
			/* Returns exchange_balance_amount */
			/* ------------------------------- */
			feeder_load_row_file_row_balance(
				exchange_balance_amount );

		exchange_balance_amount -=
			feeder_load_row->
				exchange_journal_amount;

	} while ( list_previous( feeder_load_row_list ) );
}

LIST *feeder_load_row_list( LIST *exchange_journal_list )
{
	LIST *list = list_new();
	EXCHANGE_JOURNAL *exchange_journal;
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( list_rewind( exchange_journal_list ) )
	do {
		exchange_journal = list_get( exchange_journal_list );

		feeder_load_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_load_row_new(
				exchange_journal->amount,
				exchange_journal->description,
				exchange_journal->
					journal->
					transaction_date_time,
				exchange_journal->
					journal->
					debit_amount,
				exchange_journal->
					journal->
					credit_amount,
				exchange_journal->
					journal->
					balance );

		list_set( list, feeder_load_row );

	} while ( list_next( exchange_journal_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

void feeder_load_row_list_raw_display(
		FILE *stream,
		LIST *feeder_load_row_list )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	char *display;

	if ( !stream )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: stream is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( feeder_load_row_list ) )
	do {
		feeder_load_row = list_get( feeder_load_row_list );

		display =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_row_raw_display(
				feeder_load_row );

		fprintf(
			stream,
			"%s\n",
			display );

		free( display );

	} while ( list_next( feeder_load_row_list ) );
}

char *feeder_load_row_raw_display( FEEDER_LOAD_ROW *feeder_load_row )
{
	char display[ 2048 ];
	char *ptr = display;

	if ( !feeder_load_row )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_load_row is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"description_embedded=%s; ",
		feeder_load_row->description_embedded );

	ptr += sprintf(
		ptr,
		"exchange_journal_amount=%.2lf; ",
		feeder_load_row->exchange_journal_amount );

	ptr += sprintf(
		ptr,
		"transaction_date_time=%s; ",
		feeder_load_row->transaction_date_time );

	ptr += sprintf(
		ptr,
		"international_date=%s; ",
		feeder_load_row->international_date );

	ptr += sprintf(
		ptr,
		"check_number=%d; ",
		feeder_load_row->check_number );

	ptr += sprintf(
		ptr,
		"debit_amount=%.2lf; ",
		feeder_load_row->debit_amount );

	ptr += sprintf(
		ptr,
		"credit_amount=%.2lf; ",
		feeder_load_row->credit_amount );

	ptr += sprintf(
		ptr,
		"journal_balance=%.2lf; ",
		feeder_load_row->journal_balance );

	ptr += sprintf(
		ptr,
		"file_row_balance=%.2lf",
		feeder_load_row->file_row_balance );

	return strdup( display );
}

double feeder_load_row_file_row_balance( double exchange_balance_amount )
{
	return exchange_balance_amount;
}

char *feeder_load_row_description_space_trim(
		char *exchange_journal_description )
{
	if ( !exchange_journal_description )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"exchange_journal_description is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	strdup(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sed_trim_double_spaces(
			exchange_journal_description ) );
}

boolean feeder_load_row_check_number_boolean( char *check_number_string )
{
	char *ptr = check_number_string;
	boolean got_check_boolean = 0;

	while ( *ptr )
	{
		if ( *ptr >= '1' && *ptr <= '9' )
		{
			got_check_boolean = 1;
			ptr++;
			continue;
		}

		if ( !got_check_boolean )
		{
			if ( *ptr == '0'
			||   !isdigit( *ptr ) )
			{
				return 0;
			}
		}

		if ( *ptr == ' ' ) return got_check_boolean;

		if ( got_check_boolean )
		{
			if ( !isdigit( *ptr ) )
			{
				return 0;
			}
		}

		ptr++;
	}

	return got_check_boolean;
}

