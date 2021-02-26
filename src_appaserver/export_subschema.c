/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/export_subschema.c  	*/
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
#include "environ.h"
#include "timlib.h"
#include "query.h"
#include "document.h"
#include "folder.h"
#include "application.h"
#include "list.h"
#include "appaserver_parameter_file.h"
#include "create_clone_filename.h"
#include "dictionary2file.h"
#include "dictionary_appaserver.h"
#include "session.h"

/* Constants */
/* --------- */
#define SHELL_HERE_LABEL	"all_done"

#define EXPORT_SUBSCHEMA_FILENAME_TEMPLATE	"%s/export_subschema.sh"

/* Prototypes */
/* ---------- */
void clone_table_folder_row_level_restrictions(
				char *export_subschema_filename,
				char *application_name,
				char *appaserver_data_directory,
				char *folder_name,
				char *session,
				char *login_name,
				char *role_name );

void clone_table_process(
				char *export_subschema_filename,
				char *application_name,
				char *appaserver_data_directory,
				char *folder_name,
				char *session,
				char *login_name,
				char *role_name );

LIST *get_process_name_list(	char *application_name,
				char *folder_name );

LIST *get_role_name_list(	char *application_name,
				char *folder_name );

char *get_subschema(		char *application_name,
				char *folder_name );

LIST *get_javascript_filename_list(
				char *application_name,
				char *folder_name );

void output_shell_script_header(char *export_subschema_filename );

void output_shell_script_footer(
			char *export_subschema_filename );

void append_export_subschema_file(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name );

