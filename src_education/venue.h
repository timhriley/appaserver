/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/venue.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef VENUE_H
#define VENUE_H

#include "boolean.h"
#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define VENUE_TABLE			"venue"

/* Structures */
/* ---------- */
typedef struct
{
	char *venue_name;
	int capacity;
} VENUE;

VENUE *venue_new(	char *venue_name );

VENUE *venue_fetch(	char *venue_name );

VENUE *venue_calloc(
			void );

LIST *venue_system_list(
			char *sys_string );

VENUE *venue_parse(
			char *input );

char *venue_sys_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *venue_name_escape(
			char *venue_name );

char *venue_primary_where(
			char *venue_name );

#endif

