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
#include "payment.h"

LIST *payment_registration_payment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year )
{
	return (LIST *)0;
}

double payment_amount_calculate(
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
			char *payment_date_time,
			double payment_amount_calculate,
			char *ledger_cash_account,
			char *ledger_receivable_account,
			char *program_name )
{
	return (TRANSACTION *)0;
}

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
			char *payment_date_time )
{
	return transaction_date_time;
}

