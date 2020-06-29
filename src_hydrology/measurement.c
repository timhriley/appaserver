/* $APPASERVER_HOME/src_hydrology/measurement.c  */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "hydrology_library.h"
#include "measurement.h"

MEASUREMENT_STRUCTURE *measurement_structure_new( 
			char *application_name )
{
	MEASUREMENT_STRUCTURE *m;

	m = (MEASUREMENT_STRUCTURE *)
		calloc( 1, sizeof( MEASUREMENT_STRUCTURE ) );

	if ( !m )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->application_name = application_name;
	m->argv_0 = "";

	return m;

} /* measurement_structure_new() */

boolean measurement_set_delimited_record( 
					MEASUREMENT_STRUCTURE *m,
					char *delimited_record,
					char delimiter )
{
	char station[ 128 ];
	char datatype[ 128 ];
	char date[ 128 ];
	char time[ 128 ];
	char value_string[ 128 ];
	int time_int;

	if ( character_count( delimiter, delimited_record ) < VALUE_PIECE )
	{
		if ( timlib_strncmp( delimited_record, "skipped" ) == 0 )
		{
			/* This function does strdup() for all the memory. */
			/* ----------------------------------------------- */
			m->measurement =
				measurement_strdup_new(
					delimited_record /* station_name */,
					"" /* datatype */,
					"" /* date */,
					"" /* time */,
					"" /* value_string */ );
			return 1;
		}
		else
		{
			return 0;
		}
	}

	piece( station, delimiter, delimited_record, STATION_PIECE );
	piece( datatype, delimiter, delimited_record, DATATYPE_PIECE );
	piece( date, delimiter, delimited_record, DATE_PIECE );
	piece( time, delimiter, delimited_record, TIME_PIECE );

	time_int = atoi( time );

	if ( ( strcmp( time, "0000" ) != 0 )
	&&   ( strcmp( time, "null" ) != 0 )
	&&   ( time_int <= 0 || time_int > 2359 ) )
	{
		return 0;
	}

	piece( value_string, delimiter, delimited_record, VALUE_PIECE );

	if ( m->measurement ) 
	{
		measurement_free( m->measurement );
	}

	/* This function does strdup() for all the memory. */
	/* ----------------------------------------------- */
	m->measurement =
		measurement_strdup_new(
			station,
			datatype,
			date,
			time,
			value_string );

	return 1;

} /* measurement_set_delimited_record() */

MEASUREMENT *measurement_calloc( void )
{
	MEASUREMENT *m;

	m = (MEASUREMENT *)calloc( 1, sizeof( MEASUREMENT ) );

	if ( !m )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return m;
} /* measurement_calloc() */

/* This function does strdup() for all the memory. */
/* ----------------------------------------------- */
MEASUREMENT *measurement_strdup_new(	char *station_name,
					char *datatype,
					char *date,
					char *time,
					char *value_string )
{
	MEASUREMENT *m;

	m = measurement_calloc();

	m->station_name = strdup( station_name );
	m->datatype = strdup( datatype );
	m->measurement_date = strdup( date );	
	m->measurement_time = strdup( time );
	m->value_string = strdup( value_string );

	m->measurement_value = 
		measurement_get_value(
			&m->null_value, value_string );
	return m;

} /* measurement_strdup_new() */

void measurement_free( MEASUREMENT *m )
{
	free( m->station_name );
	free( m->datatype );
	free( m->measurement_date );
	free( m->measurement_time );
	free( m );
}

void measurement_html_display( 		MEASUREMENT_STRUCTURE *m,
					FILE *html_table_pipe )
{
	measurement_non_execute_display(
				m,
				html_table_pipe );

} /* measurement_html_display() */

