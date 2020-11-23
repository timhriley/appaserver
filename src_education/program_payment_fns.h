/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_program/program_payment_fns.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PROGRAM_PAYMENT_FNS_H
#define PROGRAM_PAYMENT_FNS_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"
#include "deposit.h"

FILE *program_payment_insert_open(
			char *error_filename );

void program_payment_insert_pipe(
			FILE *insert_pipe,
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *transaction_date_time );

LIST *program_payment_system_list(
			char *sys_string,
			boolean fetch_program,
			boolean fetch_deposit );

char *program_payment_sys_string(
			char *where );

char *program_payment_primary_where(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

TRANSACTION *program_payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *program_revenue_account );

void program_payment_update(
			char *transaction_date_time,
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

FILE *program_payment_update_open(
			void );

double program_payment_amount(
			double deposit_amount );

double program_payment_fees_expense(
			double deposit_fees_expense );

double program_payment_net_payment_amount(
			double deposit_net_payment_amount );

LIST *program_payment_list(
			LIST *not_exists_program_name_list,
			char *item_title_P,
			LIST *education_program_list,
			DEPOSIT *deposit );

void program_payment_trigger(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			char *state );

void program_payment_list_insert(
			LIST *program_payment_list );

double program_payment_total(
			LIST *program_payment_list );

void program_payment_list_trigger(
			LIST *program_payment_list );

/* Safely returns heap memory */
/* -------------------------- */
char *program_payment_list_display(
			LIST *deposit_program_payment_list );

LIST *program_payment_transaction_list(
			LIST *program_payment_list );

LIST *program_payment_list_steady_state(
			LIST *deposit_program_payment_list,
			double deposit_amount,
			double transaction_fee,
			double net_payment_amount );

/* Returns static memory */
/* --------------------- */
char *program_payment_memo(
			char *program_name );

void program_payment_list_payor_entity_insert(
			LIST *deposit_program_payment_list );

#endif

