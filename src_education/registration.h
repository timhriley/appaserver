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
#include "semester.h"
#include "offering.h"
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
	/* Input */
	/* ----- */
	ENTITY *student_entity;
	SEMESTER *semester;
	char *registration_date_time;

	/* Process */
	/* ------- */
	LIST *enrollment_list;
	double tuition;
	double tuition_payment_total;
	double tuition_refund_total;
	double invoice_amount_due;
	LIST *tuition_payment_list;
	LIST *tuition_refund_list;
} REGISTRATION;

REGISTRATION *registration_parse(
			char *input,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program );

REGISTRATION *registration_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program );

REGISTRATION *registration_new(
			ENTITY *student_entity,
			SEMESTER *semester,
			char *registration_date_time );

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
			boolean fetch_program );

char *registration_sys_string(
			char *where );

FILE *registration_insert_open(
			char *error_filename );

void registration_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *registration_date_time );

void registration_list_fetch_update(
			LIST *registration_list );

void registration_fetch_update(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year );

LIST *registration_list(
			char *where );

LIST *registration_list_paypal(
			char *season_name,
			int year,
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *semester_offering_list,
			/* ----------------------------------------- */
			/* Seek out existing enrollments for refunds */
			/* ----------------------------------------- */
			LIST *paypal_deposit_list );

REGISTRATION *registration_paypal(
			SEMESTER *semester,
			ENTITY *student_entity,
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			OFFERING *offering,
			/* ----------------------------------------- */
			/* Seek out existing enrollments for refunds */
			/* ----------------------------------------- */
			LIST *paypal_deposit_list );

LIST *registration_list_enrollment_list(
			LIST *registration_list );

#endif

