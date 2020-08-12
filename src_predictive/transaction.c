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

TRANSACTION *transaction_new(	char *full_name,
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

TRANSACTION *transaction_fetch(
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	TRANSACTION *transaction;

	if ( !full_name || !street_address )
	{
		return (TRANSACTION *)0;
	}

	if ( !transaction_date_time || !*transaction_date_time )
	{
		return (TRANSACTION *)0;
	}

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 transaction_select(),
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
			transaction_primary_where(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time ) );

	return transaction;
}

char *transaction_select( void )
{
	return
		"full_name,"
		"street_address,"
		"transaction_date_time,"
		"transaction_amount,"
		"memo,"
		"check_number,"
		"lock_transaction_yn";
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
	transaction->transaction_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 4 );
	transaction->memo = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 5 );
	transaction->check_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 6 );
	transaction->lock_transaction = ( *piece_buffer == 'y' );

	return transaction;
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

FILE *transaction_insert_open( void )
{
	char sys_string[ 1024 ];
	char *field;

	field =
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^'	|"
		 "sql 2>&1						 ",
		 "transaction",
		 field );

	return popen( sys_string, "w" );
}

FILE *transaction_property_insert_open( void )
{
	char sys_string[ 1024 ];
	char *field;

	field =
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn,property_street_address";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^'	|"
		 "sql 2>&1						 ",
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

	insert_pipe = transaction_insert_open();

	transaction_date_time =
		transaction_insert_pipe(
			insert_pipe,
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction );

	pclose( insert_pipe );
	return transaction_date_time;
}

char *transaction_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction )
{
	transaction_date_time =
		transaction_race_free(
			transaction_date_time );

	fprintf(	insert_pipe,
			"%s^%s^%s^%.2lf^%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
	 		transaction_escape_full_name( full_name ),
			street_address,
			transaction_date_time,
			transaction_amount,
			transaction_memo( memo ) );

	transaction_check_insert(
		insert_pipe,
		check_number );

	transaction_lock_insert(
		insert_pipe,
		lock_transaction );

	fflush( insert_pipe );
	return transaction_date_time;
}

/* TRANSACTION with program_name addition */
/* -------------------------------------- */
FILE *transaction_program_insert_open( void )
{
	char sys_string[ 1024 ];
	char *field;

	field =
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn,program_name";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^'	|"
		 "sql 2>&1						 ",
		 "transaction",
		 field );

	return popen( sys_string, "w" );
}

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_program_insert(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *program_name,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction )
{
	FILE *insert_pipe;

	insert_pipe = transaction_program_insert_open();

	transaction_date_time =
		transaction_program_insert_pipe(
			insert_pipe,
			full_name,
			street_address,
			transaction_date_time,
			program_name,
			transaction_amount,
			memo,
			check_number,
			lock_transaction );

	pclose( insert_pipe );
	return transaction_date_time;
}

char *transation_memo( char *memo )
{
	if ( !memo || strcmp( memo, "memo" ) == 0 )
	{
		memo = "";
	}
	return memo;
}

void transaction_check_insert(
			FILE *insert_pipe,
			int check_number )
{
	if ( check_number )
		fprintf( insert_pipe, "^%d", check_number );
	else
		fprintf( insert_pipe, "^" );
}

void transaction_lock_insert(
			FILE *insert_pipe,
			boolean lock_transaction )
{
	if ( lock_transaction )
		fprintf( insert_pipe, "^y\n" );
	else
		/* Note: keep empty, not 'n' */
		/* ------------------------- */
		fprintf( insert_pipe, "^\n" );
}

char *transaction_property_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *property_street_address,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction )
{
	transaction_date_time =
		transaction_race_free(
			transaction_date_time );

	fprintf(	insert_pipe,
			"%s^%s^%s^%s^%.2lf^%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
	 		transaction_escape_full_name( full_name ),
			street_address,
			transaction_date_time,
			property_street_address,
			transaction_amount,
			transaction_memo( memo ) );

	transaction_check_insert(
		insert_pipe,
		check_number );

	transaction_lock_insert(
		insert_pipe,
		lock_transaction );

	fflush( insert_pipe );
	return transaction_date_time;
}

char *transaction_program_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *program_name,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction )
{
	transaction_date_time =
		transaction_race_free(
			transaction_date_time );

	fprintf(	insert_pipe,
			"%s^%s^%s^%s^%.2lf^%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
	 		transaction_escape_full_name( full_name ),
			street_address,
			transaction_date_time,
			program_name,
			transaction_amount,
			transaction_memo( memo ) );

	transaction_check_insert(
		insert_pipe,
		check_number );

	transaction_lock_insert(
		insert_pipe,
		lock_transaction );

	fflush( insert_pipe );
	return transaction_date_time;
}

/* Returns race-free transaction_date_time */
/* --------------------------------------- */
char *transaction_race_free( char *transaction_date_time )
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
		 "echo \"select count(1) from %s where %s;\" | sql",
		 "transaction",
		 where );

	results = pipe2string( sys_string );
	return_value = ( atoi( results ) == 1 );
	free( results );
	return return_value;
}

void transaction_refresh(
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
}

void transaction_list_journal_insert(
			LIST *transaction_list )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return;

	do {
		transaction = list_get( transaction_list );

		transaction_journal_list_insert(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->journal_list );

	} while ( list_next( transaction_list ) );
}

void transaction_journal_list_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list )
{
	journal_list_insert(
		full_name,
		street_address,
		transaction_date_time,
		journal_list );
}

void transaction_delete(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;

	field= "full_name,street_address,transaction_date_time";

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql							 ",
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
				journal_list_display(
					transaction->journal_list ) );

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
LIST *transaction_list_insert( LIST *transaction_list )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return transaction_list;

	do {
		transaction = list_get( transaction_list );

		transaction->transaction_date_time =
			transaction_insert(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				transaction->lock_transaction );

	} while( list_next( transaction_list ) );

	/* transaction_list_journal_insert( transaction_list ); */

	return transaction_list;
}

void transaction_update(	double transaction_amount,
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	FILE *update_pipe;
	char *key;

	key = "full_name,street_address,transaction_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 "transaction",
		 key );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
		"%s^%s^%s^transaction_amount^%.2lf\n",
		transaction_escape_full_name( full_name ),
		street_address,
		transaction_date_time,
		transaction_amount );

	pclose( update_pipe );
}

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
			strdup( transaction_date_time ) );

	transaction->memo = strdup( memo );
	transaction->transaction_amount = transaction_amount;

	transaction->journal_list =
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
		journal_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( credit_account ) );

	journal->credit_amount = transaction_amount;

	list_set( journal_list, journal );

	return journal_list;
}

TRANSACTION *transaction_check_seek(
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

