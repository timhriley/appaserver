/* $APPASERVER_HOME/library/basename.h */
/* ----------------------------------- */

#ifndef BASENAME_H
#define BASENAME_H

#include "boolean.h"

char *basename_get_base_name( char *argv_0, boolean strip_extension );
char *basename_base_name( char *argv_0, boolean strip_extension );

char *basename_get_extension( char *argv_0 );
char *basename_extension( char *argv_0 );

char *basename_get_directory( char *argv_0 );
char *basename_directory( char *argv_0 );

char *basename_get_filename( char *argv_0 );
char *basename_filename( char *argv_0 );

#endif
