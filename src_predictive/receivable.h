/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/receivable.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef RECEIVABLE_H
#define RECEIVABLE_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *timlib_in_clause;
	char *where;
	LIST *journal_system_list;
	double journal_debit_credit_difference_sum;
} RECEIVABLE;

/* Usage */
/* ----- */
RECEIVABLE *receivable_fetch(
			char *full_name,
			char *street_address,
			LIST *account_receivable_name_list );

/* Process */
/* ------- */
RECEIVABLE *receivable_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *receivable_where(
			char *full_name,
			char *street_address,
			char *timlib_in_clause );

#endif
