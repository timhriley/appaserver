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
} SEMESTER;

/* Safely returns heap memory */
/* -------------------------- */
char *semester_where(
			char *season_name,
			int year );

LIST *semester_offering_list(
			char *season_name,
			int year );

#endif

