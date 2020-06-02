/* ---------------------------------------------------	*/
/* src_hydrology/post_hydrology_validation_form.c	*/
/* ---------------------------------------------------	*/
/*							*/
/* This script is attached to the submit button on 	*/
/* the hydrology validation form.			*/
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
#include "insert_database.h"
#include "update_database.h"
#include "query.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "measurement_validation.h"
#include "application.h"
#include "session.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
char *get_salinity_datatype(
						char *application_name,
						char *conductivity_datatype );

LIST *get_conductivity_datatype_list(
						char *application_name );

char *insert_salinity(				char *application_name,
						char *login_name,
						char *station,
						char *begin_measurement_date,
						char *end_measurement_date,
						char *conductivity_datatype );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *station, *datatype;
	DOCUMENT *document;
	char *validation_date, *validation_time;
	char *begin_measurement_date, *end_measurement_date;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	LIST *conductivity_datatype_list;
	char *salinity_datatype;
	int return_count;

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s login_name application session station datatype begin_date end_date ignored\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	begin_measurement_date = argv[ 6 ];
	end_measurement_date = argv[ 7 ];
	/* frameset_name = argv[ 8 ]; */

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

	if ( !appaserver_library_validate_begin_end_date(
					&begin_measurement_date,
					&end_measurement_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
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
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	validation_time =
		date_get_now_time_hhmm_colon_ss(
			date_get_utc_offset() );

	validation_date =
		date_get_now_date_yyyy_mm_dd(
			date_get_utc_offset() );

	measurement_validation_insert_into_manual_validation_event(
		application_name,
		login_name,
		station,
		datatype,
		begin_measurement_date,
		end_measurement_date,
		validation_date,
		validation_time,
		MEASUREMENT_VALIDATION_VALIDATION_PROCESS );

	return_count = measurement_validation_update_measurement(
					application_name,
					login_name,
					station,
					datatype,
					validation_date,
					begin_measurement_date,
					end_measurement_date );

	printf( "<p>Measurement validation of %s/%s posted %d measurements "
		"by %s at %s %s.\n",
		station,
		datatype,
		return_count,
		login_name,
		validation_date,
		validation_time );

	conductivity_datatype_list =
		get_conductivity_datatype_list(
			application_name );

	if ( list_exists_string( conductivity_datatype_list, datatype ) )
	{
		salinity_datatype =
			insert_salinity(	application_name,
						login_name,
						station,
						begin_measurement_date,
						end_measurement_date,
						datatype );

		measurement_validation_update_measurement(
					application_name,
					login_name,
					station,
					salinity_datatype,
					validation_date,
					begin_measurement_date,
					end_measurement_date );

		printf( "<p>Measurement validation of %s/%s posted.\n",
			station,
			salinity_datatype );
	}

	document_close();

	exit( 0 );

} /* main() */

char *insert_salinity(	char *application_name,
			char *login_name,
			char *station,
			char *begin_measurement_date,
			char *end_measurement_date,
			char *conductivity_datatype )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	DICTIONARY *parameter_dictionary;
	char *salinity_datatype;

	parameter_dictionary = dictionary_new();

	dictionary_set_string(	parameter_dictionary,
				"from_measurement_date",
				begin_measurement_date );

	dictionary_set_string(	parameter_dictionary,
				"relation_operator_measurement_date",
				"between" );

	dictionary_set_string(	parameter_dictionary,
				"to_measurement_date",
				end_measurement_date );

	dictionary_set_string(	parameter_dictionary,
				"conductivity_datatype_1",
				conductivity_datatype );

	sprintf( where_clause,
"station = '%s' and measurement_date between '%s' and '%s' and datatype = '%s'",
		 station,
		 begin_measurement_date,
		 end_measurement_date,
		 conductivity_datatype );

	sprintf(sys_string,
		"conductivity2salinity '%s' '%s' '%s' '%s' \"%s\" y nohtml",
		login_name,
		application_name,
		station,
		dictionary_display_delimited( parameter_dictionary, '&' ),
		where_clause );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	salinity_datatype =
		get_salinity_datatype(
			application_name,
			conductivity_datatype );

	return salinity_datatype;

} /* insert_salinity() */

LIST *get_conductivity_datatype_list( char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=conductivity_datatype	 "
		 "			folder=conductivity_salinity_map ",
		 application_name );

	return pipe2list( sys_string );

} /* get_conductivity_datatype_list() */

char *get_salinity_datatype(
			char *application_name,
			char *conductivity_datatype )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf( where, "conductivity_datatype = '%s'", conductivity_datatype );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=conductivity_datatype	 "
		 "			folder=conductivity_salinity_map "
		 "			where=\"%s\"			 ",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* get_salinity_datatype() */
