/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/event.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "boolean.h"
#include "list.h"
#include "program.h"
#include "event.h"

char *event_primary_where(
			char *program_name,
			char *event_date,
			char *event_time )
{
	char static where[ 256 ];

	sprintf( where,
		 "program_name = '%s' and	"
		 "event_date = '%s' and		"
		 "event_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 program_name_escape( program_name ),
		 event_date,
		 event_time );

	return where;
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

EVENT *event_new(	char *program_name,
			char *event_date,
			char *event_time )
{
	EVENT *event = event_calloc();

	event->program_name = program_name;
	event->event_date = event_date;
	event->event_time = event_time;
	return event;
}

char *event_system_string(
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\" select",
		EVENT_TABLE,
		where );

	return strdup( system_string );
}

EVENT *event_fetch(	char *program_name,
			char *event_date,
			char *event_time,
			boolean fetch_program,
			boolean fetch_venue )
{
	if ( !program_name || !event_date || !event_time )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: empty program_name, event_date or event_time.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
		event_parse(
			string_pipe_fetch(
				event_system_string(
					event_primary_where(
						program_name,
						event_date,
						event_time ) ) ),
			fetch_program,
			fetch_venue );
}

EVENT *event_parse(	char *input,
			boolean fetch_program,
			boolean fetch_venue )
{
	char program_name[ 128 ];
	char event_date[ 128 ];
	char event_time[ 128 ];
	char venue_name[ 128 ];
	char street_address[ 128 ];
	char ticket_price[ 128 ];
	char revenue_account[ 128 ];
	char ticket_sale_count[ 128 ];
	char ticket_sale_total[ 128 ];
	char ticket_refund_total[ 128 ];
	char capacity_available[ 128 ];
	EVENT *event;

	if ( !input || !*input ) return (EVENT *)0;

	event = event_calloc();

	/* See: attribute_list */
	/* ------------------- */
	piece( program_name, SQL_DELIMITER, input, 0 );
	event->program_name = strdup( program_name );
	
	piece( event_date, SQL_DELIMITER, input, 1 );
	event->event_date = strdup( event_date );

	piece( event_time, SQL_DELIMITER, input, 2 );
	event->event_time = strdup( event_time );

	piece( venue_name, SQL_DELIMITER, input, 3 );
	event->venue_name = strdup( venue_name );

	piece( street_address, SQL_DELIMITER, input, 4 );
	event->street_address = strdup( street_address );

	piece( ticket_price, SQL_DELIMITER, input, 5 );
	event->ticket_price = atof( ticket_price );

	piece( revenue_account, SQL_DELIMITER, input, 6 );
	event->revenue_account = strdup( revenue_account );

	piece( ticket_sale_count, SQL_DELIMITER, input, 7 );
	event->ticket_sale_count = atoi( ticket_sale_count );

	piece( ticket_sale_total, SQL_DELIMITER, input, 8 );
	event->ticket_sale_total = atof( ticket_sale_total );

	piece( ticket_refund_total, SQL_DELIMITER, input, 9 );
	event->ticket_refund_total = atof( ticket_refund_total );

	piece( capacity_available, SQL_DELIMITER, input, 10 );
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
				event->venue_name,
				event->street_address );
	}

	return event;
}

LIST *event_system_list(
			char *system_string,
			boolean fetch_program,
			boolean fetch_venue )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *event_list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			event_list,
			event_parse(
				input,
				fetch_program,
				fetch_venue ) );
	}

	pclose( input_pipe );
	return event_list;
}

EVENT *event_long_label_seek(
			char *event_label,
			LIST *event_list )
{
	EVENT *event;

	if ( !list_rewind( event_list ) ) return (EVENT *)0;

	do {
		event = list_get( event_list );

		if ( string_strcmp(
			/* Returns static memory */
			/* --------------------- */
			event_label_long_display(
				event->program_name,
				event->event_date,
				event->event_time ),
			event_label ) == 0 )
		{
			return event;
		}
	} while ( list_next( event_list ) );
	return (EVENT *)0;
}

