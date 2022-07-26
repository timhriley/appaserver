/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/element.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef ELEMENT_H
#define ELEMENT_H

#include "list.h"
#include "boolean.h"

#define ELEMENT_TABLE			"element"

#define ELEMENT_SELECT			"element,accumulate_debit_yn"

#define ELEMENT_ASSET			"asset"
#define ELEMENT_LIABILITY		"liability"
#define ELEMENT_EQUITY			"equity"
#define ELEMENT_REVENUE			"revenue"
#define ELEMENT_EXPENSE			"expense"
#define ELEMENT_GAIN			"gain"
#define ELEMENT_LOSS			"loss"

typedef struct
{
	char *element_name;
	boolean accumulate_debit;
	LIST *subclassification_statement_list;
	LIST *account_statement_list;
	double sum;
} ELEMENT;

/* Usage */
/* ----- */
LIST *element_statement_list(
			LIST *filter_element_name_list,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_journal_latest,
			boolean fetch_memo );

/* Process */
/* ------- */

/* Usage */
/* ----- */
ELEMENT *element_fetch(	char *element_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *element_primary_where(
			char *element_name );

/* Returns heap memory */
/* ------------------- */
char *element_system_string(
			char *element_select,
			char *element_table,
			char *where );

FILE *element_pipe(
			char *element_system_string );


/* Usage */
/* ----- */
ELEMENT *element_parse( char *input );

/* Process */
/* ------- */
ELEMENT *element_new(	char *element_name );

ELEMENT *element_calloc(
			void );

/* Usage */
/* ----- */
ELEMENT *element_statement_fetch(
			char *element_name,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_journal_latest,
			boolean fetch_memo );

/* Process */
/* ------- */

/* Usage */
/* ----- */
ELEMENT *element_statement_parse(
			char *input,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_journal_latest,
			boolean fetch_memo );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double element_sum(	ELEMENT *element );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void element_account_statement_list_set(
			ELEMENT *element );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void element_prior_year_set(
			ELEMENT *prior_element /* in/out */,
			ELEMENT *current_element );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void element_list_account_statement_list_set(
			LIST *element_statement_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void element_account_transaction_count_set(
			LIST *element_statement_list,
			char *transaction_begin_date_string,
			char *transaction_date_time_closing );

/* Process */
/* ------- */

/* Public */
/* ------ */
boolean element_is_nominal(
			char *element_name );


ELEMENT *element_seek(	char *element_name,
			LIST *element_statement_list );

double element_list_debit_sum(
			LIST *element_statement_list );

double element_list_credit_sum(
			LIST *element_statement_list );

LIST *element_list_non_nominal_account_list(
			LIST *element_statement_list );

LIST *element_list_nominal_account_list(
			LIST *element_statement_list );

#endif

