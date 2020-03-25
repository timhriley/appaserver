/* --------------------------------------------------- 	*/
/* src_appaserver/create_application.c		       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "folder.h"
#include "appaserver_parameter_file.h"
#include "create_clone_filename.h"
#include "environ.h"
#include "session.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *current_application;
	char *login_name;
	char *session;
	char *destination_application;
	char *folder_name;
	char really_yn;
	char nocontent_type_yn;
	char build_shell_script_yn;
	char system_folders_yn;
	char sys_string[ 1024 ];
	DOCUMENT *document;
	LIST *folder_list;
	char tee_command_process[ 1024 ];
	char *database_management_system;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	CREATE_CLONE_FILENAME *create_clone_filename = {0};
	char *role_name;

	current_application = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		current_application );

	if ( argc != 11 )
	{
		fprintf(stderr,
"Usage: %s ignored build_shell_script_yn session login_name role destination_application system_folders_yn nocontent_type_yn really_yn database_management_system\n",
			argv[ 0 ] );
		exit( 1 );
	}

	build_shell_script_yn = *argv[ 2 ];
	session = argv[ 3 ];
	login_name = argv[ 4 ];
	role_name = argv[ 5 ];
	destination_application = argv[ 6 ];
	system_folders_yn = *argv[ 7 ];
	nocontent_type_yn = *argv[ 8 ];
	really_yn = *argv[ 9 ];
	database_management_system = argv[ 10 ];

	if ( build_shell_script_yn == 'y' ) really_yn = 'n';

	if ( !*destination_application
	||   strcmp(	destination_application,
			"destination_application" ) == 0 )
	{
		destination_application = current_application;
	}

	if ( !DIFFERENT_DESTINATION_APPLICATION_OK
	&&   timlib_strcmp(	current_application,
				TEMPLATE_APPLICATION ) != 0
	&&   timlib_strcmp(	current_application,
				destination_application ) != 0 )
	{
		printf(
"<h3>Error: For security, you must run this from the template application.</h3>\n" );
		document_close();
		exit( 1 );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !session_access(	current_application,
				session,
				login_name ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: no session=%s for user=%s found.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 session,
			 login_name );
		exit( 1 ); 
	}

	if ( really_yn != 'y' && build_shell_script_yn == 'y' )
	{
		create_clone_filename =
			create_clone_filename_new(
				destination_application,
				appaserver_parameter_file->
					appaserver_data_directory );

		if ( !zap_file(
			create_clone_filename_get_create_application_filename(
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				system_folders_yn ) ) )
		{
			document_quick_output_body(
				create_clone_filename->application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

			printf( "<p>ERROR: cannot open file for write: %s\n",
			create_clone_filename_get_create_application_filename(
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				system_folders_yn ) );
			document_close();
			exit( 1 );
		}
	}

	document = document_new( "", current_application );

	if ( nocontent_type_yn != 'y' )
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
			application_get_relative_source_directory(
				current_application ),
			0 /* not with_dynarch_menu */ );

	document_output_body( 	current_application,
				document->onload_control_string );

	sprintf( sys_string,
		 "application_folder_list.sh %s %c 2>>%s",
		 current_application,
		 system_folders_yn,
		 appaserver_error_get_filename( current_application ) );

	folder_list = pipe2list( sys_string );

	/* Exclude the application folder */
	/* ------------------------------ */
	list_remove_string( folder_list, "application" );

	if ( !list_rewind( folder_list ) )
	{
		printf( "<p>ERROR: no folders exist to create.\n" );
		document_close();
		exit( 0 );
	}

	do {
		folder_name = list_get( folder_list );

		if ( create_clone_filename )
		{
			sprintf( tee_command_process,
		 	"%s",
		create_clone_filename_get_create_table_tee_process_string(
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				folder_name ) );
		}
		else
		{
			strcpy( tee_command_process, "html_paragraph_wrapper" );
		}

		sprintf( sys_string, 
"create_table '%s' '%c' %s %s '%s' '%s' '%s' '%c' '%s' nohtml	|"
			 "%s					 ",
			 current_application,
			 build_shell_script_yn,
			 session,
			 login_name,
			 role_name,
			 destination_application,
			 folder_name,
			 really_yn,
			 database_management_system,
			 tee_command_process );

		fflush( stdout );
		if ( system( sys_string ) ){};

		if ( create_clone_filename )
		{
			list_append_pointer(
					create_clone_filename->folder_name_list,
					folder_name );
		}

	} while( list_next( folder_list ) );

	if ( create_clone_filename )
	{
		char *sys_string;
		char *application_filename;

		sys_string =
		create_clone_filename_get_create_application_combine_sys_string(
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				create_clone_filename->folder_name_list,
				system_folders_yn );

		if ( system( sys_string ) ){};

		sys_string =
		create_clone_filename_get_create_application_remove_sys_string(
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				create_clone_filename->folder_name_list );

		if ( system( sys_string ) ){};

		application_filename =
			create_clone_filename_get_create_application_filename(
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				system_folders_yn );

		sprintf( sys_string,
			 "chmod +x,g+w %s 2>/dev/null",
			 application_filename );

		if ( system( sys_string ) ){};

		printf( "<p>Created %s\n", application_filename );
	}

	if ( really_yn == 'y' )
		printf( "<h3>Process complete.</h3>\n" );

	document_close();

	return 0;

} /* main() */

