/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_transaction.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SALE_TRANSACTION_H
#define SALE_TRANSACTION_H

#include "list.h"
#include "boolean.h"
#include "predictive.h"
#include "subsidiary_transaction_state.h"
#include "subsidiary_transaction.h"

typedef struct
{
	char *transaction_date_time;
	LIST *journal_list;
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
} SALE_TRANSACTION;

/* Usage */
/* ----- */
SALE_TRANSACTION *sale_transaction_new(
		char *full_name,
		char *street_address,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *completed_date_time,
		char *shipped_date_time,
		char *arrived_date,
		char *transaction_date_time,
		double shipping_charge,
		double inventory_sale_total,
		double inventory_sale_CGS_total,
		double specific_inventory_sale_total,
		double specific_inventory_sale_CGS_total,
		double sale_invoice_amount,
		double sale_gross_revenue,
		double sale_sales_tax );

/* Process */
/* ------- */
SALE_TRANSACTION *sale_transaction_calloc(
	void );

/* Returns completed_date_time, shipped_date_time, static memory, or null */
/* ---------------------------------------------------------------------- */
char *sale_transaction_date_time(
		enum predictive_title_passage_rule
			predictive_title_passage_rule,
		char *completed_date_time,
		char *shipped_date_time,
		char *arrived_date );

/* Usage */
/* ----- */
LIST *sale_transaction_journal_list(
		double shipping_charge,
		double inventory_sale_total,
		double inventory_sale_CGS_total,
		double specific_inventory_sale_total,
		double specific_inventory_sale_CGS_total,
		double sale_invoice_amount,
		double sale_gross_revenue,
		double sale_sales_tax );

/* Process */
/* ------- */
double sale_transaction_debit_sum(
		double sale_invoice_amount,
		double inventory_sale_CGS_total,
		double specific_inventory_sale_CGS_total );

double sale_transaction_credit_sum(
		double shipping_charge,
		double inventory_sale_total,
		double specific_inventory_sale_total,
		double sale_gross_revenue,
		double sale_sales_tax );

double sale_transaction_difference(
		double sale_transaction_debit_sum,
		double sale_transaction_credit_sum );

double sale_transaction_CGS_total(
		double inventory_sale_CGS_total,
		double specific_inventory_sale_CGS_total );

double sale_transaction_inventory_total(
		double inventory_sale_total,
		double specific_inventory_sale_total );
#endif
