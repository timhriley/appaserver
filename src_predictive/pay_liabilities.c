/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/pay_liabilities.c			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver pay_liabilities ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "piece.h"
#include "date.h"
#include "pay_liabilities.h"

PAY_LIABILITIES *pay_liabilities_calloc( void )
{
	PAY_LIABILITIES *p;

	if ( ! ( p = (PAY_LIABILITIES *)
			calloc( 1, sizeof( PAY_LIABILITIES ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;

} /* pay_liabilities_calloc() */

PAY_LIABILITIES *pay_liabilities_new(
				char *application_name,
				char *fund_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				double dialog_box_payment_amount,
				char *transaction_memo )
{
	PAY_LIABILITIES *p;
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_name = {0};

	p = pay_liabilities_calloc();

	/* Input */
	/* ===== */
	p->input.starting_check_number = starting_check_number;
	p->input.dialog_box_payment_amount = dialog_box_payment_amount;

	p->input.loss_account_name =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_LOSS_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	ledger_get_vendor_payment_account_names(
				&checking_account,
				&uncleared_checks_account,
				&account_payable_name,
				application_name,
				fund_name );

	if ( starting_check_number )
		p->input.credit_account_name = uncleared_checks_account;
	else
		p->input.credit_account_name = checking_account;

	p->input.liability_account_entity_list =
		pay_liabilities_fetch_liability_account_entity_list(
				application_name );

	/* --------------------------------------- */
	/* Get sum_balance for the posted entities */
	/* from the displayed dialog box.	   */
	/* --------------------------------------- */
	p->input.entity_list =
		pay_liabilities_input_get_entity_list(
			application_name,
			fund_name,
			full_name_list,
			street_address_list );

	/* -------------------------------------------------------- */
	/* Fetch all the current liability accounts and all the     */
	/* journal ledger rows following the last zero balance row. */
	/* -------------------------------------------------------- */
	p->input.current_liability_account_list =
		pay_liabilities_fetch_current_liability_account_list(
			application_name,
			fund_name,
			(LIST *)0 /* exclude_account_name_list */ );

	p->input.purchase_order_list =
		purchase_get_amount_due_purchase_order_list(
			application_name );

	/* Process */
	/* ======= */

	/* Rearrange the account list into an entity list. */
	/* ----------------------------------------------- */
	p->process.current_liability_entity_list =
		pay_liabilities_get_current_liability_entity_list(
			p->input.current_liability_account_list,
			p->input.liability_account_entity_list
				/* exclude_entity_list */ );

	/* Process the LIABILITY_ACCOUNT_ENTITY records. */
	/* --------------------------------------------- */
	p->process.liability_account_entity_list =
		pay_liabilities_get_liability_account_entity_list(
			p->input.liability_account_entity_list,
			p->input.entity_list,
			p->input.dialog_box_payment_amount,
			starting_check_number );

	if ( starting_check_number )
	{
		starting_check_number +=
			list_length( 
				p->process.liability_account_entity_list );
	}

	/* Process the regular records. */
	/* ---------------------------- */
	p->process.entity_list =
		pay_liabilities_process_get_entity_list(
			p->input.entity_list,
			p->process.current_liability_entity_list,
			p->input.purchase_order_list,
			p->input.dialog_box_payment_amount,
			starting_check_number );

	/* Output */
	/* ------ */
	p->output.transaction_list =
		pay_liabilities_output_get_liability_account_transaction_list(
			p->process.liability_account_entity_list,
			p->input.credit_account_name,
			p->input.loss_account_name,
			transaction_memo );

	list_append_list(
		p->output.transaction_list,
		pay_liabilities_output_get_entity_transaction_list(
			p->process.entity_list,
			p->input.credit_account_name,
			p->input.loss_account_name,
			transaction_memo,
			list_length( p->output.transaction_list )
				/* seconds_to_add */ ) );

	p->output.vendor_payment_list =
		pay_liabilities_output_get_vendor_payment_list(
			p->process.entity_list,
			p->output.transaction_list );

	pay_liabilities_set_lock_transaction(
		p->output.transaction_list,
		p->output.vendor_payment_list );

	return p;

} /* pay_liabilities_new() */

LIST *pay_liabilities_output_get_liability_account_transaction_list(
				LIST *liability_account_entity_list,
				char *credit_account_name,
				char *loss_account_name,
				char *memo )
{
	LIST *transaction_list;
	TRANSACTION *transaction;
	ENTITY *entity;
	ACCOUNT *account;
	JOURNAL_LEDGER *journal_ledger;
	DATE *transaction_date_time;
	char *transaction_date_time_string;

	transaction_list = list_new();

	if ( !list_rewind( liability_account_entity_list ) )
		return transaction_list;

	transaction_date_time = date_now_new( date_get_utc_offset() );

	if ( !memo || !*memo || strcmp( memo, "memo" ) == 0 )
		memo = PAY_LIABILITIES_MEMO;

	do {
		entity =
			list_get_pointer( 
				liability_account_entity_list );


		if ( !list_rewind( entity->liability_account_list ) )
		{
			fprintf( stderr,
	"Warning in %s/%s()/%d: empty liability_account_list for (%s/%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 entity->full_name,
				 entity->street_address );
			continue;
		}

		transaction_date_time_string =
			date_display_yyyy_mm_dd_colon_hms(
				transaction_date_time );

		transaction =
			ledger_transaction_new(
				entity->full_name,
				entity->street_address,
				transaction_date_time_string,
				memo );

		transaction->transaction_amount = entity->payment_amount;
		transaction->check_number = entity->check_number;

		list_append_pointer( transaction_list, transaction );

		if ( !transaction->journal_ledger_list )
			transaction->journal_ledger_list =
				list_new();

		/* Debit accounts */
		/* -------------- */
		do {
			account =
				list_get_pointer( 
					entity->liability_account_list );

			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					account->account_name );

			journal_ledger->debit_amount = account->payment_amount;

			list_append_pointer(
				transaction->journal_ledger_list,
				journal_ledger );

		} while( list_next( entity->liability_account_list ) );

		/* Loss account */
		/* ------------ */
		if ( !timlib_dollar_virtually_same(
			entity->loss_amount, 0.0 ) )
		{
			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					loss_account_name );
	
			journal_ledger->debit_amount = entity->loss_amount;
	
			list_append_pointer(
				transaction->journal_ledger_list,
				journal_ledger );
		}

		/* Credit account */
		/* -------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				credit_account_name );

		journal_ledger->credit_amount = entity->payment_amount;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		date_increment_seconds(
			transaction_date_time,
			1 );

	} while( list_next( liability_account_entity_list ) );

	return transaction_list;

} /* pay_liabilities_output_get_liability_account_transaction_list() */

LIST *pay_liabilities_output_get_entity_transaction_list(
				LIST *process_entity_list,
				char *credit_account_name,
				char *loss_account_name,
				char *memo,
				int seconds_to_add )
{
	LIST *transaction_list;
	TRANSACTION *transaction;
	ENTITY *entity;
	ACCOUNT *account;
	JOURNAL_LEDGER *journal_ledger;
	DATE *transaction_date_time;
	char *transaction_date_time_string;

	transaction_list = list_new();

	if ( !list_rewind( process_entity_list ) )
		return transaction_list;

	transaction_date_time = date_now_new( date_get_utc_offset() );

	date_increment_seconds(
		transaction_date_time,
		seconds_to_add );

	if ( !memo || !*memo || strcmp( memo, "memo" ) == 0 )
		memo = PAY_LIABILITIES_MEMO;

	do {
		entity =
			list_get_pointer( 
				process_entity_list );

		if ( !list_rewind(
			entity->liability_account_list ) )
		{
			fprintf( stderr,
"Warning in %s/%s()/%d: empty liability_account_list for (%s/%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 entity->full_name,
				 entity->street_address );
			continue;
		}

		transaction_date_time_string =
			date_display_yyyy_mm_dd_colon_hms(
				transaction_date_time );

		transaction =
			ledger_transaction_new(
				entity->full_name,
				entity->street_address,
				transaction_date_time_string,
				memo );

		transaction->transaction_amount =
			entity->payment_amount;

		transaction->check_number = entity->check_number;

		list_append_pointer( transaction_list, transaction );

		if ( !transaction->journal_ledger_list )
			transaction->journal_ledger_list =
				list_new();

		/* Debit accounts */
		/* -------------- */
		do {
			account =
				list_get_pointer( 
					entity->
					     liability_account_list );

			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					account->account_name );

			journal_ledger->debit_amount = account->payment_amount;

			list_append_pointer(
				transaction->journal_ledger_list,
				journal_ledger );

		} while( list_next( entity->
					liability_account_list ) );

		/* Loss account */
		/* ------------ */
		if ( !timlib_dollar_virtually_same(
			entity->loss_amount, 0.0 ) )
		{
			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					loss_account_name );
	
			journal_ledger->debit_amount =
				entity->loss_amount;
	
			list_append_pointer(
				transaction->journal_ledger_list,
				journal_ledger );
		}

		/* Credit account */
		/* -------------- */
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				credit_account_name );

		journal_ledger->credit_amount = entity->payment_amount;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		date_increment_seconds(
			transaction_date_time,
			1 );

	} while( list_next( process_entity_list ) );

	return transaction_list;

} /* pay_liabilities_output_get_entity_transaction_list() */

LIST *pay_liabilities_get_current_liability_entity_list(
			LIST *current_liability_account_list,
			LIST *exclude_entity_list )
{
	LIST *current_liability_entity_list;
	ENTITY *entity;
	ACCOUNT *account;
	ACCOUNT *new_account;
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;
	double difference;

	if ( !list_rewind( current_liability_account_list ) )
		return (LIST *)0;

	current_liability_entity_list = list_new();

	do {
		account = list_get_pointer( current_liability_account_list );

		journal_ledger_list = account->journal_ledger_list;

		if ( !list_rewind( journal_ledger_list ) ) continue;

		do {
			journal_ledger =
				list_get_pointer(
					journal_ledger_list );

			if ( list_length( exclude_entity_list )
			&&   entity_list_exists(
				exclude_entity_list,
				journal_ledger->full_name,
				journal_ledger->street_address ) )
			{
				continue;
			}

			entity = entity_get_or_set(
					current_liability_entity_list,
					journal_ledger->full_name,
					journal_ledger->street_address,
					0 /* not with_strdup */ );

			difference =	journal_ledger->credit_amount -
					journal_ledger->debit_amount;

			if ( !entity->liability_account_list )
				entity->liability_account_list =
					list_new();

			new_account =
				ledger_account_get_or_set(
					entity->liability_account_list,
					account->account_name );

			entity->sum_balance += difference;
			new_account->balance += difference;

			if ( !new_account->journal_ledger_list )
				new_account->journal_ledger_list =
					list_new();

			list_append_pointer(
				new_account->journal_ledger_list,
				journal_ledger );

		} while( list_next( journal_ledger_list ) );

	} while( list_next( current_liability_account_list ) );

	return current_liability_entity_list;

} /* pay_liabilities_get_current_liability_entity_list() */

LIST *pay_liabilities_process_get_entity_list(
			LIST *input_entity_list,
			LIST *process_current_liability_entity_list,
			LIST *input_purchase_order_list,
			double dialog_box_payment_amount,
			int starting_check_number )
{
	LIST *entity_list;
	ENTITY *input_entity;
	ENTITY *current_liability_entity;
	ENTITY *new_entity;

	if ( !list_rewind( input_entity_list ) )
		return (LIST *)0;

	entity_list = list_new();

	do {
		input_entity =
			list_get_pointer(
				input_entity_list );

		/* If the entity wasn't selected in the dialog box. */
		/* ------------------------------------------------ */
		if ( ! ( current_liability_entity =
				entity_seek(
					process_current_liability_entity_list,
					input_entity->full_name,
					input_entity->street_address ) ) )
		{
			continue;
		}

		new_entity = timlib_memcpy( input_entity, sizeof( ENTITY ) );

		list_append_pointer( 
			entity_list,
			new_entity );

		if ( dialog_box_payment_amount )
		{
			new_entity->payment_amount =
				dialog_box_payment_amount;
		}
		else
		{
			new_entity->payment_amount =
				input_entity->sum_balance;
		}

		if (	new_entity->payment_amount >
			new_entity->sum_balance )
		{
			new_entity->loss_amount =
				new_entity->payment_amount -
				new_entity->sum_balance;
		}

		new_entity->liability_account_list =
			pay_liabilities_distribute_liability_account_list(
				current_liability_entity->
					liability_account_list,
				new_entity->payment_amount );

		new_entity->purchase_order_list =
			pay_liabilities_distribute_purchase_order_list(
				input_purchase_order_list,
				new_entity->payment_amount,
				new_entity->full_name,
				new_entity->street_address );

		if ( starting_check_number )
		{
			new_entity->check_number =
				starting_check_number++;
		}

	} while( list_next( input_entity_list ) );

	return entity_list;

} /* pay_liabilities_process_get_entity_list() */

LIST *pay_liabilities_distribute_purchase_order_list(
				LIST *input_purchase_order_list,
				double payment_amount,
				char *full_name,
				char *street_address )
{
	LIST *purchase_order_list;
	PURCHASE_ORDER *purchase_order;
	PURCHASE_ORDER *new_purchase_order;

	if ( !list_rewind( input_purchase_order_list ) ) return (LIST *)0;

	purchase_order_list = list_new();

	do {
		purchase_order = list_get_pointer( input_purchase_order_list );

		if ( ( strcmp( purchase_order->full_name, full_name ) != 0 )
		||   ( strcmp( purchase_order->street_address,
		       street_address ) != 0 ) )
		{
			continue;
		}

		new_purchase_order =
			timlib_memcpy(
				purchase_order,
				sizeof( PURCHASE_ORDER ) );

		list_append_pointer(
			purchase_order_list,
			new_purchase_order );

		/* If made a partial payment */
		/* ------------------------- */
		if ( payment_amount < purchase_order->amount_due )
		{
			new_purchase_order->liability_payment_amount =
				payment_amount;

			new_purchase_order->amount_due -= payment_amount;
			payment_amount = 0.0;
		}
		else
		{
			new_purchase_order->liability_payment_amount =
				new_purchase_order->amount_due;

			new_purchase_order->amount_due = 0.0;
			payment_amount -= purchase_order->amount_due;
		}

		if ( timlib_dollar_virtually_same( payment_amount, 0.0 )
		||   payment_amount <= 0.0 )
		{
			break;
		}

	} while( list_next( input_purchase_order_list ) );

	return purchase_order_list;

} /* pay_liabilities_distribute_purchase_order_list() */

LIST *pay_liabilities_fetch_current_liability_account_list(
				char *application_name,
				char *fund_name,
				LIST *exclude_account_name_list )
{
	LIST *account_list;
	ACCOUNT *account;
	char *select;
	char fund_where[ 128 ];
	char where[ 256 ];
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char in_clause_where[ 1024 ];
	char *in_clause;

	if ( list_length( exclude_account_name_list ) )
	{
		in_clause =
			timlib_with_list_get_in_clause(
				exclude_account_name_list );

		sprintf( in_clause_where,
			 "account not in (%s)",
			 in_clause );
	}
	else
	{
		strcpy( in_clause_where, "1 = 1" );
	}

	account_list = list_new();

	select =
		ledger_account_get_select(
			application_name );

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf( fund_where, "fund = '%s'", fund_name );
	}
	else
	{
		strcpy( fund_where, "1 = 1" );
	}

	sprintf( where,
		 "subclassification = 'current_liability' and	"
		 "account <> 'uncleared_checks' and		"
		 "%s and					"
		 "%s						",
		 fund_where,
		 in_clause_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
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

		account->journal_ledger_list =
			ledger_get_after_balance_zero_journal_ledger_list(
				application_name,
				account->account_name );

		if ( list_length( account->journal_ledger_list ) )
		{
			list_append_pointer( account_list, account );
		}
	}

	pclose( input_pipe );
	return account_list;

} /* pay_liabilities_fetch_current_liability_account_list() */

