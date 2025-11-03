/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and  available software. Visit appaserver.org		*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "column.h"
#include "date.h"
#include "date_convert.h"
#include "semaphore.h"
#include "sql.h"
#include "appaserver.h"
#include "update.h"
#include "list.h"
#include "environ.h"
#include "application.h"
#include "journal.h"
#include "account.h"
#include "entity.h"
#include "appaserver_error.h"
#include "predictive.h"
#include "float.h"
#include "transaction_date.h"
#include "journal_propagate.h"
#include "transaction.h"

TRANSACTION *transaction_calloc( void )
{
	TRANSACTION *transaction;

	if ( ! ( transaction = calloc( 1, sizeof ( TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return transaction;
}

TRANSACTION *transaction_new(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time )
{
	TRANSACTION *transaction;

	transaction = transaction_calloc();

	transaction->fund_name = fund_name;
	transaction->full_name = full_name;
	transaction->street_address = street_address;
	transaction->transaction_date_time = transaction_date_time;

	return transaction;
}

char *transaction_system_string(
		const char *transaction_select,
		const char *transaction_table,
		char *transaction_primary_where )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\" transaction_date_time",
		transaction_select,
		transaction_table,
		transaction_primary_where );

	return strdup( system_string );
}

TRANSACTION *transaction_fetch(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		boolean fetch_journal_list )
{
	if ( !transaction_date_time ) return NULL;

	return
	transaction_parse(
		fund_name,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_fetch(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			transaction_system_string(
				TRANSACTION_SELECT,
				TRANSACTION_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
	 			transaction_fetch_where(
					fund_name,
					full_name,
					street_address,
					transaction_date_time ) ) ),
		fetch_journal_list );
}

TRANSACTION *transaction_parse(
		char *fund_name,
		char *input,
		boolean fetch_journal_list )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	TRANSACTION *transaction;

	if ( !input || !*input ) return NULL;

	/* See TRANSACTION_SELECT */
	/* ---------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input, 2 );

	transaction =
		transaction_new(
			fund_name,
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	transaction->transaction_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	transaction->check_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer ) transaction->memo = strdup( piece_buffer );

	if ( fetch_journal_list )
	{
		transaction->journal_list =
			journal_system_list(
				fund_name,
				journal_system_string(
					JOURNAL_SELECT,
					JOURNAL_TABLE,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					transaction_primary_where(
						fund_name,
						full_name,
						street_address,
						transaction_date_time ) ),
				1 /* fetch_account */,
				1 /* fetch_subclassification */,
				1 /* fetch_element */,
				0 /* not fetch_transaction */ );
	}

	return transaction;
}

char *transaction_primary_where(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time )
{
	static char where[ 512 ];
	char *fund_where;
	char *escape_full_name;
	char *escape_street_address;
	char *escape_transaction_date_time;

	fund_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		/* If set, appends an "and" */
		/* ------------------------ */
		transaction_fund_where(
			TRANSACTION_TABLE,
			TRANSACTION_FUND_COLUMN,
			fund_name );

	escape_full_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name(
			full_name );

	escape_street_address =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_street_address(
			street_address );

	escape_transaction_date_time =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_escape_date_time(
			transaction_date_time );

	snprintf(
		where,
		sizeof ( where ),
		"%s"
		"full_name = '%s' and "
		"street_address = '%s' and "
		"transaction_date_time = '%s'",
		fund_where,
		escape_full_name,
		escape_street_address,
		escape_transaction_date_time );

	return where;
}

