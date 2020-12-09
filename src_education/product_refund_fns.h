/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product_refund_fns.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PRODUCT_REFUND_FNS_H
#define PRODUCT_REFUND_FNS_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"
#include "deposit.h"

FILE *product_refund_insert_open(
			char *error_filename );

void product_refund_insert_pipe(
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

LIST *product_refund_system_list(
			char *sys_string,
			boolean fetch_product,
			boolean fetch_deposit );

char *product_refund_sys_string(
			char *where );

char *product_refund_primary_where(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

TRANSACTION *product_refund_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *product_name,
			char *program_name,
			double refund_amount,
			double fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *product_revenue_account );

void product_refund_update(
			char *transaction_date_time,
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

FILE *product_refund_update_open(
			void );

/* Note: there's always only one of them. */
/* -------------------------------------- */
double product_refund_amount(
			double deposit_amount );

/* Note: there's always only one of them. */
/* -------------------------------------- */
double product_refund_fees_expense(
			double deposit_fees_expense );

/* Note: there's always only one of them. */
/* -------------------------------------- */
double product_refund_net_payment_amount(
			double deposit_amount,
			double transaction_fee );

/* Returns list of one, for now */
/* ---------------------------- */
LIST *product_refund_list(
			char *item_title_P,
			LIST *education_product_list,
			DEPOSIT *deposit );

void product_refund_trigger(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			char *state );

void product_refund_list_insert(
			LIST *product_refund_list );

double product_refund_total(
			LIST *product_refund_list );

void product_refund_list_trigger(
			LIST *product_refund_list );

/* Safely returns heap memory */
/* -------------------------- */
char *product_refund_list_display(
			LIST *deposit_product_refund_list );

LIST *product_refund_transaction_list(
			LIST *product_refund_list );

LIST *product_refund_list_steady_state(
			LIST *deposit_product_refund_list,
			double deposit_amount,
			double transaction_fee );

/* Returns static memory */
/* --------------------- */
char *product_refund_memo(
			char *product_name );

void product_refund_list_payor_entity_insert(
			LIST *deposit_product_refund_list );

#endif