/* ------------------------------------------ */
/* Future work: need to optionally join fund. */
/* ------------------------------------------ */
LIST *pay_liabilities_fetch_liability_account_entity_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char account_name[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char *folder;
	char *select;
	ENTITY *entity;
	JOURNAL_LEDGER *latest_ledger;
	ACCOUNT *account;
	LIST *liability_account_entity_list = list_new();

	folder = "liability_account_entity";
	select = "account,full_name,street_address";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 512 ) )
	{
		piece( account_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		if ( ! ( latest_ledger =
				ledger_get_latest_ledger(
					application_name,
					account_name,
					(char *)0 /* as_of_date */ ) ) )
		{
			continue;
		}

		if ( !latest_ledger->balance ) continue;

		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 2 );

		entity =
			entity_get_or_set(
				liability_account_entity_list,
				full_name,
				street_address,
				1 /* with_strdup */ );

		entity->sum_balance += latest_ledger->balance;

		account = ledger_account_new( strdup( account_name ) );
		account->latest_ledger = latest_ledger;
		account->balance = account->latest_ledger->balance;

		/* Prior assignment assigns the local memory. */
		/* ------------------------------------------ */
		account->latest_ledger->account_name = account->account_name;

		if ( !entity->liability_account_list )
			entity->liability_account_list = list_new();

		list_append_pointer( entity->liability_account_list, account );
	}

	pclose( input_pipe );

	return liability_account_entity_list;

} /* pay_liabilities_fetch_liability_account_entity_list() */

