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
	char escape_account[ 64 ];

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
		string_escape_quote(
			escape_account,
			account_name ) );

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
			char *account_name,
			boolean fetch_account,
			boolean fetch_subclassification,
			boolean fetch_element )
{
	JOURNAL *journal;
	char *max_prior_transaction_date_time;
	boolean none_prior = 0;

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

	max_prior_transaction_date_time =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		journal_max_prior_transaction_date_time(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_max_prior_where(
				transaction_date_time,
				account_name ),
			JOURNAL_TABLE );

	if ( max_prior_transaction_date_time
	&&   *max_prior_transaction_date_time )
	{
		transaction_date_time = max_prior_transaction_date_time;
	}
	else
	{
		if ( ! ( transaction_date_time =
				journal_minimum_transaction_date_time(
					account_name ) ) )
		{
			return (JOURNAL *)0;
		}

		none_prior = 1;
	}

	journal =
		journal_account_fetch(
			transaction_date_time,
			account_name,
			fetch_account,
			fetch_subclassification,
			fetch_element,
			0 /* not fetch_transaction */ );

	if ( journal && none_prior )
	{
		journal->previous_balance = 0.0;
	}

	return journal;
}

JOURNAL *journal_latest(
			char *account_name,
			char *transaction_date_time_closing,
			boolean fetch_transaction )
{
	JOURNAL *latest;

	if ( !account_name
	||   !transaction_date_time_closing )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latest =
		journal_account_fetch(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			journal_max_transaction_date_time(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				journal_less_equal_where(
					transaction_date_time_closing,
					account_name ),
				JOURNAL_TABLE )
					/* transaction_date_time */,
			account_name,
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			fetch_transaction );

	if ( latest && !latest->balance )
		return (JOURNAL *)0;
	else
		return latest;
}

char *journal_less_equal_where(
			char *transaction_date_time_closing,
			char *account_name )
{
	static char where[ 128 ];
	char escape_account[ 64 ];

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
		string_escape_quote(
			escape_account,
			account_name ) );

	return where;
}

char *journal_max_transaction_date_time(
			char *journal_less_equal_where,
			char *journal_table )
{
	char system_string[ 1024 ];

	if ( !journal_less_equal_where
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
		journal_less_equal_where );

	return string_pipe( system_string );
}

char *journal_transaction_account_where(
			char *transaction_date_time,
			char *account_name )
{
	static char where[ 128 ];
	char escape_account[ 64 ];

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
		 string_escape_quote(
			escape_account,
			account_name ) );

	return where;
}

JOURNAL *journal_account_fetch(
			char *transaction_date_time,
			char *account_name,
			boolean fetch_account,
			boolean fetch_subclassification,
			boolean fetch_element,
			boolean fetch_transaction )
{
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

	return
	journal_parse(
		string_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				journal_transaction_account_where(
					transaction_date_time,
					account_name ) ) ),
		fetch_account,
		fetch_subclassification,
		fetch_element,
		fetch_transaction );
}

JOURNAL *journal_parse(	char *input,
			boolean fetch_account,
			boolean fetch_subclassification,
			boolean fetch_element,
			boolean fetch_transaction )
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

	if ( fetch_transaction )
	{
		journal->transaction =
			transaction_fetch(
				journal->full_name,
				journal->street_address,
				journal->transaction_date_time,
				0 /* not fetch_journal_list */ );
	}

	if ( fetch_account )
	{
		journal->account =
			account_fetch(
				journal->account_name,
				fetch_subclassification,
				fetch_element );
	}

	return journal;
}

LIST *journal_system_list(
			char *system_string,
			boolean fetch_account,
			boolean fetch_subclassification,
			boolean fetch_element,
			boolean fetch_transaction )
{
	FILE *pipe;
	char input[ 1024 ];
	LIST *system_list;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	system_list = list_new();

	pipe = journal_input_pipe( system_string );

	while ( string_input( input, pipe, 1024 ) )
	{
		list_set(
			system_list,
			journal_parse(
				input,
				fetch_account,
				fetch_subclassification,
				fetch_element,
				fetch_transaction ) );
	}

	pclose( pipe );

	return system_list;
}

FILE *journal_input_pipe(
			char *journal_system_string )
{
	return
	popen( journal_system_string, "r" );
}

