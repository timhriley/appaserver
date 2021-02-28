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
	char *course_name;
	SEMESTER *semester;
	ENTITY *instructor_entity;
	double course_price;
	int class_capacity;
	char *revenue_account;

	/* Process */
	/* ------- */
	COURSE *course;
	int enrollment_count;
	int capacity_available;
} OFFERING;

/* Prototypes */
/* ---------- */
OFFERING *offering_new(
			char *course_name,
			SEMESTER *semester );

OFFERING *offering_fetch(
			char *course_name,
			char *season_name,
			int year,
			boolean fetch_course,
			boolean fetch_program );

OFFERING *offering_parse(
			char *input,
			boolean fetch_course,
			boolean fetch_program );

OFFERING *offering_list_seek(
			char *course_name,
			LIST *offering_list );

/* Safely returns heap memory */
/* -------------------------- */
char *offering_primary_where(
			char *course_name,
			char *season_name,
			int year );

LIST *offering_system_list(
			char *sys_string,
			boolean fetch_course,
			boolean fetch_program );

/* Safely returns heap memory */
/* --------------------------- */
char *offering_system_string(
			char *where );

void offering_fetch_update(
			char *course_name,
			char *season_name,
			int year );

void offering_list_fetch_update(
			LIST *offering_list );

LIST *offering_name_list(
			LIST *offering_list );

OFFERING *offering_calloc(
			void );

#endif

