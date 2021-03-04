/* ---------------------------------------------------	*/
/* src_alligator/visit_dataentry_wizard.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"
#include "process.h"
#include "date.h"
#include "boolean.h"
#include "piece.h"
#include "appaserver_link_file.h"
#include "alligator.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
/*
#define OUTPUT_TEMPLATE			"%s/%s/visit_dataentry_wizard_%d.html"
#define DISPLAY_PREPEND_TEMPLATE	"%s://%s/%s/visit_dataentry_wizard_%d.html"
#define DISPLAY_NONPREPEND_TEMPLATE	"/%s/visit_dataentry_wizard_%d.html"
*/
#define TARGET_WINDOW			"dataentry_window"

/* Prototypes */
/* ---------- */
/*
boolean get_nest_information(
		char **utm_easting,
		char **utm_northing,
		char **discovery_date_string,
		char *application_name,
		char *nest_number );
*/

char *get_current_year( void );

int main( int argc, char **argv )
{
	char *application_name;
	char full_application_name[ 128 ];
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	FILE *output_file;
	char *output_filename;
	char *display_filename;
	char sys_string[ 1024 ];
	pid_t process_id;
	char *login_name;
	char *session;
	char *role_name;
	char *nest_number;
	char title[ 128 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application login_name session role process_name nest_number\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];
	session = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	nest_number = argv[ 6 ];

	strcpy( full_application_name, application_name );
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

	appaserver_parameter_file = appaserver_parameter_file_new();

	process_id = getpid();

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->document_root,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"html" );

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

	display_filename =
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

	document = document_new( "", application_name );
	document_set_output_content_type( document );
	
	document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );

/*
	if ( ! get_nest_information(
		&utm_easting,
		&utm_northing,
		&discovery_date_string,
		application_name,
		nest_number ) )
	{
		printf( "<H2>ERROR: Cannot get nest information for %s\n",
			nest_number );
		document_close();
		exit( 1 );
	}
*/

/*
	sprintf( output_filename, 
		 OUTPUT_TEMPLATE,
		 appaserver_parameter_file->appaserver_mount_point,
		 application_name, 
		 process_id );
*/

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	sprintf( title, "Nest number: %s", nest_number );

	sprintf(	sys_string,
"cat %s/src_alligator/visit_dataentry_wizard_template.html		|"
"dot_process2option_value %s						|"
"sed 's/^\\.post_action/action=\"\\/cgi-bin\\/post_alligator_visit_dataentry_wizard?%s+%s+%s+%s\"/'								|"
"sed 's/^\\.nest_number/var nest_number=\"%s\"/;'			|"
"sed 's/^\\.title/%s/'							|"
"cat > %s								 ",
			appaserver_parameter_file->appaserver_mount_point,
			application_name,
			login_name,
			full_application_name,
			session,
			role_name,
			nest_number,
			title,
			output_filename );
/*
fprintf( stderr, "%s\n", sys_string );
*/
	system( sys_string );

	printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s','menubar=no,resizeable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false');\">\n",
			application_background_color(
				application_name ),
			display_filename,
			TARGET_WINDOW );

	printf( "<h2>Visit Dataentry Wizard opened in new window \n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	printf( "</h2>\n" );

	printf( "<a href='%s' target=%s>Press for wizard.</a>\n",
		display_filename,
		TARGET_WINDOW );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

boolean get_nest_information(
		char **utm_easting,
		char **utm_northing,
		char **discovery_date_string,
		char *application_name,
		char *nest_number )
{
	char sys_string[ 1024 ];
	static char local_utm_easting[ 16 ];
	static char local_utm_northing[ 16 ];
	static char local_discovery_date_string[ 16 ];
	char where_clause[ 1024 ];
	char *results;
	char *select;

	select = "utm_easting,utm_northing,discovery_date";

	sprintf(where_clause,
		"discovery_date like '%s%c' and			"
		"nest_number = %s				",
		get_current_year(),
		'%',
		nest_number );

	sprintf(sys_string,
		"get_folder_data	application=%s		 "
		"			select=%s		 "
		"			folder=nest		 "
		"			where=\"%s\"		|"
		"head -1					 ",
		application_name,
		select,
		where_clause );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece(	local_utm_easting,
		FOLDER_DATA_DELIMITER,
		results,
		0 );

	piece(	local_utm_northing,
		FOLDER_DATA_DELIMITER,
		results,
		1 );

	piece(	local_discovery_date_string,
		FOLDER_DATA_DELIMITER,
		results,
		2 );

	*utm_easting = local_utm_easting;
	*utm_northing = local_utm_northing;
	*discovery_date_string = local_discovery_date_string;

	return 1;
} /* get_nest_information() */

char *get_current_year( void )
{
	char *yyyy_mm_dd;
	static char current_year[ 16 ];

	yyyy_mm_dd = date_get_current_yyyy_mm_dd( date_get_utc_offset() );
	return piece( current_year, '-', yyyy_mm_dd, 0 );
} /* get_current_year() */
