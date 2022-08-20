/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
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
#include "list.h"
#include "environ.h"
#include "application.h"
#include "journal.h"
#include "account.h"
#include "entity.h"
#include "appaserver_error.h"
#include "predictive.h"
#include "float.h"
#include "transaction.h"

TRANSACTION *transaction_calloc( void )
{
	TRANSACTION *transaction;

	if ( ! ( transaction = calloc( 1, sizeof( TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return transaction;
}

TRANSACTION *transaction_full(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			boolean lock_transaction,
			int seconds_to_add )
{
	TRANSACTION *transaction;

	if ( !transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty transaction_date_time\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( seconds_to_add )
	{
		DATE *transaction_date;

		transaction_date =
			date_yyyy_mm_dd_hms_new(
				transaction_date_time );

		date_add_seconds( transaction_date, seconds_to_add );

		transaction_date_time =
			date_display_19(	
				transaction_date );
	}

	transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_time );

	transaction->transaction_amount = transaction_amount;
	transaction->memo = memo;
	transaction->lock_transaction = lock_transaction;
	return transaction;
}

TRANSACTION *transaction_new(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	TRANSACTION *transaction;

	transaction = transaction_calloc();

	transaction->full_name = full_name;
	transaction->street_address = street_address;
	transaction->transaction_date_time = transaction_date_time;

	return transaction;
}

char *transaction_system_string(
			char *transaction_select,
			char *transaction_table,
			char *transaction_primary_where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh \"%s\" %s \"%s\" transaction_date_time",
		 transaction_select,
		 transaction_table,
		 transaction_primary_where );

	return strdup( sys_string );
}

TRANSACTION *transaction_fetch(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			boolean fetch_journal_list )
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

	return
	transaction_parse(
		string_pipe_fetch(
			transaction_system_string(
				TRANSACTION_SELECT,
				TRANSACTION_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
	 			transaction_primary_where(
					full_name,
					street_address,
					transaction_date_time ) ) ),
		fetch_journal_list );
}

TRANSACTION *transaction_parse(
			char *input,
			boolean fetch_journal_list )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	TRANSACTION *transaction;

	if ( !input || !*input ) return (TRANSACTION *)0;

	/* See TRANSACTION_SELECT */
	/* ---------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input, 2 );

	transaction =
		transaction_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	transaction->transaction_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	transaction->memo = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	transaction->check_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	transaction->lock_transaction = ( *piece_buffer == 'y' );

	if ( fetch_journal_list )
	{
		transaction->journal_list =
			journal_system_list(
				journal_system_string(
					JOURNAL_SELECT,
					JOURNAL_TABLE,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					transaction_primary_where(
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
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	static char where[ 256 ];
	char escape_full_name[ 64 ];

	sprintf(where,
		"full_name = '%s' and		"
		"street_address = '%s' and	"
		"transaction_date_time = '%s'	",
		string_escape_quote(
			escape_full_name,
			full_name ),
		street_address,
		transaction_date_time );

	return where;
}

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			int check_number,
			char *memo,
			char lock_transaction_yn,
			LIST *journal_list,
			boolean insert_journal_list_boolean )
{
	FILE *pipe_open;
	char *race_free_date_time;

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

	if ( !transaction_amount
	||   !list_length( journal_list ) )
	{
		return (char *)0;
	}

	pipe_open =
		transaction_insert_pipe_open(
			TRANSACTION_SELECT,
			TRANSACTION_TABLE );

	race_free_date_time =
		transaction_race_free_date_time(
			transaction_date_time );

	transaction_insert_pipe(
		pipe_open,
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
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_lock( lock_transaction_yn ) );

	pclose( pipe_open );

	if ( insert_journal_list_boolean )
	{
		journal_list_insert(
			full_name,
			street_address,
			race_free_date_time,
			journal_list );

		journal_account_list_propagate(
			race_free_date_time,
			journal_extract_account_list(
				journal_list ) );
	}

	return race_free_date_time;
}

void transaction_insert_pipe(
			FILE *pipe_open,
			char *full_name,
			char *street_address,
			char *race_free_date_time,
			double transaction_amount,
			char *transaction_check_number,
			char *transaction_memo,
			char *transaction_lock )
{
	fprintf(pipe_open,
		"%s^%s^%s^%.2lf^%s^%s^%s\n",
	 	full_name,
		street_address,
		race_free_date_time,
		transaction_amount,
		transaction_check_number,
		transaction_memo,
		transaction_lock );
}

char *transaction_check_number( int check_number )
{
	static char number[ 16 ];

	if ( check_number )
		sprintf( number, "%d", check_number );
	else
		*number = '\0';

	return number;
}

char *transaction_lock( char transaction_lock_yn )
{
	static char lock[ 2 ];

	if ( transaction_lock_yn == 'y'  )
		strcpy( lock, "y" );
	else
		*lock = '\0';

	return lock;
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
		string_strcpy( return_memo, memo, TRANSACTION_MEMO_LENGTH + 1 );
		/* *( return_memo + TRANSACTION_MEMO_LENGTH ) = '\0'; */
	}

	return return_memo;
}

