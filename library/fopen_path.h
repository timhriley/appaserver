/* fopen_path.h */
/* ------------ */
/* Tim Riley    */
/* ------------ */

#ifndef FOPEN_PATH_H
#define FOPEN_PATH_H

#include <stdio.h>

FILE *fopen_path( char *baseless_filename, char *mode );
char *fopen_path_get_path_filename( void );

#endif
