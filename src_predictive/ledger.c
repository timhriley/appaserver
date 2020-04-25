/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/ledger.c				*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks ledger ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "form.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "attribute.h"
#include "application_constants.h"
#include "list.h"
#include "folder.h"
#include "column.h"
#include "semaphore.h"
#include "date_convert.h"
#include "document.h"
#include "application.h"
#include "ledger.h"
#include "purchase.h"
#include "inventory.h"
#include "customer.h"

LEDGER_ELEMENT *ledger_element_new( char *element_name )
{
	return ledger_new_element( element_name );
}

LEDGER_ELEMENT *ledger_new_element( char *element_name )
{
	LEDGER_ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof( LEDGER_ELEMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	element->element_name = element_name;
	return element;

} /* ledger_new_element() */

LEDGER_ELEMENT *ledger_account_fetch_element(
					char *application_name,
					char *account_name )
{
	LEDGER_ELEMENT *element;
	char element_name[ 128 ];
	char accumulate_debit_yn[ 8 ];
	char where[ 512 ];
	char select[ 128 ];
	char sys_string[ 1024 ];
	char buffer[ 128 ];
	char *account_table;
	char *subclassification_table;
	char *element_table;
	char *results;

	account_table =
		get_table_name(
			application_name, ACCOUNT_FOLDER_NAME );

	subclassification_table =
		get_table_name(
			application_name, SUBCLASSIFICATION_FOLDER_NAME );

	element_table =
		get_table_name(
			application_name, ELEMENT_FOLDER_NAME );

	sprintf(	select,
			"%s.element,accumulate_debit_yn",
			element_table );

	sprintf( where,
		 "%s.subclassification = %s.subclassification and	"
		 "%s.element = %s.element and				"
		 "%s.account = '%s'					",
		 account_table,
		 subclassification_table,
		 subclassification_table,
		 element_table,
		 account_table,
		 escape_character(	buffer,
					account_name,
					'\'' ) );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s,%s where %s;\" | sql.e",
		 select,
		 account_table,
		 subclassification_table,
		 element_table,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (LEDGER_ELEMENT *)0;

	piece(	element_name,
		FOLDER_DATA_DELIMITER,
		results,
		0 );

	element = ledger_element_new( strdup( element_name ) );

	piece(	accumulate_debit_yn,
		FOLDER_DATA_DELIMITER,
		results,
		1 );

	element->accumulate_debit = ( *accumulate_debit_yn == 'y' );

	return element;

} /* ledger_account_fetch_element() */

ACCOUNT *ledger_account_calloc( void )
{
	ACCOUNT *account;

	if ( ! ( account = calloc( 1, sizeof( ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return account;

} /* ledger_account_calloc() */

ACCOUNT *ledger_account_new( char *account_name )
{
	ACCOUNT *account;

	account = ledger_account_calloc();
	account->account_name = account_name;
	return account;

} /* ledger_account_new() */

ACCOUNT *ledger_account_fetch(	char *application_name,
				char *account_name )
{
	ACCOUNT *account;

	account = ledger_account_new( account_name );

	ledger_account_load(	&account->fund_name,
				&account->subclassification_name,
				&account->hard_coded_account_key,
				&account->accumulate_debit,
				application_name,
				account->account_name );

	return account;

} /* ledger_account_fetch() */

char *ledger_account_get_select( char *application_name )
{
	char *select;

	if ( ledger_fund_attribute_exists(
			application_name ) )
	{
		select =
"account,fund,subclassification,hard_coded_account_key";
	}
	else
	{
		select =
"account,null,subclassification,hard_coded_account_key";
	}

	return select;

} /* ledger_account_get_select() */

TRANSACTION *ledger_transaction_seek(
					LIST *transaction_list,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	TRANSACTION *t;

	if ( !list_rewind( transaction_list ) ) return (TRANSACTION *)0;

	do {
		t = list_get( transaction_list );

		if ( timlib_strcmp(	t->full_name,
					full_name ) != 0
		||   timlib_strcmp(	t->street_address,
					street_address ) != 0 )
		{
			continue;
		}

		if ( transaction_date_time && *transaction_date_time )
		{
			if ( timlib_strcmp(	t->transaction_date_time,
						transaction_date_time ) != 0 )
			{
				continue;
			}
		}

		return t;

	} while( list_next( transaction_list ) );

	return (TRANSACTION *)0;

} /* ledger_transaction_seek() */

ACCOUNT *ledger_subclassification_fund_seek_account(
				LIST *account_list,
				char *subclassification_name,
				char *fund_name )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( timlib_strcmp(	account->subclassification_name,
					subclassification_name ) == 0
		&&   timlib_strcmp(	account->fund_name,
					fund_name ) == 0 )
		{
			return account;
		}

	} while( list_next( account_list ) );

	return (ACCOUNT *)0;

} /* ledger_subclassification_fund_seek_account() */

ACCOUNT *ledger_seek_hard_coded_account_key_account(
				LIST *account_list,
				char *fund_name,
				char *hard_coded_account_key )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( timlib_strcmp(
			account->fund_name,
			fund_name ) == 0
		&&   timlib_strcmp(
			account->hard_coded_account_key,
			hard_coded_account_key ) == 0 )
		{
			return account;
		}

	} while( list_next( account_list ) );

	return (ACCOUNT *)0;

} /* ledger_seek_hard_coded_account_key_account() */

ACCOUNT *ledger_account_get_or_set(
				LIST *account_list,
				char *account_name )
{
	ACCOUNT *account;

	if ( ( account =
			ledger_seek_account(
				account_list,
				account_name ) ) )
	{
		return account;
	}

	account = ledger_account_new( account_name );
	list_append_pointer( account_list, account );

	return account;

} /* ledger_account_get_or_set() */

ACCOUNT *ledger_seek_account(	LIST *account_list,
				char *account_name )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( strcmp( account->account_name, account_name ) == 0 )
			return account;

	} while( list_next( account_list ) );

	return (ACCOUNT *)0;

} /* ledger_seek_account() */

LIST *ledger_fetch_account_list(	char *application_name,
					char *as_of_date )
{
	ACCOUNT *account;
	char *select;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	static LIST *account_list = {0};
	FILE *input_pipe;

	if ( account_list ) return account_list;

	account_list = list_new();
	select = ledger_account_get_select( application_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=account		",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		account = ledger_account_calloc();

		ledger_account_parse(
				&account->account_name,
				&account->fund_name,
				&account->subclassification_name,
				&account->hard_coded_account_key,
				input_buffer );

		account->accumulate_debit =
			ledger_account_get_accumulate_debit(
				application_name, account->account_name );

		if ( as_of_date )
		{
			account->latest_ledger =
				ledger_get_latest_ledger(
					application_name,
					account->account_name,
					as_of_date );
		}

		list_append_pointer( account_list, account );
	}

	pclose( input_pipe );
	return account_list;

} /* ledger_fetch_account_list() */

void ledger_account_parse(	char **account_name,
				char **fund_name,
				char **subclassification_name,
				char **hard_coded_account_key,
				char *input_buffer )
{
	char buffer[ 128 ];

	if ( account_name )
	{
		piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		if ( *buffer )
			*account_name = strdup( buffer );
	}

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *buffer )
		*fund_name = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *buffer )
		*subclassification_name = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *buffer )
		*hard_coded_account_key = strdup( buffer );

} /* ledger_account_parse() */

void ledger_account_load(	char **fund_name,
				char **subclassification_name,
				char **hard_coded_account_key,
				boolean *accumulate_debit,
				char *application_name,
				char *account_name )
{
	char *select;
	char buffer[ 128 ];
	char where[ 256 ];
	char sys_string[ 1024 ];
	char *results;

	select = ledger_account_get_select( application_name );

	sprintf(	where,
			"account = '%s'",
		 	timlib_escape_single_quotes(
				buffer,
				account_name ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=account		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot load account = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name );
		exit( 1 );
	}

	ledger_account_parse(	(char **)0 /* account_name */,
				fund_name,
				subclassification_name,
				hard_coded_account_key,
				results );

	*accumulate_debit =
		ledger_account_get_accumulate_debit(
			application_name, account_name );

} /* ledger_account_load() */

SUBCLASSIFICATION *ledger_new_subclassification( char *subclassification_name )
{
	SUBCLASSIFICATION *subclassification;

	if ( ! ( subclassification =
			calloc( 1, sizeof( SUBCLASSIFICATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	subclassification->subclassification_name = subclassification_name;
	return subclassification;

} /* ledger_new_subclassification() */

JOURNAL_LEDGER *ledger_journal_ledger_calloc( void )
{
	JOURNAL_LEDGER *ledger;

	if ( ! ( ledger = calloc( 1, sizeof( JOURNAL_LEDGER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return ledger;

} /* ledger_journal_ledger_calloc() */

JOURNAL_LEDGER *journal_ledger_new(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name )
{
	JOURNAL_LEDGER *ledger;

	ledger = ledger_journal_ledger_calloc();
	ledger->full_name = full_name;
	ledger->street_address = street_address;
	ledger->transaction_date_time = transaction_date_time;
	ledger->account_name = account_name;
	return ledger;

} /* journal_ledger_new() */

JOURNAL_LEDGER *ledger_get_latest_ledger(
					char *application_name,
					char *account_name,
					char *as_of_date )
{
	JOURNAL_LEDGER *ledger;
	char where[ 512 ];
	char select[ 128 ];
	char buffer[ 128 ];
	char sys_string[ 1024 ];
	char *ledger_table;
	char *transaction_table;
	char *results;
	char *latest_transaction_time;

	if ( ledger_exists_closing_entry(
		application_name,
		as_of_date ) )
	{
		latest_transaction_time = LEDGER_PRIOR_TRANSACTION_TIME;
	}
	else
	{
		latest_transaction_time = LEDGER_CLOSING_TRANSACTION_TIME;
	}

	ledger_table =
		get_table_name(
			application_name, LEDGER_FOLDER_NAME );

	transaction_table =
		get_table_name(
			application_name, TRANSACTION_FOLDER_NAME );

	if ( !as_of_date
	||   !*as_of_date
	||   strcmp(	as_of_date,
			"as_of_date" ) == 0 )
	{
		sprintf( where,
		"%s.transaction_date_time = %s.transaction_date_time and "
		"%s.account = '%s'					 ",
			ledger_table,
			transaction_table,
			ledger_table,
		 	timlib_escape_single_quotes( buffer, account_name ) );
	}
	else
	{
		sprintf( where,
		"%s.transaction_date_time = %s.transaction_date_time and "
		"%s.account = '%s' and					 "
		"%s.transaction_date_time <= '%s %s'			 ",
			ledger_table,
			transaction_table,
			ledger_table,
		 	timlib_escape_single_quotes( buffer, account_name ),
			transaction_table,
		 	as_of_date,
			latest_transaction_time );
	}

	sprintf(	select,
			"max( %s.transaction_date_time )",
			transaction_table );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\" | sql.e",
		 select,
		 ledger_table,
		 transaction_table,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (JOURNAL_LEDGER *)0;

	ledger = journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				results /* transaction_date_time */,
				account_name );

	ledger_load(		&ledger->full_name,
				&ledger->street_address,
				&ledger->transaction_count,
				&ledger->previous_balance,
				&ledger->debit_amount,
				&ledger->credit_amount,
				&ledger->balance,
				&ledger->memo,
				ledger->transaction_date_time,
				ledger->account_name,
				application_name );

	return ledger;

} /* ledger_get_latest_ledger() */

JOURNAL_LEDGER *ledger_get_prior_ledger(
					char *application_name,
					char *transaction_date_time,
					char *account_name )
{
	JOURNAL_LEDGER *ledger;
	char buffer[ 128 ];
	char where[ 512 ];
	char select[ 128 ];
	char sys_string[ 1024 ];
	char *table;
	char *results;

	if ( !transaction_date_time
	||   !*transaction_date_time )
	{
		return (JOURNAL_LEDGER *)0;
	}

	table = get_table_name( application_name, LEDGER_FOLDER_NAME );

	sprintf( where,
	"account = '%s' and transaction_date_time < '%s'",
		 timlib_escape_single_quotes( buffer, account_name ),
		 transaction_date_time );

	strcpy( select, "max( transaction_date_time )" );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 select,
		 table,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (JOURNAL_LEDGER *)0;

	ledger = journal_ledger_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				results /* transaction_date_time */,
				account_name );

	ledger_load(		&ledger->full_name,
				&ledger->street_address,
				&ledger->transaction_count,
				&ledger->previous_balance,
				&ledger->debit_amount,
				&ledger->credit_amount,
				&ledger->balance,
				&ledger->memo,
				ledger->transaction_date_time,
				ledger->account_name,
				application_name );

	return ledger;

} /* ledger_get_prior_ledger() */

void ledger_load(		char **full_name,
				char **street_address,
				int *transaction_count,
				double *previous_balance,
				double *debit_amount,
				double *credit_amount,
				double *balance,
				char **memo,
				char *transaction_date_time,
				char *account_name,
				char *application_name )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char select[ 256 ];
	char folder[ 128 ];
	char buffer[ 256 ];
	char piece_buffer[ 256 ];
	char *results;

	sprintf( select,
"%s.full_name,%s.street_address,transaction_count,previous_balance,debit_amount,credit_amount,balance,memo",
		 LEDGER_FOLDER_NAME,
		 LEDGER_FOLDER_NAME );

	sprintf(	folder,
			"%s,%s",
		 	LEDGER_FOLDER_NAME,
			TRANSACTION_FOLDER_NAME );

	sprintf( where,
		 "account = '%s' and					"
		 "%s.transaction_date_time = '%s' and			"
		 "%s.full_name = %s.full_name and			"
		 "%s.street_address = %s.street_address and		"
		 "%s.transaction_date_time = %s.transaction_date_time	",
		 timlib_escape_single_quotes( buffer, account_name ),
		 LEDGER_FOLDER_NAME,
		 transaction_date_time,
		 LEDGER_FOLDER_NAME,
		 TRANSACTION_FOLDER_NAME,
		 LEDGER_FOLDER_NAME,
		 TRANSACTION_FOLDER_NAME,
		 LEDGER_FOLDER_NAME,
		 TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( !results )
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot fetch from %s: %s/%s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_FOLDER_NAME,
			 transaction_date_time,
			 account_name );
		exit( 1 );
	}

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*transaction_count = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	*previous_balance = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	*debit_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	*credit_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	*balance = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 7 );
	*memo = strdup( piece_buffer );

	free( results );

} /* ledger_load() */

void ledger_list_set_balances(	LIST *ledger_list,
				boolean accumulate_debit )
{
	JOURNAL_LEDGER *ledger;
	JOURNAL_LEDGER *prior_ledger = {0};
	JOURNAL_LEDGER *first_ledger = {0};
	int transaction_count;

	if ( !list_rewind( ledger_list ) ) return;

	/* Need a separate transaction_count to keep from double counting. */
	/* --------------------------------------------------------------- */
	first_ledger = list_get_first_pointer( ledger_list );
	transaction_count = first_ledger->transaction_count;

	do {
		ledger = list_get_pointer( ledger_list );

		if ( prior_ledger )
		{
			ledger->previous_balance = prior_ledger->balance;
		}

		if ( accumulate_debit )
		{
			if ( ledger->debit_amount )
			{
				ledger->balance =
					ledger->previous_balance +
					ledger->debit_amount;
			}
			else
			if ( ledger->credit_amount )
			{
				ledger->balance =
					ledger->previous_balance -
					ledger->credit_amount;

			}
		}
		else
		{
			if ( ledger->credit_amount )
			{
				ledger->balance =
					ledger->previous_balance +
					ledger->credit_amount;

			}
			else
			if ( ledger->debit_amount )
			{
				ledger->balance =
					ledger->previous_balance -
					ledger->debit_amount;
			}
		}

		if ( timlib_dollar_virtually_same(
			ledger->balance,
			0.0 ) )
		{
			ledger->transaction_count = 0;
			transaction_count = 1;
		}
		else
		{
			ledger->transaction_count = transaction_count;
			transaction_count++;
		}

		prior_ledger = ledger;

	} while( list_next( ledger_list ) );

} /* ledger_list_set_balances() */

void ledger_account_list_propagate(
				LIST *propagate_account_list,
				char *application_name )
{
	ACCOUNT *account;

	if ( !list_rewind( propagate_account_list ) ) return;

	do {
		account = list_get_pointer( propagate_account_list );

		ledger_journal_ledger_list_propagate(
				account->journal_ledger_list,
				application_name );

	} while( list_next( propagate_account_list ) );

} /* ledger_account_list_propagate() */

void ledger_journal_ledger_list_propagate(
				LIST *journal_ledger_list,
				char *application_name )
{
	JOURNAL_LEDGER *ledger;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list_string;

	if ( !list_rewind( journal_ledger_list ) ) return;

	table_name = get_table_name( application_name, LEDGER_FOLDER_NAME );
	key_column_list_string =
		"full_name,street_address,transaction_date_time,account";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	output_pipe = popen( sys_string, "w" );

	do {
		ledger = list_get_pointer( journal_ledger_list );

		if ( !ledger->transaction_count
		||	ledger->transaction_count !=
			ledger->database_transaction_count )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^transaction_count^%d\n",
			 	ledger->full_name,
			 	ledger->street_address,
			 	ledger->transaction_date_time,
			 	ledger->account_name,
			 	ledger->transaction_count );

			ledger->database_transaction_count =
				ledger->transaction_count;
		}

		if ( timlib_dollar_virtually_same(
			ledger->previous_balance,
			0.0 )
		||   !timlib_dollar_virtually_same(
			ledger->previous_balance,
			ledger->database_previous_balance ) )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^previous_balance^%.2lf\n",
			 	ledger->full_name,
			 	ledger->street_address,
			 	ledger->transaction_date_time,
			 	ledger->account_name,
			 	ledger->previous_balance );

			ledger->database_previous_balance =
				ledger->previous_balance;
		}

		if ( timlib_dollar_virtually_same(
			ledger->balance,
			0.0 )
		||   !timlib_dollar_virtually_same(
			ledger->balance,
			ledger->database_balance ) )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^balance^%.2lf\n",
			 	ledger->full_name,
			 	ledger->street_address,
			 	ledger->transaction_date_time,
			 	ledger->account_name,
			 	ledger->balance );

		}

	} while( list_next( journal_ledger_list ) );

	pclose( output_pipe );

} /* ledger_journal_ledger_list_propagate() */

LIST *ledger_subclassification_quickly_get_account_list(
					char *application_name,
					char *subclassification_name )
{
	ACCOUNT *account;
	char where[ 128 ];
	char *select;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	LIST *account_list;
	FILE *input_pipe;

	account_list = list_new();
	select = ledger_account_get_select( application_name );

	sprintf(where,
		"subclassification = '%s'",
		subclassification_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=account		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		account = ledger_account_calloc();

		ledger_account_parse(
				&account->account_name,
				&account->fund_name,
				&account->subclassification_name,
				&account->hard_coded_account_key,
				input_buffer );

		list_append_pointer( account_list, account );
	}

	pclose( input_pipe );
	return account_list;

} /* ledger_subclassification_quickly_get_account_list() */

LIST *ledger_subclassification_get_account_list(
					double *subclassification_total,
					char *application_name,
					char *subclassification_name,
					char *fund_name,
					char *as_of_date )
{
	LIST *account_list;
	ACCOUNT *account;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char account_name[ 128 ];
	FILE *input_pipe;
	JOURNAL_LEDGER *latest_ledger;

	if ( fund_name
	&&   *fund_name
	&&   strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf(where,
			"fund = '%s' and subclassification = '%s'",
			fund_name,
			subclassification_name );
	}
	else
	{
		sprintf(where,
			"subclassification = '%s'",
			subclassification_name );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=account		"
		 "			folder=account		"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( account_name, input_pipe ) )
	{
		latest_ledger =
			ledger_get_latest_ledger(
				application_name,
				account_name,
				as_of_date );

		if ( !latest_ledger
		||   timlib_double_virtually_same(
			latest_ledger->balance,
			0.0 ) )
		{
			continue;
		}

		account =
			ledger_account_fetch(
				application_name,
				strdup( account_name ) );

		/* Change account name from stack memory to heap. */
		/* ---------------------------------------------- */
		latest_ledger->account_name = account->account_name;

		account->latest_ledger = latest_ledger;

		list_add_pointer_in_order(
			account_list,
			account,
			ledger_balance_match_function );

		*subclassification_total += account->latest_ledger->balance;
	}

	pclose( input_pipe );
	return account_list;

} /* ledger_subclassification_get_account_list() */