char *transaction_race_free_date_time( char *transaction_date_time )
{
	key_t key = TRANSACTION_SEMAPHORE_KEY;
	int semid;
	DATE *next_transaction_date_time = {0};

	if ( ( semid = semaphore( key ) ) < 0 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: semaphore() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	semaphore_wait( semid );

	while ( transaction_date_time_exists(
			TRANSACTION_TABLE,
			TRANSACTION_DATE_TIME_COLUMN,
			transaction_date_time ) )
	{
		if ( !next_transaction_date_time )
		{
			next_transaction_date_time =
				date_yyyy_mm_dd_hms_new(
					transaction_date_time );
		}

		date_increment_seconds(
			next_transaction_date_time,
			1 );

		transaction_date_time =
			date_display_yyyy_mm_dd_colon_hms(
				next_transaction_date_time );
	}

	semaphore_signal( semid );

	return transaction_date_time;
}

char *transaction_date_time(
			char *transaction_date,
			char *transaction_time )
{
	char static date_time[ 128 ];

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
	char static where[ 128 ];

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

char *transaction_closing_memo_where(
			char *transaction_closing_entry_memo )
{
	char static where[ 128 ];

	sprintf(where,
		"memo = '%s'",
		transaction_closing_entry_memo );

	return where;
}

boolean transaction_date_time_exists(
			char *transaction_table,
			char *transaction_date_time_column,
			char *transaction_date_time )
{
	char system_string[ 1024 ];
	char where[ 128 ];
	char *results;
	boolean return_value;

	sprintf(where,
		"%s = '%s'",
		transaction_date_time_column,
		transaction_date_time );

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" none",
		"count(1)",
		transaction_table,
		where );

	results = pipe2string( system_string );
	return_value = ( atoi( results ) == 1 );
	free( results );
	return return_value;
}

char *transaction_closing_entry_where(
			char *transaction_date_time_where,
			char *transaction_closing_memo_where )
{
	static char where[ 128 ];

	sprintf(where,
		"%s and %s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time_where,
		transaction_closing_memo_where );

	return where;
}

char *transaction_closing_entry_system_string(
			char *transaction_table,
			char *transaction_closing_entry_where )
{
	char system_string[ 512 ];

	if ( !transaction_table
	||   !transaction_closing_entry_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is  empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" none",
		"count(1)",
		transaction_table,
		transaction_closing_entry_where );

	return strdup( system_string );
}

boolean transaction_closing_entry_exists(
			char *transaction_table,
			char *transaction_close_time,
			char *transaction_as_of_date )
{
	char *system_string;
	char *results;
	boolean return_value;
	char *where;

	if ( !transaction_table
	||   !transaction_close_time
	||   !transaction_as_of_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_closing_entry_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_where(
				TRANSACTION_DATE_TIME_COLUMN,
				transaction_as_of_date,
				transaction_close_time ),
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_closing_memo_where(
				TRANSACTION_CLOSING_ENTRY_MEMO ) );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_closing_entry_system_string(
			transaction_table,
			where );

	results = pipe2string( system_string );
	return_value = ( atoi( results ) == 1 );
	free( results );
	free( system_string );

	return return_value;
}

char *transaction_date_time_closing(
			char *transaction_preclose_time,
			char *transaction_close_time,
			char *transaction_as_of_date,
			boolean preclose_time_boolean )
{
	static char date_time[ 32 ];

	if ( preclose_time_boolean )
	{
		sprintf(date_time,
			"%s %s",
			transaction_as_of_date,
			transaction_preclose_time );
	}
	else
	{
		sprintf(date_time,
			"%s %s",
			transaction_as_of_date,
			transaction_close_time );
	}

	return date_time;
}

char *transaction_raw_display( TRANSACTION *transaction )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	char *tmp;

	buf_ptr += sprintf(
			buf_ptr,
			"full_name = %s, "
			"street_address = %s, "
			"transaction_date_time = %s, "
			"transaction_amount = %.2lf\n",
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount );

	buf_ptr += sprintf(
			buf_ptr,
			"%s\n",
			( tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				journal_list_raw_display(
					transaction->journal_list ) ) );

	free( tmp );

	return strdup( buffer );
}

