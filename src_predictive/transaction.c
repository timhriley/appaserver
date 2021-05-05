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

LIST *transaction_system_list(
			char *sys_string,
			boolean fetch_journal_list )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *transaction_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			transaction_list,
			transaction_parse(
				input,
				fetch_journal_list ) );
	}

	pclose( input_pipe );
	return transaction_list;
}

char *transaction_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh \"%s\" %s \"%s\" transaction_date_time",
		 transaction_select(),
		 TRANSACTION_TABLE,
		 where );

	return strdup( sys_string );
}

LIST *transaction_list_fetch(
			char *where,
			boolean fetch_journal_list )
{
	return transaction_system_list(
			transaction_sys_string(
				where ),
			fetch_journal_list );
}

TRANSACTION *transaction_fetch(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	if ( !full_name
	||   !street_address
	||   !transaction_date_time
	||   !*transaction_date_time )
	{
		return (TRANSACTION *)0;
	}

	return transaction_parse(
			pipe2string(
				transaction_sys_string(
		 			/* -------------------------- */
		 			/* Safely returns heap memory */
		 			/* -------------------------- */
		 			transaction_primary_where(
						full_name,
						street_address,
						transaction_date_time ) ) ),
			1 /* fetch_joural_list */ );
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

	if ( fetch_journal_list )
	{
		transaction->journal_list =
			journal_system_list(
				journal_system_string(
					transaction_primary_where(
						full_name,
						street_address,
						transaction_date_time ),
					0 /* not fetch_check_number */,
					0 /* not fetch_memo */ ),
				0 /* not fetch_check_number */,
				0 /* not fetch_memo */ );
	}

	return transaction;
}

char *transaction_full_name_escape( char *full_name )
{
	return transaction_escape_full_name( full_name );
}

