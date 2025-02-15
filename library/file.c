/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/file.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <sys/stat.h>
#include "file.h"

boolean file_exists_boolean( char *filespecification )
{
	struct stat s;

	if ( !filespecification || !*filespecification ) return 0;

	return ( stat( filespecification, &s ) != -1 );
}

void file_skip_line( FILE *infile )
{
	char in_char;

	while ( 1 )
	{
		in_char = fgetc(infile);

		if ( in_char == EOF || in_char == FILE_LF ) return;
	}
}