void transaction_update(
			double transaction_amount,
			char *transaction_primary_where )
{
	char *update_statement;

	if ( !transaction_primary_where )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_primary_where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		transaction_update_statement(
			TRANSACTION_TABLE,
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			transaction_set_clause(
				TRANSACTION_AMOUNT_COLUMN,
				transaction_amount ),
			transaction_primary_where );

	if ( update_statement )
	{
		if ( system( update_statement ) ){}
	}
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
			transaction_amount );
	}

	if ( ptr == set_clause )
		return (char *)0;
	else
		return set_clause;
}

char *transaction_update_statement(
			char *transaction_table,
			char *transaction_set_clause,
			char *transaction_primary_where )
{
	char update_statement[ 1024 ];

	if ( !transaction_table
	||   !transaction_primary_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !transaction_set_clause ) return (char *)0;

	sprintf(update_statement,
		"update %s %s where %s",
		transaction_table,
		transaction_set_clause,
		transaction_primary_where );

	return strdup( update_statement );
}

boolean transaction_date_time_changed(
			char *preupdate_transaction_date_time )
{
	if ( !preupdate_transaction_date_time
	||   !*preupdate_transaction_date_time
	||   strcmp(	preupdate_transaction_date_time,
			"preupdate_transaction_date_time" ) == 0 )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void transaction_delete(char *full_name,
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
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( transaction =
			transaction_fetch(
				full_name,
				street_address,
				transaction_date_time,
				1 /* fetch_journal_list */ ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			transaction_date_time );
		exit( 1 );
	}


	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_primary_where(
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

	system_string =
		journal_delete_system_string(
			JOURNAL_TABLE,
			primary_where );

	if ( system( system_string ) ){}

	journal_account_list_propagate(
		transaction_date_time,
		journal_extract_account_list(
			transaction->journal_list )  );
}

char *transaction_delete_system_string(
			char *transaction_table,
			char *transaction_primary_where )
{
	char system_string[ 512 ];

	sprintf(system_string,
		"echo \"delete from %s where %s;\" | sql",
		transaction_table,
		transaction_primary_where );

	return strdup( system_string );
}

FILE *transaction_insert_pipe_open(
			char *transaction_select,
			char *transaction_table )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"insert_statement t=%s f=%s delimiter='^'	|"
/*		"tee_appaserver_error.sh			|" */
		"sql 2>&1					|"
		"html_paragraph_wrapper.e			 ",
		transaction_table,
		transaction_select );

	return popen( system_string, "w" );
}

FILE *transaction_input_pipe( char *transaction_system_string )
{
	return
	popen( transaction_system_string, "r" );
}

LIST *transaction_list(
			char *where,
			boolean fetch_journal_ledger )
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
				input,
				fetch_journal_ledger ) );
	}

	pclose( input_pipe );

	return list;
}