LIST *ledger_element_get_account_list(
					double *element_total,
					char *application_name,
					char *element_name,
					char *fund_name,
					char *as_of_date )
{
	ACCOUNT *account;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char account_name[ 128 ];
	FILE *input_pipe;
	char *folder;
	LIST *account_list;
	char *fund_where;
	JOURNAL_LEDGER *latest_ledger;

	fund_where =
		ledger_get_fund_where(
			application_name,
			fund_name );

	*element_total = 0.0;

	folder = "account,subclassification";

	sprintf(	where,
			"%s and					"
			"element = '%s' and			"
			"account.subclassification =		"
			"subclassification.subclassification	",
			fund_where,
			element_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=account			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 folder,
		 where );

	account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( account_name, input_pipe ) )
	{
		latest_ledger =
			ledger_get_latest_ledger(
				application_name,
				account_name,
				as_of_date );

		if ( !latest_ledger
		||   timlib_double_virtually_same(
			latest_ledger->balance,
			0.0 ) )
		{
			continue;
		}

		account =
			ledger_account_fetch(
				application_name,
				strdup( account_name ) );

		/* Change account name from stack memory to heap. */
		/* ---------------------------------------------- */
		latest_ledger->account_name = account->account_name;

		account->latest_ledger = latest_ledger;

		*element_total += account->latest_ledger->balance;

		list_add_pointer_in_order(
			account_list,
			account,
			ledger_balance_match_function );
	}

	pclose( input_pipe );
	return account_list;

} /* ledger_element_get_account_list() */

LIST *ledger_element_get_subclassification_list(
					double *element_total,
					char *application_name,
					char *element_name,
					char *fund_name,
					char *as_of_date )
{
	LIST *subclassification_list;
	SUBCLASSIFICATION *subclassification;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char subclassification_name[ 128 ];
	FILE *input_pipe;

	*element_total = 0.0;

	sprintf( where, "element = '%s'", element_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=subclassification	"
		 "			folder=subclassification	"
		 "			where=\"%s\"			"
		 "			order=display_order		",
		 application_name,
		 where );

	subclassification_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( subclassification_name, input_pipe ) )
	{
		subclassification =
			ledger_new_subclassification(
				strdup( subclassification_name ) );

		subclassification->account_list =
			ledger_subclassification_get_account_list(
				&subclassification->subclassification_total,
				application_name,
				subclassification->subclassification_name,
				fund_name,
				as_of_date );

		*element_total += subclassification->subclassification_total;

		list_append_pointer(	subclassification_list,
					subclassification );
	}

	pclose( input_pipe );
	return subclassification_list;

} /* ledger_element_get_subclassification_list() */

LEDGER_ELEMENT *ledger_element_list_seek(
				LIST *element_list,
				char *element_name )
{
	return ledger_element_seek( element_list, element_name );
}

LEDGER_ELEMENT *ledger_element_seek(
				LIST *element_list,
				char *element_name )
{
	LEDGER_ELEMENT *element;

	if ( !list_rewind( element_list ) ) return (LEDGER_ELEMENT *)0;

	do {
		element = list_get_pointer( element_list );

		if ( strcmp( element->element_name, element_name ) == 0 )
		{
			return element;
		}

	} while( list_next( element_list ) );

	return (LEDGER_ELEMENT *)0;

} /* ledger_element_seek() */

LIST *ledger_get_element_list(	char *application_name,
				LIST *filter_element_name_list,
				char *fund_name,
				char *as_of_date,
				boolean omit_subclassification )
{
	LIST *element_list;
	LEDGER_ELEMENT *element;
	char sys_string[ 1024 ];
	char input_buffer[ 256 ];
	char element_name[ 128 ];
	char accumulate_debit_yn[ 2 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "get_folder_data	application=%s			   "
		 "			select=element,accumulate_debit_yn "
		 "			folder=element			   ",
		 application_name );

	element_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( element_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		if ( list_length( filter_element_name_list )
		&&   !list_exists_string(
			filter_element_name_list,
			element_name ) )
		{
			continue;
		}

		piece(	accumulate_debit_yn,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		element =
			ledger_new_element(
				strdup( element_name ) );

		element->accumulate_debit = ( *accumulate_debit_yn == 'y' );

		if ( omit_subclassification )
		{
			element->account_list =
				ledger_element_get_account_list(
					&element->element_total,
					application_name,
					element->element_name,
					fund_name,
					as_of_date );
		}
		else
		{
			element->subclassification_list =
				ledger_element_get_subclassification_list(
					&element->element_total,
					application_name,
					element->element_name,
					fund_name,
					as_of_date );
		}

		list_append_pointer(	element_list,
					element );
	}

	pclose( input_pipe );

	return ledger_sort_element_list( element_list );

} /* ledger_get_element_list() */

/* So we don't need ELEMENT.sort_order */
/* ----------------------------------- */
LIST *ledger_sort_element_list( LIST *element_list )
{
	LEDGER_ELEMENT *element;
	LIST *return_element_list;

	return_element_list = list_new();

	if ( ( element = ledger_element_list_seek(
				element_list,
				LEDGER_ASSET_ELEMENT ) ) )
	{
		list_append_pointer( return_element_list, element );
	}

	if ( ( element = ledger_element_list_seek(
				element_list,
				LEDGER_LIABILITY_ELEMENT ) ) )
	{
		list_append_pointer( return_element_list, element );
	}

	if ( ( element = ledger_element_list_seek(
				element_list,
				LEDGER_REVENUE_ELEMENT ) ) )
	{
		list_append_pointer( return_element_list, element );
	}

	if ( ( element = ledger_element_list_seek(
				element_list,
				LEDGER_EXPENSE_ELEMENT ) ) )
	{
		list_append_pointer( return_element_list, element );
	}

	if ( ( element = ledger_element_list_seek(
				element_list,
				LEDGER_GAIN_ELEMENT ) ) )
	{
		list_append_pointer( return_element_list, element );
	}

	if ( ( element = ledger_element_list_seek(
				element_list,
				LEDGER_LOSS_ELEMENT ) ) )
	{
		list_append_pointer( return_element_list, element );
	}

	if ( ( element = ledger_element_list_seek(
				element_list,
				LEDGER_EQUITY_ELEMENT ) ) )
	{
		list_append_pointer( return_element_list, element );
	}

	return return_element_list;

} /* ledger_sort_element_list() */

LATEX_ROW *ledger_get_latex_liabilities_plus_equity_row(
				double liabilities_plus_equity,
				int skip_columns )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "Liabilities Plus Equity" ),
		1 /* large_bold */ );

	while ( skip_columns-- )
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );
	}

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   liabilities_plus_equity ) ),
		0 /* not large_bold */ );

	return latex_row;

} /* ledger_get_latex_liabilities_plus_equity_row() */

LATEX_ROW *ledger_get_subclassification_latex_net_income_row(
				double net_income,
				boolean is_statement_of_activities,
				double percent_denominator )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	if ( is_statement_of_activities )
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			"\\bf Change in Net Assets",
			0 /* not large_bold */ );
	}
	else
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			"\\bf Net Income",
			0 /* not large_bold */ );
	}

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   net_income ) ),
		0 /* not large_bold */ );

	if ( percent_denominator )
	{
		char buffer[ 128 ];
		double percent_of_total;

		percent_of_total =
			( net_income /
	  		percent_denominator ) * 100.0;

		sprintf(buffer,
	 		"%.1lf%c",
	 		percent_of_total,
	 		'%' );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );
	}

	return latex_row;

} /* ledger_get_subclassification_latex_net_income_row() */

LATEX_ROW *ledger_get_latex_net_income_row(
				double net_income,
				boolean is_statement_of_activities,
				double percent_denominator,
				boolean omit_subclassification )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	if ( is_statement_of_activities )
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			"Change in Net Assets",
			1 /* not large_bold */ );
	}
	else
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			"Net Income",
			1 /* not large_bold */ );
	}

	if ( !omit_subclassification )
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			"",
			0 /* not large_bold */ );
	}

	latex_append_column_data_list(
		latex_row->column_data_list,
		"",
		0 /* not large_bold */ );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   net_income ) ),
		0 /* not large_bold */ );

	if ( percent_denominator )
	{
		char buffer[ 128 ];
		double percent_of_total;

		percent_of_total =
			( net_income /
	  		percent_denominator ) * 100.0;

		sprintf( buffer,
	 		"%.1lf%c",
	 		percent_of_total,
	 		'%' );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );
	}

	return latex_row;

} /* ledger_get_latex_net_income_row() */

LIST *ledger_get_subclassification_aggregate_beginning_row_list(
				double *total_element,
				LIST *subclassification_list,
				double percent_denominator )
{
	LIST *row_list;
	SUBCLASSIFICATION *subclassification;
	LATEX_ROW *latex_row;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only do net assets. */
		/* ------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( timlib_dollar_virtually_same(
			subclassification->subclassification_total,
			0.0 ) )
		{
			continue;
		}

		*total_element += subclassification->subclassification_total;

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_append_column_data_list(
			latex_row->column_data_list,
			LEDGER_BEGINNING_BALANCE_LABEL,
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   subclassification->
					subclassification_total ) ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];
			double percent_of_total;

			percent_of_total =
				( subclassification->
					subclassification_total /
	  		  	  percent_denominator ) * 100.0;

			sprintf( buffer,
	 			"%.1lf%c",
	 			percent_of_total,
	 			'%' );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
	}

	} while( list_next( subclassification_list ) );

	return row_list;

} /* ledger_get_subclassification_aggregate_beginning_row_list() */

LIST *ledger_get_subclassification_aggregate_latex_row_list(
					double *total_element,
					LIST *subclassification_list,
					char *element_name,
					double percent_denominator )
{
	LIST *row_list;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	boolean first_time = 1;
	LATEX_ROW *latex_row;
	double percent_of_total;

	*total_element = 0.0;

	if ( !list_length( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp( element_name, LEDGER_EQUITY_ELEMENT ) ==  0 )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		sprintf( format_buffer,
			 "\\large \\bf %s",
			 element_name );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					format_buffer ) ),
			0 /* not large_bold */ );

		first_time = 0;

		if ( !ledger_exists_net_assets_equity_subclassification(
					subclassification_list  ) )
		{
			goto equity_all_done;
		}

		list_append_list(
		      row_list,
		      ledger_get_subclassification_aggregate_beginning_row_list(
				total_element,
				subclassification_list,
				percent_denominator ) );
	}

equity_all_done:

	list_rewind( subclassification_list );

	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( timlib_dollar_virtually_same(
			subclassification->subclassification_total,
			0.0 ) )
		{
			continue;
		}

		/* Don't do net assets. */
		/* -------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( first_time )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\large \\bf %s",
				 element_name );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ),
				0 /* not large_bold */ );

			first_time = 0;
		}

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					subclassification->
					    subclassification_name ) ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
			   	     subclassification->
					subclassification_total ) ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( subclassification->subclassification_total /
		  		  percent_denominator ) * 100.0;

			sprintf( buffer,
		 		"%.1lf%c",
		 		percent_of_total,
		 		'%' );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}
	
		*total_element += subclassification->subclassification_total;

	} while( list_next( subclassification_list ) );

	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	if ( ledger_exists_net_assets_equity_subclassification(
				subclassification_list  ) )
	{
		sprintf(format_buffer,
			"\\large \\bf %s",
			"Equity Ending Balance" );
	}
	else
	{
		sprintf(format_buffer,
		 	"\\large \\bf Total %s",
		 	element_name );
	}

	format_initial_capital( format_buffer, format_buffer );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( format_buffer ),
		0 /* not large_bold */ );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   *total_element ) ),
		0 /* not large_bold */ );

	if ( percent_denominator )
	{
		char buffer[ 128 ];

		percent_of_total =
			( *total_element /
	  		  percent_denominator ) * 100.0;

		sprintf( buffer,
	 		"%.1lf%c",
	 		percent_of_total,
	 		'%' );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );
	}

	/* Blank line */
	/* ---------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "" ),
		0 /* not large_bold */ );

	return row_list;

} /* ledger_get_subclassification_aggregate_latex_row_list() */

LIST *ledger_get_subclassification_display_latex_row_list(
					double *total_element,
					LIST *subclassification_list,
					char *element_name,
					boolean element_accumulate_debit,
					double percent_denominator )
{
	LIST *row_list;
	double subclassification_amount;
	ACCOUNT *account;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;
	int first_time = 1;
	LATEX_ROW *latex_row;
	double percent_of_total;

	*total_element = 0.0;

	if ( !list_length( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp( element_name, LEDGER_EQUITY_ELEMENT ) ==  0 )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		sprintf( format_buffer,
			 "\\large \\bf %s",
			 element_name );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					format_buffer ) ),
			0 /* not large_bold */ );

		first_time = 0;

		if ( ledger_exists_net_assets_equity_subclassification(
					subclassification_list  ) )
		{
			list_append_list(
				row_list,
				ledger_get_beginning_latex_row_list(
					total_element,
					subclassification_list,
					element_accumulate_debit ) );
		}
	}

	list_rewind( subclassification_list );

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Don't do net assets. */
		/* -------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( first_time )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\large \\bf %s",
				 element_name );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ),
				0 /* not large_bold */ );

			first_time = 0;
		}

		if ( list_length( subclassification->account_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\bf %s",
				 subclassification->
					subclassification_name );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ),
				0 /* not large_bold */ );
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			/* ---------------------------------------------- */
			/* Don't display single Change in net assets row. */
			/* Display the total row below.			  */
			/* ---------------------------------------------- */
			if ( strcmp(	account->account_name,
					LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS )
					!= 0 )
			{
				latex_row = latex_new_latex_row();
				list_append_pointer( row_list, latex_row );

				latex_append_column_data_list(
					latex_row->column_data_list,
					strdup( format_initial_capital(
							format_buffer,
							account->
							    account_name ) ),
					0 /* not large_bold */ );

				latex_append_column_data_list(
					latex_row->column_data_list,
					strdup( place_commas_in_money(
					   	     latest_ledger_balance ) ),
					0 /* not large_bold */ );

				if ( percent_denominator )
				{
					char buffer[ 128 ];

					latex_append_column_data_list(
						latex_row->column_data_list,
						strdup( "" ),
						0 /* not large_bold */ );

					latex_append_column_data_list(
						latex_row->column_data_list,
						strdup( "" ),
						0 /* not large_bold */ );

					percent_of_total =
						( latest_ledger_balance /
				  		percent_denominator ) * 100.0;

					sprintf( buffer,
				 		"%.1lf%c",
				 		percent_of_total,
				 		'%' );

					latex_append_column_data_list(
						latex_row->column_data_list,
						strdup( buffer ),
						0 /* not large_bold */ );
				}
			}
	
			*total_element += latest_ledger_balance;

			subclassification_amount +=
					latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		if ( subclassification_amount
		&&   list_length( subclassification_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			if ( strcmp(
				subclassification->
					subclassification_name,
				LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
			{
				sprintf(format_buffer,
					"\\bf %s",
					LEDGER_BEGINNING_BALANCE_LABEL );
			}
			else
			if ( strcmp(
				subclassification->
					subclassification_name,
				LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
			{
				sprintf( format_buffer,
					 "\\bf %s",
					 "Change in Net Assets" );
			}
			else
			{
				sprintf( format_buffer,
				 	"\\bf Total %s",
				 	subclassification->
						subclassification_name );
			}

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ),
				0 /* not large_bold */ );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( "" ),
				0 /* not large_bold */ );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( place_commas_in_money(
					   subclassification_amount ) ),
				0 /* not large_bold */ );

			if ( percent_denominator )
			{
				char buffer[ 128 ];

				latex_append_column_data_list(
					latex_row->column_data_list,
					strdup( "" ),
					0 /* not large_bold */ );

				percent_of_total =
					( subclassification_amount /
			  		percent_denominator ) * 100.0;

				sprintf( buffer,
			 		"%.1lf%c",
			 		percent_of_total,
			 		'%' );

				latex_append_column_data_list(
					latex_row->column_data_list,
					strdup( buffer ),
					0 /* not large_bold */ );
			}
		}

	} while( list_next( subclassification_list ) );

	if ( *total_element )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(format_buffer,
				"\\large \\bf %s",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(format_buffer,
			 	"\\large \\bf Total %s",
			 	element_name );
		}

		 format_initial_capital( format_buffer, format_buffer );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( format_buffer ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   *total_element ) ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( *total_element /
		  		percent_denominator ) * 100.0;

			sprintf( buffer,
		 		"%.1lf%c",
		 		percent_of_total,
		 		'%' );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}

		/* Blank line */
		/* ---------- */
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );
	}

	return row_list;

} /* ledger_get_subclassification_display_latex_row_list() */

LIST *ledger_get_beginning_latex_row_list(
				double *total_element,
				LIST *subclassification_list,
				boolean element_accumulate_debit )
{
	LIST *row_list;
	double subclassification_amount;
	ACCOUNT *account;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;
	LATEX_ROW *latex_row;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only do net assets. */
		/* ------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( list_length( subclassification->account_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\bf %s",
				 subclassification->
					subclassification_name );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ),
				0 /* not large_bold */ );
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						account->
						    account_name ) ),
				0 /* not large_bold */ );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( place_commas_in_money(
				   	latest_ledger_balance ) ),
				0 /* not large_bold */ );
	
			*total_element += latest_ledger_balance;

			subclassification_amount +=
					latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		sprintf( format_buffer,
			 "\\bf %s",
			 LEDGER_BEGINNING_BALANCE_LABEL );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					format_buffer ) ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   subclassification_amount ) ),
			0 /* not large_bold */ );

		return row_list;

	} while( list_next( subclassification_list ) );

	return (LIST *)0;

} /* ledger_get_beginning_latex_row_list() */

double ledger_get_element_value(	LIST *subclassification_list,
					boolean element_accumulate_debit )
{
	double total_element = 0.0;
	ACCOUNT *account;
	SUBCLASSIFICATION *subclassification;
	double latest_ledger_balance;

	if ( !list_rewind( subclassification_list ) ) return 0.0;
	
	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			total_element += latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return total_element;

} /* ledger_get_element_value() */

double ledger_get_net_income(	double total_revenues,
				double total_expenses,
				double total_gains,
				double total_losses )
{
	return (	total_revenues -
			total_expenses +
			total_gains -
			total_losses );

} /* ledger_get_net_income() */

char *ledger_account_list_display( LIST *account_list )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return strdup( "" );

	do {
		account = list_get( account_list );

		buf_ptr += sprintf( buf_ptr,
				    "account = %s:\n",
				    account->account_name );

		buf_ptr +=
			   sprintf(
				buf_ptr,
				"%s\n",
				ledger_list_display(
					account->journal_ledger_list ) );

	} while( list_next( account_list ) );

	return strdup( buffer );

} /* ledger_account_list_display() */

char *ledger_transaction_display( TRANSACTION *transaction )
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
				ledger_list_display(
					transaction->journal_ledger_list ) );

	return strdup( buffer );

} /* ledger_transaction_display() */

void ledger_list_text_display(	char *transaction_memo,
				LIST *ledger_list )
{
	char *heading;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	double total_debit;
	double total_credit;
	char buffer[ 128 ];
	char transaction_memo_buffer[ 128 ];
	JOURNAL_LEDGER *journal_ledger;
	int i;
	boolean displayed_transaction_memo = 0;

	if ( !list_length( ledger_list ) ) return;

	*transaction_memo_buffer = '0';

	if ( transaction_memo && *transaction_memo )
	{
		strncpy( transaction_memo_buffer, transaction_memo, 30 );
		*(transaction_memo_buffer + 30) = '\0';
		heading = "Transaction^Account^Debit^Credit";
	}
	else
	{
		heading = "Account^Debit^Credit";
	}

	strcpy( sys_string, "delimiter2padded_columns.e '^' 2" );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe, "%s\n", heading );

	total_debit = 0.0;
	total_credit = 0.0;

	/* First do all debits, then do all credits */
	/* ---------------------------------------- */
	for( i = 0; i < 2; i++ )
	{
		list_rewind( ledger_list );
	
		do {
			journal_ledger = list_get( ledger_list );
	
			if ( i == 0
			&&   !timlib_dollar_virtually_same(
				journal_ledger->debit_amount,
				0.0 ) )
			{
				if ( *transaction_memo_buffer )
				{
					if ( !displayed_transaction_memo )
					{
						fprintf(
						   output_pipe,
						   "%s^",
						   transaction_memo_buffer );

						displayed_transaction_memo = 1;
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
						journal_ledger->account_name ),
			 		journal_ledger->debit_amount );
	
				total_debit += journal_ledger->debit_amount;
			}
			else
			if ( i == 1
			&&   !timlib_dollar_virtually_same(
				journal_ledger->credit_amount,
				0.0 ) )
			{
				if ( *transaction_memo_buffer )
				{
					if ( !displayed_transaction_memo )
					{
						fprintf(
						    output_pipe,
						    "%s^",
						    transaction_memo_buffer );

						displayed_transaction_memo = 1;
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
						journal_ledger->account_name ),
			 		journal_ledger->credit_amount );
	
				total_credit += journal_ledger->credit_amount;
			}
	
		} while( list_next( ledger_list ) );
	}

	fprintf( output_pipe, "\n" );
	pclose( output_pipe );
	fflush( stdout );

} /* ledger_list_text_display() */

