/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/exchange.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "piece.h"
#include "column.h"
#include "float.h"
#include "exchange.h"

EXCHANGE_JOURNAL *exchange_journal_extract(
		LIST *list )
{
	char *get;
	char *date_posted;
	char *amount_string;
	char *name;
	char *memo = {0};
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

	while ( 1 )
	{
		list =
			/* ------------ */
			/* Returns list */
			/* ------------ */
			exchange_tag_seek_list(
				EXCHANGE_TRNTYPE_TAG,
				(char *)0 /* stop_tag */,
				list );

		if ( list_past_end( list ) ) return NULL;

		list =
			/* ------------ */
			/* Returns list */
			/* ------------ */
			exchange_tag_seek_list(
				EXCHANGE_DTPOSTED_TAG,
				(char *)0 /* stop_tag */,
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
				(char *)0 /* stop_tag */,
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
				(char *)0 /* stop_tag */,
				list );

		get = list_get( list );

		name =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			exchange_journal_datum( get );

		list =
			/* ------------ */
			/* Returns list */
			/* ------------ */
			exchange_tag_seek_list(
				EXCHANGE_MEMO_TAG,
				EXCHANGE_STMTTRN_END_TAG /* stop_tag */,
				list );

		get = list_get( list );

		if ( !exchange_tag_boolean(
			EXCHANGE_STMTTRN_END_TAG,
			get ) )
		{
			memo =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				exchange_journal_datum(
					get );
		}

		description =
			/* --------------------------- */
			/* Returns name or heap memory */
			/* --------------------------- */
			exchange_journal_description(
				name,
				memo );

		return
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		exchange_journal_new(
			date_posted,
			amount_string,
			description );			
	}

	/* Stub */
	/* ---- */
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

	exchange_journal->amount = string_atof( amount_string );
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

	if ( strlen( date_posted ) < 14 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"date_posted is too short: %s.",
			date_posted );

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
	string_substr( year, date_posted, 0, 4 );
	string_substr( month, date_posted, 4, 2 );
	string_substr( day, date_posted, 6, 2 );
	string_substr( hour, date_posted, 8, 2 );
	string_substr( minute, date_posted, 10, 2 );
	string_substr( second, date_posted, 12, 2 );

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

	journal_propagate_balance_set(
		extract_list
			/* journal_list in/out */,
		1 /* accumulate_debit */ );

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

		list_set( extract_list, exchange_journal->journal );

	} while ( list_next( exchange_journal_list ) );

	if ( !list_length( extract_list ) )
	{
		list_free( extract_list );
		extract_list = NULL;
	}
	return extract_list;
}

EXCHANGE *exchange_parse(
		char *application_name,
		char *exchange_format_filename,
		char *upload_directory )
{
	EXCHANGE *exchange;
	FILE *input_file;
	EXCHANGE_JOURNAL *exchange_journal;

	if ( !application_name
	||   !exchange_format_filename
	||   !upload_directory )
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


	exchange = exchange_calloc();

	exchange->exchange_format_filename = exchange_format_filename;

	exchange->filespecification =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		exchange_filespecification(
			application_name,
			exchange_format_filename,
			upload_directory );

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_file(
			exchange->filespecification );

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
				exchange->list_stream_fetch /* list */ );

		if ( exchange_journal )
		{
			list_set_order(
				exchange->exchange_journal_list,
				exchange_journal,
				exchange_journal_compare_function );
		}
	}

	exchange->balance_amount =
		exchange_balance_amount(
			EXCHANGE_BALAMT_TAG,
			exchange->list_stream_fetch /* list */ );

	exchange->exchange_journal_begin_amount =
		exchange_journal_begin_amount(
			exchange->exchange_journal_list /* in/out */,
			exchange->balance_amount );

	exchange->minimum_date_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		exchange_minimum_date_string(
			list_first(
				exchange->
					exchange_journal_list )
					/* first_exchange_journal */ );

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
		LIST *list )
{
	(void)list_rewind( list );

	list =
		/* ------------ */
		/* Returns list */
		/* ------------ */
		exchange_tag_seek_list(
			exchange_org_tag,
			(char *)0 /* stop_tag */,
			list );

	if ( list_past_end( list ) ) return NULL;

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	exchange_journal_datum(
		list_get( list ) );
}

double exchange_balance_amount(
		const char *exchange_balamt_tag,
		LIST *list )
{
	(void)list_rewind( list );

	list =
		/* ------------ */
		/* Returns list */
		/* ------------ */
		exchange_tag_seek_list(
			exchange_balamt_tag,
			(char *)0 /* stop_tag */,
			list );

	if ( list_past_end( list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"exchange_tag_seek_list(%s) returned empty.",
			exchange_balamt_tag );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	string_atof(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		exchange_journal_datum(
			(char *)list_get( list ) ) );
}

LIST *exchange_tag_seek_list(
		const char *tag,
		char *stop_tag,
		LIST *list )
{
	char *get;

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
		get = list_get( list );

		if ( stop_tag )
		{
			if ( exchange_tag_boolean(
				(const char *)stop_tag,
				get ) )
			{
				return list;
			}
		}

		if ( exchange_tag_boolean( tag, get ) ) return list;

		list_next( list );
	}
	return list;
}

char *exchange_journal_datum( char *list_get )
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
			(char *)0 /* stop_tag */,
			list );

	return !list_past_end( list );
}

boolean exchange_tag_boolean(
		const char *tag,
		char *list_get )
{
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

	if ( string_instr(
		(char *)tag /* substr */,
		list_get,
		1 /* occurrence */ ) > -1 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *exchange_journal_description(
		char *name,
		char *memo )
{
	char description[ 1024 ];

	if ( !name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	(void)string_search_replace(
		name /* source_destination */,
		"&amp;" /* search_string */,
		"&" /* replace_string */ );

	(void)string_trim_character(
		name /* source_destination */,
		'\\' );

	if ( !memo ) return name;

	if ( strcmp(
		name,
		memo ) == 0 )
	{
		return name;
	}

	snprintf(
		description,
		sizeof ( description ),
		"%s %s",
		name,
		memo );

	return strdup( description );
}

char *exchange_minimum_date_string( EXCHANGE_JOURNAL *first_exchange_journal )
{
	char *transaction_date_time;
	char minimum_date_string[ 128 ];

	if ( !first_exchange_journal
	||   !first_exchange_journal->journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"first_exchange_journal is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_date_time =
		first_exchange_journal->
			journal->
			transaction_date_time;

	(void)column( minimum_date_string, 0, transaction_date_time );

	return strdup( minimum_date_string );
}

char *exchange_filespecification(
		char *application_name,
		char *exchange_format_filename,
		char *upload_directory )
{
	static char filespecification[ 128 ];

	if ( !application_name
	||   !exchange_format_filename
	||   !upload_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		filespecification,
		sizeof ( filespecification ),
		"%s/%s/%s",
		upload_directory,
		application_name,
		exchange_format_filename );

	return filespecification;
}

