/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/payment.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PAYMENT_H
#define PAYMENT_H

#include "boolean.h"
#include "list.h"
#include "enrollment.h"
#include "deposit.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PAYMENT_TABLE_NAME	"payment"
#define PAYMENT_TABLE		PAYMENT_TABLE_NAME

#define PAYMENT_PRIMARY_KEY	"full_name,"			\
				"street_address,"		\
				"course_name,"			\
				"season_name,"			\
				"year,"				\
				"payor_full_name,"		\
				"payor_street_address,"		\
				"deposit_date_time"

#define PAYMENT_MEMO		"Customer Payment"

#define PAYMENT_INSERT_COLUMNS	"full_name,"			\
				"street_address,"		\
				"course_name,"			\
				"season_name,"			\
				"year,"				\
				"payor_full_name,"		\
				"payor_street_address,"		\
				"deposit_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	/* Primary key */
	/* ----------- */
	ENROLLMENT *enrollment;
	DEPOSIT *deposit;

	/* Input */
	/* ----- */
	double deposit_amount;

	/* Process */
	/* ------- */
	double registration_tuition_total;
	double deposit_transaction_fee;
	double deposit_remaining;
	double payment_amount;

	double payment_fees_expense;
	double payment_gain_donation;
	LIST *deposit_payment_list;
	LIST *deposit_registration_list;
	double receivable_credit_amount;
	double payment_cash_debit_amount;
	TRANSACTION *payment_transaction;
	char *transaction_date_time;
} PAYMENT;

/* Prototypes */
/* ---------- */
PAYMENT *payment_calloc(
			void );

PAYMENT *payment_new(	char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

PAYMENT *payment_fetch(	char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			boolean fetch_deposit,
			boolean fetch_enrollment,
			boolean fetch_transaction );

TRANSACTION *payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double gain_donation,
			double receivable_credit_amount,
			double cash_debit_amount,
			char *account_cash,
			char *account_receivable,
			char *account_fees_expense,
			char *account_gain );

PAYMENT *payment_parse(	char *input,
			boolean fetch_deposit,
			boolean fetch_enrollment,
			boolean fetch_transaction );

PAYMENT *payment_steady_state(
			DEPOSIT *deposit /* in/out */,
			double deposit_amount,
			double deposit_transaction_fee,
			char *program_name,
			char *transaction_date_time,
			PAYMENT *payment /* in only */ );

PAYMENT *payment_seek(
			LIST *deposit_payment_list,
			char *deposit_date_time );

/* -------------------------------- */
/* Place functions in payment_fns.h */
/* -------------------------------- */
#endif