void journal_propagate(	char *transaction_date_time,
			char *account_name )
{
	JOURNAL *prior;

	if (  !account_name
	||    !*account_name
	||    strcmp( account_name, "account" ) == 0 )
	{
		return;
	}

	if ( !string_atoi( transaction_date_time ) )
	{
		if ( ! ( transaction_date_time =
				journal_minimum_transaction_date_time(
					account_name ) ) )
		{
			return;
		}
	}

	prior =
		journal_prior(
			transaction_date_time,
			account_name,
			1 /* fetch_account */,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	if ( prior )
	{
		if ( !prior->account
		||   !prior->account->subclassification
		||   !prior->account->subclassification->element )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: element is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		journal_list_update(
			journal_list_balance_set(
				journal_list_prior(
					prior,
					account_name ),
				prior->
					account->
					subclassification->
					element->
					accumulate_debit
					   /* element_accumulate_debit */ ) );
	}
}

LIST *journal_list_prior(
			JOURNAL *journal_prior,
			char *account_name )
{
	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !journal_prior ) return (LIST *)0;

	return
	journal_minimum_list(
		journal_prior->transaction_date_time
			/* minimum_transaction_date_time */,
		account_name,
		journal_prior->previous_balance );
}

LIST *journal_list_balance_set(
			LIST *journal_list_prior,
			boolean account_accumulate_debit )
{
	JOURNAL *journal;
	JOURNAL *prior_journal = {0};

	if ( !list_rewind( journal_list_prior ) ) return (LIST *)0;

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

		prior_journal = journal;

	} while( list_next( journal_list_prior ) );

	return journal_list_prior;
}

char *journal_system_string(
			char *journal_select,
			char *journal_table,
			char *where )
{
	char system_string[ 4096 ];

	sprintf(system_string,
	 	"select.sh \"%s\" %s \"%s\" transaction_date_time",
		journal_select,
		journal_table,
		where );

	return strdup( system_string );
}

FILE *journal_update_pipe( char *journal_update_system_string )
{
	return
	popen( journal_update_system_string, "w" );
}

void journal_list_update( LIST *journal_list_balance_set )
{
	JOURNAL *journal;
	FILE *update_pipe;

	if ( !list_rewind( journal_list_balance_set ) ) return;

	update_pipe =
		journal_update_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_update_system_string(
				JOURNAL_TABLE ) );

	do {
		journal = list_get( journal_list_balance_set );

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

	} while( list_next( journal_list_balance_set ) );

	pclose( update_pipe );
}

char *journal_update_system_string( char *journal_table )
{
	char *key;
	char system_string[ 1024 ];

	if ( !journal_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: journal_table is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	key= "full_name,street_address,transaction_date_time,account";

	sprintf(system_string,
		"update_statement table=%s key=%s carrot=y		|"
/*		"tee_appaserver_error.sh				|" */
		"sql							 ",
		journal_table,
		key );

	return strdup( system_string );
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

	journal->account =
		account_fetch(
			debit_account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

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

	if ( credit_amount < 0.0 )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: credit_amount = %.2lf.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			credit_amount );
	}

	journal = journal_calloc();
	journal->account_name = credit_account_name;
	journal->credit_amount = credit_amount;

	journal->account =
		account_fetch(
			credit_account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	return journal;
}

void journal_list_insert(
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
		journal_insert_pipe(
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

FILE *journal_insert_pipe(
			char *journal_insert,
			char *journal_table )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"insert_statement t=%s f=%s delimiter='^'	|"
/*		"tee_appaserver_error.sh			|" */
		"sql 2>&1					|"
		"html_paragraph_wrapper.e			 ",
		journal_table,
		journal_insert );

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

LIST *journal_extract_account_list( LIST *journal_list )
{
	JOURNAL *journal;
	LIST *account_list;

	if ( !list_rewind( journal_list ) ) return (LIST *)0;

	account_list = list_new();

	do {
		journal = list_get( journal_list );

		if ( !journal->account )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: journal->account is empty for account = [%s].\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				journal->account_name );
			exit( 1 );
		}

		list_set( account_list, journal->account );

	} while ( list_next( journal_list ) );

	if ( !list_length( account_list ) )
		return (LIST *)0;
	else
		return account_list;	
}

