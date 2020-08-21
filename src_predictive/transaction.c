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
#include "transaction.h"
#include "predictive.h"

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

LIST *transaction_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *transaction_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		/* Also executes journal_list() */
		/* ---------------------------- */
		list_set( transaction_list, transaction_parse( input ) );
	}

	pclose( input_pipe );
	return transaction_list;
}

LIST *transaction_list_fetch( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 transaction_select(),
		 "transaction",
		 where,
		 transaction_select() );

	return transaction_system_list( sys_string );
}

TRANSACTION *transaction_fetch(
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];

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

	/* Also executes journal_list() */
	/* ---------------------------- */
	return transaction_parse( pipe2string( sys_string ) );
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

TRANSACTION *transaction_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	TRANSACTION *transaction;

	if ( !input || !*input ) return (TRANSACTION *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input, 2 );

	transaction = transaction_new(
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

	transaction->journal_list =
		journal_list(
			transaction_primary_where(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time ) );

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

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(
			char *full_name,
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
			memo );

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
			memo );

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
			memo );

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

char *transaction_journal_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction,
			LIST *journal_list )
{
	return transaction_refresh(
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction,
			journal_list );
}

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

char *transaction_audit( TRANSACTION *transaction )
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
				journal_list_audit(
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

/* Returns begin_date_string */
/* ------------------------- */
char *transaction_report_title_sub_title(
		char *title,
		char *sub_title,
		char *process_name,
		char *fund_name,
		char *as_of_date,
		int fund_list_length,
		char *logo_filename )
{
	char *begin_date_string;
	char begin_date_american[ 32 ];
	char end_date_american[ 32 ];

	*begin_date_american = '\0';

	sprintf(title,
		"%s",
		application_get_application_title(
			environment_application() ) );

	if ( ! ( begin_date_string = 
			transaction_beginning_date_string(
				fund_name,
				as_of_date ) ) )
	{
		return 0;
	}

	date_convert_source_international(
		begin_date_american,
		american,
		begin_date_string );

	date_convert_source_international(
		end_date_american,
		american,
		as_of_date );

	if ( fund_name
	&&   *fund_name
	&&   strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf(sub_title,
	 		"%s, Fund: %s, Beginning: %s, Ending: %s",
	 		process_name,
			fund_name,
			begin_date_american,
	 		end_date_american );
	}
	else
	if ( fund_list_length > 1 )
	{
		sprintf(sub_title,
	 		"%s, Consolidated Funds, Beginning: %s, Ending: %s",
	 		process_name,
			begin_date_american,
	 		end_date_american );
	}
	else
	{
		if ( *begin_date_american )
		{
			sprintf(sub_title,
	 			"%s, Beginning: %s, Ending: %s",
	 			process_name,
				begin_date_american,
	 			end_date_american );
		}
		else
		{
			sprintf(sub_title,
	 			"%s, as of date: %s",
	 			process_name,
	 			end_date_american );
		}
	}

	if ( !logo_filename || !*logo_filename )
	{
		char buffer[ 256 ];

		sprintf( buffer, "%s %s", title, sub_title );
		strcpy( sub_title, buffer );
	}

	format_initial_capital( sub_title, sub_title );

	return begin_date_string;
}

DATE *transaction_prior_closing_transaction_date(
			char *fund_name,
			char *ending_transaction_date )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;
	char *results;
	char ending_transaction_date_time[ 32 ];
	char transaction_date_string[ 16 ];
	DATE *prior_closing_transaction_date = {0};

	if ( fund_name && !*fund_name )
		fund_name = (char *)0;
	else
	if ( fund_name && strcmp( fund_name, "fund" ) == 0 )
		fund_name = (char *)0;

	select = "max( transaction_date_time )";

	sprintf( ending_transaction_date_time,
		 "%s %s",
		 ending_transaction_date,
		 TRANSACTION_CLOSING_TRANSACTION_TIME );

	sprintf( where,
		 "memo = '%s' and transaction_date_time < '%s'",
		 TRANSACTION_CLOSING_ENTRY_MEMO,
		 ending_transaction_date_time );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 select,
		 "transaction",
		 where );

	results = pipe2string( sys_string );

	if ( results && *results )
	{
		column( transaction_date_string, 0, results );

		prior_closing_transaction_date =
			date_yyyy_mm_dd_new(
				transaction_date_string );
	}

	return prior_closing_transaction_date;
}

/* Returns heap memory. */
/* -------------------- */
char *transaction_date_max( void )
{
	char *r;

	r = transaction_date_time_max();
	*( r + 10 ) = '\0';
	return r;
}

