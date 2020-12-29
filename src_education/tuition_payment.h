/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_payment.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef TUITION_PAYMENT_H
#define TUITION_PAYMENT_H

#include "boolean.h"
#include "list.h"
#include "enrollment.h"
#include "paypal_deposit.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TUITION_PAYMENT_TABLE		"tuition_payment"

#define TUITION_PAYMENT_PRIMARY_KEY					\
					"full_name,"			\
					"street_address,"		\
					"course_name,"			\
					"season_name,"			\
					"year,"				\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"payment_date_time"

#define TUITION_PAYMENT_MEMO		"Tuition"

#define TUITION_PAYMENT_INSERT_COLUMNS					\
					"full_name,"			\
					"street_address,"		\
					"course_name,"			\
					"season_name,"			\
					"year,"				\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"payment_date_time,"		\
					"tuition_payment_amount,"	\
					"fees_expense,"			\
					"gain_donation,"		\
					"transaction_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	ENROLLMENT *enrollment;
	char *payor_full_name;
	char *payor_street_address;
	char *payment_date_time;
	double payment_amount;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	double net_payment_amount;
	double tuition_payment_total;
	double tuition_payment_overpayment_donation;
	double tuition_payment_receivable_credit_amount;
	double tuition_payment_cash_debit_amount;

	char *transaction_date_time;
	TRANSACTION *tuition_payment_transaction;
} TUITION_PAYMENT;

/* Prototypes */
/* ---------- */
TUITION_PAYMENT *tuition_payment_calloc(
			void );

TUITION_PAYMENT *tuition_payment_new(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

TUITION_PAYMENT *tuition_payment_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			boolean fetch_enrollment,
			boolean fetch_paypal );

TUITION_PAYMENT *tuition_payment_parse(
			char *input,
			boolean fetch_enrollment,
			boolean fetch_paypal );

TUITION_PAYMENT *tuition_payment_steady_state(
			TUITION_PAYMENT *tuition_payment,
			double payment_amount,
			double merchant_transaction_fee );

TUITION_PAYMENT *tuition_payment_seek(
			LIST *paypal_tuition_payment_list,
			char *payment_date_time );

TUITION_PAYMENT *tuition_payment(
			char *season_name,
			int year,
			ENTITY *benefit_entity,
			double item_value,
			double item_fee,
			double item_gain,
			OFFERING *offering,
			PAYPAL_DEPOSIT *paypal_deposit );

boolean tuition_payment_structure(
			TUITION_PAYMENT *tuition_payment );

LIST *tuition_payment_list(
			char *season_name,
			int year,
			LIST *paypal_item_steady_state_list,
			LIST *semester_offering_list,
			PAYPAL_DEPOSIT *paypal_deposit );

/* ----------------------------------------------------- */
/* Sets tuition_payment->tuition_payment_transaction and
	tuition_payment->transaction_date_time		 */
/* ----------------------------------------------------- */
void tuition_payment_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_PAYMENT *tuition_payment,
			char *cash_account_name,
			char *account_revenue,
			char *account_fees_expense,
			char *account_overpayment_donation );

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
			double net_payment_amount,
			double overpayment_donation,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

char *tuition_payment_sys_string(
			char *where );

FILE *tuition_payment_update_open(
			void );

double tuition_payment_fees_expense(
			double paypal_transaction_fee,
			int paypal_tuition_payment_list_length );

double tution_payment_overpayment_donation(
			double payment_amount,
			LIST *enrollment_registration_list );

double tuition_payment_total(
			LIST *tuition_payment_list );

char *tuition_payment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

LIST *tuition_payment_system_list(
			char *sys_string,
			boolean fetch_enrollment,
			boolean fetch_paypal );

double tuition_payment_cash_debit_amount(
			double deposit_amount,
			double tuition_payment_fees_expense,
			int paypal_registration_list_length );

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
			char *payment_date_time,
			double payment_amount,
			double merchant_fees_expense,
			double overpayment_donation,
			char *transaction_date_time );

void tuition_payment_list_enrollment_insert(
			LIST *paypal_tuition_payment_list );

void tuition_payment_list_registration_insert(
			LIST *paypal_tuition_payment_list );

void tuition_payment_list_student_entity_insert(
			LIST *paypal_tuition_payment_list );

void tuition_payment_list_student_insert(
			LIST *paypal_tuition_payment_list );

void tuition_payment_list_payor_entity_insert(
			LIST *paypal_tuition_payment_list );

/* Safely returns heap memory */
/* -------------------------- */
char *tuition_payment_list_display(
			LIST *paypal_tuition_payment_list );

LIST *tuition_payment_registration_list(
			LIST *paypal_tuition_payment_list );

LIST *tuition_payment_list_enrollment_list(
			LIST *paypal_tuition_payment_list );

LIST *tuition_payment_enrollment_list(
			LIST *paypal_tuition_payment_list );

void tuition_payment_list_trigger(
			LIST *paypal_tuition_payment_list );

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
			char *payment_date_time,
			char *state );

void tuition_payment_list_enrollment_trigger(
			LIST *tuition_payment_list );

LIST *tuition_payment_transaction_list(
			LIST *tution_payment_list );

LIST *tuition_payment_list_steady_state(
			int *transaction_seconds_to_add,
			LIST *paypal_tuition_payment_list,
			LIST *paypal_registration_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double transaction_fee );

/* Returns static memory */
/* --------------------- */
char *tuition_payment_memo(
			char *program_name );

TRANSACTION *tuition_payment_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double gain_donation,
			double receivable_credit_amount,
			double cash_debit_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_receivable,
			char *account_fees_expense,
			char *account_overpayment_donation );

boolean tuition_payment_is_tuition(
			char *item_title_block );

void tuition_payment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *tuition_payment_list );

#endif

