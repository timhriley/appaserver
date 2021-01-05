/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_event/event.c			*/
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
#include "ticket_refund.h"
#include "ticket_sale.h"
#include "event.h"

char *event_primary_where(
			char *event_name,
			char *event_date,
			char *event_time )
{
	char static where[ 256 ];

	sprintf( where,
		 "event_name = '%s' and	"
		 "event_date = '%s' and	"
		 "event_time = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 event_name_escape( event_name ),
		 event_date,
		 event_time );

	return where;
}

char *event_name_escape( char *event_name )
{
	static char name[ 256 ];

	return string_escape_quote( name, event_name );
}

EVENT *event_calloc( void )
{
	EVENT *event;

	if ( ! ( event = calloc( 1, sizeof( EVENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return event;
}

EVENT *event_new(	char *event_name,
			char *event_date,
			char *event_time )
{
	EVENT *event = event_calloc();

	event->event_name = event_name;
	event->event_date = event_date;
	event->event_time = event_time;
	return event;
}

EVENT *event_fetch(	char *event_name,
			char *event_date,
			char *event_time,
			boolean fetch_program,
			boolean fetch_venue,
			boolean fetch_sale_list,
			boolean fetch_refund_list )
{
	char sys_string[ 1024 ];

	if ( !event_name || !*event_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty event_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 EVENT_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 event_primary_where(
			event_name,
			event_date,
			event_time ) );

	return
		event_parse(
			pipe2string( sys_string ),
			fetch_program,
			fetch_venue,
			fetch_sale_list,
			fetch_refund_list );
}

EVENT *event_parse(	char *input,
			boolean fetch_program,
			boolean fetch_venue,
			boolean fetch_sale_list,
			boolean fetch_refund_list )
{
	char event_name[ 128 ];
	char event_date[ 128 ];
	char event_time[ 128 ];
	char venue_name[ 128 ];
	char ticket_price[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char program_name[ 128 ];
	char revenue_account[ 128 ];
	char ticket_sale_count[ 128 ];
	char ticket_sale_total[ 128 ];
	char ticket_refund_total[ 128 ];
	char capacity_available[ 128 ];
	EVENT *event;

	if ( !input || !*input ) return (EVENT *)0;

	/* See: attribute_list */
	/* ------------------- */
	piece( event_name, SQL_DELIMITER, input, 0 );
	piece( event_date, SQL_DELIMITER, input, 1 );
	piece( event_time, SQL_DELIMITER, input, 2 );

	event =
		event_new(
			strdup( event_name ),
			strdup( event_date ),
			strdup( event_time ) );

	piece( venue_name, SQL_DELIMITER, input, 3 );
	event->venue_name = strdup( venue_name );

	piece( ticket_price, SQL_DELIMITER, input, 4 );
	event->ticket_price = atof( ticket_price );

	piece( season_name, SQL_DELIMITER, input, 5 );
	event->season_name = strdup( season_name );

	piece( year, SQL_DELIMITER, input, 6 );
	event->year = atoi( year );

	piece( program_name, SQL_DELIMITER, input, 7 );
	event->program_name = strdup( program_name );

	piece( revenue_account, SQL_DELIMITER, input, 8 );
	event->revenue_account = strdup( revenue_account );

	piece( ticket_sale_count, SQL_DELIMITER, input, 9 );
	event->ticket_sale_count = atoi( ticket_sale_count );

	piece( ticket_sale_total, SQL_DELIMITER, input, 10 );
	event->ticket_sale_total = atof( ticket_sale_total );

	piece( ticket_refund_total, SQL_DELIMITER, input, 11 );
	event->ticket_refund_total = atof( ticket_refund_total );

	piece( capacity_available, SQL_DELIMITER, input, 12 );
	event->capacity_available = atoi( capacity_available );

	if ( fetch_program )
	{
		event->program =
			program_fetch(
				event->program_name,
				0 /* not fetch_alias_list */ );
	}

	if ( fetch_venue )
	{
		event->venue =
			venue_fetch(
				event->venue_name );
	}

	if ( fetch_sale_list )
	{
		event->ticket_sale_list =
			ticket_sale_list_fetch(
				event_primary_where(
					event->event_name,
					event->event_date,
					event->event_time ),
				0 /* not fetch_event */ );
	}

	if ( fetch_refund_list )
	{
		event->ticket_refund_list =
			ticket_refund_list_fetch(
				event_primary_where(
					event->event_name,
					event->event_date,
					event->event_time ),
				1 /* fetch_sale */ );
	}

	return event;
}

LIST *event_list( char *where )
{
	return event_system_list(
			event_sys_string( where ),
			1 /* fetch_program */,
			1 /* fetch_venue */,
			1 /* fetch_sale_list */,
			1 /* fetch_refund_list */ );
}

LIST *event_system_list(
			char *sys_string,
			boolean fetch_program,
			boolean fetch_venue,
			boolean fetch_sale_list,
			boolean fetch_refund_list )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *event_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			event_list,
			event_parse(
				input,
				fetch_program,
				fetch_venue,
				fetch_sale_list,
				fetch_refund_list ) );
	}

	pclose( input_pipe );
	return event_list;
}

char *event_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 EVENT_TABLE,
		 where );

	return strdup( sys_string );
}

EVENT *event_name_seek(	char *event_name,
			LIST *event_list )
{
	return event_seek( event_name, event_list );
}

EVENT *event_list_seek(	char *event_name,
			LIST *event_list )
{
	return event_seek( event_name, event_list );
}

EVENT *event_seek(	char *event_name,
			LIST *event_list )
{
	EVENT *event;

	if ( !list_rewind( event_list ) ) return (EVENT *)0;

	do {
		event =
			list_get(
				event_list );

		if ( string_strcmp(	event->event_name,
					event_name ) == 0 )
		{
			return event;
		}
	} while ( list_next( event_list ) );

	return (EVENT *)0;
}

char *event_fetch_program_name(
			char *event_name,
			char *event_date,
			char *event_time )
{
	EVENT *event =
		event_fetch(	event_name,
				event_date,
				event_time,
				0 /* not fetch_program */,
				0 /* not fetch_venue */,
				0 /* not fetch_sale_list */,
				0 /* not fetch_refund_list  */);

	if ( !event )
		return (char *)0;
	else
		return event->program_name;
}

LIST *event_name_list( LIST *event_list )
{
	LIST *name_list;
	EVENT *event;

	if ( !list_rewind( event_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		event = list_get( event_list );

		list_set( name_list, event->event_name );

	} while ( list_next( event_list ) );

	return name_list;
}

FILE *event_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		EVENT_TABLE,
		EVENT_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void event_insert_pipe(
			FILE *insert_pipe,
			char *event_name,
			char *event_date,
			char *event_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		event_name_escape( event_name ),
		event_date,
		event_time );
}

