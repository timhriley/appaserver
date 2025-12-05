/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/exchange_csv.c			*/
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
#include "sql.h"
#include "date_convert.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "exchange_csv.h"

EXCHANGE_CSV *exchange_csv_fetch(
		char *application_name,
		char *csv_format_filename,
		int date_column /* one_based */,
		int description_column /* one_based */,
		int debit_column /* one_based */,
		int credit_column /* one_based */,
		int balance_column /* one_based */,
		int reference_column /* one_based */,
		boolean reverse_order_boolean,
		double balance_amount /* optional */,
		char *upload_directory )
{
	EXCHANGE_CSV *exchange_csv;
	FILE *input_file;
	char input[ 1024 ];
	EXCHANGE_CSV_JOURNAL *exchange_csv_journal;

	exchange_csv = exchange_csv_calloc();

	exchange_csv->exchange_csv_journal_list = list_new();

	exchange_csv->exchange_filespecification =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		exchange_filespecification(
			application_name,
			csv_format_filename,
			upload_directory );

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_file(
			exchange_csv->exchange_filespecification );

	while ( string_input( input, input_file, sizeof ( input ) ) )
	{
		/* Why does the U.S. Govt. escape the quote? */
		/* ----------------------------------------- */
		(void)string_unescape_character(
			input /* destination */,
			input /* datum */,
			'"' /* character_to_unescape */ );

		exchange_csv_journal =
			exchange_csv_journal_new(
				date_column,
				description_column,
				debit_column,
				credit_column,
				balance_column,
				reference_column,
				balance_amount,
				input );

		if ( !exchange_csv_journal ) continue;

		if ( reverse_order_boolean )
		{
			list_set_head(
				exchange_csv->exchange_csv_journal_list,
				exchange_csv_journal );
		}
		else
		{
			list_set(
				exchange_csv->exchange_csv_journal_list,
				exchange_csv_journal );
		}
	}

	fclose( input_file );

	if ( !list_length( exchange_csv->exchange_csv_journal_list ) )
		return exchange_csv;

	exchange_csv->exchange_journal_list =
		exchange_csv_exchange_journal_list(
			exchange_csv->exchange_csv_journal_list );

/*
exchange_journal_list_raw_display( exchange_csv->exchange_journal_list );
*/

	exchange_csv->balance_double =
		exchange_csv_balance_double(
			balance_column,
			balance_amount
				/* optional */,
			list_last( exchange_csv->exchange_journal_list )
				/* last_exchange_journal */ );

	exchange_csv->exchange_journal_begin_amount =
		exchange_journal_begin_amount(
			exchange_csv->exchange_journal_list
			/* Sets journal->previous_balance and balance */,
			exchange_csv->balance_double );

	exchange_csv->exchange_minimum_date_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		exchange_minimum_date_string(
			list_first( exchange_csv->exchange_journal_list )
				/* first_exchange_journal */ );

	return exchange_csv;
}

