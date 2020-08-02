/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include "list.h"
#include "offering.h"
#include "registration.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	OFFERING *offering;
	REGISTRATION *registration;
} ENROLLMENT;

/* No need to strdup() in. Returns everything on the heap. */
/* ------------------------------------------------------- */
ENROLLMENT *enrollment_new(
			char *full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

LIST *enrollment_offering_enrollment_list(
			char *course_name,
			char *season_name,
			int year );

LIST *enrollment_registration_enrollment_list(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year );

void enrollment_list_refresh(
			LIST *enrollment_list );

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

/* Safely returns heap memory */
/* -------------------------- */
char *enrollment_primary_where(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

#endif

