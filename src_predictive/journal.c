/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/journal.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "float.h"
#include "String.h"
#include "list.h"
#include "date.h"
#include "sql.h"
#include "piece.h"
#include "environ.h"
#include "boolean.h"
#include "entity.h"
#include "predictive.h"
#include "account.h"
#include "transaction.h"
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

char *journal_max_prior_where(
			char *transaction_date_time,
			char *account_name )
{
	static char where[ 128 ];

	if ( !transaction_date_time
	||   !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"transaction_date_time < '%s' and account = '%s'",
		transaction_date_time,
		account_name );

	return where;
}

char *journal_max_prior_transaction_date_time(
			char *journal_max_prior_where,
			char *journal_table )
{
	char system_string[ 1024 ];

	if ( !journal_max_prior_where
	||   !journal_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		"max( transaction_date_time )",
		journal_table,
		journal_max_prior_where );

	return string_pipe( system_string );
}

JOURNAL *journal_prior(	char *transaction_date_time,
			char *account_name )
{
	if ( !transaction_date_time
	||   !account_name )
	{
		return (JOURNAL *)0;
	}

	return
	journal_account_fetch(
		journal_max_prior_transaction_date_time(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_max_prior_where(
				transaction_date_time,
				account_name ),
			JOURNAL_TABLE )
				/* transaction_date_time */,
		account_name );
}

JOURNAL *journal_latest(
			char *transaction_date_time_closing,
			char *account_name )
{
	if ( !transaction_date_time_closing
	||   !account_name )
	{
		return (JOURNAL *)0;
	}

	return
	journal_account_fetch(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		journal_max_transaction_date_time(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_max_where(
				transaction_date_time_closing,
				account_name ),
			JOURNAL_TABLE )
				/* transaction_date_time */,
		account_name );
}

char *journal_max_where(
			char *transaction_date_time_closing,
			char *account_name )
{
	static char where[ 128 ];

	if ( !transaction_date_time_closing
	||   !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"transaction_date_time <= '%s' and account = '%s'",
		transaction_date_time_closing,
		account_name );

	return where;
}

char *journal_max_transaction_date_time(
			char *journal_max_where,
			char *journal_table )
{
	char system_string[ 1024 ];

	if ( !journal_max_where
	||   !journal_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		"max( transaction_date_time )",
		journal_table,
		journal_max_where );

	return string_pipe( system_string );
}

char *journal_transaction_account_where(
			char *transaction_date_time,
			char *account_name )
{
	static char where[ 128 ];

	if ( !transaction_date_time
	||   !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( where,
		 "transaction_date_time = '%s' and "
		 "account = '%s'",
		 transaction_date_time,
		 account_name );

	return where;
}

JOURNAL *journal_account_fetch(
			char *transaction_date_time,
			char *account_name )
{
	if ( !transaction_date_time
	||   !account_name )
	{
		return (JOURNAL *)0;
	}

	return
	journal_parse(
		string_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				journal_transaction_account_where(
					transaction_date_time,
					account_name ) ) ),
		0 /* not fetch_check_number */,
		0 /* not fetch_memo */ );
}

JOURNAL *journal_parse(	char *input,
			boolean fetch_check_number,
			boolean fetch_memo )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char account_name[ 128 ];
	char piece_buffer[ 1024 ];
	JOURNAL *journal;

	if ( !input || !*input ) return (JOURNAL *)0;

	/* See JOURNAL_SELECT */
	/* ------------------ */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input, 2 );
	piece( account_name, SQL_DELIMITER, input, 3 );

	journal =
		journal_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( account_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	journal->previous_balance = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	journal->debit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	journal->credit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	journal->balance = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	journal->transaction_count = atoi( piece_buffer );

	if ( fetch_check_number )
	{
		journal->check_number =
			atoi(
				journal_transaction_cell_fetch(
					full_name,
					street_address,
					transaction_date_time,
					"check_number" ) );
	}
	else
	if ( fetch_memo )
	{
		journal->memo =
			journal_transaction_cell_fetch(
				full_name,
				street_address,
				transaction_date_time,
				"memo" );
	}

	return journal;
}

