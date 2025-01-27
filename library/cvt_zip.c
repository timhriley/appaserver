#include <stdio.h>
main()
{
	FILE *join, *search_z;
	char instring[ 101 ];
	int longest_line = 0;

	join = fopen( "join.dat", "r" );
	search_z = fopen( "search_z.dat", "w" );

	if ( !join || !search_z )
	{
		fprintf( stderr, "file open error\n" );
		exit( 1 );
	}

	while( fgets( instring, 100, join ) )
	{
		instring[ strlen( instring ) - 1 ] = '\0';

		if ( strlen( instring ) > longest_line )
			longest_line = strlen( instring );

		if ( only_2_bars_exists( instring ) )
			fprintf( search_z, "%s||\n", instring );
		else
			fprintf( search_z, "%s\n", instring );
	}

	printf( "longest line: %d\n", longest_line );
}

only_2_bars_exists( char *instring )
{
	int num_bars = 0;

	while( *instring++ )
		if ( *instring == '|' )
			num_bars++;

	return ( num_bars == 2 );
}