void measurement_text_output(		MEASUREMENT *measurement,
					char delimiter )
{
	if ( !measurement )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no record set.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !measurement->null_value )
	{
		printf( "%s%c%s%c%s%c%s%c%.4lf\n",
			measurement->station_name,
			delimiter,
			measurement->datatype,
			delimiter,
			measurement->measurement_date,
			delimiter,
			measurement->measurement_time,
			delimiter,
			measurement->measurement_value );
	}
	else
	{
		printf("%s%c%s%c%s%c%s%c\n",
			measurement->station_name,
			delimiter,
			measurement->datatype,
			delimiter,
			measurement->measurement_date,
			delimiter,
			measurement->measurement_time,
			delimiter );
	}

} /* measurement_text_output() */

void measurement_insert( MEASUREMENT_STRUCTURE *m )
{
	if ( !m || !m->measurement )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no record set.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !m->insert_pipe )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: insert_pipe not set.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	measurement_output_insert_pipe(
			m->insert_pipe,
			m->measurement->station_name,
			m->measurement->datatype,
			m->measurement->measurement_date,
			m->measurement->measurement_time,
			m->measurement->measurement_value,
			m->measurement->null_value );

} /* measurement_insert() */

void measurement_non_execute_display(
				MEASUREMENT_STRUCTURE *m,
				FILE *html_table_pipe )
{
	if ( !m->measurement )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no record set.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( html_table_pipe )
	{
		if ( !m->measurement->null_value )
		{
			fprintf(html_table_pipe,
				"%s,%s,%s,%s,%.4lf\n",
				m->measurement->station_name,
				m->measurement->datatype,
				m->measurement->measurement_date,
				m->measurement->measurement_time,
				m->measurement->measurement_value );
		}
		else
		{
			fprintf(html_table_pipe,
				"%s,%s,%s,%s,null\n",
				m->measurement->station_name,
				m->measurement->datatype,
				m->measurement->measurement_date,
				m->measurement->measurement_time );
		}
	}
	else
	{
		if ( !m->measurement->null_value )
		{
			printf( "Not inserting: %s,%s,%s,%s,%.4lf\n",
				m->measurement->station_name,
				m->measurement->datatype,
				m->measurement->measurement_date,
				m->measurement->measurement_time,
				m->measurement->measurement_value );
		}
		else
		{
			printf( "Not inserting: %s,%s,%s,%s,null\n",
				m->measurement->station_name,
				m->measurement->datatype,
				m->measurement->measurement_date,
				m->measurement->measurement_time );
		}
	}

} /* measurement_non_execute_display() */

void measurement_output_insert_pipe(	FILE *insert_pipe,
					char *station,
					char *datatype,
					char *date,
					char *time,
					double value,
					boolean null_value )
{
	if ( !null_value )
	{
		fprintf( insert_pipe,
		 	"%s|%s|%s|%s|%.4lf\n",
		 	station,
		 	datatype,
		 	date,
		 	time,
		 	value );
	}
	else
	{
		fprintf( insert_pipe,
		 	"%s|%s|%s|%s|\n",
		 	station,
		 	datatype,
		 	date,
		 	time );
	}
	fflush( insert_pipe );
} /* measurement_output_insert_pipe() */

FILE *measurement_open_html_table_pipe(	void )
{
	char sys_string[ 1024 ];
	char *heading_comma_string;
	char *justify_string;

	heading_comma_string = "station,datatype,date,time,value";
	justify_string = "left,left,left,left,right";

	sprintf( sys_string,
		 "queue_top_bottom_lines.e %d |"
		 "html_table.e '' '%s' ',' '%s'",
		 MEASUREMENT_QUEUE_TOP_BOTTOM_LINES,
		 heading_comma_string,
		 justify_string );

	return popen( sys_string, "w" );

} /* measurement_open_html_table_pipe() */

FILE *measurement_open_insert_pipe(	char *application_name,
					boolean replace )

{
	char sys_string[ 4096 ];
	char *table_name;

	table_name = get_table_name( application_name, "measurement" );

	sprintf(sys_string,
		"insert_statement.e table=%s field=%s del='|' replace=%c |"
		"sql.e						 	 |"
		"cat							  ",
		table_name,
		MEASUREMENT_INSERT_LIST,
		(replace) ? 'y' : 'n' );

	return popen( sys_string, "w" );

} /* measurement_open_insert_pipe() */

