/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/payment.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "list.h"
#include "transaction.h"
#include "payment.h"

LIST *payment_registration_payment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return (LIST *)0;
}

double payment_amount(
			double deposit_remaining,
			double registration_invoice_amount_due )
{
	if ( deposit_remaining < registration_invoice_amount_due )
		return registration_invoice_amount_due;
	else
		return deposit_remaining;
}

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
			char *account_receivable )
{
	return (TRANSACTION *)0;
}

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *payment_transaction_refresh(
			char *student_full_name,
			char *student_street_address,
			char *transaction_date_time,
			char *program_name,
			double payment_amount,
			LIST *journal_list )
{
	return (char *)0;
}

char *payment_update(	double payment_amount_calculate,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time )
{
	return transaction_date_time;
}

