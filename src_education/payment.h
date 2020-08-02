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
#include "ledger.h"
#include "deposit.h"
#include "registration.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	DEPOSIT *deposit;
	REGISTRATION *registration;
	double payment_amount_database;
	double payment_amount_calculate;
	char *program_name;
	LIST *payment_registration_list;
	TRANSACTION *payment_transaction;
} PAYMENT;

LIST *payment_registration_payment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year );

double payment_amount_calculate(
			double deposit_remaining,
			double registration_invoice_amount_due );
/*
{
	if ( deposit_remaining < registration_invoice_amount_due )
		return registration_invoice_amount_due;
	else
		return deposit_remaining;
}
*/

TRANSACTION *payment_transaction(
			char *student_full_name,
			char *student_street_address,
			char *payment_date_time,
			double payment_amount_calculate,
			char *ledger_cash_account,
			char *ledger_receivable_account,
			char *program_name );

/* Returns true transaction_date_time */
/* ---------------------------------- */
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