LIST *journal_system_list(
			char *system_string,
			boolean fetch_check_number,
			boolean fetch_memo )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *system_list;

	if ( !system_string ) return (LIST *)0;

	system_list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			system_list,
			journal_parse(
				input,
				fetch_check_number,
				fetch_memo ) );
	}

	pclose( input_pipe );
	return system_list;
}

char *journal_transaction_cell_fetch(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *attribute_name )
{
	if ( !full_name
	||   !street_address
	||   !transaction_date_time
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	string_pipe(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_transaction_system_string(
			attribute_name,
			TRANSACTION_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_primary_where(
				full_name,
				street_address,
				transaction_date_time ) ) );
}

char *journal_following_latest_zero_balance_transaction_date_time(
			char *account_name,
			char *journal_table )
{
	DATE *transaction_date_time;
	char where[ 512 ];
	char system_string[ 1024 ];
	char *results;

	sprintf(where,
		"account = '%s' and balance = 0",
		account_name );

	sprintf(system_string,
		"echo \"select %s from %s where %s;\" | sql",
		"max( transaction_date_time )",
		journal_table,
		where );

	results = string_pipe( system_string );

	if ( results && *results )
	{
		transaction_date_time =
			date_yyyy_mm_dd_hms_new(
				results );

		date_increment_seconds(
			transaction_date_time,
			1 );

		return
		date_get_yyyy_mm_dd_hh_mm_ss(
			transaction_date_time );
	}
	else
	{
		return (char *)0;
	}
}

LIST *journal_account_journal_list(
			char *account_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time )
{
	if ( !account_name
	||   !begin_transaction_date_time
	||   !end_transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	journal_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_system_string(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_account_where(
				account_name,
				begin_transaction_date_time,
				end_transaction_date_time ) ),
		0 /* not fetch_check_number */,
		0 /* not fetch_memo */ );
}

char *journal_account_where(
			char *account_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time )
{
	static char where[ 128 ];

	if ( !account_name
	||   !begin_transaction_date_time
	||   !end_transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"account = '%s' and				"
		"transaction_date_time between '%s' and '%s'	",
		account_name,
		begin_transaction_date_time,
		end_transaction_date_time );

	return where;
}

void journal_propagate(	char *transaction_date_time,
			char *account_name )
{
	if (  !account_name
	||    !*account_name
	||    strcmp( account_name, "account" ) == 0
	||    !transaction_date_time
	||    !*transaction_date_time
	||    strcmp( transaction_date_time, "transaction_date_time" ) == 0 )
	{
		return;
	}

	journal_list_update(
		journal_list_set_balance(
			journal_list_prior(
				journal_prior(
					transaction_date_time,
					account_name ),
				account_name ),
			account_accumulate_debit( account_name ) ) );
}

LIST *journal_list_prior(
			JOURNAL *prior_journal,
			char *account_name )
{
	LIST *journal_list = {0};
	JOURNAL *first_journal;

	if ( !account_name ) return (LIST *)0;

	if ( prior_journal )
	{
		journal_list =
			journal_minimum_list(
				prior_journal->transaction_date_time
					/* minimum_transaction_date_time */,
				account_name );
	}
	else
	/* ------------------------------------------------- */
	/* Otherwise doing period of record for the account. */
	/* ------------------------------------------------- */
	{
		journal_list =
			journal_POR_list(
				account_name );

		if ( !list_length( journal_list ) ) return (LIST *)0;

		first_journal = list_first( journal_list );
		first_journal->previous_balance = 0.0;
		first_journal->transaction_count = 1;
	}

	/* If deleted the latest one, then no propagate. */
	/* --------------------------------------------- */
	if ( prior_journal && list_length( journal_list ) == 1 )
	{
		return (LIST *)0;
	}

	return journal_list;
}

LIST *journal_list_set_balance(
			LIST *journal_list_prior,
			boolean account_accumulate_debit )
{
	JOURNAL *journal;
	JOURNAL *prior_journal = {0};
	JOURNAL *first_journal = {0};
	int transaction_count;

	if ( !list_rewind( journal_list_prior ) ) return (LIST *)0;

	/* Need a separate transaction_count to keep from double counting. */
	/* --------------------------------------------------------------- */
	first_journal = list_first( journal_list_prior );
	transaction_count = first_journal->transaction_count;

	if ( !transaction_count ) transaction_count = 1;

	do {
		journal = list_get( journal_list_prior );

		if ( prior_journal )
		{
			journal->previous_balance = prior_journal->balance;
		}

		if ( account_accumulate_debit )
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

		if ( dollar_virtually_same(
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

	} while( list_next( journal_list_prior ) );

	return journal_list_prior;
}

char *journal_system_string(
			char *journal_select,
			char *journal_table,
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
	 	"select.sh \"%s\" %s \"%s\" transaction_date_time",
		journal_select,
		journal_table,
		where );

	return strdup( system_string );
}

void journal_list_update( LIST *journal_list_set_balance )
{
	JOURNAL *journal;
	FILE *update_pipe;

	if ( !list_rewind( journal_list_set_balance ) ) return;

	update_pipe =
		popen(	journal_update_system_string( JOURNAL_TABLE ),
			"w" );

	do {
		journal = list_get( journal_list_set_balance );

		fprintf(update_pipe,
		 	"%s^%s^%s^%s^transaction_count^%d\n",
			journal->full_name,
		 	journal->street_address,
		 	journal->transaction_date_time,
		 	journal->account_name,
		 	journal->transaction_count );

		fprintf(update_pipe,
		 	"%s^%s^%s^%s^previous_balance^%.2lf\n",
		 	journal->full_name,
		 	journal->street_address,
		 	journal->transaction_date_time,
		 	journal->account_name,
		 	journal->previous_balance );

		fprintf(update_pipe,
		 	"%s^%s^%s^%s^balance^%.2lf\n",
		 	journal->full_name,
		 	journal->street_address,
		 	journal->transaction_date_time,
		 	journal->account_name,
		 	journal->balance );

	} while( list_next( journal_list_set_balance ) );

	pclose( update_pipe );
}

void journal_account_name_list_propagate(
			char *transaction_date_time,
			LIST *account_name_list )
{
	if ( !list_rewind( account_name_list ) ) return;

	do {
		journal_propagate(
			transaction_date_time,
			list_get( account_name_list ) );

	} while( list_next( account_name_list ) );
}

LIST *journal_balance_zero_list( char *account_name )
{
	char *minimum_transaction_date_time;

	minimum_transaction_date_time =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		journal_following_latest_zero_balance_transaction_date_time(
			account_name,
			JOURNAL_TABLE );

	return
	journal_minimum_list(
		minimum_transaction_date_time,
		account_name );
}

LIST *journal_POR_list( char *account_name )
{
	return
	journal_system_list(
		journal_system_string(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			account_primary_where(
				account_name ) ),
		0 /* not fetch_check_number */,
		0 /* not fetch_memo */ );
}

char *journal_update_system_string( char *journal_table )
{
	char *key;
	char system_string[ 1024 ];

	key= "full_name,street_address,transaction_date_time,account";

	sprintf(system_string,
		"update_statement table=%s key=%s carrot=y		|"
		"tee_appaserver_error.sh				|"
		"sql							 ",
		journal_table,
		key );

	return strdup( system_string );
}

#ifdef NOT_DEFINED
FILE *journal_insert_open(
			boolean replace )
{
	char sys_string[ 1024 ];
	char *field;

	field=
		"full_name,"
		"street_address,"
		"transaction_date_time,"
		"account,"
		"debit_amount,"
		"credit_amount";

	sprintf( sys_string,
		 "insert_statement t=%s f=%s replace=%c delimiter='^'	|"
		 "tee_appaserver_error.sh				|"
		 "sql							 ",
		 JOURNAL_TABLE,
		 field,
		 (replace) ? 'y' : 'n' );

	return popen( sys_string, "w" );
}

void journal_insert(	char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name,
			double amount,
			boolean is_debit,
			boolean replace )
{
	FILE *insert_pipe;

	insert_pipe = journal_insert_open( replace );

	journal_insert_pipe(
		insert_pipe,
		full_name,
		street_address,
		transaction_date_time,
		account_name,
		amount,
		is_debit );

	pclose( insert_pipe );

	journal_propagate(
		transaction_date_time,
		account_name );
}

void journal_insert_pipe(
			FILE *insert_pipe,
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
				"%s^%s^%s^%s^%.2lf^\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				entity_escape_full_name( full_name ),
				street_address,
				transaction_date_time,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				account_escape_name(
					account_name ),
				amount );
	}
	else
	{
		fprintf(	insert_pipe,
				"%s^%s^%s^%s^^%.2lf\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				entity_escape_full_name( full_name ),
				street_address,
				transaction_date_time,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				account_escape_name(
					account_name ),
				amount );
	}
}

