/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include "list.h"
#include "transaction.h"
#include "offering.h"
#include "registration.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define ENROLLMENT_TABLE	"enrollment"

#define ENROLLMENT_PRIMARY_KEY	\
	"full_name,street_address,course_name,season_name,year"

#define ENROLLMENT_MEMO		"Course Enrollment"

/* Structures */
/* ---------- */
typedef struct
{
	OFFERING *offering;
	REGISTRATION *registration;
	TRANSACTION *enrollment_transaction;
	LIST *enrollment_payment_list;
} ENROLLMENT;

ENROLLMENT *enrollment_new(
			char *full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

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
			char *offering_revenue_account );

ENROLLMENT *enrollment_getset(
			LIST *enrollment_list,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

char *enrollment_sys_string(
			char *sys_string );

FILE *enrollment_update_open(
			void );

void enrollment_update(
			char *transaction_date_time,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

LIST *enrollment_system_list(
			char *sys_string );

LIST *enrollment_payment_list(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

ENROLLMENT *enrollment_parse(
			char *input,
			boolean fetch_payment_list,
			boolean fetch_offering );

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *enrollment_transaction_refresh(
			char *student_full_name,
			char *student_street_address,
			char *transaction_date_time,
			char *program_name,
			double payment_amount,
			char *memo,
			LIST *journal_list );

#endif

