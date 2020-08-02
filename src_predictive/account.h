/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/account.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "list.h"
#include "boolean.h"
#include "journal.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *account_name;
	char *fund_name;
	char *subclassification_name;
	char *hard_coded_account_key;
	JOURNAL *latest_ledger;
	boolean accumulate_debit;
	double balance;
	LIST *journal_ledger_list;
} ACCOUNT;

typedef struct
{
	char *subclassification_name;
	LIST *account_list;
	double subclassification_total;
} SUBCLASSIFICATION;

typedef struct
{
	char *element_name;
	boolean accumulate_debit;

	/* -------------------------- */
	/* if !omit_subclassification */
	/* -------------------------- */
	LIST *subclassification_list;

	/* ------------------------- */
	/* if omit_subclassification */
	/* ------------------------- */
	LIST *account_list;

	double element_total;
} ELEMENT;

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
char *account_escape_name(	char *account_name );

ACCOUNT *account_fetch(		char *account_name );

ACCOUNT *account_new(		char *account_name );

SUBCLASSIFICATION *subclassification_new(
				char *subclassification_name );

ELEMENT *element_new(		char *element_name ); 

ELEMENT *element_seek(		LIST *element_list,
				char *element_name );

#endif
