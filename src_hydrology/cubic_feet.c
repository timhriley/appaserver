/* $APPASERVER_HOME/src_hydrology/cubic_feet.c		*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "hydrology_library.h"
#include "aggregate_statistic.h"
#include "date_convert.h"
#include "cubic_feet.h"

CUBIC_FEET_MEASUREMENT *cubic_feet_measurement_calloc( void )
{
	CUBIC_FEET_MEASUREMENT *cubic_feet_measurement;

	if ( ! ( cubic_feet_measurement =
			calloc( 1, sizeof( CUBIC_FEET_MEASUREMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return cubic_feet_measurement;
}

CUBIC_FEET_MEASUREMENT *cubic_feet_measurement_new(
			char *measurement_date )
{
	CUBIC_FEET_MEASUREMENT *cubic_feet_measurement =
		cubic_feet_measurement_calloc();

	cubic_feet_measurement->measurement_date = measurement_date;

	return cubic_feet_measurement;
}

char *cubic_feet_measurement_system_string(
			char *application_name,
			char *cubic_feet_station_where,
			char *end_date_string,
			char *cubic_feet_units_convert_process )
{
	char system_string[ 1024 ];

	sprintf(system_string,
	"get_folder_data	application=%s		    		 "
	"			folder=measurement	    		 "
	"			select=\"%s\"		    		 "
	"			where=\"%s\"		    		 "
	"			quick=yes		   		|"
	"tr '%c' '%c' 					   		|"
	"sort						   		|"
	"real_time2aggregate_value.e sum %d %d %d '%c' daily n %s	|"
	"piece_inverse.e 3 '%c'						|"
	"%s								|"
	"cat								 ",
			application_name,
			SELECT_LIST,
			cubic_feet_station_where,
			FOLDER_DATA_DELIMITER,
			INPUT_DELIMITER,
	 		DATE_PIECE,
	 		TIME_PIECE,
	 		VALUE_PIECE,
			INPUT_DELIMITER,
			end_date_string,
			INPUT_DELIMITER,
			cubic_feet_units_convert_process );

	return strdup( system_string );
}

CUBIC_FEET_STATION *cubic_feet_station_calloc( void )
{
	CUBIC_FEET_STATION *cubic_feet_station;

	if ( ! ( cubic_feet_station =
			calloc( 1, sizeof( CUBIC_FEET_STATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return cubic_feet_station;
}

LIST *cubic_feet_station_list(
			char *application_name,
			LIST *station_name_list,
			char *datatype_name,
			char *date_where,
			char *end_date_string,
			char *cubic_feet_units_convert_process )
{
	LIST *station_list;

	if ( !date_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date_where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( station_name_list ) ) return (LIST *)0;

	station_list = list_new();

	do {
		list_set(
			station_list,
			cubic_feet_station_new(
				application_name,
				(char *)list_get( station_name_list ),
				datatype_name,
				date_where,
				end_date_string,
				cubic_feet_units_convert_process ) );

	} while ( list_next( station_name_list ) );

	return station_list;
}

CUBIC_FEET_STATION *cubic_feet_station_new(
			char *application_name,
			char *station_name,
			char *datatype_name,
			char *date_where,
			char *end_date_string,
			char *cubic_feet_units_convert_process )
{
	CUBIC_FEET_STATION *cubic_feet_station =
		cubic_feet_station_calloc();

	if ( !application_name
	||   !station_name
	||   !datatype_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !date_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date_where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cubic_feet_station->where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		cubic_feet_station_where(
			station_name,
			datatype_name,
			date_where );

	cubic_feet_station->measurement_list =
		cubic_feet_measurement_list(
			cubic_feet_measurement_system_string(
				application_name,
				cubic_feet_station->where,
				end_date_string,
				cubic_feet_units_convert_process ) );

	return cubic_feet_station;
}

char *cubic_feet_station_where(
			char *station_name,
			char *datatype_name,
			char *date_where )
{
	char where[ 512 ];

	sprintf(where,
 		"station = '%s' and	"
 		"datatype = '%s' and	"
		"%s			",
		station_name,
		datatype_name,
		date_where );

	return strdup( where );
}

CUBIC_FEET *cubic_feet_calloc( void )
{
	CUBIC_FEET *cubic_feet;

	if ( ! ( cubic_feet = calloc( 1, sizeof( CUBIC_FEET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return cubic_feet;
}

CUBIC_FEET *cubic_feet_new(
			char *application_name,
			LIST *station_name_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *units_converted )
{
	CUBIC_FEET *cubic_feet = cubic_feet_calloc();

	if ( !application_name
	||   !list_length( station_name_list )
	||   !datatype_name
	||   !begin_date_string
	||   !end_date_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cubic_feet->start_date_julian =
		cubic_feet_start_date_julian(
			begin_date_string,
			days_to_sum );

	cubic_feet->units_display =
		cubic_feet_units_display(
			application_name,
			datatype_name,
			units_converted );

	cubic_feet->date_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		cubic_feet_date_where(
			cubic_feet->start_date_julian,
			end_date_string );

	cubic_feet->title =
		cubic_feet_title(
			station_name_list,
			datatype_name );

	cubic_feet->subtitle =
		cubic_feet_subtitle(
			begin_date_string,
			end_date_string,
			days_to_sum );

	cubic_feet->subsubtitle =
		/* ------------------ */
		/* Returns date_where */
		/* ------------------ */
		cubic_feet_subsubtitle(
			cubic_feet->date_where );

	cubic_feet->units_convert_process =
		cubic_feet_units_convert_process(
			units_converted );

	cubic_feet->station_list =
		cubic_feet_station_list(
			application_name,
			station_name_list,
			datatype_name,
			cubic_feet->date_where,
			end_date_string,
			cubic_feet->units_convert_process );

	cubic_feet->measurement_hash_table =
		cubic_feet_measurement_hash_table(
			cubic_feet->start_date_julian,
			end_date_string );

	cubic_feet->measurement_hash_table =
		cubic_feet_station_total_hash_table(
			/* ------------------------------ */
			/* Returns measurement_hash_table */
			/* ------------------------------ */
			cubic_feet->measurement_hash_table,
			cubic_feet->station_list );

	cubic_feet->total_measurement_list =
		cubic_feet_total_measurement_list(
			cubic_feet->measurement_hash_table );

	cubic_feet->moving_sum_measurement_list =
		cubic_feet_moving_sum_measurement_list(
			begin_date_string,
			days_to_sum,
			cubic_feet->total_measurement_list,
			cubic_feet->measurement_hash_table );
			
	return cubic_feet;
}

