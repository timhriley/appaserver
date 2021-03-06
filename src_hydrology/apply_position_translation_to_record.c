/* apply_position_translation_to_record.c */
/* -------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "station_parse.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "piece.h"
#include "environ.h"

#define POSITION_TRANSLATION_PROCESS	"position_translation_table.sh"

#define RECORD_TYPE_OFFSET	0
#define FULL_DATE_OFFSET	1
#define TIME_OFFSET		2
#define STARTING_DATA_OFFSET	3

/* Prototypes */
/* ---------- */
void load_position_translation( STATION_PARSE *station, char *entity );

void output_results( 		STATION_PARSE *station, 
				int record_type_int,
				char *full_date,
				char *time,
				char delimiter,
				char *input_buffer );

void apply_position_translation_to_record( 
				STATION_PARSE *station,
				char delimiter,
				char *argv_0 );

int main( int argc, char **argv )
{
	char *entity;
	STATION_PARSE *station;
	char delimiter;

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s entity station delimiter\n", argv[ 0 ] );
		fprintf(stderr,
	"stdin: record_type,yyyy-mm-dd,hhmm,value_1,value_2,...\n" );
		exit( 1 );
	}

	entity = argv[ 1 ];
	station = station_parse_new( strdup( argv[ 2 ] ) );
	delimiter = *argv[ 3 ];

	load_position_translation( station, entity );

	apply_position_translation_to_record(
		station,
		delimiter,
		argv[ 0 ] );

	return 0;

} /* main() */

void apply_position_translation_to_record( 	STATION_PARSE *station, 
						char delimiter,
						char *argv_0 )
{
	char buffer[ 1024 ];
	char measurement[ 128 ];
	char record_type[ 128 ];
	char full_date[ 128 ];
	char time[ 128 ];
	int record_type_int;
	int i;

	while( get_line( buffer, stdin ) )
	{
		if ( !*buffer || *buffer == '#' )
		{
			fprintf( stderr, "%s reject1: %s\n", argv_0, buffer );
			continue;
		}

		if ( !piece( 	record_type, 
				delimiter, 
				buffer, 
				RECORD_TYPE_OFFSET ) )
		{
			fprintf( stderr, "%s reject2: %s\n", argv_0, buffer );
			continue;
		}

		if ( !piece( 	full_date, 
				delimiter, 
				buffer, 
				FULL_DATE_OFFSET ) )
		{
			fprintf( stderr, "%s reject3: %s\n", argv_0, buffer );
			continue;
		}

		if ( !piece( 	time, 
				delimiter, 
				buffer, 
				TIME_OFFSET ) )
		{
			fprintf( stderr, "%s reject4: %s\n", argv_0, buffer );
			continue;
		}

		record_type_int = atoi( record_type );
		zap_measurements( station );

		for( 	i = STARTING_DATA_OFFSET; 
			piece( measurement, delimiter, buffer, i );
			i++ )
		{
			append_measurement( 	station, 
						record_type_int,
						atof( measurement ) );
		}

		output_results( station, 
				record_type_int,
				full_date,
				time,
				delimiter,
				buffer );
	}

} /* apply_position_translation_to_record() */

void output_results( 		STATION_PARSE *station, 
				int record_type_int,
				char *full_date,
				char *time,
				char delimiter,
				char *input_buffer )
{
	STATION_PARSE_RECORD_TYPE *record_type;
	char *datatype;
	int i;

	record_type = get_record_type( station, record_type_int );

	if ( !record_type )
	{
		fprintf( stderr,
			 "PARSE error record_type=%d: %s\n",
			 record_type_int,
			 input_buffer );
		return;
	}

	for( i = 0; i < station->number_measurements; i++ )
	{
		if ( i == record_type->number_measurements_expected ) break;

		datatype = record_type->datatype_array[ i ];

		/* Ignore unused */
		/* ------------- */
		if ( strcmp( datatype, "unused" ) == 0 ) continue;

		printf( "%s%c%s%c%s%c%s%c%lf\n",
			station->station, 
			delimiter,
			datatype, 
			delimiter,
			full_date,
			delimiter,
			time,
			delimiter,
			station->measurement_array[ i ] );
	}

} /* output_results() */

void load_position_translation( STATION_PARSE *station, char *entity )
{
	FILE *p;
	char buffer[ 1024 ];
	char record_type_string[ 1024 ], rest_of_buffer[ 1024 ];
	char datatype[ 1024 ];
	STATION_PARSE_RECORD_TYPE *record_type;
	int i;

	/* ------------------------------------------------------------ */
	/* Expect: 	10|this,that					*/
	/*		60|this,that,the_other,something_else,and_more	*/
	/* ------------------------------------------------------------ */
	sprintf( buffer, 
		 "%s %s %s", 
		 POSITION_TRANSLATION_PROCESS,
		 entity,
		 station->station );

	p = popen( buffer, "r" );

	while( timlib_getline( buffer, p, 1024 ) )
	{
		if ( !*buffer ) continue;

		piece( record_type_string, '|', buffer, 0 );

		if ( !piece( rest_of_buffer, '|', buffer, 1 ) )
		{
			fprintf( stderr,
			"Cannot get '|' from %s for station = %s\n",
				 POSITION_TRANSLATION_PROCESS,
				 station->station );
			exit( 1 );
		}

		append_record_type( station, atoi( record_type_string ) );

		record_type = get_record_type( 	station, 
						atoi( record_type_string ) );
		if ( !record_type )
		{
			fprintf( stderr,
			"Cannot get record_type = %s from proces = %s\n",
				 record_type_string,
				 POSITION_TRANSLATION_PROCESS );
			exit( 1 );
		}
		for( 	i = 0; 
			piece( datatype, ',', rest_of_buffer, i ); 
			i++ )
		{
			append_record_type_datatype( record_type, datatype );
		}
	}

	pclose( p );

} /* load_position_translation() */

