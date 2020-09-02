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

/* Structures */
/* ---------- */
typedef struct
{
	char *season_name;
	int year;
	LIST *semester_offering_list;
	LIST *semester_registration_list;
} SEMESTER;

SEMESTER *semester_calloc(
			void );

SEMESTER *semester_new(
			char *season_name,
			int year );

SEMESTER *semester_fetch(
			char *season_name,
			int year,
			boolean fetch_offering_list,
			boolean fetch_registration_list );

/* Returns static memory */
/* --------------------- */
char *semester_primary_where(
			char *season_name,
			int year );

LIST *semester_offering_list(
			char *season_name,
			int year );

LIST *semester_registration_list(
			char *season_name,
			int year );

#endif

