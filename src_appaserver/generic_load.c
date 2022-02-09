/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/generic_load.c 	*/
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
#include "environ.h"
#include "timlib.h"
#include "document.h"
#include "attribute.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "folder.h"
#include "session.h"
#include "generic_load.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *role_name;
	char *process_name;
	SESSION *session;
	GENERIC_LOAD *generic_load;

	application_name = environ_exit_application_name( argv[ 0 ] );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s session role process\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];

	appaserver_parameter = appaserver_parameter_new();

	session =
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			login_name,
			session_login_name( session_key ),
			session_key,
			process_name,
			role_name );

	generic_load =
		generic_load_new(
			role_name );

	if ( !generic_load )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: generic_load_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	format_initial_capital( process_title_initial_capital,
				process_name );

	document = document_new(process_title_initial_capital,
				application_name );

	if ( !appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		document_set_output_content_type( document );
	}

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

	sprintf( title,
		 "%s %s",
		 application_title_string( application_name ),
		 process_title_initial_capital );

	printf( "<h1>%s</h1>\n", title );

	folder_name_list =
		folder_get_insert_folder_name_list(
				application_name,
				login_name,
				role_name );

	printf( "<table border>\n" );
	if ( folder_name_list && list_rewind( folder_name_list ) )
	{
		do {
			folder_name = list_get_pointer( folder_name_list );
			printf( "<tr><td>" );
			printf(
"<a href=\"%s/post_generic_load?%s+%s+%s+%s+one\" target=%s>%s</a></td>\n",
				appaserver_parameter_file_get_cgi_directory(),
				application_name,
				session,
				role_name,
				folder_name,
				PROMPT_FRAME,
				format_initial_capital(
					buffer,
					folder_name ) );
		} while( list_next( folder_name_list ) );
	}
	printf( "</table>\n" );

	document_close();

	return 0;

} /* main() */