JULIAN *cubic_feet_start_date_julian(
			char *begin_date_string,
			int days_to_sum )
{
	JULIAN *start_date_julian;

	if ( days_to_sum < 1 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid days_to_sum = %d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			days_to_sum );
		exit( 1 );
	}

	start_date_julian = julian_new_yyyy_mm_dd( begin_date_string );
	julian_decrement_days( start_date_julian, days_to_sum - 1 );

	return  start_date_julian;
}

char *cubic_feet_units_display(
			char *application_name,
			char *datatype_name,
			char *units_converted )
{
	return
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype_name,
			UNITS,
			units_converted,
			sum /* aggregate_statistic */ );
}

char *cubic_feet_title(
			LIST *station_name_list,
			char *datatype_name )
{
	char title[ 1024 ];

	sprintf(title,
		"Cubic Feet Moving Sum<br>%s/%s",
		list_display_delimited( station_name_list, ',' ),
		datatype_name );

	return strdup( title );
}

char *cubic_feet_subtitle(
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum )
{
	char subtitle[ 1024 ];

	sprintf(subtitle,
		"%d Days From: %s To: %s",
		days_to_sum,
		begin_date_string,
		end_date_string );

	return strdup( subtitle );
}

char *cubic_feet_units_convert_process(
			char *units_converted )
{
	char units_convert_process[ 1024 ];

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_convert_process,
			 "measurement_convert_units.e %s %s 1 '%c'",
			 UNITS,
			 units_converted,
			 INPUT_DELIMITER );
	}
	else
	{
		strcpy( units_convert_process, "cat" );
	}

	return strdup( units_convert_process );
}

