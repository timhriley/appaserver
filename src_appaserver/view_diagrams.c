/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/view_diagrams.c	*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver.h"
#include "folder.h"
#include "form.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "application.h"
#include "document.h"
#include "session.h"
#include "environ.h"
#include "piece.h"
#include "basename.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"view_diagrams"
#define DEFAULT_HEADING		"View Diagrams"
#define SYS_STRING		\
"ls -1 *.xls *.jpg *.doc *.pdf *.png *.txt *.ppt *.xml 2>/dev/null | sort"

/* Prototypes */
/* ---------- */
void view_diagrams_generic(
			char *application_name,
			char *appaserver_mount_point );

void view_diagrams_custom(
			char *application_name,
			boolean all_documents,
			char *login_name,
			char *relative_directory,
			char *document_root_directory,
			char *appaserver_mount_point );

int main( int argc, char **argv )
{
	char *application_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *login_name = "";
	boolean all_documents = 0;
	char *relative_directory = {0};
	char *process_name = {0};

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 2 )
	{
		fprintf( stderr,
	"Usage: %s ignored [all_documents_yn] [login_name] [unused] [relative_directory] [process_name]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	if ( argc >= 3 ) all_documents = (*argv[ 2 ] == 'y');
	if ( argc >= 4 ) login_name = argv[ 3 ];
	/* role_name = argv[ 4 ]; */
	if ( argc >= 6 ) relative_directory = argv[ 5 ];
	if ( argc >= 7 ) process_name = argv[ 6 ];
 
	appaserver_parameter_file = appaserver_parameter_file_new();
	add_pwd_to_path();

	document = document_new( "", application_name );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			"diagram_list.css",
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	if ( process_name )
	{
		char buffer[ 128 ];

		printf(	"<h1>%s</h1>\n",
			format_initial_capital(
				buffer,
				process_name ) );
	}
	else
	{
		printf( "<h1>%s</h1>\n", DEFAULT_HEADING );
	}

	if ( argc == 2 )
	{
		view_diagrams_generic(
			application_name,
			appaserver_parameter_file->appaserver_mount_point );
	}
	else
	{
		view_diagrams_custom(
			application_name,
			all_documents,
			login_name,
			relative_directory,
			appaserver_parameter_file->document_root,
			appaserver_parameter_file->appaserver_mount_point );
	}

	document_close();

	if ( process_name )
	{
		process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );
	}
	else
	{
		process_increment_execution_count(
			application_name,
			PROCESS_NAME,
			appaserver_parameter_file_get_dbms() );
	}

	exit( 0 );

} /* main() */

void view_diagrams_custom(
			char *application_name,
			boolean all_documents,
			char *login_name,
			char *relative_directory,
			char *document_root_directory,
			char *appaserver_mount_point )
{
	char current_directory[ 256 ];
	LIST *file_list;
	char *file_to_process;
	char base_name[ 128 ];
	char buffer[ 512 ];

	if ( relative_directory )
	{
		sprintf(current_directory,
			"%s/%s",
		 	document_root_directory,
			relative_directory );
	}
	else
	{
		sprintf(current_directory,
		 	"%s/%s", 
		 	appaserver_mount_point,
		 	application_first_relative_source_directory(
				application_name ) );
	}

	if ( chdir( current_directory ) == -1 )
	{
		fprintf( stderr, 
			 "Cannot change directory to %s\n",
			 current_directory );
		exit( 1 );
	}

	if ( all_documents )
		file_list = pipe2list( "ls -1 *" );
	else
		file_list = pipe2list( SYS_STRING );

	if ( !list_rewind( file_list ) )
	{
		printf( "<p>ERROR: no documents found\n" );
		document_close();
		exit( 1 );
	}

	printf( "<ul>\n" );

	do {
		file_to_process = list_get_string( file_list );

		strcpy( base_name,
			basename_get_base_name(
				file_to_process,
				1 /* strip_extension */ ) );

		if ( relative_directory )
		{
			printf(
"<li><a class=diagram target=\"%s\" href=\"/%s/%s\">%s</a>\n",
				base_name,
				relative_directory,
		 		file_to_process,
				format_initial_capital(
					buffer,
					file_to_process ) );
		}
		else
		{
			printf(
"<li><a class=diagram target=\"%s\" href=\"/appaserver/%s/%s\">%s</a>\n",
				base_name,
		 		application_first_relative_source_directory(
					application_name ),
		 		file_to_process,
				format_initial_capital(
					buffer,
					file_to_process ) );
		}

	} while( list_next( file_list ) );

	printf( "</ul>\n" );

	if ( *login_name ) form_output_back_to_top_button();

} /* view_diagrams_custom() */

void view_diagrams_generic(
			char *application_name,
			char *appaserver_mount_point )
{
	char current_directory[ 256 ];
	LIST *file_list;
	char *file_to_process;
	char base_name[ 128 ];
	char buffer[ 512 ];
	char *relative_source_directory_list_string;
	char relative_source_directory[ 128 ];
	int p;

	relative_source_directory_list_string =
		application_relative_source_directory(
			application_name );

	printf( "<ul>\n" );

	for(	p = 0;
		piece(	relative_source_directory,
			':',
			relative_source_directory_list_string,
			p );
		p++ )
	{
		sprintf(current_directory,
		 	"%s/%s", 
		 	appaserver_mount_point,
			relative_source_directory );

		if ( chdir( current_directory ) == -1 )
		{
			fprintf(stderr,
			 	"Cannot change directory to %s\n",
			 	current_directory );
			continue;
		}

		file_list = pipe2list( SYS_STRING );

		if ( !list_rewind( file_list ) ) continue;

		do {
			file_to_process = list_get_string( file_list );

			strcpy( base_name,
				basename_get_base_name(
					file_to_process,
					1 /* strip_extension */ ) );

			printf(
"<li><a class=diagram target=\"%s\" href=\"/appaserver/%s/%s\">%s</a>\n",
				base_name,
				relative_source_directory,
		 		file_to_process,
				format_initial_capital(
					buffer,
					file_to_process ) );

		} while( list_next( file_list ) );
	}

	printf( "</ul>\n" );

} /* view_diagrams_generic() */
