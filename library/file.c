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
