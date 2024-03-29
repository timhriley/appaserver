/* $APPASERVER_HOME/src_hydrology/measurement.c  */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_library.h"
#include "hydrology_library.h"
#include "measurement_backup.h"
#include "measurement.h"

MEASUREMENT_STRUCTURE *measurement_structure_calloc( void )
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

	return m;
}

MEASUREMENT_STRUCTURE *measurement_structure_new( 
			char *application_name )
{
	MEASUREMENT_STRUCTURE *m = measurement_structure_calloc();

	m->application_name = application_name;
	m->argv_0 = "";

	return m;
}

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

}

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
}

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

}

void measurement_free( MEASUREMENT *m )
{
	free( m->station_name );
	free( m->datatype );
	free( m->measurement_date );
	free( m->measurement_time );
	free( m );
}

boolean measurement_output_pipe( 	
			FILE *output_pipe,
			MEASUREMENT_STRUCTURE *m,
			boolean insert_null_values )
{
	return measurement_pipe_output( 	
			output_pipe,
			m->measurement,
			insert_null_values );
}

boolean measurement_pipe_output(
			FILE *output_pipe,
			MEASUREMENT *measurement,
			boolean insert_null_values )
{
	if ( !measurement )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty measurement.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	if ( measurement->null_value && !insert_null_values )
	{
		fprintf( stderr,
			 "Warning: null value reject %s/%s/%s/%s\n",
			 measurement->station_name,
			 measurement->datatype,
			 measurement->measurement_date,
			 measurement->measurement_time );
		return 0;
	}

	/* ------------------------------------------- */
	/* Send to insert_statement, html_table, etc.  */
	/* ------------------------------------------- */
	if ( output_pipe )
	{
		if ( !measurement->null_value )
		{
			fprintf(output_pipe,
				"%s%c%s%c%s%c%s%c%.4lf\n",
				measurement->station_name,
				MEASUREMENT_INSERT_STATEMENT_DELIMITER,
				measurement->datatype,
				MEASUREMENT_INSERT_STATEMENT_DELIMITER,
				measurement->measurement_date,
				MEASUREMENT_INSERT_STATEMENT_DELIMITER,
				measurement->measurement_time,
				MEASUREMENT_INSERT_STATEMENT_DELIMITER,
				measurement->measurement_value );
		}
		else
		{
			fprintf(output_pipe,
				"%s%c%s%c%s%c%s%cnull\n",
				measurement->station_name,
				MEASUREMENT_INSERT_STATEMENT_DELIMITER,
				measurement->datatype,
				MEASUREMENT_INSERT_STATEMENT_DELIMITER,
				measurement->measurement_date,
				MEASUREMENT_INSERT_STATEMENT_DELIMITER,
				measurement->measurement_time,
				MEASUREMENT_INSERT_STATEMENT_DELIMITER );
		}
	}
	else
	/* -------------- */
	/* Send to stdout */
	/* -------------- */
	{
		if ( !measurement->null_value )
		{
			printf( "Not inserting: %s,%s,%s,%s,%.4lf\n",
				measurement->station_name,
				measurement->datatype,
				measurement->measurement_date,
				measurement->measurement_time,
				measurement->measurement_value );
		}
		else
		{
			printf( "Not inserting: %s,%s,%s,%s,null\n",
				measurement->station_name,
				measurement->datatype,
				measurement->measurement_date,
				measurement->measurement_time );
		}
	}
	return 1;
}