char *transaction_insert(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		double transaction_amount,
		int check_number,
		char *memo,
		LIST *journal_list,
		boolean insert_journal_list_boolean )
{
	boolean lock_column_boolean;
	FILE *pipe_open;
	char *race_free_date_time;
	char *column_list_string;

	if ( !full_name
	||   !street_address
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

	if ( transaction_amount <= 0.0 )
	{
		char message[ 1024 ];

		sprintf(message,
			"transaction_amount is zero or negative:\n"
			"%s, %s, %s, %.2lf\n",
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lock_column_boolean =
		transaction_lock_column_boolean(
			TRANSACTION_TABLE,
			TRANSACTION_LOCK_COLUMN );

	column_list_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_column_list_string(
			TRANSACTION_INSERT /* input_column_list_string */,
			TRANSACTION_TABLE,
			TRANSACTION_FUND_COLUMN,
			TRANSACTION_LOCK_COLUMN );

	race_free_date_time =
		transaction_race_free_date_time(
			transaction_date_time );

	pipe_open =
		transaction_insert_pipe_open(
			column_list_string,
			TRANSACTION_TABLE );

	transaction_insert_pipe(
		pipe_open,
		fund_name,
		full_name,
		street_address,
		race_free_date_time,
		transaction_amount,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_check_number( check_number ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_memo( memo ),
		lock_column_boolean );

	pclose( pipe_open );

	if ( insert_journal_list_boolean )
	{
		journal_list_insert(
			fund_name,
			full_name,
			street_address,
			race_free_date_time,
			journal_list );

		journal_propagate_account_list(
			fund_name,
			race_free_date_time,
			journal_extract_account_list(
				journal_list ) );
	}

	return race_free_date_time;
}

void transaction_insert_pipe(
		FILE *pipe_open,
		char *fund_name,
		char *full_name,
		char *street_address,
		char *race_free_date_time,
		double transaction_amount,
		char *transaction_check_number,
		char *transaction_memo,
		boolean lock_column_boolean )
{
	char *fund_datum;
	char *insert_data_string;

	if ( transaction_amount <= 0.0 )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: negative or zero transaction_amount = %.2lf.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			transaction_amount );
	}

	fund_datum =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_fund_datum(
			TRANSACTION_TABLE,
			TRANSACTION_FUND_COLUMN,
			fund_name );

	insert_data_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		transaction_insert_data_string(
			full_name,
			street_address,
			race_free_date_time,
			transaction_amount,
			transaction_check_number,
			transaction_memo,
			lock_column_boolean,
			fund_datum );

	if ( insert_data_string )
	{
		fprintf(pipe_open,
			"%s\n",
			insert_data_string );

		free( insert_data_string );
	}
}

char *transaction_check_number( int check_number )
{
	static char number[ 16 ];

	if ( check_number )
		snprintf( number, sizeof ( number ), "%d", check_number );
	else
		*number = '\0';

	return number;
}

char *transaction_memo( char *memo )
{
	static char return_memo[ 128 ];

	if ( !memo || !*memo || strcmp( memo, "memo" ) == 0 )
	{
		*return_memo = '\0';
	}
	else
	{
		string_strcpy(
			return_memo,
			memo,
			/* ------------ */
			/* Should be 60 */
			/* ------------ */
			TRANSACTION_MEMO_LENGTH + 1 );
	}

	return
	string_escape_quote( return_memo, return_memo );
}

char *transaction_race_free_date_time( char *transaction_date_time )
{
	key_t key = TRANSACTION_SEMAPHORE_KEY;
	SEMAPHORE *semaphore;
	DATE *next_transaction_date_time = {0};

	if ( ! ( semaphore = semaphore_new( key ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"semaphore_new(%d) returned empty.",
			key );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	semaphore_wait( semaphore->id );

	while ( transaction_date_time_boolean(
			TRANSACTION_TABLE,
			transaction_date_time ) )
	{
		if ( !next_transaction_date_time )
		{
			next_transaction_date_time =
				date_yyyy_mm_dd_hms_new(
					transaction_date_time );
		}

		date_increment_second(
			next_transaction_date_time,
			1 );

		transaction_date_time =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_display_yyyy_mm_dd_colon_hms(
				next_transaction_date_time );
	}

	semaphore_signal( semaphore->id );

	if ( next_transaction_date_time )
	{
		date_free( next_transaction_date_time );
	}

	return transaction_date_time;
}

char *transaction_date_time(
		char *transaction_date,
		char *transaction_time )
{
	static char date_time[ 128 ];

	sprintf(date_time,
		"%s %s",
		transaction_date,
		transaction_time );

	return date_time;
}

char *transaction_date_time_where(
		char *transaction_date_time_column,
		char *transaction_date,
		char *transaction_time )
{
	static char where[ 256 ];

	sprintf(where,
		"%s = '%s'",
		transaction_date_time_column,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time(
			transaction_date,
			transaction_time ) );

	return where;
}

char *transaction_closing_entry_where(
		char *transaction_date_time_where,
		char *transaction_closing_memo_where )
{
	static char where[ 512 ];

	sprintf(where,
		"%s and %s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time_where,
		transaction_closing_memo_where );

	return where;
}

void transaction_update(
		double transaction_amount,
		char *transaction_primary_where )
{
	char *system_string;

	if ( !transaction_primary_where )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_primary_where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* If deleted the transaction */
	/* -------------------------- */
	if ( float_money_virtually_same( transaction_amount, 0.0 ) ) return;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_update_statement_system_string(
			TRANSACTION_TABLE,
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			transaction_set_clause(
				TRANSACTION_AMOUNT_COLUMN,
				transaction_amount ),
			transaction_primary_where );

	if ( system( system_string ) ){}
}

char *transaction_set_clause(
		char *transaction_amount_column,
		double transaction_amount )
{
	static char set_clause[ 128 ];
	char *ptr = set_clause;

	if ( transaction_amount )
	{
		ptr += sprintf( ptr, "set " );

		ptr += sprintf(
			ptr,
			"%s = %.2lf",
			transaction_amount_column,
			float_abs( transaction_amount ) );
	}

	if ( ptr == set_clause )
		return (char *)0;
	else
		return set_clause;
}

char *transaction_update_statement_system_string(
		char *transaction_table,
		char *transaction_set_clause,
		char *transaction_primary_where )
{
	char update_statement_system_string[ 1024 ];

	if ( !transaction_table
	||   !transaction_set_clause
	||   !transaction_primary_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(update_statement_system_string,
		"echo \"update %s %s where %s;\" | sql",
		transaction_table,
		transaction_set_clause,
		transaction_primary_where );

	return strdup( update_statement_system_string );
}

void transaction_delete(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time )
{
	TRANSACTION *transaction;
	char *primary_where;
	char *system_string;

	if ( !full_name
	||   !street_address
	||   !transaction_date_time )
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

	if ( ! ( transaction =
			transaction_fetch(
				fund_name,
				full_name,
				street_address,
				transaction_date_time,
				1 /* fetch_journal_list */ ) ) )
	{
		return;
	}

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_primary_where(
			fund_name,
			full_name,
			street_address,
			transaction_date_time );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_delete_system_string(
			TRANSACTION_TABLE,
			primary_where );

	if ( system( system_string ) ){}

	free( system_string );

	system_string =
		journal_delete_system_string(
			JOURNAL_TABLE,
			primary_where );

	if ( system( system_string ) ){}

	free( system_string );

	journal_propagate_account_list(
		fund_name,
		transaction_date_time,
		journal_extract_account_list(
			transaction->journal_list )  );
}

char *transaction_delete_system_string(
		char *transaction_table,
		char *transaction_primary_where )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"delete from %s where %s;\" | "
	       	"tee_appaserver.sh | "
		"sql.e",
		transaction_table,
		transaction_primary_where );

	return strdup( system_string );
}

FILE *transaction_insert_pipe_open(
		char *column_list_string,
		const char *transaction_table )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"insert_statement t=%s f=%s delimiter='^'	|"
		"tee_appaserver.sh				|"
		"sql 2>&1					|"
		"html_paragraph_wrapper.e			 ",
		transaction_table,
		column_list_string );

	return popen( system_string, "w" );
}

FILE *transaction_input_pipe( char *transaction_system_string )
{
	return
	popen( transaction_system_string, "r" );
}

LIST *transaction_list(
		char *fund_name,
		char *where,
		boolean fetch_journal_list )
{
	LIST *list;
	char input[ 1024 ];
	char *system_string;
	FILE *input_pipe;

	if ( !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_system_string(
			TRANSACTION_SELECT,
			TRANSACTION_TABLE,
			where );

	input_pipe =
		transaction_input_pipe(
			system_string );

	list = list_new();

	while( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			transaction_parse(
				fund_name,
				input,
				fetch_journal_list ) );
	}

	pclose( input_pipe );

	return list;
}

void transaction_list_insert(
		LIST *transaction_list,
		boolean insert_journal_list_boolean )
{
	TRANSACTION *transaction;

	if ( list_rewind( transaction_list ) )
	do {
		transaction =
			list_get(
				transaction_list );

		if ( transaction->transaction_amount <= 0.0 )
		{
			char message[ 1024 ];

			sprintf(message,
				"transaction_amount is zero or negative:\n"
				"%s, %s, %s, %.2lf\n",
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		transaction->transaction_date_time =
			transaction_insert(
				transaction->fund_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->check_number,
				transaction->memo,
				transaction->journal_list,
				insert_journal_list_boolean );

	} while ( list_next( transaction_list ) );
}

TRANSACTION *transaction_entity_new(
		ENTITY *entity,
		char *transaction_date_time,
		double transaction_amount,
		int check_number,
		char *memo,
		LIST *journal_list )
{
	TRANSACTION *transaction;

	if ( !entity
	||   !transaction_date_time
	||   !transaction_amount
	||   !list_length( journal_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( transaction_amount < 0.0 )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: negative transaction_amount = %.2lf.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			transaction_amount );
	}

	transaction = transaction_calloc();

	transaction->full_name = entity->full_name;
	transaction->street_address = entity->street_address;
	transaction->transaction_date_time = transaction_date_time;
	transaction->transaction_amount = transaction_amount;
	transaction->check_number = check_number;
	transaction->memo = memo;
	transaction->journal_list = journal_list;

	return transaction;
}

void transaction_list_html_display( LIST *transaction_list )
{
	if ( list_rewind( transaction_list ) )
	do {
		transaction_html_display(
			(TRANSACTION *)
				list_get( transaction_list ) );

	} while ( list_next( transaction_list ) );
}

void transaction_html_display( TRANSACTION *transaction )
{
	if ( !transaction )
	{
		char message[ 128 ];

		sprintf(message, "transaction is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_list_html_display(
		transaction->journal_list,
		transaction->transaction_date_time,
		transaction->memo,
		transaction->full_name );
}

char *transaction_display( TRANSACTION *transaction )
{
	char display[ 1024 ];

	if ( !transaction )
	{
		char message[ 128 ];

		sprintf(message, "transaction is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		display,
		sizeof ( display ),
		"full_name=%s; "
		"transaction_date_time=%s; "
		"amount=%.2lf; "
		"memo=%s\n",
		transaction->full_name,
		transaction->transaction_date_time,
		transaction->transaction_amount,
		(transaction->memo) ? transaction->memo : "" );

	return strdup( display );
}

TRANSACTION *transaction_binary(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		double transaction_amount,
		char *memo,
		char *debit_account_name,
		char *credit_account_name )
{
	TRANSACTION *transaction;

	if ( !transaction_amount ) return NULL;

	transaction =
		transaction_new(
			(char *)0 /* fund_name */,
			full_name,
			street_address,
			transaction_date_time );

	transaction->memo = memo;
	transaction->transaction_amount = transaction_amount;

	transaction->journal_list =
		journal_binary_list(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			float_abs( transaction->transaction_amount ),
			account_fetch(
				debit_account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ),
			account_fetch(
				credit_account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) );

	return transaction;
}

LIST *transaction_list_extract_account_list( LIST *transaction_list )
{
	LIST *account_list;
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return (LIST *)0;

	account_list = list_new();

	do {
		transaction = list_get( transaction_list );

		if ( list_length( transaction->journal_list ) )
		{
			journal_account_list_getset(
				account_list /* in/out */,
				transaction->journal_list );
		}

	} while ( list_next( transaction_list ) );

	return account_list;
}

void transaction_journal_list_insert(
		char *fund_name,
		LIST *transaction_list,
		boolean with_propagate )
{
	char *column_list_string;
	FILE *insert_pipe;
	TRANSACTION *transaction;
	char *first_transaction_date_time = {0};

	if ( !list_rewind( transaction_list ) ) return;

	column_list_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		journal_column_list_string(
			JOURNAL_INSERT,
			TRANSACTION_TABLE,
			TRANSACTION_FUND_COLUMN );

	insert_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		journal_insert_pipe(
			JOURNAL_TABLE,
			column_list_string );

	do {
		transaction = list_get( transaction_list );

		if ( !first_transaction_date_time )
		{
			first_transaction_date_time =
				transaction->transaction_date_time;
		}

		if ( list_length( transaction->journal_list ) )
		{
			journal_list_transaction_insert(
				insert_pipe,
				fund_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->journal_list );
		}

	} while ( list_next( transaction_list ) );

	pclose( insert_pipe );

	if ( with_propagate )
	{
		journal_propagate_account_list(
			fund_name,
			first_transaction_date_time,
			transaction_list_extract_account_list(
				transaction_list ) );
	}
}

void transaction_fetch_update(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time )
{
	TRANSACTION *transaction;

	if ( !full_name
	||   !street_address
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

	if ( ! ( transaction =
			transaction_fetch(
				fund_name,
				full_name,
				street_address,
				transaction_date_time,
				1 /* fetch_journal_list */ ) ) )
	{
		/* Might be deleted */
		/* ---------------- */
		return;
	}

	transaction_update(
		journal_transaction_amount(
			transaction->journal_list ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_primary_where(
			fund_name,
			full_name,
			street_address,
			transaction_date_time ) );
}

char *transaction_refresh(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		double transaction_amount,
		int check_number,
		char *memo,
		LIST *journal_list )
{
	if ( !full_name
	||   !street_address
	||   !transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transaction_delete(
		fund_name,
		full_name,
		street_address,
		transaction_date_time );

	/* Note: transaction_date_time shouldn't change. */
	/* --------------------------------------------- */
	transaction_date_time =
		transaction_insert(
			fund_name,
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			check_number,
			memo,
			journal_list,
			1 /* insert_journal_list */ );

	return transaction_date_time;
}

char *transaction_increment_date_time( char *transaction_date_string )
{
	static DATE *static_date_second = {0};
	char transaction_date_time[ 128 ];
	char *hhmmss;

	if ( !transaction_date_string
	||   !*transaction_date_string
	||   strcmp( transaction_date_string, "transaction_date" ) == 0 )
	{
		transaction_date_string =
			 date_now_yyyy_mm_dd(
				date_utc_offset() );
	}

	if ( !static_date_second )
	{
		static_date_second =
			date_yyyy_mm_dd_hhmmss_new(
				transaction_date_string,
				date_now_hhmmss(
					date_utc_offset() ) );
	}

	hhmmss = date_hhmmss( static_date_second );

	sprintf(transaction_date_time,
		"%s %s",
		transaction_date_string,
		hhmmss );

	free( hhmmss );

	date_increment_second( static_date_second, 1 );

	return strdup( transaction_date_time );
}

char *transaction_stamp_insert(
		TRANSACTION *transaction /* in/out */,
		boolean insert_journal_list_boolean )
{
	if ( !transaction
	||   !transaction->transaction_amount )
	{
		char message[ 128 ];

		sprintf(message, "transaction is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction->transaction_date_time =
		transaction_insert(
			transaction->fund_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			transaction->check_number,
			transaction->memo,
			transaction->journal_list,
			insert_journal_list_boolean );

	return transaction->transaction_date_time;
}

TRANSACTION *transaction_date_time_fetch(
		char *transaction_date_time,
		boolean fetch_journal_list )
{
	if ( !transaction_date_time )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_date_time is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	transaction_parse(
		(char *)0 /* fund_name */,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_fetch(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			transaction_system_string(
				TRANSACTION_SELECT,
				TRANSACTION_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_fetch_where(
					(char *)0 /* fund_name */,
					(char *)0 /* full_name */,
					(char *)0 /* street_address */,
					transaction_date_time ) ) ),
		fetch_journal_list );
}

char *transaction_date_time_fetch_where( char *transaction_date_time )
{
	static char where[ 64 ];

	snprintf(
		where,
		sizeof ( where ),
		"transaction_date_time = '%s'",
		transaction_date_time );

	return where;
}

TRANSACTION *transaction_binary_account_key(
		const char *debit_account_key,
		const char *credit_account_key,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		double transaction_amount,
		char *memo )
{
	char *debit_account_name;
	char *credit_account_name;

	if ( !full_name
	||   !street_address )
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

	if ( !transaction_date_time
	||   float_virtually_same(
		transaction_amount,
		0.0 ) )
	{
		return NULL;
	}

	debit_account_name =
		account_hard_coded_account_name(
			(char *)debit_account_key,
			0 /* not warning_only */,
			__FUNCTION__ /* calling_function_name */ );

	credit_account_name =
		account_hard_coded_account_name(
			(char *)credit_account_key,
			0 /* not warning_only */,
			__FUNCTION__ /* calling_function_name */ );

	return
	transaction_binary(
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		memo,
		debit_account_name,
		credit_account_name );
}

char *transaction_fetch_where(
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time )
{
	char *fetch_where;

	if ( full_name && street_address )
	{
		fetch_where =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_primary_where(
				fund_name,
				full_name,
				street_address,
				transaction_date_time );
	}
	else
	{
		fetch_where =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_fetch_where(
				transaction_date_time );
	}

	return fetch_where;
}

char *transaction_fetch_memo( char *transaction_date_time )
{
	TRANSACTION *transaction;

	if ( !transaction_date_time ) return NULL;

	if ( ! ( transaction =
			transaction_date_time_fetch(
				transaction_date_time,
				0 /* not fetch_journal_list */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"transaction_date_time_fetch(%s) returned empty.",
			transaction_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return transaction->memo;
}

char *transaction_fund_where(
		const char *transaction_table,
		const char *transaction_fund_column,
		char *fund_name )
{
	static char where[ 128 ];

	if ( *where ) return where;

	if ( transaction_fund_column_boolean(
		transaction_table,
		transaction_fund_column ) )
	{
		if ( !fund_name )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"fund_name is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		snprintf(
			where,
			sizeof ( where ),
			"%s = '%s' and ",
			transaction_fund_column,
			fund_name );
	}

	return where;
}

char *transaction_column_list_string(
		const char *input_column_list_string,
		const char *transaction_table,
		const char *transaction_fund_column,
		const char *transaction_lock_column )
{
	static char column_list_string[ 128 ];
	char *ptr = column_list_string;

	ptr += sprintf(
		ptr,
		"%s",
		input_column_list_string );

	if ( transaction_fund_column_boolean(
		transaction_table,
		transaction_fund_column ) )
	{
		ptr += sprintf(
			ptr,
			",%s",
			transaction_fund_column );
	}

	if ( transaction_lock_column_boolean(
		transaction_table,
		transaction_lock_column ) )
	{
		ptr += sprintf(
			ptr,
			",%s",
			transaction_lock_column );
	}

	return column_list_string;
}

char *transaction_fund_datum(
		const char *transaction_table,
		const char *transaction_fund_column,
		char *fund_name )
{
	static char datum[ 32 ];

	if ( *datum ) return datum;

	if ( transaction_fund_column_boolean(
		transaction_table,
		transaction_fund_column ) )
	{
		if ( !fund_name )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"fund_name is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		snprintf(
			datum,
			sizeof ( datum ),
			"^%s",
			fund_name );
	}

	return datum;
}

boolean transaction_lock_column_boolean(
		const char *transaction_table,
		const char *transaction_lock_column )
{
	static boolean lock_column_boolean = -1;

	if ( lock_column_boolean == -1 )
	{
		lock_column_boolean =
			folder_attribute_exists(
				(char *)transaction_table,
				(char *)transaction_lock_column );
	}

	return lock_column_boolean;
}

boolean transaction_fund_column_boolean(
		const char *transaction_table,
		const char *transaction_fund_column )
{
	static boolean fund_column_boolean = -1;

	if ( fund_column_boolean == -1 )
	{
		fund_column_boolean =
			folder_attribute_exists(
				(char *)transaction_table,
				(char *)transaction_fund_column );
	}

	return fund_column_boolean;
}

char *transaction_insert_data_string(
		char *full_name,
		char *street_address,
		char *transaction_race_free_date_time,
		double transaction_amount,
		char *transaction_check_number,
		char *transaction_memo,
		boolean transaction_lock_column_boolean,
		char *transaction_fund_datum )
{
	char insert_data_string[ 1024 ];
	char *ptr = insert_data_string;

	if ( !full_name
	||   !street_address
	||   !transaction_race_free_date_time
	||   !transaction_check_number
	||   !transaction_memo
	||   !transaction_fund_datum )
	{
		return NULL;
	}

	if ( float_dollar_virtually_same(
		transaction_amount, 0.0 ) )
	{
		return NULL;
	}

	ptr += sprintf(
		ptr,
		"%s^%s^%s^%.2lf^%s^%s%s",
	 	entity_escape_full_name( full_name ),
		entity_escape_street_address( street_address ),
		transaction_race_free_date_time,
		transaction_amount,
		transaction_check_number,
		transaction_memo,
		transaction_fund_datum );

	if ( transaction_lock_column_boolean )
	{
		ptr += sprintf(
			ptr,
			"^y" );
	}

	return strdup( insert_data_string );
}

char *transaction_lock_datum(
		const char *transaction_table,
		const char *transaction_lock_column )
{
	static char datum[ 16 ];

	if ( *datum ) return datum;

	if ( transaction_lock_column_boolean(
		transaction_table,
		transaction_lock_column ) )
	{
		snprintf(
			datum,
			sizeof ( datum ),
			"^y" );
	}

	return datum;
}

char *transaction_escape_date_time( char *transaction_date_time )
{
	static char escape_date_time[ 64 ];

	*escape_date_time = '\0';

	if ( !transaction_date_time ) return escape_date_time;

	return
	string_escape_quote(
		escape_date_time,
		transaction_date_time );
}

