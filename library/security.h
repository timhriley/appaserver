/* $APPASERVER_HOME/library/security.h			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef SECURITY_H
#define SECURITY_H

/* Includes */
/* -------- */
#include <stdio.h>
#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

/* Returns heap memory */
/* ------------------- */
char *security_sql_injection_escape(
			char *source );

#endif
