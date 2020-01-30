/* $APPASERVER_HOME/src_hydrology/measurement_frequency_reject.c	*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* ------------------------------------------------------------------- -*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "measurement.h"
#include "station_datatype.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "datatype.h"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char delimited_record[ 1024 ];
	char delimiter;
	MEASUREMENT_STRUCTURE *m;
	LIST *station_datatype_list;
	STATION_DATATYPE *station_datatype;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s delimiter\n", 
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	m = measurement_structure_new( application_name );
	station_datatype_list = list_new();

	while( timlib_get_line( delimited_record, stdin, 1024 ) )
	{
		if ( !measurement_set_delimited_record(
			m, 
			delimited_record,
			delimiter ) )
		{
			fprintf( stderr,
				 "Invalid data: %s\n",
				 delimited_record );
			continue;
		}

		station_datatype =
			station_datatype_get_or_set(
				station_datatype_list,
				m->measurement->station_name,
				m->measurement->datatype );

		if ( !station_datatype->data_collection_frequency_list )
		{
			station_datatype->data_collection_frequency_list =
				station_datatype_frequency_list(
					application_name,
					station_datatype->
						station_name,
					station_datatype->
						datatype->
						datatype_name );
		}

		if ( station_datatype_frequency_reject(
			station_datatype->data_collection_frequency_list,
			m->measurement->measurement_date,
			m->measurement->measurement_time ) )
		{
			fprintf( stderr,
				 "Violates DATA_COLLECTION_FREQUENCY: %s\n",
			 	 delimited_record );
			continue;
		}

#ifdef NOT_DEFINED
		printf( "%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			measurement_display_delimiter(
				m->measurement,
				delimiter ) );
#endif

		printf( "%s\n", delimited_record );
	}

	return 0;

} /* main() */

