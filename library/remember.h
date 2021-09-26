/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/remember.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef REMEMBER_H
#define REMEMBER_H

#include "list.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *control_string;
} REMEMBER;

/* REMEMBER operations */
/* ------------------- */
REMEMBER *remember_calloc(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *remember_button(	char *control_string );

#endif