void ledger_list_html_display(	char *transaction_memo,
				LIST *ledger_list )
{
	char *heading;
	char *justify;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	double total_debit;
	double total_credit;
	char buffer[ 128 ];
	JOURNAL_LEDGER *journal_ledger;
	int i;
	boolean displayed_transaction_memo = 0;

	if ( !list_length( ledger_list ) ) return;

	if ( transaction_memo && *transaction_memo )
	{
		heading = "Transaction,Account,Debit,Credit";
		justify = "left,left,right,right";
	}
	else
	{
		heading = "Account,Debit,Credit";
		justify = "left,right,right";
	}

	sprintf(sys_string,
		"html_table.e '' %s '^' %s",
		heading,
		justify );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	total_debit = 0.0;
	total_credit = 0.0;

	/* First do all debits, then do all credits */
	/* ---------------------------------------- */
	for( i = 0; i < 2; i++ )
	{
		list_rewind( ledger_list );
	
		do {
			journal_ledger = list_get( ledger_list );
	
			if ( i == 0
			&&   !timlib_dollar_virtually_same(
				journal_ledger->debit_amount,
				0.0 ) )
			{
				if ( transaction_memo && *transaction_memo )
				{
					if ( !displayed_transaction_memo )
					{
						fprintf( output_pipe,
						 	"%s^",
						 	transaction_memo );
						displayed_transaction_memo = 1;
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
						ledger_get_account_name(
							journal_ledger->
								account_name) ),
			 		journal_ledger->debit_amount );
	
				total_debit += journal_ledger->debit_amount;
			}
			else
			if ( i == 1
			&&   !timlib_dollar_virtually_same(
				journal_ledger->credit_amount,
				0.0 ) )
			{
				if ( transaction_memo && *transaction_memo )
				{
					if ( !displayed_transaction_memo )
					{
						fprintf( output_pipe,
						 	"%s^",
						 	transaction_memo );
						displayed_transaction_memo = 1;
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
						ledger_get_account_name(
							journal_ledger->
								account_name) ),
			 		journal_ledger->credit_amount );
	
				total_credit += journal_ledger->credit_amount;
			}
	
		} while( list_next( ledger_list ) );
	}

/*
	if ( transaction_memo && *transaction_memo )
	{
		fprintf(output_pipe,
			"^Total^%.2lf^%.2lf\n",
			total_debit,
			total_credit );
	}
	else
	{
		fprintf(output_pipe,
			"Total^%.2lf^%.2lf\n",
			total_debit,
			total_credit );
	}
*/

	pclose( output_pipe );
	fflush( stdout );

} /* ledger_list_html_display() */

char *ledger_journal_ledger_list_display(
					LIST *journal_ledger_list )
{
	return ledger_list_display( journal_ledger_list );
}

char *ledger_list_display( LIST *ledger_list )
{
	char buffer[ 65536 ];
	JOURNAL_LEDGER *ledger;
	char *buf_ptr = buffer;

	*buf_ptr = '\0';

	if ( list_rewind( ledger_list ) )
	{
		do {
			ledger = list_get_pointer( ledger_list );

			buf_ptr +=
			   sprintf(	buf_ptr,
					"\n"
					"full_name = %s, "
					"street_address = %s, "
					"account=%s, "
					"transaction_date_time=%s, "
					"transaction_count=%d, "
					"database_transaction_count=%d, "
					"previous_balance=%.2lf, "
					"database_previous_balance=%.2lf, "
					"debit_amount=%.2lf, "
					"credit_amount=%.2lf, "
					"balance=%.2lf, "
					"database_balance=%.2lf\n",
					ledger->full_name,
					ledger->street_address,
					ledger->account_name,
					ledger->transaction_date_time,
					ledger->transaction_count,
					ledger->database_transaction_count,
					ledger->previous_balance,
					ledger->database_previous_balance,
					ledger->debit_amount,
					ledger->credit_amount,
					ledger->balance,
					ledger->database_balance );
		} while( list_next( ledger_list ) );
	}

	return strdup( buffer );

} /* ledger_list_display() */

TRANSACTION *ledger_transaction_calloc(	void )
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

} /* ledger_transaction_calloc() */

TRANSACTION *ledger_transaction_fetch(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	return ledger_transaction_with_load_new(
					application_name,
					full_name,
					street_address,
					transaction_date_time );
}

TRANSACTION *ledger_transaction_with_load_new(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	TRANSACTION *transaction;

	transaction = ledger_transaction_calloc();

	transaction->full_name = full_name;
	transaction->street_address = street_address;
	transaction->transaction_date_time = transaction_date_time;

	if ( !ledger_transaction_load(	&transaction->transaction_amount,
					&transaction->
						database_transaction_amount,
					&transaction->memo,
					application_name,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time ) )
	{
		return transaction;
	}

	transaction->journal_ledger_list =
		ledger_get_journal_ledger_list(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time
				/* minimum_transaction_date_time */,
			(char *)0 /* account_name */ );

	return transaction;

} /* ledger_transaction_with_load_new() */

TRANSACTION *ledger_transaction_new(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *memo )
{
	TRANSACTION *transaction;

	transaction = ledger_transaction_calloc();

	if ( timlib_strcmp( memo, "memo" ) == 0 ) memo = (char *)0;

	transaction->full_name = full_name;
	transaction->street_address = street_address;
	transaction->transaction_date_time = transaction_date_time;
	transaction->memo = memo;

	return transaction;

} /* ledger_transaction_new() */

LIST *ledger_get_journal_ledger_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *minimum_transaction_date_time,
				char *account_name )
{
	char sys_string[ 1024 ];
	LIST *journal_ledger_list;
	char where[ 512 ];
	char *select;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char local_full_name[ 128 ];
	char local_street_address[ 128 ];
	char local_transaction_date_time[ 128 ];
	char local_account_name[ 128 ];
	char buffer[ 128 ];
	JOURNAL_LEDGER *ledger;

	select = ledger_journal_ledger_get_select();

/*
"full_name,street_address,transaction_date_time,account,transaction_count,previous_balance,debit_amount,credit_amount,balance";
*/

	if ( !full_name
	&&   !street_address
	&&   !minimum_transaction_date_time
	&&   account_name )
	{
		sprintf(where,
			"account = '%s'			",
			timlib_escape_single_quotes(
				buffer, account_name ) );
	}
	else
	if ( !full_name
	&&   !street_address
	&&   minimum_transaction_date_time
	&&   account_name )
	{
		sprintf(where,
		 	"transaction_date_time >= '%s' and		"
			"account = '%s'					",
		 	minimum_transaction_date_time,
		 	timlib_escape_single_quotes(
				buffer,
				account_name ) );
	}
	else
	if ( full_name
	&&   street_address
	&&   minimum_transaction_date_time
	&&   !account_name )
	{
		strcpy(	where,
			ledger_get_transaction_where(
				full_name,
				street_address,
				minimum_transaction_date_time
					/* transaction_date_time */,
				(char *)0 /* folder_name */,
				"transaction_date_time" ) );
	}
	else
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: invalid parameter configuration.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=\"%s\"		",
		 application_name,
		 select,
		 LEDGER_FOLDER_NAME,
		 where,
		 "transaction_date_time" );

	input_pipe = popen( sys_string, "r" );
	journal_ledger_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	local_full_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	local_street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	local_transaction_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	local_account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		if ( !full_name
		&&   !street_address
		&&   !minimum_transaction_date_time
		&&   account_name )
		{
			ledger = journal_ledger_new(
					strdup( local_full_name ),
					strdup( local_street_address ),
					strdup( local_transaction_date_time ),
					account_name );
		}
		else
		if ( !full_name
		&&   !street_address
		&&   minimum_transaction_date_time
		&&   account_name )
		{
			ledger = journal_ledger_new(
					strdup( local_full_name ),
					strdup( local_street_address ),
					strdup( local_transaction_date_time ),
					account_name );
		}
		else
		{
			ledger = journal_ledger_new(
					full_name,
					street_address,
					minimum_transaction_date_time,
					strdup( local_account_name ) );
		}

		ledger_journal_ledger_partial_parse(
			&ledger->transaction_count,
			&ledger->database_transaction_count,
			&ledger->previous_balance,
			&ledger->database_previous_balance,
			&ledger->debit_amount,
			&ledger->credit_amount,
			&ledger->balance,
			&ledger->database_balance,
			input_buffer );

		list_append_pointer( journal_ledger_list, ledger );
	}

	pclose( input_pipe );

	return journal_ledger_list;

} /* ledger_get_journal_ledger_list() */

LIST *ledger_get_check_number_journal_ledger_list(
				char *application_name,
				char *minimum_transaction_date_time,
				char *account_name )
{
	char sys_string[ 1024 ];
	LIST *journal_ledger_list;
	char where[ 512 ];
	char *join_where;
	char *select;
	char check_number_select[ 512 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char local_full_name[ 128 ];
	char local_street_address[ 128 ];
	char local_transaction_date_time[ 128 ];
	char local_account_name[ 128 ];
	char buffer[ 128 ];
	char *folder_list_string;
	JOURNAL_LEDGER *ledger;
	char check_number[ 16 ];

/*
"full_name,street_address,transaction_date_time,account,transaction_count,previous_balance,debit_amount,credit_amount,balance,check_number";
*/
	select = ledger_journal_ledger_get_select();
	sprintf( check_number_select, "%s,check_number", select );

	folder_list_string = "journal_ledger,transaction";

	join_where = ledger_get_journal_ledger_transaction_join_where();

	sprintf(where,
	 	"transaction.transaction_date_time >= '%s' and	"
		"credit_amount is not null and			"
		"account = '%s' and %s				",
	 	minimum_transaction_date_time,
	 	timlib_escape_single_quotes(
			buffer,
			account_name ),
		join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=\"%s\"		",
		 application_name,
		 check_number_select,
		 folder_list_string,
		 where,
		 "transaction_date_time" );

	input_pipe = popen( sys_string, "r" );
	journal_ledger_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	local_full_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	local_street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	local_transaction_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	local_account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	check_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			9 );

		ledger = journal_ledger_new(
				strdup( local_full_name ),
				strdup( local_street_address ),
				strdup( local_transaction_date_time ),
				account_name );

		ledger_journal_ledger_partial_parse(
			&ledger->transaction_count,
			&ledger->database_transaction_count,
			&ledger->previous_balance,
			&ledger->database_previous_balance,
			&ledger->debit_amount,
			&ledger->credit_amount,
			&ledger->balance,
			&ledger->database_balance,
			input_buffer );

		ledger->check_number = atoi( check_number );

		list_append_pointer( journal_ledger_list, ledger );
	}

	pclose( input_pipe );

	return journal_ledger_list;

} /* ledger_get_check_number_journal_ledger_list() */

void ledger_journal_ledger_partial_parse(
			int *transaction_count,
			int *database_transaction_count,
			double *previous_balance,
			double *database_previous_balance,
			double *debit_amount,
			double *credit_amount,
			double *balance,
			double *database_balance,
			char *input_buffer )
{
	char piece_buffer[ 128 ];

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		4 );
	*transaction_count =
	*database_transaction_count = atoi( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		5 );

	*previous_balance =
	*database_previous_balance =
			atof( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		6 );
	*debit_amount = atof( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		7 );
	*credit_amount = atof( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		8 );
	*balance =
	*database_balance =
			atof( piece_buffer );

} /* ledger_journal_ledger_partial_parse() */


LIST *ledger_get_year_journal_ledger_list(
				char *application_name,
				int year,
				char *account_name )
{
	char sys_string[ 1024 ];
	LIST *journal_ledger_list;
	char where[ 512 ];
	char *select;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char local_full_name[ 128 ];
	char local_street_address[ 128 ];
	char local_transaction_date_time[ 128 ];
	char local_account_name[ 128 ];
	char buffer[ 128 ];
	JOURNAL_LEDGER *ledger;

	select = ledger_journal_ledger_get_select();

/*
"full_name,street_address,transaction_date_time,account,transaction_count,previous_balance,debit_amount,credit_amount,balance";
*/

	sprintf(where,
		"account = '%s' and		 "
		"transaction_date_time like '%d-%c'",
		timlib_escape_single_quotes(
			buffer, account_name ),
		year,
		'%' );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=\"%s\"		",
		 application_name,
		 select,
		 LEDGER_FOLDER_NAME,
		 where,
		 "transaction_date_time" );

	input_pipe = popen( sys_string, "r" );

	journal_ledger_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	local_full_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	local_street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	local_transaction_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	local_account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		ledger = journal_ledger_new(
				strdup( local_full_name ),
				strdup( local_street_address ),
				strdup( local_transaction_date_time ),
				account_name );

		ledger_journal_ledger_partial_parse(
			&ledger->transaction_count,
			&ledger->database_transaction_count,
			&ledger->previous_balance,
			&ledger->database_previous_balance,
			&ledger->debit_amount,
			&ledger->credit_amount,
			&ledger->balance,
			&ledger->database_balance,
			input_buffer );

		list_append_pointer( journal_ledger_list, ledger );
	}

	pclose( input_pipe );

	return journal_ledger_list;

} /* ledger_get_year_journal_ledger_list() */

boolean ledger_transaction_load(	double *transaction_amount,
					double *database_transaction_amount,
					char **memo,
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;
	char piece_buffer[ 2048 ];

	select = "transaction_amount,memo";

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			transaction_date_time,
			(char *)0 /* folder_name */,
			"transaction_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=transaction		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	if ( *piece_buffer )
		*transaction_amount =
		*database_transaction_amount = atof( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	if ( *piece_buffer )
		*memo = strdup( piece_buffer );

	free( results );

	return 1;

} /* ledger_transaction_load() */

void ledger_transaction_insert_stream(	FILE *output_pipe,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					char *memo,
					int check_number,
					boolean lock_transaction )
{
	char entity_buffer[ 128 ];
	char memo_buffer[ 1024 ];

	if ( memo && *memo )
	{
		fprintf(	output_pipe,
				"%s^%s^%s^%.2lf^%s",
		 		escape_character(	entity_buffer,
							full_name,
							'\'' ),
				street_address,
				transaction_date_time,
				transaction_amount,
				escape_character(	memo_buffer,
							memo,
							'\'' ) );
	}
	else
	{
		fprintf(	output_pipe,
				"%s^%s^%s^%.2lf^",
		 		escape_character(	entity_buffer,
							full_name,
							'\'' ),
				street_address,
				transaction_date_time,
				transaction_amount );
	}

	if ( check_number )
		fprintf( output_pipe, "^%d", check_number );
	else
		fprintf( output_pipe, "^" );

	if ( lock_transaction )
		fprintf( output_pipe, "^y\n" );
	else
		fprintf( output_pipe, "^\n" );

} /* ledger_transaction_insert_stream() */

void ledger_journal_insert_open_stream(
				FILE **debit_account_pipe,
				FILE **credit_account_pipe,
				char *application_name )
{
	char sys_string[ 1024 ];
	char *field;
	char *table_name;

	table_name = get_table_name( application_name, "journal_ledger" );

	field=
"full_name,street_address,transaction_date_time,account,debit_amount";

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^' replace=n |"
		 "sql.e							      ",
		 table_name,
		 field );

	*debit_account_pipe = popen( sys_string, "w" );

	field=
"full_name,street_address,transaction_date_time,account,credit_amount";

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^' replace=n |"
		 "sql.e							      ",
		 table_name,
		 field );

	*credit_account_pipe = popen( sys_string, "w" );

} /* ledger_journal_insert_open_stream() */

void ledger_journal_insert_stream(
			FILE *debit_output_pipe,
			FILE *credit_output_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double amount,
			char *debit_account_name,
			char *credit_account_name )
{
	if ( debit_output_pipe )
	{
		fprintf(	debit_output_pipe,
				"%s^%s^%s^%s^%.2lf\n",
				full_name,
				street_address,
				transaction_date_time,
				debit_account_name,
				amount );
	}

	if ( credit_output_pipe )
	{
		fprintf(	credit_output_pipe,
				"%s^%s^%s^%s^%.2lf\n",
				full_name,
				street_address,
				transaction_date_time,
				credit_account_name,
				amount );
	}

} /* ledger_journal_insert_stream() */

FILE *ledger_transaction_insert_open_stream( char *application_name )
{
	char sys_string[ 1024 ];
	char field[ 128 ];
	char *table_name;
	FILE *output_pipe;

	sprintf( field,
"full_name,street_address,transaction_date_time,transaction_amount,memo,check_number,%s",
		 LEDGER_LOCK_TRANSACTION_ATTRIBUTE );

	table_name =
		get_table_name(
			application_name,
			TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e 2>&1					|"
		 "grep -vi duplicate				 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	return output_pipe;

} /* ledger_transaction_insert_open_stream() */

/* Returns transaction_list with transaction_date_time changed if needed. */
/* ---------------------------------------------------------------------- */
LIST *ledger_transaction_list_insert(	LIST *transaction_list,
					char *application_name )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return transaction_list;

	do {
		transaction = list_get_pointer( transaction_list );

		transaction->transaction_date_time =
			ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				transaction->lock_transaction,
				(LIST *)0 /* journal_ledger_list */ );

	} while( list_next( transaction_list ) );

	ledger_journal_ledger_batch_insert(
				application_name,
				transaction_list );

	return transaction_list;

} /* ledger_transaction_list_insert() */

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_binary_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction,
				char *debit_account_name,
				char *credit_account_name )
{
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	transaction_date_time =
		ledger_transaction_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction );

	ledger_journal_insert_open_stream(
			&debit_account_pipe,
			&credit_account_pipe,
			application_name );

	ledger_journal_insert_stream(
		debit_account_pipe,
		(FILE *)0 /* credit_account_pipe */,
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		debit_account_name,
		(char *)0 /* credit_account_name */ );

	ledger_journal_insert_stream(
		(FILE *)0 /* debit_account_pipe */,
		credit_account_pipe,
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		(char *)0 /* debit_account_name */,
		credit_account_name );

	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	ledger_propagate(
		application_name,
		transaction_date_time,
		debit_account_name );

	ledger_propagate(
		application_name,
		transaction_date_time,
		credit_account_name );

	return transaction_date_time;

} /* ledger_transaction_binary_insert() */

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_insert(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					char *memo,
					int check_number,
					boolean lock_transaction )
{
	FILE *output_pipe;

	transaction_date_time =
		ledger_fetch_unique_transaction_date_time(
			application_name,
			transaction_date_time );

	output_pipe =
		ledger_transaction_insert_open_stream(
			application_name );

	ledger_transaction_insert_stream(
		output_pipe,
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		memo,
		check_number,
		lock_transaction );

	pclose( output_pipe );

	return transaction_date_time;

} /* ledger_transaction_insert() */

char *ledger_fetch_unique_transaction_date_time(
					char *application_name,
					char *transaction_date_time )
{
	key_t key = LEDGER_SEMAPHORE_KEY;
	int semid;
	DATE *next_transaction_date_time = {0};

	if ( ( semid = semaphore( key ) ) < 0 )
	{
		fprintf( stderr, "Error: %s failed.\n", __FILE__ );
		exit( 1 );
	}

	semaphore_wait( semid );

	while ( ledger_transaction_exists(
			application_name,
			transaction_date_time ) )
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

} /* ledger_fetch_unique_transaction_date_time() */

void ledger_journal_ledger_insert(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name,
					double amount,
					boolean is_debit )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;

	if ( !account_name )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty account_name for (%s/%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 transaction_date_time );
		exit( 1 );
	}

	if ( is_debit )
	{
		field=
"full_name,street_address,transaction_date_time,account,debit_amount";
	}
	else
	{
		field=
"full_name,street_address,transaction_date_time,account,credit_amount";
	}

	table_name = get_table_name( application_name, "journal_ledger" );

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^' replace=n |"
		 "sql.e							      ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s^%s^%.2lf\n",
			full_name,
			street_address,
			transaction_date_time,
			account_name,
			amount );

	pclose( output_pipe );

} /* ledger_journal_ledger_insert() */

