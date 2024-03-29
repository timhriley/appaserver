/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/drop_table.c	       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "document.h"
#include "attribute.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "session.h"

/* Constants */
/* --------- */
#define SESSION_PROCESS_NAME	"table_rectification"

/* Prototypes */
/* ---------- */
char *get_sys_string( char *table_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *table_name;
	char *login_name;
	char *role_name;
	char *session;
	char *really_yn;
	char *sys_string;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
	"Usage: %s ignored session login_name role table really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	table_name = argv[ 5 ];
	really_yn = argv[ 6 ];

	/* ---------------------------------------------	*/
	/* Set environment because				*/
	/* this is executed from table_rectification.		*/
	/* ---------------------------------------------	*/
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

	/* ---------------------------------------------------- */
	/* Check path because this is executed from		*/
	/* table_rectification.					*/
	/* ---------------------------------------------------- */
	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	/* ---------------------------------------------------- */
	/* Check permissions because this is executed from	*/
	/* table_rectification.					*/
	/* ---------------------------------------------------- */
	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
			application_name,
			login_name );
	}

	if ( !session_access_process(
				application_name,
				session,
				SESSION_PROCESS_NAME,
				login_name,
				role_name ) )
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

	sys_string = get_sys_string( table_name );

	printf( "<BR><p>%s\n", sys_string );

	if ( toupper( *really_yn ) == 'Y' )
	{
		fflush( stdout );
		if ( system( sys_string ) ){};
		printf( "<BR><h3>Process complete.</h3>\n" );
	}
	else
	{
		printf( "<BR><h3>Only a test.</h3>\n" );
	}

	document_close();
	exit( 0 );
} /* main() */


char *get_sys_string( char *table_name )
{
	char buffer[ 1024 ];

	sprintf( buffer,
		 "echo \"drop table %s;\"		|"
		 "sql.e 2>&1 				|"
		 "html_paragraph_wrapper",
		 table_name );

	return strdup( buffer );
} /* get_sys_string() */

