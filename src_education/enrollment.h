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

#define ENROLLMENT_PRIMARY_KEY		"student_full_name,"		\
					"student_street_address,"	\
					"course_name,"			\
					"season_name,"			\
					"year"

#define ENROLLMENT_MEMO			"Enrollment"

#define ENROLLMENT_INSERT_COLUMNS	"student_full_name,"		\
					"student_street_address,"	\
					"course_name,"			\
					"season_name,"			\
					"year,"				\
					"enrollment_date_time,"		\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"transaction_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	ENTITY *student_entity;
	char *course_name;
	SEMESTER *semester;

	/* Process */
	/* ------- */
	char *enrollment_date_time;
	ENTITY *payor_entity;
	OFFERING *offering;
	REGISTRATION *registration;

	/* List of one */
	/* ----------- */
	LIST *course_drop_list;

	TRANSACTION *enrollment_transaction;
	char *transaction_date_time;
} ENROLLMENT;

ENROLLMENT *enrollment_new(
			ENTITY *student_entity,
			char *course_name,
			SEMESTER *semester );

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration );

ENROLLMENT *enrollment_parse(
			char *input,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_registration );

boolean enrollment_set_transaction(
			int *transaction_seconds_to_add,
			ENROLLMENT *enrollment,
			char *account_receivable,
			char *revenue_account,
			LIST *liability_entity_list );

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
			char *payor_full_name,
			char *payor_address,
			char *transaction_date_time,
			char *course_name,
			char *program_name,
			double offering_course_price,
			char *account_receivable,
			char *offering_revenue_account,
			LIST *liability_entity_list );

char *enrollment_system_string(
			char *where );

FILE *enrollment_update_open(
			void );

void enrollment_update(
			char *enrollment_date_time,
			char *payor_full_name,
			char *payor_street_address,
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
			boolean fetch_registration );

void enrollment_list_insert(
			LIST *enrollment_list );

FILE *enrollment_insert_open(
			char *error_filename );

void enrollment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *enrollment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time );

FILE *enrollment_insert_open(
			char *error_filename );

LIST *enrollment_list_offering_list(
			LIST *enrollment_list );

char *enrollment_memo(	char *course_name );

void enrollment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *enrollment_list );

void enrollment_list_fetch_update(
			LIST *enrollment_list );

char *enrollment_list_first_program_name(
			LIST *enrollment_list );

char *enrollment_list_display(
			LIST *enrollment_list );

ENROLLMENT *enrollment_integrity_fetch(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

ENROLLMENT *enrollment_list_seek(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			LIST *enrollment_list );

LIST *enrollment_list_course_drop_list(
			LIST *enrollment_list );

ENROLLMENT *enrollment_steady_state(
			ENROLLMENT *enrollment,
			char *enrollment_date_time,
			ENTITY *payor_entity );

LIST *enrollment_list_registration_list(
			LIST *enrollment_list );

#endif