void ledger_entity_update(		char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *preupdate_full_name,
					char *preupdate_street_address )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *table_name;
	char *key;
	char entity_buffer1[ 128 ];
	char entity_buffer2[ 128 ];
	char *where_full_name;
	char *where_street_address;

	if ( !transaction_date_time || !*transaction_date_time ) return;

	if ( strcmp( preupdate_full_name, "preupdate_full_name" ) != 0 )
		where_full_name = preupdate_full_name;
	else
		where_full_name = full_name;

	if ( strcmp(	preupdate_street_address,
			"preupdate_street_address" ) != 0 )
	{
		where_street_address = preupdate_street_address;
	}
	else
	{
		where_street_address = street_address;
	}

	key = "full_name,street_address,transaction_date_time";

	/* update TRANSACTION */
	/* ------------------ */
	table_name =
		get_table_name(
			application_name,
			TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s	|"
		 "sql.e					 ",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s|full_name=%s|street_address=%s|%s=y\n",
	 	escape_character(	entity_buffer1,
					where_full_name,
					'\'' ),
	 	where_street_address,
	 	transaction_date_time,
	 	escape_character(	entity_buffer2,
					full_name,
					'\'' ),
		street_address,
		LEDGER_LOCK_TRANSACTION_ATTRIBUTE );

	pclose( output_pipe );

	/* update JOURNAL_LEDGER */
	/* --------------------- */
	table_name =
		get_table_name(
			application_name,
			LEDGER_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s	|"
		 "sql.e					 ",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s|full_name=%s|street_address=%s\n",
	 	escape_character(	entity_buffer1,
					where_full_name,
					'\'' ),
	 	where_street_address,
	 	transaction_date_time,
	 	escape_character(	entity_buffer2,
					full_name,
					'\'' ),
		street_address );

	pclose( output_pipe );

} /* ledger_entity_update() */

void ledger_transaction_amount_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					double database_transaction_amount )
{
	char transaction_amount_string[ 16 ];

	if ( timlib_dollar_virtually_same(
		transaction_amount,
		database_transaction_amount ) )
	{
		return;
	}

	sprintf( transaction_amount_string,
		 "%.2lf",
		 transaction_amount );

	ledger_transaction_generic_update(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		"transaction_amount" /* attribute */,
		transaction_amount_string /* data */ );

} /* ledger_transaction_amount_update() */

void ledger_transaction_generic_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *attribute_name,
					char *data )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *transaction_table;
	char *key;
	char entity_buffer[ 128 ];
	char data_buffer[ 65536 ];

	key = "full_name,street_address,transaction_date_time";

	transaction_table =
		get_table_name(
			application_name,
			TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql.e						 ",
		 transaction_table,
		 key );

	output_pipe = popen( sys_string, "w" );

	/* Lock needs to go first in case changing transaction_date_time */
	/* ------------------------------------------------------------- */
	fprintf(output_pipe,
	 	"%s^%s^%s^%s^y\n",
	 	escape_character(	entity_buffer,
					full_name,
					'\'' ),
	 	street_address,
	 	transaction_date_time,
		LEDGER_LOCK_TRANSACTION_ATTRIBUTE );

	fprintf(output_pipe,
	 	"%s^%s^%s^%s^%s\n",
	 	escape_character(	entity_buffer,
					full_name,
					'\'' ),
	 	street_address,
	 	transaction_date_time,
		attribute_name,
	 	escape_character(	data_buffer,
					data,
					'\'' ) );

	pclose( output_pipe );

} /* ledger_transaction_generic_update() */

void ledger_journal_generic_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *attribute_name,
					char *data )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *transaction_table;
	char *key;
	char entity_buffer[ 128 ];
	char data_buffer[ 65536 ];

	key = "full_name,street_address,transaction_date_time";

	transaction_table =
		get_table_name(
			application_name, LEDGER_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 transaction_table,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^%s^%s\n",
	 	escape_character(	entity_buffer,
					full_name,
					'\'' ),
	 	street_address,
	 	transaction_date_time,
		attribute_name,
	 	escape_character(	data_buffer,
					data,
					'\'' ) );

	pclose( output_pipe );

} /* ledger_journal_generic_update() */

void ledger_transaction_memo_update(	char *application_name,
					TRANSACTION *transaction )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *transaction_table;
	char *key;
	char entity_buffer[ 128 ];
	char memo_buffer[ 65536 ];

	if ( !transaction ) return;

	key = "full_name,street_address,transaction_date_time";

	transaction_table =
		get_table_name(
			application_name, TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 transaction_table,
		 key );

	output_pipe = popen( sys_string, "w" );

	if ( transaction->memo )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^memo^%s\n",
		 	escape_character(	entity_buffer,
						transaction->full_name,
						'\'' ),
		 	transaction->street_address,
		 	transaction->transaction_date_time,
		 	escape_character(	memo_buffer,
						transaction->memo,
						'\'' ) );
	}

	pclose( output_pipe );

} /* ledger_transaction_memo_update() */

DICTIONARY *ledger_get_hard_coded_dictionary(
				char *application_name )
{
	static DICTIONARY *return_dictionary = {0};

	if ( return_dictionary ) return return_dictionary;

	if ( ledger_fund_attribute_exists(
				application_name ) )
	{
		return_dictionary = 
			ledger_get_fund_hard_coded_dictionary(
				application_name );
	}
	else
	{
		return_dictionary = 
			ledger_get_non_fund_hard_coded_dictionary(
				application_name );
	}

	return return_dictionary;

} /* ledger_get_hard_coded_dictionary() */

DICTIONARY *ledger_get_non_fund_hard_coded_dictionary(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 128 ];
	DICTIONARY *return_dictionary;

	folder = "account";

	select = "hard_coded_account_key, account";

	strcpy( where,
	 	"hard_coded_account_key is not null" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	return_dictionary =
		ledger_account_pipe2dictionary(
			sys_string,
			FOLDER_DATA_DELIMITER );

	return return_dictionary;

} /* ledger_get_non_fund_hard_coded_dictionary() */

DICTIONARY *ledger_get_fund_hard_coded_dictionary(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 128 ];
	DICTIONARY *non_fund_dictionary;
	DICTIONARY *return_dictionary;

	folder = "account";

	/* Get fund dictionary */
	/* ------------------- */
	select = "concat( fund, '|', hard_coded_account_key ), account";

	strcpy( where,
	 	"hard_coded_account_key is not null and fund is not null" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	return_dictionary =
		ledger_account_pipe2dictionary(
			sys_string,
			FOLDER_DATA_DELIMITER );

	/* Get non fund dictionary */
	/* ----------------------- */
	select = "hard_coded_account_key, account";

	strcpy( where,
	 	"hard_coded_account_key is not null and fund is null" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	non_fund_dictionary =
		ledger_account_pipe2dictionary(
			sys_string,
			FOLDER_DATA_DELIMITER );

	dictionary_append_dictionary( return_dictionary, non_fund_dictionary );

	return return_dictionary;

} /* ledger_get_fund_hard_coded_dictionary() */

char *ledger_get_hard_coded_account_name(
				char *application_name,
				char *fund_name,
				char *hard_coded_account_key,
				boolean warning_only,
				const char *calling_function_name )
{
	DICTIONARY *hard_coded_dictionary;
	char *key;
	char *account_name;

	hard_coded_dictionary =
		ledger_get_hard_coded_dictionary(
			application_name );

	key = ledger_get_hard_coded_dictionary_key(
			fund_name,
			hard_coded_account_key );

	if ( ! ( account_name =
			dictionary_fetch(
				hard_coded_dictionary,
				key ) ) )
	{
		if ( !warning_only )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d; called from %s(): cannot fetch %s from hard_coded_account_key.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 calling_function_name,
				 key );

			exit( 1 );
		}
	}

	return account_name;

} /* ledger_get_hard_coded_account_name() */

char *ledger_get_hard_coded_dictionary_key(
					char *fund_name,
					char *hard_coded_account_key )
{
	static char key[ 128 ];

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf(	key,
				"%s|%s",
				fund_name,
				hard_coded_account_key );
	}
	else
	{
		strcpy(	key, hard_coded_account_key );
	}

	return key;

} /* ledger_get_hard_coded_dictionary_key() */

void ledger_order_entry_account_names(
				char **checking_account,
				char **receivable_account,
				char **revenue_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = LEDGER_CASH_KEY;
	*checking_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = LEDGER_ACCOUNT_RECEIVABLE_KEY;
	*receivable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = LEDGER_REVENUE_KEY;
	*revenue_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

} /* ledger_order_entry_account_names() */

void ledger_get_depreciation_account_names(
				char **depreciation_expense_account,
				char **accumulated_depreciation_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = "depreciation_expense_key";
	*depreciation_expense_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = LEDGER_ACCUMULATED_KEY;
	*accumulated_depreciation_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

} /* ledger_get_depreciation_account_names() */

void ledger_get_vendor_payment_account_names(
				char **checking_account,
				char **uncleared_checks_account,
				char **account_payable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = LEDGER_CASH_KEY;
	*checking_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	*uncleared_checks_account = '\0';

	key = LEDGER_UNCLEARED_CHECKS_KEY;
	*uncleared_checks_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */,
			__FUNCTION__ );

	key = LEDGER_ACCOUNT_PAYABLE_KEY;
	*account_payable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

} /* ledger_get_vendor_payment_account_names() */

void ledger_get_customer_sale_account_names(
				char **sales_tax_payable_account,
				char **shipping_revenue_account,
				char **account_receivable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = "sales_tax_payable_key";
	*sales_tax_payable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */,
			__FUNCTION__ );

	key = "shipping_revenue_key";
	*shipping_revenue_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */,
			__FUNCTION__ );

	key = "account_receivable_key";
	*account_receivable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			1 /* warning_only */,
			__FUNCTION__ );

} /* ledger_get_customer_sale_account_names() */

void ledger_get_account_payable_account_name(
				char **account_payable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	if ( account_payable_account )
	{
		key = LEDGER_ACCOUNT_PAYABLE_KEY;
		*account_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

} /* ledger_get_account_payable_account_name() */

void ledger_get_purchase_order_account_names(
				char **sales_tax_expense_account,
				char **freight_in_expense_account,
				char **account_payable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	if ( sales_tax_expense_account )
	{
		key = "sales_tax_key";
		*sales_tax_expense_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				1 /* warning_only */,
			__FUNCTION__ );
	}

	if ( freight_in_expense_account )
	{
		key = "freight_in_key";
		*freight_in_expense_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				1 /* warning_only */,
			__FUNCTION__ );
	}

	if ( account_payable_account )
	{
		key = LEDGER_ACCOUNT_PAYABLE_KEY;
		*account_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

} /* ledger_get_purchase_order_account_names() */

void ledger_journal_delete(		char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;

	table_name = get_table_name( application_name, LEDGER_FOLDER_NAME );

	if ( account_name && *account_name )
	{
		field= "full_name,street_address,transaction_date_time,account";
	}
	else
	{
		field= "full_name,street_address,transaction_date_time";
	}

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	if ( account_name && *account_name )
	{
		fprintf(	output_pipe,
				"%s^%s^%s^%s\n",
				full_name,
				street_address,
				transaction_date_time,
				account_name );
	}
	else
	{
		fprintf(	output_pipe,
				"%s^%s^%s\n",
				full_name,
				street_address,
				transaction_date_time );
	}

	pclose( output_pipe );

} /* ledger_journal_delete() */

void ledger_delete(			char *application_name,
					char *folder_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field;
	FILE *output_pipe;
	char *table_name;

	field= "full_name,street_address,transaction_date_time";

	table_name = get_table_name( application_name, folder_name );

	sprintf( sys_string,
		 "delete_statement table=%s field=%s delimiter='^'	|"
		 "sql.e							 ",
		 table_name,
		 field );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"%s^%s^%s\n",
			full_name,
			street_address,
			transaction_date_time );

	pclose( output_pipe );

} /* ledger_delete() */

boolean ledger_propagate_journal_ledger_list(
			char *application_name,
			char *transaction_date_time,
			LIST *journal_ledger_list )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) ) return 0;

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		ledger_propagate(
			application_name,
			transaction_date_time,
			journal_ledger->account_name );

	} while( list_next( journal_ledger_list ) );

	return 1;

} /* ledger_propagate_journal_ledger_list() */

void ledger_propagate_account_name_list(
			char *application_name,
			char *transaction_date_time,
			LIST *account_name_list )
{
	char *account_name;

	if ( !list_rewind( account_name_list ) ) return;

	do {
		account_name = list_get_pointer( account_name_list );

		ledger_propagate(
			application_name,
			transaction_date_time,
			account_name );

	} while( list_next( account_name_list ) );

} /* void ledger_propagate_account_name_list() */

void ledger_propagate(			char *application_name,
					char *transaction_date_time,
					char *account_name )
{
	JOURNAL_LEDGER *prior_ledger;
	LIST *propagate_journal_ledger_list;

	if ( !account_name )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: empty account_name for transaction_date_time = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 transaction_date_time );
		return;
	}

	prior_ledger =
		ledger_get_prior_ledger(
			application_name,
			transaction_date_time,
			account_name );

	propagate_journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			account_name );

	ledger_journal_ledger_list_propagate(
			propagate_journal_ledger_list,
			application_name );

} /* ledger_propagate() */

char *ledger_get_transaction_where(	char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *folder_name,
					char *date_time_column )
{
	static char where[ 1024 ];
	char buffer[ 512 ];

	if ( folder_name )
	{
		if ( full_name )
		{
			sprintf(where,
		 		"%s.full_name = '%s' and		"
	  	 		"%s.street_address = '%s' and		"
	  	 		"%s.%s = '%s'		 		",
				folder_name,
		 		escape_character(	buffer,
							full_name,
							'\'' ),
				folder_name,
		 		street_address,
				folder_name,
		 		date_time_column,
		 		transaction_date_time );
		}
		else
		{
			sprintf(where,
	  	 		"%s.%s = '%s'",
				folder_name,
		 		date_time_column,
		 		transaction_date_time );
		}
	}
	else
	{
		if ( full_name )
		{
			sprintf(where,
		 		"full_name = '%s' and			"
	  	 		"street_address = '%s' and		"
	  	 		"%s = '%s'		 		",
		 		escape_character(	buffer,
							full_name,
							'\'' ),
		 		street_address,
		 		date_time_column,
		 		transaction_date_time );
		}
		else
		{
			sprintf(where,
	  	 		"%s = '%s'",
		 		date_time_column,
		 		transaction_date_time );
		}
	}

	return where;

} /* ledger_get_transaction_where() */

LIST *ledger_get_propagate_journal_ledger_list(
					char *application_name,
					JOURNAL_LEDGER *prior_ledger,
					char *account_name )
{
	LIST *ledger_list = {0};
	boolean accumulate_debit;
	JOURNAL_LEDGER *first_journal_ledger;

	if ( !account_name ) return (LIST *)0;

	accumulate_debit =
		ledger_account_get_accumulate_debit(
			application_name, account_name );

	if ( prior_ledger )
	{
		ledger_list =
			ledger_get_journal_ledger_list(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				prior_ledger->transaction_date_time
					/* minimum_transaction_date_time */,
				account_name );
	}
	else
	/* ------------------------------------------------- */
	/* Otherwise doing period of record for the account. */
	/* ------------------------------------------------- */
	{
		ledger_list =
			ledger_get_journal_ledger_list(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* minimum_transaction_date_time */,
				account_name );

		if ( !list_length( ledger_list ) ) return (LIST *)0;

		first_journal_ledger = list_get_first_pointer( ledger_list );
		first_journal_ledger->previous_balance = 0.0;
		first_journal_ledger->transaction_count = 1;
	}

	/* If deleted the latest one, then no propagate. */
	/* --------------------------------------------- */
	if ( prior_ledger && list_length( ledger_list ) == 1 )
	{
		return (LIST *)0;
	}

	ledger_list_set_balances(	ledger_list,
					accumulate_debit );

	return ledger_list;

} /* ledger_get_propagate_journal_ledger_list() */

/* Returns heap memory. */
/* -------------------- */
char *ledger_max_transaction_date(
				char *application_name )
{
	char *r;

	r = ledger_fetch_max_transaction_date_time( application_name );

	*( r + 10 ) = '\0';

	return r;

} /* ledger_max_transaction_date() */

/* Returns heap memory. */
/* -------------------- */
char *ledger_fetch_max_transaction_date_time(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *select;

	select = "max( transaction_date_time )";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		",
		 application_name,
		 select,
		 TRANSACTION_FOLDER_NAME );

	return pipe2string( sys_string );

} /* ledger_fetch_max_transaction_date_time() */

boolean ledger_transaction_exists(	char *application_name,
					char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;
	boolean return_value;

	select = "count";

	where = ledger_get_transaction_where(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				transaction_date_time,
				"transaction" /* folder_name */,
				"transaction_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 TRANSACTION_FOLDER_NAME,
		 where );

	results = pipe2string( sys_string );
	return_value = (*results == '1');
	free( results );

	return return_value;

} /* ledger_transaction_exists() */

LIST *ledger_fetch_transaction_list(	char *application_name,
					char *where_clause )
{
	char sys_string[ 1024 ];
	char *select;
	char input_buffer[ 512 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char check_number[ 128 ];
	char property_street_address[ 128 ];
	char memo[ 1024 ];
	LIST *transaction_list = list_new();
	FILE *input_pipe;
	TRANSACTION *transaction;
	boolean property_street_address_exists;

	property_street_address_exists =
		attribute_exists(
			application_name,
			"transaction"
				/* folder_name */,
			"property_street_address"
				/* attribute_name */ );

	if ( property_street_address_exists )
	{
		select =
"full_name,street_address,transaction_date_time,memo,check_number,property_street_address";
	}
	else
	{
		select = "full_name,street_address,transaction_date_time,memo,check_number";
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=transaction		"
		 "			where=\"%s\"			"
		 "			order=transaction_date_time    |"
		 "cat							",
		 application_name,
		 select,
		 (where_clause) ? where_clause : "1 = 1" );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	full_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	transaction_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	memo,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		transaction = ledger_transaction_new(
					strdup( full_name ),
					strdup( street_address ),
					strdup( transaction_date_time ),
					strdup( memo ) );

		piece(	check_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		transaction->check_number = atoi( check_number );

		if ( property_street_address_exists )
		{
			piece(	property_street_address,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				5 );

			transaction->property_street_address =
				strdup( property_street_address );
		}

		transaction->journal_ledger_list =
			ledger_get_journal_ledger_list(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time
					/* minimum_transaction_date_time */,
				(char *)0 /* account_name */ );

		list_append_pointer( transaction_list, transaction );
	}

	pclose( input_pipe );
	return transaction_list;

} /* ledger_fetch_transaction_list() */

char *ledger_get_transaction_date_time( char *transaction_date )
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

} /* ledger_get_transaction_date_time() */

LIST *ledger_transaction_date_time_account_name_list(
				char *application_name,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf(	where,
			"transaction_date_time = '%s'",
			transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=account		"
		 "			folder=journal_ledger	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	return pipe2list( sys_string );

} /* ledger_transaction_date_time_account_name_list() */

double ledger_get_account_debit_amount(	LIST *journal_ledger_list,
					char *account_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( ! ( journal_ledger =
			ledger_journal_ledger_seek(
				journal_ledger_list,
				account_name ) ) )
	{
		return 0.0;
	}
	else
	{
		return journal_ledger->debit_amount;
	}

} /* ledger_get_account_debit_amount() */

double ledger_get_account_credit_amount(LIST *journal_ledger_list,
					char *account_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( ! ( journal_ledger =
			ledger_journal_ledger_seek(
				journal_ledger_list,
				account_name ) ) )
	{
		return 0.0;
	}
	else
	{
		return journal_ledger->credit_amount;
	}

} /* ledger_get_account_credit_amount() */

JOURNAL_LEDGER *ledger_seek_journal_ledger(
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) )
		return (JOURNAL_LEDGER *)0;

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		if ( timlib_strcmp(	journal_ledger->full_name,
					full_name ) == 0
		&&   timlib_strcmp(	journal_ledger->street_address,
					street_address ) == 0
		&&   timlib_strcmp(	journal_ledger->transaction_date_time,
					transaction_date_time ) == 0 )
		{
			if ( !account_name ) return journal_ledger;

			if ( strcmp(	account_name,
					journal_ledger->account_name ) == 0 )
			{
				return journal_ledger;
			}
		}

	} while( list_next( journal_ledger_list ) );

	return (JOURNAL_LEDGER *)0;

} /* ledger_seek_journal_ledger() */

