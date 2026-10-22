/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/contra_account.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"
#include "account_journal.h"

#define CONTRA_ACCOUNT_SELECT	"account,"		\
				"contra_to_account"

#define CONTRA_ACCOUNT_TABLE	"contra_account"

typedef struct
{
	LIST *list;
	LIST *account_name_list;
	double balance_total;
	char *net_label;
} CONTRA_ACCOUNT_LIST;

/* Usage */
/* ----- */
CONTRA_ACCOUNT_LIST *contra_account_list_new(
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *fund_name,
		char *end_date_time_string,
		LIST *contra_account_fetch_list
			/* Sets each matching account_journal_latest */,
		char *contra_to_account );

/* Process */
/* ------- */
CONTRA_ACCOUNT_LIST *contra_account_list_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *contra_account_list_net_label(
		char *contra_to_account,
		LIST *account_name_list );

typedef struct
{
	char *account_name;
	char *contra_to_account;
	ACCOUNT_JOURNAL *account_journal_latest;
} CONTRA_ACCOUNT;

/* Usage */
/* ----- */
LIST *contra_account_fetch_list(
		const char *contra_account_select,
		const char *contra_account_table );

/* Usage */
/* ----- */
CONTRA_ACCOUNT *contra_account_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CONTRA_ACCOUNT *contra_account_new(
		char *account_name,
		char *contra_to_account );

/* Process */
/* ------- */
CONTRA_ACCOUNT *contra_account_calloc(
		void );

/* Usage */
/* ----- */
CONTRA_ACCOUNT *contra_account_seek(
		LIST *contra_account_list,
		char *account_name );

/* Usage */
/* ----- */
boolean contra_account_boolean(
		LIST *contra_account_list,
		char *account_name );

/* Usage */
/* ----- */
LIST *contra_account_insert_statement_list(
		const char *insert_column_list_string,
		const char *contra_account_table,
		LIST *account_name_list,
		char *contra_to_account );

