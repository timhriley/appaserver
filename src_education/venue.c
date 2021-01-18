/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/venue.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "list.h"
#include "venue.h"

char *venue_primary_where(
			char *venue_name,
			char *street_address )
{
	char static where[ 128 ];

	sprintf(where,
		"venue_name = '%s' and	"
		"street_address = '%s'	",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		venue_name_escape( venue_name ),
		street_address );

	return where;
}

char *venue_name_escape( char *venue_name )
{
	static char name[ 256 ];

	return string_escape_quote( name, venue_name );
}

VENUE *venue_calloc( void )
{
	VENUE *venue;

	if ( ! ( venue = calloc( 1, sizeof( VENUE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return venue;
}

VENUE *venue_new(	char *venue_name,
			char *street_address )
{
	VENUE *venue = venue_calloc();

	venue->venue_name = venue_name;
	venue->street_address = street_address;
	return venue;
}

VENUE *venue_fetch(	char *venue_name,
			char *street_address )
{
	char sys_string[ 1024 ];

	if ( !venue_name || !*venue_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty venue_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 VENUE_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 venue_primary_where(
			venue_name,
			street_address ) );

	return
		venue_parse(
			pipe2string( sys_string ) );
}

VENUE *venue_parse( char *input )
{
	char venue_name[ 128 ];
	char street_address[ 128 ];
	char capacity[ 128 ];
	VENUE *venue;

	if ( !input || !*input ) return (VENUE *)0;

	/* See: attribute_list */
	/* ------------------- */
	piece( venue_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );

	venue =
		venue_new(
			strdup( venue_name ),
			strdup( street_address ) );

	piece( capacity, SQL_DELIMITER, input, 2 );
	venue->capacity = atoi( capacity );

	return venue;
}

LIST *venue_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *venue_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			venue_list,
			venue_parse(
				input ) );
	}

	pclose( input_pipe );
	return venue_list;
}

char *venue_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 VENUE_TABLE,
		 where );

	return strdup( sys_string );
}

