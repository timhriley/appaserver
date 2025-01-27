/* utility/produce_socket_message.c 			*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main( int argc, char **argv )
{
	struct sockaddr_in addr;
	int fd, ttl = 64;
	char *destination_ip_address;
	int socket_number;
	char *message;

	if ( argc != 4 )
	{
		fprintf( stderr,
	"Usage: %s destination_ip_address socket_number message\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	destination_ip_address = argv[ 1 ];
	socket_number = atoi( argv[ 2 ] );
	message = argv[ 3 ];

	fd = socket( AF_INET, SOCK_DGRAM, 0 );
	setsockopt( fd, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &ttl, 1 );
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr( destination_ip_address );
	addr.sin_port = htons( socket_number );

	sendto(	fd,
		message,
		strlen( message ),
		0,
		(struct sockaddr *) &addr,
		sizeof( addr ) );

	return 0;
} /* main() */