LIST *journal_ledger_delete(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	return journal_delete(
			full_name,
			street_address,
			transaction_date_time );
}

LIST *journal_delete(	char *full_name,
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
		 "tee_appaserver_error.sh				|"
		 "sql							 ",
		 JOURNAL_TABLE,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s\n",
			full_name,
			street_address,
			transaction_date_time );

	pclose( output_pipe );
	return account_name_list;
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
		entity_escape_full_name( full_name ),
		street_address,
		transaction_date_time );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 select,
		 JOURNAL_TABLE,
		 where,
		 select );

	return pipe2list( sys_string );
}

void journal_list_transaction_date_time_propagate(
			char *transaction_date_time,
			LIST *journal_list )
{
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return;

	do {
		journal = list_get( journal_list );

		/* Executes journal_list_set_balances() */
		/* ------------------------------------ */
		journal_propagate(
			transaction_date_time,
			journal->account_name );

	} while( list_next( journal_list ) );
}

char *journal_list_audit(
			LIST *journal_list )
{
	char buffer[ 65536 ];
	JOURNAL *journal;
	char *buf_ptr = buffer;

	*buf_ptr = '\0';

	if ( list_rewind( journal_list ) )
	{
		do {
			journal = list_get( journal_list );

			buf_ptr +=
			   sprintf(	buf_ptr,
					"\n"
					"full_name = %s, "
					"street_address = %s, "
					"account=%s, "
					"transaction_date_time=%s, "
					"transaction_count=%d, "
					"transaction_count_database=%d, "
					"previous_balance=%.2lf, "
					"previous_balance_database=%.2lf, "
					"debit_amount=%.2lf, "
					"credit_amount=%.2lf, "
					"balance=%.2lf, "
					"balance_database=%.2lf\n",
					journal->full_name,
					journal->street_address,
					journal->account_name,
					journal->transaction_date_time,
					journal->transaction_count,
					journal->transaction_count_database,
					journal->previous_balance,
					journal->previous_balance_database,
					journal->debit_amount,
					journal->credit_amount,
					journal->balance,
					journal->balance_database );
		} while( list_next( journal_list ) );
	}
	return strdup( buffer );
}

