/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/paypal.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "paypal.h"

PAYPAL *paypal_calloc( void )
{
	PAYPAL *p;

	if ( ! ( p = calloc( 1, sizeof( PAYPAL ) ) ) )
	{
		fprintf(stderr,
			"%s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return p;
}

