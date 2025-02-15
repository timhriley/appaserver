/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/file.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "boolean.h"

#define FILE_LF	10
#define FILE_CR	13

/* Usage */
/* ----- */
boolean file_exists_boolean(
		char *filespecification );

/* Usage */
/* ----- */
void file_skip_line(
		FILE *infile );

#endif
