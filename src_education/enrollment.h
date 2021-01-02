/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include "list.h"
#include "boolean.h"
#include "offering.h"
#include "registration.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define ENROLLMENT_TABLE		"enrollment"

#define ENROLLMENT_PRIMARY_KEY		\
	"full_name,street_address,course_name,season_name,year"

#define ENROLLMENT_MEMO			"Enrollment"

#define ENROLLMENT_INSERT_COLUMNS	"full_name,"		\
					"street_address,"	\
					"course_name,"		\
					"season_name,"		\
					"year,"			\
					"transaction_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	OFFERING *offering;
	REGISTRATION *registration;

	/* Process */
	/* ------- */
	LIST *tuition_payment_list;
	LIST *tuition_refund_list;
	char *enrollment_cancelled_date;

	TRANSACTION *enrollment_transaction;
	char *transaction_date_time;
} ENROLLMENT;

/* Builds enrollment and registration */
/* ---------------------------------- */
ENROLLMENT *enrollment_new(
			ENTITY *student_entity,
			char *course_name,
			char *season_name,
			int year );

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list,
			boolean fetch_offering,
			boolean fetch_registration );

ENROLLMENT *enrollment_parse(
			char *input,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list,
			boolean fetch_offering,
			boolean fetch_registration );

ENROLLMENT *enrollment_steady_state(
			int *transaction_seconds_to_add,
			ENROLLMENT *enrollment,
			double deposit_amount );

void enrollment_set_transaction(
			int *transaction_seconds_to_add,
			ENROLLMENT *enrollment,
			char *account_receivable,
			char *revenue_account );

/* Returns static memory */
/* --------------------- */
char *enrollment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

TRANSACTION *enrollment_transaction(
			char *student_full_name,
			char *street_address,
			char *registration_date_time
				/* transaction_date_time */,
			char *program_name,
			double offering_course_price,
			char *account_receivable,
			char *offering_revenue_account,
			int seconds_to_add );

char *enrollment_sys_string(
			char *where );

FILE *enrollment_update_open(
			void );

void enrollment_cancelled_date_update(
			FILE *update_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *cancelled_date );

LIST *enrollment_system_list(
			char *sys_string,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list,
			boolean fetch_offering,
			boolean fetch_registration );

LIST *enrollment_tuition_payment_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

LIST *enrollment_tuition_refund_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

FILE *enrollment_insert_open(
			char *error_filename );

void enrollment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *transaction_date_time );

LIST *enrollment_course_name_list(
			LIST *enrollment_list );

void enrollment_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

char *enrollment_memo(	char *program_name );

void enrollment_list_steady_state(
			int *transaction_seconds_to_add,
			LIST *registration_enrollment_list,
			double deposit_amount );

void enrollment_list_cancelled_update(
			LIST *enrollment_list,
			char *season_name,
			int year );

void enrollment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *enrollment_list );

#endif

