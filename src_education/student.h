/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/student.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef STUDENT_H
#define STUDENT_H

#include "boolean.h"
#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define STUDENT_TABLE		"student"

#define STUDENT_INSERT_COLUMNS	"full_name,"		\
				"street_address"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
} STUDENT;

STUDENT *student_new(	char *student_full_name,
			char *street_address );

/* Returns static memory */
/* --------------------- */
char *student_name_escape(
			char *student_full_name );
char *student_escape_name(
			char *student_full_name );

FILE *student_insert_open(
			char *error_filename );

void student_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address );

#endif