HASH_TABLE *cubic_feet_measurement_hash_table(
			JULIAN *cubic_feet_start_date_julian,
			char *cubic_feet_end_date_string )
{
	HASH_TABLE *measurement_hash_table;
	CUBIC_FEET_MEASUREMENT *cubic_feet_measurement;
	JULIAN *julian;

	if ( !cubic_feet_start_date_julian )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cubic_feet_start_date_julian is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !cubic_feet_end_date_string
	||   !date_convert_is_valid_international(
		cubic_feet_end_date_string ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cubic_feet_end_date_string is invalid.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	julian =
		julian_new(
			cubic_feet_start_date_julian->
				current );

	measurement_hash_table = hash_table_new( hash_table_large );

	while ( 1 )
	{
		cubic_feet_measurement =
			cubic_feet_measurement_new(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				julian_display_yyyy_mm_dd(
					julian->current ) );

		hash_table_set(
			measurement_hash_table,
			cubic_feet_measurement->measurement_date, 
			cubic_feet_measurement );

		if ( strcmp(
			cubic_feet_measurement->measurement_date,
			cubic_feet_end_date_string ) == 0 )
		{
			break;
		}

		julian->current =
			julian_increment_day(
				julian->current );
	}

	return measurement_hash_table;
}

HASH_TABLE *cubic_feet_station_total_hash_table(
			HASH_TABLE *measurement_hash_table,
			LIST *cubic_feet_station_list )
{
	CUBIC_FEET_STATION *cubic_feet_station;
	CUBIC_FEET_MEASUREMENT *cubic_feet_measurement;
	CUBIC_FEET_MEASUREMENT *cubic_feet_station_measurement;

	if ( !measurement_hash_table )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: measurement_hash_table is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( cubic_feet_station_list ) )
		return measurement_hash_table;

	do {
		cubic_feet_station = list_get( cubic_feet_station_list );

		if ( !list_rewind( cubic_feet_station->measurement_list ) )
			continue;

		do {
			cubic_feet_station_measurement =
				list_get(
					cubic_feet_station->
						measurement_list );

			if ( ! ( cubic_feet_measurement =
					hash_table_get(
						measurement_hash_table,
						cubic_feet_station_measurement->
							measurement_date ) ) )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: hash_table_get(%s) returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					cubic_feet_station_measurement->
						measurement_date );
				exit( 1 );
			}

			cubic_feet_measurement->
				measurement_station_total +=
					cubic_feet_station_measurement->
						measurement_daily_average;

		} while ( list_next(
				cubic_feet_station->
					measurement_list ) );

	} while ( list_next( cubic_feet_station_list ) );

	return measurement_hash_table;
}

LIST *cubic_feet_total_measurement_list(
			HASH_TABLE *measurement_hash_table )
{
	LIST *key_list;
	LIST *measurement_list;

	key_list =
		hash_table_ordered_key_list(
			measurement_hash_table );

	if ( !list_rewind( key_list ) ) return (LIST *)0;

	measurement_list = list_new();

	do {
		list_set(
			measurement_list,
			hash_table_get(
				measurement_hash_table,
				list_get( key_list ) ) );

	} while ( list_next( key_list ) );

	return measurement_list;
}

