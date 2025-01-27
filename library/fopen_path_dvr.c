#include <stdio.h>
#include <stdlib.h>
#include "fopen_path.h"

int main()
{
	FILE *f;
	char buffer[ 1024 ];
	char *filename = "sql_parameter_file.dat";

	f = fopen_path( filename, "r" );
	if ( !f )
	{
		fprintf( stderr, "Error: cannot find file = %s\n", filename );
	}
	while( fgets( buffer, 1023, f ) ) printf( "%s", buffer );
	fclose( f );
	return 0;
}
