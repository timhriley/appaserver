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
			char *account_cash,
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
			double deposit_payment_amount );

double program_payment_fees_expense(
			double deposit_fees_expense );

double program_payment_net_payment_amount(
			double deposit_net_payment_amount );

LIST *program_payment_list(
			char *item_title_P,
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

#endif
