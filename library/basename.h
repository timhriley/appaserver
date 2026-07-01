/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/basename.h 					*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef BASENAME_H
#define BASENAME_H

#include "boolean.h"

/* Returns static memory */
/* --------------------- */
char *basename_base_name(
		char *filename,
		boolean strip_extension );

/* Returns static memory */
/* --------------------- */
char *basename_extension(
		char *filename );

/* Returns static memory or "" */
/* --------------------------- */
char *basename_directory(
		char *filename );

/* Returns static memory */
/* --------------------- */
char *basename_filename(
		char *filename );

#endif