LIST *pay_liabilities_get_liability_account_entity_list(
		LIST *input_liability_account_entity_list,
		LIST *input_entity_list,
		double dialog_box_payment_amount,
		int starting_check_number )
{
	LIST *liability_account_entity_list;
	ENTITY *liability_account_entity;
	ENTITY *new_entity;
	ENTITY *input_entity;

	if ( !list_rewind( input_liability_account_entity_list ) )
		return (LIST *)0;

	liability_account_entity_list = list_new();

	do {
		liability_account_entity =
			list_get_pointer(
				input_liability_account_entity_list );

		/* If the entity wasn't selected in the dialog box. */
		/* ------------------------------------------------ */
		if ( ! ( input_entity =
				entity_seek(
					input_entity_list,
					liability_account_entity->full_name,
					liability_account_entity->
						street_address ) ) )
		{
			continue;
		}

		new_entity = timlib_memcpy( input_entity, sizeof( ENTITY ) );

		list_append_pointer( 
			liability_account_entity_list,
			new_entity );

		if ( dialog_box_payment_amount )
		{
			new_entity->payment_amount = dialog_box_payment_amount;
		}
		else
		{
			new_entity->payment_amount = input_entity->sum_balance;
		}

		new_entity->liability_account_list =
			pay_liabilities_distribute_liability_account_list(
				liability_account_entity->
					liability_account_list,
				new_entity->payment_amount );

		if (	new_entity->payment_amount >
			new_entity->sum_balance )
		{
			new_entity->loss_amount =
				new_entity->payment_amount -
				new_entity->sum_balance;
		}

		if ( starting_check_number )
		{
			new_entity->check_number = starting_check_number++;
		}

	} while( list_next( input_liability_account_entity_list ) );

	return liability_account_entity_list;

} /* pay_liabilities_get_liability_account_entity_list() */

