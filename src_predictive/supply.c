/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/supply.c			*/
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
#include "boolean.h"
#include "list.h"
#include "sql.h"
#include "supply.h"

SUPPLY *supply_new( char *supply_name )
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

	supply->supply_name = supply_name;

	return supply;
}

SUPPLY *supply_parse( char *input )
{
	char supply_name[ 128 ];
	char piece_buffer[ 128 ];
	SUPPLY *supply;

	if ( !input ) return (SUPPLY *)0;

	/* See attribute_list supply */
	/* ------------------------------ */
	piece( supply_name, SQL_DELIMITER, input, 0 );

	supply =
		supply_new(
			strdup( supply_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	supply->account_name = strdup( piece_buffer );

	return supply;
}

SUPPLY *supply_fetch( char *supply_name )
{
	return supply_parse(
		pipe2string(
			supply_system_string(
				supply_primary_where(
					supply_name ) ) ) );
}

char *supply_primary_where( char *supply_name )
{
	static char where[ 256 ];

	sprintf(where,
		"supply_name = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		supply_name_escape( supply_name ) );

	return where;
}

char *supply_system_string(
			char *where )
{
	static char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\" supply_name",
		where,
		SUPPLY_TABLE );

	return system_string;
}

char *supply_name_escape(
			char *supply_name )
{
	static char escape[ 128 ];

	return string_escape_quote(
		escape,
		supply_name );
}

