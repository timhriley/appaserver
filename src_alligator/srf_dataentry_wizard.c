/* -----------------------------------------------------	*/
/* $APPASERVER_HOME/src_alligator/srf_dataentry_wizard.c	*/
/* -----------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------	*/

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
#include "appaserver_link_file.h"
#include "alligator.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define TARGET_WINDOW			"dataentry_window"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char full_application_name[ 128 ];
	char *discovery_date_string;
	char *primary_researcher;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	FILE *output_file;
	char *output_filename;
	char *display_filename;
	char sys_string[ 2048 ];
	pid_t process_id;
	char *login_name;
	char *session;
	char *role_name;
	char title[ 128 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s application login_name session role process_name discovery_date primary_researcher\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];
	session = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	discovery_date_string = argv[ 6 ];
	primary_researcher = argv[ 7 ];

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

	appaserver_parameter_file = new_appaserver_parameter_file();

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
	
	if ( !*discovery_date_string
	||   strcmp( discovery_date_string, "discovery_date" ) == 0 )
	{
		printf(
"<H3>ERROR: You must select a Discovery Date-Primary Researcher</H3>\n" );
		document_close();
		exit( 1 );
	}

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

	sprintf( title, "%s %s", discovery_date_string, primary_researcher );

	sprintf(	sys_string,
"cat %s/src_alligator/srf_dataentry_wizard_template.html		|"
"dot_process2option_value %s						|"
"sed 's/^\\.post_action/action=\"\\/cgi-bin\\/post_alligator_srf_dataentry_wizard?%s+%s+%s+%s\"/'								|"
"sed 's/^\\.discovery_date/var discovery_date=\"%s\"/;'			|"
"sed 's/^\\.primary_researcher/var primary_researcher=\"%s\";/'		|"
"sed 's/^\\.title/%s/'							|"
"cat > %s								 ",
			appaserver_parameter_file->appaserver_mount_point,
			application_name,
			login_name,
			full_application_name,
			session,
			role_name,
			discovery_date_string,
			primary_researcher,
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

	printf( "<h2>SRF Dataentry Wizard opened in new window.\n" );
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

