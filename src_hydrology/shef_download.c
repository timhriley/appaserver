/* ---------------------------------------------------	*/
/* src_hydrology/shef_download.c			*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "hydrology_library.h"
#include "shef_datatype_code.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "validation_level.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"shef_download"
#define COLUMN_LIST		 "station,datatype,measurement_date,measurement_time,measurement_value"
#define EMAIL_SUBJECT		"Shef Formatted Output"

/* Prototypes */
/* ---------- */
void run_process(	char *application_name,
			char *output_filename,
			char *where_clause,
			char *email_address,
			int expected_count_per_day );

int main( int argc, char **argv )
{
	char *person, *application_name, *session, *station, *datatype;
	char *begin_date, *end_date;
	char where_clause[ 1024 ];
	char *email_address;
	char *output_filename;
	char *ftp_filename;
	DOCUMENT *document;
	int expected_count_per_day;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int process_id = getpid();
	char *validation_level_string;
	enum validation_level validation_level;
	char *database_string = {0};
	APPASERVER_LINK_FILE *appaserver_link_file;
				
	if ( argc < 9 )
	{
		fprintf( stderr, 
"Usage: %s person session application station datatype begin_date end_date validation_level [email_address]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	person = argv[ 1 ];
	session = argv[ 2 ];
	application_name = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	begin_date = argv[ 6 ];
	end_date = argv[ 7 ];
	validation_level_string = argv[ 8 ];

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

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station,
					datatype );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					appaserver_parameter_file->
						database_management_system,
					(PROCESS_GENERIC_OUTPUT *)0,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			PROCESS_NAME /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( argc == 10
	&&   *argv[ 9 ]
	&&   strcmp( argv[ 9 ], "email_address" ) != 0 )
		email_address = argv[ 9 ];
	else
		email_address = (char *)0;

	validation_level =
		validation_level_get_validation_level(
			validation_level_string);

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	if ( !*datatype || strcmp( datatype, "datatype" ) == 0 )
	{
		sprintf( where_clause,
		 	"station = '%s' and				"
		 	"measurement_date between '%s' and '%s' and	"
		 	"datatype <> 'voltage'				",
		 	station,
		 	begin_date,
		 	end_date );
	}
	else
	{
		sprintf( where_clause,
		 	"station = '%s' and				"
		 	"datatype = '%s' and				"
		 	"measurement_date between '%s' and '%s' and	"
		 	"datatype <> 'voltage'				",
		 	station,
		 	datatype,
		 	begin_date,
		 	end_date );
	}

	strcat( where_clause,
		/* ----------------------- */
		/* Returns program memory. */
		/* ----------------------- */
		hydrology_library_provisional_where(
			validation_level ) );

	expected_count_per_day = get_latest_measurements_per_day(
					application_name,
					station,
					datatype );

	if ( !expected_count_per_day ) expected_count_per_day = 24;

	run_process(	application_name,
			output_filename,
			where_clause,
			email_address,
			expected_count_per_day );

	printf( "<h1>Shef Formatted Transmission<br></h1>\n" );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	search_replace_string( 
		where_clause, "1 = 1 and ", "" );
	search_replace_string( 
		where_clause, "1 = 1", "" );

	printf( "<BR><p>Search criteria: %s<hr>\n", 
		where_clause );

	if ( email_address 
	&&   strcmp( email_address, "email_address" ) != 0 )
	{
		printf( "<BR><p>Output emailed to %s\n", 
			email_address );
	}
	else
	{
		appaserver_library_output_ftp_prompt(
					ftp_filename, 
					TRANSMIT_PROMPT,
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
	}

	document_close();
	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void run_process(	char *application_name,
			char *output_filename,
			char *where_clause,
			char *email_address,
			int expected_count_per_day )
{
	char buffer[ 1024 ];
	char *nohup_command, *background_command;
	char email_command[ 1024 ];

	if ( email_address )
	{
		nohup_command = "nohup";
		background_command = "&";
		sprintf( email_command,
			 "mailx -s \"%s\" %s",
			 EMAIL_SUBJECT,
			 email_address );
	}
	else
	{
		nohup_command = "";
		background_command = "";
		sprintf( email_command, "cat > %s", output_filename );
	}

	sprintf( buffer,
	"%s get_folder_data	application=%s				  "
	"			folder=measurement			  "
	"			select='%s'				  "
	"			where=\"%s\"				 |"
	"tr '%c' '|'							 |"
	"toupper_to_delimiter.e '|'					 |"
	"measurement2shef %s %d						 |"
	"%s %s								  ",
		 nohup_command,
		 application_name,
		 COLUMN_LIST,
		 where_clause,
		 FOLDER_DATA_DELIMITER,
		 application_name,
		 expected_count_per_day,
		 email_command,
		 background_command );

	system( buffer );
} /* run_process() */
