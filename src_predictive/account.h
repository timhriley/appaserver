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

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
char *account_escape_name(
			char *account_name );
char *account_name_escape(
			char *account_name );

ACCOUNT *account_fetch(	char *account_name );

ACCOUNT *account_new(	char *account_name );

boolean account_accumulate_debit(
			char *account_name );

#endif
