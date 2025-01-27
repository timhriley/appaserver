/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/liability.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "appaserver.h"
#include "folder.h"
#include "appaserver_error.h"
#include "transaction.h"
#include "subclassification.h"
#include "application.h"
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
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		timlib_in_clause(
			account_current_liability_name_list );

	liability->entity_where =
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
				liability->entity_where ),
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	if ( !list_length( liability->journal_system_list ) )
	{
		free( liability );
		return NULL;
	}

	liability->journal_credit_debit_difference_sum =
		journal_credit_debit_difference_sum(
			liability->journal_system_list );

	if (	!liability->journal_credit_debit_difference_sum
	||	liability->journal_credit_debit_difference_sum < 0.0 )
	{
		list_free( liability->journal_system_list );
		free( liability );
		return NULL;
	}

	liability->liability_account_list =
		liability_account_list_new(
			liability->journal_system_list );

	return liability;
}

char *liability_entity_where(
		char *full_name,
		char *street_address,
		char *timlib_in_clause )
{
	static char where[ 512 ];

	if ( !full_name
	||   !street_address
	||   !timlib_in_clause )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"full_name = '%s' and "
		"street_address = '%s' and "
		"account in (%s)",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name(
			full_name ),
			street_address,
			timlib_in_clause );

	return where;
}

LIABILITY *liability_account_fetch( char *liability_account_name )
{
	LIABILITY *liability;

	if ( !liability_account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: liability_account_name empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability = liability_calloc();

	liability->account_where =
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
				liability->account_where ),
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	if ( !list_length( liability->journal_system_list ) )
	{
		free( liability );
		return NULL;
	}

	liability->journal_credit_debit_difference_sum =
		journal_credit_debit_difference_sum(
			liability->journal_system_list );

	if ( !liability->journal_credit_debit_difference_sum
	||   liability->journal_credit_debit_difference_sum < 0.0 )
	{
		list_free( liability->journal_system_list );
		free( liability );
		return NULL;
	}

	liability->liability_account_list =
		liability_account_list_new(
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

	if ( ! ( liability = calloc( 1, sizeof ( LIABILITY ) ) ) )
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
			calloc( 1, sizeof ( LIABILITY_ACCOUNT_ENTITY ) ) ) )
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
	piece( account_name, SQL_DELIMITER, string_input, 0 );

	liability_account_entity =
		liability_account_entity_new(
			strdup( account_name ) );

	if ( piece( full_name, SQL_DELIMITER, string_input, 1 ) )
	{
		piece( street_address, SQL_DELIMITER, string_input, 2 );

		liability_account_entity->entity =
			entity_new(
				strdup( full_name ),
				strdup( street_address ) );
	}

	return liability_account_entity;
}

LIST *liability_account_entity_list( void )
{
	if ( !folder_column_boolean(
		LIABILITY_ACCOUNT_ENTITY_TABLE,
		"account" /* column_name */ ) )
	{
		return (LIST *)0;
	}

	return
	liability_account_entity_system_list(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		liability_account_entity_system_string(
			LIABILITY_ACCOUNT_ENTITY_SELECT,
			LIABILITY_ACCOUNT_ENTITY_TABLE ) );
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
	LIST *list;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set( list, liability_account_entity_parse( input ) );
	}

	pclose( input_pipe );
	return list;
}

