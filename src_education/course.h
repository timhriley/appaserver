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

#define COURSE_INSERT_COLUMNS	"course_name,"		\
				"course_price"

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
char *course_escape_name(
			char *course_name );

FILE *course_insert_open(
			char *error_filename );

void course_insert_pipe(
			FILE *insert_pipe,
			char *course_name,
			double course_price );

#endif

