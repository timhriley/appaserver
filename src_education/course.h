/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/course.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef COURSE_H
#define COURSE_H

#include "boolean.h"
#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define COURSE_TABLE		"course"

/* Structures */
/* ---------- */
typedef struct
{
	char *course_name;
	char *program_name;
	double course_price;
	char *description;
} COURSE;

COURSE *course_fetch(	char *course_name );

COURSE *course_new(	char *course_name );

COURSE *course_parse(	char *input );

/* Returns static memory */
/* --------------------- */
char *course_primary_where(
			char *course_name );

/* Returns static memory */
/* --------------------- */
char *course_name_escape(
			char *course_name );

#endif

