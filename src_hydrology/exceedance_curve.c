/* --------------------------------------------------- 	*/
/* src_everglades/exceedance_curve.c		       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"

/* Constants */
/* --------- */
#define REAL_TIME_SELECT_LIST "measurement_value,measurement_date,measurement_time,station"
#define AGGREGATION_SELECT_LIST "measurement_date,measurement_time,measurement_value,station"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *where_clause;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	char *aggregate_statistic_string;
	enum aggregate_statistic aggregate_statistic;
	char sys_string[ 1024 ];
	char aggregate_process[ 1024 ];
	char *select_list;
	char *units;
	char *units_converted;
	char units_converted_process[ 1024 ];
	int station_piece;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s ignored \"where_clause\" aggregate_level aggregate_statistic units units_converted\n",
			argv[ 0 ] );
		exit( 1 );
	}

	/* application_name = argv[ 1 ]; */
	where_clause = argv[ 2 ];
	aggregate_level_string = argv[ 3 ];
	aggregate_statistic_string = argv[ 4 ];
	units = argv[ 5 ];
	units_converted = argv[ 6 ];

	aggregate_level =
		aggregate_level_get_aggregate_level(
			aggregate_level_string );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			aggregate_statistic_string,
			aggregate_level );

	if ( aggregate_level == real_time )
	{
		strcpy( aggregate_process, "cat" );
		select_list = REAL_TIME_SELECT_LIST;
		station_piece = 3;
	}
	else
	{
		sprintf( aggregate_process,
		"real_time2aggregate_value.e %s 0 1 2 '%c' '%s' n ''  |"
		"piece_shift_left.e '%c'			      |"
		"piece_shift_left.e '%c'			      |"
		"cat						       ",
		aggregate_statistic_get_string( aggregate_statistic ),
		FOLDER_DATA_DELIMITER,
		aggregate_level_get_string( aggregate_level ),
		FOLDER_DATA_DELIMITER,
		FOLDER_DATA_DELIMITER );

		select_list = AGGREGATION_SELECT_LIST;
		station_piece = 1;
	}

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s 0 '^' %d",
			 units,
			 units_converted,
			 station_piece );
		units = units_converted;
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	sprintf(sys_string,
		"get_folder_data	application=%s			  "
		"			folder=measurement		  "
		"			select=\"%s\"			  "
		"			where=\"%s\"			  "
		"			quick=yes			 |"
		"%s							 |"
		"piece_exceedance.e '%c'				 |"
		"%s							 |"
		"cat							  ",
		application_name,
		select_list,
		where_clause,
		aggregate_process,
		FOLDER_DATA_DELIMITER,
		units_converted_process );

	if ( system( sys_string ) ) {};

	exit( 0 );

} /* main() */

