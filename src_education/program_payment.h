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
#include "program.h"
#include "program_payment_item_title.h"
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
	/* Input */
	/* ----- */
	PROGRAM *program;
	DEPOSIT *deposit;

	/* Process */
	/* ------- */
	double program_payment_amount;
	double program_payment_fees_expense;
	double program_payment_net_payment_amount;

	PROGRAM_PAYMENT_ITEM_TITLE *program_payment_item_title;
	char *transaction_date_time;
	TRANSACTION *program_payment_transaction;
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
			boolean fetch_program,
			boolean fetch_deposit );

PROGRAM_PAYMENT *program_payment_parse(
			char *input,
			boolean fetch_program,
			boolean fetch_deposit );

PROGRAM_PAYMENT *program_payment_steady_state(
			PROGRAM_PAYMENT *program_payment,
			double deposit_payment_amount,
			double deposit_transaction_fee,
			double deposit_net_payment_amount );

PROGRAM_PAYMENT *program_payment(
			char *item_title_P,
			int program_number,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit );

/* ---------------------------------------- */
/* Place functions in program_payment_fns.h */
/* ---------------------------------------- */
#endif