char *transaction_escape_full_name( char *full_name )
{
	static char escape_full_name[ 256 ];

	if ( !full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty full_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

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

FILE *transaction_insert_open(
			boolean replace )
{
	char sys_string[ 1024 ];
	char *field;

	field =	"full_name,"
		"street_address,"
		"transaction_date_time,"
		"transaction_amount,"
		"memo,"
		"check_number,"
		"lock_transaction_yn";

	sprintf( sys_string,
		 "insert_statement t=%s f=%s replace=%c delimiter='^'	|"
		 "tee_appaserver_error.sh				|"
		 "sql 2>&1						 ",
		 TRANSACTION_TABLE,
		 field,
		 (replace) ? 'y' : 'n' );

	return popen( sys_string, "w" );
}

FILE *transaction_property_insert_open(
			boolean replace )
{
	char sys_string[ 1024 ];
	char *field;

	field =	"full_name,"
		"street_address,"
		"transaction_date_time,"
		"transaction_amount,"
		"memo,"
		"check_number,"
		"lock_transaction_yn,"
		"property_street_address";

	sprintf( sys_string,
		 "insert_statement t=%s f=%s replace=%c delimiter='^'	|"
		 "tee_appaserver_error.sh				|"
		 "sql 2>&1						 ",
		 TRANSACTION_TABLE,
		 field,
		 (replace) ? 'y' : 'n' );

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
			LIST *journal_list,
			boolean replace )
{
	transaction_date_time =
		transaction_insert(
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction,
			replace );

	journal_account_name_list_propagate(
		transaction_date_time,
		/* ------------------------- */
		/* Returns account_name_list */
		/* ------------------------- */
		journal_list_insert(
			full_name,
			street_address,
			transaction_date_time,
			journal_list,
			replace ) );

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
			boolean lock_transaction,
			boolean replace )
{
	FILE *insert_pipe;

	if ( !full_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty full_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	if ( !street_address )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty street_address.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	if ( !transaction_date_time )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty transaction_date_time.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	insert_pipe = transaction_insert_open( replace );

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
			/* Returns memo or "" */
			/* ------------------ */
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

char *transaction_memo( char *memo )
{
	if ( !memo || strcmp( memo, "memo" ) == 0 )
	{
		memo = "";
	}

	if ( strlen( memo ) > TRANSACTION_MEMO_LENGTH )
	{
		*( memo + TRANSACTION_MEMO_LENGTH ) = '\0';
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
			(property_street_address)
				? property_street_address
				: "",
			transaction_amount,
			/* ------------------ */
			/* Returns memo or "" */
			/* ------------------ */
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
			char *transaction_date_time,
			char *program_name,
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
			(program_name) ? program_name : "",
			transaction_amount,
			/* ------------------ */
			/* Returns memo or "" */
			/* ------------------ */
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

	while ( transaction_date_time_exists( transaction_date_time ) )
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
			char *transaction_date,
			char *transaction_time )
{
	char static where[ 128 ];

	sprintf(where,
		"transaction_date_time = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time(
			transaction_date,
			transaction_time ) );

	return where;
}

char *transaction_closing_memo_where( void )
{
	char static where[ 128 ];

	sprintf(where,
		"memo = '%s'",
		TRANSACTION_CLOSING_ENTRY_MEMO );

	return where;
}

boolean transaction_date_time_exists( char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *results;
	boolean return_value;

	sprintf( sys_string,
		 "select.sh \"%s\" %s \"%s\" none",
		 "count(1)",
		 TRANSACTION_TABLE,
		 transaction_date_time );

	results = pipe2string( sys_string );
	return_value = ( atoi( results ) == 1 );
	free( results );
	return return_value;
}

boolean transaction_closing_entry_exists(
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *results;
	boolean return_value;
	char where[ 256 ];
	char transaction_date[ 16 ];

	column( transaction_date, 0, transaction_date_time );

	sprintf(where,
		"%s and %s",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 transaction_date_time_where(
			transaction_date,
			TRANSACTION_CLOSING_TRANSACTION_TIME ),
		 transaction_closing_memo_where() );

	sprintf( sys_string,
		 "select.sh \"%s\" %s \"%s\" none",
		 "count(1)",
		 TRANSACTION_TABLE,
		 where );

	results = pipe2string( sys_string );
	return_value = ( atoi( results ) == 1 );
	free( results );
	return return_value;
}

char *transaction_date_time_closing(
			char *transaction_date,
			boolean preclose_time,
			boolean closing_entry_exists )
{
	static char date_time[ 32 ];

	if ( !closing_entry_exists )
	{
		sprintf(date_time,
			"%s %s",
			transaction_date,
			TRANSACTION_CLOSING_TRANSACTION_TIME );
	}
	else
	if ( preclose_time )
	{
		sprintf(date_time,
			"%s %s",
			transaction_date,
			TRANSACTION_PRECLOSE_TRANSACTION_TIME );
	}
	else
	{
		sprintf(date_time,
			"%s %s",
			transaction_date,
			TRANSACTION_CLOSING_TRANSACTION_TIME );
	}

	return date_time;
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
	/* Note: transaction_date_time shouldn't change. */
	/* --------------------------------------------- */
	transaction_date_time =
		transaction_insert(
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction,
			1 /* replace */ );

	journal_account_name_list_propagate(
		transaction_date_time,
		/* ------------------------- */
		/* Returns account_name_list */
		/* ------------------------- */
		journal_list_insert(
			full_name,
			street_address,
			transaction_date_time,
			journal_list,
			1 /* replace */ ) );

	return transaction_date_time;
}

void transaction_list_journal_list_insert(
			LIST *transaction_list )
{
	transaction_list_journal_insert( transaction_list );
}

void transaction_delete(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;

	if ( !full_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty full_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	if ( !street_address )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty street_address.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	if ( !transaction_date_time )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty transaction_date_time.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	field= "full_name,street_address,transaction_date_time";

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "tee_appaserver_error.sh				|"
		 "sql							 ",
		 TRANSACTION_TABLE,
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

		if ( transaction_date_time )
		{
			if ( string_strcmp(	t->full_name,
						full_name ) == 0
			&&   string_strcmp(	t->street_address,
						street_address ) == 0
			&&   string_strcmp(	t->transaction_date_time,
						transaction_date_time ) == 0 )
			{
				return t;
			}
		}
		else
		{
			if ( string_strcmp(	t->full_name,
						full_name ) == 0
			&&   string_strcmp(	t->street_address,
						street_address ) == 0 )
			{
				return t;
			}
		}

	} while( list_next( transaction_list ) );
	return (TRANSACTION *)0;
}

void transaction_list_stderr( LIST *transaction_list )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return;

	do {
		transaction = list_get( transaction_list );
		transaction_stderr( transaction );

	} while( list_next( transaction_list ) );
}

void transaction_stderr( TRANSACTION *transaction )
{
	fprintf( stderr,
		 "%s\n",
		 journal_list_display(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			transaction->memo,
			transaction->journal_list ) );
}

/* Returns transaction_list with transaction_date_time changed if needed. */
/* ---------------------------------------------------------------------- */
LIST *transaction_list_insert(
			LIST *transaction_list,
			boolean lock_transaction )
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
				lock_transaction,
				0 /* not replace */ );

	} while( list_next( transaction_list ) );

	transaction_list_journal_insert( transaction_list );

	return transaction_list;
}

