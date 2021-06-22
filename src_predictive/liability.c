/* --------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/liability.c	 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "entity.h"
#include "list.h"
#include "transaction.h"
#include "account.h"
#include "liability.h"

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek(
			char *account_name,
			LIST *liability_account_entity_list )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !list_rewind( liability_account_entity_list ) )
		return (LIABILITY_ACCOUNT_ENTITY *)0;

	do {

		liability_account_entity =
			list_get( 
				liability_account_entity_list );

		if ( timlib_strcmp(
			liability_account_entity->account_name,
			account_name ) == 0 )
		{
			return liability_account_entity;
		}
			

	} while ( list_next( liability_account_entity_list ) );
	return (LIABILITY_ACCOUNT_ENTITY *)0;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_calloc(
			void )
{
	LIABILITY_ACCOUNT_ENTITY *a;

	if ( ! ( a =
		    (LIABILITY_ACCOUNT_ENTITY *)
			calloc( 1, sizeof( LIABILITY_ACCOUNT_ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}
	return a;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_parse(
			char *input )
{
	char account_name[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !input ) return (LIABILITY_ACCOUNT_ENTITY *)0;

	/* See: attribute_list */
	/* ------------------- */
	piece( account_name, SQL_DELIMITER, input, 0 );
	piece( full_name, SQL_DELIMITER, input, 1 );
	piece( street_address, SQL_DELIMITER, input, 2 );

	liability_account_entity = liability_account_entity_calloc();

	liability_account_entity->account_name = strdup( account_name );

	liability_account_entity->entity =
		entity_new(
			strdup( full_name ),
			strdup( street_address ) );

	return liability_account_entity;
}

LIST *liability_account_entity_list( void )
{
	return	liability_account_entity_system_list(
			liability_account_entity_system_string(
				"1 = 1" /* where */ ) );
}

char *liability_account_entity_system_string(
			char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf( system_string,
		 "select.sh '*' %s \"%s\" none",
		 "liability_account_entity",
		 where );

	return strdup( system_string );
}

LIST *liability_account_entity_system_list( char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *liability_account_entity_list;

	if ( !system_string ) return (LIST *)0;

	liability_account_entity_list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	liability_account_entity_list,
				liability_account_entity_parse( input ) );
	}

	pclose( input_pipe );
	return liability_account_entity_list;
}

LIABILITY *liability_calloc( void )
{
	LIABILITY *a;

	if ( ! ( a = (LIABILITY *) calloc( 1, sizeof( LIABILITY ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}
	return a;
}

LIST *liability_current_account_list( void )
{
	char where[ 256 ];
	char system_string[ 1024 ];
	LIST *current_account_list;
	LIST *return_account_list = {0};
	ACCOUNT *account;

	sprintf( where,
		 "subclassification = 'current_liability' and	"
		 "account <> 'uncleared_checks' 		" );

	sprintf( system_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 account_select(),
		 ACCOUNT_TABLE_NAME,
		 where,
		 "account" );

	current_account_list = account_system_list( system_string );

	if ( !list_rewind( current_account_list ) ) return (LIST *)0;

	do {
		account = list_get( current_account_list );

		account->account_after_balance_zero_journal_list =
			account_after_balance_zero_journal_list(
				account->account_name );

		if ( list_length(
			account->
				transaction_after_balance_zero_journal_list ) )
		{

			if ( !return_account_list )
				return_account_list = list_new();

			list_set( return_account_list, account );
		}

	} while ( list_next( current_account_list ) );

	return return_account_list;
}

ENTITY *liability_account_entity(
			LIST *liability_account_entity_list,
			char *account_name )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !list_rewind( liability_account_entity_list ) )
		return (ENTITY *)0;

	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( string_strcmp( 
			liability_account_entity->account_name,
			account_name ) == 0 )
		{
			return liability_account_entity->entity;
		}

	} while ( list_next( liability_account_entity_list ) );
	return (ENTITY *)0;
}

LIST *liability_steady_state_entity_list(
			LIST *entity_list )
{
	ENTITY *entity;

	if ( !list_rewind( entity_list ) ) return (LIST *)0;

	do {
		entity = list_get( entity_list );

		liability_steady_state_entity(
			entity,
			entity->liability_after_balance_zero_journal_list );

	} while ( list_next( entity_list ) );

	return entity_list;
}

