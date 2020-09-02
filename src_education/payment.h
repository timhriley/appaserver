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
#include "deposit.h"
#include "enrollment.h"
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
	ENROLLMENT *enrollment;
	DEPOSIT *deposit;
	double payment_amount;
	double payment_fees_expense;
	double payment_gain_donation;
	LIST *payment_registration_list;
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
			char *deposit_date_time );

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

LIST *payment_system_list(
			char *sys_string );

PAYMENT *payment_parse(	char *input );

LIST *payment_deposit_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year );

FILE *payment_update_open(
			void );

PAYMENT *payment_steady_state(
			ENROLLMENT *enrollment,
			DEPOSIT *deposit,
			LIST *deposit_payment_list,
			double deposit_transaction_fee );

double payment_fees_expense(
			double deposit_transaction_fee,
			LIST *deposit_payment_list );

double payment_gain_donation(
			double deposit_amount,
			LIST *deposit_registration_list );

double payment_total(	LIST *payment_list );

double payment_amount(
			double deposit_remaining,
			double registration_invoice_amount_due );

#endif

