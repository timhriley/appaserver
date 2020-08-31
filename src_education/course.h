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

/* Structures */
/* ---------- */
typedef struct
{
	char *course_name;
	double course_price;
} COURSE;

COURSE *course_fetch(	char *course_name );

char *course_select(	void );

COURSE *course_new(	char *course_name );

/* See: attribute_list course */
/* -------------------------- */
COURSE *course_parse(	char *input );

/* Safely returns heap memory */
/* -------------------------- */
char *course_primary_where(
			char *course_name );

#endif

