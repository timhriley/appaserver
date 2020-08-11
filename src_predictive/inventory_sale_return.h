/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale_return.h		*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks inventory_sale_return ADT.		*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INVENTORY_SALE_RETURN_H
#define INVENTORY_SALE_RETURN_H

#include "list.h"
#include "hash_table.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *return_date_time;
	int returned_quantity;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} INVENTORY_SALE_RETURN;

/* Operations */
/* ---------- */
LIST *inventory_sale_return_fetch_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name );

LIST *inventory_sale_fetch_return_list(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name );

INVENTORY_SALE_RETURN *inventory_sale_return_parse(
				char *application_name,
				char *full_name,
				char *street_address,
				char *input_buffer );

char *inventory_sale_return_get_select(
				void );

INVENTORY_SALE_RETURN *inventory_sale_return_list_seek(
				LIST *inventory_sale_return_list,
				char *return_date_time );

TRANSACTION *inventory_sale_return_build_transaction(
				char **transaction_date_time,
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				double retail_price,
				double discount_amount,
				char *inventory_account_name,
				char *return_date_time,
				int returned_quantity,
				int sold_quantity,
				double sum_customer_payment_amount );

void inventory_fetch_inventory_sale_return_account_names(
				char **account_receivable_account,
				char **account_payable_account,
				char *application_name,
				char *fund_name );

LIST *inventory_sale_return_get_journal_ledger_list(
				double *transaction_amount,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double retail_price,
				double discount_amount,
				char *inventory_account_name,
				char *account_receivable_account,
				char *account_payable_account,
				int returned_quantity,
				int sold_quantity,
				double sum_customer_payment_amount );

void inventory_sale_return_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *return_date_time,
				char *transaction_date_time,
				char *database_transaction_date_time );

FILE *inventory_sale_return_get_update_pipe(
				char *application_name );

void inventory_sale_return_pipe_update(
				FILE *update_pipe,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *inventory_name,
				char *return_date_time,
				char *transaction_date_time,
				char *database_transaction_date_time );

char *inventory_sale_return_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				LIST *journal_ledger_list );

boolean inventory_sale_return_list_delete(
				LIST *inventory_sale_return_list,
				char *return_date_time );

#endif

