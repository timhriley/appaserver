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
#include "date.h"
#include "semaphore.h"
#include "sql.h"
#include "list.h"
#include "journal.h"
#include "transaction.h"

TRANSACTION *transaction_new(	char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo )
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

	transaction->full_name = full_name;
	transaction->street_address = street_address;
	transaction->transaction_date_time = transaction_date_time;

	if ( string_strcmp( memo, "memo" ) == 0 )memo = (char *)0;

	transaction->memo = memo;

	return transaction;
}

TRANSACTION *transaction_program_parse( char *input_buffer )
{
	char program_name[ 128 ];
	TRANSACTION *transaction;

	if ( !input_buffer ) return (TRANSACTION *)0;

	if ( ! ( transaction = transaction_parse( input_buffer ) ) )
		return (TRANSACTION *)0;

	piece( program_name, SQL_DELIMITER, input_buffer, 7 );
	transaction->program_name = strdup( program_name );

	return transaction;
}

char *transaction_property_select( void )
{
	return
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn,property_street_address";
}

TRANSACTION *transaction_property_parse( char *input_buffer )
{
	char property_street_address[ 128 ];
	TRANSACTION *transaction;

	if ( !input_buffer ) return (TRANSACTION *)0;

	if ( ! ( transaction = transaction_parse( input_buffer ) ) )
		return (TRANSACTION *)0;

	piece( property_street_address, SQL_DELIMITER, input_buffer, 7 );

	transaction->property_street_address =
		strdup( property_street_address );

	return transaction;
}

char *transaction_select( boolean with_program )
{
	char *select;

	if ( with_program )
	{
		select =
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn,program_name";
	}
	else
	{
		select =
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn";
	}
	return select;
}

TRANSACTION *transaction_parse( char *input_buffer )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	TRANSACTION *transaction;

	if ( !input_buffer ) return (TRANSACTION *)0;

	piece( full_name, SQL_DELIMITER, input_buffer, 0 );
	piece( street_address, SQL_DELIMITER, input_buffer, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input_buffer, 2 );

	transaction = transaction_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 3 );
	transaction->transaction_amount =
	transaction->database_transaction_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 4 );
	transaction->memo = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 5 );
	transaction->check_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 6 );
	transaction->lock_transaction = ( *piece_buffer == 'y' );

	return transaction;
}

TRANSACTION *transaction_fetch(
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	TRANSACTION *transaction;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 transaction_select( 0 /* with_program */ ),
		 "transaction",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 transaction_primary_where(
			full_name,
			street_address,
			transaction_date_time ) );

	transaction = transaction_parse( pipe2string( sys_string ) );

	transaction->journal_list =
		journal_list(
			transaction->full_name,
			transation->street_address,
			transaction->transaction_date_time );

	return transaction;
}

char *transaction_escape_memo( char *memo )
{
	static char escape_memo[ 1024 ];

	string_escape_quote( escape_memo, memo );
	return escape_memo;
}

char *transaction_full_name_escape( char *full_name )
{
	return transaction_escape_full_name( full_name );
}

char *transaction_escape_full_name( char *full_name )
{
	static char escape_full_name[ 256 ];

	string_escape_quote( escape_full_name, full_name );
	return escape_full_name;
}

/* Safely returns heap memory */
/* -------------------------- */
char *transaction_primary_where(
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char where[ 1024 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "transaction_date_time = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 transaction_escape_full_name( full_name ),
		 street_address,
		 transaction_date_time );

	return strdup( where );
}

FILE *transaction_insert_pipe( void )
{
	char sys_string[ 1024 ];
	char *field;

	field =
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e 2>&1					 ",
		 "transaction",
		 field );

	return popen( sys_string, "w" );
}

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(	char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction )
{
	FILE *insert_pipe;

	transaction_date_time =
		transaction_unique_transaction_date_time(
			transaction_date_time );

	insert_pipe = transaction_insert_pipe();

	if ( memo && *memo )
	{
		fprintf(	insert_pipe,
				"%s^%s^%s^%.2lf^%s",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
		 		transaction_escape_full_name( full_name ),
				street_address,
				transaction_date_time,
				transaction_amount,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_escape_memo( memo ) );
	}
	else
	{
		fprintf(	insert_pipe,
				"%s^%s^%s^%.2lf^",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
		 		transaction_escape_full_name( full_name ),
				street_address,
				transaction_date_time,
				transaction_amount );
	}

	if ( check_number )
		fprintf( insert_pipe, "^%d", check_number );
	else
		fprintf( insert_pipe, "^" );

	if ( lock_transaction )
		fprintf( insert_pipe, "^y\n" );
	else
		fprintf( insert_pipe, "^\n" );

	pclose( insert_pipe );
	return transaction_date_time;
}

