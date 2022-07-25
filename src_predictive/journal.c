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
	JOURNAL *journal = journal_calloc();

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
		account_name,
		0 /* not fetch_memo */ );
}

JOURNAL *journal_latest(
			char *account_name,
			char *transaction_date_time_closing,
			boolean fetch_memo )
{
	if ( !account_name
	||   !transaction_date_time_closing )
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
		account_name,
		fetch_memo );
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
			char *account_name,
			boolean fetch_memo )
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
		fetch_memo );
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
			account_accumulate_debit(
				account_name,
				1 /* expect_many */ ) ) );
}

LIST *journal_list_prior(
			JOURNAL *journal_prior,
			char *account_name )
{
	LIST *journal_list = {0};
	JOURNAL *first_journal;

	if ( !account_name ) return (LIST *)0;

	if ( journal_prior )
	{
		journal_list =
			journal_minimum_list(
				journal_prior->transaction_date_time
					/* minimum_transaction_date_time */,
				account_name );
	}
	else
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
	if ( journal_prior && list_length( journal_list ) == 1 )
	{
		journal_list = (LIST *)0;
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

JOURNAL *journal_list_latest( LIST *journal_list )
{
	if ( !list_length( journal_list ) )
		return (JOURNAL *)0;

	return list_last( journal_list );
}

LIST *journal_account_distinct_entity_list(
			char *journal_table,
			LIST *account_name_list )
{
	char system_string[ 1024 ];
	char input[ 256 ];
	FILE *pipe;
	ENTITY *entity;
	LIST *list = {0};
	char full_name[ 128 ];
	char street_address[ 128 ];

	if ( !list_length( account_name_list ) ) return (LIST *)0;

	sprintf(system_string,
	 	"select.sh \"%s\" %s \"account in (%s)\" | sort -u",
		"full_name,street_address",
		journal_table,
		timlib_in_clause( account_name_list ) );

	pipe = popen( system_string, "r" );

	while( string_input( input, pipe, 256 ) )
	{
		piece( full_name, SQL_DELIMITER, input, 0 );
		piece( street_address, SQL_DELIMITER, input, 1 );

		entity =
			entity_new(
				strdup( full_name ),
				strdup( street_address ) );

		if ( !list ) list = list_new();

		list_set( list, entity );
	}

	pclose( pipe );

	return list;
}

JOURNAL *journal_calloc( void )
{
	JOURNAL *journal;

	if ( ! ( journal = calloc( 1, sizeof( JOURNAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return journal;
}

JOURNAL *journal_debit_new(
			char *debit_account_name,
			double debit_amount )
{
	JOURNAL *journal;

	if ( !debit_account_name
	||   !debit_amount )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	journal = journal_calloc();
	journal->account_name = debit_account_name;
	journal->debit_amount = debit_amount;

	return journal;
}

JOURNAL *journal_credit_new(
			char *credit_account_name,
			double credit_amount )
{
	JOURNAL *journal;

	if ( !credit_account_name
	||   !credit_amount )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	journal = journal_calloc();
	journal->account_name = credit_account_name;
	journal->credit_amount = credit_amount;

	return journal;
}

void journal_list_insert(
			char *appaserver_error_filename,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list )
{
	FILE *pipe;
	JOURNAL *journal;

	if ( !full_name
	||   !street_address
	||   !transaction_date_time
	||   !list_rewind( journal_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pipe =
		journal_insert_open(
			appaserver_error_filename,
			JOURNAL_INSERT,
			JOURNAL_TABLE );

	do {
		journal = list_get( journal_list );

		journal_insert(
			pipe,
			full_name,
			street_address,
			transaction_date_time,
			journal->account_name,
			journal->debit_amount,
			journal->credit_amount );

	} while( list_next( journal_list ) );

	pclose( pipe );
}

FILE *journal_insert_open(
			char *appaserver_error_filename,
			char *journal_insert,
			char *journal_table )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"insert_statement t=%s f=%s delimiter='^'	|"
		"tee_appaserver_error.sh			|"
		"sql 2>>%s					 ",
		journal_table,
		journal_insert,
		appaserver_error_filename );

	return popen( system_string, "w" );
}

void journal_insert(	FILE *pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name,
			double debit_amount,
			double credit_amount )
{
	char debit_amount_string[ 16 ];
	char credit_amount_string[ 16 ];

	if ( dollar_virtually_same(
		debit_amount,
		0.0 )
	&&   dollar_virtually_same(
		credit_amount,
		0.0 ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: both debit_amount and credit_amount are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		pclose( pipe );
		exit( 1 );
	}

	*debit_amount_string = '\0';
	*credit_amount_string = '\0';

	if ( !dollar_virtually_same(
		debit_amount,
		0.0 ) )
	{
		sprintf( debit_amount_string, "%.2lf", debit_amount );
	}
	else
	{
		sprintf( credit_amount_string, "%.2lf", credit_amount );
	}

	fprintf(pipe,
		"%s^%s^%s^%s^%s^%s\n",
		full_name,
		street_address,
		transaction_date_time,
		account_name,
		debit_amount_string,
		credit_amount_string );
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

double journal_debit_credit_difference_sum(
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

double journal_credit_debit_difference_sum(
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

LIST *journal_fetch_account_name_list(
			char *journal_table,
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh account %s \"%s\"",
		journal_table,
		where );

	return pipe2list( system_string );
}

char *journal_delete_system_string(
			char *journal_table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !journal_table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"echo \"delete from %s where %s;\" | sql",
		journal_table,
		where );

	return strdup( system_string );
}

LIST *journal_extract_account_name_list( LIST *journal_list )
{
	JOURNAL *journal;
	LIST *account_name_list;

	if ( !list_rewind( journal_list ) ) return (LIST *)0;

	account_name_list = list_new();

	do {
		journal = list_get( journal_list );
		list_set( account_name_list, journal->account_name );

	} while ( list_next( journal_list ) );

	return account_name_list;	
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

char *journal_list_raw_display(
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
					"previous_balance=%.2lf, "
					"debit_amount=%.2lf, "
					"credit_amount=%.2lf, "
					"balance=%.2lf\n",
					journal->full_name,
					journal->street_address,
					journal->account_name,
					journal->transaction_date_time,
					journal->transaction_count,
					journal->previous_balance,
					journal->debit_amount,
					journal->credit_amount,
					journal->balance );

		} while( list_next( journal_list ) );
	}
	return strdup( buffer );
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

double journal_account_list_debit_sum(
			LIST *journal_list,
			LIST *account_name_list )
{
	double sum;
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );

		if ( list_string_exists(
			journal->account_name,
			account_name_list ) )
		{
			if ( journal->debit_amount )
				sum += journal->debit_amount;
			else
				sum -= journal->credit_amount;
		}

	} while ( list_next( journal_list ) );

	return sum;
}

double journal_account_list_credit_sum(
			LIST *journal_list,
			LIST *account_name_list )
{
	double sum;
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );

		if ( list_string_exists(
			journal->account_name,
			account_name_list ) )
		{
			if ( journal->credit_amount )
				sum += journal->credit_amount;
			else
				sum -= journal->debit_amount;
		}

	} while ( list_next( journal_list ) );

	return sum;
}

double journal_debit_sum( LIST *journal_list )
{
	double sum;
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );

		if ( journal->debit_amount ) sum += journal->debit_amount;

	} while ( list_next( journal_list ) );

	return sum;
}

double journal_list_transaction_amount( LIST *journal_list )
{
	static LIST *cash_name_list = {0};
	static LIST *current_liability_name_list = {0};
	double cash_sum;
	double current_liability_sum;

	if ( !cash_name_list )
	{
		cash_name_list =
			account_cash_name_list(
				ACCOUNT_TABLE,
				SUBCLASSIFICATION_CASH );
	}

	if ( !current_liability_name_list )
	{
		current_liability_name_list =
			account_current_liability_name_list(
				ACCOUNT_TABLE,
				SUBCLASSIFICATION_CURRENT_LIABILITY,
				ACCOUNT_UNCLEARED_CHECKS );
	}

	cash_sum =
		journal_account_list_debit_sum(
			journal_list,
			cash_name_list );

	current_liability_sum =
		journal_account_list_credit_sum(
			journal_list,
			current_liability_name_list );

	if ( cash_sum )
	{
		return cash_sum;
	}
	else
	if ( current_liability_sum )
	{
		return current_liability_sum;
	}
	else
	{
		return journal_debit_sum( journal_list );
	}
}

LIST *journal_minimum_list(
			char *minimum_transaction_date_time,
			char *account_name )
{
	if ( !minimum_transaction_date_time
	||   !account_name )
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
			journal_minimum_where(
				minimum_transaction_date_time,
				account_name ) ),
		0 /* not fetch_check_number */,
		0 /* not fetch_memo */ );
}

char *journal_minimum_where(
			char *minimum_transaction_date_time,
			char *account_name )
{
	static char where[ 128 ];

	if ( !minimum_transaction_date_time
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
		"account = '%s' and transaction_date_time >= '%s'",
		account_name,
		minimum_transaction_date_time );

	return where;
}

