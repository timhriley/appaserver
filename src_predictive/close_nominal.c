/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/close_nominal.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "close_nominal.h"

CLOSE_NOMINAL *close_nominal_calloc( void )
{
	CLOSE_NOMINAL *close_nominal;

	if ( ! ( close_nominal = calloc( 1, sizeof( CLOSE_NOMINAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return close_nominal;
}

