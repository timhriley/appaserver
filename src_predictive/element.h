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
	/* Input */
	/* ----- */
	char *element_name;
	boolean accumulate_debit;

	/* Process */
	/* ------- */
	LIST *element_subclassification_list;
} ELEMENT;

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

ELEMENT *element_parse( char *input );

ELEMENT *element_new(	char *element_name ); 

ELEMENT *element_calloc(
			void );


/* Usage */
/* ----- */
LIST *element_subclassification_list(
			char *element_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time );

/* Usage */
/* ----- */
LIST *element_account_list(
			LIST *subclassification_list );

#endif

