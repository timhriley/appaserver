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
#include "entity.h"
#include "course.h"
#include "semester.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define OFFERING_PRIMARY_KEY	"course_name,season_name,year"

#define OFFERING_TABLE		"offering"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	COURSE *course;
	SEMESTER *semester;
	ENTITY *instructor_entity;

	double course_price;
	int class_capacity;
	char *revenue_account;

	/* Process */
	/* ------- */
	int enrollment_count;
	int capacity_available;
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

OFFERING *offering_parse(
			char *input,
			boolean fetch_course,
			boolean fetch_enrollment_list );

OFFERING *offering_seek(
			char *course_name,
			LIST *offering_list );

OFFERING *offering_steady_state(
			OFFERING *offering,
			LIST *offering_enrollment_list );

OFFERING *offering_course_name_seek(
			char *course_name,
			LIST *offering_list );

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
			int year,
			boolean fetch_course );

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

void offering_trigger(
			char *course_name,
			char *season_name,
			int year );

#endif

