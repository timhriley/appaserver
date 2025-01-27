/* utility/consume_socket_message.c 			*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MSGSIZE	 4096

int main( int argc, char **argv )
{
	struct sockaddr_in addr;
	unsigned int fd, addrlen;
	struct ip_mreq mreq;
	char message[ MSGSIZE + 1 ];
	char *source_ip_address;
	int socket_number;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s source_ip_address socket_number\n",
			argv[ 0 ] );
		exit( 1 );
	}

	source_ip_address = argv[ 1 ];
	socket_number = atoi( argv[ 2 ] );
	fd = socket( AF_INET, SOCK_DGRAM, 0 );
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_port = htons( socket_number );
	bind( fd, (struct sockaddr *) &addr, sizeof( addr ) );
	mreq.imr_multiaddr.s_addr = inet_addr( source_ip_address );
	mreq.imr_interface.s_addr = htonl( INADDR_ANY );
	setsockopt(	fd,
			IPPROTO_IP,
			IP_ADD_MEMBERSHIP,
			(char *) &mreq,
			sizeof( mreq ) );

	for( ;; )
	{
		memset( message, 0, sizeof( message ) );
		recvfrom(	fd,
				message,
				MSGSIZE,
				0,
				(struct sockaddr *) &addr,
				&addrlen );
		puts( message );
	}

	return 0;
} /* main() */