/* Returns heap memory. */
/* -------------------- */
char *transaction_date_time_max( void )
{
	char sys_string[ 1024 ];
	char *select;

	select = "max( transaction_date_time )";

	sprintf( sys_string,
		 "echo \"select %s from %s;\" | sql",
		 select,
		 "transaction" );

	return pipe2string( sys_string );
}

char *transaction_closing_date_time( char *as_of_date )
{
	return transaction_date_time_closing( as_of_date );
}

char *transaction_closing_transaction_date_time( char *as_of_date )
{
	return transaction_date_time_closing( as_of_date );
}

char *transaction_date_time_closing(
			char *as_of_date )
{
	char transaction_date_time[ 32 ];

	sprintf(	transaction_date_time,
			"%s %s",
			as_of_date,
			TRANSACTION_CLOSING_TRANSACTION_TIME );

	return strdup( transaction_date_time );
}

boolean transaction_date_time_exists(
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *results;

	sprintf( where,
		 "transaction_date_time = '%s'",
		 transaction_date_time );

	sprintf( sys_string,
		 "echo \"select count(1) from %s where %s;\" | sql",
		 "transaction",
		 where );

	results = pipe2string( sys_string );

	if ( results && ( atoi( results ) == 1 ) )
		return 1;
	else
		return 0;
}

LIST *transaction_fund_name_list( void )
{
	char sys_string[ 512 ];

	if ( !predictive_fund_attribute_exists() )
	{
		return (LIST *)0;
	}

	sprintf( sys_string,
		 "echo \"select %s from %s order by %s;\" | sql",
		 "fund",
		 "fund",
		 "fund" );

	return pipe2list( sys_string );
}

char *transaction_journal_join( void )
{
	char *join_where;

	join_where =	"journal_ledger.full_name ="
			"	transaction.full_name and"
			"journal_ledger.street_address ="
			"	transaction.street_address and"
			"journal_ledger.transaction_date_time ="
			"	transaction.transaction_date_time";

	return join_where;
}

char *transaction_generate_date_time( char *transaction_date )
{
	return transaction_time_append( transaction_date );
}

char *transaction_time_append( char *transaction_date )
{
	char transaction_date_time[ 32 ];
	char *transaction_time;
	DATE *date;
	static char prior_transaction_time[ 32 ] = {0};

	if ( !transaction_date
	||   !*transaction_date
	||   strcmp( transaction_date, "transaction_date" ) == 0 )
	{
		transaction_date =
			 date_get_now_yyyy_mm_dd(
				date_get_utc_offset() );
	}

	if ( !*prior_transaction_time )
	{
		transaction_time =
			date_get_now_hh_colon_mm_colon_ss(
				date_get_utc_offset() );

		sprintf( transaction_date_time,
			 "%s %s",
			 transaction_date,
			 transaction_time );

		strcpy( prior_transaction_time, transaction_time );
	}
	else
	{
		sprintf( transaction_date_time,
			 "%s %s",
			 transaction_date,
			 prior_transaction_time );

		date = date_yyyy_mm_dd_hms_new(	transaction_date_time );
		date_increment_seconds( date, 1 );
		transaction_time = date_get_hms( date );

		sprintf( transaction_date_time,
			 "%s %s",
			 transaction_date,
			 transaction_time );

		strcpy( prior_transaction_time,
			transaction_time );

		date_free( date );
	}

	return strdup( transaction_date_time );
}

char *transaction_date_maximum( void )
{
	char sys_string[ 1024 ];
	char *select;
	char *results;
	char transaction_date_string[ 16 ];

	select = "max( transaction_date_time )";

	sprintf( sys_string,
		 "echo \"select %s from %s;\" | sql",
		 select,
		 "transaction" );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );
}

char *transaction_date_minimum( void )
{
	char sys_string[ 1024 ];
	char *select;
	char *results;
	char transaction_date_string[ 16 ];

	select = "min( transaction_date_time )";

	sprintf( sys_string,
		 "echo \"select %s from %s;\" | sql",
		 select,
		 "transaction" );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );
}

char *transaction_date_prior_closing_beginning(
			char *fund_name,
			char *as_of_date )
{
	return transaction_beginning_date_string(
			fund_name,
			as_of_date
				/* ending_transaction_date */ );
}

