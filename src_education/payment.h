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
#include "transaction.h"
#include "enrollment.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	DEPOSIT *deposit;
	ENROLLMENT *enrollment;
	double payment_amount_database;
	double payment_amount;
	LIST *payment_registration_list;
	TRANSACTION *payment_transaction;
} PAYMENT;

LIST *payment_registration_payment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year );

double payment_amount(
			double deposit_remaining,
			double registration_invoice_amount_due );

TRANSACTION *payment_transaction(
			char *student_full_name,
			char *student_street_address,
			char *deposit_date_time
				/* transaction_date_time */,
			char *program_name,
			double payment_amount
				/* transaction_amount */,
			double fees_expense,
			double gain_donation,
			char *account_cash,
			char *account_gain_donation,
			char *account_receivable );

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *payment_transaction_refresh(
			char *student_full_name,
			char *student_street_address,
			char *transaction_date_time,
			char *program_name,
			double payment_amount,
			LIST *journal_list );

char *payment_update(	double payment_amount_calculate,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

#endif

