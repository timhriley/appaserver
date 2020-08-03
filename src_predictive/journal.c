/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/journal.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "boolean.h"
#include "transaction.h"
#include "account.h"
#include "journal.h"

JOURNAL *journal_new(		char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name )
{
	JOURNAL *journal;

	if ( ! ( journal = calloc( 1, sizeof( JOURNAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	journal->full_name = full_name;
	journal->street_address = street_address;
	journal->transaction_date_time = transaction_date_time;
	journal->account_name = account_name;
	return journal;
}

JOURNAL *journal_prior(		char *transaction_date_time,
				char *account_name )
{
	JOURNAL *journal;
	char where[ 512 ];
	char *select;
	char sys_string[ 1024 ];
	char *results;

	if ( !transaction_date_time || !*transaction_date_time )
	{
		return (JOURNAL *)0;
	}

	sprintf( where,
		 "account = '%s' and transaction_date_time < '%s'",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 account_escape_name( account_name ),
		 transaction_date_time );

	select = "max( transaction_date_time )";

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 select,
		 "journal",
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (JOURNAL *)0;

	journal =
		journal_transaction_date_time_fetch(
			results /* transaction_date_time */,
			account_name );

	return journal;
}

FILE *journal_insert_pipe( void )
{
	char sys_string[ 1024 ];
	char *field;

	field=
"full_name,street_address,transaction_date_time,account,debit_amount,credit_amount";

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 "journal",
		 field );

	return popen( sys_string, "w" );
}

void journal_insert(		FILE *insert_pipe,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name,
				double amount,
				boolean is_debit )
{
	if ( is_debit )
	{
		fprintf(	insert_pipe,
				"%s^%s^%s^%s^%.2lf\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_escape_full_name( full_name ),
				street_address,
				transaction_date_time,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				account_escape_name( account_name ),
				amount );
	}
	else
	{
		fprintf(	insert_pipe,
				"%s^%s^%s^%s^%.2lf\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_escape_full_name( full_name ),
				street_address,
				transaction_date_time,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				account_escape_name( account_name ),
				amount );
	}
}

char *journal_select( void )
{
	return
"full_name,street_address,transaction_date_time,account_name,previous_balance,debit_amount,credit_amount,balance";
}

JOURNAL *journal_parse( char *input_buffer )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char account_name[ 128 ];
	char piece_buffer[ 1024 ];
	JOURNAL *journal;

	if ( !input_buffer ) return (JOURNAL *)0;

	piece( full_name, SQL_DELIMITER, input_buffer, 0 );
	piece( street_address, SQL_DELIMITER, input_buffer, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input_buffer, 2 );
	piece( account_name, SQL_DELIMITER, input_buffer, 3 );

	journal = journal_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( account_name ) );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 3 );
	journal->previous_balance = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 4 );
	journal->debit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 5 );
	journal->credit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 6 );
	journal->balance = atof( piece_buffer );

	return journal;
}

JOURNAL *journal_transaction_date_time_fetch(
				char *transaction_date_time,
				char *account_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf( where,
		 "transaction_date_time = '%s' and	"
		 "account = '%s'			",
		 transaction_date_time,
		 account_escape_name( account_name ) );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 journal_select(),
		 "journal_ledger",
		 where );

	return journal_parse( pipe2string( sys_string ) );
}

void journal_delete(		char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;
	LIST *account_name_list;

	account_name_list =
		journal_account_name_list(
			full_name,
			street_address,
			transaction_date_time );

	field= "full_name,street_address,transaction_date_time";

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 "journal_ledger",
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s\n",
			full_name,
			street_address,
			transaction_date_time );

	pclose( output_pipe );
}

void journal_ledger_list_propagate(
			char *transaction_date_time,
			LIST *journal_ledger_list )
{
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return;

	do {
		journalx = list_get( journal_list );

		journal_account_propagate(
			transaction_date_time,
			journal->account_name );

	} while( list_next( journal_list ) );
}

void journal_account_name_list_propagate(
			char *transaction_date_time,
			LIST *account_name_list )
{
	char *account_name;

	if ( !list_rewind( account_name_list ) ) return;

	do {
		account_name = list_get( account_name_list );

		journal_account_propagate(
			transaction_date_time,
			account_name );

	} while( list_next( account_name_list ) );
}

void journal_account_propagate(		char *transaction_date_time,
					char *account_name )
{
	JOURNAL *prior_journal;
	LIST *propagate_journal_list;

	if ( !account_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty account_name: %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 transaction_date_time );
		exit( 1 );
	}

	prior_journal =
		journal_prior(
			transaction_date_time,
			account_name );

	propagate_journal_list =
		journal_propagate_journal_list(
			prior_ledger,
			account_name );

	journal_ledger_list_propagate(
			propagate_journal_list );
}

