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

void canvass_street_fetch_list(
		char *canvass_name,
		boolean include_boolean,
		LIST *street_list,
		LIST *canvass_street_list )
{
	char *where;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	CANVASS_STREET *canvass_street;

	if ( !canvass_name
	||   !street_list
	||   !canvass_street_list )
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
			canvass_name,
			include_boolean );

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
		canvass_street =
			canvass_street_parse(
				street_list,
				input );

		list_set(
			canvass_street_list,
			canvass_street );
	}

	pclose( input_pipe );
}

char *canvass_street_where(
		char *canvass_name,
		boolean include_boolean )
{
	char *include_where;
	static char where[ 512 ];

	include_where =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		canvass_street_include_where(
			include_boolean );

	snprintf(
		where,
		sizeof ( where ),
		"canvass = '%s' and "
		"canvass_date is null and "
		"ifnull( bypass_yn,'n' ) <> 'y' and "
		"%s",
		canvass_name,
		include_where );

	return where;
}

char *canvass_street_include_where( boolean include_boolean )
{
	if ( include_boolean )
		return "include_yn = 'y'";
	else
		return "(include_yn = 'n' or include_yn is null)";
}

CANVASS_STREET *canvass_street_parse(
		LIST *street_list,
		char *input )
{
	char street_name[ 128 ];
	char city[ 128 ];
	char state_code[ 128 ];
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

	piece( street_name, SQL_DELIMITER, input, 0 );
	piece( city, SQL_DELIMITER, input, 1 );
	piece( state_code, SQL_DELIMITER, input, 2 );

	canvass_street = canvass_street_calloc();

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

