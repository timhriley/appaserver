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
#include "enrollment.h"
#include "deposit.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TUITION_REFUND_TABLE		"tuition_refund"

#define TUITION_REFUND_PRIMARY_KEY					\
					"full_name,"			\
					"street_address,"		\
					"course_name,"			\
					"season_name,"			\
					"year,"				\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"deposit_date_time"

#define TUITION_REFUND_MEMO		"Tuition Refund"

#define TUITION_REFUND_INSERT_COLUMNS					\
					"full_name,"			\
					"street_address,"		\
					"course_name,"			\
					"season_name,"			\
					"year,"				\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"deposit_date_time,"		\
					"tuition_refund_amount,"	\
					"fees_expense,"			\
					"overpayment_loss,"		\
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
	double tuition_refund_amount;
	double tuition_refund_fees_expense;
	double tuition_refund_total;
	double tuition_refund_overpayment_loss;
	double tuition_refund_revenue_debit_amount;
	double tuition_refund_cash_credit_amount;

	char *transaction_date_time;
	TRANSACTION *tuition_refund_transaction;
} TUITION_REFUND;

/* Prototypes */
/* ---------- */
TUITION_REFUND *tuition_refund_calloc(
			void );

TUITION_REFUND *tuition_refund_new(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

TUITION_REFUND *tuition_refund_fetch(
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

TUITION_REFUND *tuition_refund_parse(
			char *input,
			boolean fetch_deposit,
			boolean fetch_enrollment );

TUITION_REFUND *tuition_refund_steady_state(
			TUITION_REFUND *tuition_refund,
			LIST *deposit_tuition_refund_list,
			LIST *deposit_registration_list,
			LIST *registration_enrollment_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double deposit_transaction_fee,
			int transaction_seconds_to_add );

TUITION_REFUND *tuition_refund_seek(
			LIST *deposit_tuition_refund_list,
			char *deposit_date_time );

TUITION_REFUND *tuition_refund(
			char *season_name,
			int year,
			char *item_title_P,
			int student_number,
			DEPOSIT *deposit );

boolean tuition_refund_structure(
			TUITION_REFUND *tuition_refund );

LIST *tuition_refund_list(
			char *season_name,
			int year,
			char *item_title_P,
			DEPOSIT *deposit );

/* ---------------------------------------- */
/* Place functions in tuition_refund_fns.h */
/* ---------------------------------------- */
#endif

