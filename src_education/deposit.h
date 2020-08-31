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
#include "semester.h"
#include "payment.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define DEPOSIT_TABLE		"deposit"
#define DEPOSIT_PRIMARY_KEY	\
   "payor_full_name,payor_street_address,season_name,year,deposit_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *payor_entity;
	SEMESTER *semester;
	char *deposit_date_time;
	double deposit_amount;
	double transaction_fee;
	double net_revenue;
	double account_balance;
	int check_number;
	char *transaction_ID;
	char *invoice_number;
	LIST *deposit_payment_list;
} DEPOSIT;

LIST *deposit_registration_list(
			LIST *deposit_payment_list );

LIST *deposit_payment_list(
			char *payor_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

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

void deposit_insert(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double deposit_total );

/* Returns static memory */
/* --------------------- */
char *deposit_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

DEPOSIT *deposit_new(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

LIST *deposit_system_list(
			char *sys_string,
			boolean fetch_payment_list );

DEPOSIT *deposit_fetch(	char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_payment_list );

DEPOSIT *deposit_parse( char *input,
			boolean fetch_payment_list );

/* Safely returns heap memory */
/* -------------------------- */
char *deposit_sys_string(
			char *where );

#endif

