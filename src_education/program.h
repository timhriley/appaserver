/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_program/program.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PROGRAM_H
#define PROGRAM_H

#include "boolean.h"
#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PROGRAM_TABLE		"program"

#define PROGRAM_INSERT_COLUMNS	"program_name"

/* Structures */
/* ---------- */
typedef struct
{
	char *program_name;
	char *description;
} PROGRAM;

PROGRAM *program_new(	char *program_name );

FILE *program_insert_open(
			char *error_filename );

void program_insert_pipe(
			FILE *insert_pipe,
			char *program_name );

#endif

