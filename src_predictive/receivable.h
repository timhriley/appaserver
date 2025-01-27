/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/receivable.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RECEIVABLE_H
#define RECEIVABLE_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *account_name;
	double debit_amount;
} RECEIVABLE_ACCOUNT;

/* Usage */
/* ----- */
RECEIVABLE_ACCOUNT *receivable_account_getset(
		LIST *list,
		char *account_name );

/* Usage */
/* ------ */

/* Safely returns */
/* -------------- */
RECEIVABLE_ACCOUNT *receivable_account_new(
		char *account_name );

/* Process */
/* ------- */
RECEIVABLE_ACCOUNT *receivable_account_calloc(
		void );

typedef struct
{
	LIST *list;
} RECEIVABLE_ACCOUNT_LIST;

/* Usage */
/* ----- */
RECEIVABLE_ACCOUNT_LIST *receivable_account_list_new(
		LIST *journal_system_list );

/* Process */
/* ------- */
RECEIVABLE_ACCOUNT_LIST *receivable_account_list_calloc(
		void );

typedef struct
{
	LIST *journal_system_list;
	double journal_debit_credit_difference_sum;
	RECEIVABLE_ACCOUNT_LIST *receivable_account_list;
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

/* Returns heap memory */
/* ------------------- */
char *receivable_where(
		char *full_name,
		char *street_address,
		char *timlib_in_clause );

#endif