char *transaction_beginning_date_string(
			char *fund_name,
			char *ending_transaction_date )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;
	char folder[ 128 ];
	char *results;
	char transaction_date_string[ 16 ];
	DATE *prior_closing_transaction_date = {0};

	if ( fund_name && !*fund_name )
		fund_name = (char *)0;
	else
	if ( fund_name && strcmp( fund_name, "fund" ) == 0 )
		fund_name = (char *)0;

	/* Get the prior closing entry then return its tomorrow. */
	/* ----------------------------------------------------- */
	if ( ending_transaction_date )
	{
		prior_closing_transaction_date =
			transaction_prior_closing_transaction_date(
				fund_name,
				ending_transaction_date );
	
		if ( prior_closing_transaction_date )
		{
			date_increment_days(
				prior_closing_transaction_date,
				1.0,
				0 /* utc_offset */ );
	
			return date_get_yyyy_mm_dd_string(
					prior_closing_transaction_date );
		}
	}

	/* No closing entries */
	/* ------------------ */
	select = "min( transaction_date_time )";

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		/* Get the first entry for the fund. */
		/* --------------------------------- */
		sprintf(folder,
		 	"%s,%s",
		 	JOURNAL_FOLDER_NAME,
		 	ACCOUNT_FOLDER_NAME );

		sprintf(where,
		 	"fund = '%s' and				"
			"%s.account = %s.account			",
		 	fund_name,
			JOURNAL_FOLDER_NAME,
			ACCOUNT_FOLDER_NAME );
	}
	else
	{
		/* Get the first entry. */
		/* -------------------- */
		strcpy( folder, JOURNAL_FOLDER_NAME );

		strcpy( where, "1 = 1" );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );
}

double transaction_net_income(
			double total_revenues,
			double total_expenses,
			double total_gains,
			double total_losses )
{
	return (	total_revenues -
			total_expenses +
			total_gains -
			total_losses );
}

double transaction_net_income_fetch(
			char *fund_name,
			char *as_of_date )
{
	char sys_string[ 1024 ];
	char *results_string;

	sprintf(sys_string,
"income_statement process \"%s\" \"%s\" omit output_medium y",
		(fund_name) ? fund_name : "",
		as_of_date );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );
}

void transaction_journal_list_pipe_display(
			FILE *output_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *memo,
			LIST *journal_list )
{
	JOURNAL *journal;
	char buffer[ 256 ];
	char full_name_buffer[ 256 ];

	if ( list_length( journal_list ) != 2 )
	{
		fprintf( output_pipe,
			 "Expecting a debit and credit journal entry.\n" );
	}

	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		fprintf( output_pipe,
		 	 "Memo: %s\n",
		 	 memo );
	}

	list_rewind( journal_list );

	journal = list_get( journal_list );

	if ( strcmp( street_address, "null" ) != 0 )
	{
		sprintf( full_name_buffer,
			 "%s/%s",
			 full_name,
			 street_address );
	}
	else
	{
		strcpy( full_name_buffer, full_name );
	}

	fprintf( output_pipe,
		 "%s^%s^%.2lf^\n",
		 full_name_buffer,
		 format_initial_capital(
			buffer,
			journal->account_name ),
		 journal->debit_amount );

	list_next( journal_list );
	journal = list_get( journal_list );

	fprintf( output_pipe,
		 "%s^%s^^%.2lf\n",
		 transaction_date_time,
		 format_initial_capital(
			buffer,
			journal->account_name ),
		 journal->credit_amount );
}

LIST *transaction_after_balance_zero_journal_list(
			char *account_name )
{
	char *transaction_date_time_string;

	if ( ( transaction_date_time_string =
			transaction_latest_zero_balance_transaction_date_time(
				account_name ) ) )
	{
		DATE *transaction_date_time;

		transaction_date_time =
			date_yyyy_mm_dd_hms_new(
				transaction_date_time_string );

		/* Need to start with the transaction following zero balance. */
		/* ---------------------------------------------------------- */
		date_increment_seconds(
			transaction_date_time,
			1 );

		transaction_date_time_string =
			date_get_yyyy_mm_dd_hh_mm_ss(
				transaction_date_time );
	}

	return journal_minimum_account_journal_list(
			transaction_date_time_string
				/* minimum_transaction_date_time */,
			account_name );
}

char *transaction_latest_zero_balance_transaction_date_time(
			char *account_name )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *results;

	sprintf( where,
		 "account = '%s' and balance = 0",
		 account_name_escape( account_name ) );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 "max( transaction_date_time )",
		 JOURNAL_TABLE_NAME,
		 where );

	results = pipe2string( sys_string );

	if ( results && *results )
		return results;
	else
		return (char *)0;
}

