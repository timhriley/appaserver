/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/semester.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef SEMESTER_H
#define SEMESTER_H

#include "boolean.h"
#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define SEMESTER_TABLE		"semester"

/* Structures */
/* ---------- */
typedef struct
{
	char *season_name;
	int year;
	LIST *offering_list;
	LIST *event_list;
} SEMESTER;

SEMESTER *semester_calloc(
			void );

SEMESTER *semester_new(
			char *season_name,
			int year );

SEMESTER *semester_fetch(
			char *season_name,
			int year,
			boolean fetch_offering_list );

/* Returns static memory */
/* --------------------- */
char *semester_primary_where(
			char *season_name,
			int year );

char *semester_system_string(
			char *where );

SEMESTER *semester_parse(
			char *input,
			boolean fetch_offering_list );

LIST *semester_offering_list(
			char *season_name,
			int year );

#endif