JOURNAL_LEDGER *ledger_check_number_seek_journal_ledger(
				LIST *cash_journal_ledger_list,
				int check_number )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( cash_journal_ledger_list ) )
		return (JOURNAL_LEDGER *)0;

	do {
		journal_ledger = list_get_pointer( cash_journal_ledger_list );

		if ( journal_ledger->check_number == check_number )
		{
			return journal_ledger;
		}

	} while( list_next( cash_journal_ledger_list ) );

	return (JOURNAL_LEDGER *)0;

} /* ledger_check_number_seek_journal_ledger() */

JOURNAL_LEDGER *ledger_journal_ledger_seek(
					LIST *journal_ledger_list,
					char *account_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) ) return (JOURNAL_LEDGER *)0;

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		if ( timlib_strcmp(	journal_ledger->account_name,
					account_name ) == 0 )
		{
			return journal_ledger;
		}

	} while( list_next( journal_ledger_list ) );

	return (JOURNAL_LEDGER *)0;

} /* ledger_journal_ledger_seek() */

void ledger_debit_credit_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *debit_account_name,
					char *credit_account_name,
					double transaction_amount )
{
	FILE *update_pipe;

	if (	!full_name
	||	!street_address
	||	!transaction_date_time
	||	!debit_account_name
	||	!credit_account_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty primary information.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 ); 
	}

	update_pipe = ledger_open_update_pipe( application_name );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^debit_amount^%.2lf\n",
	 	full_name,
	 	street_address,
	 	transaction_date_time,
	 	debit_account_name,
	 	transaction_amount );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^credit_amount^%.2lf\n",
	 	full_name,
	 	street_address,
	 	transaction_date_time,
	 	credit_account_name,
	 	transaction_amount );

	pclose( update_pipe );

} /* ledger_debit_credit_update() */

void ledger_journal_ledger_transaction_date_time_update(
			FILE *update_pipe,
			char *full_name,
			char *street_address,
			char *preupdate_transaction_date_time,
			char *account_name,
			char *transaction_date_time )
{
	char buffer[ 128 ];

	if (	!full_name
	||	!street_address
	||	!preupdate_transaction_date_time
	||	!account_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty primary key.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		pclose( update_pipe );
		exit( 1 ); 
	}

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^transaction_date_time^%s\n",
		escape_character(	buffer,
					full_name,
					'\'' ),
	 	street_address,
	 	preupdate_transaction_date_time,
	 	account_name,
	 	(transaction_date_time)
			? transaction_date_time
			: "" );

	fflush( update_pipe );

} /* ledger_journal_ledger_transaction_date_time_update() */

void ledger_journal_ledger_update(	FILE *update_pipe,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *account_name,
					double debit_amount,
					double credit_amount )
{
	char buffer[ 128 ];

	if (	!full_name
	||	!street_address
	||	!transaction_date_time
	||	!account_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty primary key.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		pclose( update_pipe );
		exit( 1 ); 
	}

	if ( !timlib_dollar_virtually_same(
		debit_amount,
		0.0 ) )
	{
		fprintf(update_pipe,
		 	"%s^%s^%s^%s^debit_amount^%.2lf\n",
			escape_character(	buffer,
						full_name,
						'\'' ),
		 	street_address,
		 	transaction_date_time,
		 	account_name,
		 	debit_amount );
	}
	else
	if ( !timlib_dollar_virtually_same(
		credit_amount,
		0.0 ) )
	{
		fprintf(update_pipe,
		 	"%s^%s^%s^%s^credit_amount^%.2lf\n",
			escape_character(	buffer,
						full_name,
						'\'' ),
		 	street_address,
		 	transaction_date_time,
		 	account_name,
		 	credit_amount );
	}
	else
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: both debit_amount and credit_amount are zero for account = %s and transaction_date_time = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name,
			 transaction_date_time );

		form_output_content_type();
		printf( 
"<h3>Warning in %s/%s()/%d: both debit_amount and credit_amount are zero for account = %s and transaction_date_time = %s.</h3>\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name,
			 transaction_date_time );
		fflush( stdout );
	}

	fflush( update_pipe );

} /* ledger_journal_ledger_update() */

FILE *ledger_open_update_pipe( char *application_name )
{
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];

	table_name = get_table_name( application_name, LEDGER_FOLDER_NAME );

	key_column_list_string =
		"full_name,street_address,transaction_date_time,account";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	return popen( sys_string, "w" );

} /* ledger_open_update_pipe() */

void ledger_update_transaction_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *new_transaction_date_time )
{
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];
	FILE *update_pipe;

	key_column_list_string =
		"full_name,street_address,transaction_date_time";

	table_name =
		get_table_name(
			application_name,
			TRANSACTION_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
	 	full_name,
	 	street_address,
	 	transaction_date_time,
	 	new_transaction_date_time );

	pclose( update_pipe );

	table_name =
		get_table_name(
			application_name,
			LEDGER_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
	 	full_name,
	 	street_address,
	 	transaction_date_time,
	 	new_transaction_date_time );

	pclose( update_pipe );

} /* ledger_update_transaction_date_time() */

char *ledger_transaction_get_select( void )
{
	char *select =
"transaction.full_name,transaction.street_address,transaction.transaction_date_time,memo";

	return select;

} /* ledger_transaction_get_select() */

void ledger_transaction_parse(
				char **full_name,
				char **street_address,
				char **transaction_date_time,
				char **memo,
				char *input_buffer )
{
	char piece_buffer[ 1024 ];

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	*transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	*memo = strdup( piece_buffer );

} /* ledger_transaction_parse() */

HASH_TABLE *ledger_get_transaction_hash_table(
				char *application_name,
				char *inventory_name )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder_list_string;
	char *inventory_subquery;
	char where[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	HASH_TABLE *transaction_hash_table;

	transaction_hash_table = hash_table_new( hash_table_huge );

	select = ledger_transaction_get_select();

	folder_list_string = "transaction,purchase_order";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"purchase_order"     /* one2m_folder_name */,
			"inventory_purchase" /* mto1_folder_name */,
			"purchase_date_time" /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "purchase_order.transaction_date_time is not null",
		 inventory_subquery,
		 ledger_get_transaction_purchase_order_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		ledger_set_transaction_hash_table(
			transaction_hash_table,
			input_buffer );
	}

	pclose( input_pipe );

	folder_list_string = "transaction,customer_sale";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"customer_sale"      /* one2m_folder_name */,
			"inventory_sale"     /* mto1_folder_name */,
			"sale_date_time"     /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "customer_sale.transaction_date_time is not null",
		 inventory_subquery,
		 ledger_get_transaction_customer_sale_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		ledger_set_transaction_hash_table(
			transaction_hash_table,
			input_buffer );
	}

	pclose( input_pipe );

	return transaction_hash_table;

} /* ledger_get_transaction_hash_table() */

void ledger_set_transaction_hash_table(
				HASH_TABLE *transaction_hash_table,
				char *input_buffer )
{
	TRANSACTION *transaction;
	char *key;

	transaction = ledger_transaction_calloc();

	ledger_transaction_parse(
			&transaction->full_name,
			&transaction->street_address,
			&transaction->transaction_date_time,
			&transaction->memo,
			input_buffer );

	key = ledger_get_transaction_hash_table_key(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time );

	hash_table_set_pointer(
			transaction_hash_table,
			strdup( key ),
			transaction );

} /* ledger_set_transaction_hash_table() */

char *ledger_get_transaction_hash_table_key(
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	static char key[ 256 ];

	sprintf(	key,
			"%s^%s^%s",
			full_name,
			street_address,
			transaction_date_time );

	return key;

} /* ledger_get_transaction_hash_table_key() */

char *ledger_journal_ledger_get_select( void )
{
	char *select =
"journal_ledger.full_name,journal_ledger.street_address,journal_ledger.transaction_date_time,account,transaction_count,previous_balance,debit_amount,credit_amount,balance";

	return select;

} /* ledger_journal_ledger_get_select() */

char *ledger_get_transaction_purchase_order_join_where( void )
{
	char *join_where;

	join_where =	"transaction.full_name =			"
			"	purchase_order.full_name and		"
			"transaction.street_address =			"
			"	purchase_order.street_address and	"
			"transaction.transaction_date_time =		"
			"	purchase_order.transaction_date_time	";

	return join_where;

} /* ledger_get_transaction_purchase_order_join_where() */

char *ledger_get_transaction_customer_sale_join_where( void )
{
	char *join_where;

	join_where =	"transaction.full_name =			"
			"	customer_sale.full_name and		"
			"transaction.street_address =			"
			"	customer_sale.street_address and	"
			"transaction.transaction_date_time =		"
			"	customer_sale.transaction_date_time	";

	return join_where;

} /* ledger_get_transaction_purchase_order_join_where() */

char *ledger_get_journal_ledger_purchase_order_join_where( void )
{
	char *join_where;

	join_where =	"journal_ledger.full_name =			"
			"	purchase_order.full_name and		"
			"journal_ledger.street_address =		"
			"	purchase_order.street_address and	"
			"journal_ledger.transaction_date_time =		"
			"	purchase_order.transaction_date_time	";

	return join_where;

} /* ledger_get_journal_ledger_purchase_order_join_where() */

char *ledger_get_journal_ledger_customer_sale_join_where( void )
{
	char *join_where;

	join_where =	"journal_ledger.full_name =			"
			"	customer_sale.full_name and		"
			"journal_ledger.street_address =		"
			"	customer_sale.street_address and	"
			"journal_ledger.transaction_date_time =		"
			"	customer_sale.transaction_date_time	";

	return join_where;

} /* ledger_get_journal_ledger_customer_sale_join_where() */

char *ledger_get_journal_ledger_transaction_join_where( void )
{
	char *join_where;

	join_where =	"journal_ledger.full_name =			"
			"	transaction.full_name and		"
			"journal_ledger.street_address =		"
			"	transaction.street_address and		"
			"journal_ledger.transaction_date_time =		"
			"	transaction.transaction_date_time	";

	return join_where;

} /* ledger_get_journal_ledger_transaction_order_join_where() */

void ledger_journal_ledger_parse(
				char **full_name,
				char **street_address,
				char **transaction_date_time,
				char **account_name,
				int *transaction_count,
				int *database_transaction_count,
				double *previous_balance,
				double *database_previous_balance,
				double *debit_amount,
				double *credit_amount,
				double *balance,
				double *database_balance,
				char *input_buffer )
{
	char piece_buffer[ 1024 ];

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	*transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	*account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	*transaction_count =
	*database_transaction_count = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	*previous_balance =
	*database_previous_balance = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	*debit_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	*credit_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	*balance =
	*database_balance = atof( piece_buffer );

} /* ledger_journal_ledger_parse() */

HASH_TABLE *ledger_get_journal_ledger_hash_table(
				char *application_name,
				char *inventory_name )
{
	char sys_string[ 2048 ];
	char *select;
	char *folder_list_string;
	char *inventory_subquery;
	char where[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	HASH_TABLE *journal_ledger_hash_table;

	journal_ledger_hash_table = hash_table_new( hash_table_huge );

	select = ledger_journal_ledger_get_select();

	folder_list_string = "journal_ledger,purchase_order";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"purchase_order"     /* one2m_folder_name */,
			"inventory_purchase" /* mto1_folder_name */,
			"purchase_date_time" /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "purchase_order.transaction_date_time is not null",
		 inventory_subquery,
		 ledger_get_journal_ledger_purchase_order_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		ledger_set_journal_ledger_hash_table(
			journal_ledger_hash_table,
			input_buffer );
	}

	pclose( input_pipe );

	folder_list_string = "journal_ledger,customer_sale";

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"customer_sale"  /* one2m_folder_name */,
			"inventory_sale" /* mto1_folder_name */,
			"sale_date_time" /* foreign_attribute_name */ );

	sprintf( where,
		 "%s and %s and "
		 "customer_sale.transaction_date_time is not null",
		 inventory_subquery,
		 ledger_get_journal_ledger_customer_sale_join_where() );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_list_string,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		ledger_set_journal_ledger_hash_table(
			journal_ledger_hash_table,
			input_buffer );
	}

	pclose( input_pipe );

	return journal_ledger_hash_table;

} /* ledger_get_journal_ledger_hash_table() */

void ledger_set_journal_ledger_hash_table(
				HASH_TABLE *journal_ledger_hash_table,
				char *input_buffer )
{
	JOURNAL_LEDGER *journal_ledger;
	char *key;

	journal_ledger = ledger_journal_ledger_calloc();

	ledger_journal_ledger_parse(
			&journal_ledger->full_name,
			&journal_ledger->street_address,
			&journal_ledger->transaction_date_time,
			&journal_ledger->account_name,
			&journal_ledger->transaction_count,
			&journal_ledger->database_transaction_count,
			&journal_ledger->previous_balance,
			&journal_ledger->database_previous_balance,
			&journal_ledger->debit_amount,
			&journal_ledger->credit_amount,
			&journal_ledger->balance,
			&journal_ledger->database_balance,
			input_buffer );

	key = ledger_get_journal_ledger_hash_table_key(
			journal_ledger->full_name,
			journal_ledger->street_address,
			journal_ledger->transaction_date_time,
			journal_ledger->account_name );

	hash_table_set_pointer(
		journal_ledger_hash_table,
		strdup( key ),
		journal_ledger );

} /* ledger_get_journal_ledger_hash_table() */

char *ledger_get_journal_ledger_hash_table_key(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *account_name )
{
	static char key[ 256 ];

	if ( !account_name ) return (char *)0;

	sprintf(	key,
			"%s^%s^%s^%s",
			full_name,
			street_address,
			transaction_date_time,
			account_name );

	return key;

} /* ledger_get_journal_ledger_hash_table_key() */

char *ledger_get_shipped_date_transaction_date_time(
				char *shipped_date )
{
	char transaction_date_time[ 32 ];

	sprintf( transaction_date_time,
		 "%s %s",
		 shipped_date,
		 date_get_now_hh_colon_mm_colon_ss(
			date_get_utc_offset() ) );

	return strdup( transaction_date_time );

} /* ledger_get_shipped_date_transaction_date_time() */

