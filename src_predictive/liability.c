/* ---------------------------------------------*/
/* $APPASERVER_HOME/src_predictive/liability.c	*/
/* ---------------------------------------------*/
/*						*/
/* Freely available software: see Appaserver.org*/
/* ---------------------------------------------*/

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

LIST *liability_current_liability_account_list( void )
{
	char where[ 256 ];
	char sys_string[ 1024 ];
	LIST *entire_account_list;
	LIST *return_account_list;
	ACCOUNT *account;

/*
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
*/

	sprintf( where,
		 "subclassification = 'current_liability' and	"
		 "account <> 'uncleared_checks' 		" );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 account_select(),
		 "account",
		 where,
		 "account" );

	entire_account_list = account_system_list( sys_string );

	if ( !list_rewind( entire_account_list ) ) return (LIST *)0;

	return_account_list = list_new();

	do {
		account = list_get( entire_account_list );

		account->transaction_after_balance_zero_journal_list =
			transaction_after_balance_zero_journal_list(
				account->account_name );

		if ( list_length(
			account->
				transaction_after_balance_zero_journal_list ) )
		{
			list_set( return_account_list, account );
		}

	} while ( list_next( entire_account_list ) );

	return return_account_list;
}

LIST *liability_entity_list( LIST *account_list )
{
	return account_entity_list( account_list );
}

LIST *liability_account_entity_list(
			LIST *entity_list,
			double dialog_box_payment_amount,
			int starting_check_number,
			LIST *current_liability_account_list )
{
	ENTITY *entity;
	ACCOUNT *account;

	if ( !list_rewind( entity_list ) ) return (LIST *)0;

	account_entity_list = list_new();

	do {
		entity = list_get( entity_list );

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
}
