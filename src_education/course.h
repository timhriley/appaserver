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
#include "program.h"

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
	char *description;
	char *program_name;
	PROGRAM *program;
} COURSE;

COURSE *course_fetch(	char *course_name,
			boolean fetch_program,
			boolean fetch_alias_list );

COURSE *course_new(	char *course_name );

COURSE *course_parse(	char *input,
			boolean fetch_program,
			boolean fetch_alias_list );

/* Returns static memory */
/* --------------------- */
char *course_primary_where(
			char *course_name );

/* Returns heap memory */
/* ------------------- */
char *course_name_escape(
			char *course_name );
char *course_escape_name(
			char *course_name );

char *course_program_name(
			COURSE *course );

#endif