JOURNAL *journal_account_name_seek(
			LIST *journal_list,
			char *account_name )
{
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return (JOURNAL *)0;

	do {
		journal = list_get( journal_list );

		if ( strcmp(	journal->account_name,
				account_name ) == 0 )
		{
			return journal;
		}
	} while( list_next( journal_list ) );

	return (JOURNAL *)0;
}

JOURNAL *journal_transaction_date_time_seek(
			LIST *journal_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return (JOURNAL *)0;

	do {
		journal = list_get( journal_list );

		if ( string_strcmp(
			journal->full_name,
			full_name ) == 0
		&&   string_strcmp(
			journal->street_address,
			street_address ) == 0
		&&   string_strcmp(
			journal->transaction_date_time,
			transaction_date_time ) == 0 )
		{
			return journal;
		}
	} while( list_next( journal_list ) );

	return (JOURNAL *)0;
}

JOURNAL *journal_account_name_getset(
			LIST *journal_list,
			char *account_name )
{
	JOURNAL *journal;

	if ( ( journal =
			journal_account_name_seek(
				journal_list,
				account_name ) ) )
	{
		return journal;
	}

	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			account_name );

	list_set( journal_list, journal );
	return journal;
}

LIST *journal_list_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list,
			boolean replace )
{
	LIST *account_name_list;
	JOURNAL *journal;
	FILE *insert_pipe;
	double amount;
	boolean is_debit;

	if ( !full_name
	||   !street_address
	||   !transaction_date_time
	||   !list_rewind( journal_list ) )
	{
		return (LIST *)0;
	}

	account_name_list = list_new();
	insert_pipe = journal_insert_open( replace );

	do {
		journal = list_get( journal_list );

		if ( !dollar_virtually_same(
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

		journal_insert_pipe(
			insert_pipe,
			full_name,
			street_address,
			transaction_date_time,
			journal->account_name,
			amount,
			is_debit );

		list_set(
			account_name_list,
			journal->account_name );

	} while( list_next( journal_list ) );

	pclose( insert_pipe );
	return account_name_list;
}

char *journal_year_where(
			int tax_year,
			char *account_name,
			char *predictive_preclose_time )
{
	static char where[ 256 ];
	char begin_date[ 32 ];
	char end_date[ 32 ];

	sprintf(begin_date,
		"%d-01-01 00:00:00",
		year );

	sprintf(end_date,
		"%d-12-31 %s",
		year,
		predictive_preclose_time );

	sprintf(where,
		"account = '%s' and		 	"
		"transaction_date_time >= '%s' and	"
		"transaction_date_time <= '%s'	",
		account_name,
		begin_date,
		end_date );

	return where;
}

LIST *journal_year_list(
			int year,
			char *account_name )
{
	char *where;

	if ( !year
	||   !account_name )
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
		journal_year_where(
			tax_year,
			account_name,
			PREDICTIVE_PRECLOSE_TIME );

	return
	journal_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_system_string(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			where ),
		0 /* not fetch_check_number */,
		1 /* fetch_memo */ );
}

double journal_amount(
			double debit_amount,
			double credit_amount,
			boolean accumulate_debit )
{
	double return_amount = 0.0;

	if ( accumulate_debit )
	{
		if ( debit_amount )
		{
			return_amount = debit_amount;
		}
		else
		if ( credit_amount )
		{
			return_amount = 0.0 - credit_amount;
		}
	}
	else
	{
		if ( credit_amount )
		{
			return_amount = credit_amount;
		}
		else
		if ( debit_amount )
		{
			return_amount = 0.0 - debit_amount;
		}
	}
	return return_amount;
}

JOURNAL *journal_check_number_seek(
			LIST *journal_list,
			int check_number )
{
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return (JOURNAL *)0;

	do {
		journal = list_get( journal_list );

		if ( journal->check_number == check_number )
		{
			return journal;
		}

	} while( list_next( journal_list ) );

	return (JOURNAL *)0;
}

void journal_list_pipe_display(
			FILE *output_pipe,
			char *transaction_memo,
			char *heading,
			LIST *journal_list )
{
	double total_debit;
	double total_credit;
	char buffer[ 128 ];
	char memo_buffer[ 256 ];
	JOURNAL *journal;
	int i;
	boolean displayed_memo = 0;

	if ( !list_length( journal_list ) ) return;

	if ( transaction_memo && *transaction_memo )
	{
		strncpy(
			memo_buffer,
			transaction_memo,
			TRANSACTION_MEMO_LENGTH );
		*(memo_buffer + TRANSACTION_MEMO_LENGTH ) = '\0';
	}
	else
	{
		*memo_buffer = '\0';
	}

	journal = list_first( journal_list );

	if ( journal->full_name && *journal->full_name )
	{
		sprintf(memo_buffer +
		    	strlen( memo_buffer ),
			"<br>%s",
			journal->full_name );
	}

	if ( journal->transaction_date_time && *journal->transaction_date_time )
	{
		sprintf(memo_buffer +
		    	strlen( memo_buffer ),
			"<br>%s",
			journal->transaction_date_time );
	}

	if ( heading ) fprintf( output_pipe, "%s\n", heading );

	total_debit = 0.0;
	total_credit = 0.0;

	/* First do all debits, then do all credits */
	/* ---------------------------------------- */
	for( i = 0; i < 2; i++ )
	{
		list_rewind( journal_list );
	
		do {
			journal = list_get( journal_list );
	
			if ( i == 0
			&&   !timlib_dollar_virtually_same(
				journal->debit_amount,
				0.0 ) )
			{
				if ( *memo_buffer )
				{
					if ( !displayed_memo )
					{
						fprintf(
						   output_pipe,
						   "%s^",
						   memo_buffer );

						displayed_memo = 1;
					}
					else
					{
						fprintf( output_pipe, "^" );
					}
				}

				fprintf(output_pipe,
			 		"%s^%.2lf^\n",
					format_initial_capital(
						buffer,
						journal->account_name ),
			 		journal->debit_amount );
	
				total_debit += journal->debit_amount;
			}
			else
			if ( i == 1
			&&   !timlib_dollar_virtually_same(
				journal->credit_amount,
				0.0 ) )
			{
				if ( *memo_buffer )
				{
					if ( !displayed_memo )
					{
						fprintf(
						    output_pipe,
						    "%s^",
						    memo_buffer );

						displayed_memo = 1;
					}
					else
					{
						fprintf( output_pipe, "^" );
					}
				}

				fprintf(output_pipe,
			 		"%s^^%.2lf\n",
					format_initial_capital(
						buffer,
						journal->account_name ),
			 		journal->credit_amount );
	
				total_credit += journal->credit_amount;
			}
	
		} while( list_next( journal_list ) );
	}

	fprintf( output_pipe, "\n" );
	fflush( stdout );
}

void journal_list_text_display(
			char *transaction_memo,
			LIST *journal_list )
{
	char *heading;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	if ( !list_length( journal_list ) ) return;

	if ( transaction_memo && *transaction_memo )
	{
		heading = "Transaction^Account^Debit^Credit";
	}
	else
	{
		heading = "Account^Debit^Credit";
	}

	strcpy( sys_string, "delimiter2padded_columns.e '^' 2" );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	journal_list_pipe_display(
			output_pipe,
			transaction_memo,
			heading,
			journal_list );

	pclose( output_pipe );
	fflush( stdout );
}

void journal_list_html_display(
			LIST *journal_list,
			char *transaction_memo )
{
	char *heading;
	char *justify;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	if ( !list_length( journal_list ) ) return;

	if ( transaction_memo && *transaction_memo )
	{
		heading = "Transaction,Account,Debit,Credit";
		justify = "left,left,right,right";
	}
	else
	{
		heading = ",Account,Debit,Credit";
		justify = "left,left,right,right";
	}

	sprintf(sys_string,
		"html_table.e '' %s '^' %s",
		heading,
		justify );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	journal_list_pipe_display(
			output_pipe,
			transaction_memo,
			(char *)0 /* heading */,
			journal_list );

	pclose( output_pipe );
	fflush( stdout );
}

char *journal_minimum_where(
			char *minimum_transaction_date_time,
			char *account_name )
{
	static char where[ 128 ];

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !minimum_transaction_date_time )
	{
		sprintf(where,
			"account = '%s'",
	 		account_name );
	}
	else
	{
		sprintf(where,
	 		"transaction_date_time >= '%s' and		"
			"account = '%s'					",
	 		minimum_transaction_date_time,
	 		account_name );
	}

	return where;
}