void measurement_update(	char *application_name,
				char *station,
				char *datatype,
				char *date,
				char *time,
				double value )
{
	char sys_string[ 4096 ];
	char *table_name;

	table_name = get_table_name( application_name, "measurement" );

	sprintf(sys_string,
		"echo \"update %s				 "
		"      set measurement_value = %lf		 "
		"      where station = '%s'			 "
		"	 and datatype = '%s'			 "
		"	 and measurement_date = '%s'		 "
		"	 and measurement_time = '%s';\"		|"
		"sql.e 2>&1					|"
		"html_paragraph_wrapper.e		 	 ",
		table_name,
		value,
		station,
		datatype,
		date,
		time );

	if ( system( sys_string ) ) {};

} /* measurement_update() */

double measurement_get_value(	boolean *null_value,
				char *value_string )
{
	if ( !*value_string
	||   strcmp( value_string, "null" ) == 0 )
	{
		*null_value = 1;
		return NULL_REPLACEMENT;
	}
	else
	{
		*null_value = 0;
		return atof( value_string );
	}
} /* measurement_get_value() */

LIST *measurement_fetch_list(	FILE *input_pipe,
				char delimiter )
{
	LIST *measurement_list;
	char input_buffer[ 1024 ];
	MEASUREMENT *measurement;

	if ( !input_pipe )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: input_pipe is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (LIST *)0;
	}

	measurement_list = list_new();

	while ( timlib_get_line( input_buffer, input_pipe, 1024 ) )
	{
		if ( ! ( measurement = measurement_parse(
						input_buffer,
						delimiter ) ) )
		{
			fprintf( stderr,
				 "Error in %s/%s()/%d: cannot parse [%s].\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );

			pclose( input_pipe );
			exit( 1 );
		}

		list_append_pointer( measurement_list, measurement );
	}

	return measurement_list;

} /* measurement_fetch_list(); */

FILE *measurement_open_input_pipe(	char *application_name,
					char *where_clause,
					char delimiter )
{
	char sys_string[ 65536 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			folder=measurement	 "
		 "			select=%s		 "
		 "			where=\"%s\"		 "
		 "			order=select		|"
		 "tr '%c' '%c'					 ",
		 application_name,
		 MEASUREMENT_SELECT_LIST,
		 where_clause,
		 FOLDER_DATA_DELIMITER,
		 delimiter );

	return popen( sys_string, "r" );

} /* measurement_open_input_pipe() */

boolean measurement_structure_fetch(	MEASUREMENT_STRUCTURE *m,
					FILE *input_pipe )
{
	char buffer[ 1024 ];

	if ( !timlib_get_line( buffer, input_pipe, 1024 ) ) return 0;

	return measurement_set_delimited_record(
			m,
			buffer,
			',' );

} /* measurement_structure_fetch() */

void measurement_set_argv_0( MEASUREMENT_STRUCTURE *m, char *argv_0 )
{
	m->argv_0 = argv_0;
}

FILE *measurement_open_delete_pipe( char *application_name )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = get_table_name( application_name, "measurement" );

	sprintf( sys_string,
"delete_statement.e %s station,datatype,measurement_date,measurement_time | sql.e",
		 table_name );
	return popen( sys_string, "w" );
} /* measurement_open_delete_pipe() */

/* Returns static memory */
/* --------------------- */
char *measurement_display_delimiter(
			MEASUREMENT *m,
			char delimiter )
{
	static char buffer[ 1024 ];

	if ( !m->null_value )
	{
		sprintf( buffer,
		 	"%s%c%s%c%s%c%s%c%.3lf",
		 	m->station_name,
			delimiter,
		 	m->datatype,
			delimiter,
		 	m->measurement_date,
			delimiter,
		 	m->measurement_time,
			delimiter,
		 	m->measurement_value );
	}
	else
	{
		sprintf( buffer,
		 	"%s%c%s%c%s%c%s%c",
		 	m->station_name,
			delimiter,
		 	m->datatype,
			delimiter,
		 	m->measurement_date,
			delimiter,
		 	m->measurement_time,
			delimiter );
	}

	return buffer;

} /* measurement_display_delimiter() */

