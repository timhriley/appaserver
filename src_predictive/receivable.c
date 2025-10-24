/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/receivable.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "appaserver_error.h"
#include "journal.h"
#include "receivable.h"

RECEIVABLE *receivable_calloc( void )
{
	RECEIVABLE *receivable;

	if ( ! ( receivable = calloc( 1, sizeof ( RECEIVABLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return receivable;
}

RECEIVABLE *receivable_fetch(
		char *full_name,
		char *street_address,
		LIST *account_receivable_name_list )
{
	RECEIVABLE *receivable;
	char *in_clause;
	char *where;
	char *system_string;

	if ( !full_name
	||   !street_address
	||   !list_length( account_receivable_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	receivable = receivable_calloc();

	in_clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_in_clause(
			account_receivable_name_list
				/* data_list */ );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		receivable_where(
			full_name,
			street_address,
			in_clause );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_system_string(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			where );

	receivable->journal_system_list =
		journal_system_list(
			(char *)0 /* fund_name */,
			system_string,
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	free( system_string );
	free( where );
	free( in_clause );

	if ( !list_length( receivable->journal_system_list ) )
	{
		free( receivable );
		return NULL;
	}

	receivable->journal_debit_credit_difference_sum =
		journal_debit_credit_difference_sum(
			receivable->journal_system_list );

	if ( !receivable->journal_debit_credit_difference_sum
	||   receivable->journal_debit_credit_difference_sum < 0.0 )
	{
		free( receivable );
		return NULL;
	}

	receivable->receivable_account_list =
		receivable_account_list_new(
			receivable->journal_system_list );

	return receivable;
}

char *receivable_where(
		char *full_name,
		char *street_address,
		char *string_in_clause )
{
	char where[ 65536 ];

	if ( !full_name
	||   !street_address
	||   !string_in_clause )
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
		"full_name = '%s' and "
		"street_address = '%s' and "
		"account in (%s)",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( full_name ),
		street_address,
		string_in_clause );

	return strdup( where );
}

RECEIVABLE_ACCOUNT *receivable_account_getset(
		LIST *list,
		char *account_name )
{
	RECEIVABLE_ACCOUNT *receivable_account;

	if ( !list
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

	if ( !list_rewind( list ) )
	{
		receivable_account =
			receivable_account_new(
				account_name );

		list_set( list, receivable_account );

		return receivable_account;
	}

	do {
		receivable_account = list_get( list );

		if ( strcmp(
			account_name,
			receivable_account->account_name ) == 0 )
		{
			return receivable_account;
		}
	} while ( list_next( list ) );

	receivable_account =
		receivable_account_new(
			account_name );

	list_set( list, receivable_account );

	return receivable_account;
}

RECEIVABLE_ACCOUNT *receivable_account_new( char *account_name )
{
	RECEIVABLE_ACCOUNT *receivable_account;

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

	receivable_account = receivable_account_calloc();
	receivable_account->account_name = account_name;

	return receivable_account;
}

RECEIVABLE_ACCOUNT *receivable_account_calloc( void )
{
	RECEIVABLE_ACCOUNT *receivable_account;

	if ( ! ( receivable_account =
			calloc( 1, sizeof ( RECEIVABLE_ACCOUNT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return receivable_account;
}

RECEIVABLE_ACCOUNT_LIST *receivable_account_list_new(
		LIST *journal_system_list )
{
	RECEIVABLE_ACCOUNT_LIST *receivable_account_list;
	JOURNAL *journal;
	RECEIVABLE_ACCOUNT *receivable_account;

	if ( !list_rewind( journal_system_list ) )
	{
		return (RECEIVABLE_ACCOUNT_LIST *)0;
	}

	receivable_account_list = receivable_account_list_calloc();
	receivable_account_list->list = list_new();

	do {
		journal = list_get( journal_system_list );

		receivable_account =
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			receivable_account_getset(
				receivable_account_list->list,
				journal->account_name );

		if ( journal->debit_amount )
		{
			receivable_account->debit_amount +=
				journal->debit_amount;
		}
		else
		{
			receivable_account->debit_amount -=
				journal->credit_amount;
		}

	} while ( list_next( journal_system_list ) );

	return receivable_account_list;
}

RECEIVABLE_ACCOUNT_LIST *receivable_account_list_calloc( void )
{
	RECEIVABLE_ACCOUNT_LIST *receivable_account_list;

	if ( ! ( receivable_account_list =
			calloc( 1, sizeof ( RECEIVABLE_ACCOUNT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return receivable_account_list;
}

