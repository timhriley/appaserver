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
#include "deposit.h"
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
					"payment_amount,"		\
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
	DEPOSIT *deposit;

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
			boolean fetch_deposit );

TUITION_PAYMENT *tuition_payment_parse(
			char *input,
			boolean fetch_deposit,
			boolean fetch_enrollment );

TUITION_PAYMENT *tuition_payment_steady_state(
			TUITION_PAYMENT *tuition_payment,
			LIST *deposit_tuition_payment_list,
			LIST *deposit_registration_list,
			LIST *registration_enrollment_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double deposit_transaction_fee );

TUITION_PAYMENT *tuition_payment_seek(
			LIST *deposit_tuition_payment_list,
			char *deposit_date_time );

TUITION_PAYMENT *tuition_payment(
			LIST *not_exists_course_name_list,
			char *season_name,
			int year,
			char *item_title_P,
			int student_number,
			DEPOSIT *deposit );

boolean tuition_payment_structure(
			TUITION_PAYMENT *tuition_payment );

LIST *tuition_payment_list(
			LIST *not_exists_course_name_list,
			char *season_name,
			int year,
			char *item_title_P,
			DEPOSIT *deposit );

/* ---------------------------------------- */
/* Place functions in tuition_payment_fns.h */
/* ---------------------------------------- */
#endif

