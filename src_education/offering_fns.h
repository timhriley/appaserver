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

int offering_enrollment_count(
			LIST *enrollment_list );

int offering_capacity_available(
			int class_capacity,
			int enrollment_count );

/* Returns program memory */
/* ---------------------- */
char *offering_select(	void );

/* Safely returns heap memory */
/* -------------------------- */
char *offering_primary_where(
			char *course_name,
			char *season_name,
			int year );

LIST *offering_list(	char *where );

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

void offering_update(	int enrollment_count,
			int capacity_available,
			char *course_name,
			char *season_name,
			int year );

boolean offering_exists(
			char *season_name,
			int year,
			char *course_name );

void offering_fetch_update(
			char *course_name,
			char *season_name,
			int year );

void offering_list_fetch_update(
			LIST *course_name_list,
			char *season_name,
			int year );

LIST *offering_name_list(
			LIST *offering_list );

#endif