char *ledger_get_account_subclassification_name(
				char *application_name,
				char *account_name )
{
	char where[ 256 ];
	char sys_string[ 1024 ];
	char buffer[ 128 ];

	sprintf( where, "account = '%s'",
		 timlib_escape_single_quotes(
			buffer,
			account_name ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=subclassification	"
		 "			folder=account			"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* ledger_get_account_subclassification_name() */

char *ledger_get_supply_expense_account(
				char *application_name,
				char *supply_name )
{
	char where[ 256 ];
	char sys_string[ 1024 ];

	sprintf( where, "supply_name = '%s'", supply_name );
	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=account			"
		 "			folder=supply			"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* ledger_get_supply_expense_account() */

LEDGER *ledger_new( void )
{
	LEDGER *ledger;

	if ( ! ( ledger = calloc( 1, sizeof( LEDGER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger->element_list = list_new();

	return ledger;

} /* ledger_new() */

CONTRA_ACCOUNT *ledger_contra_account_new(	char *account_name,
						char *contra_to_account_name )
{
	CONTRA_ACCOUNT *contra_account;

	if ( ! ( contra_account = calloc( 1, sizeof( CONTRA_ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	contra_account->account_name = account_name;
	contra_account->contra_to_account_name = contra_to_account_name;

	return contra_account;

} /* ledger_contra_account_new() */

LIST *ledger_get_contra_account_list( char *application_name )
{
	LIST *contra_account_list;
	CONTRA_ACCOUNT *contra_account;
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	char account_name[ 128 ];
	char contra_to_account_name[ 128 ];
	FILE *input_pipe;
	char *select;

	select = "account,contra_to_account";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=contra_account	",
		 application_name,
		 select );

	contra_account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	contra_to_account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		contra_account =
			ledger_contra_account_new(
				strdup( account_name ),
				strdup( contra_to_account_name ) );

		list_append_pointer( contra_account_list, contra_account );
	}

	pclose( input_pipe );
	return contra_account_list;

} /* ledger_get_contra_account_list() */

CONTRA_ACCOUNT *ledger_seek_contra_account(
				LIST *contra_account_list,
				char *account_name )
{
	CONTRA_ACCOUNT *contra_account;

	if ( !list_rewind( contra_account_list ) ) return (CONTRA_ACCOUNT *)0;

	do {
		contra_account = list_get( contra_account_list );

		if ( strcmp(	contra_account->account_name,
				account_name ) == 0 )
		{
			return contra_account;
		}

	} while( list_next( contra_account_list ) );

	return (CONTRA_ACCOUNT *)0;

} /* ledger_seek_contra_account() */

boolean ledger_account_get_accumulate_debit(
					char *application_name,
					char *account_name )
{
	LEDGER_ELEMENT *element;

	if ( ! ( element =
			ledger_account_fetch_element(
				application_name,
				account_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot fetch element for account = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name );
		return 0;
	}

	return element->accumulate_debit;

} /* ledger_account_get_accumulate_debit() */

int ledger_balance_match_function(
				ACCOUNT *account_from_list,
				ACCOUNT *account_compare )
{
	if ( !account_from_list->latest_ledger
	||   !account_from_list->latest_ledger->balance )
	{
		return 1;
	}

	if ( !account_compare->latest_ledger
	||   !account_compare->latest_ledger->balance )
	{
		return 1;
	}

	/* Sort descending */
	/* --------------- */
	if (	account_from_list->latest_ledger->balance <=
		account_compare->latest_ledger->balance )
	{
		return 1;
	}
	else
	{
		return -1;
	}

} /* ledger_balance_match_function() */

void ledger_propagate_element_list(
				char *application_name,
				char *transaction_date_time_string,
				LIST *element_list )
{
	LEDGER_ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get_pointer( element_list );

		if ( !list_rewind( element->subclassification_list ) ) continue;

		do {
			subclassification =
				list_get_pointer(
					element->subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;

			do {
				account =
					list_get_pointer( 
						subclassification->
							account_list );

				if ( account->latest_ledger
				&&   account->latest_ledger->balance )
				{
					ledger_propagate(
						application_name,
						transaction_date_time_string,
						account->account_name );
				}


			} while( list_next( subclassification->account_list ) );

		} while( list_next( element->subclassification_list ) );

	} while( list_next( element_list ) );

} /* ledger_propagate_element_list() */

boolean ledger_transaction_date_time_exists(
				char *application_name,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *results;

	sprintf( where,
		 "transaction_date_time = '%s'",
		 transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=transaction	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( results && ( atoi( results ) == 1 ) )
		return 1;
	else
		return 0;

} /* ledger_transaction_date_time_exists() */

char *ledger_get_existing_closing_transaction_date_time(
				char *application_name,
				char *as_of_date )
{
	char transaction_date_time[ 32 ];
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *results;

	sprintf(transaction_date_time,
		"%s %s",
		as_of_date,
		LEDGER_CLOSING_TRANSACTION_TIME );

	sprintf( where,
		 "transaction_date_time = '%s' and memo = '%s'",
		 transaction_date_time,
		 LEDGER_CLOSING_ENTRY_MEMO );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=transaction	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( results && ( atoi( results ) == 1 ) )
		return strdup( transaction_date_time );
	else
		return (char *)0;

} /* ledger_get_existing_closing_transaction_date_time() */

char *ledger_get_closing_transaction_date_time(
				char *as_of_date )
{
	char transaction_date_time[ 32 ];

	sprintf(	transaction_date_time,
			"%s %s",
			as_of_date,
			LEDGER_CLOSING_TRANSACTION_TIME );

#ifdef NOT_DEFINED
	char sys_string[ 1024 ];
	char where[ 128 ];
	char *results;
	sprintf( where,
		 "transaction_date_time = '%s'",
		 transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=transaction	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( results && ( atoi( results ) == 1 ) )
		return strdup( transaction_date_time ) ;
	else
		return (char *)0;
#endif

	return strdup( transaction_date_time );

} /* ledger_get_closing_transaction_date_time() */

DATE *ledger_prior_closing_transaction_date(
				char *application_name,
				char *fund_name,
				char *ending_transaction_date )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;
	char folder[ 128 ];
	char *results;
	char ending_transaction_date_time[ 32 ];
	char transaction_date_string[ 16 ];
	DATE *prior_closing_transaction_date = {0};

	if ( fund_name && !*fund_name )
		fund_name = (char *)0;
	else
	if ( fund_name && strcmp( fund_name, "fund" ) == 0 )
		fund_name = (char *)0;

	strcpy( folder, TRANSACTION_FOLDER_NAME );
	select = "max( transaction_date_time )";

	sprintf( ending_transaction_date_time,
		 "%s %s",
		 ending_transaction_date,
		 LEDGER_CLOSING_TRANSACTION_TIME );

	sprintf( where,
		 "memo = '%s' and transaction_date_time < '%s'",
		 LEDGER_CLOSING_ENTRY_MEMO,
		 ending_transaction_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
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

} /* ledger_prior_closing_transaction_date() */

char *ledger_beginning_transaction_date(
				char *application_name,
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
			ledger_prior_closing_transaction_date(
				application_name,
				fund_name,
				ending_transaction_date );
	
		if ( prior_closing_transaction_date )
		{
			date_increment_days(
				prior_closing_transaction_date,
				1 );
	
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
		 	LEDGER_FOLDER_NAME,
		 	ACCOUNT_FOLDER_NAME );

		sprintf(where,
		 	"fund = '%s' and				"
			"%s.account = %s.account			",
		 	fund_name,
			LEDGER_FOLDER_NAME,
			ACCOUNT_FOLDER_NAME );
	}
	else
	{
		/* Get the first entry. */
		/* -------------------- */
		strcpy( folder, LEDGER_FOLDER_NAME );

		strcpy( where, "1 = 1" );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );

} /* ledger_beginning_transaction_date() */

LIST *ledger_get_fund_name_list( char *application_name )
{
	return ledger_fetch_fund_name_list( application_name );
}

LIST *ledger_fetch_fund_name_list( char *application_name )
{
	char sys_string[ 512 ];

	if ( !ledger_fund_attribute_exists(
				application_name ) )
	{
		return (LIST *)0;
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=fund	"
		 "			folder=fund	",
		 application_name );

	return pipe2list( sys_string );

} /* ledger_fetch_fund_name_list() */

/* Returns begin_date_string */
/* ------------------------- */
char *ledger_get_report_title_sub_title(
		char *title,
		char *sub_title,
		char *process_name,
		char *application_name,
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
			application_name ) );

	if ( ! ( begin_date_string = 
			ledger_beginning_transaction_date(
				application_name,
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

} /* ledger_get_report_title_sub_title() */

/* Reset debit_amount and credit_amount without having to refresh. */
/* --------------------------------------------------------------- */
boolean ledger_journal_ledger_list_reset_amount(
			LIST *journal_ledger_list,
			double amount )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( list_length( journal_ledger_list ) != 2 )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: expecting list length of 2. Got %d, instead.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( journal_ledger_list ) );
		return 0;
	}

	list_rewind( journal_ledger_list );

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		if ( !timlib_dollar_virtually_same(
			journal_ledger->debit_amount,
			0.0 ) )
		{
			journal_ledger->debit_amount = amount;
		}
		else
		if ( !timlib_dollar_virtually_same(
			journal_ledger->credit_amount,
			0.0 ) )
		{
			journal_ledger->credit_amount = amount;
		}
		else
		{
			return 0;
		}

	} while( list_next( journal_ledger_list ) );

	return 1;

} /* ledger_journal_ledger_list_reset_amount() */

boolean ledger_journal_ledger_list_amount_update(
			char *application_name,
			LIST *journal_ledger_list )
{
	JOURNAL_LEDGER *journal_ledger;
	FILE *update_pipe;

	if ( !list_rewind( journal_ledger_list ) ) return 0;

	update_pipe = ledger_open_update_pipe( application_name );

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		ledger_journal_ledger_update(
			update_pipe,
			journal_ledger->full_name,
			journal_ledger->street_address,
			journal_ledger->transaction_date_time,
			journal_ledger->account_name,
			journal_ledger->debit_amount,
			journal_ledger->credit_amount );

	} while( list_next( journal_ledger_list ) );

	pclose( update_pipe );
	return 1;

} /* ledger_journal_ledger_list_amount_update() */

LIST *ledger_get_inventory_account_name_list(
			char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "populate_account.sh %s inventory ''",
		 application_name );

	return pipe2list( sys_string );

} /* ledger_get_inventory_account_name_list() */

LIST *ledger_get_service_account_name_list(
			char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "populate_account.sh %s hourly_service ''",
		 application_name );

	return pipe2list( sys_string );

} /* ledger_get_service_account_name_list() */

ACCOUNT *ledger_element_list_account_seek(
			LIST *element_list,
			char *account_name )
{
	LEDGER_ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( element_list ) ) return (ACCOUNT *)0;

	do {
		element = list_get_pointer( element_list );

		if ( list_length( element->account_list ) )
		{
			return ledger_seek_account(
					element->account_list,
					account_name );
		}

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		do {
			subclassification =
				list_get_pointer(
					element->
						subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;


			if ( ( account =
				ledger_seek_account(
					subclassification->account_list,
					account_name ) ) )
			{
				return account;
			}

		} while( list_next( element->subclassification_list ) );

	} while( list_next( element_list ) );

	return (ACCOUNT *)0;

} /* ledger_element_list_account_seek() */

boolean ledger_is_period_element( char *element_name )
{
	if ( !element_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty element_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( strcmp(	element_name,
			LEDGER_REVENUE_ELEMENT ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			LEDGER_EXPENSE_ELEMENT ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			LEDGER_GAIN_ELEMENT ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			LEDGER_LOSS_ELEMENT ) == 0 )
		return 1;
	else
		return 0;

} /* ledger_is_period_element() */

double ledger_get_fraction_of_year(
				char *prior_date_string,
				char *date_string )
{
	int days_between;
	DATE *prior_date;
	DATE *date;

	if ( !prior_date_string
	||   !*prior_date_string
	||   !date_string
	||   !*date_string )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: empty date = [prior=%s or date=%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 prior_date_string,
			 date_string );
		return 0.0;
	}

	prior_date =
		date_yyyy_mm_dd_new(
			prior_date_string );

	date = date_yyyy_mm_dd_new( date_string );

	days_between =
		date_subtract_days(
			date /* later_date */,
			prior_date /* earlier_date */ );

	return 	(double)days_between /
		(double)date_get_days_in_year(
				date_get_year(
					prior_date ) );

} /* ledger_get_fraction_of_year() */

boolean ledger_propagate_account_list_exists(
		LIST *propagate_account_list,
		char *account_name )
{
	ACCOUNT *account;

	if ( !list_rewind( propagate_account_list ) ) return 0;

	do {
		account = list_get_pointer( propagate_account_list );

		if ( strcmp( account_name, account->account_name ) == 0 )
			return 1;

	} while( list_next( propagate_account_list ) );

	return 0;

} /* ledger_propagate_account_list_exists() */

void ledger_append_propagate_account_list(
			LIST *propagate_account_list,
			char *transaction_date_time,
			char *account_name,
			char *application_name )
{
	JOURNAL_LEDGER *prior_ledger;
	ACCOUNT *account;

	if ( ledger_propagate_account_list_exists(
		propagate_account_list,
		account_name ) )
	{
		return;
	}

	account = ledger_account_new( account_name );

	prior_ledger = ledger_get_prior_ledger(
				application_name,
				transaction_date_time,
				account->account_name );

	account->journal_ledger_list =
		ledger_get_propagate_journal_ledger_list(
			application_name,
			prior_ledger,
			account->account_name );

	list_append_pointer( propagate_account_list, account );

} /* ledger_append_propagate_account_list() */

char *ledger_get_latest_zero_balance_transaction_date_time(
					char *application_name,
					char *account_name )
{
	char buffer[ 128 ];
	char where[ 512 ];
	char select[ 128 ];
	char sys_string[ 1024 ];
	char *table;
	char *results;

	table = get_table_name( application_name, LEDGER_FOLDER_NAME );

	sprintf( where,
	"account = '%s' and balance = 0",
		 timlib_escape_single_quotes( buffer, account_name ) );

	strcpy( select, "max( transaction_date_time )" );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 select,
		 table,
		 where );

	results = pipe2string( sys_string );

	if ( results && *results )
		return results;
	else
		return (char *)0;

} /* ledger_get_latest_zero_balance_transaction_date_time() */

ACCOUNT *ledger_account_list_seek(	LIST *account_list,
					char *account_name )
{
	return ledger_seek_account( account_list, account_name );

}

LIST *ledger_get_after_balance_zero_journal_ledger_list(
				char *application_name,
				char *account_name )
{
	char *transaction_date_time_string;

	if ( ( transaction_date_time_string =
			ledger_get_latest_zero_balance_transaction_date_time(
				application_name,
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

	return ledger_get_journal_ledger_list(
				application_name,
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				transaction_date_time_string
					/* minimum_transaction_date_time */,
				account_name );

} /* ledger_get_after_balance_zero_journal_ledger_list() */

JOURNAL_LEDGER *ledger_get_or_set_journal_ledger(
				LIST *journal_ledger_list,
				char *account_name )
{
	JOURNAL_LEDGER *ledger;

	if ( !journal_ledger_list )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: journal_ledger_list is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( list_rewind( journal_ledger_list ) )
	{
		do {
			ledger = list_get_pointer( journal_ledger_list );

			if ( strcmp( ledger->account_name, account_name ) == 0 )
			{
				return ledger;
			}

		} while( list_next( journal_ledger_list ) );
	}

	ledger = journal_ledger_new(	(char *)0 /* full_name */,
					(char *)0 /* street_address */,
					(char *)0 /* transaction_date_time */,
					account_name );

	list_append_pointer( journal_ledger_list, ledger );

	return ledger;

} /* ledger_get_or_set_journal_ledger() */

int ledger_get_non_empty_subclassification_list_length(
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;
	int list_length = 0;

	if ( !list_rewind( subclassification_list ) ) return 0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( !timlib_double_virtually_same(
			subclassification->subclassification_total,
			0.0 ) )
		{
			list_length++;
		}

	} while( list_next( subclassification_list ) );

	return list_length;

} /* ledger_get_non_empty_subclassification_list_length() */

TRANSACTION *ledger_inventory_purchase_hash_table_build_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table )
{
	char *inventory_account_name;
	static LIST *inventory_account_name_list = {0};
	static char *account_payable_account = {0};
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *key;

	key = ledger_get_transaction_hash_table_key(
			full_name,
			street_address,
			transaction_date_time );

	if ( ! ( transaction =
			hash_table_fetch( 
				transaction_hash_table,
				key ) ) )
	{
		return (TRANSACTION *)0;
	}

	if ( !inventory_account_name_list )
	{
		inventory_account_name_list =
			ledger_get_inventory_account_name_list(
				application_name );

		ledger_get_account_payable_account_name(
			&account_payable_account,
			application_name,
			fund_name );
	}

	/* ========================= */
	/* Build journal_ledger_list */
	/* ========================= */
	transaction->journal_ledger_list = list_new();

	/* Account payable */
	/* --------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			account_payable_account );


	if ( key && ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Inventory and cost of goods sold */
	/* -------------------------------- */
	if ( list_rewind( inventory_account_name_list ) )
	{
		do {
			inventory_account_name =
				list_get_pointer(
					inventory_account_name_list );

			key = ledger_get_journal_ledger_hash_table_key(
					full_name,
					street_address,
					transaction_date_time,
					inventory_account_name );


			if ( key && ( journal_ledger =
					hash_table_fetch( 
						journal_ledger_hash_table,
						key ) ) )
			{
				list_append_pointer(
					transaction->journal_ledger_list,
					journal_ledger );
			}

		} while( list_next( inventory_account_name_list ) );
	}

	return transaction;

} /* ledger_inventory_purchase_hash_table_build_transaction() */

boolean ledger_folder_exists(	char *application_name,
				char *folder_name )
{
	return folder_exists_folder(
			application_name,
			folder_name );

} /* ledger_folder_exists() */

boolean ledger_title_passage_rule_attribute_exists(
				char *application_name,
				char *folder_name )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	 	"folder_attribute_exists.sh %s %s title_passage_rule",
	 	application_name,
	 	folder_name );

	return ( system( sys_string ) == 0 );

} /* ledger_title_passage_rule_attribute_exists() */

boolean ledger_fund_attribute_exists( char *application_name )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	 	"folder_attribute_exists.sh %s account fund",
	 	application_name );

	return ( system( sys_string ) == 0 );

} /* ledger_fund_attribute_exists() */

char *ledger_get_fund_where(	char *application_name,
				char *fund_name )
{
	char where[ 128 ];

	if ( !fund_name
	||   !*fund_name
	||   strcmp( fund_name, "fund" ) == 0 )
	{
		strcpy( where, "1 = 1" );
	}
	else
	if ( ledger_fund_attribute_exists(
			application_name ) )
	{
		sprintf(where,
		 	"fund = '%s'",
		 	fund_name );
	}
	else
	{
		strcpy( where, "1 = 1" );
	}

	return strdup( where );

} /* ledger_get_fund_where() */

TRANSACTION *ledger_sale_hash_table_build_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table )
{
	static LIST *inventory_account_name_list = {0};
	static char *sales_tax_payable_account = {0};
	static char *shipping_revenue_account = {0};
	static char *receivable_account = {0};
	char *inventory_account_name;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *key;

	key = ledger_get_transaction_hash_table_key(
			full_name,
			street_address,
			transaction_date_time );

	if ( ! ( transaction =
			hash_table_fetch( 
				transaction_hash_table,
				key ) ) )
	{
		return (TRANSACTION *)0;
	}

	if ( !inventory_account_name_list )
	{
		inventory_account_name_list =
			ledger_get_inventory_account_name_list(
				application_name );

		ledger_get_customer_sale_account_names(
			&sales_tax_payable_account,
			&shipping_revenue_account,
			&receivable_account,
			application_name,
			fund_name );
	}

	if ( !sales_tax_payable_account ) return (TRANSACTION *)0;

	/* ========================= */
	/* Build journal_ledger_list */
	/* ========================= */
	transaction->journal_ledger_list = list_new();

	/* Sales revenue */
	/* ------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			"sales_revenue_account" );


	if ( key && ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Sales tax payable */
	/* ----------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			sales_tax_payable_account );

	if ( key && ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Shipping revenue */
	/* ---------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			shipping_revenue_account );

	if ( key && ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Accounts receivable */
	/* ------------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			receivable_account );

	if ( key && ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* Inventory and cost_of_goods_sold */
	/* -------------------------------- */
	if ( list_rewind( inventory_account_name_list ) )
	{
		do {
			inventory_account_name =
				list_get_pointer(
					inventory_account_name_list );

			key = ledger_get_journal_ledger_hash_table_key(
					full_name,
					street_address,
					transaction_date_time,
					inventory_account_name );

			if ( key && ( journal_ledger =
					hash_table_fetch( 
						journal_ledger_hash_table,
						key ) ) )
			{
				list_append_pointer(
					transaction->journal_ledger_list,
					journal_ledger );
			}

		} while( list_next( inventory_account_name_list ) );
	}

	return transaction;

} /* ledger_sale_hash_table_build_transaction() */

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction,
				LIST *journal_ledger_list )
{
	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	if ( !list_length( journal_ledger_list ) ) return (char *)0;

	if ( list_length( journal_ledger_list ) == 1 )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: list_length( journal_ledger_list ) = 1.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return ledger_transaction_journal_ledger_insert(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				transaction_amount,
				memo,
				check_number,
				lock_transaction,
				journal_ledger_list );

} /* ledger_transaction_refresh() */

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_journal_ledger_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction,
				LIST *journal_ledger_list )
{
	LIST *account_name_list;
	char *account_name;
	JOURNAL_LEDGER *journal_ledger;
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	transaction_date_time =
		ledger_transaction_insert(
			application_name,
			full_name,
			street_address,
			transaction_date_time,
			transaction_amount,
			memo,
			check_number,
			lock_transaction );

	/* If inserting journal ledger list as a batch */
	/* ------------------------------------------- */
	if ( !list_length( journal_ledger_list ) )
		return transaction_date_time;

	account_name_list = list_new();

	ledger_journal_insert_open_stream(
			&debit_account_pipe,
			&credit_account_pipe,
			application_name );

	list_rewind( journal_ledger_list );

	do {
		journal_ledger =
			list_get_pointer( 
				journal_ledger_list );

		if ( timlib_dollar_virtually_same(
			journal_ledger->debit_amount,
			0.0 )
		&&   timlib_dollar_virtually_same(
			journal_ledger->credit_amount,
			0.0 ) )
		{
			continue;
		}

		if ( !timlib_dollar_virtually_same(
			journal_ledger->debit_amount,
			0.0 ) )
		{
			ledger_journal_insert_stream(
				debit_account_pipe,
				(FILE *)0 /* credit_account_pipe */,
				full_name,
				street_address,
				transaction_date_time,
				journal_ledger->debit_amount
					/* amount */,
				journal_ledger->account_name
					/* debit_account_name */,
				(char *)0 /* credit_account_name */ );
		}
		else
		{
			ledger_journal_insert_stream(
				(FILE *)0 /* debit_account_pipe */,
				credit_account_pipe,
				full_name,
				street_address,
				transaction_date_time,
				journal_ledger->credit_amount
					/* amount */,
				(char *)0 /* debit_account_name */,
				journal_ledger->account_name
					/* credit_account_name */ );
		}

		list_append_pointer(
			account_name_list,
			journal_ledger->account_name );

	} while( list_next( journal_ledger_list ) );

	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	if ( list_rewind( account_name_list ) )
	{
		do {
			account_name = list_get_pointer( account_name_list );

			ledger_propagate(
				application_name,
				transaction_date_time,
				account_name );

		} while( list_next( account_name_list ) );
	}

	return transaction_date_time;

} /* ledger_transaction_journal_ledger_insert() */

void ledger_journal_ledger_batch_insert(
				char *application_name,
				LIST *transaction_list )
{
	LIST *account_name_list;
	char *account_name;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	FILE *debit_account_pipe = {0};
	FILE *credit_account_pipe = {0};
	char *propagate_transaction_date_time = {0};

	if ( !list_rewind( transaction_list ) ) return;

	account_name_list = list_new();

	ledger_journal_insert_open_stream(
			&debit_account_pipe,
			&credit_account_pipe,
			application_name );

	do {
		transaction = list_get_pointer( transaction_list );

		if ( !list_rewind( transaction->journal_ledger_list ) )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: empty journal_ledger_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			continue;
		}

		if ( !propagate_transaction_date_time )
			propagate_transaction_date_time =
				transaction->transaction_date_time;

		do {
			journal_ledger =
				list_get_pointer( 
					transaction->journal_ledger_list );

			if ( timlib_dollar_virtually_same(
				journal_ledger->debit_amount,
				0.0 )
			&&   timlib_dollar_virtually_same(
				journal_ledger->credit_amount,
				0.0 ) )
			{
				fprintf( stderr,
		"Warning in %s/%s()/%d: both debit and credit are zero.\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__ );
				continue;
			}

			if ( !timlib_dollar_virtually_same(
				journal_ledger->debit_amount,
				0.0 ) )
			{
				ledger_journal_insert_stream(
					debit_account_pipe,
					(FILE *)0 /* credit_account_pipe */,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					journal_ledger->debit_amount
						/* amount */,
					journal_ledger->account_name
						/* debit_account_name */,
					(char *)0 /* credit_account_name */ );
			}
			else
			{
				ledger_journal_insert_stream(
					(FILE *)0 /* debit_account_pipe */,
					credit_account_pipe,
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					journal_ledger->credit_amount
						/* amount */,
					(char *)0 /* debit_account_name */,
					journal_ledger->account_name
						/* credit_account_name */ );
			}

			list_append_unique_string(
				account_name_list,
				journal_ledger->account_name );

		} while( list_next( transaction->journal_ledger_list ) );

	} while( list_next( transaction_list ) );

	pclose( debit_account_pipe );
	pclose( credit_account_pipe );

	if ( list_rewind( account_name_list ) )
	{
		do {
			account_name = list_get_pointer( account_name_list );

			ledger_propagate(
				application_name,
				propagate_transaction_date_time,
				account_name );

		} while( list_next( account_name_list ) );
	}

} /* ledger_journal_ledger_batch_insert() */

TRANSACTION *ledger_inventory_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *inventory_purchase_list,
				char *fund_name )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *account_payable_account = {0};
	double sum_debit_amount = 0.0;

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Exits if account_payable_account is not found. */
	/* ---------------------------------------------- */
	ledger_get_account_payable_account_name(
				&account_payable_account,
				application_name,
				fund_name );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->journal_ledger_list =
		purchase_inventory_distinct_account_extract(
			&sum_debit_amount,
			inventory_purchase_list );

	/* Empty if deleted */
	/* ---------------- */
	if ( !list_rewind( transaction->journal_ledger_list ) )
		return (TRANSACTION *)0;

	/* account_payable */
	/* --------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction_date_time,
			account_payable_account );

	journal_ledger->credit_amount = sum_debit_amount;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	transaction->transaction_amount = sum_debit_amount;

	return transaction;

} /* ledger_inventory_build_transaction() */