/* Also sets entity->entity_liability_debit_account_name */
/* ----------------------------------------------------- */
LIST *liability_entity_list(
			LIST *liability_account_list,
			LIST *input_entity_list,
			double dialog_box_payment_amount )
{
	ACCOUNT *account;
	LIST *journal_list;
	JOURNAL *journal;
	LIST *entity_list;
	ENTITY *entity;

	if ( !list_rewind( liability_account_list ) ) return (LIST *)0;

	entity_list = list_new();

	do {
		account = list_get( liability_account_list );

		journal_list =
			account->
				transaction_after_balance_zero_journal_list;

		if ( !list_rewind( journal_list ) ) continue;

		do {
			journal = list_get( journal_list );

			if ( !journal->full_name )
			{
				fprintf(stderr,
				"ERROR in %s/%s()/%d: empty full_name.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			if ( input_entity_list
			&&   !entity_seek(
				journal->full_name,
				journal->street_address,
				input_entity_list ) )
			{
				continue;
			}

			entity =
				entity_getset(
					entity_list,
					journal->full_name,
					journal->street_address,
					0 /* not with_strdup */ );

			entity->entity_liability_debit_account_name =
				entity_liability_debit_account_name(
					account->account_name );

			entity->dialog_box_payment_amount =
				dialog_box_payment_amount;

		} while( list_next( journal_list ) );

	} while( list_next( liability_account_list ) );

	return entity_list;
}

char *liability_credit_account_name(
			int starting_check_number )
{
	if ( starting_check_number )
		return account_uncleared_checks( (char *)0 /* fund_name */ );
	else
		return account_cash( (char *)0 /* fund_name */ );
}

LIST *liability_transaction_list(
			LIST *liability_entity_list,
			char *liability_credit_account_name,
			int starting_check_number )
{
	LIST *transaction_list;
	ENTITY *entity;
	DATE *transaction_date_time;

	if ( !list_rewind( liability_entity_list ) ) return (LIST *)0;

	transaction_list = list_new();

	transaction_date_time =
		date_now_new(
			date_get_utc_offset() );

	do {
		entity = list_get( liability_entity_list );

		list_set(
			transaction_list,
			liability_transaction(
				entity->full_name,
				entity->street_address,
				date_display_19( transaction_date_time ),
				entity->
				     entity_liability_payment_amount,
				entity->
				     entity_liability_additional_payment_amount,
				entity->
				     entity_liability_debit_account_name,
				entity_liability_account_name,
				LIABILITY_MEMO,
				starting_check_number ) );

		date_increment_seconds(
			transaction_date_time,
			1 );

		if ( starting_check_number )
			starting_check_number++;

	} while( list_next( liability_entity_list ) );

	return transaction_list;
}

ENTITY *liability_steady_state_entity(
			ENTITY *entity,
			LIST *liability_after_balance_zero_journal_list )
{
	if ( !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty entity.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	entity->liability_entity_amount_due =
		liability_entity_amount_due(
			liability_after_balance_zero_journal_list );

	entity->liability_entity_payment_amount =
		liability_entity_payment_amount(
			entity->dialog_box_payment_amount,
			entity->liability_entity_amount_due );

	entity->liability_entity_additional_payment_amount =
		liability_entity_additional_payment_amount(
			entity->dialog_box_payment_amount,
			entity->liability_entity_amount_due );

	return entity;
}

double liability_entity_payment_amount(
			double dialog_box_payment_amount,
			double liability_entity_amount_due )
{
	if ( !dialog_box_payment_amount )
		return liability_entity_amount_due;

	if ( dialog_box_payment_amount < liability_entity_amount_due )
		return dialog_box_payment_amount;
	else
		return liability_entity_amount_due;
}

double liability_entity_additional_payment_amount(
			double dialog_box_payment_amount,
			double liability_entity_payment_amount )
{
	double additional_payment_amount;

	if ( !dialog_box_payment_amount ) return 0.0;

	additional_payment_amount =
		dialog_box_payment_amount -
		liability_entity_payment_amount;

	if ( additional_payment_amount <= 0.0 )
		return 0.0;
	else
		return additional_payment_amount;
}

LIST *liability_tax_redirect_account_list(
			LIST *liability_current_account_list,
			LIST *liability_account_entity_list )
{
	ACCOUNT *account;
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !list_rewind( liability_current_account_list ) )
		return (LIST *)0;

	do {
		account = list_get( liability_current_account_list );

		if ( ( liability_account_entity =
			liability_account_entity_seek(
				account->account_name,
				liability_account_entity_list ) ) )
		{
			liability_set_entity(
				account->
				    transaction_after_balance_zero_journal_list,
				liability_account_entity->
					entity->
					full_name,
				liability_account_entity->
					entity->
					street_address );
		}

	} while ( list_next( liability_current_account_list ) );

	return liability_current_account_list;
}

void liability_set_entity(
			LIST *transaction_after_balance_zero_journal_list,
			char *full_name,
			char *street_address )
{
	JOURNAL *journal;

	if ( !list_rewind( transaction_after_balance_zero_journal_list ) )
		return;

	do {
		journal =
			list_get(
				transaction_after_balance_zero_journal_list );

		journal->full_name = full_name;
		journal->street_address = street_address;

	} while ( list_next( transaction_after_balance_zero_journal_list ) );
}

LIST *liability_after_balance_zero_entity_list(
			LIST *liability_entity_list,
			LIST *liability_account_list )
{
	ENTITY *entity;

	if ( !list_rewind( liability_entity_list ) ) return (LIST *)0;

	do {
		entity = list_get( liability_entity_list );

		entity->entity_liability_journal_list =
			entity_liability_journal_list(
				liability_account_list,
				entity->full_name,
				entity->street_address );

	} while ( list_next( liability_entity_list ) );

	return liability_entity_list;
}

LIABILITY *liability_new(
			double dialog_box_payment_amount,
			int starting_check_number  )
{
	LIABILITY *liability;

	liability = liability_calloc();

	liability->dialog_box_payment_amount = dialog_box_payment_amount;
	liability->starting_check_number = starting_check_number;

	return liability;
}

double liability_prepaid(
			char *payor_full_name,
			char *payor_street_address )
{
	LIST *entity_list = list_new();
	LIABILITY *liability = liability_calloc();
	ENTITY *entity;

	list_set(
		entity_list,
		entity_new(
			payor_full_name,
			payor_street_address ) );

	liability->liability_current_account_list =
		liability_current_account_list();

	liability->liability_entity_list =
		/* ----------------------------------------------------- */
		/* Also sets entity->liability_entity_debit_account_name */
		/* ----------------------------------------------------- */
		liability_entity_list(
			liability->liability_current_account_list,
			entity_list /* input_entity_list */,
			0.0 /* dialog_box_payment_amount */ );

	liability->liability_after_balance_zero_entity_list =
		liability_after_balance_zero_entity_list(
			liability->liability_entity_list,
			liability->liability_current_account_list );


	if ( !list_rewind(
		liability->liability_after_balance_zero_entity_list ) )
	{
		return 0.0;
	}

	entity =
		list_get(
			liability->
				liability_after_balance_zero_entity_list );

	if ( !list_length(
		entity->
			liability_after_balance_zero_journal_list ) )
	{
		return 0.0;
	}

	if ( ( entity->entity_liability_amount_due =
		entity_liability_amount_due(
			entity_liability_journal_list(
				liability->liability_current_account_list,
				entity->full_name,
				entity->street_address ) ) ) > 0.0 )
	{
		return entity->liability_entity_amount_due;
	}

	return 0.0;
}

TRANSACTION *liability_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double payment_amount,
			double additional_payment_amount,
			char *entity_liability_debit_account_name,
			char *liability_credit_account_name,
			char *memo,
			int check_number )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( !payment_amount ) return (TRANSACTION *)0;

	transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_time );

	transaction->memo = memo;
	transaction->transaction_amount = payment_amount;
	transaction->check_number = check_number;

	if ( !transaction->journal_list )
		transaction->journal_list =
			list_new();

	/* Debit accounts */
	/* -------------- */
	journal =
		journal_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			entity_liability_debit_account_name );

	journal->debit_amount = payment_amount + additional_payment_amount;
	list_set( transaction->journal_list, journal );

	/* Credit account */
	/* -------------- */
	journal =
		journal_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			liability_credit_account_name );

	journal->credit_amount = payment_amount + additional_payment_amount;
	list_set( transaction->journal_list, journal );

	return transaction;
}

