/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/deposit.c		*/
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
#include "environ.h"
#include "list.h"
#include "transaction.h"
#include "deposit.h"

LIST *deposit_registration_list(
			LIST *deposit_payment_list )
{
	return (LIST *)0;
}

double deposit_remaining(
			double deposit_amount,
			LIST *deposit_registration_list )
{
	return 0.0;
/*
	return		deposit_amount -
			sum( B->registration->registration_tuition() );
*/
}

double deposit_total( LIST *deposit_payment_list )
{
	/* return sum( payment_list->payment->payment_amount ); */
	return 0.0;
}

double deposit_net_revenue(
			double deposit_amount,
			double transaction_fee )
{
	return deposit_amount - transaction_fee;
}

TRANSACTION *deposit_fee_transaction(
			char *financial_institution_full_name,
			char *financial_institution_street_address,
			char *deposit_date_time,
			double transaction_fee,
			char *account_fees_expense,
			char *account_cash,
			char *program_name )
{
	return (TRANSACTION *)0;
}

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
			char *transaction_date_time )
{
	return (char *)0;
}


double deposit_gain_donation_amount(
			double deposit_amount,
			LIST *deposit_registration_list )
{
	double remaining;
	double donation_amount;

	if ( ( remaining =
			deposit_remaining(
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