TRANSACTION *ledger_specific_inventory_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *specific_inventory_purchase_list,
				char *fund_name )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory;
	char *sales_tax_expense_account = {0};
	char *freight_in_expense_account = {0};
	char *account_payable_account = {0};
	double sum_debit_amount = 0.0;

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Exits if account_payable_account is not found. */
	/* ---------------------------------------------- */
	ledger_get_purchase_order_account_names(
				&sales_tax_expense_account,
				&freight_in_expense_account,
				&account_payable_account,
				application_name,
				fund_name );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->journal_ledger_list = list_new();

	/* SPECIFIC_INVENTORY_PURCHASE */
	/* --------------------------- */
	if ( list_rewind( specific_inventory_purchase_list ) )
	{
		do {
			purchase_specific_inventory =
				list_get_pointer(
					specific_inventory_purchase_list );

			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction_date_time,
					purchase_specific_inventory->
						inventory_account_name );

			journal_ledger->debit_amount =
				purchase_specific_inventory->
					capitalized_extension;

			list_append_pointer(
				transaction->journal_ledger_list,
				journal_ledger );

			sum_debit_amount +=
				purchase_specific_inventory->
					capitalized_extension;

		} while( list_next( specific_inventory_purchase_list ) );
	}

	/* account_payable */
	/* --------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction_date_time,
			account_payable_account );

	journal_ledger->credit_amount = sum_debit_amount;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	transaction->transaction_amount = sum_debit_amount;

	return transaction;

} /* ledger_specific_inventory_build_transaction() */

TRANSACTION *ledger_purchase_order_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				double sales_tax,
				double freight_in,
				LIST *supply_purchase_list,
				LIST *service_purchase_list,
				LIST *fixed_asset_purchase_list,
				LIST *prepaid_asset_purchase_list,
				char *fund_name )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	PURCHASE_SERVICE *purchase_service;
	char *sales_tax_expense_account = {0};
	char *freight_in_expense_account = {0};
	char *account_payable_account = {0};
	double sum_debit_amount = 0.0;

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Exits if account_payable_account is not found. */
	/* ---------------------------------------------- */
	ledger_get_purchase_order_account_names(
				&sales_tax_expense_account,
				&freight_in_expense_account,
				&account_payable_account,
				application_name,
				fund_name );

	if ( sales_tax && !sales_tax_expense_account )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: sales tax exists without sales tax expense account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( freight_in && !freight_in_expense_account )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: freight in exists without freight in expense account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->journal_ledger_list = list_new();

	/* SUPPLY_PURCHASE */
	/* --------------- */
	if ( list_length( supply_purchase_list ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			purchase_supply_distinct_account_extract(
				&sum_debit_amount,
				supply_purchase_list ) );
	}

	/* SERVICE_PURCHASE */
	/* ---------------- */
	if ( list_rewind( service_purchase_list ) )
	{
		do {
			purchase_service =
				list_get_pointer(
					service_purchase_list );

			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction_date_time,
					purchase_service->account_name );

			journal_ledger->debit_amount =
				purchase_service->extension;

			list_append_pointer(
				transaction->journal_ledger_list,
				journal_ledger );

			sum_debit_amount += purchase_service->extension;

		} while( list_next( service_purchase_list ) );
	}

	/* FIXED_ASSET_PURCHASE */
	/* -------------------- */
	if ( list_length( fixed_asset_purchase_list ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			purchase_fixed_asset_distinct_account_extract(
				&sum_debit_amount,
				fixed_asset_purchase_list ) );
	}

	/* PREPAID_ASSET_PURCHASE */
	/* ---------------------- */
	if ( list_length( prepaid_asset_purchase_list ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			purchase_prepaid_asset_distinct_account_extract(
				&sum_debit_amount,
				prepaid_asset_purchase_list ) );
	}

	/* sales_tax */
	/* --------- */
	if ( sales_tax )
	{
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction_date_time,
				sales_tax_expense_account );

		journal_ledger->debit_amount = sales_tax;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		sum_debit_amount += sales_tax;
	}


	/* freight_in */
	/* ---------- */
	if ( freight_in )
	{
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction_date_time,
				freight_in_expense_account );

		journal_ledger->debit_amount = freight_in;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		sum_debit_amount += freight_in;
	}

	/* account_payable */
	/* --------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction_date_time,
			account_payable_account );

	journal_ledger->credit_amount = sum_debit_amount;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	transaction->transaction_amount = sum_debit_amount;

	return transaction;

} /* ledger_purchase_order_build_transaction() */

TRANSACTION *ledger_customer_sale_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *inventory_sale_list,
				LIST *specific_inventory_sale_list,
				LIST *fixed_service_sale_list,
				LIST *hourly_service_sale_list,
				double shipping_revenue,
				double sales_tax,
				double invoice_amount,
				char *fund_name )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *sales_tax_payable_account = {0};
	char *shipping_revenue_account = {0};
	char *receivable_account = {0};
	double sales_revenue_amount = {0};
	double service_revenue_amount = {0};

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_get_customer_sale_account_names(
		&sales_tax_payable_account,
		&shipping_revenue_account,
		&receivable_account,
		application_name,
		fund_name );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->journal_ledger_list =
		ledger_get_credit_journal_ledger_list(
			&sales_revenue_amount,
			&service_revenue_amount,
			inventory_sale_list,
			specific_inventory_sale_list,
			fixed_service_sale_list,
			hourly_service_sale_list );

	/* shipping_revenue */
	/* ---------------- */
	if ( shipping_revenue )
	{
		if ( !shipping_revenue_account )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: shipping_revenue exists without shipping_revenue_account set.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction_date_time,
				shipping_revenue_account );

		journal_ledger->credit_amount = shipping_revenue;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		sales_revenue_amount += shipping_revenue;
	}

	/* sales_tax */
	/* --------- */
	if ( sales_tax )
	{
		if ( !sales_tax_payable_account )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: sales_tax exists without sales_tax_payable_account set.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction_date_time,
				sales_tax_payable_account );

		journal_ledger->credit_amount = sales_tax;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );
	}

	/* account_receivable */
	/* ------------------ */
	if ( invoice_amount )
	{
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction_date_time,
				receivable_account );

		journal_ledger->debit_amount = invoice_amount;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		transaction->transaction_amount = invoice_amount;
	}

	return transaction;

} /* ledger_customer_sale_build_transaction() */

LIST *ledger_get_credit_journal_ledger_list(
				double *sales_revenue_amount,
				double *service_revenue_amount,
				LIST *inventory_sale_list,
				LIST *specific_inventory_sale_list,
				LIST *fixed_service_sale_list,
				LIST *hourly_service_sale_list )
{
	LIST *journal_ledger_list;

	journal_ledger_list = list_new();

	/* INVENTORY_SALE */
	/* -------------- */
	if ( list_length( inventory_sale_list ) )
	{
		journal_ledger_list =
			customer_sale_inventory_distinct_account_extract(
				sales_revenue_amount,
				/* --------------------------- */
				/* Returns journal_ledger_list */
				/* --------------------------- */
				journal_ledger_list,
				inventory_sale_list );
	}

	/* SPECIFIC_INVENTORY_SALE */
	/* ----------------------- */
	if ( list_rewind( specific_inventory_sale_list ) )
	{
		SPECIFIC_INVENTORY_SALE *specific_inventory_sale;
		JOURNAL_LEDGER *journal_ledger;

		do {
			specific_inventory_sale =
				list_get_pointer(
					specific_inventory_sale_list );

			/* Debit cost_of_goods_sold */
			/* ------------------------ */
			journal_ledger =
				ledger_get_or_set_journal_ledger(
					journal_ledger_list,
					specific_inventory_sale->
					     cost_of_goods_sold_account_name );

			journal_ledger->debit_amount +=
				specific_inventory_sale->
					cost_of_goods_sold;

			/* Credit specific_inventory */
			/* ------------------------- */
			journal_ledger =
				ledger_get_or_set_journal_ledger(
					journal_ledger_list,
					specific_inventory_sale->
						credit_account_name );

			journal_ledger->credit_amount +=
				specific_inventory_sale->
					cost_of_goods_sold;

			*sales_revenue_amount +=
				specific_inventory_sale->
					extension;

		} while( list_next( specific_inventory_sale_list ) );
	}

	/* FIXED_SERVICE_SALE */
	/* ------------------ */
	if ( list_length( fixed_service_sale_list ) )
	{
		journal_ledger_list =
			customer_sale_fixed_service_distinct_account_extract(
				service_revenue_amount,
				/* --------------------------- */
				/* Returns journal_ledger_list */
				/* --------------------------- */
				journal_ledger_list,
				fixed_service_sale_list );
	}

	/* HOURLY_SERVICE_SALE */
	/* ------------------- */
	if ( list_length( hourly_service_sale_list ) )
	{
		journal_ledger_list =
			customer_sale_hourly_service_distinct_account_extract(
				service_revenue_amount,
				/* --------------------------- */
				/* Returns journal_ledger_list */
				/* --------------------------- */
				journal_ledger_list,
				hourly_service_sale_list );
	}

	return journal_ledger_list;

} /* ledger_get_credit_journal_ledger_list() */

void ledger_get_payroll_account_names(
				char **salary_wage_expense_account,
				char **payroll_tax_account,
				char **payroll_payable_account,
				char **federal_withholding_payable_account,
				char **state_withholding_payable_account,
				char **social_security_payable_account,
				char **medicare_payable_account,
				char **retirement_plan_payable_account,
				char **health_insurance_payable_account,
				char **union_dues_payable_account,
				char **federal_unemployment_tax_payable_account,
				char **state_unemployment_tax_payable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	if ( salary_wage_expense_account )
	{
		key = "salary_wage_expense_key";
		*salary_wage_expense_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

	if ( payroll_tax_account )
	{
		key = "payroll_tax_key";
		*payroll_tax_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

	if ( payroll_payable_account )
	{
		key = "payroll_payable_key";
		*payroll_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

	if ( federal_withholding_payable_account )
	{
		key = "federal_withholding_payable_key";
		*federal_withholding_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

	if ( state_withholding_payable_account )
	{
		key = "state_withholding_payable_key";
		*state_withholding_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				1 /* warning_only */,
			__FUNCTION__ );
	}

	if ( social_security_payable_account )
	{
		key = "social_security_payable_key";
		*social_security_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

	if ( medicare_payable_account )
	{
		key = "medicare_payable_key";
		*medicare_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

	if ( retirement_plan_payable_account )
	{
		key = "retirement_plan_payable_key";
		*retirement_plan_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				1 /* warning_only */,
			__FUNCTION__ );
	}

	if ( health_insurance_payable_account )
	{
		key = "health_insurance_payable_key";
		*health_insurance_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				1 /* warning_only */,
			__FUNCTION__ );
	}

	if ( union_dues_payable_account )
	{
		key = "union_dues_payable_key";
		*union_dues_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				1 /* warning_only */,
			__FUNCTION__ );
	}

	if ( federal_unemployment_tax_payable_account )
	{
		key = "federal_unemployment_tax_payable_key";
		*federal_unemployment_tax_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

	if ( state_unemployment_tax_payable_account )
	{
		key = "state_unemployment_tax_payable_key";
		*state_unemployment_tax_payable_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

} /* ledger_get_payroll_account_names() */

void ledger_get_investment_account_names(
				char **unrealized_investment,
				char **realized_gain,
				char **realized_loss,
				char **checking_account,
				char **contributed_capital_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = "unrealized_investment_key";
	*unrealized_investment =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = "realized_investment_gain_key";
	*realized_gain =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = "realized_investment_loss_key";
	*realized_loss =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = LEDGER_CASH_KEY;
	*checking_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = "contributed_capital_key";
	*contributed_capital_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

} /* ledger_get_investment_account_names() */

double ledger_output_html_subclassification_list(
					HTML_TABLE *html_table,
					LIST *subclassification_list,
					char *element_name,
					boolean element_accumulate_debit,
					double percent_denominator )
{
	double total_element = 0.0;
	double subclassification_amount;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	double latest_ledger_balance;
	boolean first_time = 1;
	double percent_of_total;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp(	element_name,
			LEDGER_EQUITY_ELEMENT ) ==  0 )
	{
		sprintf(	element_title,
				"<h2>%s</h2>",
				format_initial_capital(
					format_buffer,
					element_name ) );

		html_table_set_data(
				html_table->data_list,
				element_title );

		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
		html_table->data_list = list_new();

		/* Maybe financial_position, so display beginning balance. */
		/* ------------------------------------------------------- */
		total_element =
			ledger_output_net_assets_html_subclassification_list(
				html_table,
				subclassification_list,
				element_accumulate_debit );

		first_time = 0;
	}

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Skip net assets. */
		/* ---------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( first_time )
		{
			sprintf(	element_title,
					"<h2>%s</h2>",
					format_initial_capital(
						format_buffer,
						element_name ) );

			html_table_set_data(
					html_table->data_list,
					element_title );

			html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
			html_table->data_list = list_new();

			first_time = 0;
		}

		/* -------------------------------------------- */
		/* If more than one account,			*/
		/* then display the subclassification name.	*/
		/* -------------------------------------------- */
		if ( list_length( subclassification->account_list ) > 1 )
		{
			html_table->data_list = list_new();

			sprintf(element_title,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(	html_table->data_list,
						element_title );
	
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
			html_table->data_list = list_new();
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			/* ---------------------------------------------- */
			/* Don't display single Change in net assets row. */
			/* Display the total row below.			  */
			/* ---------------------------------------------- */
			if ( strcmp(	account->account_name,
					LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS )
					!= 0 )
			{
				html_table_set_data(
					html_table->data_list,
					strdup( format_initial_capital(
						buffer,
						account->account_name ) ) );
	
				html_table_set_data(
					html_table->data_list,
					strdup( place_commas_in_money(
						   latest_ledger_balance ) ) );

				if ( percent_denominator )
				{
					char buffer[ 128 ];

					percent_of_total =
						( latest_ledger_balance /
						  percent_denominator ) * 100.0;

					sprintf( buffer,
						 "%.1lf%c",
						 percent_of_total,
						 '%' );

					html_table_set_data(
						html_table->data_list,
						strdup( "" ) );

					html_table_set_data(
						html_table->data_list,
						strdup( "" ) );

					html_table_set_data(
						html_table->data_list,
						strdup( buffer ) );
				}

				html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
	
				list_free_string_list( html_table->data_list );
				html_table->data_list = list_new();
			}

			total_element += latest_ledger_balance;

			subclassification_amount += latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf( buffer,
				 "<h3>%s</h3>",
				 "Change in Net Assets" );
		}
		else
		{
			sprintf( buffer,
			 	"<h3>Total %s</h3>",
			 	format_initial_capital(
					format_buffer,
			 		subclassification->
					    subclassification_name ) );
		}

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );

		html_table_set_data( html_table->data_list, strdup( "" ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification_amount ) ) );
	
		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( subclassification_amount /
				  percent_denominator ) * 100.0;

			sprintf( buffer,
				 "%.1lf%c",
				 percent_of_total,
				 '%' );

			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
		}

		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		html_table->data_list = list_new();

	} while( list_next( subclassification_list ) );

	if ( !timlib_double_virtually_same( total_element, 0.0 ) )
	{
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(element_title,
				"<h2>%s</h2>",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(element_title,
				"<h2>Total %s</h2>",
				element_name );
		}

		html_table_set_data(	html_table->data_list,
					element_title );
	
		html_table_set_data( html_table->data_list, strdup( "" ) );
		html_table_set_data( html_table->data_list, strdup( "" ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				total_element ) ) );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( total_element /
				  percent_denominator ) * 100.0;

			sprintf( buffer,
				 "%.1lf%c",
				 percent_of_total,
				 '%' );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
		}

		html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
		html_table->data_list = list_new();
	}
	return total_element;

} /* ledger_output_html_subclassification_list() */

double ledger_output_net_assets_html_subclassification_list(
				HTML_TABLE *html_table,
				LIST *subclassification_list,
				boolean element_accumulate_debit )
{
	double subclassification_amount;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	double latest_ledger_balance;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only display net assets. */
		/* ------------------------ */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( list_length( subclassification->account_list ) > 1 )
		{
			html_table->data_list = list_new();

			sprintf(element_title,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(	html_table->data_list,
						element_title );
	
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
			html_table->data_list = list_new();
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_ledger_balance =
					account->latest_ledger->balance;
			}
			else
			{
				latest_ledger_balance =
					0.0 - account->latest_ledger->balance;
			}

			html_table_set_data(
				html_table->data_list,
				strdup( format_initial_capital(
					buffer,
					account->account_name ) ) );
	
			html_table_set_data(
				html_table->data_list,
				strdup( place_commas_in_money(
					   latest_ledger_balance ) ) );
	
			html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
	
			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();

			subclassification_amount +=
				latest_ledger_balance;

		} while( list_next( subclassification->account_list ) );

		sprintf( buffer,
			 "<h3>%s</h3>",
			 LEDGER_BEGINNING_BALANCE_LABEL );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );
		html_table_set_data( html_table->data_list, strdup( "" ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification_amount ) ) );
	
		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table->data_list = list_new();

		return subclassification_amount;

	} while( list_next( subclassification_list ) );

	return 0.0;

} /* ledger_output_net_assets_html_subclassification_list() */

double ledger_output_html_account_list(
					HTML_TABLE *html_table,
					LIST *account_list,
					char *element_name,
					boolean element_accumulate_debit,
					double percent_denominator )
{
	double total_element = 0.0;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	char element_title[ 128 ];
	double latest_ledger_balance;
	double percent_of_total;

	if ( !list_length( account_list ) ) return 0.0;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(	element_title,
			"<h2>%s</h2>",
			format_initial_capital(
				format_buffer,
				element_name ) );

	html_table_set_data(
			html_table->data_list,
			element_title );

	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	html_table->data_list = list_new();

	/* Maybe financial_position, so display beginning balance. */
	/* ------------------------------------------------------- */
/*
	total_element =
		ledger_output_net_assets_html_subclassification_list(
			html_table,
			subclassification_list,
			element_accumulate_debit );
*/

	list_rewind( account_list );
	do {
		account = list_get_pointer( account_list );

		if ( !account->latest_ledger
		||   !account->latest_ledger->balance )
			continue;

		if (	element_accumulate_debit ==
			account->accumulate_debit )
		{
			latest_ledger_balance =
				account->latest_ledger->balance;
		}
		else
		{
			latest_ledger_balance =
				0.0 - account->latest_ledger->balance;
		}

		html_table_set_data(
			html_table->data_list,
			strdup( format_initial_capital(
				buffer,
				account->account_name ) ) );
	
		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				   latest_ledger_balance ) ) );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( latest_ledger_balance /
				  percent_denominator ) * 100.0;

			sprintf( buffer,
				 "%.1lf%c",
				 percent_of_total,
				 '%' );

			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
		}

		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

		total_element += latest_ledger_balance;

	} while( list_next( account_list ) );

	if ( !timlib_double_virtually_same( total_element, 0.0 ) )
	{
		sprintf(element_title,
			"<h2>Total %s</h2>",
			element_name );

		html_table_set_data(	html_table->data_list,
					element_title );
	
		html_table_set_data( html_table->data_list, strdup( "" ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				total_element ) ) );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( total_element /
				  percent_denominator ) * 100.0;

			sprintf( buffer,
				 "%.1lf%c",
				 percent_of_total,
				 '%' );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
		}

		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table->data_list = list_new();
	}

	return total_element;

} /* ledger_output_html_account_list() */

double ledger_output_subclassification_aggregate_html_element(
					HTML_TABLE *html_table,
					LIST *subclassification_list,
					char *element_name,
					double percent_denominator )
{
	double total_element = 0.0;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	boolean first_time = 1;
	double percent_of_total;

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp(	element_name,
			LEDGER_EQUITY_ELEMENT ) ==  0 )
	{
		sprintf(	element_title,
				"<h2>%s</h2>",
				format_initial_capital(
					format_buffer,
					element_name ) );

		html_table_set_data(
				html_table->data_list,
				element_title );

		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
		html_table->data_list = list_new();

		first_time = 0;

		if ( !ledger_exists_net_assets_equity_subclassification(
					subclassification_list ) )
		{
			goto equity_all_done;
		}

		total_element =
			ledger_output_net_assets_html_subclassification_list(
				html_table,
				subclassification_list,
				0 /* not element_accumulate_debit */ );

		first_time = 0;
	}

