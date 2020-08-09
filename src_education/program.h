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

/* Structures */
/* ---------- */
typedef struct
{
	char *program_name;
	char *description;
} PROGRAM;

PROGRAM *program_new(	char *program_name );

PROGRAM *program_fetch( char *progam_name );

/* Safely returns heap memory */
/* -------------------------- */
char *program_where(	char *program_name );

#endif

