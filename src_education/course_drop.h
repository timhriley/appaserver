/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/course_drop.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef COURSE_DROP_H
#define COURSE_DROP_H

#include "list.h"
#include "boolean.h"
#include "offering.h"
#include "enrollment.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define COURSE_DROP_TABLE		"course_drop"

#define COURSE_DROP_PRIMARY_KEY		"student_full_name,"		\
					"student_street_address,"	\
					"course_name,"			\
					"season_name,"			\
					"year"

#define COURSE_DROP_MEMO		"Course Drop"

#define COURSE_DROP_INSERT_COLUMNS	"student_full_name,"		\
					"student_street_address,"	\
					"course_name,"			\
					"season_name,"			\
					"course_drop_date_time,"	\
					"year,"				\
					"refund_due_yn,"		\
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
	boolean refund_due;

	/* Process */
	/* ------- */
	char *course_drop_date_time;
	ENTITY *payor_entity;
	ENROLLMENT *enrollment;
	TRANSACTION *course_drop_transaction;
	char *transaction_date_time;
} COURSE_DROP;

COURSE_DROP *course_drop_new(
			ENTITY *student_entity,
			char *course_name,
			SEMESTER *semester );

COURSE_DROP *course_drop_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_enrollment,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_registration,
			boolean fetch_transaction );

COURSE_DROP *course_drop_parse(
			char *input,
			boolean fetch_enrollment,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_registration,
			boolean fetch_transaction );

boolean course_drop_set_transaction(
			int *transaction_seconds_to_add,
			COURSE_DROP *course_drop,
			char *course_drop_date_time,
			char *course_name,
			char *program_name,
			char *revenue_account,
			char *account_payable );

/* Returns static memory */
/* --------------------- */
char *course_drop_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

TRANSACTION *course_drop_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *course_name,
			char *program_name,
			double offering_course_price,
			char *offering_revenue_account,
			char *account_payable );

char *course_drop_system_string(
			char *where );

FILE *course_drop_update_open(
			void );

void course_drop_update(
			char *course_drop_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

LIST *course_drop_system_list(
			char *sys_string,
			boolean fetch_enrollment,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_registration,
			boolean fetch_transaction );

FILE *course_drop_insert_open(
			char *error_filename );

void course_drop_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *course_drop_date_time,
			boolean refund_due,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time );

FILE *course_drop_insert_open(
			char *error_filename );

LIST *course_drop_course_name_list(
			LIST *course_drop_list );

char *course_drop_memo(	char *course_name );

void course_drop_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *course_drop_list );

void course_drop_list_fetch_update(
			LIST *course_drop_list );

LIST *course_drop_registration_list(
			LIST *course_drop_list );

char *course_drop_list_display(
			LIST *course_drop_list );

COURSE_DROP *course_drop_steady_state(
			COURSE_DROP *course_drop,
			char *course_drop_date_time,
			ENTITY *payor_entity );

#endif