/* Returns static memory */
/* --------------------- */
char *measurement_display( MEASUREMENT *m )
{
	static char buffer[ 1024 ];

	if ( !m->null_value )
	{
		sprintf(buffer,
		 	"%s,%s,%s,%s,%.3lf",
		 	m->station_name,
		 	m->datatype,
		 	m->measurement_date,
		 	m->measurement_time,
		 	m->measurement_value );
	}
	else
	{
		sprintf(buffer,
		 	"%s,%s,%s,%s,",
		 	m->station_name,
		 	m->datatype,
		 	m->measurement_date,
		 	m->measurement_time );
	}

	return buffer;

} /* measurement_display() */

void measurement_delete( FILE *delete_pipe, MEASUREMENT *m )
{
	fprintf( delete_pipe,
		 "%s|%s|%s|%s\n",
		 m->station_name,
		 m->datatype,
		 m->measurement_date,
		 m->measurement_time );

} /* measurement_delete() */

MEASUREMENT_UPDATE *measurement_update_new( void )
{
	MEASUREMENT_UPDATE *m;

	m = (MEASUREMENT_UPDATE *)
		calloc( 1,
			sizeof( MEASUREMENT_UPDATE ) );

	if ( !m )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return m;

} /* measurement_update_new() */

/*
#define MEASUREMENT_SELECT_LIST	 	"station,datatype,measurement_date,measurement_time,measurement_value"
*/

MEASUREMENT *measurement_parse(		char *buffer,
					char delimiter )
{
	MEASUREMENT *m;
	char piece_buffer[ 128 ];
	static boolean first_time = 0;
	static double prior_value = 0.0;

	if ( timlib_character_count( delimiter, buffer ) != 4 )
	{
		return (MEASUREMENT *)0;
	}

	m = measurement_calloc();
	m->station_name = strdup( piece( piece_buffer, delimiter, buffer, 0 ) );
	m->datatype = strdup( piece( piece_buffer, delimiter, buffer, 1 ) );

	m->measurement_date =
		strdup( piece( piece_buffer, delimiter, buffer, 2 ) );

	m->measurement_time =
		strdup( piece( piece_buffer, delimiter, buffer, 3 ) );

	m->value_string =
		strdup( piece( piece_buffer, delimiter, buffer, 4 ) );

	if ( !*m->value_string )
		m->null_value = 1;
	else
		m->measurement_value = atof( m->value_string );

	if ( first_time )
	{
		first_time = 0;
	}
	else
	{
		m->delta_value = m->measurement_value - prior_value;
	}

	prior_value = m->measurement_value;

	return m;

} /* measurement_parse() */

void measurement_change_text_output(	LIST *measurement_list,
					char delimiter )
{
	MEASUREMENT *measurement;

	if ( !list_rewind( measurement_list ) ) return;

	do {
		measurement = list_get( measurement_list );

		printf( "%s%c%s%c%s%c%s%c%.3lf%c",
			 measurement->station_name,
			 delimiter,
			 measurement->datatype,
			 delimiter,
			 measurement->measurement_date,
			 delimiter,
			 measurement->measurement_time,
			 delimiter,
			 measurement->measurement_value,
			 delimiter );

		if ( measurement->measurement_update )
		{
			printf( "%.3lf\n",
				 measurement->
					measurement_update->
					measurement_update );
		}
		else
		{
			printf( "%.3lf\n",
				 measurement->measurement_value );
		}

	} while ( list_next( measurement_list ) );

} /* measurement_change_text_output() */

