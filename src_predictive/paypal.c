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

PAYPAL *paypal_fetch(	char *spreadsheet_name,
			char *spreadsheet_filename )
{
	PAYPAL *paypal = paypal_calloc();

	if ( ! ( paypal->spreadsheet =
			spreadsheet_fetch(
				spreadsheet_name ) ) )
	{
		return (PAYPAL *)0;
	}

	if ( ! ( paypal->spreadsheet->spreadsheet_minimum_date =
			spreadsheet_minimum_date(
				&paypal->spreadsheet->maximum_date,
				spreadsheet_filename ) ) )
	{
		return (PAYPAL *)0;
	}

	paypal->spreadsheet_filename = spreadsheet_filename;

	return paypal;
}

PAYPAL *paypal_new(	char *spreadsheet_name,
			char *spreadsheet_filename )
{
	PAYPAL *paypal = paypal_calloc();

	paypal->spreadsheet =
		spreadsheet_new(
			spreadsheet_name );

	paypal->spreadsheet_filename = spreadsheet_filename;
	return paypal;
}