LIST *journal_minimum_list(
			char *minimum_transaction_date_time,
			char *account_name )
{
	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	journal_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_system_string(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_minimum_where(
				minimum_transaction_date_time,
				account_name ),
		0 /* not fetch_check_number */,
		0 /* not fetch_memo */ );
}

LIST *journal_binary_journal_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *debit_account,
			char *credit_account )
{
	return journal_binary_list(
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			debit_account,
			credit_account );
}

LIST *journal_binary_list(
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
			full_name,
			street_address,
			transaction_date_time,
			debit_account );

	journal->debit_amount = transaction_amount;
	journal->transaction_date_time = transaction_date_time;

	list_set( journal_list, journal );

	journal =
		journal_new(
			full_name,
			street_address,
			transaction_date_time,
			credit_account );

	journal->credit_amount = transaction_amount;
	journal->transaction_date_time = transaction_date_time;

	list_set( journal_list, journal );

	return journal_list;
}

void journal_list_stdout( LIST *journal_list )
{
	char buffer[ 128 ];
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return;

	do {
		journal = list_get( journal_list );
	
		if ( !dollar_virtually_same(
			journal->debit_amount,
			0.0 ) )
		{
			printf( "%s^%.2lf^^%.2lf\n",
				format_initial_capital(
					buffer,
					journal->account_name ),
			 	journal->debit_amount,
				journal->balance );
		}
		else
		if ( !dollar_virtually_same(
			journal->credit_amount,
			0.0 ) )
		{
			printf( "%s^^%.2lf^%.2lf\n",
				format_initial_capital(
					buffer,
					journal->account_name ),
			 	journal->credit_amount,
				journal->balance );
		}
	
	} while( list_next( journal_list ) );
}