LIST *pay_liabilities_distribute_liability_account_list(
			LIST *liability_account_list,
			double entity_payment_amount )
{
	LIST *return_liability_account_list;
	ACCOUNT *account;
	ACCOUNT *new_account;

	if ( !list_rewind( liability_account_list ) ) return (LIST *)0;

	return_liability_account_list = list_new();

	do {
		account = list_get_pointer( liability_account_list );

		if ( !account->balance )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: balance is zero for account = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 account->account_name );
			exit( 1 );
		}

		new_account = timlib_memcpy( account, sizeof( ACCOUNT ) );

		/* If partial payment. */
		/* ------------------- */
		if ( entity_payment_amount < account->balance )
		{
			new_account->payment_amount = entity_payment_amount;

			entity_payment_amount = 0.0;
		}
		else
		{
			new_account->payment_amount = account->balance;

			entity_payment_amount -= account->balance;
		}

		list_append_pointer(
			return_liability_account_list,
			new_account );

		if ( timlib_dollar_virtually_same(
			entity_payment_amount, 0.0 ) )
		{
			break;
		}

	} while( list_next( liability_account_list ) );

	return return_liability_account_list;

} /* pay_liabilities_distribute_liability_account_list() */

LIST *pay_liabilities_input_get_entity_list(
			char *application_name,
			char *fund_name,
			LIST *full_name_list,
			LIST *street_address_list )
{
	char *full_name;
	char *street_address;
	ENTITY *entity;
	LIST *entity_list;

	if ( !list_rewind( full_name_list ) ) return (LIST *)0;

	list_rewind( street_address_list );
	entity_list = list_new();

	do {
		full_name = list_get_pointer( full_name_list );
		street_address = list_get_pointer( street_address_list );

		entity = entity_new( full_name, street_address );

		if ( ! ( entity->sum_balance =
				pay_liabilities_fetch_sum_balance(
					application_name,
					fund_name,
					full_name,
					street_address ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get sum_balance for %s/%s.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 full_name,
				 street_address );
			exit( 1 );
		}

		list_append_pointer(
			entity_list,
			entity );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	return entity_list;

} /* pay_liabilities_input_get_entity_list() */

double pay_liabilities_fetch_sum_balance(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address )
{
	char sys_string[ 128 ];
	char *record;
	char input_full_name[ 128 ];
	char input_street_address_balance[ 128 ];
	char input_street_address[ 128 ];
	char input_balance[ 16 ];
	static LIST *entity_record_list = {0};

	if ( !entity_record_list )
	{
		sprintf( sys_string,
		 	"populate_print_checks_entity %s '%s'",
		 	application_name,
			fund_name );

		entity_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( entity_record_list ) ) return 0.0;

	do {
		record = list_get_pointer( entity_record_list );

		if ( character_count(
			FOLDER_DATA_DELIMITER,
			record ) != 1 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: not one delimiter in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 record );

			return 0.0;
		}

		piece(	input_full_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	input_street_address_balance,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		piece(	input_street_address,
			'[',
			input_street_address_balance,
			0 );

		piece( input_balance, '[', input_street_address_balance, 1 );

		if ( strcmp( input_full_name, full_name ) == 0
		&&   strcmp( input_street_address, street_address ) == 0 )
		{
			return atof( input_balance );
		}

	} while( list_next( entity_record_list ) );

	return 0.0;

} /* pay_liabilities_fetch_sum_balance() */

LIST *pay_liabilities_output_get_vendor_payment_list(
				LIST *process_entity_list,
				LIST *transaction_list )
{
	LIST *vendor_payment_list = {0};
	ENTITY *entity;
	PURCHASE_ORDER *purchase_order;
	VENDOR_PAYMENT *vendor_payment;
	TRANSACTION *transaction;

	if ( !list_rewind( process_entity_list ) ) return (LIST *)0;

	do {
		entity = list_get_pointer( process_entity_list );

		if ( !list_rewind( entity->purchase_order_list ) )
			continue;

		do {
			purchase_order =
				list_get_pointer(
					entity->purchase_order_list );

			if ( ! ( transaction =
					ledger_transaction_seek(
						transaction_list,
						purchase_order->full_name,
						purchase_order->street_address,
						(char *)0
						/* transaction_date_time */) ) )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot seek transaction for (%s/%s)\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					purchase_order->full_name,
					purchase_order->street_address );
				exit( 1 );
			}

			vendor_payment =
				purchase_vendor_payment_new(
					transaction->transaction_date_time
						/* payment_date_time */ );

			vendor_payment->full_name = purchase_order->full_name;

			vendor_payment->street_address =
				purchase_order->street_address;

			vendor_payment->purchase_date_time =
				purchase_order->purchase_date_time;

			vendor_payment->payment_amount =
				purchase_order->liability_payment_amount;

			vendor_payment->check_number =
				entity->check_number;

			vendor_payment->transaction = transaction;

			vendor_payment->purchase_order = purchase_order;

			if ( !vendor_payment_list )
				vendor_payment_list =
					list_new();

			list_append_pointer(
				vendor_payment_list,
				vendor_payment );

		} while( list_next( entity->purchase_order_list ) );

	} while ( list_next( process_entity_list ) );

	return vendor_payment_list;

} /* pay_liabilities_output_get_vendor_payment_list() */

