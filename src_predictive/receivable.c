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
#include "journal.h"
#include "receivable.h"

RECEIVABLE *receivable_calloc( void )
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

	return receivable;
}

RECEIVABLE *receivable_fetch(
			char *full_name,
			char *street_address,
			LIST *account_receivable_name_list )
{
	RECEIVABLE *receivable = receivable_calloc();

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

	receivable->timlib_in_clause =
		timlib_in_clause(
			account_receivable_name_list );

	receivable->where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		receivable_where(
			full_name,
			street_address,
			receivable->timlib_in_clause );

	receivable->journal_system_list =
		journal_system_list(
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				receivable->where ),
		0 /* not fetch_check_number */,
		0 /* not fetch_memo */ );

	receivable->journal_debit_credit_difference_sum =
		journal_debit_credit_difference_sum(
			receivable->journal_system_list );

	return receivable;
}

char *receivable_where(
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