char *transaction_unique_transaction_date_time(
				char *transaction_date_time )
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

	while ( transaction_exists( transaction_date_time ) )
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

boolean transaction_exists( char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *results;
	boolean return_value;

	sprintf( where, "transaction_date_time = '%s'", transaction_date_time );

	sprintf( sys_string,
		 "echo \"select count(1) from %s where %s;\" | sql.e",
		 "transaction",
		 where );

	results = pipe2string( sys_string );
	return_value = ( atoi( results ) == 1 );
	free( results );
	return return_value;
}

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction,
			LIST *journal_list )
{
	transaction_delete(
		full_name,
		street_address,
		transaction_date_time );

	/* Also does a propagate for each account */
	/* -------------------------------------- */
	journal_delete(	full_name,
			street_address,
			transaction_date_time );

	return transaction_journal_insert(
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction,
			journal_list );
}

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_journal_insert(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction,
				LIST *journal_list )
{
	transaction_date_time =
		transaction_insert(
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction );

	transaction_journal_list_insert(
		full_name,
		street_address,
		transaction_date_time,
		journal_list );

	return transaction_date_time;
}

void transaction_journal_list_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list )
{
	LIST *account_name_list;
	JOURNAL *journal;
	FILE *insert_pipe;
	double amount;
	boolean is_debit;

	if ( !list_rewind( journal_list ) ) return;

	account_name_list = list_new();
	insert_pipe = journal_insert_pipe();

	do {
		journal = list_get( journal_list );

		if ( !timlib_dollar_virtually_same(
			journal->debit_amount,
			0.0 ) )
		{
			amount = journal->debit_amount;
			is_debit = 1;
		}
		else
		{
			amount = journal->credit_amount;
			is_debit = 0;
		}

		journal_insert(	insert_pipe,
				full_name,
				street_address,
				transaction_date_time,
				journal->account_name,
				amount,
				is_debit );

		list_append_pointer(
			account_name_list,
			journal->account_name );

	} while( list_next( journal_list ) );

	pclose( insert_pipe );

	journal_account_name_list_propagate(
		transaction_date_time,
		account_name_list );
}

void transaction_delete(char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;

	field= "full_name,street_address,transaction_date_time";

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 "transaction",
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s\n",
			full_name,
			street_address,
			transaction_date_time );

	pclose( output_pipe );
}

char *transaction_display( TRANSACTION *transaction )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;

	buf_ptr += sprintf(	buf_ptr,
				"full_name = %s, "
				"street_address = %s, "
				"transaction_date_time = %s\n",
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time );

	buf_ptr += sprintf(	buf_ptr,
				"%s\n",
				ledger_list_display(
					transaction->journal_ledger_list ) );

	return strdup( buffer );
}

TRANSACTION *transaction_seek(
			LIST *transaction_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	TRANSACTION *t;

	if ( !list_rewind( transaction_list ) ) return (TRANSACTION *)0;

	do {
		t = list_get( transaction_list );

		if ( string_strcmp(	t->full_name,
					full_name ) == 0
		&&   string_strcmp(	t->street_address,
					street_address ) == 0
		&&   string_strcmp(	t->transaction_date_time,
					transaction_date_time ) == 0 )
		{
			return t;
		}

	} while( list_next( transaction_list ) );
	return (TRANSACTION *)0;
}