void transaction_amount_fetch_update(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	TRANSACTION *transaction;

	if ( ! ( transaction =
			/* Also fetches journal_list */
			/* ------------------------- */
			transaction_fetch(
				full_name,
				street_address,
				transaction_date_time ) ) )
	{
		return;
	}

	transaction->transaction_amount =
		journal_transaction_amount(
			transaction->journal_list );

	transaction_update(
			transaction->transaction_amount,
			full_name,
			street_address,
			transaction_date_time );
}

void transaction_update(
			double transaction_amount,
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
		 TRANSACTION_TABLE,
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
			double transaction_amount,
			char *memo,
			char *debit_account,
			char *credit_account )
{
	TRANSACTION *transaction;

	if ( !transaction_amount ) return (TRANSACTION *)0;

	transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_time );

	transaction->memo = strdup( memo );
	transaction->transaction_amount = transaction_amount;

	transaction->journal_list =
		journal_binary_journal_list(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			debit_account,
			credit_account );

	return transaction;
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

void transaction_report_title_sub_title(
		char *title,
		char *sub_title,
		char *process_name,
		LIST *fund_name_list,
		char *begin_date_string,
		char *as_of_date,
		char *logo_filename )
{
	char begin_date_american[ 32 ];
	char end_date_american[ 32 ];
	char *fund_name = {0};

	*begin_date_american = '\0';

	sprintf(title,
		"%s",
		application_title(
			environment_application() ) );

	date_convert_source_international(
		begin_date_american,
		american,
		begin_date_string );

	date_convert_source_international(
		end_date_american,
		american,
		as_of_date );

	if ( list_length( fund_name_list ) ==  1 )
	{
		fund_name = list_first( fund_name_list );
	}

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
	if ( list_length( fund_name_list ) > 1 )
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
}

DATE *transaction_prior_closing_transaction_date(
			char *as_of_date )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;
	char *results;
	char ending_transaction_date_time[ 32 ];
	char transaction_date_string[ 16 ];
	DATE *prior_closing_transaction_date = {0};

	select = "max( transaction_date_time )";

	sprintf( ending_transaction_date_time,
		 "%s %s",
		 as_of_date,
		 TRANSACTION_CLOSING_TRANSACTION_TIME );

	sprintf( where,
		 "memo = '%s' and transaction_date_time < '%s'",
		 TRANSACTION_CLOSING_ENTRY_MEMO,
		 ending_transaction_date_time );

	sprintf( sys_string,
		 "select.sh \"%s\" %s \"%s\" none",
		 select,
		 TRANSACTION_TABLE,
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

	if ( ! ( r = transaction_date_time_max() ) ) return (char *)0;

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
		 "select.sh \"%s\" %s",
		 select,
		 TRANSACTION_TABLE );

	return pipe2string( sys_string );
}

LIST *transaction_fund_name_list( void )
{
	char *sys_string;

	if ( !predictive_fund_attribute_exists() )
	{
		return (LIST *)0;
	}

	sys_string = "select.sh fund fund where fund";

	return pipe2list( sys_string );
}

char *transaction_journal_join( void )
{
	char *join_where;

	join_where =	"journal_ledger.full_name =			"
			"	transaction.full_name and		"
			"journal_ledger.street_address =		"
			"	transaction.street_address and		"
			"journal_ledger.transaction_date_time =		"
			"	transaction.transaction_date_time	";

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
			 date_now_yyyy_mm_dd(
				date_utc_offset() );
	}

	if ( !*prior_transaction_time )
	{
		transaction_time =
			date_now_hh_colon_mm_colon_ss(
				date_utc_offset() );

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
		 TRANSACTION_TABLE );

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
		 TRANSACTION_TABLE );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );
}

