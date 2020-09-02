/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/offering_fns.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef OFFERING_FNS_H
#define OFFERING_FNS_H

#include "list.h"
#include "boolean.h"
#include "account.h"

double offering_course_price(
			LIST *offering_list,
			char *course_name,
			char *season_name,
			int year );

int offering_class_capacity(
			LIST *offering_list,
			char *course_name,
			char *season_name,
			int year );

int offering_enrollment_count(
			LIST *enrollment_list );

int offering_capacity_available(
			int class_capacity,
			int offering_enrollment_count );

void offering_refresh(
			int offering_enrollment_count,
			int offering_capacity_available,
			char *course_name,
			char *season_name,
			int year );

/* Returns program memory */
/* ---------------------- */
char *offering_select(	void );

/* Safely returns heap memory */
/* -------------------------- */
char *offering_primary_where(
			char *course_name,
			char *season_name,
			int year );

LIST *offering_system_list(
			char *sys_string,
			boolean fetch_course,
			boolean fetch_enrollment_list );

/* Safely returns heap memory */
/* --------------------------- */
char *offering_sys_string(
			char *where );

FILE *offering_update_open(
			void );

LIST *offering_enrollment_list(
			char *course_name,
			char *season_name,
			int year );

ACCOUNT *offering_revenue_account(
			void );

void offering_update(	int enrollment_count,
			int capacity_available,
			char *course_name,
			char *season_name,
			int year );

#endif

