#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "timlib.h"
#define MSGSIZE 63

extern int errno;

int main( int argc, char **argv )
{
	int fd;
	char buffer[ MSGSIZE + 1 ];
	char *fifo_filename;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s fifo_filename\n", argv[ 0 ] );
		exit( 1 );
	}

	fifo_filename = argv[ 1 ];

	/* Open for read/write because the write will force blocking. */
	/* ---------------------------------------------------------- */
	if ( ( fd = open( fifo_filename, O_RDWR ) ) < 0 ) 
	{
		perror( "Could not open fifo" );
		fprintf( stderr,
			 "Cannot open %s for read/write.\n",
			 fifo_filename );
		exit( 1 );
	}

	while( 1 )
	{
		memset( buffer, 0, MSGSIZE + 1 );

		if ( read( fd, buffer, MSGSIZE + 1 ) < 0 )
		{
			perror( "Cannot read from pipe." );
			exit( 1 );
		}

		rtrim( buffer );

		if ( strcmp( buffer, "bye" ) == 0 ) break;

		printf( "%s\n", buffer );
	}
	close( fd );
	return 0;
}

