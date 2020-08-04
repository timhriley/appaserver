/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/deposit.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef DEPOSIT_H
#define DEPOSIT_H

#include "boolean.h"
#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *payor_full_name;
	char *payor_street_address;
	char *course_name;
	char *season_name;
	int year;
	char *deposit_date_time;
	double transaction_fee;
	double deposit_amount;
	char *program_name;
} DEPOSIT;

LIST *deposit_registration_list(
			LIST *deposit_payment_list );

double deposit_remaining(
			double deposit_amount,
			LIST *deposit_registration_list );
/*
{
	return		deposit_amount -
			sum( B->registration->registration_tuition() );
}
*/


double deposit_gain_donation_amount(
			double deposit_amount,
			double deposit_payment_list );
/*
{
	if ( deposit_remaining(
		deposit_amount,
		deposit_payment_list ) > 0 )
	{
		gain_donation_amount = donation_remaining;
	}
	else
	{
		gain_donation_amount = 0
	}
}
*/

double deposit_total(	LIST *deposit_payment_list );
/*
{
	return sum( payment_list->payment->payment_amount );
}
*/

double deposit_net_revenue(
			double deposit_amount,
			double transaction_fee );
/*
{
	return A - B;
}
*/

TRANSACTION *deposit_fee_transaction(
			char *financial_institution_full_name,
			char *financial_institution_street_address,
			char *deposit_date_time,
			double transaction_fee,
			char *ledger_fees_expense_account,
			char *ledger_cash_account,
			char *program_name );

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *deposit_insert(	char *financial_institution_full_name,
			char *financial_institution_street_address,
			char *deposit_date_time,
			double deposit_payment_total(),
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			char *year,
			char *transaction_date_time );

#endif

