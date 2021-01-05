/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/program_donation.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PROGRAM_DONATION_H
#define PROGRAM_DONATION_H

#include "boolean.h"
#include "list.h"
#include "program.h"
#include "tuition_payment.h"
#include "ticket_sale.h"
#include "product_sale.h"
#include "paypal_item.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PROGRAM_DONATION_TABLE		"program_donation"

#define PROGRAM_DONATION_PRIMARY_KEY	"program_name"

#define PROGRAM_DONATION_INSERT_COLUMNS	"program_name,"			\
					"payment_date_time,"		\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"donation_amount,"		\
					"net_payment_amount,"		\
					"transaction_date_time,"	\
					"merchant_fees_expense,"	\
					"paypal_date_time"

#define PROGRAM_DONATION_MEMO		"Program Donation"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	PROGRAM *program;
	ENTITY *payor_entity;
	char *payment_date_time;
	double donation_amount;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	double net_payment_amount;

	TRANSACTION *program_donation_transaction;
	char *transaction_date_time;
} PROGRAM_DONATION;

PROGRAM_DONATION *program_donation_calloc(
			void );

PROGRAM_DONATION *program_donation_fetch(
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_program );

PROGRAM_DONATION *program_donation_parse(
			char *input,
			boolean fetch_program );

PROGRAM_DONATION *program_donation_steady_state(
			PROGRAM_DONATION *program_donation,
			double donation_amount,
			double merchant_fees_expense );

FILE *program_donation_insert_open(
			char *error_filename );

void program_donation_insert_pipe(
			FILE *insert_pipe,
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			double donation_amount,
			double net_payment_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time );

LIST *program_donation_system_list(
			char *sys_string,
			boolean fetch_program );

char *program_donation_sys_string(
			char *where );

char *program_donation_primary_where(
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address );

void program_donation_list_set_transaction(
			int *seconds_to_add,
			LIST *program_donation_list );

/* ------------------------------------------------------- */
/* Sets program_donation->program_donation_transaction and
	program_donation->transaction_date_time		   */
/* ------------------------------------------------------- */
void program_donation_set_transaction(
			int *seconds_to_add,
			PROGRAM_DONATION *program_donation,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *program_revenue_account );

TRANSACTION *program_donation_transaction(
			int *seconds_to_add,
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			double donation_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *program_revenue_account );

LIST *program_donation_transaction_list(
			LIST *program_donation_list );

void program_donation_update(
			double net_payment_amount,
			char *transaction_date_time,
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address );

FILE *program_donation_update_open(
			void );

void program_donation_trigger(
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *state );

void program_donation_list_insert(
			LIST *program_donation_list );

double program_donation_total(
			LIST *program_donation_list );

void program_donation_list_trigger(
			LIST *program_donation_list );

/* Safely returns heap memory */
/* -------------------------- */
char *program_donation_list_display(
			LIST *program_donation_list );

LIST *program_donation_list_steady_state(
			LIST *program_donation_list );

/* Returns static memory */
/* --------------------- */
char *program_donation_memo(
			char *program_name );

void program_donation_list_payor_entity_insert(
			LIST *program_donation_list );

LIST *program_donation_list_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *program_list );

PROGRAM_DONATION *program_donation_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			PROGRAM *program );

LIST *program_donation_list_overpayment(
			double item_value,
			double item_fee,
			ENTITY *payor_entity,
			char *paypal_date_time,
			TUITION_PAYMENT *tuition_payment,
			TICKET_SALE *ticket_sale,
			PRODUCT_SALE *product_sale );

#endif

