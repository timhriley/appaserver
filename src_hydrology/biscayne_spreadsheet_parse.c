/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/biscayne_spreadsheet_parse.c	*/
/* -----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "date_convert.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "julian.h"
#include "hydrology_library.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *datatype_name;
	int column_number;
} DATATYPE;

/* Constants */
/* --------- */
#define TEMPERATURE_HEADING		"Temp"
#define BOTTOM_TEMPERATURE_DATATYPE	"bottom_temperature"
#define SURFACE_TEMPERATURE_DATATYPE	"surface_temperature"
#define SALINITY_HEADING		"Sal"
#define SALINITY_DATATYPE		"salinity_raw"
#define CONDUCTIVITY_HEADING		"SpCond"
#define CONDUCTIVITY_DATATYPE		"conductivity"
#define DEPTH_HEADING			"Depth"
#define DEPTH_DATATYPE			"depth"
#define FEET_PER_METER			3.281

/* Prototypes */
/* ---------- */
boolean get_is_odd_station(		char *station );

LIST *with_input_buffer_get_datatype_list(
					char *input_buffer,
					boolean is_odd_station );

LIST *get_datatype_list(		char *filename,
					boolean is_odd_station );

DATATYPE *new_datatype(			char *datatype_name );

boolean datatype_exists(		char *application_name,
					char *datatype_name );

void biscayne_spreadsheet_parse(	char *filename,
					char *station );

boolean station_filename_synchronized(	char *station,
					char *parameter_filename );