char *transaction_date_prior_closing_beginning(
			char *as_of_date )
{
	return transaction_beginning_date_string(
			as_of_date
				/* ending_transaction_date */ );
}

char *transaction_prior_close_beginning_date(
			char *as_of_date )
{
	return transaction_beginning_date_string(
			as_of_date
				/* ending_transaction_date */ );
}

char *transaction_beginning_date_string(
			char *ending_transaction_date )
{
	char sys_string[ 1024 ];
	char *select;
	char *results;
	char transaction_date_string[ 16 ];
	DATE *prior_closing_transaction_date = {0};

	/* Get the prior closing entry then return its tomorrow. */
	/* ----------------------------------------------------- */
	if ( ending_transaction_date )
	{
		prior_closing_transaction_date =
			transaction_prior_closing_transaction_date(
				ending_transaction_date );
	
		if ( prior_closing_transaction_date )
		{
			date_increment_days(
				prior_closing_transaction_date,
				1.0 );
	
			return date_get_yyyy_mm_dd_string(
					prior_closing_transaction_date );
		}
	}

	/* No closing entries */
	/* ------------------ */
	select = "min( transaction_date_time )";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		",
		 environment_application_name(),
		 select,
		 JOURNAL_TABLE );

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
	char buffer[ 128 ];

	sprintf(sys_string,
"income_statement session login_name role process \"%s\" \"%s\" 0 '' '' '' y",
		(fund_name) ? fund_name : "",
		column(
			buffer,
			0,
			as_of_date ) );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );
}

char *transaction_full_name_display(
			char *full_name,
			char *street_address )
{
	static char full_name_display[ 256 ];

	*full_name_display = '\0';

	if ( !full_name || !street_address ) return full_name_display;

	if ( *street_address
	&&   string_loose_strcmp( street_address, "null" ) != 0
	&&   string_loose_strcmp( street_address, "any" ) != 0
	&&   string_loose_strcmp( street_address, "unknown" ) != 0 )
	{
		sprintf(full_name_display,
			"%s/%s",
			full_name,
			street_address );
	}
	else
	{
		string_strcpy( full_name_display, full_name, 256 );
	}

	return full_name_display;
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

	fprintf( output_pipe,
		 "%s^%s^%.2lf^\n",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 transaction_full_name_display(
			full_name,
			street_address ),
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
		 JOURNAL_TABLE,
		 where );

	results = pipe2string( sys_string );

	if ( results && *results )
		return results;
	else
		return (char *)0;
}

LIST *transaction_date_time_account_name_list(
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf(	where,
			"transaction_date_time >= '%s'",
			transaction_date_time );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 "distinct account",
		 JOURNAL_TABLE,
		 where,
		 "account" );

	return pipe2list( sys_string );
}

char *transaction_program_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *program_name,
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

	journal_ledger_delete(
		full_name,
		street_address,
		transaction_date_time );

	transaction_date_time =
		transaction_program_insert(
			full_name,
			street_address,
			transaction_date_time,
			program_name,
			transaction_amount,
			memo,
			check_number,
			lock_transaction,
			0 /* not replace */ );

	journal_account_name_list_propagate(
		transaction_date_time,
		/* ------------------------- */
		/* Returns account_name_list */
		/* ------------------------- */
		journal_list_insert(
			full_name,
			street_address,
			transaction_date_time,
			journal_list,
			0 /* not replace */ ) );

	return transaction_date_time;
}

void transaction_list_journal_insert(
			LIST *transaction_list )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return;

	do {
		transaction =
			list_get(
				transaction_list );

		journal_account_name_list_propagate(
			transaction->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_list_insert(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->journal_list,
				0 /* not replace */ ) );

	} while ( list_next( transaction_list ) );
}

void transaction_list_html_display(
			LIST *transaction_list )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return;

	do {
		transaction =
			list_get(
				transaction_list );

		journal_list_html_display(
			transaction->journal_list,
			transaction->memo );

	} while ( list_next( transaction_list ) );

}

