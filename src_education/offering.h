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
#include "course.h"
#include "account.h"

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
	LIST *enrollment_list;
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
			LIST *enrollment_list,
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

OFFERING *offering_parse(
			char *input,
			boolean fetch_course,
			boolean fetch_enrollment_list );

OFFERING *offering_seek(
			LIST *semester_offering_list,
			char *course_name,
			char *season_name,
			int year );

/* Returns static memory */
/* --------------------- */
char *offering_escape_course_name(
			char *course_name );

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

#endif