void clone_table_folder(char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_relation(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_attribute(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_folder_attribute(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_javascript_folders(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_javascript_files(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_row_security_role_update(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_role_folder(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_role(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_subschema(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_role_operation(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name );

int main( int argc, char **argv )
{
	char *application_name;
	DOCUMENT *document;
	LIST *folder_name_list;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *parameter_dictionary_string;
	DICTIONARY *parameter_dictionary;
	char *folder_name;
	char export_subschema_filename[ 512 ];
	char *session;
	char *login_name;
	char *role_name;
	DICTIONARY_APPASERVER *dictionary_appaserver;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s ignored session login_name role not_used_filler parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	/* argv[ 1 ] ignored. */
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	/* argv[ 5 ] ignored. */
	parameter_dictionary_string = argv[ 6 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	parameter_dictionary = 
		dictionary_string2dictionary(
			parameter_dictionary_string );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				parameter_dictionary,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	parameter_dictionary =
		dictionary_appaserver->
			working_post_dictionary;

	folder_name_list = 
		dictionary_get_index_list(
			parameter_dictionary,
			"folder" );

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

	sprintf( export_subschema_filename,
		 EXPORT_SUBSCHEMA_FILENAME_TEMPLATE,
		 appaserver_parameter_file->appaserver_data_directory );

	if ( !list_length( folder_name_list ) )
	{
		printf( "<P>No folders selected.\n" );
		document_close();
		exit( 0 );
	}

	output_shell_script_header( export_subschema_filename );

	list_rewind( folder_name_list );

	do {
		folder_name = list_get_pointer( folder_name_list );

		clone_table_folder(	export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_relation(	export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_attribute(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_folder_attribute(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_role_folder(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_row_security_role_update(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_javascript_folders(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_javascript_files(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_process(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_role(	export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_subschema(	export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_role_operation(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

		clone_table_folder_row_level_restrictions(
					export_subschema_filename,
					application_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					folder_name,
					session,
					login_name,
					role_name );

	} while( list_next( folder_name_list ) );

	output_shell_script_footer( export_subschema_filename );

	printf( "<p>Created %s\n", export_subschema_filename );

	document_close();

	exit( 0 );

} /* main() */

void clone_table_folder(char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = folder_name;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"folder" /* folder_name */,
			"folder" /* attribute_name */,
			where_data,
			where_data,
			"y" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"folder" );

} /* clone_table_folder() */

void clone_table_relation(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = folder_name;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"relation" /* folder_name */,
			"folder" /* attribute_name */,
			where_data,
			where_data,
			"y" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"relation" );

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"relation" /* folder_name */,
			"related_folder" /* attribute_name */,
			where_data,
			where_data,
			"n" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"relation" );

} /* clone_table_relation() */

void clone_table_attribute(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *attribute_name;
	LIST *attribute_name_list;

	attribute_name_list = 
		appaserver_library_get_attribute_name_list(
			application_name, folder_name );

	if ( !list_rewind( attribute_name_list ) )
	{
		printf( "<p>ERROR: cannot find attributes for folder = %s\n",
			folder_name );
		document_close();
		exit( 0 );
	}
	
	do {
		attribute_name = list_get_pointer( attribute_name_list );

		where_data = attribute_name;
	
		sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
				application_name,
				session,
				login_name,
				role_name,
				application_name,
				"attribute" /* folder_name */,
				"attribute" /* attribute_name */,
				where_data,
				where_data,
				"y" /* delete_yn */,
				"n" /* really_yn */,
				"y" /* output2file_yn */ );
	
		if ( system( sys_string ) ){};
	
		append_export_subschema_file(	export_subschema_filename,
						application_name,
						appaserver_data_directory,
						"attribute" );
	} while( list_next( attribute_name_list ) );

} /* clone_table_attribute() */

void clone_table_folder_attribute(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = folder_name;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"folder_attribute" /* folder_name */,
			"folder" /* attribute_name */,
			where_data,
			where_data,
			"y" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"folder_attribute" );

} /* clone_table_folder_attribute() */

void clone_table_row_security_role_update(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = folder_name;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"row_security_role_update" /* folder_name */,
			"folder" /* attribute_name */,
			where_data,
			where_data,
			"n" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"row_security_role_update" );

} /* clone_table_row_security_role_update() */

void clone_table_role_folder(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = folder_name;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"role_folder" /* folder_name */,
			"folder" /* attribute_name */,
			where_data,
			where_data,
			"n" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"role_folder" );

} /* clone_table_role_folder() */

void clone_table_folder_row_level_restrictions(
				char *export_subschema_filename,
				char *application_name,
				char *appaserver_data_directory,
				char *folder_name,
				char *session,
				char *login_name,
				char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = folder_name;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"folder_row_level_restrictions" /* folder_name */,
			"folder" /* attribute_name */,
			where_data,
			where_data,
			"n" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"folder_row_level_restrictions" );

} /* clone_table_folder_row_level_restrictions() */

void clone_table_role(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *application_role_name;
	LIST *role_name_list;

	role_name_list = 
		get_role_name_list(
			application_name,
			folder_name );

	if ( !list_rewind( role_name_list ) ) return;
	
	do {
		application_role_name =
			list_get_pointer(
				role_name_list );

		where_data = application_role_name;
	
		sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
				application_name,
				session,
				login_name,
				role_name,
				application_name,
				"role" /* folder_name */,
				"role" /* attribute_name */,
				where_data,
				where_data,
				"n" /* delete_yn */,
				"n" /* really_yn */,
				"y" /* output2file_yn */ );

		if ( system( sys_string ) ){};
	
		append_export_subschema_file(	export_subschema_filename,
						application_name,
						appaserver_data_directory,
						"role" );

	} while( list_next( role_name_list ) );

} /* clone_table_role() */

void clone_table_process(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *process_name;
	LIST *process_name_list;

	process_name_list = 
		get_process_name_list(
			application_name, folder_name );

	if ( !list_rewind( process_name_list ) ) return;
	
	do {
		process_name =
			list_get_pointer(
				process_name_list );

		where_data = process_name;
	
		sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
				application_name,
				session,
				login_name,
				role_name,
				application_name,
				"process" /* folder_name */,
				"process" /* attribute_name */,
				where_data,
				where_data,
				"y" /* delete_yn */,
				"n" /* really_yn */,
				"y" /* output2file_yn */ );

		if ( system( sys_string ) ){};
	
		append_export_subschema_file(	export_subschema_filename,
						application_name,
						appaserver_data_directory,
						"process" );

	} while( list_next( process_name_list ) );

} /* clone_table_process() */

void clone_table_javascript_files(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *javascript_filename;
	LIST *javascript_filename_list;

	javascript_filename_list = 
		get_javascript_filename_list(
			application_name, folder_name );

	if ( !list_rewind( javascript_filename_list ) ) return;
	
	do {
		javascript_filename =
			list_get_pointer(
				javascript_filename_list );

		where_data = javascript_filename;
	
		sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
				application_name,
				session,
				login_name,
				role_name,
				application_name,
				"javascript_files" /* folder_name */,
				"javascript_filename" /* attribute_name */,
				where_data,
				where_data,
				"y" /* delete_yn */,
				"n" /* really_yn */,
				"y" /* output2file_yn */ );
	
		if ( system( sys_string ) ){};
	
		append_export_subschema_file(	export_subschema_filename,
						application_name,
						appaserver_data_directory,
						"javascript_files" );
	} while( list_next( javascript_filename_list ) );

} /* clone_table_javascript_files() */


void clone_table_javascript_folders(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = folder_name;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"javascript_folders" /* folder_name */,
			"folder" /* attribute_name */,
			where_data,
			where_data,
			"y" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"javascript_folders" );

} /* clone_table_javascript_folders() */

void clone_table_subschema(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *subschema;

	if ( ! ( subschema = get_subschema( application_name, folder_name ) ) )
	{
		return;
	}

	where_data = subschema;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"subschemas" /* folder_name */,
			"subschema" /* attribute_name */,
			where_data,
			where_data,
			"n" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"subschemas" );

} /* clone_table_subschema() */

void clone_table_role_operation(
			char *export_subschema_filename,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = folder_name;

	sprintf(sys_string,
"clone_folder %s n %s %s \"%s\" %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"role_operation" /* folder_name */,
			"folder" /* attribute_name */,
			where_data,
			where_data,
			"n" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	append_export_subschema_file(	export_subschema_filename,
					application_name,
					appaserver_data_directory,
					"role_operation" );

} /* clone_table_role_operation() */

void append_export_subschema_file(	char *export_subschema_filename,
					char *application_name,
					char *appaserver_data_directory,
					char *folder_name )
{
	CREATE_CLONE_FILENAME *create_clone_filename;
	char clone_folder_filename[ 512 ];
	char sys_string[ 1024 ];

	create_clone_filename = create_clone_filename_new(
					application_name,
					appaserver_data_directory );

	create_clone_filename_get_clone_folder_compressed_filename(
				clone_folder_filename,
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				folder_name );

	sprintf(sys_string,
		"zcat %s						|"
		"sed 's/^insert into %s/insert into $%s/'		|"
		"sed 's/^delete from %s/delete from $%s/'		|"
		"sed 's/^insert into %s_%s/insert into $%s/'		|"
		"sed 's/^delete from %s_%s/delete from $%s/'		|"
		"cat >> %s",
		clone_folder_filename,
		folder_name,
		folder_name,
		folder_name,
		folder_name,
		application_name,
		folder_name,
		folder_name,
		application_name,
		folder_name,
		folder_name,
		export_subschema_filename );

	if ( system( sys_string ) ){};

	sprintf(sys_string,
		"rm %s",
		clone_folder_filename );

	if ( system( sys_string ) ){};

} /* append_export_subschema_file() */

void output_shell_script_footer( char *export_subschema_filename )
{
	char sys_string[ 512 ];
	FILE *export_subschema_file;

	export_subschema_file = fopen( export_subschema_filename, "a" );

	fprintf(export_subschema_file,
		"%s\n) | sql.e 2>&1 | grep -iv duplicate\n\nexit 0\n",
		SHELL_HERE_LABEL );

	fclose( export_subschema_file );

	sprintf( sys_string,
		 "chmod +x,g+w %s 2>/dev/null",
		 export_subschema_filename );
	if ( system( sys_string ) ){};

} /* output_shell_script_footer() */

void output_shell_script_header( char *export_subschema_filename )
{
	FILE *output_file;

	output_file = fopen( export_subschema_filename, "w" );

	if ( !output_file )
	{
		printf( "<P>ERROR opening %s for write.\n",
			export_subschema_filename );
		document_close();
		exit( 0 );
	}

	fprintf( output_file,
	"#!/bin/sh\n" );

	fprintf( output_file,
"if [ \"$APPASERVER_DATABASE\" != \"\" ]\n"
"then\n"
"	application=$APPASERVER_DATABASE\n"
"elif [ \"$DATABASE\" != \"\" ]\n"
"then\n"
"	application=$DATABASE\n"
"fi\n"
"\n"
"if [ \"$application\" = \"\" ]\n"
"then\n"
"	echo \"Error in `basename.e $0 n`: you must first:\" 1>&2\n"
"	echo \"$ . set_database\" 1>&2\n"
"	exit 1\n"
"fi\n" );

	/* These were once wrapped around `get_table_name ...` */
	/* --------------------------------------------------- */
	fprintf( output_file,
	"folder=\"folder\"\n" );

	fprintf( output_file,
	"relation=\"relation\"\n" );

	fprintf( output_file,
	"attribute=\"attribute\"\n" );

	fprintf( output_file,
	"folder_attribute=\"folder_attribute\"\n" );

	fprintf( output_file,
	"role_folder=\"role_folder\"\n" );

	fprintf( output_file,
	"row_security_role_update=\"row_security_role_update\"\n" );

	fprintf( output_file,
	"folder_row_level_restrictions=\"folder_row_level_restrictions\"\n" );

	fprintf( output_file,
	"subschemas=\"subschemas\"\n" );

	fprintf( output_file,
	"role_operation=\"role_operation\"\n" );

	fprintf( output_file,
	"javascript_folders=\"javascript_folders\"\n" );

	fprintf( output_file,
	"javascript_files=\"javascript_files\"\n" );

	fprintf( output_file,
	"process=\"process\"\n" );

	fprintf( output_file,
	"role=\"role\"\n" );

	fprintf(output_file,
		"\n(\ncat << %s\n",
		SHELL_HERE_LABEL );

	fclose( output_file );

} /* output_shell_script_header() */

LIST *get_role_name_list(	char *application_name,
				char *folder_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];
	char *select;

	select = "role";

	sprintf(where_clause,
		"folder = '%s'",
		folder_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=role_folder		 "
		"			where=\"%s\"			|"
		"sort -u						 ",
		application_name,
		select,
		where_clause );

	return pipe2list( sys_string );

} /* get_role_name_list() */

LIST *get_process_name_list( char *application_name, char *folder_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];
	char *select;
	char *results;
	char process_name[ 128 ];
	LIST *process_list = {0};

	select = "populate_drop_down_process,post_change_process";

	sprintf(where_clause,
		"folder = '%s'",
		folder_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=folder			"
		"			where=\"%s\"			",
		application_name,
		select,
		where_clause );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot fetch from folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	piece( process_name, FOLDER_DATA_DELIMITER, results, 0 );
	if ( *process_name )
	{
		process_list = list_new();
		list_append_pointer( process_list, strdup( process_name ) );
	}

	piece( process_name, FOLDER_DATA_DELIMITER, results, 1 );
	if ( *process_name )
	{
		if ( !process_list ) process_list = list_new();
		list_append_pointer( process_list, strdup( process_name ) );
	}

	return process_list;

} /* get_process_name_list() */

LIST *get_javascript_filename_list( char *application_name, char *folder_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];

	sprintf(where_clause,
		"folder = '%s'",
		folder_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=javascript_filename	"
		"			folder=javascript_folders	"
		"			where=\"%s\"			",
		application_name,
		where_clause );

	return pipe2list( sys_string );

} /* get_javascript_filename_list() */

char *get_subschema(		char *application_name,
				char *folder_name )
{
	char sys_string[ 1024 ];
	char *results;
	char where[ 256 ];

	sprintf( where, "folder = '%s'", folder_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=subschema	"
		 "			folder=folder		"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results )
		return (char *)0;
	else
		return results;
} /* get_subschema() */