VENDOR_PAYMENT *pay_liabilities_vendor_payment_seek(
				LIST *vendor_payment_list,
				char *full_name,
				char *street_address )
{
	VENDOR_PAYMENT *vendor_payment;

	if ( !list_rewind( vendor_payment_list ) ) return (VENDOR_PAYMENT *)0;

	do {
		vendor_payment = list_get_pointer( vendor_payment_list );

		if ( timlib_strcmp(	vendor_payment->full_name,
					full_name ) == 0
		&&   timlib_strcmp(	vendor_payment->street_address,
					street_address ) == 0 )
		{
			return vendor_payment;
		}
	} while( list_next( vendor_payment_list ) );

	return (VENDOR_PAYMENT *)0;

} /* pay_liabilities_vendor_payment_seek() */

void pay_liabilities_set_lock_transaction(
			LIST *transaction_list,
			LIST *vendor_payment_list )
{
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) ) return;

	do {
		transaction = list_get_pointer( transaction_list );

		if ( (boolean)pay_liabilities_vendor_payment_seek(
				vendor_payment_list,
				transaction->full_name,
				transaction->street_address ) )
		{
			transaction->lock_transaction = 1;
		}
	} while( list_next( transaction_list ) );

} /* pay_liabilities_set_lock_transaction() */

LIST *pay_liabilities_fetch_liability_account_list(
				char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=account			"
		 "			folder=liability_account_entity	",
		 application_name );

	return pipe2list( sys_string );

} /* pay_liabilities_fetch_liability_account_list() */

/* -------------------------------- */
/* Returns memo or strdup() memory. */
/* -------------------------------- */
char *pay_liabilities_transaction_memo(	char *application_name,
					char *fund_name,
					char *memo,
					int check_number )
{
	static char *uncleared_checks_account = {0};

	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		return memo;
	}

	if ( !uncleared_checks_account )
	{
		uncleared_checks_account =
			ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				LEDGER_UNCLEARED_CHECKS_KEY,
				0 /* not warning_only */,
				__FUNCTION__ );
	}

	if ( check_number )
		return uncleared_checks_account;
	else
		return memo;

} /* pay_liabilities_transaction_memo() */

