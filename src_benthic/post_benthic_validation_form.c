/* ---------------------------------------------------	*/
/* src_benthic/post_benthic_validation_form.c		*/
/* ---------------------------------------------------	*/
/*							*/
/* This script is attached to the submit button on 	*/
/* the benthic validation form.				*/
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
#include "sampling_point.h"

/* Constants */
/* --------- */
#define USER_COLUMN_NAME	"validated_login_name"
#define DATE_COLUMN_NAME	"validated_date"
#define TIME_COLUMN_NAME	"validated_time"

/* Prototypes */
/* ---------- */
char *get_title_string(		char *collection_name,
				char *location,
				int site_number );

void post_benthic_validation_form(
				char *application_name,
				char *anchor_date,
				char *anchor_time,
				char *location,
				char *site_number_string,
				char *login_name,
				char *validation_date,
				char *validation_time );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session;
	char *anchor_date;
	char *anchor_time;
	char *location;
	char *site_number_string;
	DOCUMENT *document;
	char *validation_date, *validation_time;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};

	if ( argc < 10 )
	{
		fprintf( stderr, 
"Usage: %s application login_name session ignored anchor_date anchor_time location site_number [ignored]\n",
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

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	login_name = argv[ 2 ];
	session = argv[ 3 ];
	/* process_name = argv[ 4 ]; */
	anchor_date = argv[ 5 ];
	anchor_time = argv[ 6 ];
	location = argv[ 7 ];
	site_number_string = argv[ 8 ];

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

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

	validation_time =
		date_get_now_time_hhmm(
			date_get_utc_offset() );

	post_benthic_validation_form(
				application_name,
				anchor_date,
				anchor_time,
				location,
				site_number_string,
				login_name,
				validation_date,
				validation_time );

	document_close();

	exit( 0 );

} /* main() */

void post_benthic_validation_form(
				char *application_name,
				char *anchor_date,
				char *anchor_time,
				char *location,
				char *site_number_string,
				char *login_name,
				char *validation_date,
				char *validation_time )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *key_list_string;
	FILE *output_pipe;
	char *title;
	int results;
	SAMPLING_POINT *sampling_point;

	sampling_point =
		sampling_point_new(
			anchor_date,
			anchor_time,
			location,
			atoi( site_number_string ) );

	if ( !sampling_point_load(
			&sampling_point->collection_name,
			&sampling_point->actual_latitude,
			&sampling_point->actual_longitude,
			&sampling_point->boat_lead_researcher,
			&sampling_point->second_boat_researcher,
			&sampling_point->collection_diver1,
			&sampling_point->collection_diver2,
			&sampling_point->begin_collection_dive_time,
			&sampling_point->end_collection_dive_time,
			&sampling_point->collection_air_pressure_in_PSI,
			&sampling_point->collection_air_pressure_out_PSI,
			&sampling_point->begin_habitat_dive_time,
			&sampling_point->end_habitat_dive_time,
			&sampling_point->habitat_diver,
			&sampling_point->habitat_air_pressure_in_PSI,
			&sampling_point->habitat_air_pressure_out_PSI,
			&sampling_point->notes,
			&sampling_point->bucket_number,
			&sampling_point->tide,
			&sampling_point->project,
			sampling_point,
			application_name ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: cannot seek sampling_point of (%s,%s,%s,%d)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			sampling_point->anchor_date,
			sampling_point->anchor_time,
			sampling_point->location,
			sampling_point->site_number );
		exit( 1 );
	}

	title = get_title_string(	sampling_point->collection_name,
					sampling_point->location,
					sampling_point->site_number );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	results = system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	key_list_string = "anchor_date,anchor_time,location,site_number";

	table_name = get_table_name( application_name, "sampling_point" );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql.e						 ",
		 table_name,
		 key_list_string );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^%s\n",
		 anchor_date,
		 anchor_time,
		 location,
		 site_number_string,
		 USER_COLUMN_NAME,
		 login_name );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^%s\n",
		 anchor_date,
		 anchor_time,
		 location,
		 site_number_string,
		 DATE_COLUMN_NAME,
		 validation_date );

	fprintf( output_pipe,
		 "%s^%s^%s^%s^%s^%s\n",
		 anchor_date,
		 anchor_time,
		 location,
		 site_number_string,
		 TIME_COLUMN_NAME,
		 validation_time );

	printf( "<h3>Validation complete.</h3>\n" );

	pclose( output_pipe );

} /* post_benthic_validation_form() */

char *get_title_string(	char *collection_name,
			char *location,
			int site_number )
{
	char static title[ 1024 ];

	sprintf(title,
		"Validate %s, %s/%d",
		collection_name,
		location,
		site_number );

	format_initial_capital( title, title );

	return title;
} /* get_title_string() */

