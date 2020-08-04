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
#include "folder.h"
#include "environ.h"
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
		journal_account_name_fetch(
			account_name,
			results /* transaction_date_time */ );
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

void journal_program_insert(	FILE *insert_pipe,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *program_name,
				char *account_name,
				double amount,
				boolean is_debit )
{
	if ( is_debit )
	{
		fprintf(	insert_pipe,
				"%s^%s^%s^%s^%s^%.2lf\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_escape_full_name( full_name ),
				street_address,
				transaction_date_time,
				program_name,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				account_escape_name( account_name ),
				amount );
	}
	else
	{
		fprintf(	insert_pipe,
				"%s^%s^%s^%s^%s^%.2lf\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_escape_full_name( full_name ),
				street_address,
				transaction_date_time,
				program_name,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				account_escape_name( account_name ),
				amount );
	}
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

/* Returns program memory */
/* ---------------------- */
char *journal_select( void )
{
	if ( journal_program_name_exists() )
	{
		return
"full_name,street_address,transaction_date_time,account_name,previous_balance,debit_amount,credit_amount,balance,program_name";
	}
	else
	{
		return
"full_name,street_address,transaction_date_time,account_name,previous_balance,debit_amount,credit_amount,balance";
	}
}

JOURNAL *journal_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char account_name[ 128 ];
	char piece_buffer[ 1024 ];
	JOURNAL *journal;

	if ( !input ) return (JOURNAL *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input, 2 );
	piece( account_name, SQL_DELIMITER, input, 3 );

	journal = journal_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( account_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	journal->previous_balance = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	journal->debit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	journal->credit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	journal->balance = atof( piece_buffer );

	if ( journal_program_name_exists() )
	{
		piece( piece_buffer, SQL_DELIMITER, input, 7 );
		journal->program = program_new( strdup( piece_buffer ) );
	}

	return journal;
}

LIST *journal_parse_sys_string(	char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	JOURNAL *journal;
	LIST *journal_list;

	journal_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		if ( ( journal = journal_parse( input ) ) )
		{
			list_set( journal_list, journal );
		}
	}

	pclose( input_pipe );
	return journal_list;
}

JOURNAL *journal_account_name_fetch(
				char *account_name,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf( where,
		 "account = '%s' and			"
		 "transaction_date_time = '%s' 		",
		 account_escape_name( account_name ),
		 transaction_date_time );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 journal_select(),
		 JOURNAL_FOLDER_NAME,
		 where );

	return journal_parse( pipe2string( sys_string ) );
}

LIST *journal_list_account_name(
			char *account_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf( where,
		 "account = '%s' 			",
		 account_escape_name( account_name ) );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 journal_select(),
		 JOURNAL_FOLDER_NAME,
		 where );

	return journal_parse_sys_string( sys_string );
}

/* Also does a propagate for each account */
/* -------------------------------------- */
void journal_delete(	char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
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
		 JOURNAL_FOLDER_NAME,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s\n",
			full_name,
			street_address,
			transaction_date_time );

	pclose( output_pipe );

	journal_account_name_list_propagate(
			transaction_date_time,
			account_name_list );
}

void journal_list_transaction_date_time_propagate(
			char *transaction_date_time,
			LIST *journal_list )
{
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return;

	do {
		journal = list_get( journal_list );

		journal_account_name_propagate(
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

		journal_account_name_propagate(
			transaction_date_time,
			account_name );

	} while( list_next( account_name_list ) );
}

void journal_account_name_propagate(
				char *transaction_date_time,
				char *account_name )
{
	JOURNAL *prior;

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

	prior =
		journal_prior(
			transaction_date_time,
			account_name );

	journal_list_transaction_date_time_propagate(
		transaction_date_time,
		journal_prior_propagate_journal_list(
			prior,
			account_name ) );
}

boolean journal_program_name_exists( void )
{
	static int exists = -1;

	if ( exists == -1 )
	{
		exists =
			folder_exists_attribute(
				environment_application(),
				JOURNAL_FOLDER_NAME,
				"program_name" /* attribute_name */ );
	}
	return exists;
}

LIST *journal_account_name_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 1024 ];

	select = "account";

	sprintf(where,
		"full_name = '%s' and		"
		"street_address = '%s' and	"
		"transaction_date_time = '%s'	",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_escape_full_name( full_name ),
		street_address,
		transaction_date_time );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 select,
		 JOURNAL_FOLDER_NAME,
		 where,
		 select );

	return pipe2list( sys_string );
}

