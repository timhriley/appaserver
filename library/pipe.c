/* library/pipe.c */
/* -------------- */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pipe.h"

PIPE *pipe_new(		int read_port_number,
			int write_port_number,
			char *read_ip_address,
			char *write_ip_address )
{
	PIPE *pipe;

	if ( ! ( pipe = (PIPE *)calloc( 1, sizeof( PIPE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pipe->read_port_number = read_port_number;
	pipe->write_port_number = write_port_number;

	pipe->read_ip_address = read_ip_address;
	pipe->write_ip_address = write_ip_address;

	return pipe;
} /* pipe_new() */

boolean pipe_send(	char *write_ip_address,
			int write_port_number,
			char *pipe_text )
{
	struct sockaddr_in addr;
	int fd, ttl = 64;

	if ( ( fd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
	{
		perror( "cannot open socket" );
		exit( 1 );
	}

	setsockopt( fd, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &ttl, 1 );
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr( write_ip_address );
	addr.sin_port = htons( write_port_number );

	sendto(	fd,
		pipe_text,
		strlen( pipe_text ),
		0,
		(struct sockaddr *) &addr,
		sizeof( addr ) );

	close( fd );
	return 1;
} /* pipe_send() */

char *pipe_receive(	char *read_ip_address,
			int read_port_number )
{
	static struct sockaddr_in addr = {0};
	static int fd, addrlen;
	static struct ip_mreq mreq;
	static char pipe[ MSGSIZE + 1 ];
	static boolean first_time = 1;

	if ( first_time )
	{
		if ( ( fd = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
		{
			perror( "cannot open socket" );
			exit( 1 );
		}

		memset( &addr, 0, sizeof( addr ) );
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl( INADDR_ANY );
		addr.sin_port = htons( read_port_number );
		bind( fd, (struct sockaddr *) &addr, sizeof( addr ) );
		mreq.imr_multiaddr.s_addr = inet_addr( read_ip_address );
		mreq.imr_interface.s_addr = htonl( INADDR_ANY );
		setsockopt(	fd,
				IPPROTO_IP,
				IP_ADD_MEMBERSHIP,
				(char *) &mreq,
				sizeof( mreq ) );
		first_time = 0;
	}

	memset( pipe, 0, sizeof( pipe ) );
	recvfrom(	fd,
			pipe,
			MSGSIZE,
			0,
			(struct sockaddr *) &addr,
			(socklen_t*) &addrlen );

	if ( strcmp( pipe, PIPE_ALL_DONE ) == 0 )
	{
		fprintf( stderr, "%s() returning.\n", __FUNCTION__ );
		close( fd );
		return (char *)0;
	}
	else
	{
		return pipe;
	}

} /* pipe_receive() */

void pipe_free( PIPE *pipe )
{
	free( pipe );
} /* pipe_free() */

