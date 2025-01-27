#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

void local_gethostbyname( char *hostname );

int main( int argc, char **argv )
{
	while ( --argc )
	{
		local_gethostbyname( *++argv );
	}
	return 0;
}

void local_gethostbyname( char *hostname )
{
	struct hostent *h;
	struct in_addr **i;
	int not_first_time;

	if ( ! (h = gethostbyname( hostname ) ) )
	{
		printf( "%s: unknown\n", hostname );
	}
	else
	{
		printf( "%s: ", h->h_name );
		not_first_time = 0;
		i = (struct in_addr **)h->h_addr_list;
		while ( *i )
		{
			if ( not_first_time ) printf( "," );
			printf( "%s",
				inet_ntoa( (*i)->s_addr ) );
			i++;
			not_first_time = 1;
		}
		printf( "\n" );
	}
}

