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
#include "transaction.h"

TRANSACTION *transaction_new(	char *full_name,
				char *street_address,
				char *transaction_date_time )
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
	return transaction;
}

char *transaction_program_select( void )
{
	return
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn,program_name";
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

char *transaction_select( void )
{
	return
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,lock_transaction_yn";
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

TRANSACTION *transaction_fetch(
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];

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

	return transaction_parse( pipe2string( sys_string ) );
}

char *transaction_escape_memo( char *memo )
{
	static char escape_memo[ 1024 ];

	string_escape_quote( escape_memo, memo );
	return escape_memo;
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
