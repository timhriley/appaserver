/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/random_session.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "session.h"
#include "security.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "environ.h"

int main( int argc, char **argv )
{
	char *application_name;
	char session_number[ SESSION_SIZE + 1 ];
	char *ptr = session_number;
	int i, n;
	int seed;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 2 )
	{
		fprintf( stderr,
			 "Usage: %s seed\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	seed = atoi( argv[ 1 ] );
	seed += time( (time_t *)0 );

	srand48( seed );

	/* Start with at least 1 */
	/* --------------------- */
	n = security_random( SESSION_SIZE - 1 ) - 1 + '1';

	*ptr++ = n;

	for( i = 0; i < SESSION_SIZE - 1; i++ )
	{
		n = security_random( 10 ) - 1 + '0';
		*ptr++ = n;
	}

	*ptr = '\0';

	printf( "%s\n", session_number );

	return 0;
}

