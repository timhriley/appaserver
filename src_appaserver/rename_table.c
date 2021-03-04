/* --------------------------------------------------- 	*/
/* rename_table.c				       	*/
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
#include "piece.h"
#include "document.h"
#include "folder.h"
#include "attribute.h"
#include "boolean.h"
#include "application.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */
#define RENAME_TABLE_FILENAME_TEMPLATE		 "%s/rename_table.sh"

/* Prototypes */
/* ---------- */
void make_executable(		char *rename_table_filename );

char *create_shell_script(	char *sys_string,
				char *appaserver_data_directory );

char *get_sys_string(		char *old_table_name,
				char *new_table_name,
				boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *old_folder_name;
	char *new_folder_name;
	char parsed_new_folder_name[ 128 ];
	char old_table_name[ 128 ];
	char new_table_name[ 128 ];
	char *sys_string;
	boolean execute;
	char *output_filename;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	/* exit( 1 ) upon failure */
	/* ---------------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s ignored ignored ignored ignored old_folder new_folder execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	old_folder_name = argv[ 5 ];
	new_folder_name = argv[ 6 ];
	execute = ( *argv[ 7 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	piece_last(	parsed_new_folder_name, 
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			new_folder_name );

	strcpy( old_table_name, 
		get_table_name( application_name, old_folder_name ) );

	strcpy( new_table_name, 
		get_table_name( application_name, parsed_new_folder_name ) );

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

	sys_string = get_sys_string( old_table_name, new_table_name, execute );

	printf( "<BR><p>%s\n", sys_string );

	if ( execute )
	{
		fflush( stdout );
		system( sys_string );
		printf( "<BR><h3>Process complete</h3>\n" );
	}
	else
	{
		output_filename =
			create_shell_script(
				sys_string,
				appaserver_parameter_file->
					appaserver_data_directory );

		printf( "<BR><p>Created: %s\n", output_filename );
	}

	document_close();

	return 0;

} /* main() */


char *get_sys_string(	char *old_table_name,
			char *new_table_name,
			boolean execute )
{
	char buffer[ 1024 ];

	sprintf( buffer,
		 "echo \"alter table %s rename as %s;\" | sql.e",
		 old_table_name,
		 new_table_name );

	if ( execute )
	{
		sprintf( buffer + strlen( buffer ),
			 " 2>&1 | html_paragraph_wrapper" );
	}

	return strdup( buffer );

} /* get_sys_string() */

char *create_shell_script(	char *sys_string,
				char *appaserver_data_directory )
{
	char *shell_snippet;
	char rename_table_filename[ 128 ];
	FILE *output_file;

	shell_snippet = environ_get_shell_snippet();

	sprintf(rename_table_filename,
	 	RENAME_TABLE_FILENAME_TEMPLATE,
	 	appaserver_data_directory );

	if ( ! ( output_file = fopen( rename_table_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 rename_table_filename );

		exit( 1 );
	}

	fprintf( output_file,
		 "%s\n",
		 shell_snippet );

	fprintf( output_file,
		 "%s\n",
		 sys_string );

	fprintf( output_file, "\nexit 0\n" );

	fclose( output_file );

	make_executable( rename_table_filename );

	return strdup( rename_table_filename );

} /* create_shell_script() */

void make_executable( char *rename_table_filename )
{
	char sys_string[ 512 ];

	sprintf(sys_string,
		"chmod -x,ug+x %s 2>/dev/null",
		rename_table_filename );

	system( sys_string );
}