boolean measurement_text_output(
			MEASUREMENT *measurement,
			char delimiter,
			boolean insert_null_values )
{
	if ( !measurement ) return 0;

	if ( measurement->null_value && !insert_null_values )
	{
		fprintf( stderr,
			 "Warning: null value reject %s/%s/%s/%s\n",
			 measurement->station_name,
			 measurement->datatype,
			 measurement->measurement_date,
			 measurement->measurement_time );
		return 0;
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
	return 1;
}

boolean measurement_insert(
			FILE *insert_pipe,
			MEASUREMENT_STRUCTURE *m,
			boolean insert_null_value )
{
	if ( !m || !m->measurement ) return 0;

	if ( !insert_pipe )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: insert_pipe not set.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( m->measurement->null_value && !insert_null_value )
	{
		fprintf( stderr,
			 "%s(): Null value rejection %s/%s/%s/%s\n",
			 __FUNCTION__,
			 m->measurement->station_name,
			 m->measurement->datatype,
			 m->measurement->measurement_date,
			 m->measurement->measurement_time );
		return 0;
	}

	measurement_output_insert_pipe(
			insert_pipe,
			m->measurement->station_name,
			m->measurement->datatype,
			m->measurement->measurement_date,
			m->measurement->measurement_time,
			m->measurement->measurement_value,
			m->measurement->null_value );
	return 1;
}

boolean measurement_non_execute_display(
			FILE *output_pipe,
			MEASUREMENT_STRUCTURE *m,
			boolean insert_null_values )
{
	return measurement_output_pipe(
			output_pipe,
			m,
			insert_null_values );
}

void measurement_output_insert_pipe(
			FILE *insert_pipe,
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
		 	"%s%c%s%c%s%c%s%c%.4lf\n",
		 	station,
			MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		 	datatype,
			MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		 	date,
			MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		 	time,
			MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		 	value );
	}
	else
	{
		fprintf( insert_pipe,
		 	"%s%c%s%c%s%c%s%c\n",
		 	station,
			MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		 	datatype,
			MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		 	date,
			MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		 	time,
			MEASUREMENT_INSERT_STATEMENT_DELIMITER );
	}

	fflush( insert_pipe );
}

FILE *measurement_open_html_table_pipe( void )
{
	char sys_string[ 1024 ];
	char *heading_comma_string;
	char *justify_string;

	heading_comma_string = MEASUREMENT_SELECT_LIST;
	justify_string = "left,left,left,left,right";

	sprintf( sys_string,
		 "queue_top_bottom_lines.e %d |"
		 "html_table.e '' '%s' '%c' '%s'",
		 MEASUREMENT_QUEUE_TOP_BOTTOM_LINES,
		 heading_comma_string,
		 MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		 justify_string );

	return popen( sys_string, "w" );
}

FILE *measurement_open_insert_statement_pipe(
			boolean replace )

{
	char sys_string[ 4096 ];

	sprintf(sys_string,
	"insert_statement table=%s field=%s delimiter='%c' replace=%c   |"
	"cat						  		 ",
		"measurement",
		MEASUREMENT_INSERT_LIST,
		MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		(replace) ? 'y' : 'n' );

	return popen( sys_string, "w" );
}

FILE *measurement_open_insert_pipe(
			boolean replace )

{
	char sys_string[ 4096 ];

	sprintf(sys_string,
	"insert_statement table=%s field=%s delimiter='%c' replace=%c	|"
	"sql 							 	|"
	"cat							  	 ",
		"measurement",
		MEASUREMENT_INSERT_LIST,
		MEASUREMENT_INSERT_STATEMENT_DELIMITER,
		(replace) ? 'y' : 'n' );

	return popen( sys_string, "w" );
}

void measurement_update(
			char *station,
			char *datatype,
			char *date,
			char *time,
			double value )
{
	char sys_string[ 4096 ];

	sprintf(sys_string,
		"echo \"update %s				 "
		"      set measurement_value = %lf		 "
		"      where station = '%s'			 "
		"	 and datatype = '%s'			 "
		"	 and measurement_date = '%s'		 "
		"	 and measurement_time = '%s';\"		|"
		"sql.e 2>&1					|"
		"html_paragraph_wrapper.e		 	 ",
		"measurement",
		value,
		station,
		datatype,
		date,
		time );

	if ( system( sys_string ) ) {};
}

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
}

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

	while ( string_input( input_buffer, input_pipe, 1024 ) )
	{
		if ( ! ( measurement =
				measurement_parse(
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

		list_set( measurement_list, measurement );
	}
	return measurement_list;
}

FILE *measurement_open_input_pipe(
			char *application_name,
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
}

boolean measurement_structure_fetch(	MEASUREMENT_STRUCTURE *m,
					FILE *input_pipe )
{
	char buffer[ 1024 ];

	if ( !timlib_get_line( buffer, input_pipe, 1024 ) ) return 0;

	return measurement_set_delimited_record(
			m,
			buffer,
			',' );

}

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
}

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

}

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
}

void measurement_delete( FILE *delete_pipe, MEASUREMENT *m )
{
	fprintf( delete_pipe,
		 "%s|%s|%s|%s\n",
		 m->station_name,
		 m->datatype,
		 m->measurement_date,
		 m->measurement_time );

}

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

}

/*
#define MEASUREMENT_SELECT_LIST	 	"station,datatype,measurement_date,measurement_time,measurement_value"
*/