LIST *journal_list( char *where_clause )
{
	char sys_string[ 1024 ];

	if ( !where_clause ) return (LIST *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 journal_select(),
		 JOURNAL_FOLDER_NAME,
		 where_clause,
		 journal_select() );

	return journal_parse_sys_string( sys_string );
}

LIST *journal_minimum_transaction_date_time_list(
			char *minimum_transaction_date_time,
			char *account_name )
{
	char where_clause[ 1024 ];

	sprintf(where_clause,
		"transaction_date_time >= '%s' and		"
		"account = '%s'					",
		minimum_transaction_date_time,
		account_name_escape( account_name ) );

	return journal_list( where_clause );
}

LIST *journal_prior_propagate_journal_list(
			JOURNAL *prior,
			char *account_name )
{
	LIST *journal_list = {0};
	boolean accumulate_debit;
	JOURNAL *first_journal;

	if ( !account_name ) return (LIST *)0;

	accumulate_debit =
		account_accumulate_debit(
			account_name );

	if ( prior )
	{
		journal_list =
			journal_minimum_transaction_date_time_list(
				prior->transaction_date_time
					/* minimum_transaction_date_time */,
				account_name );
	}
	else
	/* ------------------------------------------------- */
	/* Otherwise doing period of record for the account. */
	/* ------------------------------------------------- */
	{
		journal_list =
			journal_list_account_name(
				account_name );

		if ( !list_length( journal_list ) ) return (LIST *)0;

		first_journal = list_get_first_pointer( journal_list );
		first_journal->previous_balance = 0.0;
		first_journal->transaction_count = 1;
	}

	/* If deleted the latest one, then no propagate. */
	/* --------------------------------------------- */
	if ( prior && list_length( journal_list ) == 1 )
	{
		return (LIST *)0;
	}

	journal_list_propagate(	journal_list,
				accumulate_debit );

	return journal_list;
}

void journal_list_propagate(
			LIST *journal_list,
			boolean accumulate_debit )
{
	JOURNAL *journal;
	JOURNAL *prior_journal = {0};
	JOURNAL *first_journal = {0};
	int transaction_count;

	if ( !list_rewind( journal_list ) ) return;

	first_journal = list_get_first_pointer( journal_list );
	transaction_count = first_journal->transaction_count;

	do {
		journal = list_get( journal_list );

		if ( prior_journal )
		{
			journal->previous_balance = prior_journal->balance;
		}

		if ( accumulate_debit )
		{
			if ( journal->debit_amount )
			{
				journal->balance =
					journal->previous_balance +
					journal->debit_amount;
			}
			else
			if ( journal->credit_amount )
			{
				journal->balance =
					journal->previous_balance -
					journal->credit_amount;

			}
		}
		else
		{
			if ( journal->credit_amount )
			{
				journal->balance =
					journal->previous_balance +
					journal->credit_amount;

			}
			else
			if ( journal->debit_amount )
			{
				journal->balance =
					journal->previous_balance -
					journal->debit_amount;
			}
		}

		if ( timlib_dollar_virtually_same(
			journal->balance,
			0.0 ) )
		{
			journal->transaction_count = 0;
			transaction_count = 1;
		}
		else
		{
			journal->transaction_count = transaction_count;
			transaction_count++;
		}

		prior_journal = journal;

	} while( list_next( journal_list ) );
}

