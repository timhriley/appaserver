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

#define TICKET_SALE_PRIMARY_KEY		"program_name,"			\
					"event_date,"			\
					"event_time,"			\
					"payor_full_name,"		\
					"payor_street_address"

#define TICKET_SALE_INSERT_COLUMNS	"program_name,"			\
					"event_date,"			\
					"event_time,"			\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"sale_date_time,"		\
					"quantity,"			\
					"ticket_price,"			\
					"extended_price,"		\
					"merchant_fees_expense,"	\
					"net_payment_amount,"		\
					"transaction_date_time,"	\
					"paypal_date_time"

#define TICKET_SALE_MEMO		"Ticket Sale"

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
	int quantity;
	double ticket_price;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	char *sale_date_time;
	EVENT *event;
	double extended_price;
	double net_payment_amount;

	TRANSACTION *ticket_sale_transaction;
	char *transaction_date_time;
} TICKET_SALE;

TICKET_SALE *ticket_sale_calloc(
			void );

TICKET_SALE *ticket_sale_fetch(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_event,
			boolean fetch_transaction );

TICKET_SALE *ticket_sale_parse(
			char *input,
			boolean fetch_event,
			boolean fetch_transaction );

TICKET_SALE *ticket_sale_steady_state(
			TICKET_SALE *ticket_sale,
			int quantity,
			double ticket_price,
			double merchant_fees_expense,
			char *sale_date_time );

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
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			int quantity,
			double ticket_price,
			double extended_price,
			double merchant_fees_expense,
			double net_payment_amount,
			char *transaction_date_time,
			char *paypal_date_time );

LIST *ticket_sale_system_list(
			char *system_string,
			boolean fetch_event,
			boolean fetch_transaction );

char *ticket_sale_system_string(
			char *where );

char *ticket_sale_primary_where(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address );

TRANSACTION *ticket_sale_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *program_name,
			double extended_price,
			double merchant_fees_expense,
			double net_payment_amount,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account );

void ticket_sale_update(
			char *sale_date_time,
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address );

FILE *ticket_sale_update_open(
			void );

void ticket_sale_list_insert(
			LIST *ticket_sale_list );

/* Safely returns heap memory */
/* -------------------------- */
char *ticket_sale_list_display(
			LIST *ticket_sale_list );

LIST *ticket_sale_transaction_list(
			LIST *ticket_sale_list );

/* Returns static memory */
/* --------------------- */
char *ticket_sale_memo(
			char *program_name );

void ticket_sale_list_payor_entity_insert(
			LIST *ticket_sale_list );

TICKET_SALE *ticket_sale_new(
			char *program_name,
			char *event_date,
			char *event_time,
			ENTITY *payor_entity );

LIST *ticket_sale_list_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *semester_event_list,
			double paypal_amount );

TICKET_SALE *ticket_sale_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			EVENT *event,
			double paypal_amount,
			int paypal_item_list_length );

TICKET_SALE *ticket_sale_integrity_fetch(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address );

char *ticket_sale_integrity_where(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address );

TICKET_SALE *ticket_sale_list_seek(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			LIST *ticket_sale_list );

boolean ticket_sale_list_any_exists(
			LIST *ticket_sale_list,
			LIST *existing_ticket_sale_list );

#endif

