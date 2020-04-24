/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/sfwmd_spreadsheet_parse.c	*/
/* --------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "sfwmd_station_datatype.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *filename;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	filename = argv[ 1 ];

	sfwmd_spreadsheet_parse(
		application_name,
		filename );

	exit( 0 );

} /* main() */

void sfwmd_spreadsheet_parse(	char *application_name,
				char *filename )
{
	HASH_TABLE *sfwmd_station_datatype_hash_table;
	FILE *input_file;
	char input_buffer[ 1024 ];
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;
	int line_number = 0;
	char db_key[ 1024 ];

	sfwmd_station_datatype_hash_table =
		sfwmd_station_datatype_hash_table_load(
			application_name );

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );
		exit( 1 );
	}

	while( get_line( input_buffer, input_file ) )
	{
		line_number++;

		*db_key = '\0';

		if ( ! ( sfwmd_station_datatype =
				sfwmd_parse(
					/* ------------------- */
					/* Expect stack memory */
					/* ------------------- */
					db_key,
					sfwmd_station_datatype_hash_table,
					input_buffer ) ) )
		{
			fprintf(stderr,
"Error in line %d: cannot get station datatype for dbkey=[%s] in [%s]\n",
				line_number,
				db_key,
				input_buffer );
			continue;
		}

		if ( !*sfwmd_station_datatype->station )
		{
			fprintf(stderr,
		"Error in line %d: cannot get station for [%s]\n",
				line_number,
				input_buffer );
			continue;
		}

		if ( !*sfwmd_station_datatype->datatype )
		{
			fprintf(stderr,
		"Error in line %d: cannot get datatype for [%s]\n",
				line_number,
				input_buffer );
			continue;
		}

		if ( sfwmd_station_datatype->is_null )
		{
			printf(	"%s^%s^%s^null^^%s^%s\n",
				sfwmd_station_datatype->station,
				sfwmd_station_datatype->datatype,
				sfwmd_station_datatype->
					measurement_date_string,
				sfwmd_station_datatype->
					last_validation_date,
				(sfwmd_station_datatype->provisional)
					? "provisional"
					: "" );
		}
		else
		{
			printf(	"%s^%s^%s^null^%.3lf^%s^%s\n",
				sfwmd_station_datatype->station,
				sfwmd_station_datatype->datatype,
				sfwmd_station_datatype->
					measurement_date_string,
				sfwmd_station_datatype->
					measurement_value,
				sfwmd_station_datatype->
					last_validation_date,
				(sfwmd_station_datatype->provisional)
					? "provisional"
					: "" );
		}
	}

	fclose( input_file );

} /* sfwmd_spreadsheet_parse() */

