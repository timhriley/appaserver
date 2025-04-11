/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/exchange.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "exchange.h"

EXCHANGE_JOURNAL *exchange_journal_extract(
		const char *exchange_trntype_tag,
		LIST *list )
{
	char *get;
	char *date_posted;
	char *amount_string;
	char *name;
	char *memo;
	char *description;

	if ( !list )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	while ( !list_past_end( list ) )
	{
		list =
			/* ------------ */
			/* Returns list */
			/* ------------ */
			exchange_tag_seek_list(
				EXCHANGE_TRNTYPE_TAG,
				list );

		if ( !list_past_end( list ) )
		{
			list =
				/* ------------ */
				/* Returns list */
				/* ------------ */
				exchange_tag_seek_list(
					EXCHANGE_DTPOSTED_TAG,
					list );

			get = list_get( list );

			date_posted =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				exchange_journal_datum( get );

			list =
				/* ------------ */
				/* Returns list */
				/* ------------ */
				exchange_tag_seek_list(
					EXCHANGE_TRNAMT_TAG,
					list );

			get = list_get( list );

			amount_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				exchange_journal_datum( get );

			list =
				/* ------------ */
				/* Returns list */
				/* ------------ */
				exchange_tag_seek_list(
					EXCHANGE_NAME_TAG,
					list );

			get = list_get( list );

			description =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				exchange_journal_datum( get );

			return
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			exchange_journal_new(
				date_posted,
				amount_string,
				description );			
		}

		list_next( list );
	}
	return NULL;
}

EXCHANGE_JOURNAL *exchange_journal_new(
		char *date_posted,
		char *amount_string,
		char *description )
{
	EXCHANGE_JOURNAL *exchange_journal;

	if ( !date_posted
	||   !amount_string
	||   !description )
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

	exchange_journal = exchange_journal_calloc();

	exchange_journal->journal = journal_calloc();

	exchange_journal->journal->transaction_date_time =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		exchange_journal_transaction_date_time(
			date_posted );

	exchange_journal->journal->debit_amount =
		exchange_journal_debit_amount(
			amount_string );

	exchange_journal->journal->credit_amount =
		exchange_journal_credit_amount(
			amount_string );

	exchange_journal->description = description;

	return exchange_journal;
}

EXCHANGE_JOURNAL *exchange_journal_calloc( void )
{
	EXCHANGE_JOURNAL *exchange_journal;

	if ( ! ( exchange_journal = calloc( 1, sizeof ( EXCHANGE_JOURNAL ) ) ) )
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

	return exchange_journal;
}

boolean exchange_open_tag_boolean(
		const char *exchange_ofx_tag,
		LIST *list_stream_fetch )
{
}

char *exchange_journal_transaction_date_time( char *date_posted )
{
	char year[ 5 ];
	char month[ 3 ];
	char day[ 3 ];
	char hour[ 3 ];
	char minute[ 3 ];
	char second[ 3 ];
	char transaction_date_time[ 128 ];

	if ( !date_posted )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"date_posted is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strlen( posted_date ) < 14 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"posted_date is too short: %s.",
			posted_date );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	/* Sample input: 20250404120000[0:GMT]	*/
	/*               ^   ^ ^ ^ ^ ^		*/
	/* 		 |   | | | | |		*/
	/*               Y   M D H M S		*/
	/*               0123456789012		*/
	/* ------------------------------------	*/
	string_instr( year, date_posted, 0, 4 );
	string_instr( month, date_posted, 4, 2 );
	string_instr( day, date_posted, 6, 2 );
	string_instr( hour, date_posted, 8, 2 );
	string_instr( minute, date_posted, 10, 2 );
	string_instr( second, date_posted, 12, 2 );

	snprintf(
		transaction_date_time,
		sizeof( transaction_date_time ),
		"%s-%s-%s %s:%s:%s",
		year,
		month,
		day,
		hour,
		minute,
		second );

	return strdup( transaction_date_time );
}