/* Returns transaction_list with transaction_date_time changed if needed. */
/* ---------------------------------------------------------------------- */
LIST *ledger_transaction_list_insert(	LIST *transaction_list,
					char *application_name )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return transaction_list;

	do {
		transaction = list_get_pointer( transaction_list );

		transaction->transaction_date_time =
			ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				transaction->lock_transaction,
				(LIST *)0 /* journal_ledger_list */ );

	} while( list_next( transaction_list ) );

	transaction_list_journal_insert( transaction_list );

	ledger_journal_ledger_batch_insert(
				application_name,
				transaction_list );

	return transaction_list;
}

void ledger_transaction_insert_stream(	FILE *output_pipe,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					char *memo,
					int check_number,
					boolean lock_transaction )
{
	char entity_buffer[ 128 ];
	char memo_buffer[ 1024 ];

	if ( memo && *memo )
	{
		fprintf(	output_pipe,
				"%s^%s^%s^%.2lf^%s",
		 		escape_character(	entity_buffer,
							full_name,
							'\'' ),
				street_address,
				transaction_date_time,
				transaction_amount,
				escape_character(	memo_buffer,
							memo,
							'\'' ) );
	}
	else
	{
		fprintf(	output_pipe,
				"%s^%s^%s^%.2lf^",
		 		escape_character(	entity_buffer,
							full_name,
							'\'' ),
				street_address,
				transaction_date_time,
				transaction_amount );
	}

	if ( check_number )
		fprintf( output_pipe, "^%d", check_number );
	else
		fprintf( output_pipe, "^" );

	if ( lock_transaction )
		fprintf( output_pipe, "^y\n" );
	else
		fprintf( output_pipe, "^\n" );

} /* ledger_transaction_insert_stream() */

FILE *ledger_transaction_insert_open_stream( char *application_name )
{
	char sys_string[ 1024 ];
	char field[ 128 ];
	char *table_name;
	FILE *output_pipe;

	sprintf( field,
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,%s",
		 LEDGER_LOCK_TRANSACTION_ATTRIBUTE );

	table_name =
		get_table_name(
			application_name,
			TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e 2>&1					|"
		 "grep -vi duplicate				 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	return output_pipe;

} /* ledger_transaction_insert_open_stream() */

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_binary_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction,
				char *debit_account_name,
				char *credit_account_name )
{
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	transaction_date_time =
		transaction_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction );

	journal_stream_open(
			&debit_account_pipe,
			&credit_account_pipe,
			application_name );

	journal_stream_output(
		debit_account_pipe,
		(FILE *)0 /* credit_account_pipe */,
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		debit_account_name,
		(char *)0 /* credit_account_name */ );

	journal_stream_output(
		(FILE *)0 /* debit_account_pipe */,
		credit_account_pipe,
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		(char *)0 /* debit_account_name */,
		credit_account_name );

	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	ledger_propagate(
		application_name,
		transaction_date_time,
		debit_account_name );

	ledger_propagate(
		application_name,
		transaction_date_time,
		credit_account_name );

	return transaction_date_time;
}

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_insert(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					char *memo,
					int check_number,
					boolean lock_transaction )
{
	FILE *output_pipe;

	transaction_date_time =
		ledger_fetch_unique_transaction_date_time(
			application_name,
			transaction_date_time );

	output_pipe =
		ledger_transaction_insert_open_stream(
			application_name );

	ledger_transaction_insert_stream(
		output_pipe,
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		memo,
		check_number,
		lock_transaction );

	pclose( output_pipe );

	return transaction_date_time;

} /* ledger_transaction_insert() */

void ledger_transaction_memo_update(	char *application_name,
					TRANSACTION *transaction )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *transaction_table;
	char *key;
	char entity_buffer[ 128 ];
	char memo_buffer[ 65536 ];

	if ( !transaction ) return;

	key = "full_name,street_address,transaction_date_time";

	transaction_table =
		get_table_name(
			application_name, TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 transaction_table,
		 key );

	output_pipe = popen( sys_string, "w" );

	if ( transaction->memo )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^memo^%s\n",
		 	escape_character(	entity_buffer,
						transaction->full_name,
						'\'' ),
		 	transaction->street_address,
		 	transaction->transaction_date_time,
		 	escape_character(	memo_buffer,
						transaction->memo,
						'\'' ) );
	}

	pclose( output_pipe );

} /* ledger_transaction_memo_update() */

