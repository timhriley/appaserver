/* --------------------------------------------------- 	*/
/* hydrology_verification_process.c		       	*/
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
#include "date.h"
#include "list.h"
#include "piece.h"
#include "application.h"
#include "hash_table.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"
#include "session.h"

/* Constants */
/* --------- */
#define PROCESS_NAME				"verification_process"
#define STATION_DATATYPE_PRIMARY_KEY		"station,datatype"
#define F_ARRAY_PREVIOUS_COUNT			0
#define F_ARRAY_PREVIOUS_SUM			1
#define F_ARRAY_PREVIOUS_AVERAGE		2
#define F_ARRAY_PREVIOUS_LOW			3
#define F_ARRAY_PREVIOUS_HIGH			4

/* Structures */
/* ---------- */
typedef struct
{
	int expected_count;
	double warning_upper_bound;
	double warning_lower_bound;
	int warning_upper_bound_ignore;
	int warning_lower_bound_ignore;
} STATION_DATATYPE_EXPECTED;

/* Prototypes */
/* ---------- */
char *get_count_warning_yn( 	
			double current_count,
			char *station,
			char *datatype,
			HASH_TABLE *station_datatype_expected_hash_table );
char *get_bound_warning_yn( 	
			double current_low,
			double current_high,
			char *station,
			char *datatype,
			HASH_TABLE *station_datatype_expected_hash_table );
STATION_DATATYPE_EXPECTED *station_datatype_expected_new(
			char *expected_count_string,
			char *warning_upper_bound_string,
			char *warning_lower_bound_string );
HASH_TABLE *get_station_datatype_expected_hash_table( 
					char *table_name );
void run_process( 	char *application_name,
			char *date_string, 
			int html_ok,
			HASH_TABLE *station_datatype_expected_hash_table );

void get_statistics_for_station_datatype( 	
					double *f_array,
					char *station, 
					char *datatype,
					char *date_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *table_name;
	char *date_string;
	int html_ok = 1;
	DOCUMENT *document = {0};
	HASH_TABLE *station_datatype_expected_hash_table;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc < 6 )
	{
		fprintf(stderr,
"Usage: %s ignored ignored ignored ignored today|yesterday|yyyy-mm-dd [nohtml]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	/* application_name = argv[ 1 ]; */
	/* session = argv[ 2 ]; */
	/* login_name = argv[ 3 ]; */
	/* role_name = argv[ 4 ]; */
	date_string = argv[ 5 ];

	if ( argc == 7 )
		html_ok = ( strcmp( argv[ 6 ], "nohtml" ) != 0 );

	appaserver_parameter_file = new_appaserver_parameter_file();

	table_name = 
		get_table_name( application_name, "station_datatype" );

	station_datatype_expected_hash_table = 
		get_station_datatype_expected_hash_table( table_name );

	if ( strcmp( date_string, "today" ) == 0 )
	{
		DATE *date = date_get_today_new( date_get_utc_offset() );
		date_string = date_get_yyyy_mm_dd_string( date );
	}
	else
	if ( strcmp( date_string, "yesterday" ) == 0  
	||   strcmp( date_string, "YESTERDAY" ) == 0 )
	{
		date_string =
			date_get_yesterday_yyyy_mm_dd_string(
				date_get_utc_offset() );
	}

	if ( html_ok )
	{
		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );
	
		printf( "<br><p>" );
	}

	printf( "Load statistics report for date: %s\n", date_string );

	if ( html_ok ) printf( "<br><hl>\n" );

	/* Do the work */
	/* ----------- */
	run_process(	application_name,
			date_string, 
			html_ok,
			station_datatype_expected_hash_table );

	if ( html_ok ) printf( "<BR><h1>" );

	printf( "Process complete.\n" );

	if ( html_ok ) document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

void run_process(	char *application_name,
			char *date_string, 
			int html_ok,
			HASH_TABLE *station_datatype_expected_hash_table )
{
	char sys_string[ 128 ];
	LIST *datatype_list;
	LIST *station_list;
	char *station, *datatype;
	double *f_array;
	LIST *primary_key_data_list;

station_datatype_expected_hash_table = (HASH_TABLE *)0;

	f_array = (double *)calloc( 5, sizeof( double ) );
	sprintf( sys_string, "telemetry_station_list.sh %s", application_name );
	station_list = pipe2list( sys_string );
	
	if ( !list_rewind( station_list ) )
	{
		fprintf( stderr, "Station list failed to load\n" );
		exit( 1 ); 
	}

	do {
		station = list_get_string( station_list );

		sprintf( sys_string,
			 "datatype_list.sh %s %s", 
			 application_name, station );
		datatype_list = pipe2list( sys_string );

		if ( list_rewind( datatype_list ) )
		{
			do {
				datatype = list_get_string( datatype_list );
				get_statistics_for_station_datatype(	
								f_array,
								station, 
								datatype, 
								date_string );

/*
				char *count_warning_yn;
				count_warning_yn = 
					get_count_warning_yn(
					   f_array[ F_ARRAY_PREVIOUS_COUNT ],
					   station,
					   datatype,
					   station_datatype_expected_hash_table
					 );

				char *bound_warning_yn;
				bound_warning_yn = 
					get_bound_warning_yn(
					   f_array[ F_ARRAY_PREVIOUS_LOW ],
					   f_array[ F_ARRAY_PREVIOUS_HIGH ],
					   station,
					   datatype,
					   station_datatype_expected_hash_table
					 );
*/

				/* Set the primary key data */
				/* ------------------------ */
				primary_key_data_list = list_new();
				list_append_string(	primary_key_data_list,
							station );
				list_append_string(	primary_key_data_list,
							datatype );

				if ( html_ok ) printf( "<pre>" );
				printf(
			"%-12.12s %23.23s count/sum/avg: ",
					station, 
					datatype );
				fflush( stdout );
	
				printf( 
			"%3.0lf/%8.2lf/%7.2lf\n",
					f_array[ F_ARRAY_PREVIOUS_COUNT ],
					f_array[ F_ARRAY_PREVIOUS_SUM ],
					f_array[ F_ARRAY_PREVIOUS_AVERAGE ] );
				if ( html_ok ) printf( "</pre>" );
				fflush( stdout );
	
			} while( list_next( datatype_list ) );
		}
	} while( list_next( station_list ) );
} /* run_process() */

void get_statistics_for_station_datatype( 	
					double *f_array,
					char *station, 
					char *datatype, 
					char *date_string )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "echo \"select count(*), 				"
		 "		sum( measurement_value ), 		"
		 "		avg( measurement_value ),		"
		 "		min( measurement_value ),		"
		 "		max( measurement_value )		"
		 "       from measurement				"
		 "	 where station = '%s'				"
		 "	   and datatype = '%s'			        "
		 "	   and measurement_date = '%s';\"	       |"
		 "sql.e '|'						",
		 station,
		 datatype,
		 date_string );

	results = pipe2string( sys_string );

	if ( results ) timlib_atof_array( f_array, results );

} /* get_statistics_for_station_datatype() */


