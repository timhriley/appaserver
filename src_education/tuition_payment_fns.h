/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_payment_fns.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef TUITION_PAYMENT_FNS_H
#define TUITION_PAYMENT_FNS_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *tuition_payment_transaction_refresh(
			char *student_full_name,
			char *student_street_address,
			char *transaction_date_time,
			char *program_name,
			double transaction_amount,
			char *memo,
			LIST *journal_list );

void tuition_payment_update(
			double payment_amount,
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

char *tuition_payment_sys_string(
			char *where );

FILE *tuition_payment_update_open(
			void );

double tuition_payment_fees_expense(
			double deposit_transaction_fee,
			int deposit_tuition_payment_list_length );

double tution_payment_gain_donation(
			double deposit_amount,
			int deposit_registration_list_length );

double tuition_payment_total(
			LIST *tuition_payment_list );

double tuition_payment_amount(
			double deposit_amount,
			double registration_invoice_amount_due,
			int deposit_registration_list_length );

char *tuition_payment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

LIST *tuition_payment_system_list(
			char *sys_string,
			boolean fetch_deposit,
			boolean fetch_enrollment );

double tuition_payment_cash_debit_amount(
			double deposit_amount,
			double tuition_payment_fees_expense,
			int deposit_registration_list_length );

void tuition_payment_list_insert(
			LIST *tuition_payment_list );

FILE *tuition_payment_insert_open(
			char *error_filename );

void tuition_payment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			double payment_amount,
			double fees_expense,
			double gain_donation,
			char *transaction_date_time );

void tuition_payment_list_enrollment_insert(
			LIST *deposit_tuition_payment_list );

void tuition_payment_list_registration_insert(
			LIST *deposit_tuition_payment_list );

void tuition_payment_list_student_entity_insert(
			LIST *deposit_tuition_payment_list );

void tuition_payment_list_student_insert(
			LIST *deposit_tuition_payment_list );

void tuition_payment_list_payor_entity_insert(
			LIST *deposit_tuition_payment_list );

/* Safely returns heap memory */
/* -------------------------- */
char *tuition_payment_list_display(
			LIST *deposit_tuition_payment_list );

LIST *tuition_payment_registration_list(
			LIST *deposit_tuition_payment_list );

LIST *tuition_payment_list_enrollment_list(
			LIST *deposit_tuition_payment_list );

LIST *tuition_payment_enrollment_list(
			LIST *deposit_tuition_payment_list );

void tuition_payment_list_trigger(
			LIST *deposit_tuition_payment_list );

double tuition_payment_receivable_credit_amount(
			double tuition_payment_amount );

double tuition_payment_total(
			LIST *tuition_payment_list );

void tuition_payment_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *state );

void tuition_payment_list_enrollment_trigger(
			LIST *tuition_payment_list );

LIST *tuition_payment_transaction_list(
			LIST *tution_payment_list );

LIST *tuition_payment_list_steady_state(
			int *transaction_seconds_to_add,
			LIST *deposit_tuition_payment_list,
			LIST *deposit_registration_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double transaction_fee );

/* Returns static memory */
/* --------------------- */
char *tuition_payment_memo(
			char *program_name );

TRANSACTION *tuition_payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double gain_donation,
			double receivable_credit_amount,
			double cash_debit_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_receivable,
			char *account_fees_expense,
			char *account_gain,
			int seconds_to_add );

boolean tuition_payment_is_tuition(
			char *item_title_block );

void tuition_payment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *tuition_payment_list );

#endif

