/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/journal.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "float.h"
#include "appaserver_error.h"
#include "appaserver.h"
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
#include "transaction_date.h"
#include "journal.h"

JOURNAL *journal_new(
		char *full_name,
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

char *journal_maximum_prior_where(
		char *transaction_date_time,
		char *account_name )
{
	static char where[ 128 ];

	if ( !transaction_date_time
	||   !account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"transaction_date_time < '%s' and account = '%s'",
		transaction_date_time,
		account_name );

	return where;
}

char *journal_maximum_prior_transaction_date_time(
		char *journal_maximum_prior_where,
		const char *journal_table )
{
	char system_string[ 1024 ];
	char *transaction_date_time;

	if ( !journal_maximum_prior_where )
	{
		char message[ 128 ];

		sprintf(message, "journal_maximum_prior_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		"max( transaction_date_time )",
		journal_table,
		journal_maximum_prior_where );

	transaction_date_time = string_pipe( system_string );

	if ( transaction_date_time && !*transaction_date_time )
	{
		transaction_date_time = (char *)0;
	}

	return transaction_date_time;
}

JOURNAL *journal_prior(
		char *transaction_date_time,
		char *account_name,
		boolean fetch_account,
		boolean fetch_subclassification,
		boolean fetch_element )
{
	JOURNAL *journal;
	char *max_prior_transaction_date_time;

	if ( !transaction_date_time
	||   !account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	max_prior_transaction_date_time =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		journal_maximum_prior_transaction_date_time(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_maximum_prior_where(
				transaction_date_time,
				account_name ),
			JOURNAL_TABLE );

	if ( !max_prior_transaction_date_time ) return NULL;

	journal =
		journal_account_fetch(
			max_prior_transaction_date_time,
			account_name,
			fetch_account,
			fetch_subclassification,
			fetch_element,
			0 /* not fetch_transaction */ );

	return journal;
}

JOURNAL *journal_latest(
		const char *journal_table,
		char *account_name,
		char *end_date_time_string,
		boolean fetch_transaction_boolean,
		boolean zero_balance_boolean )
{
	JOURNAL *fetch;

	if ( !account_name
	||   !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fetch =
		journal_account_fetch(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			journal_maximum_transaction_date_time(
				journal_table,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				journal_less_equal_where(
					end_date_time_string,
					account_name ) )
					/* transaction_date_time */,
			account_name,
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			fetch_transaction_boolean );

	if ( !zero_balance_boolean && fetch && !fetch->balance )
		return NULL;
	else
		return fetch;
}

char *journal_less_equal_where(
		char *end_date_time_string,
		char *account_name )
{
	static char where[ 128 ];
	char escape_account[ 64 ];

	if ( !end_date_time_string
	||   !account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"transaction_date_time <= '%s' and account = '%s'",
		end_date_time_string,
		string_escape_quote(
			escape_account,
			account_name ) );

	return where;
}

char *journal_maximum_transaction_date_time(
		const char *journal_table,
		char *journal_less_equal_where )
{
	char system_string[ 1024 ];

	if ( !journal_less_equal_where )
	{
		char message[ 128 ];

		sprintf(message, "journal_less_equal_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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
	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !transaction_date_time
	||   !*transaction_date_time )
	{
		return NULL;
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

JOURNAL *journal_parse(
		char *input,
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

	if ( !input || !*input ) return NULL;

	/* See JOURNAL_SELECT */
	/* ------------------ */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input, 2 );
	piece( account_name, SQL_DELIMITER, input, 3 );

	journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		journal_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( account_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer ) journal->previous_balance = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer ) journal->debit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer ) journal->credit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	if ( *piece_buffer ) journal->balance = atof( piece_buffer );

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

FILE *journal_input_pipe( char *journal_system_string )
{
	return
	popen( journal_system_string, "r" );
}

char *journal_system_string(
		const char *journal_select,
		const char *journal_table,
		char *where )
{
	char system_string[ STRING_16K ];

	if ( !where ) where = "1 = 1";

	snprintf(
		system_string,
		sizeof ( system_string ),
	 	"select.sh \"%s\" %s \"%s\" transaction_date_time",
		journal_select,
		journal_table,
		where );

	return strdup( system_string );
}

void journal_list_update( LIST *update_statement_list )
{
	char *update_statement;
	FILE *update_pipe;
	char *system_string;

	if ( !list_rewind( update_statement_list ) ) return;

	system_string =
/*		"tee_appaserver.sh | " */
		"sql.e";

	update_pipe =
		appaserver_output_pipe(
			system_string );

	do {
		update_statement = list_get( update_statement_list );

		fprintf(update_pipe,
			"%s\n", 
			update_statement );

	} while( list_next( update_statement_list ) );

	pclose( update_pipe );
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

	if ( ! ( journal = calloc( 1, sizeof ( JOURNAL ) ) ) )
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

	if ( !debit_account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: debit_account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( debit_amount <= 0.0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid debit_amount = %.2lf.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			debit_amount );
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

	if ( !credit_account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: credit_account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( credit_amount <= 0.0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid credit_amount = %.2lf.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			credit_amount );
		exit( 1 );
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

		if ( !journal->account )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"for full_name=%s, journal->account is empty.",
				full_name );

			pclose( pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		journal_insert(
			pipe,
			full_name,
			street_address,
			transaction_date_time,
			journal->account->account_name,
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

	snprintf(
		system_string,
		sizeof ( system_string ),
		"insert_statement t=%s f=%s delimiter='^'	|"
		"tee_appaserver.sh				|"
		"sql.e 2>&1					|"
		"html_paragraph_wrapper.e			 ",
		journal_table,
		journal_insert );

	return popen( system_string, "w" );
}

void journal_insert(
		FILE *pipe,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		double debit_amount,
		double credit_amount )
{
	char debit_amount_string[ 32 ];
	char credit_amount_string[ 32 ];

	if ( float_dollar_virtually_same(
		debit_amount,
		0.0 )
	&&   float_dollar_virtually_same(
		credit_amount,
		0.0 ) )
	{
		return;
	}

	*debit_amount_string = '\0';
	*credit_amount_string = '\0';

	if ( !float_dollar_virtually_same(
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

double journal_debit_credit_difference_sum( LIST *journal_list )
{
	JOURNAL *journal;
	double sum = 0.0;
	double difference;

	if ( list_rewind( journal_list ) )
	do {
		journal = list_get( journal_list );

		difference =	journal->debit_amount -
				journal->credit_amount;

		sum += difference;

	} while ( list_next( journal_list ) );

	return sum;
}

double journal_credit_debit_difference_sum( LIST *journal_list )
{
	JOURNAL *journal;
	double sum = 0.0;
	double difference;

	if ( list_rewind( journal_list ) )
	do {
		journal = list_get( journal_list );

		difference =	journal->credit_amount -
				journal->debit_amount;

		sum += difference;

	} while ( list_next( journal_list ) );

	return sum;
}

char *journal_delete_system_string(
		const char *journal_table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"delete from %s where %s;\" | "
		"tee_appaserver.sh | "
		"sql.e",
		journal_table,
		where );

	return strdup( system_string );
}

LIST *journal_extract_account_list( LIST *journal_list )
{
	JOURNAL *journal;
	LIST *account_list = list_new();

	if ( list_rewind( journal_list ) )
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
	{
		list_free( account_list );
		account_list = NULL;
	}

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

	if ( float_virtually_same(
		transaction_amount, 0.0 ) )
	{
		return (LIST *)0;
	}

	journal_list = list_new();

	journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
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
		char *transaction_memo,
		char *transaction_full_name )
{
	char *heading;
	char *justify;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	heading = "Transaction,Account,Debit,Credit";
	justify = "left,left,right,right";

	snprintf(
		sys_string,
		sizeof ( sys_string ),
		"html_table.e '' %s '^' %s",
		heading,
		justify );


	printf( "<div style=\"margin: 3%c\">\n", '%' );
	fflush( stdout );

	output_pipe = popen( sys_string, "w" );

	if ( list_length( journal_list ) )
	{
		journal_list_pipe_display(
			output_pipe,
			transaction_date_time,
			transaction_memo,
			transaction_full_name,
			journal_list );
	}

	pclose( output_pipe );
	fflush( stdout );
	printf( "</div>\n" );
	fflush( stdout );
}

char *journal_list_display( LIST *journal_list )
{
	char display[ STRING_64K ];
	char *ptr = display;
	JOURNAL *journal;

	*ptr = '\0';

	if ( list_rewind( journal_list ) )
	do {
		journal = list_get( journal_list );

		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_display( journal ) );

	} while ( list_next( journal_list ) );

	return strdup( display );
}

char *journal_display( JOURNAL *journal )
{
	static char display[ 256 ];
	char *ptr = display;
	char buffer1[ 128 ];
	char buffer2[ 128 ];
	char buffer3[ 128 ];

	if ( !journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy(	buffer1,
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			journal->debit_amount ) );

	strcpy(	buffer2,
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			journal->credit_amount ) );

	ptr += sprintf( ptr,
		"%s^%s^Dr: %s^Cr: %s",
		format_initial_capital(
			buffer3,
			journal->account_name ),
		journal->transaction_date_time,
		buffer1,
		buffer2 );

	if ( journal->transaction
	&&   journal->transaction->memo )
	{
		ptr += sprintf( ptr,
			"^%s",
			journal->transaction->memo );
	}

	ptr += sprintf( ptr, "\n" );

	return display;
}

void journal_list_pipe_display(
		FILE *output_pipe,
		char *transaction_date_time,
		char *transaction_memo,
		char *transaction_full_name,
		LIST *journal_list )
{
	char buffer[ 128 ];
	char memo_buffer[ 256 ];
	JOURNAL *journal;
	int i;
	boolean displayed_memo = 0;
	char *full_name = {0};

	if ( !list_length( journal_list ) ) return;

	*memo_buffer = '\0';

	journal = list_first( journal_list );

	if ( transaction_full_name )
		full_name = transaction_full_name;
	else
	if ( journal->full_name && *journal->full_name )
		full_name = journal->full_name;

	if ( full_name )
	{
		sprintf(memo_buffer +
		    	strlen( memo_buffer ),
			"<br>%s",
			full_name );
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

	/* First do all debits, then do all credits */
	/* ---------------------------------------- */
	for( i = 0; i < 2; i++ )
	{
		list_rewind( journal_list );
	
		do {
			journal = list_get( journal_list );
	
			if ( i == 0
			&&   !float_dollar_virtually_same(
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
			 		"%s^%s^\n",
					format_initial_capital(
						buffer,
						journal->account_name ),
					/* ----------------------*/
					/* Returns static memory */
					/* ----------------------*/
					string_commas_dollar(
			 			journal->debit_amount ) );
			}
			else
			if ( i == 1
			&&   !float_dollar_virtually_same(
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
			 		"%s^^%s\n",
					format_initial_capital(
						buffer,
						journal->account_name ),
					/* ----------------------*/
					/* Returns static memory */
					/* ----------------------*/
					string_commas_dollar(
			 			journal->credit_amount ) );
			}
	
		} while( list_next( journal_list ) );
	}

	fprintf( output_pipe, "\n" );

	/* To prevent stdout race condition */
	/* -------------------------------- */
	fflush( output_pipe );
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
	char debit_buffer[ 32 ];
	char credit_buffer[ 32 ];

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
		(char *)0 /* transaction_full_name */,
		journal_list );

	strcpy(	debit_buffer,
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			debit_sum ) );

	strcpy(	credit_buffer,
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			credit_sum ) );

	fprintf(output_pipe,
		"Total^^%s^%s\n",
		debit_buffer,
		credit_buffer );

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

double journal_transaction_amount( LIST *journal_list )
{
	JOURNAL *journal;
	double transaction_amount = {0};

	if ( list_rewind( journal_list ) )
	do {
		journal = list_get( journal_list );

		if ( journal->debit_amount > transaction_amount )
		{
			transaction_amount = journal->debit_amount;
		}

	} while ( list_next( journal_list ) );

	return transaction_amount;
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
				TRANSACTION_DATE_PRECLOSE_TIME ) ),
		0 /* not fetch_account */,
		0 /* not fetch_subclassification */,
		0 /* not fetch_element */,
		fetch_transaction );
}

char *journal_year_where(
		int tax_year,
		char *account_name,
		const char *transaction_date_preclose_time )
{
	static char where[ 128 ];
	char begin_date_time[ 32 ];
	char end_date_time[ 32 ];
	char escape_account[ 64 ];

	if ( !tax_year
	||   !account_name )
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
		transaction_date_preclose_time );

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
	JOURNAL_PROPAGATE *journal_propagate;

	if ( list_rewind( journal_extract_account_list ) )
	do {
		account =
			list_get(
				journal_extract_account_list );

		journal_propagate =
			journal_propagate_new(
				transaction_date_time,
				account->account_name );

		if ( journal_propagate )
		{
			journal_list_update(
				journal_propagate->
					update_statement_list );
		}

	} while ( list_next( journal_extract_account_list ) );
}

int journal_transaction_count(
		const char *journal_table,
		char *account_name,
		char *transaction_begin_date_string,
		char *end_date_time_string )
{
	char where[ 512 ];
	char system_string[ 1024 ];
	char escape_account[ 64 ];

	if ( !account_name
	||   !transaction_begin_date_string
	||   !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"account = '%s' and				"
		"transaction_date_time between '%s' and '%s'	",
		string_escape_quote(
			escape_account,
			account_name ),
		transaction_begin_date_string,
		end_date_time_string );

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
		 "echo \"select %s from %s where %s order by %s;\" | sql.e",
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

JOURNAL *journal_seek(
		char *transaction_date_time,
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

	if ( list_rewind( journal_system_list ) )
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

	return NULL;
}

char *journal_minimum_transaction_date_time( char *account_name )
{
	char system_string[ 1024 ];
	char *where;
	char *transaction_date_time;

	if ( !account_name )
	{
		char message[ 128 ];

		sprintf(message, "account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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

	transaction_date_time = string_pipe( system_string );

	if ( transaction_date_time && !*transaction_date_time )
	{
		transaction_date_time = (char *)0;
	}

	return transaction_date_time;
}

double journal_signed_balance(
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
	char system_string[ 2048 ];
	char *select;
	char where[ 1024 ];

	select = "account";

	snprintf(
		where,
		sizeof ( where ),
		"full_name = '%s' and		"
		"street_address = '%s' and	"
		"transaction_date_time = '%s'	",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( full_name ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_street_address( street_address ),
		transaction_date_time );

	sprintf(system_string,
		"echo \"select %s from %s where %s order by %s;\" | sql.e",
		select,
		JOURNAL_TABLE,
		where,
		select );

	return pipe2list( system_string );
}

double journal_first_account_balance( char *account_name )
{
	char *minimum_transaction_date_time;
	JOURNAL *journal;

	if ( !account_name )
	{
		char message[ 128 ];

		sprintf(message, "account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( minimum_transaction_date_time =
			journal_minimum_transaction_date_time(
				account_name ) ) )
	{
		char message[ 128 ];

		sprintf(message,
		"journal_minimum_transaction_date_time(%s) returned empty.",
			account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	journal =
		journal_account_fetch(
			minimum_transaction_date_time,
			account_name,
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	return journal->balance;
}

double journal_balance_sum( LIST *journal_list )
{
	double sum = 0.0;
	JOURNAL *journal;

	if ( list_rewind( journal_list ) )
	do {
		journal = list_get( journal_list );

		sum += journal->balance;

	} while ( list_next( journal_list ) );

	return sum;
}

LIST *journal_tax_form_list(
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		const char *transaction_date_preclose_time,
		char *account_name )
{
	char *where;

	if ( !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
	||   !account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		journal_tax_form_where(
			tax_form_fiscal_begin_date,
			tax_form_fiscal_end_date,
			transaction_date_preclose_time,
			account_name );

	return
	journal_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
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
		const char *transaction_date_preclose_time,
		char *account_name )
{
	static char where[ 256 ];

	if ( !tax_form_fiscal_begin_date
	||   !tax_form_fiscal_end_date
	||   !account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"transaction_date_time between	"
		"'%s 00:00:00' and '%s %s' and	"
		"account = '%s'			",
		tax_form_fiscal_begin_date,
		tax_form_fiscal_end_date,
		transaction_date_preclose_time,
		account_name );

	return where;
}

double journal_amount(
		double debit_amount,
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
	char escape_address[ 128 ];
	char escape_account[ 128 ];
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

	snprintf(
		where,
		sizeof ( where ),
		"full_name = '%s' and "
		"street_address = '%s' and "
		"transaction_date_time = '%s' and "
		"account = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name(
			full_name ),
		string_escape_quote(
			escape_address,
			street_address ),
		transaction_date_time,
		string_escape_quote(
			escape_account,
			account_name ) );

	return strdup( where );
}

JOURNAL_PROPAGATE *journal_propagate_new(
		char *transaction_date_time,
		char *account_name )
{
	JOURNAL_PROPAGATE *journal_propagate;

	if ( !account_name )
	{
		char message[ 128 ];

		sprintf(message, "account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_propagate = journal_propagate_calloc();

	/* If doing period of record */
	/* ------------------------- */
	if ( !string_atoi( transaction_date_time ) )
	{
		transaction_date_time =
			journal_minimum_transaction_date_time(
				account_name );
	}

	/* If no journal entries for this account */
	/* -------------------------------------- */
	if ( !transaction_date_time ) return NULL;

	journal_propagate->journal_prior =
		journal_prior(
			transaction_date_time,
			account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */,
			1 /* fetch_account */ );

	journal_propagate->prior_transaction_date_time =
		journal_propagate_prior_transaction_date_time(
			journal_propagate->journal_prior );

	journal_propagate->prior_previous_balance =
		journal_propagate_prior_previous_balance(
			journal_propagate->journal_prior );

	journal_propagate->journal_list =
		journal_propagate_journal_list(
			account_name,
			journal_propagate->prior_transaction_date_time,
			journal_propagate->prior_previous_balance );

	if ( list_length( journal_propagate->journal_list ) )
	{
		journal_propagate->accumulate_debit =
			journal_propagate_accumulate_debit(
				account_name,
				journal_propagate->journal_prior );

		journal_propagate_balance_set(
			journal_propagate->journal_list /* in/out */,
			journal_propagate->accumulate_debit );

		journal_propagate->update_statement_list =
			journal_propagate_update_statement_list(
				JOURNAL_TABLE,
				journal_propagate->journal_list );
	}

	return journal_propagate;
}

JOURNAL_PROPAGATE *journal_propagate_calloc( void )
{
	JOURNAL_PROPAGATE *journal_propagate;

	if ( ! ( journal_propagate =
			calloc( 1, sizeof ( JOURNAL_PROPAGATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return journal_propagate;
}

char *journal_propagate_prior_transaction_date_time(
		JOURNAL *journal_prior )
{
	if ( journal_prior )
		return journal_prior->transaction_date_time;
	else
		return (char *)0;
}

double journal_propagate_prior_previous_balance(
		JOURNAL *journal_prior )
{
	if ( journal_prior )
		return journal_prior->previous_balance;
	else
		return 0.0;
}

boolean journal_propagate_accumulate_debit(
		char *account_name,
		JOURNAL *journal_prior )
{
	boolean accumulate_debit;

	if ( journal_prior )
	{
		accumulate_debit =
			journal_prior->
				account->
				subclassification->
				element->
				accumulate_debit;
	}
	else
	{
		accumulate_debit =
			account_accumulate_debit(
				account_name );
	}

	return accumulate_debit;
}

LIST *journal_propagate_journal_list(
		char *account_name,
		char *prior_transaction_date_time,
		double prior_previous_balance )
{
	LIST *journal_list;

	if ( !account_name )
	{
		char message[ 128 ];

		sprintf(message, "account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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
				journal_propagate_greater_equal_where(
					account_name,
					prior_transaction_date_time ) ),
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
			prior_previous_balance;
	}

	return journal_list;
}

void journal_propagate_balance_set(
		LIST *journal_list,
		boolean accumulate_debit )
{
	JOURNAL *journal;
	JOURNAL *prior_journal = {0};

	if ( list_rewind( journal_list ) )
	do {
		journal = list_get( journal_list );

		if ( prior_journal )
		{
			journal->previous_balance = prior_journal->balance;
		}

		journal->balance =
			journal_propagate_balance(
				accumulate_debit,
				journal->previous_balance,
				journal->debit_amount,
				journal->credit_amount );

		prior_journal = journal;

	} while( list_next( journal_list ) );
}

double journal_propagate_balance(
		boolean accumulate_debit,
		double previous_balance,
		double debit_amount,
		double credit_amount )
{
	double balance = 0.0;

	if ( accumulate_debit )
	{
		if ( debit_amount )
		{
			balance =
				previous_balance +
				debit_amount;
		}
		else
		if ( credit_amount )
		{
			balance =
				previous_balance -
				credit_amount;
		}
	}
	else
	{
		if ( credit_amount )
		{
			balance =
				previous_balance +
				credit_amount;
		}
		else
		if ( debit_amount )
		{
			balance =
				previous_balance -
				debit_amount;
		}
	}

	return balance;
}

void journal_propagate_previous_balance_set(
		LIST *journal_list /* in/out */,
		double end_balance )
{
	JOURNAL *journal;
	JOURNAL *prior_journal = {0};

	if ( list_go_tail( journal_list ) )
	do {
		journal = list_get( journal_list );

		if ( !prior_journal )
		{
			journal->balance = end_balance;
		}
		else
		{
			journal->balance = prior_journal->previous_balance;
		}

		journal->previous_balance =
			journal_propagate_previous_balance(
				journal->debit_amount,
				journal->credit_amount,
				journal->balance );

		prior_journal = journal;

	} while( list_prior( journal_list ) );
}

double journal_propagate_previous_balance(
		double debit_amount,
		double credit_amount,
		double balance )
{
	double previous_balance = 0.0;

	if ( debit_amount )
	{
		previous_balance =
			balance -
			debit_amount;
	}
	else
	if ( credit_amount )
	{
		previous_balance =
			balance +
			credit_amount;
	}

	return previous_balance;
}

char *journal_propagate_greater_equal_where(
		char *account_name,
		char *prior_transaction_date_time )
{
	static char where[ 128 ];

	if ( prior_transaction_date_time )
	{
		snprintf(
			where,
			sizeof ( where ),
			"account = '%s' and transaction_date_time >= '%s'",
			account_name,
			prior_transaction_date_time );
	}
	else
	{
		snprintf(
			where,
			sizeof ( where ),
			"account = '%s'",
			account_name );
	}

	return where;
}

LIST *journal_propagate_update_statement_list(
		const char *journal_table,
		LIST *journal_propagate_journal_list )
{
	LIST *update_statement_list = list_new();
	JOURNAL *journal;
	char *update_statement;

	if ( list_rewind( journal_propagate_journal_list ) )
	do {
		journal =
			list_get(
				journal_propagate_journal_list );

		update_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_propagate_update_statement(
				journal_table,
				journal->full_name,
				journal->street_address,
				journal->transaction_date_time,
				journal->account_name,
				journal->previous_balance,
				journal->balance );

		list_set(
			update_statement_list,
			update_statement );

	} while ( list_next( journal_propagate_journal_list ) );

	if ( !list_length( update_statement_list ) )
	{
		list_free( update_statement_list );
		update_statement_list = NULL;
	}

	return update_statement_list;
}

char *journal_propagate_update_statement(
		const char *journal_table,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		double previous_balance,
		double balance )
{
	char *primary_where;
	char update_statement[ 2048 ];

	if ( !full_name
	||   !street_address
	||   !transaction_date_time
	||   !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_primary_where(
			full_name,
			street_address,
			transaction_date_time,
			account_name );

	snprintf(
		update_statement,
		sizeof ( update_statement ),
		"update %s "
		"set previous_balance = %.2lf, "
		"balance = %.2lf "
		"where %s;",
		journal_table,
		previous_balance,
		balance,
		primary_where );

	free( primary_where );

	return strdup( update_statement );
}

double journal_debit_credit_sum_difference(
		boolean element_accumulate_debit,
		LIST *journal_list )
{
	double debit_sum;
	double credit_sum;
	double sum_difference;

	debit_sum =
		journal_debit_sum(
			journal_list );

	credit_sum =
		journal_credit_sum(
			journal_list );

	if ( element_accumulate_debit )
		sum_difference =
			debit_sum - credit_sum;
	else
		sum_difference =
			credit_sum - debit_sum;

	return sum_difference;
}

LIST *journal_entity_list(
		const char *journal_select,
		const char *journal_table,
		char *full_name,
		char *street_address,
		char *account_name )
{
	if ( !full_name
	||   !street_address
	||   !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	journal_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_system_string(
			journal_select,
			journal_table,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			journal_entity_where(
				full_name,
				street_address,
				account_name ) ),
		0 /* not fetch_account */,
		0 /* not fetch_subclassification */,
		0 /* not fetch_element */,
		0 /* not fetch_transaction */ );
}

LIST *journal_transaction_list(
		const char *journal_select,
		const char *journal_table,
		char *full_name,
		char *street_address,
		char *transaction_date_time )
{
	LIST *list = {0};

	if ( full_name
	&&   street_address
	&&   transaction_date_time )
	{
		list =
			journal_system_list(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				journal_system_string(
					journal_select,
					journal_table,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					transaction_primary_where(
						full_name,
						street_address,
						transaction_date_time ) ),
				1 /* fetch_account */,
				1 /* fetch_subclassification */,
				1 /* fetch_element */,
				0 /* not fetch_transaction */ );
	}

	return list;
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
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"%s and account = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			full_name,
			street_address ),
		account_name );

	return where;
}

boolean journal_list_match_boolean(
		LIST *journal1_list,
		LIST *journal2_list )
{
	JOURNAL *journal1;

	if (	list_length( journal1_list ) !=
		list_length( journal2_list ) )
	{
		return 0;
	}

	if ( list_rewind( journal1_list ) )
	do {
		journal1 = list_get( journal1_list );

		if ( !journal1->account )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"journal1->account is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

	if ( !journal_exists_boolean(
		journal2_list /* journal_list */,
		journal1 /* match_journal */ ) )
	{
		return 0;
	}

	} while ( list_next( journal1_list ) );

	return 1;
}

boolean journal_exists_boolean(
		LIST *journal_list,
		JOURNAL *match_journal )
{
	JOURNAL *journal;

	if ( !match_journal
	||   !match_journal->account )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"match_journal is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( journal_list ) )
	do {
		journal = list_get( journal_list );

		if ( journal_match_boolean(
			match_journal /* journal1 */,
			journal /* journal2 */ ) )
		{
			return 1;
		}

	} while ( list_next( journal_list ) );

	return 0;
}

boolean journal_match_boolean(
		JOURNAL *journal1,
		JOURNAL *journal2 )
{
	if ( !journal1
	||   !journal1->account
	||   !journal2
	||   !journal2->account )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp(
		journal1->account->account_name,
		journal2->account->account_name ) != 0 )
	{
		return 0;
	}

	if ( journal1->debit_amount )
	{
		if ( !float_virtually_same(
			journal1->debit_amount,
			journal2->debit_amount ) )
		{
			return 0;
		}
	}
	else
	if ( journal1->credit_amount )
	{
		if ( !float_virtually_same(
			journal1->credit_amount,
			journal2->credit_amount ) )
		{
			return 0;
		}
	}

	return 1;
}

JOURNAL *journal_account_new(
		double journal_amount,
		ACCOUNT *debit_account,
		ACCOUNT *credit_account )
{
	JOURNAL *journal;

	if ( !journal_amount
	|| ( !debit_account
	&&   !credit_account ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal = journal_calloc();

	if ( debit_account )
	{
		journal->debit_amount = journal_amount;
		journal->account = debit_account;
	}
	else
	{
		journal->credit_amount = journal_amount;
		journal->account = credit_account;
	}

	if ( !journal->account->account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
	"for journal_amount=%.2lf, journal->account->account_name is empty.",
			journal_amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return journal;
}

char *journal_list_last_memo( LIST *journal_list )
{
	JOURNAL *journal;

	if ( !list_length( journal_list ) ) return NULL;

	/* Returns the last element or null */
	/* -------------------------------- */
	journal = list_last( journal_list );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	transaction_fetch_memo(
		journal->transaction_date_time );
}

