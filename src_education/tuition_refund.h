/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_refund.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef TUITION_REFUND_H
#define TUITION_REFUND_H

#include "boolean.h"
#include "list.h"
#include "registration.h"
#include "semester.h"
#include "entity.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TUITION_REFUND_TABLE		"tuition_refund"

#define TUITION_REFUND_PRIMARY_KEY	"student_full_name,"		\
					"student_street_address,"	\
					"season_name,"			\
					"year,"				\
					"refund_date_time"

#define TUITION_REFUND_INSERT_COLUMNS	"student_full_name,"		\
					"student_street_address,"	\
					"season_name,"			\
					"year,"				\
					"refund_date_time,"		\
					"refund_amount,"		\
					"merchant_fees_expense,"	\
					"net_refund_amount,"		\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"transaction_date_time,"	\
					"paypal_date_time"

#define TUITION_REFUND_MEMO		"Tuition refund"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	ENTITY *student_entity;
	SEMESTER *semester;
	char *refund_date_time;
	double refund_amount;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	ENTITY *payor_entity;
	REGISTRATION *registration;
	double net_refund_amount;

	TRANSACTION *tuition_refund_transaction;
	char *transaction_date_time;
} TUITION_REFUND;

TUITION_REFUND *tuition_refund_calloc(
			void );

TUITION_REFUND *tuition_refund_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time,
			boolean fetch_registration );

TUITION_REFUND *tuition_refund_parse(
			char *input,
			boolean fetch_registration );

TUITION_REFUND *tuition_refund_steady_state(
			TUITION_REFUND *tuition_refund,
			double refund_amount,
			double merchant_fees_expense,
			ENTITY *payor_entity );

FILE *tuition_refund_insert_open(
			char *error_filename );

void tuition_refund_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time,
			double refund_amount,
			double merchant_fees_expense,
			double net_refund_amount,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *paypal_date_time );

LIST *tuition_refund_system_list(
			char *system_string,
			boolean fetch_registration );

char *tuition_refund_system_string(
			char *where );

char *tuition_refund_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time );

void tuition_refund_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *tuition_refund_list );

/* ------------------------------------------------------- */
/* Sets tuition_refund->tuition_refund_transaction and
	tuition_refund->transaction_date_time		   */
/* ------------------------------------------------------- */
void tuition_refund_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_REFUND *tuition_refund,
			char *account_payable,
			char *account_cash,
			char *account_fees_expense );

TRANSACTION *tuition_refund_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			double refund_amount,
			double merchant_fees_expense,
			double net_refund_amount,
			char *account_payable,
			char *account_cash,
			char *account_fees_expense );

void tuition_refund_update(
			double refund_amount,
			double net_refund_amount,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time );

FILE *tuition_refund_update_open(
			void );

void tuition_refund_list_insert(
			LIST *tuition_refund_list );

/* Safely returns heap memory */
/* -------------------------- */
char *tuition_refund_list_display(
			LIST *tuition_refund_list );

LIST *tuition_refund_transaction_list(
			LIST *tuition_refund_list );

LIST *tuition_refund_list_steady_state(
			LIST *tuition_refund_list,
			double refund_amount,
			double merchant_fees_expense,
			ENTITY *payor_entity );

void tuition_refund_list_payor_entity_insert(
			LIST *tuition_refund_list );

void tuition_refund_list_student_insert(
			LIST *tuition_refund_list );

void tuition_refund_list_student_entity_insert(
			LIST *refund_list );

TUITION_REFUND *tuition_refund_new(
			ENTITY *student_entity,
			SEMESTER *semester,
			char *refund_date_time );

TUITION_REFUND *tuition_refund_seek(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *refund_date_time,
			LIST *tuition_refund_list );

boolean tuition_refund_list_any_exists(
			LIST *tuition_refund_list,
			LIST *existing_tuition_refund_list );

double tuition_refund_amount(
			double refund_amount );

double tuition_refund_total(
			LIST *tuition_refund_list );

double tuition_refund_fee_total(
			LIST *tuition_refund_list );

LIST *tuition_refund_list_registration_list(
			LIST *tuition_refund_list );

#endif

