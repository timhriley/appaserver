/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale_return.c		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "date.h"
#include "piece.h"
#include "list.h"
#include "transaction.h"
#include "entity.h"
#include "inventory_sale_return.h"

#ifdef NOT_DEFINED
/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale_return.c		*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks inventory_sale_return ADT.		*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "piece.h"
#include "appaserver_library.h"
#include "inventory_sale_return.h"
#include "customer.h"
#include "entity.h"

INVENTORY_SALE_RETURN *inventory_sale_return_new( void )
{
	INVENTORY_SALE_RETURN *h;

	h = (INVENTORY_SALE_RETURN *)
		calloc(
			1,
			sizeof( INVENTORY_SALE_RETURN ) );

	if ( !h )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return h;

} /* inventory_sale_return_new() */

INVENTORY_SALE_RETURN *inventory_sale_return_list_seek(
				LIST *inventory_sale_return_list,
				char *return_date_time )
{
	INVENTORY_SALE_RETURN *i;

	if ( !list_rewind( inventory_sale_return_list ) )
		return (INVENTORY_SALE_RETURN *)0;

	do {
		i = list_get( inventory_sale_return_list );

		if ( strcmp( i->return_date_time, return_date_time ) == 0 )
		{
			return i;
		}

	} while( list_next( inventory_sale_return_list ) );

	return (INVENTORY_SALE_RETURN *)0;

} /* inventory_sale_list_return_seek() */

char *inventory_sale_return_get_select( void )
{
	char *select;

	select =
	"return_date_time,returned_quantity,transaction_date_time";

	return select;
}

LIST *inventory_sale_return_fetch_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *inventory_name )
{
	return inventory_sale_fetch_return_list(
					application_name,
					full_name,
					street_address,
					sale_date_time,
					inventory_name );
}

LIST *inventory_sale_fetch_return_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *sale_date_time,
					char *inventory_name )
{
	char *select;
	char *folder;
	char where[ 1024 ];
	char sys_string[ 2048 ];
	char *ledger_where;
	char *inventory_where;
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	INVENTORY_SALE_RETURN *inventory_sale_return;
	LIST *return_list;

	select = inventory_sale_return_get_select();

	folder = "inventory_sale_return";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	inventory_where = inventory_get_where( inventory_name );

	sprintf( where, "%s and %s", ledger_where, inventory_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	return_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		inventory_sale_return =
			inventory_sale_return_parse(
				application_name,
				full_name,
				street_address,
				input_buffer );

		list_append_pointer(	return_list,
					inventory_sale_return );
	}

	pclose( input_pipe );

	return return_list;

} /* inventory_sale_fetch_return_list() */

INVENTORY_SALE_RETURN *inventory_sale_return_parse(
				char *application_name,
				char *full_name,
				char *street_address,
				char *input_buffer )
{
	INVENTORY_SALE_RETURN *p;
	char piece_buffer[ 256 ];

	p = inventory_sale_return_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	p->return_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	p->returned_quantity = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *piece_buffer )
	{
		p->transaction_date_time =
		p->database_transaction_date_time =
			strdup( piece_buffer );

		p->transaction =
			ledger_transaction_with_load_new(
					application_name,
					full_name,
					street_address,
					p->transaction_date_time );
	}

	return p;

} /* inventory_sale_return_parse() */

void inventory_fetch_inventory_sale_return_account_names(
				char **account_receivable_account,
				char **account_payable_account,
				char *application_name,
				char *fund_name )
{
	char *key;

	key = LEDGER_ACCOUNT_RECEIVABLE_KEY;
	*account_receivable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = LEDGER_ACCOUNT_PAYABLE_KEY;
	*account_payable_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			key,
			0 /* not warning_only */,
			__FUNCTION__ );

} /* inventory_fetch_inventory_sale_return_account_names() */

TRANSACTION *inventory_sale_return_build_transaction(
					char **transaction_date_time,
					char *application_name,
					char *fund_name,
					char *full_name,
					char *street_address,
					double retail_price,
					double discount_amount,
					char *inventory_account_name,
					char *return_date_time,
					int returned_quantity,
					int sold_quantity,
					double sum_customer_payment_amount )
{
	TRANSACTION *transaction;
	char *account_receivable_account = {0};
	char *account_payable_account = {0};

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Exits if any ACCOUNT.hard_coded_account_key is missing. */
	/* ------------------------------------------------------- */
	inventory_fetch_inventory_sale_return_account_names(
			&account_receivable_account,
			&account_payable_account,
			application_name,
			fund_name );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			return_date_time /* transaction_date */,
			(char *)0 /* memo  */ );

	*transaction_date_time = transaction->transaction_date_time;

	transaction->journal_ledger_list =
		inventory_sale_return_get_journal_ledger_list(
			&transaction->transaction_amount,
			full_name,
			street_address,
			transaction->transaction_date_time,
			retail_price,
			discount_amount,
			inventory_account_name,
			account_receivable_account,
			account_payable_account,
			returned_quantity,
			sold_quantity,
			sum_customer_payment_amount );

	return transaction;
}

