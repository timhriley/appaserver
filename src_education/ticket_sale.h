/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/ticket_sale.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef TICKET_SALE_H
#define TICKET_SALE_H

#include "boolean.h"
#include "list.h"
#include "event.h"
#include "paypal_item.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TICKET_SALE_TABLE		"ticket_sale"

#define TICKET_SALE_PRIMARY_KEY		"event_name,"			\
					"event_date,"			\
					"event_time,"			\
					"sale_date_time,"		\
					"payor_full_name,"		\
					"payor_street_address"

#define TICKET_SALE_INSERT_COLUMNS	"event_name,"			\
					"event_date,"			\
					"event_time,"			\
					"sale_date_time,"		\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"quantity,"			\
					"ticket_price,"			\
					"extended_price,"		\
					"net_payment_amount,"		\
					"transaction_date_time,"	\
					"merchant_fees_expense,"	\
					"paypal_date_time"

#define TICKET_SALE_MEMO		"Ticket Sale"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	EVENT *event;
	/* PAYPAL_DEPOSIT *paypal_deposit; */
	ENTITY *payor_entity;
	char *sale_date_time;
	int quantity;
	double ticket_price;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	double extended_price;
	double net_payment_amount;

	TRANSACTION *ticket_sale_transaction;
	char *transaction_date_time;
} TICKET_SALE;

TICKET_SALE *ticket_sale_calloc(
			void );

TICKET_SALE *ticket_sale_fetch(
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_event,
			boolean fetch_paypal );

TICKET_SALE *ticket_sale_parse(
			char *input,
			boolean fetch_event,
			boolean fetch_paypal );

TICKET_SALE *ticket_sale_steady_state(
			TICKET_SALE *ticket_sale,
			int quantity,
			double ticket_price,
			double merchant_fees_expense );

TICKET_SALE *ticket_sale_paypal(
			EVENT *event,
			double item_value,
			double item_fee,
			char *paypal_date_time );

void ticket_sale_list_set_transaction(
			int *seconds_to_add,
			LIST *ticket_sale_list );

/* ------------------------------------------------------- */
/* Sets ticket_sale->ticket_sale_transaction and
	ticket_sale->transaction_date_time		   */
/* ------------------------------------------------------- */
void ticket_sale_set_transaction(
			int *seconds_to_add,
			TICKET_SALE *ticket_sale,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account );

FILE *ticket_sale_insert_open(
			char *error_filename );

void ticket_sale_insert_pipe(
			FILE *insert_pipe,
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			int quantity,
			double ticket_price,
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time );

LIST *ticket_sale_system_list(
			char *sys_string,
			boolean fetch_event,
			boolean fetch_paypal );

char *ticket_sale_sys_string(
			char *where );

char *ticket_sale_primary_where(
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address );

void ticket_sale_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *ticket_sale_list );

TRANSACTION *ticket_sale_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *event_name,
			char *program_name,
			double extended_price,
			double merchant_fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *revenue_account );

void ticket_sale_update(
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address );

FILE *ticket_sale_update_open(
			void );

LIST *ticket_sale_list_paypal(
			LIST *paypal_item_list,
			LIST *event_list,
			char *paypal_date_time );

void ticket_sale_trigger(
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *state );

void ticket_sale_list_insert(
			LIST *ticket_sale_list );

double ticket_sale_total(
			LIST *ticket_sale_list );

void ticket_sale_list_trigger(
			LIST *ticket_sale_list );

/* Safely returns heap memory */
/* -------------------------- */
char *ticket_sale_list_display(
			LIST *ticket_sale_list );

LIST *ticket_sale_transaction_list(
			LIST *ticket_sale_list );

LIST *ticket_sale_list_steady_state(
			LIST *ticket_sale_list );

/* Returns static memory */
/* --------------------- */
char *ticket_sale_memo(
			char *event_name );

void ticket_sale_list_payor_entity_insert(
			LIST *ticket_sale_list );

TICKET_SALE *ticket_sale_new(
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address );

#endif