char *journal_list_display(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			LIST *journal_list )
{
	JOURNAL *journal;
	char full_name_buffer[ 128 ];
	char buffer[ 65536 ];
	char *ptr = buffer;

	if ( timlib_strcmp( street_address, "null" ) != 0
	&&   timlib_strcmp( street_address, "unknown" ) != 0 )
	{
		printf(	full_name_buffer,
			"%s/%s",
			full_name,
			street_address );
	}
	else
	{
		strcpy( full_name_buffer, full_name );
	}

	ptr += sprintf(
		ptr,
		"Name = %s; ",
		full_name_buffer );

	ptr += sprintf(
		ptr,
		"Transaction date time = %s; ",
		(transaction_date_time) ? transaction_date_time : "NULL" );

	ptr += sprintf(
		ptr,
	 	"Transaction amount = %.2lf; ",
	 	transaction_amount );

	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		ptr += sprintf(
			ptr,
		 	"Memo = %s; ",
		 	memo );
	}

	if ( !list_rewind( journal_list ) )
	{
		ptr += sprintf(
			ptr,
			"Warning: empty journal list" );

		return strdup( buffer );
	}

	do {

		journal = list_get( journal_list );

		ptr += sprintf( 
			ptr,
			"%s",
			journal_display(
				journal->account_name,
				journal->previous_balance,
				journal->debit_amount,
				journal->credit_amount,
				journal->balance ) );

	} while ( list_next( journal_list ) );

	return strdup( buffer );
}

