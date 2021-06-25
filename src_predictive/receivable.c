/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/receivable.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "boolean.h"
#include "account.h"
#include "receivable.h"

RECEIVABLE *receivable_new(
			char *full_name,
			char *street_address )
{
	RECEIVABLE *receivable;

	if ( ! ( receivable = calloc( 1, sizeof( RECEIVABLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	receivable->full_name = full_name;
	receivable->street_address = street_address;

	return receivable;
}

RECEIVABLE *receivable_steady_state(
			RECEIVABLE *receivable )
{
	receivable->receivable_entity_account_list =
		/* ------------------------------------ */
		/* Sets account->account_receivable_due */
		/* ------------------------------------ */
		receivable_entity_account_list(
			receivable->full_name,
			receivable->street_address );

	receivable->receivable_expecting =
		receivable_expecting(
			receivable->
				receivable_entity_account_list );

	return receivable;
}

LIST *receivable_entity_account_list(
			char *full_name,
			char *street_address )
{
	char *where;
	char system_string[ 1024 ];
	LIST *receivable_account_list;
	LIST *return_account_list = {0};
	ACCOUNT *account;

	where = "subclassification = 'receivable'";

	sprintf( system_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 account_select(),
		 ACCOUNT_TABLE_NAME,
		 where,
		 "account" );

	receivable_account_list = account_system_list( system_string );

	if ( !list_rewind( receivable_account_list ) ) return (LIST *)0;

	do {
		account = list_get( receivable_account_list );

		account->journal_list =
			journal_entity_account_journal_list(
				account->account_name,
				full_name,
				street_address );

		if ( list_length(
			account->journal_list ) )
		{
			account->account_receivable_due =
				journal_debit_difference_sum(
					account->journal_list );

			if ( account->account_receivable_due )
			{
				if ( !return_account_list )
					return_account_list = list_new();

				list_set( return_account_list, account );
			}
		}

	} while ( list_next( receivable_account_list ) );

	return return_account_list;
}

double receivable_expecting(
	LIST *receivable_entity_account_list )
{
	LIST *account_list;
	ACCOUNT *account;
	double expecting;

	account_list = receivable_entity_account_list;

	if ( !list_rewind( account_list ) ) return 0.0;

	expecting = 0.0;

	do {
		account = list_get( account_list );

		expecting += account->account_receivable_due;

	} while ( list_next( account_list ) );

	return expecting;
}

