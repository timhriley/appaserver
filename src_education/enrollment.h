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
	ENTITY *student_entity;
	COURSE *course;
	SEMESTER *semester;
	char *enrollment_date_time;

	/* Process */
	/* ------- */
	REGISTRATION *registration;
	OFFERING *offering;

	/* List of one */
	/* ----------- */
	LIST *course_drop_list;

	TRANSACTION *enrollment_transaction;
	char *transaction_date_time;
} ENROLLMENT;

ENROLLMENT *enrollment_new(
			ENTITY *student_entity,
			COURSE *course,
			SEMESTER *semester,
			char *enrollment_date_time );

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration,
			boolean fetch_transaction );

ENROLLMENT *enrollment_parse(
			char *input,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration,
			boolean fetch_transaction );

boolean enrollment_set_transaction(
			int *transaction_seconds_to_add,
			ENROLLMENT *enrollment,
			char *account_receivable,
			char *revenue_account,
			char *program_name,
			char *registration_date_time );

/* Returns static memory */
/* --------------------- */
char *enrollment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

TRANSACTION *enrollment_transaction(
			int *seconds_to_add,
			char *student_full_name,
			char *street_address,
			char *transaction_date_time,
			char *program_name,
			double offering_course_price,
			char *account_receivable,
			char *offering_revenue_account,
			char *account_payable,
			LIST *liability_entity_list );

char *enrollment_sys_string(
			char *where );

FILE *enrollment_update_open(
			void );

void enrollment_update(
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

LIST *enrollment_system_list(
			char *sys_string,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration,
			boolean fetch_transaction );

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

FILE *enrollment_insert_open(
			char *error_filename );

LIST *enrollment_course_name_list(
			LIST *enrollment_list );

char *enrollment_memo(	char *program_name );

void enrollment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *enrollment_list );

void enrollment_list_fetch_update(
			LIST *enrollment_list,
			char *season_name,
			int year );

void enrollment_fetch_update(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

void enrollment_list_update(
			LIST *enrollment_list,
			char *season_name,
			int year );

char *enrollment_list_first_program_name(
			LIST *enrollment_list );

char *enrollment_list_revenue_account(
			LIST *enrollment_list );

char *enrollment_list_display(
			LIST *enrollment_list );

ENROLLMENT *enrollment_integrity_fetch(
			char *full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

ENROLLMENT *enrollment_list_seek(
			char *full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			LIST *enrollment_list );

LIST *enrollment_list_course_drop_list(
			LIST *enrollment_list );

#endif

