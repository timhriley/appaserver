/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_payment.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef TUITION_PAYMENT_H
#define TUITION_PAYMENT_H

#include "boolean.h"
#include "list.h"
#include "registration.h"
#include "offering.h"
#include "semester.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TUITION_PAYMENT_TABLE		"tuition_payment"

#define TUITION_PAYMENT_PRIMARY_KEY					\
					"full_name,"			\
					"street_address,"		\
					"season_name,"			\
					"year,"				\
					"payment_date_time"

#define TUITION_PAYMENT_MEMO		"Tuition Payment"

#define TUITION_PAYMENT_INSERT_COLUMNS					\
					"full_name,"			\
					"street_address,"		\
					"season_name,"			\
					"year,"				\
					"payment_date_time,"		\
					"payment_amount,"		\
					"merchant_fees_expense,"	\
					"net_payment_amount,"		\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"transaction_date_time,"	\
					"paypal_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	ENTITY *student_entity;
	SEMESTER *semester;
	ENTITY *payor_entity;
	char *payment_date_time;
	double payment_amount;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	REGISTRATION *registration;
	double net_payment_amount;

	TRANSACTION *tuition_payment_transaction;
	char *transaction_date_time;
} TUITION_PAYMENT;

/* Prototypes */
/* ---------- */
TUITION_PAYMENT *tuition_payment_calloc(
			void );

TUITION_PAYMENT *tuition_payment_new(
			ENTITY *student_entity,
			SEMESTER *semester,
			char *payment_date_time );

TUITION_PAYMENT *tuition_payment_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payment_date_time,
			boolean fetch_registration );

TUITION_PAYMENT *tuition_payment_parse(
			char *input,
			boolean fetch_registration );

void tuition_payment_update(
			double net_payment_amount,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *payment_date_time );

char *tuition_payment_system_string(
			char *where );

FILE *tuition_payment_update_open(
			void );

char *tuition_payment_primary_where(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payment_date_time );

LIST *tuition_payment_system_list(
			char *system_string,
			boolean fetch_registration );

void tuition_payment_list_insert(
			LIST *tuition_payment_list );

FILE *tuition_payment_insert_open(
			char *error_filename );

void tuition_payment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payment_date_time,
			double payment_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *paypal_date_time );

void tuition_payment_list_student_entity_insert(
			LIST *tuition_payment_list );

void tuition_payment_list_student_insert(
			LIST *tuition_payment_list );

void tuition_payment_list_payor_entity_insert(
			LIST *tuition_payment_list );

LIST *tuition_payment_transaction_list(
			LIST *tution_payment_list );

/* Returns tuition_payment */
/* ----------------------- */
TUITION_PAYMENT *tuition_payment_steady_state(
			TUITION_PAYMENT *tuition_payment,
			double payment_amount,
			double merchant_transaction_fee,
			ENTITY *payor_entity );

void tuition_payment_list_set_transaction(
			int *seconds_to_add,
			LIST *tuition_payment_list );

/* ----------------------------------------------------- */
/* Sets tuition_payment->tuition_payment_transaction and
	tuition_payment->transaction_date_time		 */
/* ----------------------------------------------------- */
void tuition_payment_set_transaction(
			int *seconds_to_add,
			TUITION_PAYMENT *tuition_payment,
			char *cash_account_name,
			char *account_fees_expense,
			char *account_revenue );

TRANSACTION *tuition_payment_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			double payment_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *cash_account_name,
			char *account_fees_expense,
			char *account_receivable );

boolean tuition_payment_is_tuition(
			char *item_title_block );

TUITION_PAYMENT *tuition_payment_integrity_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year );

char *tuition_payment_integrity_where(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

boolean tuition_payment_list_any_exists(
			LIST *tuition_payment_list,
			LIST *existing_tuition_payment_list );

TUITION_PAYMENT *tuition_payment_seek(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *payment_date_time,
			LIST *tuition_payment_list );

double tuition_payment_total(
			LIST *tuition_payment_list );

double tuition_payment_fee_total(
			LIST *tuition_payment_list );

#endif