char *journal_display(
			char *account_name,
			double previous_balance,
			double debit_amount,
			double credit_amount,
			double balance )
{
	char buffer[ 1024 ];
	char *ptr = buffer;

	ptr += sprintf(
		ptr,
		"account = %s; previous_balance = %.2lf; ",
		account_name_display( account_name ),
		previous_balance );

	if ( debit_amount )
	{
		ptr += sprintf(
			ptr,
			"debit  = %12.2lf; ",
			debit_amount );

		ptr += sprintf(
			ptr,
			"%12s",
			"" );
	}
	else
	{
		ptr += sprintf(
			ptr,
			"%12s",
			"" );

		ptr += sprintf(
			ptr,
			"credit = %12.2lf; ",
			credit_amount );
	}

	ptr += sprintf(
		ptr,
		"balance = %12.2lf\n",
		balance );

	return strdup( buffer );
}

LIST *journal_list_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list )
{
	LIST *account_name_list;
	JOURNAL *journal;
	double amount;
	boolean is_debit;

	if ( !full_name
	||   !street_address
	||   !transaction_date_time
	||   !list_rewind( journal_list ) )
	{
		return (LIST *)0;
	}

	account_name_list = list_new();

	do {
		journal = list_get( journal_list );

		if ( !dollar_virtually_same(
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

		journal_insert_pipe(
				insert_pipe,
				full_name,
				street_address,
				transaction_date_time,
				journal->account_name,
				amount,
				is_debit );

		list_set(
			account_name_list,
			journal->account_name );

	} while( list_next( journal_list ) );

	return account_name_list;
}

JOURNAL *journal_merchant_fees_expense(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *account_fees_expense )
{
	JOURNAL *journal;

	if ( double_virtually_same(
		merchant_fees_expense,
		0.0 ) )
	{
		return (JOURNAL *)0;
	}

	journal =
		journal_new(
			full_name,
			street_address,
			transaction_date_time,
			account_fees_expense );

	if ( merchant_fees_expense < 0 )
	{
		journal->debit_amount = 0.0 - merchant_fees_expense;
	}
	else
	{
		journal->credit_amount = merchant_fees_expense;
	}

	return journal;
}

double journal_list_debit_sum(
			LIST *journal_list )
{
	JOURNAL *journal;
	double sum;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );
		sum += journal->debit_amount;

	} while ( list_next( journal_list ) );

	return sum;
}