TRANSACTION *ledger_binary_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double transaction_amount,
			char *memo )
{
	TRANSACTION *transaction;

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->transaction_amount = transaction_amount;

	transaction->journal_ledger_list =
		ledger_build_binary_ledger_list(
				full_name,
				street_address,
				transaction_date_time,
				transaction->transaction_amount,
				debit_account,
				credit_account );

	return transaction;

} /* ledger_build_binary_transaction() */

TRANSACTION *transaction_binary(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *debit_account,
				char *credit_account,
				double transaction_amount,
				char *memo )
{
	TRANSACTION *transaction;

	transaction =
		transaction_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( memo );

	transaction->transaction_amount = transaction_amount;

	transaction->journal_ledger_list =
		transaction_binary_journal_list(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				debit_account,
				credit_account );

	return transaction;
}

LIST *transaction_binary_journal_list(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *debit_account,
				char *credit_account )
{
	LIST *journal_list;
	JOURNAL *journal;

	if ( !debit_account
	||   !*debit_account
	||   !credit_account
	||   !*credit_account )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty account name(s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( timlib_double_virtually_same(
		transaction_amount, 0.0 ) )
	{
		return (LIST *)0;
	}

	journal_list = list_new();

	journal =
		journal_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( debit_account ) );

	journal->debit_amount = transaction_amount;

	list_set( journal_list, journal );

	journal =
		journal_ledger_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( credit_account ) );

	journal->credit_amount = transaction_amount;

	list_set( journal_list, journal );

	return journal_list;
}

TRANSACTION *ledger_check_number_seek_transaction(
				LIST *transaction_list,
				int check_number )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return (TRANSACTION *)0;

	do {
		transaction = list_get( transaction_list );

		if ( transaction->check_number == check_number )
			return transaction;

	} while ( list_next( transaction_list ) );

	return (TRANSACTION *)0;
}

void transaction_list_journal_insert( LIST *transaction_list )
{
	LIST *account_name_list;
	char *account_name;
	TRANSACTION *transaction;
	JOURNAL *journal;
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( transaction_list ) ) return;

	account_name_list = list_new();

	journal_open_stream(
			&debit_account_pipe,
			&credit_account_pipe );

	do {
		transaction = list_get( transaction_list );

		if ( !list_rewind( transaction->journal_list ) )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: empty journal_ledger_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			continue;
		}

		if ( !propagate_transaction_date_time )
			propagate_transaction_date_time =
				transaction->transaction_date_time;

		do {
			journal_ledger =
				list_get_pointer( 
					transaction->journal_ledger_list );

			if ( timlib_dollar_virtually_same(
				journal_ledger->debit_amount,
				0.0 )
			&&   timlib_dollar_virtually_same(
				journal_ledger->credit_amount,
				0.0 ) )
			{
				fprintf( stderr,
		"Warning in %s/%s()/%d: both debit and credit are zero.\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__ );
				continue;
			}

			if ( !timlib_dollar_virtually_same(
				journal_ledger->debit_amount,
				0.0 ) )
			{
				journal_stream_output(
					debit_account_pipe,
					(FILE *)0 /* credit_account_pipe */,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					journal_ledger->debit_amount
						/* amount */,
					journal_ledger->account_name
						/* debit_account_name */,
					(char *)0 /* credit_account_name */ );
			}
			else
			{
				journal_stream_output(
					(FILE *)0 /* debit_account_pipe */,
					credit_account_pipe,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					journal_ledger->credit_amount
						/* amount */,
					(char *)0 /* debit_account_name */,
					journal_ledger->account_name
						/* credit_account_name */ );
			}

			list_append_unique_string(
				account_name_list,
				journal_ledger->account_name );

		} while( list_next( transaction->journal_ledger_list ) );

	} while( list_next( transaction_list ) );

	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	if ( list_rewind( account_name_list ) )
	{
		do {
			account_name = list_get_pointer( account_name_list );

			ledger_propagate(
				application_name,
				propagate_transaction_date_time,
				account_name );

		} while( list_next( account_name_list ) );
	}
}

