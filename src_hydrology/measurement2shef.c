/* -------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/measurement2shef.c	*/
/* -------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/
/* Input from stdin: piped delimited measurement rows	*/
/*			1) station			*/
/*			2) datatype			*/
/*			3) date				*/
/*			4) time				*/
/*			5) measurement value		*/
/* Output to stdout: shef format			*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "shef_datatype_code.h"
#include "environ.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */
#define STATION_PIECE		0
#define DATATYPE_PIECE		1
#define MEASUREMENT_DATE_PIECE	2
#define MEASUREMENT_TIME_PIECE	3
#define MEASUREMENT_VALUE_PIECE	4

/* Prototypes */
/* ---------- */
char *get_data_interval_string( int expected_count_per_day );

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	char *application;
	char station[ 128 ];
	char measurement_date[ 128 ];
	char datatype[ 128 ];
	char measurement_time[ 128 ];
	char measurement_value[ 128 ];
	char year[ 128 ];
	char month[ 128 ];
	char day[ 128 ];
	SHEF_DATATYPE_CODE *shef_datatype_code;
	char shef_code[ 128 ];
	int expected_count_per_day;
	char *data_interval_string;
	char *database_string = {0};
	char *shef_download_code;

	if ( argc != 3 )
	{
		fprintf( stderr, 
"Usage: echo \"station|datatype|date|time|value\" | %s application expected_count_per_day\n", argv[ 0 ] );
		exit( 1 );
	}

	application = argv[ 1 ];
	expected_count_per_day = atoi( argv[ 2 ] );

	if ( timlib_parse_database_string(	&database_string,
						application ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application );

	shef_datatype_code = 
		shef_datatype_code_new( application );

	while( get_line( buffer, stdin ) )
	{

		piece( station, '|', buffer, STATION_PIECE );
		piece( datatype, '|', buffer, DATATYPE_PIECE );
		piece( measurement_date, '|', buffer, MEASUREMENT_DATE_PIECE );
		piece( measurement_time, '|', buffer, MEASUREMENT_TIME_PIECE );
		piece( measurement_value, '|', buffer, MEASUREMENT_VALUE_PIECE);

		piece( year, '-', measurement_date, 0 );
		piece( month, '-', measurement_date, 1 );
		piece( day, '-', measurement_date, 2 );

		if ( strlen( month ) == 1 )
		{
			*(month + 2) = '\0';
			*(month + 1) = *month;
			*month = '0';
		}

		if ( strlen( day ) == 1 )
		{
			*(day + 2) = '\0';
			*(day + 1) = *day;
			*day = '0';
		}

		shef_download_code =
			shef_datatype_code_get_shef_download_code(
			 station,
			 datatype,
			 shef_datatype_code->shef_download_datatype_list );

		if ( shef_download_code )
		{
			strcpy( shef_code, shef_download_code );
		}
		else
		{
			strcpy( shef_code, datatype );
		}
	
		up_string( shef_code );

		data_interval_string = 
			get_data_interval_string( expected_count_per_day );

		if ( *measurement_value )
		{
			printf( ".E/%s/%s/%s%s%s/%s/%s/%.3lf\n",
			 	station,
			 	shef_code,
			 	year,
			 	month,
			 	day,
			 	measurement_time,
			 	data_interval_string,
			 	atof( measurement_value ) );
		}
		else
		{
			printf( ".E/%s/%s/%s%s%s/%s/%s/\n",
			 	station,
			 	shef_code,
			 	year,
			 	month,
			 	day,
			 	measurement_time,
			 	data_interval_string );
		}
	}

	return 0;
} /* main() */

char *get_data_interval_string( int expected_count_per_day )
{
	static char data_interval_string[ 128 ];

	if ( expected_count_per_day <= 24 )
	{
		sprintf(	data_interval_string, 
				"DIH%02d", 
				24 / expected_count_per_day );
	}
	else
	{
		sprintf(	data_interval_string, 
				"DIM%02d", 
				1440 / expected_count_per_day );
	}
	return data_interval_string;
}
