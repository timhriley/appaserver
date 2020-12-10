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

PAYPAL *paypal_fetch(	char *spreadsheet_filename,
			char *date_label )
{
	PAYPAL *paypal = paypal_calloc();

	if ( ! ( paypal->spreadsheet =
			spreadsheet_fetch(
				spreadsheet_filename,
				date_label ) ) )
	{
		return (PAYPAL *)0;
	}

	return paypal;
}

/* Looks like:
Play Theory in  Improv for High Schoolers (Spring 2020) (Child: Atticus Weaver,Andy Madrigal Villalobos)
*/

char *paypal_entity_delimit( char *item_title_P )
{
	boolean inside_paran = 0;
	char *ptr = item_title_P;

	if ( !ptr ) return (char *)0;

	while( *ptr )
	{
		if ( *ptr == '(' )
		{
			inside_paran = 1;
			ptr++;
			continue;
		}
		if ( *ptr == ')' )
		{
			inside_paran = 0;
			ptr++;
			continue;
		}
		if ( *ptr == PAYPAL_ITEM_DELIMITER && inside_paran )
		{
			*ptr = PAYPAL_ENTITY_DELIMITER;
		}
		ptr++;
	}
	return item_title_P;
}

/* Looks like:
Mary Poppins Junior Tickets: Saturday, March 28, 7:00pm, Mary Poppins Junior Tickets: Monday, March 30, 7:00pm
*/

char *paypal_date_remove( char *item_title_P )
{
	boolean inside_colon = 0;
	int comma_count = 0;
	char destination[ 1024 ];
	char *ptr = destination;

	if ( !item_title_P ) return (char *)0;

	while( *item_title_P )
	{
		if ( *item_title_P == ':' && !inside_colon )
		{
			inside_colon = 1;
			comma_count = 0;
			item_title_P++;
			continue;
		}
		if ( *item_title_P == ',' && inside_colon )
		{
			comma_count++;

			if ( comma_count < 3 )
			{
				item_title_P++;
				continue;
			}
			*ptr++ = *item_title_P++;
			inside_colon = 0;
			continue;
		}
		*ptr++ = *item_title_P++;
	}
	*ptr = '\0';
	return strdup( destination );
}

