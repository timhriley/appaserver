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
	LIST *account_subclassification_account_name_list;
	LIST *account_list;
	LIST *account_balance_sort_list;
} SUBCLASSIFICATION;

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_fetch(
			char *subclassification_name,
			boolean fetch_element );

/* Returns static memory */
/* --------------------- */
char *subclassification_primary_where(
			char *subclassification_name );

/* Returns heap memory */
/* ------------------- */
char *subclassification_system_string(
			char *subclassification_select,
			char *subclassification_table,
			char *where );

SUBCLASSIFICATION *subclassification_parse(
			char *input,
			boolean fetch_element );

SUBCLASSIFICATION *subclassification_new(
			char *subclassification_name );

SUBCLASSIFICATION *subclassification_calloc(
			void );

/* Usage */
/* ----- */
SUBCLASSIFICATION *subclassification_element_fetch(
			char *subclassification_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time );

/* Public */
/* ------ */
LIST *subclassification_element_subclassification_name_list(
			char *element_primary_where,
			char *subclassification_table,
			char *order_column );

#endif
