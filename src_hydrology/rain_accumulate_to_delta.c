/* --------------------------------------------------- 	*/
/* rain_accumulate_to_delta.c			       	*/
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
#include "query.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"
#include "hydrology_library.h"
#include "session.h"

/* Constants */
/* --------- */
#define PROCESS_NAME			"rain_accumulate_to_delta"
#define INPUT_DELIMITER			'|'
#define SOURCE_DATATYPE			"rain_cumulative"
#define DESTINATION_DATATYPE		"rain"
#define MEASUREMENT_SELECT_LIST		 "station,datatype,measurement_date,measurement_time,measurement_value"
#define DATATYPE_PIECE			1
#define MEASUREMENT_VALUE_PIECE		4

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	register int counter = 0;
	char *application_name;
	char *station;
	char *begin_date, *end_date;
	char where_clause[ 1024 ];
	char record[ 4096 ];
	char buffer[ 4096 ];
	char *table_name;
	FILE *input_pipe;
	FILE *insert_pipe;
	char measurement_value_string[ 128 ];
	double measurement_value;
	double old_measurement_value = 0.0;
	double delta = 0.0;
	DOCUMENT *document;
	char really_yn;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	char *login_name;
	char *role_name;
	char *database_string = {0};

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s application session login_name role station begin_date end_date really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	station = argv[ 5 ];
	begin_date = argv[ 6 ];
	end_date = argv[ 7 ];
	really_yn = *argv[ 8 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	table_name = 
	get_table_name( application_name, "measurement" );

	sprintf( where_clause,
		 "    station = '%s'					  "
		 "and datatype = '%s'					  "
		 "and measurement_date between '%s' and '%s'		  ",
		 station,
		 SOURCE_DATATYPE,
		 begin_date,
		 end_date );

	sprintf(buffer, 
		"get_folder_data	application=%s			  "
		"			folder=%s			  "
		"			select='%s'			  "
		"			where=\"%s\"			 |"
		"tr '%c' '%c'						  ",
		application_name,
		"measurement",
		MEASUREMENT_SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER );

	input_pipe = popen( buffer, "r" );

	sprintf(buffer,
		"insert_statement.e %s %s | sql 2>&1 | html_paragraph_wrapper",
		table_name,
		MEASUREMENT_SELECT_LIST );

	insert_pipe = popen( buffer, "w" );

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

	printf( "<h3>Accumulate rain for %s</h3>\n", station);

	old_measurement_value =
		hydrology_library_get_latest_before_measurement_value(
			application_name,
			station,
			SOURCE_DATATYPE,
			begin_date );

	/* Sample input: "BD,stage,1999-01-01,1000,10" */
	/* ------------------------------------------- */
	while( get_line( record, input_pipe ) )
	{
		counter++;

		if ( really_yn != 'y' )
		{
			printf( "<br>%s ==>", record );
		}

		if ( !piece( 	measurement_value_string,
				INPUT_DELIMITER,
				record,
				MEASUREMENT_VALUE_PIECE ) )
		{
			fprintf(stderr,
				"Error in %s: bad input: (%s)\n",
				argv[ 0 ],
				record );
			exit( 1 );
		}

		if ( *measurement_value_string )
		{
			measurement_value = atof( measurement_value_string );
	
			/* If they reset the device */
			/* ------------------------ */
			if ( measurement_value < old_measurement_value )
			{
				delta = measurement_value;
				old_measurement_value = 
					measurement_value;
			}
			else
			if ( measurement_value == old_measurement_value)
			{
				delta = 0.0;

				old_measurement_value = 
					measurement_value;
			}
			else
			if ( measurement_value > old_measurement_value )
			{
				delta = 
					measurement_value - 
					old_measurement_value;
	
				old_measurement_value = 
					measurement_value;
			}
	
			sprintf( measurement_value_string, "%lf", delta );
	
			replace_piece(	record, 
					INPUT_DELIMITER, 
					DESTINATION_DATATYPE,
					DATATYPE_PIECE );
	
			replace_piece(	record, 
					INPUT_DELIMITER, 
					measurement_value_string,
					MEASUREMENT_VALUE_PIECE );
	
			if ( really_yn == 'y' )
			{
				fprintf( insert_pipe, "%s\n", record );
			}
			else
			{
				printf( "%s=%lf\n",
					DESTINATION_DATATYPE,
					delta );
			}
		}
		else
			printf( "null(ignored)\n" );
	}

	pclose( input_pipe );
	pclose( insert_pipe );

	if ( really_yn == 'y' )
	{
		printf(
		"<br>Processed complete. Records processed: %d\n", counter );
		process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( 
		"<br>Not processed. Records to process: %d\n", counter );
	}

	document_close();
	exit( 0 );
} /* main() */

