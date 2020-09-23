/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_program/program_payment.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PROGRAM_PAYMENT_H
#define PROGRAM_PAYMENT_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PROGRAM_PAYMENT_TABLE		"program_payment"

#define PROGRAM_PAYMENT_PRIMARY_KEY	"program_name"

#define PROGRAM_PAYMENT_INSERT_COLUMNS	"program_name"		\
					"payor_full_name,"	\
					"payor_street_address,"	\
					"season_name,"		\
					"year,"			\
					"deposit_date_time,"	\
					"payment_amount,"	\
					"fees_expense,"		\
					"net_payment_amount,"	\
					"transaction_date_time"

#define PROGRAM_PAYMENT_MEMO		"Program Payment"

/* Structures */
/* ---------- */
typedef struct
{
	char *program_name;
	char *payor_full_name;
	char *payor_street_address;
	char *season_name;
	int year;
	char *deposit_date_time;
	double payment_amount;
	double fees_expense;
	double net_payment_amount;
	char *transaction_date_time;
	TRANSACTION *program_payment_transaction;
	PROGRAM *program;
} PROGRAM_PAYMENT;

PROGRAM_PAYMENT *program_payment_calloc(
			void );

PROGRAM_PAYMENT *program_payment_fetch(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_program );

FILE *program_payment_insert_open(
			char *error_filename );

void program_payment_insert_pipe(
			FILE *insert_pipe,
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *transaction_date_time );

LIST *program_payment_system_list(
			char *sys_string,
			boolean fetch_program );

char *program_payment_sys_string(
			char *where );

PROGRAM_PAYMENT *program_payment_parse(
			char *input,
			boolean fetch_program );

char *program_payment_primary_where(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

TRANSACTION *program_payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *account_cash,
			char *account_fees_expense,
			char *program_revenue_account );

void program_payment_update(
			char *transaction_date_time,
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

FILE *program_payment_update_open(
			void );

#endif

