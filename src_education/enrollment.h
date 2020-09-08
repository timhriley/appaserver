/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include "list.h"
#include "boolean.h"
#include "offering.h"
#include "registration.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define ENROLLMENT_TABLE	"enrollment"

#define ENROLLMENT_PRIMARY_KEY	\
	"full_name,street_address,course_name,season_name,year"

#define ENROLLMENT_MEMO		"Course Enrollment"

/* Structures */
/* ---------- */
typedef struct
{
	OFFERING *offering;
	REGISTRATION *registration;
	TRANSACTION *enrollment_transaction;
	LIST *enrollment_payment_list;
} ENROLLMENT;

ENROLLMENT *enrollment_new(
			char *full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

ENROLLMENT *enrollment_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_payment_list );

ENROLLMENT *enrollment_getset(
			LIST *enrollment_list,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

ENROLLMENT *enrollment_parse(
			char *input,
			boolean fetch_payment_list );

ENROLLMENT *enrollment_steady_state(
			REGISTRATION *registration,
			OFFERING *offering,
			LIST *enrollment_payment_list,
			/* ----------------------------- */
			/* Don't take anything from here */
			/* ----------------------------- */
			ENROLLMENT *enrollment );

/* ----------------------------------- */
/* Place functions in enrollment_fns.h */
/* ----------------------------------- */
#endif