int main( int argc, char **argv )
{
	char *application_name;
	char *filename;
	char *station;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	if ( argc != 3 )
	{
		fprintf( stderr, 
			 "Usage: %s filename station\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	filename = argv[ 1 ];
	station = argv[ 2 ];

	if ( !datatype_exists( application_name, BOTTOM_TEMPERATURE_DATATYPE ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: the datatype bottom temperature has changed. It should be (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			BOTTOM_TEMPERATURE_DATATYPE );
		exit( 0 );
	}

	if ( !datatype_exists( application_name, SURFACE_TEMPERATURE_DATATYPE ))
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: the datatype surface temperature has changed. It should be (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			SURFACE_TEMPERATURE_DATATYPE );
		exit( 0 );
	}

	if ( !datatype_exists( application_name, SALINITY_DATATYPE ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: the datatype salinity has changed. It should be (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			SALINITY_DATATYPE );
		exit( 0 );
	}

	if ( !datatype_exists( application_name, DEPTH_DATATYPE ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: the datatype depth has changed. It should be (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			DEPTH_DATATYPE );
		exit( 0 );
	}

	if ( !datatype_exists( application_name, CONDUCTIVITY_DATATYPE ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: the datatype conductivity has changed. It should be (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			CONDUCTIVITY_DATATYPE );
		exit( 0 );
	}

	if ( !station_filename_synchronized(
					station,
					filename ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: the chosen station conflicts with the filename.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
	}

	biscayne_spreadsheet_parse(
		filename,
		station );

	return 0;

} /* main() */

void biscayne_spreadsheet_parse(
			char *filename,
			char *station )
{
	FILE *input_file;
	char input_buffer[ 1024 ];
	char measurement_date_american[ 32 ];
	char measurement_date_international[ 32 ];
	char measurement_time[ 32 ];
	char measurement_value_string[ 32 ];
	double measurement_value;
	LIST *datatype_list;
	DATATYPE *datatype;
	boolean is_odd_station;
	int line_number = 0;
	JULIAN *measurement_date_time = julian_new_julian( 0.0 );

	is_odd_station = get_is_odd_station( station );

	datatype_list = get_datatype_list(	filename,
						is_odd_station );

	if ( !list_length( datatype_list ) ) return;

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open [%s] for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );
		exit( 1 );
	}

	while( get_line( input_buffer, input_file ) )
	{
		line_number++;

		if ( !*input_buffer ) continue;
		if ( !isdigit( *input_buffer ) ) continue;

		/* Measurement date */
		/* ---------------- */
		column( measurement_date_american, 0, input_buffer );
		date_convert_source_american(
				measurement_date_international,
				international,
				measurement_date_american );

		if ( !date_convert_is_valid_international(
				measurement_date_international ) )
		{
			fprintf( stderr,
				 "Invalid date in line %d: %s\n",
				 line_number,
				 input_buffer );
			continue;
		}

		/* Measurement time */
		/* ---------------- */
		if ( !column( measurement_time, 1, input_buffer ) )
		{
			fprintf( stderr,
				 "Invalid time in line %d: %s\n",
				 line_number,
				 input_buffer );
			continue;
		}

		piece_inverse( measurement_time, ':', 2 );
		trim_character( measurement_time, ':', measurement_time );

		if ( strlen( measurement_time ) != 4 )
		{
			fprintf( stderr,
				 "Invalid time in line %d: %s\n",
				 line_number,
				 input_buffer );
			continue;
		}

		julian_set_yyyy_mm_dd_hhmm(
					measurement_date_time,
					measurement_date_international,
					measurement_time );

		list_rewind( datatype_list );

		do {
			datatype = list_get_pointer( datatype_list );

			column(	measurement_value_string,
				datatype->column_number,
				input_buffer );

			measurement_value = atof( measurement_value_string );

			printf(		"%s^%s^%s^%s^%.3lf\n",
					station,
					datatype->datatype_name,
					julian_display_yyyymmdd(
						measurement_date_time->
							current ),
					julian_display_hhmm(
						measurement_date_time->
							current ),
					measurement_value );

		} while( list_next( datatype_list ) );
	}

	fclose( input_file );

} /* biscayne_spreadsheet_parse() */

boolean station_filename_synchronized(
					char *station,
					char *parameter_filename )
{
	int station_number;
	int file_number;
	char *filename;
	char file_number_string[ 3 ];

	if ( instr( "BISC", station, 1 ) == -1 ) return 1;

	filename = basename_get_filename( parameter_filename );

	if ( strlen( filename ) < 3 ) return 1;

	*file_number_string = *filename;
	*(file_number_string + 1 ) = *(filename + 1);
	*(file_number_string + 2 ) = '\0';
	file_number = atoi( file_number_string );

	if ( !file_number ) return 1;

	station_number = atoi( station + 4 );

	if ( !station_number ) return 1;

	return ( station_number == file_number );

} /* station_filename_synchronized() */

boolean datatype_exists( char *application_name, char *datatype_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 128 ];

	sprintf( where_clause, "datatype = '%s'", datatype_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=datatype			"
		 "			where=\"%s\"			",
		 application_name,
		 where_clause );

	return atoi( pipe2string( sys_string ) );

} /* datatype_exists() */

DATATYPE *new_datatype( char *datatype_name )
{
	DATATYPE *datatype;

	if ( ! ( datatype = (DATATYPE *)calloc( 1, sizeof( DATATYPE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype->datatype_name = datatype_name;
	return datatype;

} /* new_datatype() */

LIST *get_datatype_list(	char *filename,
				boolean is_odd_station )
{
	FILE *input_file;
	char input_buffer[ 1024 ];

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open [%s] for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );
		exit( 1 );
	}

	while( get_line( input_buffer, input_file ) )
	{
		if ( instr( "Date", input_buffer, 1 ) != -1 )
		{
			fclose( input_file );

			return with_input_buffer_get_datatype_list(
						input_buffer,
						is_odd_station );
		}
	}

	fclose( input_file );
	return (LIST *)0;

} /* get_datatype_list() */

LIST *with_input_buffer_get_datatype_list(	char *input_buffer,
						boolean is_odd_station )
{
	LIST *datatype_list = list_new();
	DATATYPE *datatype;
	char datatype_heading[ 128 ];
	int column_number;

	for(	column_number = 0;
		column(	datatype_heading,
			column_number,
			input_buffer );
		column_number++ )
	{
		if ( strcmp( datatype_heading, "Date" ) == 0 )
		{
			continue;
		}
		else
		if ( strcmp( datatype_heading, "Time" ) == 0 )
		{
			continue;
		}
		else
		if ( strcmp( datatype_heading, "Battery" ) == 0 )
		{
			continue;
		}
		else
		if ( strcmp(	datatype_heading,
				TEMPERATURE_HEADING ) == 0 )
		{
			if ( is_odd_station )
			{
				datatype =
					new_datatype(
					SURFACE_TEMPERATURE_DATATYPE );
			}
			else
			{
				datatype =
					new_datatype(
					BOTTOM_TEMPERATURE_DATATYPE );
			}
			datatype->column_number = column_number;
		}
		else
		if ( strcmp(	datatype_heading,
				DEPTH_HEADING ) == 0 )
		{
			datatype = new_datatype( DEPTH_DATATYPE );
			datatype->column_number = column_number;
		}
		else
		if ( strcmp(	datatype_heading,
				SALINITY_HEADING ) == 0 )
		{
			datatype = new_datatype( SALINITY_DATATYPE );
			datatype->column_number = column_number;
		}
		else
		if ( strcmp(	datatype_heading,
				CONDUCTIVITY_HEADING ) == 0 )
		{
			datatype = new_datatype( CONDUCTIVITY_DATATYPE );
			datatype->column_number = column_number;
		}
		else
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot recognize heading of %s.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				datatype_heading );
			exit( 0 );
		}

		list_append_pointer( datatype_list, datatype );
	}

	return datatype_list;

} /* with_input_buffer_get_datatype_list() */

boolean get_is_odd_station( char *station )
{
	char last_character_string[ 2 ];
	int last_character_integer;
	int str_len = strlen( station );

	if ( !str_len ) return 0;

	*last_character_string = *(station + str_len - 1 );
	*(last_character_string + 1) = '\0';

	if ( isalpha( *last_character_string ) )
	{
		return 0;
	}
	else
	{
		last_character_integer = atoi( last_character_string );
		return (last_character_integer % 2);
	}

} /* get_is_odd_station() */

