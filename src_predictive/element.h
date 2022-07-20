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
#include "account.h"

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
	/* Attributes */
	/* ---------- */
	char *element_name;
	boolean accumulate_debit;
	LIST *subclassification_statement_list;
	double value;
} ELEMENT;

/* Usage */
/* ----- */
LIST *element_statement_list(
			LIST *filter_element_name_list,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_latest_journal );

/* Process */
/* ------- */
LIST *element_list_sort(
			LIST *element_list );

/* Usage */
/* ----- */
ELEMENT *element_fetch(	char *element_name );

/* Process */
/* ------- */

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
			boolean fetch_latest_journal );

/* Process */
/* ------- */
ELEMENT *element_statement_parse(
			char *input,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_latest_journal );

double element_value(	ELEMENT *element );

/* Private */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *element_system_string(
			char *element_select,
			char *element_table,
			char *where );

FILE *element_input_pipe(
			char *element_system_string );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *element_primary_where(
			char *element_name );

boolean element_is_nominal(
			char *element_name );


LIST *element_account_list(
			ELEMENT *element );

LIST *element_seek(	char *element_name,
			LIST *element_statement_list );

#endif