EXCHANGE_CSV *exchange_csv_calloc( void )
{
	EXCHANGE_CSV *exchange_csv;

	if ( ! ( exchange_csv = calloc( 1, sizeof ( EXCHANGE_CSV ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return exchange_csv;
}

LIST *exchange_csv_exchange_journal_list( LIST *exchange_csv_journal_list )
{
	LIST *exchange_journal_list = list_new();
	EXCHANGE_CSV_JOURNAL *exchange_csv_journal;

	if ( list_rewind( exchange_csv_journal_list ) )
	do {
		exchange_csv_journal =
			list_get(
				exchange_csv_journal_list );

		list_set(
			exchange_journal_list,
			exchange_csv_journal->exchange_journal );

	} while ( list_next( exchange_csv_journal_list ) );

	if ( !list_length( exchange_journal_list ) )
	{
		list_free( exchange_journal_list );
		exchange_journal_list = NULL;
	}

	return exchange_journal_list;
}

double exchange_csv_balance_double(
		int balance_column,
		double balance_amount /* optional */,
		EXCHANGE_JOURNAL *last_exchange_journal )
{
	double balance_double;

	if ( !last_exchange_journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"last_exchange_journal is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( balance_column >= 1 )
		balance_double =
			last_exchange_journal->
				journal->
				balance;
	else
		balance_double = balance_amount;

	return balance_double;
}

EXCHANGE_CSV_JOURNAL *exchange_csv_journal_new(
		int date_column /* one based */,
		int description_column /* one based */,
		int debit_column /* one based */,
		int credit_column /* one based */,
		int balance_column /* one based */,
		int reference_column /* one based */,
		double balance_amount /* optional */,
		char *input )
{
	EXCHANGE_CSV_JOURNAL *exchange_csv_journal;

	exchange_csv_journal = exchange_csv_journal_calloc();

	exchange_csv_journal->american_date =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		exchange_csv_journal_american_date(
			date_column,
			input );

	exchange_csv_journal->international_date =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		exchange_csv_journal_international_date(
			exchange_csv_journal->american_date );

	if ( !exchange_csv_journal->international_date )
	{
		free( exchange_csv_journal->american_date );

		return NULL;
	}

	exchange_csv_journal->description =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		exchange_csv_journal_description(
			description_column,
			input );

	if ( !exchange_csv_journal->description )
	{
		free( exchange_csv_journal->american_date );
		free( exchange_csv_journal->international_date );

		return NULL;
	}

	exchange_csv_journal->amount_double =
		exchange_csv_journal_amount_double(
			debit_column,
			credit_column,
			input );

	if ( !exchange_csv_journal->amount_double )
	{
		free( exchange_csv_journal->american_date );
		free( exchange_csv_journal->international_date );
		free( exchange_csv_journal->description );

		return NULL;
	}

	exchange_csv_journal->amount_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		exchange_csv_journal_amount_string(
			exchange_csv_journal->amount_double );

	exchange_csv_journal->balance_double =
		exchange_csv_journal_balance_double(
			balance_column,
			balance_amount,
			input );

	if ( reference_column >= 1 )
	{
		exchange_csv_journal->reference =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			exchange_csv_journal_reference(
				reference_column,
				input );
	}

	exchange_csv_journal->description_reference =
		/* ------------------------------------------------------- */
		/* Returns exchange_csv_journal_description or heap memory */
		/* ------------------------------------------------------- */
		exchange_csv_journal_description_reference(
			exchange_csv_journal->description,
			exchange_csv_journal->reference );

	exchange_csv_journal->exchange_journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		exchange_csv_journal_resolve(
			exchange_csv_journal->international_date,
			exchange_csv_journal->amount_string,
			exchange_csv_journal->balance_double,
			exchange_csv_journal->description_reference );

	return exchange_csv_journal;
}

EXCHANGE_CSV_JOURNAL *exchange_csv_journal_calloc( void )
{
	EXCHANGE_CSV_JOURNAL *exchange_csv_journal;

	if ( ! ( exchange_csv_journal =
			calloc( 1,
				sizeof ( EXCHANGE_CSV_JOURNAL ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return exchange_csv_journal;
}

char *exchange_csv_journal_american_date(
		int date_column,
		char *input )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	exchange_csv_journal_piece_string(
		date_column /* column_one_based */,
		input );
}

char *exchange_csv_journal_description(
		int description_column,
		char *input )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	exchange_csv_journal_piece_string(
		description_column /* column_one_based */,
		input );
}

char *exchange_csv_journal_reference(
		int reference_column,
		char *input )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	exchange_csv_journal_piece_string(
		reference_column /* column_one_based */,
		input );
}

char *exchange_csv_journal_description_reference(
		char *exchange_csv_journal_description,
		char *exchange_csv_journal_reference )
{
	if ( !exchange_csv_journal_description )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"exchange_csv_journal_description is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !exchange_csv_journal_reference )
	{
		return exchange_csv_journal_description;
	}
	else
	{
		char description_reference[ 1024 ];

		snprintf(
			description_reference,
			sizeof ( description_reference ),
			"%s %s",
			exchange_csv_journal_description,
			exchange_csv_journal_reference );

		return strdup( description_reference );
	}
}

EXCHANGE_JOURNAL *exchange_csv_journal_resolve(
		char *international_date,
		char *amount_string,
		double balance_double,
		char *description_reference )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	exchange_journal_new(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		exchange_csv_journal_date_posted(
			international_date ),
		amount_string,
		description_reference /* description */,
		balance_double );
}

char *exchange_csv_journal_international_date( char *american_date )
{
	char *international_date;

	if ( !american_date || !*american_date ) return (char *)0;

	international_date =
		/* ----------------------------------------------- */
		/* Returns heap memory, source_date_string or null */
		/* ----------------------------------------------- */
		date_convert_source_american(
			date_convert_international /* destination_enum */,
			american_date /* source_date_string */ );

	if ( !international_date )
		return (char *)0;
	else
	if ( international_date == american_date )
		return strdup( international_date );
	else
		return international_date;
}

char *exchange_csv_journal_piece_string(
		int column_one_based,
		char *input )
{
	char buffer[ 1024 ];
	char *strdup;

	strdup =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_strdup(
			/* ---------------------------- */
			/* Returns destination or null */
			/* ---------------------------- */
			piece_quote_comma(
				buffer /* destination */,
				input,
				column_one_based - 1 ) );

	return strdup;
}

double exchange_csv_journal_amount_double(
		int debit_column,
		int credit_column,
		char *input )
{
	double amount_double;
	char *piece_string;

	piece_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		exchange_csv_journal_piece_string(
			debit_column /* column_one_based */,
			input );

	if ( !credit_column )
	{
		amount_double = string_atof( piece_string );
	}
	else
	{
		double debit_double;
		double credit_double;

		debit_double = string_atof( piece_string );

		piece_string =
			exchange_csv_journal_piece_string(
				credit_column /* column_one_based */,
				input );

		credit_double = string_atof( piece_string );
		amount_double = debit_double - credit_double;
	}

	return amount_double;
}

char *exchange_csv_journal_amount_string( double amount_double )
{
	return
	strdup(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		float_string(
			amount_double,
			2 /* decimal_places */ ) );
}

double exchange_csv_journal_balance_double(
		int balance_column,
		double balance_amount,
		char *input )
{
	double balance_double;

	if ( !balance_column )
	{
		balance_double = balance_amount;
	}
	else
	{
		balance_double =
			string_atof(
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				exchange_csv_journal_piece_string(
					balance_column /* column_one_based */,
					input ) );
	}

	return balance_double;
}

char *exchange_csv_journal_date_posted( char *international_date )
{
	char date_posted[ 128 ];

	if ( !international_date )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"international_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strlen( international_date ) != 10 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"Expecting date length of 10 for [%s].",
			international_date );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy( date_posted, international_date );

	(void)string_trim_character(
		date_posted /* datum */,
		'-' /* character */ );

	strcat( date_posted, "000000" );

	return strdup( date_posted );
}