HASH_TABLE *get_station_datatype_expected_hash_table( char *table_name )
{
	HASH_TABLE *h;
	char buffer[ 1024 ];
	char station_datatype[ 512 ];
	char expected_count_string[ 512 ];
	char warning_upper_bound_string[ 512 ];
	char warning_lower_bound_string[ 512 ];
	STATION_DATATYPE_EXPECTED *station_datatype_expected;
	FILE *p;

	h = hash_table_new( hash_table_medium );

	sprintf( buffer,
		 "echo \"select	concat( station, datatype),		 "
		 "		expected_count,				 "
		 "		warning_upper_bound,			 "
		 "		warning_lower_bound			 "
		 "	 from %s					 "
		 "	 order by station;\"				|"
		 "sql '|'						 ",
		 table_name );

	
	p = popen( buffer, "r" );
	while( get_line( buffer, p ) )
	{
		piece( station_datatype, '|', buffer, 0 );
		piece( expected_count_string, '|', buffer, 1 );
		piece( warning_upper_bound_string, '|', buffer, 2 );
		piece( warning_lower_bound_string, '|', buffer, 3 );

		station_datatype_expected = station_datatype_expected_new(
			expected_count_string,
			warning_upper_bound_string,
			warning_lower_bound_string );
		hash_table_set(	h, 
				strdup( station_datatype) , 
				station_datatype_expected );
	}
	pclose( p );
	return h;
} /* get_station_datatype_expected_hash_table() */

STATION_DATATYPE_EXPECTED *station_datatype_expected_new(
			char *expected_count_string,
			char *warning_upper_bound_string,
			char *warning_lower_bound_string )
{
	
	STATION_DATATYPE_EXPECTED *a;

	a = (STATION_DATATYPE_EXPECTED *)
			calloc( 1, sizeof( STATION_DATATYPE_EXPECTED ) );
	
	a->expected_count = atoi( expected_count_string );

	if ( !*warning_upper_bound_string )
		a->warning_upper_bound_ignore = 1;
	else
		a->warning_upper_bound = atof( warning_upper_bound_string );

	if ( !*warning_lower_bound_string )
		a->warning_lower_bound_ignore = 1;
	else
		a->warning_lower_bound = atof( warning_lower_bound_string );

	return a;
} /* station_datatype_expected_new() */

char *get_count_warning_yn( 	
			double current_count,
			char *station,
			char *datatype,
			HASH_TABLE *station_datatype_expected_hash_table )
{
	STATION_DATATYPE_EXPECTED *station_datatype_expected;
	char hash_table_key[ 512 ];

	sprintf( hash_table_key, "%s%s", station, datatype );
	station_datatype_expected = (STATION_DATATYPE_EXPECTED *)
			hash_table_get( station_datatype_expected_hash_table,
					hash_table_key );
	if ( !station_datatype_expected )
		return "n";
	else
	if ( !station_datatype_expected->expected_count )
		return "n";
	else
		return 
		( current_count != station_datatype_expected->expected_count )
			? "y" : "n";
} /* get_count_warning_yn() */

char *get_bound_warning_yn( 	
			double current_low,
			double current_high,
			char *station,
			char *datatype,
			HASH_TABLE *station_datatype_expected_hash_table )
{
	STATION_DATATYPE_EXPECTED *station_datatype_expected;
	char hash_table_key[ 512 ];

	sprintf( hash_table_key, "%s%s", station, datatype );
	station_datatype_expected = (STATION_DATATYPE_EXPECTED *)
			hash_table_get( station_datatype_expected_hash_table,
					hash_table_key );
	if ( !station_datatype_expected )
		return "n";
	else
	if ( !station_datatype_expected->warning_lower_bound_ignore
	&&   current_low <= station_datatype_expected->warning_lower_bound )
		return "y";
	else
	if ( !station_datatype_expected->warning_upper_bound_ignore
	&&   current_high >= station_datatype_expected->warning_upper_bound )
		return "y";
	else
		return "n";
} /* get_bound_warning_yn() */

