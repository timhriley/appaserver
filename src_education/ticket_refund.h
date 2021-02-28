/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/ticket_refund.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef TICKET_REFUND_H
#define TICKET_REFUND_H

#include "boolean.h"
#include "list.h"
#include "ticket_sale.h"
#include "paypal_item.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TICKET_REFUND_TABLE		"ticket_refund"

#define TICKET_REFUND_PRIMARY_KEY	"program_name,"			\
					"event_date,"			\
					"event_time,"			\
					"payor_full_name,"		\
					"payor_street_address"

#define TICKET_REFUND_INSERT_COLUMNS	"program_name,"			\
					"event_date,"			\
					"event_time,"			\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"refund_date_time,"		\
					"extended_price,"		\
					"refund_amount,"		\
					"merchant_fees_expense,"	\
					"net_payment_amount,"		\
					"transaction_date_time,"	\
					"paypal_date_time"

#define TICKET_REFUND_MEMO		"Ticket refund"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *program_name;
	char *event_date;
	char *event_time;
	ENTITY *payor_entity;
	double extended_price;
	double refund_amount;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	char *refund_date_time;
	TICKET_SALE *ticket_sale;
	double net_refund_amount;

	TRANSACTION *ticket_refund_transaction;
	char *transaction_date_time;
} TICKET_REFUND;

TICKET_REFUND *ticket_refund_calloc(
			void );

TICKET_REFUND *ticket_refund_fetch(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_sale,
			boolean fetch_event,
			boolean fetch_transaction );

TICKET_REFUND *ticket_refund_parse(
			char *input,
			boolean fetch_sale,
			boolean fetch_event,
			boolean fetch_transaction );

TICKET_REFUND *ticket_refund_steady_state(
			TICKET_REFUND *ticket_refund,
			double refund_amount,
			double merchant_fees_expense,
			char *refund_date_time );

FILE *ticket_refund_insert_open(
			char *error_filename );

void ticket_refund_insert_pipe(
			FILE *insert_pipe,
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			double extended_price,
			double refund_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *transaction_date_time,
			char *paypal_date_time );

LIST *ticket_refund_system_list(
			char *system_string,
			boolean fetch_sale,
			boolean fetch_event,
			boolean fetch_transaction );

char *ticket_refund_system_string(
			char *where );

char *ticket_refund_primary_where(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address );

void ticket_refund_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *ticket_refund_list );

/* ------------------------------------------------------- */
/* Sets ticket_refund->ticket_refund_transaction and
	ticket_refund->transaction_date_time		   */
/* ------------------------------------------------------- */
void ticket_refund_set_transaction(
			int *transaction_seconds_to_add,
			TICKET_REFUND *ticket_refund,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account );

TRANSACTION *ticket_refund_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *program_name,
			double refund_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *cash_account_name,
			char *account_fees_expense,
			char *purchase_revenue_account );

void ticket_refund_update(
			char *refund_date_time,
			double net_refund_amount,
			char *transaction_date_time,
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address );

FILE *ticket_refund_update_open(
			void );

void ticket_refund_list_insert(
			LIST *ticket_refund_list );

/* Safely returns heap memory */
/* -------------------------- */
char *ticket_refund_list_display(
			LIST *ticket_refund_list );

LIST *ticket_refund_transaction_list(
			LIST *ticket_refund_list );

/* Returns static memory */
/* --------------------- */
char *ticket_refund_memo(
			char *refund_name );

LIST *ticket_refund_list_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *education_event_list );

TICKET_REFUND *ticket_refund_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			EVENT *event );

LIST *ticket_refund_event_list(
			LIST *ticket_refund_list );

TICKET_REFUND *ticket_refund_new(
			char *program_name,
			char *event_date,
			char *event_time,
			ENTITY *payor_entity );

boolean ticket_refund_list_any_exists(
			LIST *ticket_refund_list,
			LIST *existing_ticket_refund_list );

TICKET_REFUND *ticket_refund_list_seek(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			LIST *ticket_refund_list );
#endif