LIST *journal_binary_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			ACCOUNT *debit_account,
			ACCOUNT *credit_account )
{
	LIST *journal_list;
	JOURNAL *journal;

	if ( !debit_account
	||   !credit_account )
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
			debit_account->account_name );

	journal->account = debit_account;
	journal->debit_amount = transaction_amount;
	journal->transaction_date_time = transaction_date_time;

	list_set( journal_list, journal );

	journal =
		journal_new(
			full_name,
			street_address,
			transaction_date_time,
			credit_account->account_name );

	journal->account = credit_account;
	journal->credit_amount = transaction_amount;
	journal->transaction_date_time = transaction_date_time;

	list_set( journal_list, journal );

	return journal_list;
}

void journal_list_html_display(
			LIST *journal_list,
			char *transaction_date_time,
			char *transaction_memo )
{
	char *heading;
	char *justify;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	if ( !list_length( journal_list ) ) return;

	heading = "Transaction,Account,Debit,Credit";
	justify = "left,left,right,right";

	sprintf(sys_string,
		"html_table.e '' %s '^' %s",
		heading,
		justify );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	journal_list_pipe_display(
		output_pipe,
		transaction_date_time,
		transaction_memo,
		(char *)0 /* heading */,
		journal_list );

	pclose( output_pipe );
	fflush( stdout );
}

void journal_list_pipe_display(
			FILE *output_pipe,
			char *transaction_date_time,
			char *transaction_memo,
			char *heading,
			LIST *journal_list )
{
	char buffer[ 128 ];
	char memo_buffer[ 256 ];
	JOURNAL *journal;
	int i;
	boolean displayed_memo = 0;

	if ( !list_length( journal_list ) ) return;

	*memo_buffer = '\0';

	journal = list_first( journal_list );

	if ( journal->full_name && *journal->full_name )
	{
		sprintf(memo_buffer +
		    	strlen( memo_buffer ),
			"<br>%s",
			journal->full_name );
	}

	sprintf(memo_buffer +
	    	strlen( memo_buffer ),
		"<br>%s",
		transaction_date_time );

	if ( transaction_memo && *transaction_memo )
	{
		sprintf(memo_buffer +
	    		strlen( memo_buffer ),
			"<br>%s",
			transaction_memo );
	}

	/* *(memo_buffer + TRANSACTION_MEMO_LENGTH ) = '\0'; */

	if ( heading ) fprintf( output_pipe, "%s\n", heading );

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
			}
	
		} while( list_next( journal_list ) );
	}

	fprintf( output_pipe, "\n" );
	fflush( stdout );
}

void journal_list_sum_html_display(
			LIST *journal_list,
			char *transaction_date_time,
			char *transaction_memo,
			double debit_sum,
			double credit_sum )
{
	char *heading;
	char *justify;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	if ( !list_length( journal_list ) ) return;

	heading = "Transaction,Account,Debit,Credit";
	justify = "left,left,right,right";

	sprintf(sys_string,
		"html_table.e '' %s '^' %s",
		heading,
		justify );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	journal_list_pipe_display(
		output_pipe,
		transaction_date_time,
		transaction_memo,
		(char *)0 /* heading */,
		journal_list );

	fprintf(output_pipe,
		"Total^^%.2lf^%.2lf\n",
		debit_sum,
		credit_sum );

	pclose( output_pipe );
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

double journal_credit_sum( LIST *journal_list )
{
	double sum;
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );

		if ( journal->credit_amount ) sum += journal->credit_amount;

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
				ACCOUNT_UNCLEARED_CHECKS,
				ACCOUNT_CREDIT_CARD_KEY );
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
			char *account_name,
			double journal_prior_previous_balance )
{
	LIST *journal_list;

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

	journal_list =
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
				journal_greater_equal_where(
					minimum_transaction_date_time,
					account_name ) ),
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	if ( list_length( journal_list ) )
	{
		JOURNAL *first_journal;

		first_journal =
			list_first(
				journal_list );

		first_journal->previous_balance =	
			journal_prior_previous_balance;
	}

	return journal_list;
}