LIST *inventory_sale_return_get_journal_ledger_list(
				double *transaction_amount,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double retail_price,
				double discount_amount,
				char *inventory_account_name,
				char *account_receivable_account,
				char *account_payable_account,
				int returned_quantity,
				int sold_quantity,
				double sum_customer_payment_amount )
{
	JOURNAL_LEDGER *journal_ledger;
	LIST *journal_ledger_list;
	double inventory_amount;
	char *credit_account_name;
	double discount_amount_per_item;

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	journal_ledger_list = list_new();

	discount_amount_per_item =
		discount_amount / (double)sold_quantity;

	inventory_amount =	( retail_price - discount_amount_per_item ) *
				(double)returned_quantity;

	*transaction_amount = inventory_amount;

	/* Credit account */
	/* -------------- */
	if ( !timlib_dollar_virtually_same(
			sum_customer_payment_amount,
			0.0 ) )
	{
		credit_account_name = account_receivable_account;
	}
	else
	{
		credit_account_name = account_payable_account;
	}

	journal_ledger = journal_ledger_new(
				full_name,
				street_address,
				transaction_date_time,
				credit_account_name );

	journal_ledger->credit_amount = *transaction_amount;

	list_append_pointer( journal_ledger_list, journal_ledger );

	/* Debit account */
	/* ------------- */
	journal_ledger = journal_ledger_new(
				full_name,
				street_address,
				transaction_date_time,
				inventory_account_name );

	journal_ledger->debit_amount = inventory_amount;

	list_append_pointer( journal_ledger_list, journal_ledger );

	return journal_ledger_list;

} /* inventory_sale_return_get_journal_ledger_list() */

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *inventory_sale_return_journal_ledger_refresh(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					LIST *journal_ledger_list )
{
	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	transaction_date_time =
		ledger_transaction_journal_ledger_insert(
				application_name,
				full_name,
				street_address,
				transaction_date_time,
				transaction_amount,
				(char *)0 /* memo */,
				0 /* check_number */,
				1 /* lock_transaction */,
				journal_ledger_list );

	return transaction_date_time;

} /* inventory_sale_return_journal_ledger_refresh() */

FILE *inventory_sale_return_get_update_pipe(
				char *application_name )
{
	FILE *update_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *key_column_list;

	table_name =
		get_table_name(
			application_name,
			"inventory_sale_return" );

	key_column_list =
"full_name,street_address,sale_date_time,inventory_name,return_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list );

	update_pipe = popen( sys_string, "w" );

	return update_pipe;

} /* inventory_return_sale_get_update_pipe() */

void inventory_sale_return_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *return_date_time,
				char *transaction_date_time,
				char *database_transaction_date_time )
{
	FILE *update_pipe;

	update_pipe =
		inventory_sale_return_get_update_pipe(
			application_name );

	inventory_sale_return_pipe_update(
			update_pipe,
			full_name,
			street_address,
			sale_date_time,
			inventory_name,
			return_date_time,
			transaction_date_time,
			database_transaction_date_time );

	pclose( update_pipe );

} /* inventory_sale_return_update() */

void inventory_sale_return_pipe_update(
				FILE *update_pipe,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *return_date_time,
				char *transaction_date_time,
				char *database_transaction_date_time )
{
	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) != 0 )
	{
		fprintf(update_pipe,
			"%s^%s^%s^%s^%s^transaction_date_time^%s\n",
	 		full_name,
	 		street_address,
	 		sale_date_time,
	 		inventory_name,
			return_date_time,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

} /* inventory_sale_return_pipe_update() */

boolean inventory_sale_return_list_delete(
			LIST *inventory_sale_return_list,
			char *return_date_time )
{
	if ( !inventory_sale_return_list_seek(
		inventory_sale_return_list,
		return_date_time ) )
	{
		return 0;
	}

	list_delete_current( inventory_sale_return_list );
	return 1;

} /* inventory_sale_return_list_delete() */
#endif
