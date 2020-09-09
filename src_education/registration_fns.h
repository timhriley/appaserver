/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/registration_fns.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef REGISTRATION_FNS_H
#define REGISTRATION_FNS_H

#include "boolean.h"
#include "list.h"

double registration_tuition(
			LIST *enrollment_list );

double registration_payment_total(
			LIST *registration_payment_list );

double registration_invoice_amount_due(
			double registration_tuition,
			double payment_total );

void registration_enrollment_list_refresh(
			LIST *registration_enrollment_list );

/* Returns program memory */
/* ---------------------- */
char *registration_select(
			void );

/* Returns static memory */
/* --------------------- */
char *registration_primary_where(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

/* Returns static memory */
/* --------------------- */
char *registration_escape_full_name(
			char *full_name );

LIST *registration_enrollment_list(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

LIST *registration_payment_list(
			LIST *registration_enrollment_list );

double registration_tuition_total(
			LIST *registration_list );

LIST *registration_system_list(
			char *sys_string,
			boolean fetch_enrollment_list );

char *registration_sys_string(
			char *where );

void registration_update(
			double registration_tuition,
			double registration_payment_total,
			double registration_invoice_amount_due,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year );

FILE *registration_update_open(
			void );

double registration_tuition_total(
			LIST *deposit_registration_list );

#endif