void transaction_list_insert(
			LIST *transaction_list,
			boolean insert_journal_list_boolean )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return;

	do {
		transaction =
			list_get(
				transaction_list );

		if ( transaction->transaction_amount
		&&   list_length( transaction->journal_list ) )
		{
			transaction->transaction_date_time =
				transaction_insert(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					transaction->transaction_amount,
					transaction->check_number,
					transaction->memo,
					TRANSACTION_LOCK_Y,
					transaction->journal_list,
					insert_journal_list_boolean );
		}

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
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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

void transaction_list_html_display(
			LIST *transaction_list )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) )
	{
		return;
	}

	do {
		transaction =
			list_get(
				transaction_list );

		journal_list_html_display(
			transaction->journal_list,
			transaction->memo );

	} while ( list_next( transaction_list ) );
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

	if ( !transaction_amount ) return (TRANSACTION *)0;

	transaction =
		transaction_new(
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

boolean transaction_as_of_date_populated( char *as_of_date )
{
	if ( as_of_date
	&&   *as_of_date
	&&   strcmp( as_of_date, "as_of_date" ) != 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *transaction_as_of_date(
			char *transaction_table,
			char *as_of_date )
{
	if ( transaction_as_of_date_populated( as_of_date ) )
	{
		return as_of_date;
	}
	else
	{
		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		transaction_date_max( transaction_table );
	}
}

char *transaction_date_max( char *transaction_table )
{
	char *date_time_max;

	if ( ! ( date_time_max =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			transaction_date_time_max(
				transaction_table ) ) )
	{
		return (char *)0;
	}

	*( date_time_max + 10 ) = '\0';

	return date_time_max;
}

char *transaction_date_time_max( char *transaction_table )
{
	char system_string[ 128 ];
	char *select;

	select = "max( transaction_date_time )";

	sprintf(system_string,
		"select.sh \"%s\" %s",
		select,
		transaction_table );

	return pipe2string( system_string );
}

DATE *transaction_prior_closing_transaction_date(
			char *transaction_close_time,
			char *transaction_closing_entry_memo,
			char *transaction_table,
			char *transaction_as_of_date )
{
	char where[ 512 ];
	char system_string[ 1024 ];
	char *select;
	char *results;
	char ending_transaction_date_time[ 32 ];
	char transaction_date_string[ 16 ];
	DATE *prior_closing_transaction_date = {0};

	if ( !transaction_close_time
	||   !transaction_closing_entry_memo
	||   !transaction_table
	||   !transaction_as_of_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	select = "max( transaction_date_time )";

	sprintf(ending_transaction_date_time,
		"%s %s",
		transaction_as_of_date,
		transaction_close_time );

	sprintf(where,
		"memo = '%s' and transaction_date_time < '%s'",
		transaction_closing_entry_memo,
		ending_transaction_date_time );

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" none",
		 select,
		 transaction_table,
		 where );

	results = pipe2string( system_string );

	if ( results && *results )
	{
		column( transaction_date_string, 0, results );

		prior_closing_transaction_date =
			date_yyyy_mm_dd_new(
				transaction_date_string );
	}

	return prior_closing_transaction_date;
}

char *transaction_minimum_transaction_date_string(
			char *transaction_table )
{
	char system_string[ 1024 ];
	char transaction_date_string[ 16 ];
	char *results;


	sprintf(system_string,
		"get_folder_data	application=%s		"
		"			select=\"%s\"		"
		"			folder=%s		",
		environment_application_name(),
		"min( transaction_date_time )",
		transaction_table );

	results = pipe2string( system_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );
}

char *transaction_begin_date_string(
			char *transaction_table,
			char *transaction_as_of_date )
{
	DATE *prior_closing_transaction_date = {0};

	prior_closing_transaction_date =
		transaction_prior_closing_transaction_date(
			TRANSACTION_CLOSE_TIME,
			TRANSACTION_CLOSING_ENTRY_MEMO,
			transaction_table,
			transaction_as_of_date );

	if ( prior_closing_transaction_date )
	{
		date_increment_days(
			prior_closing_transaction_date,
			1.0 );

		return date_yyyy_mm_dd( prior_closing_transaction_date );
	}

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	transaction_minimum_transaction_date_string(
		transaction_table );
}

LIST *transaction_list_extract_account_list(
			LIST *transaction_list )
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
			LIST *transaction_list )
{
	FILE *insert_pipe;
	TRANSACTION *transaction;
	char *first_transaction_date_time = {0};

	if ( !list_rewind( transaction_list ) ) return;

	insert_pipe =
		journal_insert_pipe(
			JOURNAL_INSERT,
			JOURNAL_TABLE );

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
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->journal_list );
		}

	} while ( list_next( transaction_list ) );

	pclose( insert_pipe );

	journal_account_list_propagate(
		first_transaction_date_time,
		transaction_list_extract_account_list(
			transaction_list ) );
}

