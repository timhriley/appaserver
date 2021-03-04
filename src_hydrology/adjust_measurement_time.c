/* --------------------------------------------------- 	*/
/* src_hydrology/adjust_measurement_time.c	       	*/
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
#include "julian.h"
#include "update_statement.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"
#include "session.h"

/* Constants */
/* --------- */
#define PROCESS_NAME			"adjust_measurement_time"
#define INPUT_DELIMITER			'|'
#define UPDATE_ATTRIBUTE_LIST		 "measurement_date,measurement_time"
#define SELECT_ATTRIBUTE_LIST		 "station,datatype,measurement_date,measurement_time"
#define DATE_PIECE			2
#define TIME_PIECE			3

/* Prototypes */
/* ---------- */
void get_update_statement(	char *destination,
				char *record,
				char *date_string,
				char *time_string,
				char *application_name );

int main( int argc, char **argv )
{
	register int counter = 0;
	char *application_name;
	char *where_clause;
	char record[ 4096 ];
	char buffer[ 4096 ];
	char update_statement[ 4096 ];
	FILE *input_pipe;
	FILE *update_pipe;
	char date_string[ 128 ];
	char time_string[ 128 ];
	char *adjusted_date_string;
	char *adjusted_time_string;
	DOCUMENT *document;
	float hours_to_bump;
	int really_yn;
	JULIAN *julian;
	char sort_process[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s application ignored ignored ignored \"where_clause\" hours_to_bump really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	/* login_name = argv[ 3 ]; */
	/* role_name = argv[ 4 ]; */
	where_clause = argv[ 5 ];
	hours_to_bump = atof( argv[ 6 ] );
	really_yn = *argv[ 7 ];

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

	if ( !hours_to_bump )
	{
		printf( "<h3>Error: please enter an Hours To Bump.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( hours_to_bump > 0.0 )
		strcpy( sort_process, "sort -r" );
	else
		strcpy( sort_process, "cat" );

	sprintf(buffer, 
	"get_folder_data	application=%s				  "
	"			folder=%s				  "
	"			select='%s'				  "
	"			where=\"%s\"				  "
	"			order=select				 |"
		"tr '%c' '%c'						 |"
		"%s							  ",
		application_name,
		"measurement",
		SELECT_ATTRIBUTE_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		sort_process );

	input_pipe = popen( buffer, "r" );

	sprintf(buffer, "sql.e 2>&1 | html_paragraph_wrapper.e" );

	update_pipe = popen( buffer, "w" );

	/* Sample input: "BD,stage,1999-01-01,1000" */
	/* ---------------------------------------- */
	while( get_line( record, input_pipe ) )
	{
		counter++;

		if ( !piece( 	date_string,
				INPUT_DELIMITER,
				record,
				DATE_PIECE ) )
		{
			fprintf(stderr,
				"Error in %s: bad input: (%s)\n",
				argv[ 0 ],
				record );
			exit( 1 );
		}

		if ( !piece( 	time_string,
				INPUT_DELIMITER,
				record,
				TIME_PIECE ) )
		{
			fprintf(stderr,
				"Error in %s: bad input: (%s)\n",
				argv[ 0 ],
				record );
			exit( 1 );
		}

		julian = julian_yyyy_mm_dd_hhmm_new( date_string, time_string );

		julian->current =
			julian_increment_hours_double(
				julian->current,
				hours_to_bump );

		adjusted_date_string =
			julian_get_yyyy_mm_dd( julian->current );

		adjusted_time_string = julian_get_hhmm( julian->current );

		get_update_statement(
				update_statement,
				record,
				adjusted_date_string,
				adjusted_time_string,
				application_name );

		free( adjusted_date_string );
		free( adjusted_time_string );

		if ( really_yn == 'y' )
		{
			fprintf( update_pipe, "%s\n", update_statement );
		}
		else
		{
			printf( "<p>%s\n", update_statement );
		}
	}

	pclose( input_pipe );
	pclose( update_pipe );

	if ( really_yn == 'y' )
	{
		printf(
		"<p>Processed complete. Records processed: %d\n", counter );
		process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( 
		"<p>Not processed. Records to process: %d\n", counter );
	}

	document_close();
	exit( 0 );
} /* main() */


void get_update_statement(	char *destination,
				char *record,
				char *date_string,
				char *time_string,
				char *application_name )
{
	char *table_name;
	LIST *attribute_list;
	LIST *data_list;
	LIST *primary_attribute_list;
	LIST *primary_data_list;
	UPDATE_STATEMENT *u;

	table_name = get_table_name( application_name, "measurement" );

	attribute_list = list_new();
	list_append_string( attribute_list, "measurement_date" );
	list_append_string( attribute_list, "measurement_time" );

	data_list = list_new();
	list_append_string( data_list, date_string );
	list_append_string( data_list, time_string );

	primary_attribute_list = string2list( SELECT_ATTRIBUTE_LIST, ',' );
	primary_data_list = string2list( record, '|' );

	u = new_update_statement( table_name );
	update_statement_set_attribute_list( u, attribute_list );
	update_statement_set_data_list( u, data_list );

	update_statement_set_primary_attribute_list(
					u, primary_attribute_list );

	update_statement_set_primary_data_list( u, primary_data_list );

	if ( !update_statement_get( destination, u ) ) exit( 1 );

} /* get_update_statement() */

