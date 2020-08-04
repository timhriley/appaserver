/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/program.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef PROGRAM_H
#define PROGRAM_H

#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *program_name;
} PROGRAM;

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
char *program_escape_name(	char *program_name );

PROGRAM *program_new(		char *program_name );

#endif