MEASUREMENT *measurement_parse(	
			char *buffer,
			char delimiter )
{
	MEASUREMENT *m;
	char piece_buffer[ 128 ];
	static boolean first_time = 0;
	static double prior_value = 0.0;

	if ( string_character_count( delimiter, buffer ) < 4 )
	{
		return (MEASUREMENT *)0;
	}

	m = measurement_calloc();
	m->station_name = strdup( piece( piece_buffer, delimiter, buffer, 0 ) );
	m->datatype = strdup( piece( piece_buffer, delimiter, buffer, 1 ) );

	m->measurement_date =
		strdup( piece(	piece_buffer,
				delimiter,
				buffer,
				2 ) );

	m->measurement_time =
		strdup( piece(	piece_buffer,
				delimiter,
				buffer,
				3 ) );

	m->value_string =
		strdup( piece(	piece_buffer,
				delimiter,
				buffer,
				4 ) );

	if ( !*m->value_string )
		m->null_value = 1;
	else
		m->measurement_value =
			atof( m->value_string );

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
}

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
}

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
}

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
}

MEASUREMENT *measurement_fetch(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time )
{
	if ( !station
	||   !datatype
	||   !measurement_date
	||   !measurement_time )
	{
		return (MEASUREMENT *)0;
	}

	return	measurement_parse(
			pipe2string(
				measurement_sys_string(
		 			/* -------------------------- */
		 			/* Safely returns heap memory */
		 			/* -------------------------- */
		 			measurement_primary_where(
						station,
						datatype,
						measurement_date,
						measurement_time ) ) ),
			SQL_DELIMITER );
}

char *measurement_primary_where(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time  )
{
	char where[ 512 ];

	sprintf(where,
		"station = '%s' and		"
		"datatype = '%s' and		"
		"measurement_date = '%s' and	"
		"measurement_time = '%s'	",
		station,
		datatype,
		measurement_date,
		measurement_time );

	return strdup( where );
}

char *measurement_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 MEASUREMENT_TABLE,
		 where );

	return strdup( sys_string );
}

LIST *measurement_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *measurement_list;

	input_pipe = popen( sys_string, "r" );
	measurement_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			measurement_list,
			measurement_parse(
				input,
				SQL_DELIMITER ) );
	}
	pclose( input_pipe );
	return measurement_list;
}

void measurement_list_table_display(
			LIST *measurement_list )
{
	MEASUREMENT *measurement;
	FILE *output_pipe;

	output_pipe = measurement_open_html_table_pipe();

	if ( !list_rewind( measurement_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		measurement = list_get( measurement_list );

		measurement_pipe_output(
			output_pipe,
			measurement,
			1 /* insert_null_values */ );

	} while ( list_next( measurement_list ) );

	pclose( output_pipe );
}

void measurement_list_output_pipe(
			FILE *output_pipe,
			LIST *measurement_list )
{
	measurement_list_pipe_output(
			output_pipe,
			measurement_list );
}

void measurement_list_pipe_output(
			FILE *output_pipe,
			LIST *measurement_list )
{
	MEASUREMENT *measurement;

	if ( !list_rewind( measurement_list ) )
	{
		return;
	}

	do {
		measurement = list_get( measurement_list );

		measurement_pipe_output(
			output_pipe,
			measurement,
			1 /* insert_null_values */ );

	} while ( list_next( measurement_list ) );
}

MEASUREMENT_BACKUP *measurement_convert_measurement_backup(
			MEASUREMENT *measurement )
{
	MEASUREMENT_BACKUP *measurement_backup;

	if ( !measurement )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty measurement\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	measurement_backup = measurement_backup_calloc();

	measurement_backup->station = measurement->station_name;
	measurement_backup->datatype = measurement->datatype;
	measurement_backup->measurement_date = measurement->measurement_date;
	measurement_backup->measurement_time = measurement->measurement_time;
	measurement_backup->original_value = measurement->measurement_value;

	return measurement_backup;
}

void measurement_list_stdout( LIST *measurement_list )
{
	MEASUREMENT *measurement;

	if ( !list_rewind( measurement_list ) )
	{
		return;
	}

	do {
		measurement = list_get( measurement_list );

		measurement_pipe_output(
			stdout,
			measurement,
			1 /* insert_null_values */ );

	} while ( list_next( measurement_list ) );
}

