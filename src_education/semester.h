/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/semester.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef SEMESTER_H
#define SEMESTER_H

#include "boolean.h"
#include "list.h"
#include "offering.h"
#include "registration.h"
#include "enrollment.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *season_name;
	int year;
	LIST *offering_list;
	LIST *registration_list;
} SEMESTER;

SEMESTER *semester_new(	char *season_name,
			int year );

SEMESTER *semster_fetch(
			char *season_name,
			int year );

ENROLLMENT *semester_enrollment_getset(
			LIST *semester_registration_list,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

/* Safely returns heap memory */
/* -------------------------- */
char *semester_primary_where(
			char *season_name,
			int year );

#endif

