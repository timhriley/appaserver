/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_equity.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CLOSE_EQUITY_H
#define CLOSE_EQUITY_H

#include "list.h"
#include "boolean.h"
#include "equity_journal.h"

#define CLOSE_EQUITY_TABLE		"close_equity"

#define CLOSE_EQUITY_SELECT		"account,"		\
					"equity_account,"	\
					"reverse_yn"

#define CLOSE_EQUITY_PRIMARY_KEY	"account"

typedef struct
{
	char *account_name;
	boolean reverse_boolean;
	EQUITY_JOURNAL *equity_journal;
} CLOSE_EQUITY;

/* Usage */
/* ----- */
LIST *close_equity_list(
		const char *close_equity_table,
		const char *close_equity_select,
		const char *close_equity_primary_key,
		LIST *equity_journal_list );

/* Usage */
/* ----- */
CLOSE_EQUITY *close_equity_parse(
		LIST *equity_journal_list,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_EQUITY *close_equity_new(
		char *account_name,
		boolean reverse_boolean );

/* Process */
/* ------- */
CLOSE_EQUITY *close_equity_calloc(
		void );

/* Usage */
/* ----- */
CLOSE_EQUITY *close_equity_seek(
		LIST *close_equity_list,
		char *account_name );

#endif
