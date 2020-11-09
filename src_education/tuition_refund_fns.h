/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_refund_fns.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef TUITION_REFUND_FNS_H
#define TUITION_REFUND_FNS_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *tuition_refund_transaction_refresh(
			char *student_full_name,
			char *student_street_address,
			char *transaction_date_time,
			char *program_name,
			double transaction_amount,
			char *memo,
			LIST *journal_list );

void tuition_refund_update(
			double refund_amount,
			double fees_expense,
			double overpayment_loss,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

char *tuition_refund_sys_string(
			char *where );

FILE *tuition_refund_update_open(
			void );

double tuition_refund_fees_expense(
			double deposit_transaction_fee,
			int deposit_tuition_refund_list_length );

double tution_refund_overpayment_loss(
			double deposit_overpayment_loss,
			int deposit_registration_list_length );

double tuition_refund_total(
			LIST *tuition_refund_list );

double tuition_refund_amount(
			double deposit_amount,
			double registration_tuition,
			int deposit_registration_list_length );

char *tuition_refund_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

LIST *tuition_refund_system_list(
			char *sys_string,
			boolean fetch_deposit,
			boolean fetch_enrollment );

double tuition_refund_cash_credit_amount(
			double deposit_amount,
			double tuition_refund_fees_expense,
			int deposit_registration_list_length );

void tuition_refund_list_insert(
			LIST *tuition_refund_list );

FILE *tuition_refund_insert_open(
			char *error_filename );

void tuition_refund_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			double refund_amount,
			double fees_expense,
			double overpayment_loss,
			char *transaction_date_time );

void tuition_refund_list_enrollment_insert(
			LIST *deposit_tuition_refund_list );

void tuition_refund_list_registration_insert(
			LIST *deposit_tuition_refund_list );

void tuition_refund_list_student_entity_insert(
			LIST *deposit_tuition_refund_list );

void tuition_refund_list_student_insert(
			LIST *deposit_tuition_refund_list );

void tuition_refund_list_payor_entity_insert(
			LIST *deposit_tuition_refund_list );

/* Safely returns heap memory */
/* -------------------------- */
char *tuition_refund_list_display(
			LIST *deposit_tuition_refund_list );

LIST *tuition_refund_registration_list(
			LIST *deposit_tuition_refund_list );

LIST *tuition_refund_enrollment_list(
			LIST *deposit_tuition_refund_list );

void tuition_refund_list_trigger(
			LIST *deposit_tuition_refund_list );

double tuition_refund_receivable_credit_amount(
			double tuition_refund_amount );

void tuition_refund_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *state );

void tuition_refund_list_enrollment_trigger(
			LIST *tuition_refund_list );

LIST *tuition_refund_transaction_list(
			LIST *tution_refund_list );

LIST *tuition_refund_list_steady_state(
			LIST *deposit_tuition_refund_list,
			LIST *deposit_registration_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double transaction_fee );

/* Returns static memory */
/* --------------------- */
char *tuition_refund_memo(
			char *program_name );

TRANSACTION *tuition_refund_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double refund_amount,
			double fees_expense,
			double overpayment_loss,
			double receivable_credit_amount,
			double cash_debit_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_revenue,
			char *account_fees_expense,
			char *account_loss,
			int seconds_to_add );

double tuition_refund_revenue_debit_amount(
			double refund_amount );

char *tuition_refund_list_display(
			LIST *refund_list );

#endif

