/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_refund.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef TUITION_REFUND_H
#define TUITION_REFUND_H

#include "boolean.h"
#include "list.h"
#include "tuition_payment.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TUITION_REFUND_TABLE		"tuition_refund"

#define TUITION_REFUND_PRIMARY_KEY	"full_name,"			\
					"street_address,"		\
					"course_name,"			\
					"season_name,"			\
					"year,"				\
					"payor_street_address,"		\
					"payment_date_time,"		\
					"refund_date_time"

#define TUITION_REFUND_INSERT_COLUMNS	"full_name,"			\
					"street_address,"		\
					"course_name,"			\
					"season_name,"			\
					"year,"				\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"payment_date_time,"		\
					"refund_date_time,"		\
					"refund_amount,"		\
					"net_payment_amount,"		\
					"transaction_date_time,"	\
					"merchant_fees_expense,"	\
					"paypal_date_time"

#define TUITION_REFUND_MEMO		"Tuition refund"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	TUITION_PAYMENT *tuition_payment;
	/* PAYPAL_DEPOSIT *paypal_deposit; */
	ENTITY *payor_entity;
	char *refund_date_time;
	double refund_amount;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	double net_refund_amount;

	TRANSACTION *tuition_refund_transaction;
	char *transaction_date_time;
} TUITION_REFUND;

TUITION_REFUND *tuition_refund_calloc(
			void );

TUITION_REFUND *tuition_refund_fetch(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time,
			boolean fetch_payment,
			boolean fetch_paypal );

TUITION_REFUND *tuition_refund_parse(
			char *input,
			boolean fetch_payment,
			boolean fetch_paypal );

TUITION_REFUND *tuition_refund_steady_state(
			TUITION_REFUND *tuition_refund,
			double refund_amount,
			double merchant_fees_expense );

TUITION_REFUND *tuition_refund_paypal(
			LIST *offering_list,
			char *item_data,
			double item_value,
			double item_fee,
			char *paypal_date_time );

FILE *tuition_refund_insert_open(
			char *error_filename );

void tuition_refund_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time,
			double refund_amount,
			double net_payment_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time );

LIST *tuition_refund_system_list(
			char *sys_string,
			boolean fetch_payment,
			boolean fetch_paypal );

char *tuition_refund_sys_string(
			char *where );

char *tuition_refund_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time );

void tuition_refund_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *tuition_refund_list );

/* ------------------------------------------------------- */
/* Sets tuition_refund->tuition_refund_transaction and
	tuition_refund->transaction_date_time		   */
/* ------------------------------------------------------- */
void tuition_refund_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_REFUND *tuition_refund,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account );

TRANSACTION *tuition_refund_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *course_name,
			char *program_name,
			double refund_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *offering_revenue_account );

void tuition_refund_update(
			double net_refund_amount,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time );

FILE *tuition_refund_update_open(
			void );

LIST *tuition_refund_list_paypal(
			LIST *paypal_item_steady_state_list,
			LIST *tuition_refund_list,
			char *paypal_date_time );

void tuition_refund_trigger(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time,
			char *state );

void tuition_refund_list_insert(
			LIST *tuition_refund_list );

double tuition_refund_total(
			LIST *tuition_refund_list );

void tuition_refund_list_trigger(
			LIST *tuition_refund_list );

/* Safely returns heap memory */
/* -------------------------- */
char *tuition_refund_list_display(
			LIST *tuition_refund_list );

LIST *tuition_refund_transaction_list(
			LIST *tuition_refund_list );

LIST *tuition_refund_list_steady_state(
			LIST *tuition_refund_list,
			double refund_amount,
			double merchant_fees_expense );

/* Returns static memory */
/* --------------------- */
char *tuition_refund_memo(
			char *refund_name );

void tuition_refund_list_payor_entity_insert(
			LIST *tuition_refund_list );

LIST *tuition_refund_registration_list(
			LIST *tuition_refund_list );

LIST *tuition_refund_enrollment_list(
			LIST *tuition_refund_list );

#endif