LIST *cubic_feet_measurement_list( char *system_string )
{
	char input_buffer[ 128 ];
	char measurement_date[ 64 ];
	char daily_average[ 64 ];
	FILE *input_pipe = popen( system_string, "r" );
	LIST *measurement_list = list_new();
	CUBIC_FEET_MEASUREMENT *cubic_feet_measurement;

	while( timlib_get_line( input_buffer, input_pipe, 64 ) )
	{
		piece( measurement_date, INPUT_DELIMITER, input_buffer, 0 );
		piece( daily_average, INPUT_DELIMITER, input_buffer, 1 );

		list_set(
			measurement_list,
			( cubic_feet_measurement =
				cubic_feet_measurement_new(
					strdup( measurement_date ) ) ) );

		cubic_feet_measurement->measurement_daily_average =
			atof( daily_average );
	} 

	pclose( input_pipe );

	return measurement_list;
}

LIST *cubic_feet_moving_sum_measurement_list(
			char *begin_date_string,
			int days_to_sum,
			LIST *cubic_feet_total_measurement_list,
			HASH_TABLE *measurement_hash_table )
{
	char *temp_filename;

	temp_filename =
		cubic_feet_moving_sum_produce(
			begin_date_string,
			days_to_sum,
			cubic_feet_total_measurement_list );

	return 
	cubic_feet_moving_sum_consume(
		temp_filename,
		measurement_hash_table,
		cubic_feet_total_measurement_list );
}

char *cubic_feet_moving_sum_produce(
			char *begin_date_string,
			int days_to_sum,
			LIST *cubic_feet_total_measurement_list )
{
	char temp_filename[ 128 ];
	char system_string[ 1024 ];
	FILE *output_pipe;
	CUBIC_FEET_MEASUREMENT *cubic_feet_measurement;

	sprintf(temp_filename,
		"/tmp/cubic_feet_%d.dat",
		getpid() );

	sprintf(system_string,
		"moving_average.e %d '%c' average %s > %s",
		days_to_sum,
		INPUT_DELIMITER,
		begin_date_string,
		temp_filename );

	output_pipe = popen( system_string, "w" );

	if ( list_rewind( cubic_feet_total_measurement_list ) )
	{
		do {
			cubic_feet_measurement =
				list_get(
					cubic_feet_total_measurement_list );

			fprintf(output_pipe,
				"%s%c%.4lf\n",
				cubic_feet_measurement->measurement_date,
				INPUT_DELIMITER,
				cubic_feet_measurement->
					measurement_station_total );

		} while ( list_next( cubic_feet_total_measurement_list ) );
	}

	pclose( output_pipe );

	return strdup( temp_filename );
}

LIST *cubic_feet_moving_sum_consume(
			char *temp_filename,
			HASH_TABLE *measurement_hash_table,
			LIST *cubic_feet_total_measurement_list )
{
	FILE *input_file;
	char input_buffer[ 128 ];
	char measurement_date[ 64 ];
	char moving_sum[ 64 ];
	CUBIC_FEET_MEASUREMENT *cubic_feet_measurement;

	if ( ! ( input_file = fopen( temp_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			temp_filename );
		exit( 1 );
	}

	while( timlib_get_line( input_buffer, input_file, 128 ) )
	{
		piece( measurement_date, INPUT_DELIMITER, input_buffer, 0 );
		piece( moving_sum, INPUT_DELIMITER, input_buffer, 1 );

		if ( ! ( cubic_feet_measurement =
				hash_table_get(
					measurement_hash_table,
					measurement_date ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: hash_table_get(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				measurement_date );

			fclose( input_file );
			exit( 1 );
		}

		cubic_feet_measurement->moving_sum = atof( moving_sum );
	}

	fclose( input_file );

	return cubic_feet_total_measurement_list;
}

char *cubic_feet_date_where(
			JULIAN *start_date_julian,
			char *end_date_string )
{
	static char date_where[ 128 ];

	sprintf(date_where,
		"measurement_date between '%s' and '%s'",
		julian_display_yyyy_mm_dd(
			start_date_julian->current ),
		end_date_string );

	return date_where;
}

char *cubic_feet_subsubtitle( char *date_where )
{
	return date_where;
}

