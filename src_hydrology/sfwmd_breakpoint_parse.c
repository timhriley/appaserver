/* -------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/sfwmd_breakpoint_parse.c	*/
/* -------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------	*/

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
#include "column.h"
#include "application.h"
#include "basename.h"
#include "date_convert.h"
#include "sfwmd_station_datatype.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
SFWMD_STATION_DATATYPE *get_sfwmd_station_datatype(
				HASH_TABLE *sfwmd_station_datatype_hash_table,
				char *input_buffer );

char *to_yyyy_mm_dd( char *d, char *s );

char *to_hhmm( char *d, char *s );

void sfwmd_breakpoint_parse(		char *application_name,
					char *filename );

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

	sfwmd_breakpoint_parse( application_name, filename );

	return 0;

} /* main() */

char *to_hhmm( char *d, char *s )
{
	char time_only[ 128 ];

	if ( !column( time_only, 1, s ) ) return (char *)0;

	if ( character_count( ':', time_only ) == 2 )
	{
		piece_inverse( time_only, ':', 2 );
	}

	timlib_delete_character( time_only, ':' );

	sprintf( d, 
		 "%04d",
		 atoi( time_only ) );

	return d;

} /* to_hhmm() */

char *to_yyyy_mm_dd( char *d, char *s )
{
	char date_only[ 128 ];
	DATE_CONVERT *date;

	column( date_only, 0, s );

	date = date_convert_new_date_convert( 
				international,
				date_only );

	strcpy( d, date->return_date );
	date_convert_free( date );

	return d;

} /* to_yyyy_mm_dd() */

void sfwmd_breakpoint_parse(	char *application_name,
				char *filename )
{
	HASH_TABLE *sfwmd_station_datatype_hash_table;
	FILE *input_file;
	char input_buffer[ 1024 ];
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;

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

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		if ( ! ( sfwmd_station_datatype =
				get_sfwmd_station_datatype(
					sfwmd_station_datatype_hash_table,
					input_buffer ) ) )
		{
			fprintf( stderr,
				 "Cannot find dbkey in %s\n",
				 input_buffer );
			continue;
		}

		if ( !*sfwmd_station_datatype->station )
		{
			fprintf( stderr,
				 "Cannot find station in %s\n",
				 input_buffer );
			continue;
		}

		if ( !*sfwmd_station_datatype->datatype )
		{
			fprintf( stderr,
				 "Cannot find datatype in %s\n",
				 input_buffer );
			continue;
		}


		if ( sfwmd_station_datatype->is_null )
		{
		 	printf( "%s^%s^%s^%s^\n",
				 sfwmd_station_datatype->station,
				 sfwmd_station_datatype->datatype,
				 sfwmd_station_datatype->
					measurement_date_string,
				 sfwmd_station_datatype->
					measurement_time_string );
		}
		else
		{
		 	printf( "%s^%s^%s^%s^%.3lf\n",
				 sfwmd_station_datatype->station,
				 sfwmd_station_datatype->datatype,
				 sfwmd_station_datatype->
					measurement_date_string,
				 sfwmd_station_datatype->
					measurement_time_string,
				 sfwmd_station_datatype->
					measurement_value );
		}
	}

	fclose( input_file );

} /* load_sfwmd_breakpoint_data() */

SFWMD_STATION_DATATYPE *get_sfwmd_station_datatype(
				HASH_TABLE *sfwmd_station_datatype_hash_table,
				char *input_buffer )
{
	char process_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;
	static char measurement_date_string[ 16 ];
	static char measurement_time_string[ 16 ];
	char *measurement_date_pointer;
	char *measurement_time_pointer;

	strcpy( process_buffer, input_buffer );
	search_replace_string( process_buffer, "\"", "" );

	/* piece db_key */
	/* ------------ */
	if ( !piece( piece_buffer, ',', process_buffer, 2 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( sfwmd_station_datatype =
			hash_table_fetch(
				sfwmd_station_datatype_hash_table,
				piece_buffer ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	/* piece measurement_date */
	/* ---------------------- */
	if ( !piece( piece_buffer, ',', process_buffer, 0 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( measurement_date_pointer =
			to_yyyy_mm_dd(
				measurement_date_string,
				piece_buffer ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( measurement_time_pointer =
			to_hhmm(
				measurement_time_string,
				piece_buffer ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	strcpy(	sfwmd_station_datatype->measurement_date_string,
		measurement_date_pointer );

	strcpy(	sfwmd_station_datatype->measurement_time_string,
		measurement_time_pointer );

	/* piece measurement_value */
	/* ----------------------- */
	if ( !piece( piece_buffer, ',', process_buffer, 3 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( !*piece_buffer )
	{
		sfwmd_station_datatype->is_null = 1;
	}
	else
	{
		sfwmd_station_datatype->is_null = 0;
		sfwmd_station_datatype->measurement_value =
			atof( piece_buffer );
	}

	if ( sfwmd_station_datatype->conversion_factor )
	{
		sfwmd_station_datatype->measurement_value *=
			sfwmd_station_datatype->conversion_factor;
	}

	strcpy(	sfwmd_station_datatype->last_validation_date_string,
		sfwmd_station_datatype->measurement_date_string );

	return sfwmd_station_datatype;

} /* get_sfwmd_station_datatype() */