char *journal_greater_equal_where(
			char *minimum_transaction_date_time,
			char *account_name )
{
	static char where[ 128 ];
	char escape_account[ 64 ];

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
		string_escape_quote(
			escape_account,
			account_name ),
		minimum_transaction_date_time );

	return where;
}

LIST *journal_year_list(
			int tax_year,
			char *account_name,
			boolean fetch_transaction )
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
			journal_year_where(
				tax_year,
				account_name,
				TRANSACTION_PRECLOSE_TIME ) ),
		0 /* not fetch_account */,
		0 /* not fetch_subclassification */,
		0 /* not fetch_element */,
		fetch_transaction );
}

char *journal_year_where(
			int tax_year,
			char *account_name,
			char *transaction_preclose_time )
{
	static char where[ 128 ];
	char begin_date_time[ 32 ];
	char end_date_time[ 32 ];
	char escape_account[ 64 ];

	if ( !tax_year
	||   !account_name
	||   !transaction_preclose_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(begin_date_time,
		"%d-01-01 00:00:00",
		tax_year );

	sprintf(end_date_time,
		"%d-12-31 %s",
		tax_year,
		transaction_preclose_time );

	sprintf(where,
		"account = '%s' and "
		"transaction_date_time between '%s' and '%s'",
		string_escape_quote(
			escape_account,
			account_name ),
		begin_date_time,
		end_date_time );

	return where;
}

void journal_account_list_propagate(
			char *transaction_date_time,
			LIST *journal_extract_account_list )
{
	ACCOUNT *account;
	JOURNAL *prior;

	if ( !transaction_date_time )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_date_time is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( journal_extract_account_list ) ) return;

	do {
		account =
			list_get(
				journal_extract_account_list );

		if ( !account->subclassification
		||   !account->subclassification->element )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: element is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		prior =
			journal_prior(
				transaction_date_time,
				account->account_name,
				0 /* not fetch_account */,
				0 /* not fetch_subclassification */,
				0 /* not fetch_element */ );

		if ( prior )
		{
			journal_list_update(
				journal_list_balance_set(
					journal_list_prior(
						prior,
						account->account_name ),
					account->
						subclassification->
						element->
						accumulate_debit ) );
		}

	} while ( list_next( journal_extract_account_list ) );
}

int journal_transaction_count(
			char *journal_table,
			char *account_name,
			char *transaction_begin_date_string,
			char *transaction_date_time_closing )
{
	char where[ 512 ];
	char system_string[ 1024 ];
	char escape_account[ 64 ];

	sprintf(where,
		"account = '%s' and				"
		"transaction_date_time between '%s' and '%s'	",
		string_escape_quote(
			escape_account,
			account_name ),
		transaction_begin_date_string,
		transaction_date_time_closing );

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		"count(1)",
		journal_table,
		where );

	return atoi( string_pipe( system_string ) );
}

LIST *journal_date_time_account_name_list(
			char *journal_table,
			char *transaction_date_time )
{
	char system_string[ 1024 ];
	char where[ 128 ];

	if ( !transaction_date_time
	||   !*transaction_date_time
	||   strcmp(	transaction_date_time,
			"transaction_date_time" ) == 0
	||   strcmp(	transaction_date_time,
			"preupdate_transaction_date_time" ) == 0 )
	{
		transaction_date_time = "1960-01-01 00:00:00";
	}

	sprintf(where,
		"transaction_date_time >= '%s'",
		transaction_date_time );

	sprintf( system_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 "distinct account",
		 journal_table,
		 where,
		 "account" );

	return pipe2list( system_string );
}

void journal_list_transaction_insert(
			FILE *pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list )
{
	JOURNAL *journal;

	if ( !pipe
	||   !full_name
	||   !street_address
	||   !transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( pipe ) pclose( pipe );

		exit( 1 );
	}

	if ( !list_rewind( journal_list ) ) return;

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

	} while ( list_next( journal_list ) );
}

void journal_account_list_getset(
			LIST *account_list /* in/out */,
			LIST *journal_list )
{
	JOURNAL *journal;

	if ( !account_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( journal_list ) ) return;

	do {
		journal = list_get( journal_list );

		if ( !journal->account )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: journal->account is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		(void)account_getset(
			account_list /* in/out */,
			journal->account );

	} while ( list_next( journal_list ) );
}