/*
#define MEASUREMENT_INSERT_ORDER	 "station,datatype,measurement_date,measurement_time,measurement_value"
*/
MEASUREMENT *measurement_variable_parse(
					char *buffer,
					char delimiter,
					LIST *order_integer_list )
{
	MEASUREMENT *m;
	char piece_buffer[ 128 ];
	int *piece_offset;
	static boolean first_time = 0;
	static double prior_value = 0.0;

	if ( timlib_character_count( delimiter, buffer ) != 4 )
	{
		return (MEASUREMENT *)0;
	}

	if ( list_length( order_integer_list ) != 5 )
	{
		return (MEASUREMENT *)0;
	}

	list_rewind( order_integer_list );
	m = measurement_calloc();

	/* Parse station */
	/* ------------- */
	piece_offset = (int *)list_get_pointer( order_integer_list );

	m->station_name = strdup( piece(
					piece_buffer,
					delimiter,
					buffer,
					*piece_offset ) );

	/* Parse datatype */
	/* -------------- */
	list_next( order_integer_list );
	piece_offset = (int *)list_get_pointer( order_integer_list );

	m->datatype = strdup( piece(
					piece_buffer,
					delimiter,
					buffer,
					*piece_offset ) );

	/* Parse measurement_date */
	/* ---------------------- */
	list_next( order_integer_list );
	piece_offset = (int *)list_get_pointer( order_integer_list );

	m->measurement_date = strdup( piece(
					piece_buffer,
					delimiter,
					buffer,
					*piece_offset ) );

	/* Parse measurement_time */
	/* ---------------------- */
	list_next( order_integer_list );
	piece_offset = (int *)list_get_pointer( order_integer_list );

	m->measurement_time = strdup( piece(
					piece_buffer,
					delimiter,
					buffer,
					*piece_offset ) );

	/* Parse measurement_value */
	/* ----------------------- */
	list_next( order_integer_list );
	piece_offset = (int *)list_get_pointer( order_integer_list );

	m->value_string = strdup( piece(
					piece_buffer,
					delimiter,
					buffer,
					*piece_offset ) );

	if ( !*m->value_string )
		m->null_value = 1;
	else
		m->measurement_value = atof( m->value_string );

	if ( first_time )
	{
		first_time = 0;
	}
	else
	{
		m->delta_value = m->measurement_value - prior_value;
	}

	if ( first_time )
	{
		first_time = 0;
	}
	else
	{
		m->delta_value = m->measurement_value - prior_value;
	}

	prior_value = m->measurement_value;

	return m;

} /* measurement_variable_parse() */

JULIAN *measurement_adjust_time_to_sequence(
				JULIAN *measurement_date_time,
				char *sequence_list_string )
{
	int time_integer;
	int sequence_integer;
	int minute_earlier;
	int minute_later;
	static LIST *sequence_list = {0};
	char *hhmm_string;
	char last_two_digits[ 3 ];

	if ( !sequence_list )
	{
		sequence_list = 
			list_delimiter_string_to_list(
				sequence_list_string,
				',' );

		if ( !list_length( sequence_list ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: empty sequence list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	hhmm_string = julian_get_hhmm_string( measurement_date_time->current );

	if ( strlen( hhmm_string ) != 4 )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: expecting time of 4 digits but got (%s) instead. Continuing...\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 hhmm_string );
		return measurement_date_time;
	}

	*last_two_digits = *(hhmm_string + 2);
	*(last_two_digits + 1) = *(hhmm_string + 3);
	*(last_two_digits + 2) = '\0';

	time_integer = atoi( last_two_digits );

	list_rewind( sequence_list );
	do {
		sequence_integer =
			atoi( list_get_pointer( sequence_list ) );

		minute_earlier = sequence_integer - 1;
		if ( minute_earlier == -1 ) minute_earlier = 59;

		minute_later = sequence_integer + 1;

		if ( time_integer == minute_earlier )
		{
			measurement_date_time->current =
				julian_increment_minutes(
					measurement_date_time->current,
					1.0 );
			return measurement_date_time;
		}
		else
		if ( time_integer == minute_later )
		{
			measurement_date_time->current =
				julian_increment_minutes(
					measurement_date_time->current,
					-1.0 );
			return measurement_date_time;
		}
	} while( list_next( sequence_list ) );

	return measurement_date_time;

} /* measurement_adjust_time_to_sequence() */

