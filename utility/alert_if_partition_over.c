/* utility/alert_if_partition_over.c */
/* --------------------------------- */

#include <stdio.h>

main( int argc; char **argv )
{
	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s capacity_percentage\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	alert_if_partition_over( argv[ 1 ] ) ;

} /* main() */


alert_if_partition_over( char *capacity_percentage )
{
	char buffer[ 1024 ];
	FILE *p;

	p = popen( "df", "r" );
	while( fgets( buffer, 1023, p ) )
	{
		buffer[ strlen( buffer ) ] = '\0';

		if ( strcmp( buffer, "/dev", 4 ) == 0 )
		{
			char percentage[ 1024 ];
			char device[ 1024 ];
		}


	}
	
}

char *column( char *d, char *s, int which )
{
	int space_flag = 0;
	static ret_buffer[ 1024 ];

	while( *s )
	{
		if ( !space_flag )
		{
			if ( which )
		}
		else
		{
		}
		s++;
	}

	return ret_buffer;
}
