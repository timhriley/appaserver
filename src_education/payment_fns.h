/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/payment_fns.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PAYMENT_FNS_H
#define PAYMENT_FNS_H

#include "boolean.h"
#include "list.h"

/* ---------- */
/* Prototypes */
/* ---------- */

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *payment_transaction_refresh(
			char *student_full_name,
			char *student_street_address,
			char *transaction_date_time,
			char *program_name,
			double transaction_amount,
			char *memo,
			LIST *journal_list );

void payment_update(	double payment_amount,
			double fees_expense,
			double gain_donation,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

char *payment_sys_string(
			char *where );

LIST *payment_deposit_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year );

FILE *payment_update_open(
			void );

double payment_fees_expense(
			double deposit_transaction_fee,
			LIST *deposit_payment_list );

double payment_gain_donation(
			double deposit_amount,
			LIST *deposit_registration_list );

double payment_total(	LIST *payment_list );

double payment_amount(
			double deposit_amount,
			double deposit_invoice_amount_due );

char *payment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

LIST *payment_system_list(
			char *sys_string,
			boolean fetch_deposit,
			boolean fetch_enrollment );

double payment_cash_debit_amount(
			double payment_amount,
			double payment_gain_donation,
			double payment_fees_expense );

void payment_list_insert(
			LIST *payment_list );

FILE *payment_insert_open(
			char *error_filename );

void payment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

void payment_list_enrollment_insert(
			LIST *payment_list );

void payment_list_registration_insert(
			LIST *deposit_payment_list );

void payment_list_offering_insert(
			LIST *deposit_payment_list );

void payment_list_course_insert(
			LIST *deposit_payment_list );

void payment_list_student_insert(
			LIST *deposit_payment_list );

void payment_list_entity_insert(
			LIST *deposit_payment_list );

#endif