equity_all_done:

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( timlib_dollar_virtually_same(
			subclassification->subclassification_total,
			0.0 ) )
		{
			continue;
		}

		/* Don't display net assets. */
		/* ------------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( first_time )
		{
			sprintf(	element_title,
					"<h2>%s</h2>",
					format_initial_capital(
						format_buffer,
						element_name ) );

			html_table_set_data(
					html_table->data_list,
					element_title );

			html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
			html_table->data_list = list_new();

			first_time = 0;
		}

		total_element += subclassification->subclassification_total;

		sprintf(buffer,
		 	"%s",
		 	format_initial_capital(
				format_buffer,
		 		subclassification->
				    subclassification_name ) );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification->
					subclassification_total ) ) );
	
		if ( percent_denominator )
		{
			char buffer[ 128 ];

			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			percent_of_total =
				( subclassification->
					subclassification_total /
				  percent_denominator ) * 100.0;

			sprintf( buffer,
				 "%.1lf%c",
				 percent_of_total,
				 '%' );

			html_table_set_data(
					html_table->data_list,
					strdup( buffer ) );
		}

		html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		html_table->data_list = list_new();

	} while( list_next( subclassification_list ) );

	if ( !timlib_double_virtually_same( total_element, 0.0 ) )
	{
		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(element_title,
				"<h2>%s</h2>",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(element_title,
				"<h2>Total %s</h2>",
				element_name );
		}

		html_table_set_data(	html_table->data_list,
					element_title );
	
		html_table_set_data( html_table->data_list, strdup( "" ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				total_element ) ) );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( total_element /
				  percent_denominator ) * 100.0;

			sprintf( buffer,
				 "%.1lf%c",
				 percent_of_total,
				 '%' );

			html_table_set_data(
					html_table->data_list,
					strdup( buffer ) );
		}

		html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );
		html_table->data_list = list_new();
	}

	return total_element;

} /* ledger_output_subclassification_aggregate_html_element() */

void ledger_output_net_income(		HTML_TABLE *html_table,
					double net_income,
					boolean is_statement_of_activities,
					double percent_denominator,
					int skip_columns )
{
	double percent_of_total;

	html_table->data_list = list_new();

	if ( is_statement_of_activities )
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Change in Net Assets</h2>" );
	}
	else
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Net Income</h2>" );
	}

	while( skip_columns-- )
	{
		html_table_set_data( html_table->data_list, strdup( "" ) );
	}

	html_table_set_data(	html_table->data_list,
				strdup( place_commas_in_money( net_income ) ) );

	if ( percent_denominator )
	{
		char buffer[ 128 ];

		percent_of_total =
			( net_income /
			  percent_denominator ) * 100.0;

		sprintf( buffer,
			 "%.1lf%c",
			 percent_of_total,
			 '%' );

		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );
	}

	html_table_output_data( html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
	html_table->data_list = list_new();

} /* ledger_output_net_income() */

void ledger_output_subclassification_aggregate_net_income(
					HTML_TABLE *html_table,
					double net_income,
					boolean is_statement_of_activities,
					double percent_denominator )
{
	double percent_of_total;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	html_table->data_list = list_new();

	if ( is_statement_of_activities )
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Change in Net Assets</h2>" );
	}
	else
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Net Income</h2>" );
	}
	html_table_set_data( html_table->data_list, strdup( "" ) );

	html_table_set_data(	html_table->data_list,
				strdup( place_commas_in_money( net_income ) ) );

	if ( percent_denominator )
	{
		char buffer[ 128 ];

		percent_of_total =
			( net_income /
			  percent_denominator ) * 100.0;

		sprintf( buffer,
			 "%.1lf%c",
			 percent_of_total,
			 '%' );

		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );
	}

	html_table_output_data( html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

	html_table->data_list = list_new();

} /* ledger_output_subclassification_aggregate_net_income() */

LIST *ledger_get_subclassification_omit_latex_row_list(
					double *total_element,
					LIST *account_list,
					char *element_name,
					boolean element_accumulate_debit,
					double percent_denominator )
{
	LIST *row_list;
	ACCOUNT *account;
	char total_element_label[ 128 ];
	char format_buffer[ 128 ];
	double latest_ledger_balance;
	LATEX_ROW *latex_row;
	double percent_of_total;

	*total_element = 0.0;

	if ( !list_rewind( account_list ) ) return (LIST *)0;

	row_list = list_new();

	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	sprintf( format_buffer,
		 "\\large \\bf %s",
		 element_name );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( format_initial_capital(
				format_buffer,
				format_buffer ) ),
		0 /* not large_bold */ );

	sprintf(format_buffer,
	 	"\\large \\bf Total %s",
	 	element_name );

	format_initial_capital( total_element_label, format_buffer );

	do {
		account =
			list_get_pointer( account_list ); 

		if ( !account->latest_ledger
		||   !account->latest_ledger->balance )
			continue;

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		if (	element_accumulate_debit ==
				account->accumulate_debit )
		{
			latest_ledger_balance =
				account->latest_ledger->balance;
		}
		else
		{
			latest_ledger_balance =
				0.0 - account->latest_ledger->balance;
		}

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					account->
					    account_name ) ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
			   	     latest_ledger_balance ) ),
			0 /* not large_bold */ );

		/* Blank space for the element column. */
		/* ----------------------------------- */
		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( latest_ledger_balance /
		  		  percent_denominator ) * 100.0;

			sprintf( buffer,
		 		"%.1lf%c",
		 		percent_of_total,
		 		'%' );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}
	
		*total_element += latest_ledger_balance;

	} while( list_next( account_list ) );

	if ( *total_element )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( total_element_label ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   *total_element ) ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( *total_element /
		  		percent_denominator ) * 100.0;

			sprintf( buffer,
		 		"%.1lf%c",
		 		percent_of_total,
		 		'%' );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}

		/* Blank line */
		/* ---------- */
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

	} /* if total_element */

	return row_list;

} /* ledger_get_subclassification_omit_latex_row_list() */

boolean ledger_exists_net_assets_equity_subclassification(
					LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;

	if ( !list_rewind( subclassification_list ) ) return 0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( strcmp(
			subclassification->
				subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			return 1;
		}
	} while( list_next( subclassification_list ) );

	return 0;

} /* ledger_exists_net_assets_equity_subclassification() */

boolean ledger_exists_journal_ledger(
				LIST *journal_ledger_list,
				char *full_name,
				char *street_address,
				char *transaction_date,
				double transaction_amount )
{
	JOURNAL_LEDGER *journal_ledger;
	double abs_transaction_amount;

	if ( !list_rewind( journal_ledger_list ) ) return 0;

	if ( timlib_dollar_virtually_same( transaction_amount, 0.0 ) )
		return 0;

	abs_transaction_amount = float_abs( transaction_amount );

	do {
		journal_ledger = list_get( journal_ledger_list );

		if ( timlib_strcmp(
			journal_ledger->full_name,
			full_name ) != 0
		||   timlib_strcmp(
			journal_ledger->street_address,
			street_address ) != 0
		||   timlib_strncmp(
			journal_ledger->transaction_date_time,
			transaction_date ) != 0 )
		{
			continue;
		}

		if ( timlib_dollar_virtually_same(
			abs_transaction_amount,
			journal_ledger->debit_amount )
		||   timlib_dollar_virtually_same(
			abs_transaction_amount,
			journal_ledger->credit_amount ) )
		{
			return 1;
		}

	} while( list_next( journal_ledger_list ) );

	return 0;

} /* ledger_exists_journal_ledger() */

/* Returns transaction_date_time */
/* ----------------------------- */
char *ledger_transaction_date_time_update(
			/* ------------------------------------------ */
			/* Sets journal_ledger->transaction_date_time */
			/* ------------------------------------------ */
			LIST *journal_ledger_list,
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *preupdate_transaction_date_time )
{
	JOURNAL_LEDGER *journal_ledger;
	FILE *update_pipe;

	transaction_date_time =
		ledger_fetch_unique_transaction_date_time(
			application_name,
			transaction_date_time );

	ledger_transaction_generic_update(
		application_name,
		full_name,
		street_address,
		preupdate_transaction_date_time,
		"transaction_date_time" /* attribute */,
		transaction_date_time /* data */ );

	if ( !list_rewind( journal_ledger_list ) )
		return transaction_date_time;

	update_pipe = ledger_open_update_pipe( application_name );

	do {
		journal_ledger = list_get_pointer( journal_ledger_list );

		journal_ledger->transaction_date_time =
			transaction_date_time;

		ledger_journal_ledger_transaction_date_time_update(
			update_pipe,
			full_name,
			street_address,
			preupdate_transaction_date_time,
			journal_ledger->account_name,
			journal_ledger->transaction_date_time );

	} while( list_next( journal_ledger_list ) );

	pclose( update_pipe );

	ledger_propagate_journal_ledger_list(
			application_name,
			transaction_date_time,
			journal_ledger_list );

	return transaction_date_time;

} /* ledger_transaction_date_time_update() */

DICTIONARY *ledger_account_pipe2dictionary( char *sys_string, char delimiter )
{
	char buffer[ 65536 ];
	char data[ 65536 ];
	char key[ 1024 ];
	FILE *p = popen( sys_string, "r" );
	DICTIONARY *d = dictionary_small_new();

	*data = '\0';

	while( get_line( buffer, p ) )
	{
		if ( delimiter )
		{
			piece( key, delimiter, buffer, 0 );

			if ( !piece( data, delimiter, buffer, 1 ) )
				*data = '\0';
		}
		else
		{
			strcpy( key, buffer );
		}

		if ( !timlib_character_exists( key, ',' ) )
		{
			dictionary_set_pointer(
				d,
				strdup( key ),
				strdup( data ) );
		}
		else
		{
			char key_piece[ 1024 ];
			int i;

			for(	i = 0;
				piece( key_piece, ',', key, i ); 
				i++ )
			{
				dictionary_set_pointer(
					d,
					strdup( key_piece ),
					strdup( data ) );
			}
		}
	}

	pclose( p );
	return d;

} /* ledger_account_pipe2dictionary() */

LIST *ledger_build_binary_ledger_list(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *debit_account,
				char *credit_account )
{
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

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

	journal_ledger_list = list_new();

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction_date_time,
			debit_account );

	journal_ledger->debit_amount = transaction_amount;

	list_append_pointer( journal_ledger_list, journal_ledger );

	journal_ledger =
		journal_ledger_new(
			full_name,
			street_address,
			transaction_date_time,
			credit_account );

	journal_ledger->credit_amount = transaction_amount;

	list_append_pointer( journal_ledger_list, journal_ledger );

	return journal_ledger_list;

} /* ledger_build_binary_ledger_list() */

LIST *ledger_get_binary_ledger_list(
				double transaction_amount,
				char *debit_account,
				char *credit_account )
{
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

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

	journal_ledger_list = list_new();

	journal_ledger =
		journal_ledger_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			debit_account );

	journal_ledger->debit_amount = transaction_amount;

	list_append_pointer(
		journal_ledger_list,
		journal_ledger );

	journal_ledger =
		journal_ledger_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			credit_account );

	journal_ledger->credit_amount = transaction_amount;

	list_append_pointer(
		journal_ledger_list,
		journal_ledger );

	return journal_ledger_list;

} /* ledger_get_binary_ledger_list() */

LIST *ledger_consolidate_journal_ledger_list(
			char *application_name,
			LIST *journal_ledger_list )
{
	LIST *account_name_list;
	JOURNAL_LEDGER *journal_ledger;
	JOURNAL_LEDGER *new_journal_ledger;
	LIST *new_journal_ledger_list;
	char *account_name;

	if ( !list_length( journal_ledger_list ) ) return (LIST *)0;

	account_name_list =
		ledger_get_unique_account_name_list(
			journal_ledger_list );

	if ( list_length( journal_ledger_list ) ==
	     list_length( account_name_list ) )
	{
		return journal_ledger_list;
	}

	new_journal_ledger_list = list_new();

	list_rewind( account_name_list );

	do {
		account_name = list_get( account_name_list );

		list_rewind( journal_ledger_list );

		do {
			journal_ledger = list_get( journal_ledger_list );

			if ( strcmp( journal_ledger->account_name,
				     account_name ) != 0 )
			{
				continue;
			}

			new_journal_ledger =
				ledger_get_or_set_journal_ledger(
					new_journal_ledger_list,
					account_name );

			if ( ledger_account_get_accumulate_debit(
				application_name, account_name ) )
			{
				if ( journal_ledger->debit_amount )
				{
					new_journal_ledger->debit_amount +=
						journal_ledger->debit_amount;
				}
				else
				{
					new_journal_ledger->debit_amount -=
						journal_ledger->credit_amount;
				}
			}
			else
			{
				if ( journal_ledger->credit_amount )
				{
					new_journal_ledger->credit_amount +=
						journal_ledger->credit_amount;
				}
				else
				{
					new_journal_ledger->credit_amount -=
						journal_ledger->debit_amount;
				}
			}

		} while( list_next( journal_ledger_list ) );

	} while( list_next( account_name_list ) );

	return new_journal_ledger_list;

} /* ledger_consolidate_journal_ledger_list() */

LIST *ledger_get_unique_account_name_list(
			LIST *journal_ledger_list )
{
	LIST *account_name_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) ) return (LIST *)0;

	account_name_list = list_new();

	do {
		journal_ledger =
			list_get_pointer( 
				journal_ledger_list );

		list_append_unique_string(
			account_name_list,
			journal_ledger->account_name );

	} while( list_next( journal_ledger_list ) );

	return account_name_list;

} /* ledger_get_unique_account_name_list() */

double ledger_debit_credit_get_amount(
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

} /* ledger_debit_credit_get_amount() */

double ledger_get_amount(	JOURNAL_LEDGER *ledger,
				boolean accumulate_debit )
{
	if ( !ledger ) return 0.0;

	return ledger_debit_credit_get_amount(
				ledger->debit_amount,
				ledger->credit_amount,
				accumulate_debit );

} /* ledger_get_amount() */

char *ledger_earlier_of_two_date( char *date1, char *date2 )
{
	char *return_date;

	if ( timlib_strcmp( date1, date2 ) < 0 )
		return_date = date1;
	else
		return_date = date2;

	return return_date;

} /* ledger_earlier_of_two_date() */

boolean ledger_property_street_address_attribute_exists(
				char *application_name )
{
	static int results = -1;

	if ( results == -1 )
	{
		results = attribute_exists(
				application_name,
				"purchase_order",
				"property_street_address" );
	}

	return (boolean)results;

} /* ledger_property_street_address_attribute_exists() */

boolean ledger_tax_recovery_period_attribute_exists(
				char *application_name )
{
	static int results = -1;

	if ( results == -1 )
	{
		results = attribute_exists(
				application_name,
				"fixed_asset_purchase",
				"tax_recovery_period" );
	}

	return (boolean)results;

} /* ledger_property_street_address_attribute_exists() */

void ledger_transaction_delete_propagate(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *journal_ledger_list )
{
	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_journal_ledger_list_propagate(
			journal_ledger_list,
			application_name );

} /* ledger_transaction_delete_propagate() */

TRANSACTION *ledger_binary_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double transaction_amount,
			char *memo )
{
	return ledger_build_binary_transaction(
			full_name,
			street_address,
			transaction_date_time,
			debit_account,
			credit_account,
			transaction_amount,
			memo );
}

TRANSACTION *ledger_build_binary_transaction(
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
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->transaction_amount = transaction_amount;

	transaction->journal_ledger_list =
		ledger_build_binary_ledger_list(
				full_name,
				street_address,
				transaction_date_time,
				transaction->transaction_amount,
				debit_account,
				credit_account );

	return transaction;

} /* ledger_build_binary_transaction() */

void ledger_transaction_output_pipe_display(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *journal_ledger_list )
{
	JOURNAL_LEDGER *journal_ledger;
	char buffer[ 256 ];
	char full_name_buffer[ 256 ];

	if ( list_length( journal_ledger_list ) != 2 )
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

	list_rewind( journal_ledger_list );

	journal_ledger = list_get_pointer( journal_ledger_list );

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
			journal_ledger->account_name ),
		 journal_ledger->debit_amount );

	list_next( journal_ledger_list );
	journal_ledger = list_get_pointer( journal_ledger_list );

	fprintf( output_pipe,
		 "%s^%s^^%.2lf\n",
		 transaction_date_time,
		 format_initial_capital(
			buffer,
			journal_ledger->account_name ),
		 journal_ledger->credit_amount );

} /* ledger_transaction_output_pipe_display() */

char *ledger_get_minimum_transaction_date(
					char *application_name )
{
	return ledger_beginning_transaction_date(
				application_name,
				(char *)0 /* fund_name */,
				(char *)0 /* ending_transaction_date */ );

}

char *ledger_get_non_cash_account_name(
				char *application_name,
				TRANSACTION *transaction )
{
	static char *checking_account = {0};
	JOURNAL_LEDGER *journal_ledger;

	if ( !transaction ) return "";

	if ( !checking_account )
	{
		checking_account =
			ledger_get_hard_coded_account_name(
				application_name,
				(char *)0 /* fund_name */,
				LEDGER_CASH_KEY,
				0 /* not warning_only */,
			__FUNCTION__ );
	}

	if ( !list_rewind( transaction->journal_ledger_list ) )
		return LEDGER_NOT_SET_ACCOUNT;

	do {
		journal_ledger =
			list_get_pointer(
				transaction->journal_ledger_list );

		if ( timlib_strcmp(	journal_ledger->account_name,
					checking_account ) != 0 )
		{
			return ledger_get_account_name(
					journal_ledger->account_name );
		}
	} while( list_next( transaction->journal_ledger_list ) );

	return LEDGER_NOT_SET_ACCOUNT;

} /* ledger_get_non_cash_account_name() */

char *ledger_get_account_name( char *account_name )
{
	if ( !account_name )
		return LEDGER_NOT_SET_ACCOUNT;
	else
		return account_name;

} /* ledger_get_account_name() */

double ledger_get_sales_tax(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *results;
	char *select = "sales_tax";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=customer_sale	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0.0;

	return atof( results );

} /* ledger_get_sales_tax() */

double ledger_get_total_payment(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	char *table_name;
	char sys_string[ 1024 ];
	char *where;
	char *results_string;

	table_name = get_table_name( application_name, "customer_payment" );

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "echo \"	select sum( payment_amount )	 "
		 "		from %s				 "
		 "		where %s;\"			|"
		 "sql.e						 ",
		 table_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );

} /* ledger_get_total_payment() */

TRANSACTION *ledger_check_number_seek_transaction(
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

} /* ledger_check_number_seek_transaction() */

JOURNAL_LEDGER *ledger_account_seek_journal_ledger(
					LIST *journal_ledger_list,
					char *account_name )
{
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( journal_ledger_list ) ) return (JOURNAL_LEDGER *)0;

	do {
		journal_ledger = list_get( journal_ledger_list );

		if ( strcmp(	journal_ledger->account_name,
				account_name ) == 0  )
		{
			return journal_ledger;
		}

	} while ( list_next( journal_ledger_list ) );

	return (JOURNAL_LEDGER *)0;

} /* ledger_account_seek_journal_ledger() */

JOURNAL_LEDGER *ledger_seek_uncleared_journal_ledger(
					char *application_name,
					char *fund_name,
					LIST *journal_ledger_list )
{
	JOURNAL_LEDGER *journal_ledger;
	static char *account_name = {0};

	if ( !account_name )
	{
		char *key;

		key = LEDGER_UNCLEARED_CHECKS_KEY;

		account_name =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				key,
				0 /* not warning_only */,
				__FUNCTION__ );
	}

	if ( !list_rewind( journal_ledger_list ) ) return (JOURNAL_LEDGER *)0;

	do {
		journal_ledger = list_get( journal_ledger_list );

		if ( strcmp(	journal_ledger->account_name,
				account_name ) == 0  )
		{
			return journal_ledger;
		}

	} while ( list_next( journal_ledger_list ) );

	return (JOURNAL_LEDGER *)0;

} /* ledger_seek_uncleared_journal_ledger() */

boolean ledger_exists_closing_entry(
				char *application_name,
				char *as_of_date )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *results;
	static boolean exists_closing_entry = -1;

	if ( exists_closing_entry != -1 )
		return exists_closing_entry;

	sprintf( where,
		 "transaction_date_time = '%s %s' and	"
		 "memo = '%s'				",
		 as_of_date,
		 LEDGER_CLOSING_TRANSACTION_TIME,
		 LEDGER_CLOSING_ENTRY_MEMO );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=transaction	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( atoi( results ) )
		exists_closing_entry = 1;
	else
		exists_closing_entry = 0;

	return exists_closing_entry;

} /* ledger_exists_closing_entry() */

