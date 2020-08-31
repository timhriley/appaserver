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
#include "transaction.h"

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

double deposit_gain_donation_amount(
			double deposit_amount,
			LIST *deposit_registration_list );
/*
{
	double remaining;
	double donation_amount;

	if ( ( remaing = deposit_remaining(
			deposit_amount,
			deposit_registration_list ) ) > 0 )
	{
		donation_amount = remaining;
	}
	else
	{
		donation_amount = 0.0;
	}
	return donation_amount;
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
			char *account_fees_expense,
			char *account_cash,
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

/* Safely returns heap memory */
/* -------------------------- */
char *deposit_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			char *year,
			char *deposit_date_time );

DEPOSIT *deposit_new(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

#endif

