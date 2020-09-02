/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/offering.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef OFFERING_H
#define OFFERING_H

#include "boolean.h"
#include "list.h"
#include "account.h"
#include "course.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define OFFERING_PRIMARY_KEY	"course_name,season_name,year"

/* Structures */
/* ---------- */
typedef struct
{
	char *course_name;
	char *season_name;
	int year;
	char *instructor_full_name;
	char *street_address;
	int class_capacity;
	int offering_enrollment_count;
	int offering_capacity_available;
	ACCOUNT *offering_revenue_account;
	COURSE *course;
	LIST *offering_enrollment_list;
} OFFERING;

/* Prototypes */
/* ---------- */
OFFERING *offering_new(
			char *course_name,
			char *season_name,
			int year );

OFFERING *offering_fetch(
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_course,
			boolean fetch_enrollment_list );

OFFERING *offering_getset(
			LIST *offering_list,
			char *course_name,
			char *season_name,
			int year );

OFFERING *offering_parse(
			char *input,
			boolean fetch_course,
			boolean fetch_enrollment_list );

OFFERING *offering_seek(
			LIST *semester_offering_list,
			char *course_name,
			char *season_name,
			int year );

OFFERING *offering_steady_state(
			char *course_name,
			char *season_name,
			int year,
			char *instructor_full_name,
			char *street_address,
			double class_capacity,
			LIST *offering_enrollment_list );

#endif

