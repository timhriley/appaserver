/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/paypal_datasheet.c		*/
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
#include "paypal_datasheet.h"

PAYPAL_DATASHEET *paypal_datasheet_calloc( void )
{
	PAYPAL_DATASHEET *a;

	if ( ! ( a = (PAYPAL_DATASHEET *)
			calloc( 1, sizeof( PAYPAL_DATASHEET ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}
	return a;
}

