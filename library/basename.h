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
		char *argv_0,
		boolean strip_extension );

/* Returns static memory */
/* --------------------- */
char *basename_extension(
		char *argv_0 );

/* Returns static memory or "" */
/* --------------------------- */
char *basename_directory(
		char *argv_0 );

/* Returns static memory */
/* --------------------- */
char *basename_filename(
		char *argv_0 );

/* To retire */
/* --------- */
char *basename_get_base_name( char *argv_0, boolean strip_extension );
char *basename_get_extension( char *argv_0 );
char *basename_get_directory( char *argv_0 );
char *basename_get_filename( char *argv_0 );

#endif
