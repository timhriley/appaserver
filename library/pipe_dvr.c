#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pipe.h"
#include "timlib.h"

#define IP_ADDRESS		"192.168.1.103"
#define READ_PORT_NUMBER	12345
#define WRITE_PORT_NUMBER	12345

void my_pipe_send( void );
void my_pipe_receive( void );

int main( int argc, char **argv )
{
	if ( argc != 2 )
	{
		fprintf( stderr,
			 "Usage: %s send|receive\n", argv[ 0 ] );
		exit( 1 );
	}

	if ( strcmp( argv[ 1 ], "receive" ) == 0 )
		my_pipe_receive();

	if ( strcmp( argv[ 1 ], "send" ) == 0 )
		my_pipe_send();

	return 0;
} /* main() */

void my_pipe_receive( void )
{
	PIPE *pipe;
	char *pipe_text;

	pipe = pipe_new(
			READ_PORT_NUMBER,
			WRITE_PORT_NUMBER,
			IP_ADDRESS,
			IP_ADDRESS );

	while( ( pipe_text = pipe_receive(
			pipe->read_ip_address,
			pipe->read_port_number ) ) )
	{
		printf( "%s\n", pipe_text );
	}

	pipe_free( pipe );

} /* my_pipe_receive() */

void my_pipe_send( void )
{
	PIPE *pipe;
	FILE *input_pipe;
	char input_buffer[ 1024 ];

	pipe = pipe_new(
			READ_PORT_NUMBER,
			WRITE_PORT_NUMBER,
			IP_ADDRESS,
			IP_ADDRESS );

	input_pipe = popen( "ls -1 *", "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		pipe_send(	pipe->write_ip_address,
				pipe->write_port_number,
				input_buffer );
		/* sleep( 1 ); <-- show stopper */
	}

	pipe_send(	pipe->write_ip_address,
			pipe->write_port_number,
			PIPE_ALL_DONE );

	pipe_free( pipe );
	pclose( input_pipe );

} /* my_pipe_send() */

