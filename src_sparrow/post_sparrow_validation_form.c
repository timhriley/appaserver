/* ---------------------------------------------------	*/
/* src_sparrow/post_sparrow_validation_form.c		*/
/* ---------------------------------------------------	*/
/*							*/
/* This script is attached to the submit button on 	*/
/* the sparrow validation form.				*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "date.h"
#include "folder.h"
#include "operation.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "session.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"
#include "sparrow_library.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"validate_site_visit"
#define USER_COLUMN_NAME	"person_validating"
#define DATE_COLUMN_NAME	"date_validated"
#define FLAG_COLUMN_NAME	"validated_yn"

/* Prototypes */
/* ---------- */
char *get_title_string(		char *site_id,
				int survey_year,
				int survey_number );

void post_sparrow_validation_form(
				char *application_name,
				char *quad_sheet,
				int site_number,
				char *visit_date,
				char *login_name,
				char *validation_date );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session;
	char *quad_sheet;
	int site_number;
	char *visit_date;
	char *validation_date;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application login_name session quad_sheet site_number visit_date\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	login_name = argv[ 2 ];
	session = argv[ 3 ];
	quad_sheet = argv[ 4 ];
	site_number = atoi( argv[ 5 ] );
	visit_date = argv[ 6 ];

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access(	application_name,
				session,
				login_name ) )
	{
		session_access_failed_message_and_exit(
			application_name, session, login_name );
	}

	if ( !session_access_process(
					application_name,
					session,
					PROCESS_NAME,
					login_name,
					(char *)0 /* role_name */ ) )
	{
		session_access_failed_message_and_exit(
			application_name, session, login_name );
	}

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

	validation_date =
		date_get_now_yyyy_mm_dd(
			date_get_utc_offset() );

	post_sparrow_validation_form(
				application_name,
				quad_sheet,
				site_number,
				visit_date,
				login_name,
				validation_date );

	document_close();

	exit( 0 );

} /* main() */

void post_sparrow_validation_form(
				char *application_name,
				char *quad_sheet,
				int site_number,
				char *visit_date,
				char *login_name,
				char *validation_date )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *key_list_string;
	FILE *output_pipe;
	char *title;

	title = sparrow_library_get_title_string(
				quad_sheet,
				site_number,
				visit_date );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	key_list_string = "quad_sheet,site_number,visit_date";

	table_name = get_table_name( application_name, "site_visit" );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql.e						 ",
		 table_name,
		 key_list_string );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%d^%s^%s^%s\n",
		 quad_sheet,
		 site_number,
		 visit_date,
		 USER_COLUMN_NAME,
		 login_name );

	fprintf( output_pipe,
		 "%s^%d^%s^%s^%s\n",
		 quad_sheet,
		 site_number,
		 visit_date,
		 DATE_COLUMN_NAME,
		 validation_date );

	fprintf( output_pipe,
		 "%s^%d^%s^%s^y\n",
		 quad_sheet,
		 site_number,
		 visit_date,
		 FLAG_COLUMN_NAME );

	printf( "<h3>Validation complete.</h3>\n" );

	pclose( output_pipe );

} /* post_sparrow_validation_form() */

