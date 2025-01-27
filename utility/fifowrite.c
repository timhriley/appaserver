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
	int fd, nwrite;
	char buffer[ MSGSIZE + 1 ] = {0};
	char *fifo_filename;

	if ( argc != 2 )
	{
		fprintf(	stderr,
				"Usage: echo message | %s fifo_filename\n",
				argv[ 0 ] );
		exit( 1 );
	}

	fifo_filename = argv[ 1 ];

	if ( ( fd = open( fifo_filename, O_WRONLY ) ) < 0 ) 
	{
		perror( "Could not open pipe" );
		fprintf( stderr, "Cannot open %s for write.\n", fifo_filename );
		exit( 1 );
	}

	while( get_line( buffer, stdin ) )
	{
		if ( strlen( buffer ) > MSGSIZE )
		{
			fprintf( stderr, "Too long: %s\n", argv[ 1 ] );
			exit( 1 );
		}

		*( buffer + strlen( buffer ) ) = LF;

		if ( ( nwrite = write( fd, buffer, MSGSIZE + 1 ) ) <= 0 )
		{
			if ( nwrite == 0 ) errno = EAGAIN;
			perror( "Could not write to pipe" );
			exit( 1 );
		}
		memset( buffer, 0, sizeof( buffer ) );
	}
	close( fd );
	return 0;
}