EVENT *event_short_label_seek(
			char *event_label,
			LIST *event_list )
{
	EVENT *event;

	if ( !list_rewind( event_list ) ) return (EVENT *)0;

	do {
		event = list_get( event_list );

		if ( string_strcmp(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			event_label_short_display(
				event->program_name,
				event->event_date,
				event->event_time ),
			event_label ) == 0 )
		{
			return event;
		}
	} while ( list_next( event_list ) );
	return (EVENT *)0;
}

EVENT *event_list_seek(	char *program_name,
			LIST *event_list )
{
	EVENT *event;

	if ( !list_rewind( event_list ) ) return (EVENT *)0;

	do {
		event =
			list_get(
				event_list );

		if ( string_strcmp(	event->program_name,
					program_name ) == 0 )
		{
			return event;
		}
	} while ( list_next( event_list ) );

	return (EVENT *)0;
}

LIST *event_program_name_list( LIST *event_list )
{
	LIST *name_list;
	EVENT *event;

	if ( !list_rewind( event_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		event = list_get( event_list );

		list_set( name_list, event->program_name );

	} while ( list_next( event_list ) );

	return name_list;
}

LIST *event_label_list( LIST *event_list )
{
	EVENT *event;
	LIST *label_list;

	if ( !list_rewind( event_list ) ) return (LIST *)0;

	label_list = list_new();

	do {
		event = list_get( event_list );

		list_set(
			label_list,
			strdup( 
				/* Returns static memory */
				/* --------------------- */
				event_label_long_display(
					event->program_name,
					event->event_date,
					event->event_time ) ) );

		list_set(
			label_list,
			strdup(
				/* Returns static memory */
				/* --------------------- */
				event_label_short_display(
					event->program_name,
					event->event_date,
					event->event_time ) ) );

	} while ( list_next( event_list ) );

	return label_list;
}

char *event_label_long_display(
			char *program_name,
			char *event_date,
			char *event_time )
{
	static char paypal_display[ 512 ];
	DATE *date;

	date = date_yyyy_mm_dd_new( event_date );

	if ( !date )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: invalid event_date = [%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			event_date );
		return (char *)0;
	}

	date_set_time_hhmm( date, event_time );

	sprintf(paypal_display,
		"%s Tickets: %s, %s %d, %s",
		program_name,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		date_display_day_name( date ),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		date_display_month_name( date ),
		date_day_integer( date ),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		date_display_time_ampm( date ) );

	return paypal_display;
}

char *event_label_short_display(
			char *program_name,
			char *event_date,
			char *event_time )
{
	static char paypal_display[ 256 ];
	DATE *date;

	date = date_yyyy_mm_dd_new( event_date );

	if ( !date )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: invalid event_date = [%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			event_date );
		return (char *)0;
	}

	date_set_time_hhmm( date, event_time );

	sprintf(paypal_display,
		"%s %s %d%s: %d%s",
		program_name,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		date_display_month_name( date ),
		date_day_integer( date ),
		/* --------------------- */
		/* Returns program memory */
		/* ---------------------- */
		date_display_th_st_rd_nd( date ),
		date_hour_time( date ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		date_display_ampm( date ) );

	return paypal_display;
}

void event_fetch_update(
			char *program_name,
			char *event_date,
			char *event_time )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"ticket_sale_count.sh \"%s\" '%s' '%s'",
		program_name,
		event_date,
		event_time );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"ticket_sale_total.sh \"%s\" '%s' '%s'",
		program_name,
		event_date,
		event_time );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"ticket_refund_total.sh \"%s\" '%s' '%s'",
		program_name,
		event_date,
		event_time );

	if ( system( sys_string ) ){}

	sprintf(sys_string,
		"event_capacity_available.sh \"%s\" '%s' '%s'",
		program_name,
		event_date,
		event_time );

	if ( system( sys_string ) ){}
}

void event_list_fetch_update(
			LIST *event_list )
{
	EVENT *event;

	if ( !list_rewind( event_list ) ) return;

	do {
		event = list_get( event_list );

		event_fetch_update(
			event->program_name,
			event->event_date,
			event->event_time );

	} while ( list_next( event_list ) );
}
