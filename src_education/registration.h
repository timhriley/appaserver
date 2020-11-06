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
	char *student_full_name;
	char *street_address;
	char *season_name;
	int year;
	double registration_tuition;
	double registration_tuition_payment_total;
	double registration_tuition_refund_total;
	double registration_invoice_amount_due;
	char *registration_date_time;
	LIST *registration_enrollment_list;
	LIST *registration_tuition_payment_list;
} REGISTRATION;

REGISTRATION *registration_getset(
			LIST *registration_list,
			char *student_full_name,
			char *student_street_addres,
			char *season_name,
			int year );

REGISTRATION *registration_parse(
			char *input,
			boolean fetch_enrollment_list );

REGISTRATION *registration_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			boolean fetch_enrollment_list );

REGISTRATION *registration_seek(
			LIST *semester_registration_list,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

REGISTRATION *registration_new(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

REGISTRATION *registration_steady_state(
			REGISTRATION *registration,
			LIST *registration_enrollment_list,
			LIST *semester_offering_list );

/* ------------------------------------- */
/* Place functions in registration_fns.h */
/* ------------------------------------- */
#endif

