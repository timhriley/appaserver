/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/registration.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "boolean.h"
#include "list.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define REGISTRATION_TABLE		"registration"

#define REGISTRATION_PRIMARY_KEY	"full_name,"			\
					"street_address,"		\
					"season_name,"			\
					"year"

#define REGISTRATION_INSERT_COLUMNS	"full_name,"			\
					"street_address,"		\
					"season_name,"			\
					"year,"				\
					"registration_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *student_entity;
	char *season_name;
	int year;
	double tuition;
	double tuition_payment_total;
	double tuition_refund_total;
	double invoice_amount_due;
	char *registration_date_time;
	LIST *enrollment_list;
	LIST *registration_tuition_payment_list;
	LIST *registration_tuition_refund_list;
} REGISTRATION;

REGISTRATION *registration_seek(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			LIST *registration_list );

REGISTRATION *registration_parse(
			char *input,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list );

REGISTRATION *registration_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list );

REGISTRATION *registration_new(
			ENTITY *student_entity,
			char *season_name,
			int year );

REGISTRATION *registration_steady_state(
			REGISTRATION *registration,
			LIST *enrollment_list,
			LIST *semester_offering_list );

double registration_tuition(
			LIST *enrollment_list,
			LIST *semester_offering_list );

double registration_tuition_payment_total(
			LIST *registration_tuition_payment_list );

double registration_tuition_refund_total(
			LIST *registration_tuition_refund_list );

double registration_invoice_amount_due(
			double registration_tuition,
			double tution_payment_total );

void registration_enrollment_list_refresh(
			LIST *enrollment_list );

/* Returns static memory */
/* --------------------- */
char *registration_primary_where(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

/* Returns static memory */
/* --------------------- */
char *registration_name_escape(
			char *full_name );
char *registration_escape_name(
			char *full_name );
char *registration_escape_full_name(
			char *full_name );

double registration_tuition_total(
			LIST *registration_list );

LIST *registration_system_list(
			char *sys_string,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_tuition_payment_list,
			boolean fetch_tuition_refund_list );

char *registration_sys_string(
			char *where );

void registration_update(
			double registration_tuition,
			double registration_tuition_payment_total,
			double registration_tuition_refund_total,
			double registration_invoice_amount_due,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

FILE *registration_update_open(
			void );

double registration_tuition_total(
			LIST *registration_list );

FILE *registration_insert_open(
			char *error_filename );

void registration_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *registration_date_time );

void registration_list_update(
			LIST *registration_list,
			char *season_name,
			int year );

void registration_trigger(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

LIST *registration_enrollment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program );

LIST *registration_tuition_payment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year );

LIST *registration_tuition_refund_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year );

FILE *registration_enrollment_insert_open(
			char *error_filename );

void registration_enrollment_list_insert(
			FILE *insert_pipe,
			LIST *enrollment_list );

TRANSACTION *registration_enrollment_seek_transaction(
			LIST *enrollment_list );

void registration_list_fetch_update(
			LIST *registration_list,
			char *season_name,
			int year );

void registration_fetch_update(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year );

LIST *registration_course_name_list(
			LIST *registration_list );

#endif

