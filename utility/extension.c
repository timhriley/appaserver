/* extension.c */
/* ----------- */
/* Tim Riley   */
/* ----------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trim_extension( char *filename );
char *leave_extension( char *filename );

int main( int argc, char **argv )
{
	int exit_usage = 0;

	if ( argc != 3 )
		exit_usage = 1;
	else
	if ( strcmp( argv[ 2 ], "trim" ) == 0 )
		printf( "%s\n", trim_extension( argv[ 1 ] ) );
	else
	if ( strcmp( argv[ 2 ], "leave" ) == 0 )
		printf( "%s\n", leave_extension( argv[ 1 ] ) );
	else
		exit_usage = 1;

	if ( exit_usage )
	{
		fprintf(stderr,
			"Usage: %s filename_with_extension [trim/leave]\n",
			argv[ 0 ] );
		exit( 1 );
	}
	return 0;

} /*  main() */


char *trim_extension( char *filename )
{
	char *end;

	end = filename + strlen( filename );
	while( end != filename )
	{
		if ( *--end == '.' )
		{
			*end = '\0';
			return filename;
		}
	}
	return filename;

} /* trim_extension() */

char *leave_extension( char *filename )
{
	char *end;

	end = filename + strlen( filename );
	while( end != filename )
	{
		if ( *--end == '.' )
		{
			return end + 1;
		}
	}
	return filename;

} /* leave_extension() */

