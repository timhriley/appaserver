/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/supply.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "piece.h"
#include "sql.h"
#include "security.h"
#include "supply.h"

SUPPLY *supply_calloc( void )
{
	SUPPLY *supply;

	if ( ! ( supply =
			calloc( 1, sizeof( SUPPLY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return supply;
}

SUPPLY *supply_new( char *supply_name )
{
	SUPPLY *supply;

	if ( !supply_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: supply_name is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	supply = supply_calloc();
	supply->supply_name = supply_name;

	return supply;
}

SUPPLY *supply_parse( char *input )
{
	char supply_name[ 128 ];
	char buffer[ 128 ];
	SUPPLY *supply;

	if ( !input || !*input ) return NULL;

	/* See SUPPLY_SELECT */
	/* ----------------- */
	piece( supply_name, SQL_DELIMITER, input, 0 );

	supply =
		supply_new(
			strdup( supply_name ) );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) supply->account_name = strdup( buffer );

	return supply;
}

SUPPLY *supply_fetch( char *supply_name )
{
	return
	supply_parse(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				SUPPLY_SELECT,
				SUPPLY_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				supply_primary_where(
					supply_name ) ) ) );
}

char *supply_primary_where( char *supply_name )
{
	static char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"supply_name = '%s'",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_escape( supply_name ) );

	return where;
}