JOURNAL *journal_seek(	char *transaction_date_time,
			char *account_name,
			LIST *journal_system_list )
{
	JOURNAL *journal;

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

	if ( !list_rewind( journal_system_list ) ) return (JOURNAL *)0;

	do {
		journal = list_get( journal_system_list );

		if ( strcmp(	journal->transaction_date_time,
				transaction_date_time ) == 0
		&&   strcmp(	journal->account_name,
				account_name ) == 0 )
		{
			return journal;
		}

	} while ( list_next( journal_system_list ) );

	return (JOURNAL *)0;
}

char *journal_minimum_transaction_date_time( char *account_name )
{
	char system_string[ 1024 ];
	char *where;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		account_primary_where(
			account_name );

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		"min( transaction_date_time )",
		JOURNAL_TABLE,
		where );

	return
	string_pipe( system_string );
}

double journal_balance(
			double balance,
			boolean accumulate_debit )
{
	if ( accumulate_debit )
		return balance;
	else
		return -balance;
}

LIST *journal_account_name_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	char system_string[ 1024 ];
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

	sprintf(system_string,
		"echo \"select %s from %s where %s order by %s;\" | sql",
		select,
		JOURNAL_TABLE,
		where,
		select );

	return pipe2list( system_string );
}

double journal_prior_account_end_balance(
			char *feeder_load_file_minimum_date,
			char *account_name )
{
	JOURNAL *prior;
	double end_balance = {0};

	if ( ( prior =
		journal_prior(
			feeder_load_file_minimum_date
				/* transaction_date_time */,
			account_name,
			1 /* fetch_account */,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ ) ) )
	{
		if ( prior->
			account->
			subclassification->
			element->
			accumulate_debit )
		{
			end_balance = prior->balance;
		}
		else
		{
			end_balance = -prior->balance;
		}
	}
	else
	{
		end_balance = 0.0;
	}

	return end_balance;
}

double journal_balance_sum( LIST *journal_list )
{
	double sum;
	JOURNAL *journal;

	if ( !list_rewind( journal_list ) ) return 0.0;

	sum = 0.0;

	do {
		journal = list_get( journal_list );

		sum += journal->balance;

	} while ( list_next( journal_list ) );

	return sum;
}

LIST *journal_tax_form_list(
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *transaction_preclose_time,
			char *account_name )
{
	char *where;

	if ( !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
	||   !transaction_preclose_time
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
		journal_tax_form_where(
			tax_form_fiscal_begin_date,
			tax_form_fiscal_end_date,
			transaction_preclose_time,
			account_name );

	return
	journal_system_list(
		journal_system_string(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			where ),
		1 /* fetch_account */,
		1 /* fetch_subclassification */,
		1 /* fetch_element */,
		0 /* not fetch_transaction */ );
}

char *journal_tax_form_where(
			char *tax_form_fiscal_begin_date,
			char *tax_form_fiscal_end_date,
			char *transaction_preclose_time,
			char *account_name )
{
	static char where[ 128 ];

	if ( !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
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
		"transaction_date_time between	"
		"'%s 00:00:00' and '%s %s' and	"
		"account = '%s'			",
		tax_form_fiscal_begin_date,
		tax_form_fiscal_end_date,
		transaction_preclose_time,
		account_name );


	return where;
}

double journal_amount(	double debit_amount,
			double credit_amount,
			boolean element_accumulate_debit )
{
	double amount = {0};

	if ( element_accumulate_debit )
	{
		if ( debit_amount )
		{
			amount = debit_amount;
		}
		else
		if ( credit_amount )
		{
			amount = -credit_amount;
		}
	}
	else
	{
		if ( credit_amount )
		{
			amount = credit_amount;
		}
		else
		if ( debit_amount )
		{
			amount = -debit_amount;
		}
	}

	return amount;
}

char *journal_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account_name )
{
	char escape_account[ 64 ];
	char where[ 1024 ];

	if ( !full_name
	||   !street_address
	||   !transaction_date_time
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
		"transaction_date_time = '%s' and	"
		"account = '%s'				",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name(
			full_name ),
		street_address,
		transaction_date_time,
		string_escape_quote(
			escape_account,
			account_name ) );

	return strdup( where );
}

