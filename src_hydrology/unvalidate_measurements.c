/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/unvalidate_measurements.c	*/
/* --------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "date.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "measurement_validation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void get_validation_information(
		char *begin_measurement_date,
		char *end_measurement_date,
		char *station,
		char *datatype,
		char *application_name,
		char *validation_login_name,
		char *validation_date,
		char *validation_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *validation_login_name;
	char *validation_date;
	char *validation_time;
	char *now_validation_date;
       	char *now_validation_time;
	char *now_login_name;
	char begin_measurement_date[ 16 ], end_measurement_date[ 16 ];
	char station[ 16 ], datatype[ 32 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
				
	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s application process now_login_name validation_login_name validation_date validation_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	now_login_name = argv[ 3 ];
	validation_login_name = argv[ 4 ];
	validation_date = argv[ 5 ];
	validation_time = argv[ 6 ];

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

	now_validation_time =
		date_get_now_time_hhmm_colon_ss(
			date_get_utc_offset() );

	now_validation_date =
		date_get_now_date_yyyy_mm_dd(
			date_get_utc_offset() );

	get_validation_information(
		begin_measurement_date,
		end_measurement_date,
		station,
		datatype,
		application_name,
		validation_login_name,
		validation_date,
		validation_time );

	measurement_validation_insert_into_manual_validation_event(
		application_name,
		now_login_name,
		station,
		datatype,
		begin_measurement_date,
		end_measurement_date,
		now_validation_date,
		now_validation_time,
		MEASUREMENT_VALIDATION_UNVALIDATION_PROCESS );

	measurement_validation_update_measurement(
					application_name,
					now_login_name,
					station,
					datatype,
					(char *)0 /* validation_date */,
					begin_measurement_date,
					end_measurement_date );

	printf( "<p>Measurement %s of %s/%s posted<br>"
		"by %s at %s %s.\n",
		MEASUREMENT_VALIDATION_UNVALIDATION_PROCESS,
		station,
		datatype,
		now_login_name,
		now_validation_date,
		now_validation_time );

	process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

	document_close();
	exit( 0 );
} /* main() */

void get_validation_information(
		char *begin_measurement_date,
		char *end_measurement_date,
		char *station,
		char *datatype,
		char *application_name,
		char *validation_login_name,
		char *validation_date,
		char *validation_time )
{
	char *results;
	char sys_string[ 1024 ];
	char *select =
		"station,datatype,begin_measurement_date,end_measurement_date";
	char where_clause[ 256 ];

	sprintf(where_clause,
		"login_name = '%s' and			"
		"validation_date = '%s' and		"
		"validation_time = '%s'			",
		validation_login_name,
		validation_date,
		validation_time );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=manual_validation_event	"
		"			where=\"%s\"			",
		application_name,
		select,
		where_clause );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		char msg[ 1024 ];

		sprintf(msg,
"Error: Could not find the Manual Validation Event for login_name = %s, validation_date = %s, validation_time = %s.\n",
			validation_login_name,
			validation_date,
			validation_time );

		fprintf(stderr,
"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			msg );

		printf( "<h3>%s</h3>\n", msg );
		document_close();
		exit( 1 );
	}

	piece(	station,
		FOLDER_DATA_DELIMITER,
		results,
		0 );

	piece(	datatype,
		FOLDER_DATA_DELIMITER,
		results,
		1 );

	piece(	begin_measurement_date,
		FOLDER_DATA_DELIMITER,
		results,
		2 );

	piece(	end_measurement_date,
		FOLDER_DATA_DELIMITER,
		results,
		3 );

} /* get_validation_information() */

