/* ---------------------------------------------------	*/
/* src_hydrology/sfwmd_spreadsheet_parse.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

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
SFWMD_STATION_DATATYPE *sfwmd_station_datatype_fetch(
				/* ------------------- */
				/* Expect stack memory */
				/* ------------------- */
				char *db_key,
				HASH_TABLE *sfwmd_station_datatype_hash_table,
				char *input_buffer );

char *to_yyyy_mm_dd( char *d, char *s );

int ora_month2integer( char *mon );

char *month_array[] = { 	"JAN",
				"FEB",
				"MAR",
				"APR",
				"MAY",
				"JUN",
				"JUL",
				"AUG",
				"SEP",
				"OCT",
				"NOV",
				"DEC",
				(char *)0 };

void sfwmd_spreadsheet_parse(	char *application_name,
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

	sfwmd_spreadsheet_parse(
		application_name,
		filename );

	exit( 0 );

} /* main() */

char *to_yyyy_mm_dd( char *d, char *s )
{
	int mm_int;
	char yyyy[ 128 ], mon[ 128 ], dd[ 128 ];

	if ( ! piece( dd, '-', s, 0 ) ) return (char *)0;
	if ( ! piece( mon, '-', s, 1 ) ) return (char *)0;
	if ( ! piece( yyyy, '-', s, 2 ) ) return (char *)0;

	mm_int = ora_month2integer( mon );
	if ( mm_int < 0 ) return (char *)0;

	sprintf( d, 
		 "%s-%02d-%02d",
		 yyyy,
		 mm_int + 1,
		 atoi( dd ) );

	return d;

} /* to_yyyy_mm_dd() */

int ora_month2integer( char *mon )
{
	char **array_ptr = month_array;
	register int i = 0;

	while( *array_ptr )
	{
		if ( strcmp( mon, *array_ptr ) == 0 )
		{
			return i;
		}
		else
		{
			i++;
			array_ptr++;
		}
	}
	return -1;
} /* ora_month2integer() */

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
		load_sfwmd_station_datatype_hash_table(
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
				sfwmd_station_datatype_fetch(
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
			printf(	"%s^%s^%s^null^\n",
				sfwmd_station_datatype->station,
				sfwmd_station_datatype->datatype,
				sfwmd_station_datatype->
					measurement_date_string );
		}
		else
		{
			printf(	"%s^%s^%s^null^%.3lf\n",
				sfwmd_station_datatype->station,
				sfwmd_station_datatype->datatype,
				sfwmd_station_datatype->
					measurement_date_string,
				sfwmd_station_datatype->
					measurement_value );
		}
	}

	fclose( input_file );

} /* sfwmd_spreadsheet_parse() */

SFWMD_STATION_DATATYPE *sfwmd_station_datatype_fetch(
				/* ------------------- */
				/* Expect stack memory */
				/* ------------------- */
				char *db_key,
				HASH_TABLE *sfwmd_station_datatype_hash_table,
				char *input_buffer )
{
	char process_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;
	static char measurement_date_string[ 16 ];
	char last_validation_date_string[ 16 ];
	char *measurement_date_pointer;
	char *last_validation_date_pointer;

	strcpy( process_buffer, input_buffer );
	search_replace_string( process_buffer, "\"", "" );

	/* piece db_key */
	/* ------------ */
	*db_key = '\0';
	if ( !piece( db_key, ',', process_buffer, 1 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( sfwmd_station_datatype =
		hash_table_fetch(
			sfwmd_station_datatype_hash_table,
			db_key ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	/* piece measurement_date */
	/* ---------------------- */
	if ( !piece( piece_buffer, ',', process_buffer, 2 ) )
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

	strcpy(	sfwmd_station_datatype->measurement_date_string,
		measurement_date_pointer );

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

	/* piece measurement_update_method */
	/* ------------------------------- */
	if ( !piece( piece_buffer, ',', process_buffer, 4 ) )
	{
		*sfwmd_station_datatype->measurement_update_method = '\0';
	}
	else
	{
		if ( strcasecmp(
			sfwmd_station_datatype->measurement_update_method,
			"m" ) == 0 )
		{
			sfwmd_station_datatype->is_null = 1;
			*sfwmd_station_datatype->
				measurement_update_method = '\0';
		}
		else
		{
			strcpy(	sfwmd_station_datatype->
					measurement_update_method,
				piece_buffer );
		}
	}

	/* piece last_validation_date */
	/* -------------------------- */
	if ( piece( piece_buffer, ',', process_buffer, 5 ) )
	{
		if ( ! ( last_validation_date_pointer =
				to_yyyy_mm_dd(
					last_validation_date_string,
					piece_buffer ) ) )
		{
			last_validation_date_pointer = "";
		}
	}
	else
	{
		last_validation_date_pointer = "";
	}

	strcpy(	sfwmd_station_datatype->last_validation_date_string,
		last_validation_date_pointer );

	return sfwmd_station_datatype;

} /* sfwmd_station_datatype_fetch() */