double exchange_journal_debit_amount( char *amount_string )
{
	double amount =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		string_atof( amount_string );

	if ( float_virtually_same( amount, 0.0 ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"amount_string is zero: %s.",
			amount_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( amount < 0.0 )
	{
		return 0.0;
	}
	else
	{
		return amount;
	}
}

double exchange_journal_credit_amount( char *amount_string )
{
	double amount =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		string_atof( amount_string );

	if ( float_virtually_same( amount, 0.0 ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"amount_string is zero: %s.",
			amount_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( amount < 0.0 )
	{
		return -amount;
	}
	else
	{
		return 0.0;
	}
}

double exchange_journal_begin_amount(
		LIST *exchange_journal_list,
		double exchange_balance_amount )
{
	LIST *extract_list;
	JOURNAL *journal;

	extract_list =
		exchange_journal_extract_list(
			exchange_journal_list );

	if ( !list_length( extract_list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"exchange_journal_extract_list() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_propagate_previous_balance_set(
		extract_list
			/* journal_list in/out */,
		exchange_balance_amount
			/* end_balance */ );

	journal = list_first( extract_list );

	return journal->previous_balance;
}

LIST *exchange_journal_extract_list( LIST *exchange_journal_list )
{
	EXCHANGE_JOURNAL *exchange_journal;
	LIST *extract_list = list_new();

	if ( list_rewind( exchange_journal_list ) )
	do {
		exchange_journal = list_get( exchange_journal_list );

		list_set( extract_list, exchange_journal->journal;

	} while ( list_next( exchange_journal_list ) );

	if ( !list_length( extract_list ) )
	{
		list_free( extract_list );
		extract_list = NULL;
	}
	return extract_list;
}

EXCHANGE *exchange_parse( char *filespecification )
{
	EXCHANGE *exchange;
	FILE *input_file;
	EXCHANGE_JOURNAL *exchange_journal;

	if ( !filespecification || !*filespecification )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"filespecification is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	exchange = exchange_calloc();

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_file(
			filespecification );

	exchange->list_stream_fetch =
		list_stream_fetch(
			input_file );

	fclose( input_file );

	exchange->open_tag_boolean =
		exchange_open_tag_boolean(
			EXCHANGE_OFX_TAG,
			exchange->list_stream_fetch );

	if ( !exchange->open_tag_boolean )
	{
		return exchange;
	}

	exchange->financial_institution =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		exchange_financial_institution(
			EXCHANGE_ORG_TAG,
			exchange->list_stream_fetch );

	exchange->exchange_journal_list = list_new();

	(void)list_rewind( exchange->list_stream_fetch );

	while ( !list_past_end( exchange->list_stream_fetch ) )
	{
		exchange_journal =
			exchange_journal_extract(
				EXCHANGE_TRNTYPE_TAG,
				exchange->list_stream_fetch /* list */ );

		if ( exchange_journal )
		{
			list_set_order(
				exchange_journal_list,
				exchange_journal,
				exchange_journal_compare_function );
		}
	}

double exchange_balance_amount(
	const char *EXCHANGE_BALAMT_TAG,
	LIST *list_stream_fetch() );

double exchange_journal_begin_amount(
	exchange_journal_list,
	exchange_balance_amount() );


	return exchange;
}

EXCHANGE *exchange_calloc( void )
{
	EXCHANGE *exchange;

	if ( ! ( exchange = calloc( 1, sizeof ( EXCHANGE ) ) ) )
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

	return exchange;
}

char *exchange_financial_institution(
		const char *exchange_org_tag,
		LIST *list_stream_fetch )
{
}

double exchange_balance_amount(
		const char *exchange_balamt_tag,
		LIST *list_stream_fetch )
{
}

LIST *exchange_tag_seek_list(
		const char *tag,
		LIST *list )
{
	char *string;
	int instr;

	if ( !list )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	while ( !list_past_end( list ) )
	{
		string = list_get( list );

		instr =
			string_instr(
				tag /* substr */,
				string,
				1 /* occurrence */ );

		if ( instr != 0 ) return list;

		list_next( list );
	}
	return list;
}

char *exchange_journal_datum( char *list_get );
{
	char datum[ 128 ];

	if ( !list_get )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"list_get is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	(void )piece( datum, '>', list_get, 1 );

	if ( !*datum )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"piece(%s,'>') returned empty.",
			list_get );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return strdup( datum );
}

int exchange_journal_compare_function(
		EXCHANGE_JOURNAL *exchange_journal_from_list,
		EXCHANGE_JOURNAL *exchange_journal_compare )
{
	JOURNAL *journal_from_list = exchange_journal_from_list->journal;
	JOURNAL *journal_compare = exchange_journal_compare->journal;

	return
	string_strcmp(
		journal_from_list->transaction_date_time,
		journal_compare->transaction_date_time );
}

boolean exchange_open_tag_boolean(
		const char *exchange_ofx_tag,
		LIST *list )
{
	(void)list_rewind( list );

	list =
		/* ------------ */
		/* Returns list */
		/* ------------ */
		exchange_tag_seek_list(
			exchange_ofx_tag,
			list );

	return !list_past_end( list );
}

