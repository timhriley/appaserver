/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/paypal_sweep.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PAYPAL_SWEEP_H
#define PAYPAL_SWEEP_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PAYPAL_SWEEP_TABLE		"paypal_sweep"

#define PAYPAL_SWEEP_PRIMARY_KEY					\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"paypal_date_time"

#define PAYPAL_SWEEP_INSERT_COLUMNS					\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"paypal_date_time,"		\
					"sweep_amount,"			\
					"transaction_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *payor_full_name;
	char *payor_street_address;
	char *paypal_date_time;
	double sweep_amount;

	/* Process */
	/* ------- */
	TRANSACTION *paypal_sweep_transaction;
	char *transaction_date_time;

} PAYPAL_SWEEP;

PAYPAL_SWEEP *paypal_sweep_new(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time );

PAYPAL_SWEEP *paypal_sweep_fetch(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			boolean fetch_transaction );

LIST *paypal_sweep_list(
			char *where,
			boolean fetch_transaction );

PAYPAL_SWEEP *paypal_sweep_calloc(
			void );

LIST *paypal_sweep_system_list(
			char *system_string,
			boolean fetch_transaction );

PAYPAL_SWEEP *paypal_sweep_parse(
			char *input,
			boolean fetch_transaction );

char *paypal_sweep_system_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *paypal_sweep_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time  );

void paypal_sweep_insert(
			PAYPAL_SWEEP *paypal_sweep );

FILE *paypal_sweep_insert_open(
			char *error_filename );

void paypal_sweep_insert_pipe(
			FILE *insert_pipe,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			double sweep_amount,
			char *transaction_date_time );

TRANSACTION *paypal_sweep_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			double sweep_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_cash );

FILE *paypal_sweep_update_open(
			void );

void paypal_sweep_update(
			char *transaction_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time );

void paypal_sweep_set_transaction(
			int *seconds_to_add,
			PAYPAL_SWEEP *paypal_sweep );

PAYPAL_SWEEP *paypal_sweep_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double paypal_amount );

/* Safely returns heap memory */
/* -------------------------- */
char *paypal_sweep_display(
			PAYPAL_SWEEP *paypal_sweep );

LIST *paypal_sweep_transaction_list(
			PAYPAL_SWEEP *paypal_sweep );

PAYPAL_SWEEP *paypal_sweep_list_seek(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			LIST *paypal_sweep_list );

boolean paypal_sweep_exists(
			PAYPAL_SWEEP *paypal_sweep,
			LIST *existing_paypal_sweep_list );

#endif

