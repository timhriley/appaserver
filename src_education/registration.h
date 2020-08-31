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
#define REGISTRATION_PRIMARY_KEY "full_name,street_address,season_name,year"

/* Structures */
/* ---------- */
typedef struct
{
	char *student_full_name;
	char *street_address;
	char *season_name;
	int year;
	double registration_tuition;
	double registration_payment_total;
	double registration_invoice_amount_due;
	char *registration_date_time;
	LIST *registration_enrollment_list;
} REGISTRATION;

REGISTRATION *registration_getset(
			LIST *semester_registration_list,
			char *student_full_name,
			char *student_street_addres,
			char *season_name,
			int year );

double registration_tuition(
			LIST *enrollment_list );

double registration_payment_total(
			LIST *enrollment_list );

double registration_invoice_amount_due(
			double registration_tuition,
			double registration_payment_total );

void registration_payment_list_refresh(
			LIST *registration_payment_list );

void registration_enrollment_list_refresh(
			LIST *registration_enrollment_list );

void registration_refresh(
			double registration_tuition,
			double registration_payment_total,
			double registration_invoice_amount_due,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

/* Returns program memory */
/* ---------------------- */
char *registration_select(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *registration_primary_where(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

REGISTRATION *registration_parse(
			char *input );

REGISTRATION *registration_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

REGISTRATION *registration_seek(
			LIST *semester_registration_list,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

/* Returns static memory */
/* --------------------- */
char *registration_escape_full_name(
			char *full_name );

REGISTRATION *registration_new(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

LIST *registration_list(
			char *where_clause );

#endif

