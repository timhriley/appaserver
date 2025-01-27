/* fopen_path.c */
/* ------------ */
/* Tim Riley    */
/* ------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "environ.h"
#include "fopen_path.h"

static char *global_fopen_path_filename = {0};

char *fopen_path_get_path_filename( void )
{
	return global_fopen_path_filename;
}

FILE *fopen_path( char *baseless_filename, char *mode )
{
	char *path;
	char *directory;
	LIST *directory_list;
	FILE *f;
	char filename[ 1024 ];

	/* Force search */
	/* ------------ */
	set_path( "/etc" );
	set_path( "." );

	path = getenv( "PATH" );
	if ( !path )
	{
		fprintf( stderr, 
			 "%s/%s(%s) cannot get PATH\n", 
			 __FILE__,
			 __FUNCTION__,
			 baseless_filename );
		exit( 1 );
	}
	directory_list = list_string2list( path, ':' );
	if ( !list_rewind( directory_list ) )
	{
		fprintf( stderr,
			 "Warning: %s/%s(%s) got empth PATH\n",
			 __FILE__,
			 __FUNCTION__,
			 baseless_filename );
	}
	do {
		directory = list_get_string( directory_list );
		sprintf( filename, "%s/%s", directory, baseless_filename );
		if ( ( f = fopen( filename, mode ) ) )
		{
			global_fopen_path_filename = strdup( filename );
			return f;
		}
	} while( list_next( directory_list ) );
	return (FILE *)0;
} /* fopen_path() */

