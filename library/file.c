/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/file.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
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

char *file_temp_filespecification( char *key )
{
	char temp_filespecification[ 128 ];

	if ( key && *key )
	{
		sprintf(temp_filespecification,
			"/tmp/%s_temp_%d.txt",
			key,
			getpid() );
	}
	else
	{
		sprintf(temp_filespecification,
			"/tmp/temp_%d.txt",
			getpid() );
	}

	return strdup( temp_filespecification );
}

void file_remove( char *filespecification )
{
	char system_string[ 1024 ];

	if ( !filespecification )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: filespecification is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm %s 2>/dev/null",
		filespecification );

	if ( system( system_string ) ){}
}

