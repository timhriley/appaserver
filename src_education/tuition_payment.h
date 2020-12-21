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
					"deposit_date_time"

#define TUITION_PAYMENT_MEMO		"Tuition"

#define TUITION_PAYMENT_INSERT_COLUMNS					\
					"full_name,"			\
					"street_address,"		\
					"course_name,"			\
					"season_name,"			\
					"year,"				\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"deposit_date_time,"		\
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
	PAYPAL_DEPOSIT *paypal_deposit;

	/* Process */
	/* ------- */
	double tuition_payment_amount;
	double tuition_payment_fees_expense;
	double tuition_payment_total;
	double tuition_payment_gain_donation;
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
			char *deposit_date_time );

TUITION_PAYMENT *tuition_payment_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			boolean fetch_enrollment,
			boolean fetch_paypal );

TUITION_PAYMENT *tuition_payment_parse(
			char *input,
			boolean fetch_paypal,
			boolean fetch_enrollment );

TUITION_PAYMENT *tuition_payment_steady_state(
			int *transaction_seconds_to_add,
			TUITION_PAYMENT *tuition_payment,
			LIST *paypal_tuition_payment_list,
			LIST *paypal_registration_list,
			LIST *registration_enrollment_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double paypal_transaction_fee );

TUITION_PAYMENT *tuition_payment_seek(
			LIST *paypal_tuition_payment_list,
			char *deposit_date_time );

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

void tuition_payment_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_PAYMENT *tuition_payment,
			char *cash_account_name,
			char *account_revenue,
			char *account_fees_expense,
			char *account_gain );

/* ---------------------------------------------------- */
/* Place functions that don't reference TUITION_PAYMENT */
/* in tuition_payment_fns.h				*/
/* ---------------------------------------------------- */
#endif

