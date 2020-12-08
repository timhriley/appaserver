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

/* ------------------ */
/* See offering_fns.h */
/* ------------------ */

#endif

