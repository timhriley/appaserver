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
			liability_account_entity_sys_string(
				"1 = 1" /* where */ ) );
}

char *liability_account_entity_sys_string(
			char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" none",
		 "liability_account_entity",
		 where );

	return strdup( sys_string );
}

LIST *liability_account_entity_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *liability_account_entity_list;

	if ( !sys_string ) return (LIST *)0;

	liability_account_entity_list = list_new();

	input_pipe = popen( sys_string, "r" );

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

LIST *liability_current_account_list(
			LIST *liability_account_entity_list )
{
	char where[ 256 ];
	char sys_string[ 1024 ];
	LIST *entire_account_list;
	LIST *return_account_list;
	ACCOUNT *account;

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
			account->liability_account_entity =
				liability_account_entity(
					liability_account_entity_list,
					account->account_name );

			list_set( return_account_list, account );
		}

	} while ( list_next( entire_account_list ) );

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

LIST *liability_entity_list( LIST *account_list )
{
	ACCOUNT *account;
	LIST *journal_list;
	JOURNAL *journal;
	LIST *entity_list;
	ENTITY *entity;

	if ( !list_rewind( account_list ) ) return (LIST *)0;

	entity_list = list_new();

	do {
		account = list_get( account_list );

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

			if ( account->liability_account_entity )
			{
				entity =
					entity_getset(
						entity_list,
						account->
						     liability_account_entity->
						     full_name,
						account->
						     liability_account_entity->
						     street_address,
						0 /* not with_strdup */ );
			}
			else
			{
				entity =
					entity_getset(
						entity_list,
						journal->full_name,
						journal->street_address,
						0 /* not with_strdup */ );
			}

			if ( !entity->liability_journal_list )
			{
				entity->liability_journal_list = list_new();
			}

			list_set( entity->liability_journal_list, journal );

		} while( list_next( journal_list ) );

	} while( list_next( account_list ) );

	return entity_list;
}

double liability_entity_amount_due( LIST *journal_list )
{
	JOURNAL *journal;
	double amount_due;
	double difference;

	if ( !list_rewind( journal_list ) ) return 0.0;

	amount_due = 0.0;

	do {
		journal = list_get( journal_list );

		difference =	journal->credit_amount -
				journal->debit_amount;

		amount_due += difference;

	} while ( list_next( journal_list ) );

	return amount_due;
}

