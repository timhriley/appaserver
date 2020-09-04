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
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define DEPOSIT_TABLE		"deposit"

#define DEPOSIT_PRIMARY_KEY	"payor_full_name,"		\
				"payor_street_address,"		\
				"season_name,"			\
				"year,"				\
				"deposit_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	/* ----------- */
	/* Primary key */
	/* ----------- */
	ENTITY *payor_entity;
	SEMESTER *semester;
	char *deposit_date_time;
	/* ---------- */
	/* Attributes */
	/* ---------- */
	double deposit_amount;
	double transaction_fee;
	double net_revenue;
	double account_balance;
	int check_number;
	char *transaction_ID;
	char *invoice_number;
	/* ------- */
	/* Process */
	/* ------- */
	double deposit_remaining;
	double deposit_net_revenue;
	LIST *deposit_payment_list;
	LIST *deposit_registration_list;
	double deposit_payment_total;
	double deposit_gain_donation;
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
			double registration_tuition_total );

double deposit_net_revenue(
			double deposit_amount,
			double transaction_fee );

void deposit_insert(	char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double deposit_amount,
			double transaction_fee,
			double net_revenue,
			double account_balance,
			int check_number,
			char *transaction_ID,
			char *invoice_number,
			double payment_total,
			double gain_donation );

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

void deposit_insert_pipe(
			FILE *insert_pipe,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double deposit_amount,
			double transaction_fee,
			double net_revenue,
			double account_balance,
			int check_number,
			char *transaction_ID,
			char *invoice_number,
			double payment_total,
			double gain_donation );

void deposit_insert(	char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double deposit_amount,
			double transaction_fee,
			double net_revenue,
			double account_balance,
			int check_number,
			char *transaction_ID,
			char *invoice_number,
			double payment_total,
			double gain_donation );

FILE *deposit_insert_open(
			void );

double deposit_payment_total(
			LIST *deposit_payment_list );

DEPOSIT *deposit_steady_state(
			ENTITY *payor_entity,
			SEMESTER *semester,
			char *deposit_date_time,
			double deposit_amount,
			double deposit_transaction_fee,
			LIST *deposit_payment_list,
			double registration_invoice_amount_due );

#endif

