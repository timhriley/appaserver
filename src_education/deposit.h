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

#define DEPOSIT_INSERT_COLUMNS	"payor_full_name,"		\
				"payor_street_address,"		\
				"season_name,"			\
				"year,"				\
				"deposit_date_time,"		\
				"deposit_amount,"		\
				"transaction_fee,"		\
				"net_revenue,"			\
				"account_balance,"		\
				"transaction_ID,"		\
				"invoice_number,"		\
				"from_email_address"

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
	char *transaction_ID;
	char *invoice_number;
	char *from_email_address;
	/* ------- */
	/* Process */
	/* ------- */
	double deposit_remaining;
	double deposit_net_revenue;
	LIST *deposit_tuition_payment_list;
	LIST *deposit_program_payment_list;
	LIST *deposit_enrollment_list;
	LIST *deposit_registration_list;
	LIST *deposit_course_list;
	double deposit_tuition_payment_total;
	double deposit_program_payment_total;
	double deposit_gain_donation;
	double deposit_invoice_amount_due;
} DEPOSIT;

LIST *deposit_registration_list(
			LIST *deposit_tuition_payment_list );

LIST *deposit_fetch_program_payment_list(
			char *payor_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_program );

LIST *deposit_fetch_tuition_payment_list(
			char *payor_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_deposit,
			boolean fetch_enrollment );

double deposit_remaining(
			double deposit_amount,
			double deposit_tuition_payment_total );

double deposit_net_revenue(
			double deposit_amount,
			double transaction_fee );

/* Returns static memory */
/* --------------------- */
char *deposit_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

DEPOSIT *deposit_calloc(
			void );

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

double deposit_tuition_payment_total(
			LIST *deposit_tuition_payment_list );

double deposit_program_payment_total(
			LIST *deposit_program_payment_list );

DEPOSIT *deposit_steady_state(
			double deposit_amount,
			double transaction_fee,
			LIST *deposit_tuition_payment_list,
			LIST *deposit_program_payment_list,
			/* ----------------------------- */
			/* Don't take anything from here */
			/* ----------------------------- */
			DEPOSIT *deposit );

void deposit_update(
			double deposit_payment_total,
			double deposit_net_revenue,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

FILE *deposit_update_open(
			void );

LIST *deposit_enrollment_list(
			LIST *deposit_payment_list );

double deposit_invoice_amount_due(
			LIST *deposit_registration_list );

double deposit_gain_donation(
			double deposit_amount,
			double deposit_payment_total );

void deposit_list_insert(
			LIST *deposit_list );

FILE *deposit_insert_open(
			char *error_filename );

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
			char *transaction_ID,
			char *invoice_number,
			char *from_email_address );

void deposit_list_payment_insert(
			LIST *deposit_list );

void deposit_list_program_payment_insert(
			LIST *deposit_list );

void deposit_list_enrollment_insert(
			LIST *deposit_list );

void deposit_list_registration_insert(
			LIST *deposit_list );

void deposit_list_student_insert(
			LIST *deposit_list );

void deposit_list_student_entity_insert(
			LIST *deposit_list );

void deposit_list_payor_entity_insert(
			LIST *deposit_list );

void deposit_list_enrollment_trigger(
			char *season_name,
			int year,
			LIST *deposit_list );

void deposit_enrollment_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

void deposit_list_payment_trigger(
			char *season_name,
			int year,
			LIST *deposit_list );

void deposit_list_program_payment_trigger(
			char *season_name,
			int year,
			LIST *deposit_list );

void deposit_tuition_payment_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

LIST *deposit_course_name_list(
			LIST *deposit_list );

void deposit_program_payment_trigger(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

LIST *deposit_tuition_payment_list(
			char *season_name,
			int year,
			char *item_title_P,
			double gross_revenue_H,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit );

LIST *deposit_program_payment_list(
			char *season_name,
			int year,
			char *item_title_P,
			double gross_revenue_H,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit );

#endif

