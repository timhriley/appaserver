/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/canvass_street.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "sql.h"
#include "canvass_street.h"

CANVASS_STREET *canvass_street_calloc( void )
{
	CANVASS_STREET *canvass_street;

	if ( ! ( canvass_street = calloc( 1, sizeof ( CANVASS_STREET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return canvass_street;
}

LIST *canvass_street_list(
		char *canvass_name,
		LIST *street_list )
{
	LIST *list = list_new();
	char *where;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];

	if ( !canvass_name
	||   !street_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		canvass_street_where(
			canvass_name );

	system_string =
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			CANVASS_STREET_SELECT,
			CANVASS_STREET_TABLE,
			where );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		list_set(
			list,
			canvass_street_parse(
				street_list,
				input ) );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

char *canvass_street_where( char *canvass_name )
{
	static char where[ 512 ];

	if ( !canvass_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: canvass_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		where,
		sizeof ( where ),
		"canvass = '%s' and "
		"canvass_date is null and "
		"ifnull( bypass_yn,'n' ) <> 'y'",
		canvass_name );

	return where;
}

CANVASS_STREET *canvass_street_parse(
		LIST *street_list,
		char *input )
{
	char street_name[ 128 ];
	char city[ 128 ];
	char state_code[ 128 ];
	char include_yn[ 128 ];
	CANVASS_STREET *canvass_street;

	if ( !input )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: input is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* See CANVASS_STREET_SELECT */
	/* ------------------------- */
	piece( street_name, SQL_DELIMITER, input, 0 );
	piece( city, SQL_DELIMITER, input, 1 );
	piece( state_code, SQL_DELIMITER, input, 2 );
	piece( include_yn, SQL_DELIMITER, input, 3 );

	canvass_street = canvass_street_calloc();

	if ( *include_yn )
		canvass_street->include_boolean =
			( *include_yn == 'y' );

	canvass_street->street =
		street_seek(
			street_list,
			street_name,
			city,
			state_code );

	if ( !canvass_street->street )
	{
		free( canvass_street );
		canvass_street = NULL;
	}

	return canvass_street;
}

void canvass_street_output(
		CANVASS_STREET *canvass_street,
		int distance_yards )
{
	if ( !canvass_street
	||   !canvass_street->street )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: canvass_street is empty or incomplete.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	printf(	"%s^%d^%d^%d^%d\n",
		canvass_street->
			street->
			street_name,
		canvass_street->
			street->
			apartment_count,
		canvass_street->
			street->
			house_count,
		canvass_street->
			street->
			total_count,
		distance_yards );
}
