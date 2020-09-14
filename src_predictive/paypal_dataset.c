/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/paypal_dataset.c		*/
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
#include "paypal_dataset.h"

PAYPAL_DATASET *paypal_dataset_calloc( void )
{
	PAYPAL_DATASET *a;

	if ( ! ( a = (PAYPAL_DATASET *)
			calloc( 1, sizeof( PAYPAL_DATASET ) ) ) )
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

