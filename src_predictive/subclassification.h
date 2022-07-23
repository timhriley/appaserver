/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/subclassification.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef SUBCLASSIFICATION_H
#define SUBCLASSIFICATION_H

#include "list.h"
#include "boolean.h"
#include "element.h"

#define SUBCLASSIFICATION_TABLE			"subclassification"

#define SUBCLASSIFICATION_SELECT		"subclassification,"	\
						"element,"		\
						"display_order"

#define SUBCLASSIFICATION_NET_ASSETS		"net_assets"
#define SUBCLASSIFICATION_CONTRIBUTED_CAPITAL	"contributed_capital"
#define SUBCLASSIFICATION_RETAINED_EARNINGS	"retained_earnings"
#define SUBCLASSIFICATION_DRAWING		"drawing"
#define SUBCLASSIFICATION_CURRENT_LIABILITY	"current_liability"
#define SUBCLASSIFICATION_RECEIVABLE		"receivable"
#define SUBCLASSIFICATION_CASH			"cash"

#define SUBCLASSIFICATION_NOTANUMBER		"NAN"

#define SUBCLASSIFICATION_CHANGE_IN_NET_ASSETS	"change_in_net_assets"

#define SUBCLASSIFICATION_BEGINNING_BALANCE_LABEL	\
						"Equity Beginning Balance"

#define SUBCLASSIFICATION_NET_ASSETS		"net_assets"

typedef struct
{
	char *subclassification_name;
	char *element_name;
	int display_order;
	ELEMENT *element;
	LIST *account_statement_list;
	double sum;
} SUBCLASSIFICATION;

/* Usage */
/* ----- */
LIST *subclassification_statement_list(
			char *element_primary_where,
			char *transaction_date_time_closing,
			boolean fetch_account_list,
			boolean fetch_journal_latest );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *subclassification_system_string(
			char *subclassification_select,
			char *subclassification_table,
			char *element_primary_where );

FILE *subclassification_pipe(
			char *subclassification_system_string );

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_statement_parse(
			char *input,
			char *transaction_date_time_closing,
			boolean fetch_account_list,
			boolean fetch_journal_latest );

/* Process */
/* ------- */

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_fetch(
			char *subclassification_name,
			boolean fetch_element );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *subclassification_primary_where(
			char *subclassification_name );

SUBCLASSIFICATION *subclassification_parse(
			char *input,
			boolean fetch_element );

SUBCLASSIFICATION *subclassification_new(
			char *subclassification_name );

SUBCLASSIFICATION *subclassification_calloc(
			void );

/* Usage */
/* ----- */
LIST *subclassification_account_list(
			LIST *subclassification_statement_list );

/* Process */
/* ------- */


/* Usage */
/* ----- */
double subclassification_list_sum(
			/* --------------------------- */
			/* Sets subclassification->sum */
			/* --------------------------- */
			LIST *subclassification_statement_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void subclassification_list_prior_year_set(
			LIST *prior_subclassification_list /* in/out */,
			LIST *current_subclassification_list );

/* Process */
/* ------- */
SUBCLASSIFICATION *subclassification_seek(
			char *subclassification_name,
			LIST *subclassification_list );

/* Usage */
/* ----- */
void subclassification_prior_year_set(
			SUBCLASSIFICATION *prior_subclassification /* in/out */,
			SUBCLASSIFICATION *current_subclassification );

/* Process */
/* ------- */

/* Public */
/* ------ */
double subclassification_list_debit_sum(
			LIST *subclassification_statement_list,
			boolean element_accumulate_debit );

double subclassification_list_credit_sum(
			LIST *subclassification_statement_list,
			boolean element_accumulate_debit );

#endif