/* TRANSACTION with program_name addition */
/* -------------------------------------- */
FILE *transaction_program_insert_open( boolean replace )
{
	char sys_string[ 1024 ];
	char *field;

	field =	"full_name,"
		"street_address,"
		"transaction_date_time,"
		"program_name,"
		"transaction_amount,"
		"memo,"
		"check_number,"
		"lock_transaction_yn";

	sprintf( sys_string,
		 "insert_statement t=%s f=%s replace=%c delimiter='^'	|"
		 "tee_appaserver_error.sh				|"
		 "sql 2>&1						 ",
		 TRANSACTION_TABLE,
		 field,
		 (replace) ? 'y' : 'n' );

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
			boolean lock_transaction,
			boolean replace )
{
	FILE *insert_pipe;

	insert_pipe = transaction_program_insert_open( replace );

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

/* ---------------------------------------------- */
/* Returns account_name_list			  */
/* ---------------------------------------------- */
/* Note: transaction_date_time changed if needed. */
/* ---------------------------------------------- */
LIST *transaction_list_journal_program_insert(
			char **first_transaction_date_time,
			LIST *transaction_list,
			boolean replace )
{
	TRANSACTION *transaction;
	FILE *insert_pipe;
	LIST *account_name_list = list_new();

	if ( !list_length( transaction_list ) ) return (LIST *)0;

	if ( !first_transaction_date_time ) return (LIST *)0;

	if ( replace )
	{
		list_append_unique_string_list(
			account_name_list /* destination_list */,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			transaction_list_delete(
				transaction_list ) );
	}

	list_rewind( transaction_list );
	*first_transaction_date_time = (char *)0;

	insert_pipe = transaction_program_insert_open( replace );

	do {
		transaction =
			list_get(
				transaction_list );

		transaction->transaction_date_time =
			transaction_program_insert_pipe(
				insert_pipe,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->program_name,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				1 /* lock_transaction */ );

		if ( !*first_transaction_date_time )
		{
			*first_transaction_date_time =
				transaction->
					transaction_date_time;
		}

	} while ( list_next( transaction_list ) );

	pclose( insert_pipe );

	insert_pipe = journal_insert_open( replace );

	list_rewind( transaction_list );

	do {
		transaction =
			list_get(
				transaction_list );

		list_append_unique_string_list(
			account_name_list /* destination_list */,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_list_insert_pipe(
				insert_pipe,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->journal_list )
					/* source_list */ );

	} while ( list_next( transaction_list ) );

	pclose( insert_pipe );

	return account_name_list;
}

LIST *transaction_list_delete(
			LIST *transaction_list )
{
	TRANSACTION *transaction;
	LIST *account_name_list;

	if ( !list_rewind( transaction_list ) ) return (LIST *)0;

	account_name_list = list_new();

	do {
		transaction = list_get( transaction_list );

		transaction_delete(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time );

		list_append_list(
			account_name_list,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time ) );

	} while ( list_next( transaction_list ) );
	return account_name_list;
}

char *transaction_list_minimum_transaction_date_time(
			LIST *transaction_list )
{
	TRANSACTION *transaction;
	char *minimum_transaction_date_time = {0};

	if ( !list_rewind( transaction_list ) ) return (char *)0;

	do {
		transaction = list_get( transaction_list );

		if ( !minimum_transaction_date_time )
		{
			minimum_transaction_date_time =
				transaction->transaction_date_time;
		}
		else
		if ( strcmp(
			transaction->transaction_date_time,
 			minimum_transaction_date_time ) < 0 )
		{
			minimum_transaction_date_time =
				transaction->transaction_date_time;
		}

	} while ( list_next( transaction_list ) );

	return minimum_transaction_date_time;
}

char *transaction_date_time_earlier(
			char *transaction_date_time,
			char *preupdate_transaction_date_time )
{
	if ( !preupdate_transaction_date_time
	||   !*preupdate_transaction_date_time
	||   strcmp(	preupdate_transaction_date_time,
			"preupdate_transaction_date_time" ) == 0 )
	{
		return transaction_date_time;
	}

	if ( strcmp(	transaction_date_time,
			preupdate_transaction_date_time ) <= 0 )
	{
		return transaction_date_time;
	}
	else
	{
		return preupdate_transaction_date_time;
	}
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