double journal_list_credit_sum(
			LIST *journal_list )
{
	JOURNAL *journal;
	double sum;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );
		sum += journal->credit_amount;

	} while ( list_next( journal_list ) );

	return sum;
}

double journal_transaction_amount(
			LIST *journal_list )
{
	double debit_sum;
	double credit_sum;

	debit_sum = journal_list_debit_sum( journal_list );
	credit_sum = journal_list_credit_sum( journal_list );

	if ( debit_sum > credit_sum )
		return debit_sum;
	else
		return credit_sum;
}

LIST *journal_list_account_name_list(
			LIST *account_name_list,
			LIST *journal_list )
{
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return account_name_list;

	do {
		journal = list_get( journal_list );

		if ( !account_name_list ) account_name_list = list_new();

		list_set_unique(
			account_name_list,
			journal->account_name );

	} while( list_next( journal_list ) );

	return account_name_list;
}

LIST *journal_date_time_account_name_list(
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

char *journal_entity_where(
			char *full_name,
			char *street_address,
			char *account_name )
{
	static char where[ 256 ];

	if ( !full_name
	||   !street_address
	||   !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
	 	"full_name = '%s' and			"
		"street_address = '%s' and		"
		"account = '%s'				",
		full_name,
		street_address,
	 	account_name );

	return where;
}

LIST *journal_entity_list(
			char *full_name,
			char *street_address,
			char *account_name )
{
	return
	journal_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_system_string(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_entity_where(
				full_name,
				street_address,
				account_name ) ),
		0 /* not fetch_check_number */,
		0 /* not fetch_memo */ );
}

double journal_debit_difference_sum(
			LIST *journal_list )
{
	JOURNAL *journal;
	double sum;
	double difference;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );

		difference =	journal->debit_amount -
				journal->credit_amount;

		sum += difference;

	} while ( list_next( journal_list ) );

	return sum;
}

double journal_credit_difference_sum(
			LIST *journal_list )
{
	JOURNAL *journal;
	double sum;
	double difference;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );

		difference =	journal->credit_amount -
				journal->debit_amount;

		sum += difference;

	} while ( list_next( journal_list ) );

	return sum;
}

LIST *journal_list(	char *full_name,
			char *street_address,
			char *transaction_date_time )
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
	journal_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
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
		0 /* not fetch_check_number */,
		0 /* not fetch_memo */ );
}

char *journal_transaction_system_string(
			char *attribute_name,
			char *transaction_table,
			char *transaction_primary_where )
{
	char system_string[ 1024 ];

	if ( !attribute_name
	||   !transaction_table
	||   !transaction_primary_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
	 	"select.sh \"%s\" %s \"%s\"",
		attribute_name,
		transaction_table,
		transaction_primary_where );

	return strdup( system_string );
}

#endif
