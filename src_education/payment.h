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

#define PAYMENT_PRIMARY_KEY	"full_name,"			\
				"street_address,"		\
				"season_name,"			\
				"year,"				\
				"payor_full_name,"		\
				"payor_street_address,"		\
				"deposit_date_time"

#define PAYMENT_MEMO		"Customer Payment"

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
	TRANSACTION *payment_transaction;
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
			boolean fetch_enrollment,
			boolean fetch_deposit );

TRANSACTION *payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double gain_donation,
			char *account_cash,
			char *account_receivable,
			char *account_fees_expense,
			char *account_gain );

PAYMENT *payment_parse(	char *input );

PAYMENT *payment_steady_state(
			ENROLLMENT *enrollment,
			DEPOSIT *deposit,
			double deposit_amount,
			double deposit_transaction_fee,
			char *program_name );

boolean payment_extract_structure(
			REGISTRATION **registraton,
			OFFERING **offering,
			ENROLLMENT **enrollment,
			DEPOSIT **deposit,
			SEMESTER **semester,
			PAYMENT *payment );

void payment_stamp_structure(
			REGISTRATION *registration,
			OFFERING *offering,
			DEPOSIT *deposit,
			SEMESTER *semester );

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

#endif