LIST *liability_account_entity_account_name_list(
		LIST *liability_account_entity_list )
{
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;
	LIST *account_name_list;

	account_name_list = list_new();

	if ( !list_rewind( liability_account_entity_list ) )
	{
		return account_name_list;
	}

	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( !liability_account_entity->account_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			account_name_list,
			liability_account_entity->account_name );

	} while ( list_next( liability_account_entity_list ) );

	return account_name_list;
}

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek(
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

char *liability_payment_credit_account_name(
			int starting_check_number,
			char *account_cash,
			char *account_uncleared_checks )
{
	if ( starting_check_number )
		return account_uncleared_checks;
	else
		return account_cash;
}

LIABILITY_ENTITY *liability_entity_account_name_new(
			char *account_name,
			ENTITY *entity )
{
	LIABILITY_ENTITY *liability_entity;

	if ( !account_name
	||   !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_entity = liability_entity_calloc();
	liability_entity->entity = entity;

	if ( ! ( liability_entity->liability =
			liability_account_fetch(
				account_name ) ) )
	{
		free( liability_entity );
		return (LIABILITY_ENTITY *)0;
	}

	liability_entity->amount_due =
		liability_entity_amount_due(
			liability_entity->liability,
			(RECEIVABLE *)0 /* receivable */ );

	return liability_entity;
}

LIABILITY_ENTITY *liability_entity_account_list_new(
			LIST *account_current_liability_name_list,
			LIST *account_receivable_name_list,
			ENTITY *entity )
{
	LIABILITY_ENTITY *liability_entity;

	if ( !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: entity is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( account_current_liability_name_list ) )
		return (LIABILITY_ENTITY *)0;

	liability_entity = liability_entity_calloc();
	liability_entity->entity = entity;

	if ( ! ( liability_entity->liability =
			liability_entity_fetch(
				entity->full_name,
				entity->street_address,
				account_current_liability_name_list ) ) )
	{
		free( liability_entity );
		return (LIABILITY_ENTITY *)0;
	}

	if ( list_length( account_receivable_name_list ) )
	{
		liability_entity->receivable =
			receivable_fetch(
				entity->full_name,
				entity->street_address,
				account_receivable_name_list );
	}

	liability_entity->amount_due =
		liability_entity_amount_due(
			liability_entity->liability,
			liability_entity->receivable );

	return liability_entity;
}

double liability_entity_amount_due(
			LIABILITY *liability,
			RECEIVABLE *receivable )
{
	double amount_due;

	if ( !liability ) return 0.0;

	if ( ! ( amount_due = liability->journal_credit_debit_difference_sum ) )
	{
		return 0.0;
	}

	if ( receivable )
	{
		amount_due -= receivable->journal_debit_credit_difference_sum;
	}

	return amount_due;
}

LIABILITY_ENTITY *liability_entity_calloc( void )
{
	LIABILITY_ENTITY *liability_entity;

	if ( ! ( liability_entity = calloc( 1, sizeof ( LIABILITY_ENTITY ) ) ) )
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
		char *dialog_box_memo,
		char *data_directory,
		char *process_name,
		char *session_key,
		LIST *entity_full_street_list )
{
	LIABILITY_PAYMENT *liability_payment;

	if ( !application_name
	||   !data_directory
	||   !process_name
	||   !session_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	liability_payment = liability_payment_calloc();

	if ( !list_length( entity_full_street_list ) )
	{
		liability_payment->error_message =
			liability_payment_error_message(
				"Please select an entity." );

		return liability_payment;
	}

	liability_payment->transaction_memo =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_memo( dialog_box_memo );

	if ( list_length( entity_full_street_list ) > 1
	&& (  dialog_box_payment_amount
	||    *liability_payment->transaction_memo ) )
	{
		liability_payment->error_message =
			liability_payment_error_message(
			"Please choose only one entity for this option." );

		return liability_payment;
	}

	liability_payment->liability_calculate =
		liability_calculate_new(
			application_name );

	if ( !liability_payment->liability_calculate )
	{
		liability_payment->error_message =
			liability_payment_error_message(
			"No liabilities due." );

		return liability_payment;
	}

	liability_payment->entity_list =
		liability_payment_entity_list(
			entity_full_street_list,
			liability_payment->
				liability_calculate->
				liability_entity_list );

	if ( !list_length( liability_payment->entity_list ) )
	{
		liability_payment->error_message =
			liability_payment_error_message(
				"Could not fulfill the liabilities." );

		return liability_payment;
	}

	if ( starting_check_number )
	{
		liability_payment->check_list =
			check_list_new(
				application_name,
				dialog_box_payment_amount,
				starting_check_number,
				liability_payment->transaction_memo,
				data_directory,
				process_name,
				session_key,
				liability_payment->entity_list );
	}

	liability_payment->credit_account_name =
		liability_payment_credit_account_name(
			starting_check_number,
			account_cash(
				ACCOUNT_CASH_KEY ),
			account_uncleared_checks(
				ACCOUNT_UNCLEARED_CHECKS_KEY ) );

	liability_payment->liability_transaction_list =
		liability_transaction_list_new(
			dialog_box_payment_amount,
			starting_check_number,
			liability_payment->transaction_memo,
			liability_payment->entity_list,
			liability_payment->credit_account_name );

	if ( !liability_payment->liability_transaction_list )
	{
		liability_payment->error_message =
			liability_payment_error_message(
				"Could not generate the transactions." );

		return liability_payment;
	}

	return liability_payment;
}

LIABILITY_PAYMENT *liability_payment_calloc( void )
{
	LIABILITY_PAYMENT *liability_payment;

	if ( ! ( liability_payment =
			calloc( 1, sizeof ( LIABILITY_PAYMENT ) ) ) )
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

LIABILITY_ACCOUNT *liability_account_getset(
			LIST *list,
			char *account_name )
{
	LIABILITY_ACCOUNT *liability_account;

	if ( !list
	||   !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( list ) )
	{
		liability_account =
			liability_account_new(
				account_name );

		list_set(
			list,
			liability_account );

		return liability_account;
	}

	do {
		liability_account = list_get( list );

		if ( strcmp(
			account_name,
			liability_account->account_name ) == 0 )
		{
			return liability_account;
		}

	} while ( list_next( list ) );

	liability_account =
		liability_account_new(
			account_name );

	list_set(
		list,
		liability_account );

	return liability_account;
}

LIABILITY_ACCOUNT *liability_account_new( char *account_name )
{
	LIABILITY_ACCOUNT *liability_account;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_account = liability_account_calloc();
	liability_account->account_name = account_name;

	return liability_account;
}

LIABILITY_ACCOUNT *liability_account_calloc( void )
{
	LIABILITY_ACCOUNT *liability_account;

	if ( ! ( liability_account =
			calloc( 1, sizeof ( LIABILITY_ACCOUNT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_account;
}

LIABILITY_ACCOUNT_LIST *liability_account_list_new(
			LIST *journal_system_list )
{
	LIABILITY_ACCOUNT_LIST *liability_account_list;
	JOURNAL *journal;
	LIABILITY_ACCOUNT *liability_account;

	if ( !list_rewind( journal_system_list ) )
	{
		return (LIABILITY_ACCOUNT_LIST *)0;
	}

	liability_account_list = liability_account_list_calloc();
	liability_account_list->list = list_new();

	do {
		journal = list_get( journal_system_list );

		liability_account =
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			liability_account_getset(
				liability_account_list->list,
				journal->account_name );

		if ( journal->credit_amount )
		{
			liability_account->credit_amount +=
				journal->credit_amount;
		}
		else
		{
			liability_account->credit_amount -=
				journal->debit_amount;
		}

	} while( list_next( journal_system_list ) );

	return liability_account_list;
}

LIABILITY_ACCOUNT_LIST *
	liability_account_list_calloc(
			void )
{
	LIABILITY_ACCOUNT_LIST *liability_account_list;

	if ( ! ( liability_account_list =
			calloc( 1,
				sizeof ( LIABILITY_ACCOUNT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_account_list;
}

LIABILITY_CALCULATE *liability_calculate_new( char *application_name )
{
	LIABILITY_CALCULATE *liability_calculate;

	if ( !application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_calculate = liability_calculate_calloc();

	liability_calculate->liability_account_entity_list =
		liability_account_entity_list();

	liability_calculate->exclude_account_name_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		liability_account_entity_account_name_list(
			liability_calculate->
				liability_account_entity_list );

	list_set(
		liability_calculate->exclude_account_name_list,
		ACCOUNT_UNCLEARED_CHECKS );

	liability_calculate->account_current_liability_name_list =
		account_current_liability_name_list(
			ACCOUNT_TABLE,
			SUBCLASSIFICATION_CURRENT_LIABILITY,
			ACCOUNT_CREDIT_CARD_KEY,
			liability_calculate->exclude_account_name_list );

	liability_calculate->journal_account_distinct_entity_list =
		journal_account_distinct_entity_list(
			JOURNAL_TABLE,
			liability_calculate->
				account_current_liability_name_list
					/* account_name_list */ );

	liability_calculate->account_receivable_name_list =
		account_receivable_name_list(
			ACCOUNT_TABLE,
			SUBCLASSIFICATION_RECEIVABLE );

	if ( ! ( liability_calculate->entity_self =
			entity_self_fetch(
				0 /* not fetch_entity_boolean */ ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_self_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_calculate->liability_entity_list = list_new();

	if ( list_length(
		liability_calculate->
			liability_account_entity_list ) )
	{
		list_set_list(
			liability_calculate->liability_entity_list,
			liability_entity_list_account(
				liability_calculate->
					liability_account_entity_list ) );
	}

	if ( list_length(
		liability_calculate->
			journal_account_distinct_entity_list ) )
	{
		list_set_list(
			liability_calculate->liability_entity_list,
			liability_entity_list_entity(
				liability_calculate->
					account_current_liability_name_list,
				liability_calculate->
					journal_account_distinct_entity_list,
				liability_calculate->
					account_receivable_name_list,
				liability_calculate->
					entity_self ) );
	}

	if ( !list_length( liability_calculate->liability_entity_list ) )
	{
		free( liability_calculate );
		return (LIABILITY_CALCULATE *)0;
	}
	else
	{
		return liability_calculate;
	}
}

LIABILITY_CALCULATE *liability_calculate_calloc( void )
{
	LIABILITY_CALCULATE *liability_calculate;

	if ( ! ( liability_calculate =
			calloc( 1, sizeof ( LIABILITY_CALCULATE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_calculate;
}

LIABILITY_JOURNAL_LIST *liability_journal_list_new(
		double dialog_box_payment_amount,
		char *liability_payment_credit_account_name,
		LIABILITY_ENTITY *liability_entity )
{
	LIABILITY_JOURNAL_LIST *liability_journal_list;
	LIABILITY_ACCOUNT *liability_account;

	if ( !liability_payment_credit_account_name
	||   !liability_entity
	||   !liability_entity->liability
	||   !liability_entity->liability->liability_account_list
	||   !list_rewind(
		liability_entity->
			liability->
			liability_account_list->
			list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_journal_list = liability_journal_list_calloc();
	liability_journal_list->journal_list = list_new();

	liability_journal_list->transaction_amount =
		liability_journal_list_transaction_amount(
			dialog_box_payment_amount,
			liability_entity->amount_due );

	if ( !liability_journal_list->transaction_amount )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: liability_journal_list->transaction_amount is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set(
		liability_journal_list->journal_list,
		journal_credit_new(
			liability_payment_credit_account_name,
			liability_journal_list->
				transaction_amount ) );

	do {
		liability_account =
			list_get(
				liability_entity->
					liability->
					liability_account_list->
					list );

		if ( !liability_account->credit_amount ) continue;

		list_set(
			liability_journal_list->journal_list,
			journal_debit_new(
				liability_account->account_name,
				(dialog_box_payment_amount)
					? dialog_box_payment_amount
					: liability_account->credit_amount ) );

		if ( liability_entity->receivable
		&&   liability_entity->receivable->receivable_account_list
		&&   list_rewind(
			liability_entity->
				receivable->
				receivable_account_list->
				list ) )
		{
			RECEIVABLE_ACCOUNT *receivable_account;

			do {
				receivable_account =
					list_get(
						liability_entity->
						       receivable->
						       receivable_account_list->
						       list );

				list_set(
					liability_journal_list->journal_list,
					journal_credit_new(
						receivable_account->
							account_name,
						receivable_account->
							debit_amount ) );

			} while ( list_next(
					liability_entity->
						receivable->
						receivable_account_list->
						list ) );
		}

	} while ( list_next(
			liability_entity->
				liability->
				liability_account_list->
				list ) );

	return liability_journal_list;
}

LIABILITY_JOURNAL_LIST *liability_journal_list_calloc( void )
{
	LIABILITY_JOURNAL_LIST *liability_journal_list;

	if ( ! ( liability_journal_list =
			calloc( 1, sizeof ( LIABILITY_JOURNAL_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_journal_list;
}

double liability_journal_list_transaction_amount(
		double dialog_box_payment_amount,
		double liability_entity_amount_due )
{
	if ( dialog_box_payment_amount )
		return dialog_box_payment_amount;
	else
		return liability_entity_amount_due;
}

LIABILITY_TRANSACTION_LIST *
	liability_transaction_list_new(
		double dialog_box_payment_amount,
		int starting_check_number,
		char *transaction_memo,
		LIST *liability_payment_entity_list,
		char *credit_account_name )
{
	LIABILITY_TRANSACTION_LIST *liability_transaction_list;
	LIABILITY_ENTITY *liability_entity;

	if ( dialog_box_payment_amount
	&& list_length( liability_payment_entity_list ) > 1 )
	{
		return (LIABILITY_TRANSACTION_LIST *)0;
	}

	if ( !list_rewind( liability_payment_entity_list ) )
	{
		return (LIABILITY_TRANSACTION_LIST *)0;
	}

	liability_transaction_list = liability_transaction_list_calloc();

	liability_transaction_list->list = list_new();

	liability_transaction_list->transaction_date_time =
		date_now_new(
			date_utc_offset() );

	do {
		liability_entity = list_get( liability_payment_entity_list );

		list_set(
			liability_transaction_list->list,
			liability_transaction_new(
				dialog_box_payment_amount,
				starting_check_number
					/* check_number */,
				transaction_memo,
				credit_account_name,
				liability_entity,
				liability_transaction_list->
					transaction_date_time ) );

		if ( starting_check_number )
		{
			starting_check_number++;
		}

		date_increment_seconds(
			liability_transaction_list->transaction_date_time,
			1 );

	} while ( list_next( liability_payment_entity_list ) );

	return liability_transaction_list;
}

LIABILITY_TRANSACTION_LIST *
	liability_transaction_list_calloc(
		void )
{
	LIABILITY_TRANSACTION_LIST *liability_transaction_list;

	if ( ! ( liability_transaction_list =
			calloc( 1, sizeof ( LIABILITY_TRANSACTION_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_transaction_list;
}

LIABILITY_TRANSACTION *liability_transaction_new(
		double dialog_box_payment_amount,
		int check_number,
		char *transaction_memo,
		char *credit_account_name,
		LIABILITY_ENTITY *liability_entity,
		DATE *transaction_date_time )
{
	LIABILITY_TRANSACTION *liability_transaction;

	if ( !transaction_memo
	||   !credit_account_name
	||   !liability_entity
	||   !liability_entity->entity
	||   !transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	liability_transaction = liability_transaction_calloc();

	liability_transaction->liability_journal_list =
		liability_journal_list_new(
			dialog_box_payment_amount,
			credit_account_name,
			liability_entity );

	if ( !liability_transaction->liability_journal_list
	||   !liability_transaction->
		liability_journal_list->
		transaction_amount )
	{
		free( liability_transaction );
		return (LIABILITY_TRANSACTION *)0;
	}

	liability_transaction->transaction =
		transaction_entity_new(
			liability_entity->entity,
			date_display_19( transaction_date_time ),
			liability_transaction->
				liability_journal_list->
				transaction_amount,
			check_number,
			(*transaction_memo)
				? transaction_memo
				: LIABILITY_TRANSACTION_MEMO,
			liability_transaction->
				liability_journal_list->
				journal_list );

	return liability_transaction;
}

LIABILITY_TRANSACTION *liability_transaction_calloc( void )
{
	LIABILITY_TRANSACTION *liability_transaction;

	if ( ! ( liability_transaction =
			calloc( 1, sizeof ( LIABILITY_TRANSACTION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return liability_transaction;
}

void liability_transaction_list_insert(
		LIABILITY_TRANSACTION_LIST *
			liability_transaction_list )
{
	if ( !liability_transaction_list )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: liability_transaction_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( liability_transaction_list->list ) ) return;

	/* May reset transaction_date_time */
	/* ------------------------------- */
	transaction_list_insert(
		liability_transaction_list_extract(
			liability_transaction_list->list ),
		1 /* insert_journal_list_boolean */,
		0 /* not transaction_lock_boolean */ );
}

LIST *liability_transaction_list_extract( LIST *list )
{
	LIST *transaction_list;
	LIABILITY_TRANSACTION *liability_transaction;

	if ( !list_rewind( list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		liability_transaction = list_get( list );

		list_set(
			transaction_list,
			liability_transaction->transaction );

	} while ( list_next( list ) );

	return transaction_list;
}

void liability_transaction_list_html_display(
		LIABILITY_TRANSACTION_LIST *
			liability_transaction_list )
{
	if ( !liability_transaction_list )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: liability_transaction_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( liability_transaction_list->list ) ) return;

	transaction_list_html_display(
		liability_transaction_list_extract(
			liability_transaction_list->list ) );
}

void liability_calculate_stdout( LIST *liability_entity_list )
{
	LIABILITY_ENTITY *liability_entity;

	if ( !list_rewind( liability_entity_list ) ) return;

	do {
		liability_entity =
			list_get(
				liability_entity_list );

		if ( !float_virtually_same(
				liability_entity->amount_due,
				0.0 )
		&&   liability_entity->amount_due > 0.0 )
		{
			printf(	"%s\n",
				liability_entity_display(
					liability_entity ) );
		}

	} while ( list_next( liability_entity_list ) );
}

char *liability_entity_display( LIABILITY_ENTITY *liability_entity )
{
	static char display[ 128 ];

	if ( !liability_entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: liability_entity is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(display,
		"%s^%s [%.2lf]",
		liability_entity->
			entity->
			full_name,
		liability_entity->
			entity->
			street_address,
		liability_entity->amount_due );

	return display;
}

char *liability_payment_error_message( char *message )
{
	return message;
}

LIST *liability_entity_list_account(
	LIST *liability_account_entity_list )
{
	LIST *liability_entity_list = list_new();
	LIABILITY_ACCOUNT_ENTITY *liability_account_entity;
	LIABILITY_ENTITY *liability_entity;

	if ( list_rewind( liability_account_entity_list ) )
	do {
		liability_account_entity =
			list_get(
				liability_account_entity_list );

		if ( !liability_account_entity->account_name
		||   !liability_account_entity->entity )
		{
			continue;
		}

		liability_entity =
			liability_entity_account_name_new(
				liability_account_entity->account_name,
				liability_account_entity->entity );

		if ( liability_entity )
		{
			list_set(
				liability_entity_list,
				liability_entity );
		}

	} while ( list_next( liability_account_entity_list ) );

	if ( !list_length( liability_entity_list ) )
	{
		list_free( liability_entity_list );
		liability_entity_list = NULL;
	}

	return liability_entity_list;
}

LIST *liability_entity_list_entity(
		LIST *account_current_liability_name_list,
		LIST *journal_account_distinct_entity_list,
		LIST *account_receivable_name_list,
		ENTITY_SELF *entity_self )
{
	ENTITY *entity;
	LIABILITY_ENTITY *liability_entity;
	LIST *liability_entity_list;

	if ( !entity_self )
	{
		char message[ 128 ];

		sprintf(message, "entity_self is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( account_current_liability_name_list )
	||   !list_rewind( journal_account_distinct_entity_list ) )
	{
		return (LIST *)0;
	}

	liability_entity_list = list_new();

	do {
		entity =
			list_get(
				journal_account_distinct_entity_list );

		if ( strcmp(
			entity->full_name,
			entity_self->full_name ) == 0
		&&   strcmp(
			entity->street_address,
			entity_self->street_address ) == 0 )
		{
			continue;
		}

		if ( ( liability_entity =
				liability_entity_account_list_new(
					account_current_liability_name_list,
					account_receivable_name_list,
					entity ) ) )
		{
			list_set( liability_entity_list, liability_entity );
		}

	} while ( list_next( journal_account_distinct_entity_list ) );

	return liability_entity_list;
}

LIABILITY_ENTITY *liability_entity_seek(
		char *full_name,
		char *street_address,
		LIST *liability_entity_list )
{
	LIABILITY_ENTITY *liability_entity;

	if ( list_rewind( liability_entity_list ) )
	do {
		liability_entity =
			list_get(
				liability_entity_list );

		if ( !liability_entity->entity )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: liability_entity->entity is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(
			full_name,
			liability_entity->entity->full_name ) == 0
		&&   strcmp(
			street_address,
			liability_entity->entity->street_address ) == 0 )
		{
			return liability_entity;
		}

	} while ( list_next( liability_entity_list ) );

	return (LIABILITY_ENTITY *)0;
}

LIST *liability_payment_entity_list(
		LIST *entity_full_street_list,
		LIST *liability_entity_list )
{
	ENTITY *entity;
	LIABILITY_ENTITY *liability_entity;
	LIST *entity_list = list_new();

	if ( list_rewind( entity_full_street_list ) )
	do {
		entity = list_get( entity_full_street_list );

		if ( ! ( liability_entity =
				liability_entity_seek(
					entity->full_name,
					entity->street_address,
					liability_entity_list ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: liability_entity_seek(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				entity->full_name );
			exit( 1 );
		}

		list_set(
			entity_list,
			liability_entity );

	} while ( list_next( entity_full_street_list ) );

	if ( !list_length( entity_list ) )
	{
		list_free( entity_list );
		entity_list = NULL;
	}

	return entity_list;
}
