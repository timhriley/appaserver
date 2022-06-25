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
#include "subclassification.h"
#include "account.h"
#include "journal.h"
#include "liability.h"

LIABILITY *liability_entity_fetch(
			char *full_name,
			char *street_address,
			LIST *account_current_liability_name_list )
{
	LIABILITY *liability = liability_calloc();

	if ( !full_name
	||   !street_address
	||   !list_length( account_current_liability_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability->timlib_in_clause =
		timlib_in_clause(
			account_current_liability_name_list );

	liability->liability_entity_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_entity_where(
			full_name,
			street_address,
			liability->timlib_in_clause );

	liability->journal_system_list =
		journal_system_list(
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				liability->liability_entity_where ),
			0 /* not fetch_check_number */,
			0 /* not fetch_memo */ );

	liability->journal_credit_debit_difference_sum =
		journal_credit_debit_difference_sum(
			liability->journal_system_list );

	return liability;
}

char *liability_entity_where(
			char *full_name,
			char *street_address,
			char *timlib_in_clause )
{
	static char where[ 512 ];

	sprintf(where,
		"full_name = '%s and "
		"street_address = '%s' and "
		"account in (%s)",
			full_name,
			street_address,
			timlib_in_clause );

	return where;
}

LIABILITY *liability_account_fetch(
			char *liability_account_name )
{
	LIABILITY *liability = liability_calloc();

	if ( !liability_account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: liability_account_name empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability->liability_account_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_account_where(
			liability_account_name );

	liability->journal_system_list =
		journal_system_list(
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				liability->liability_account_where ),
			0 /* not fetch_check_number */,
			0 /* not fetch_memo */ );

	liability->journal_credit_debit_difference_sum =
		journal_credit_debit_difference_sum(
			liability->journal_system_list );

	return liability;
}


char *liability_account_where( char *liability_account_name )
{
	static char where[ 128 ];

	sprintf(where,
		"account = '%s'",
		liability_account_name );

	return where;
}

LIABILITY *liability_calloc( void )
{
	LIABILITY *liability;

	if ( ! ( liability = calloc( 1, sizeof( LIABILITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}
	return liability;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_calloc( void )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( ! ( liability_account_entity =
			calloc( 1, sizeof( LIABILITY_ACCOUNT_ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return liability_account_entity;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_new(
			char *account_name )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity =
		liability_account_entity_calloc();

	liability_account_entity->account_name = account_name;

	return liability_account_entity;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_parse(
			char *string_input )
{
	char account_name[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !string_input ) return (LIABILITY_ACCOUNT_ENTITY *)0;

	/* See LIABILITY_ACCOUNT_ENTITY_SELECT */
	/* ------------------------------------ */
	piece( account_name, SQL_DELIMITER, input, 0 );

	liability_account_entity =
		liability_account_entity_new(
			strdup( account_name );

	if ( piece( full_name, SQL_DELIMITER, input, 1 ) )
	{
		piece( street_address, SQL_DELIMITER, input, 2 );

		liability_account_entity->entity =
			entity_new(
				strdup( full_name ),
				strdup( street_address ) );
	}

	return liability_account_entity;
}

LIST *liability_account_entity_list( void )
{
	return
	liability_account_entity_system_list(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_account_entity_system_string(
			LIABILITY_ACCOUNT_ENTITY_SELECT,
			LIABILITY_ACCOUNT_ENTITY_TABLE );
}

char *liability_account_entity_system_string(
			char *select,
			char *table )
{
	static char system_string[ 128 ];

	sprintf(system_string,
		"select.sh \"%s\" %s",
		select,
		table );

	return system_string;
}

LIST *liability_account_entity_system_list( char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *liability_account_entity_list;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

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

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek_account(
			char *account_name,
			LIST *liability_account_entity_list )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( liability_account_entity_list ) )
	{
		return (LIABILITY_ACCOUNT_ENTITY *)0;
	}

	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( string_strcmp(
			liability_account_entity->account_name,
			account_name ) == 0 )
		{
			return liability_account_entity;
		}

	} while ( list_next( liability_account_entity_list ) );

	return (LIABILITY_ACCOUNT_ENTITY *)0;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek_entity(
			char *full_name,
			char *street_address,
			LIST *liability_account_entity_list )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;

	if ( !full_name
	||   !street_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( liability_account_entity_list ) )
	{
		return (LIABILITY_ACCOUNT_ENTITY *)0;
	}

	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( !liability_account_entity->entity ) continue;

		if ( strcmp(
			liability_account_entity->entity->full_name,
			full_name ) == 0
		&&   strcmp(
			liability_account_entity->entity->street_address,
			street_address ) == 0 )
		{
			return liability_account_entity;
		}

	} while ( list_next( liability_account_entity_list ) );

	return (LIABILITY_ACCOUNT_ENTITY *)0;
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

char *liability_account_where( void )
{
	static char where[ 128 ];

	sprintf(where,
		"subclassification = 'current_liability' and "
		"account <> 'uncleared_checks'" );

	return where;
}

LIST *liability_balance_zero_account_list(
			char *liability_account_where )
{
	char system_string[ 1024 ];
	LIST *local_account_list;
	LIST *return_account_list = {0};
	ACCOUNT *account;

	local_account_list =
		account_list(
			liability_account_where,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element );

	if ( !list_rewind( local_account_list ) ) return (LIST *)0;

	do {
		account = list_get( local_account_list );

		account->balance_zero_journal_list =
			account_balance_zero_journal_list(
				account->account_name );

		if ( list_length( account->balance_zero_journal_list ) )
		{

			if ( !return_account_list )
			{
				return_account_list = list_new();
			}

			list_set( return_account_list, account );
		}

	} while ( list_next( local_account_list ) );

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
			LIST *liability_balance_zero_entity_list )
{
	ENTITY *entity;
	LIST *entity_list;

	entity_list = liability_balance_zero_entity_list;

	if ( !list_rewind( entity_list ) ) return entity_list;

	do {
		entity = list_get( entity_list );

		entity_liability_steady_state(
			entity,
			entity->entity_balance_zero_account_list );

	} while ( list_next( entity_list ) );

	return entity_list;
}

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
				balance_zero_journal_list;

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
				entity->entity_balance_zero_account_list,
				liability_credit_account_name,
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

LIABILITY *liability_new(
			double dialog_box_payment_amount,
			int starting_check_number,
			LIST *entity_full_street_list )
{
	LIABILITY *liability;

	liability = liability_calloc();

	liability->liability_account_entity_list =
		liability_account_entity_list();

	liability->account_where = liability_account_where();

	/* Sets account->following_balance_zero_journal_list */
	/* ------------------------------------------------- */
	liability->following_balance_zero_account_list =
		liability_following_balance_zero_account_list(
			liability->account_where );


/* Redirects LIABILITY_ACCOUNT_ENTITY */
LIST *liability_tax_redirect_account_list(
	liability_balance_zero_account_list(),
	liability_account_entity_list() );

LIST *liability_entity_list(
	liability_tax_redirect_account_list()
		/* liability_account_list */,
	entity_full_street_list,
	dialog_box_payment_amount );

/* Sets entity_balance_zero_account_list */
LIST *liability_balance_zero_entity_list(
	liability_entity_list(),
	liability_tax_redirect_account_list()
		/* liability_account_list */ );

LIST *liability_steady_state_entity_list(
	liability_balance_zero_entity_list() );

char *liability_credit_account_name(
	int starting_check_number );

LIST *liability_transaction_list(
	liability_steady_state_entity_list(),
	liability_credit_account_name(),
	starting_check_number );

TRANSACTION *liability_transaction(
	full_name,
	street_address,
	transaction_date_time,
	entity_liability_payment_amount(),
	entity_liability_additional_payment_amount(),
	entity_balance_zero_account_list(),
	liability_credit_account_name(),
	memo,
	check_number );

	return liability;
}

TRANSACTION *liability_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double payment_amount,
			double additional_payment_amount,
			LIST *entity_balance_zero_account_list,
			char *liability_credit_account_name,
			char *memo,
			int check_number )
{
	TRANSACTION *transaction;
	JOURNAL *journal;
	LIST *account_list;
	ACCOUNT *account;
	double proportional_additional_payment_amount;

	if ( !payment_amount ) return (TRANSACTION *)0;

	account_list = entity_balance_zero_account_list;

	if ( !list_rewind( account_list ) ) return (TRANSACTION *)0;

	proportional_additional_payment_amount =
		additional_payment_amount /
		(double)list_length( account_list );

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
	do {
		account = list_get( account_list );

		journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account->account_name );

		journal->debit_amount =
			account->account_liability_due +
			proportional_additional_payment_amount;

		list_set( transaction->journal_list, journal );

	} while ( list_next( account_list ) );

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

LIABILITY_ENTITY *liability_entity_new(
			double dialog_box_payment_amount,
			LIST *liability_account_entity_list,
			LIST *account_current_liability_name_list,
			LIST *account_receivable_name_list,
			ENTITY *entity )
{
	LIABILITY_ENTITY *liability_entity = liability_entity_calloc();

	if ( !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: entity is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_entity->liability_account_entity =
		liability_account_entity_seek_entity(
			entity->full_name,
			entity->street_address,
			liability_account_entity_list );

	if ( liability_entity->liability_account_entity )
	{
		liability_entity->liability =
			liability_account_fetch(
				liability_entity->
					liability_account_entity->
						accout_name );
	}
	else
	{
		liability_entity->liability =
			liability_entity_fetch(
				entity->full_name,
				entity->street_address,
				account_current_liability_name_list );
	}

	if ( !liability_entity->liability )
	{
		free( liability_entity );
		return (LIABILITY_ENTITY *)0;
	}

	if ( !liability_account_entity )
	{
		liability_entity->receivable =
			receivable_fetch(
				entity->full_name,
				entity->street_address,
				account_receivable_name_list );
	}

	liability_entity->amount_due =
		liability_entity_amount_due(
			dialog_box_payment_amount,
			liability_entity->liability,
			liability_entity->receivable );

	return liability_entity;
}

double liability_entity_amount_due(
			double dialog_box_payment_amount,
			LIABILITY *liability,
			RECEIVABLE *receivable )
{
	double amount_due;

	if ( !liability )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: liability is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( dialog_box_payment_amount )
	{
		return dialog_box_payment_amount;
	}

	if ( ! ( amount_due = liability->journal_credit_debit_difference_sum ) )
	{
		return 0.0;
	}

	if ( receivable )
	{
		amount_due += receivable->journal_debit_credit_difference_sum;
	}

	return amount_due;
}

LIABILITY_ENTITY *liability_entity_calloc( void )
{
	LIABILITY_ENTITY *liability_entity;

	if ( ! ( liability_entity = calloc( 1, sizeof( LIABILITY_ENTITY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_entity;
}

LIABILITY_PAYMENT *liability_payment_new(
			char *application_name,
			double dialog_box_payment_amount,
			int starting_check_number,
			char *document_root_directory,
			char *process_name,
			char *session_key,
			LIST *entity_full_street_list )
{
	LIABILITY_PAYMENT *liability_payment;

	if ( !list_length( entity_full_street_list ) )
	{
		return (LIABILITY_PAYMENT *)0;
	}

	liability_payment = liability_payment_calloc();

	liability_payment->liability_account_entity_list =
		liability_account_entity_list();

	liability_payment->account_current_liability_name_list =
		account_current_liability_name_list(
			ACCOUNT_TABLE,
			SUBCLASSIFICATION_CURRENT_LIABILITY,
			ACCOUNT_UNCLEARED_CHECKS );

	liability_payment->account_receivable_name_list =
		account_receivable_name_list(
			ACCOUNT_TABLE,
			SUBCLASSIFICATION_RECEIVABLE );

	liability_payment->liability_entity_list = list_new();

	if ( list_length( entity_full_street_list ) == 1 )
	{
		list_set(
			liability_payment->liability_entity_list,
			liability_entity_new(
				dialog_box_payment_amount,
				liability_payment->
					liability_account_entity_list,
				liability_payment->
					account_current_liability_name_list,
				liability_payment->
					account_receivable_name_list,
				list_first( entity_full_street_list ) ) );
	}
	else
	{
		ENTITY *entity;

		list_rewind( entity_full_street_list );

		do {
			entity = list_get( entity_full_street_list );

			list_set(
				liability_payment->liability_entity_list,
				liability_entity_new(
					0 /* dialog_box_payment_amount */,
					liability_payment->
					    liability_account_entity_list,
					liability_payment->
					    account_current_liability_name_list,
					liability_payment->
					    account_receivable_name_list,
					entity ) );

		} while ( list_next( entity_full_street_list ) );
	}

	if ( !list_length( liability_payment->liability_entity_list ) )
	{
		list_free( liability_payment->liability_entity_list );
		free( liability_payment );
		return (LIABILITY_PAYMENT *)0;
	}

	if ( starting_check_number )
	{
		liability_payment->liability_check_list =
			liability_check_list(
				application_name,
				starting_check_number,
				document_root_directory,
				process_name,
				session_key,
				check_personal_size,
				liability_payment->liability_entity_list );
	}

	liability_payment->liability_transaction_list =
		liability_transaction_list(
			starting_check_number,
			liability_payment->liability_entity_list );

	return liability_payment;
}

LIABILITY_PAYMENT *liability_payment_calloc( void )
{
	LIABILITY_PAYMENT *liability_payment;

	if ( ! ( liability_payment =
			calloc( 1, sizeof( LIABILITY_PAYMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_payment;
}

