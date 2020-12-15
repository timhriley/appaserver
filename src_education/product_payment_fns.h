/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product_payment_fns.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PRODUCT_PAYMENT_FNS_H
#define PRODUCT_PAYMENT_FNS_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"
#include "deposit.h"

FILE *product_payment_insert_open(
			char *error_filename );

void product_payment_insert_pipe(
			FILE *insert_pipe,
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *transaction_date_time );

LIST *product_payment_system_list(
			char *sys_string,
			boolean fetch_product,
			boolean fetch_deposit );

char *product_payment_sys_string(
			char *where );

char *product_payment_primary_where(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

TRANSACTION *product_payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *product_name,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *product_revenue_account,
			int seconds_to_add );

void product_payment_update(
			char *transaction_date_time,
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

FILE *product_payment_update_open(
			void );

/* Note: there's always only one of them. */
/* -------------------------------------- */
double product_payment_amount(
			double deposit_amount );

/* Note: there's always only one of them. */
/* -------------------------------------- */
double product_payment_fees_expense(
			double deposit_fees_expense );

/* Note: there's always only one of them. */
/* -------------------------------------- */
double product_payment_net_payment_amount(
			double deposit_amount,
			double transaction_fee );

LIST *product_payment_list(
			LIST *paypal_item_list,
			LIST *product_list,
			DEPOSIT *deposit );

void product_payment_trigger(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			char *state );

void product_payment_list_insert(
			LIST *product_payment_list );

double product_payment_total(
			LIST *product_payment_list );

void product_payment_list_trigger(
			LIST *product_payment_list );

/* Safely returns heap memory */
/* -------------------------- */
char *product_payment_list_display(
			LIST *deposit_product_payment_list );

LIST *product_payment_transaction_list(
			LIST *product_payment_list );

LIST *product_payment_list_steady_state(
			int *transaction_seconds_to_add,
			LIST *deposit_product_payment_list,
			double deposit_amount,
			double transaction_fee );

/* Returns static memory */
/* --------------------- */
char *product_payment_memo(
			char *product_name );

void product_payment_list_payor_entity_insert(
			LIST *deposit_product_payment_list );

void product_payment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *product_payment_list );

#endif

