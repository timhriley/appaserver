/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/subclassification.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef SUBCLASSIFICATION_H
#define SUBCLASSIFICATION_H

#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *subclassification_name;
} SUBCLASSIFICATION;

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
char *subclassification_escape_name(
				char *subclassification_name );

SUBCLASSIFICATION *subclassification_fetch(
				char *subclassification_name );

SUBCLASSIFICATION *subclassification_new(
				char *subclassification_name );

#endif
