/* --------------------------------------------------- 	*/
/* src_appaserver/export_application.c		       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* Note: this creates clone_application .		*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "document.h"
#include "folder.h"
#include "application.h"
#include "list.h"
#include "appaserver_parameter_file.h"
#include "create_clone_filename.h"
#include "environ.h"
#include "appaserver_error.h"
#include "session.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
char *get_clone_application_remove_sql_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list );

char *get_clone_application_remove_shell_script_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list );

char *get_clone_application_combine_sql_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list,
			char system_folders_yn );

char *get_clone_application_combine_shell_script_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list,
			char system_folders_yn );

LIST *subtract_exclude_application_export(
			char *application_name,
			LIST *folder_name_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *folder_name;
	char *delete_yn;
	char sys_string[ 1024 ];
	char *sys_string_ptr;
	char system_folders_yn;
	DOCUMENT *document;
	LIST *folder_name_list;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	CREATE_CLONE_FILENAME *create_clone_filename = {0};
	char *database_management_system;
	char *export_output;
	char nocontent_type_yn;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 12 )
	{
		fprintf(stderr,
"Usage: %s ignored session login_name ignored ignored delete_yn system_folders_yn filler no_content_type_yn database_management_system export_output\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session = argv[ 2 ];
	login_name = argv[ 3 ];
	/* ignored = argv[ 4 ]; */
	/* ignored = argv[ 5 ]; */
	delete_yn = argv[ 6 ];
	system_folders_yn = *argv[ 7 ];
	/* really_yn = argv[ 8 ]; */
	nocontent_type_yn = *argv[ 9 ];
	database_management_system = argv[ 10 ];
	export_output = argv[ 11 ];

	if ( strcmp( export_output, "spreadsheet" ) == 0 )
	{
		sprintf( sys_string,
			 "export_application_spreadsheet '%s' '%c'",
			 application_name,
			 system_folders_yn );

		if ( system( sys_string ) ){};
		exit( 0 );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( "", application_name );

	if ( nocontent_type_yn != 'y' )
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

	create_clone_filename =
		create_clone_filename_new(
			application_name,
			appaserver_parameter_file->
				appaserver_data_directory );

	if ( !zap_file(
		create_clone_filename_get_clone_application_compressed_filename(
			&create_clone_filename->directory_name,
			create_clone_filename->application_name,
			create_clone_filename->appaserver_data_directory,
			system_folders_yn,
			export_output ) ) )
	{
		printf( "<p>ERROR in %s/%s(): cannot write to %s\n",
		   	__FILE__,
		   	__FUNCTION__,
	create_clone_filename_get_clone_application_compressed_filename(
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				system_folders_yn,
				export_output ) );
		document_close();
		exit( 1 );
	}

	/* Do the work */
	/* ----------- */
	sprintf( sys_string,
		 "application_folder_list.sh %s %c", 
		 application_name, system_folders_yn );

	folder_name_list = pipe2list( sys_string );

	/* Subtract those with the exclude flag set. */
	/* ----------------------------------------- */
	folder_name_list =
		subtract_exclude_application_export(
			application_name,
			folder_name_list );

	/* Subtract the application folder. */
	/* -------------------------------- */
	list_subtract_string( folder_name_list, "application" );

	if ( !list_rewind( folder_name_list ) )
	{
		printf(
		"<h3>Warning: there is nothing available to output.</h3>\n" );
		document_close();
		exit( 0 );
	}

	do {
		folder_name = list_get( folder_name_list );

		if ( strcmp( folder_name, "appaserver_sessions" ) == 0 )
			continue;

		printf( "<p>%s:\n", folder_name );

		sprintf( sys_string,
"clone_folder %s y %s %s filler %s %s attribute old_data new_data nohtml %s %s %s %s %s",
			 application_name,
			 session,
			 login_name,
			 application_name,
			 folder_name,
			 delete_yn,
			 "n" /* really_yn */,
			 database_management_system,
			 "y" /* output2file_yn */,
			 export_output );

		fflush( stdout );
		if ( system( sys_string ) ){};

		list_append_pointer(
			create_clone_filename->folder_name_list,
			folder_name );

	} while( list_next( folder_name_list ) );

	if ( timlib_strcmp( export_output, "shell_script" ) == 0 )
	{
		sys_string_ptr =
		get_clone_application_combine_shell_script_sys_string(
			&create_clone_filename->directory_name,
			create_clone_filename->application_name,
			create_clone_filename->appaserver_data_directory,
			create_clone_filename->folder_name_list,
			system_folders_yn );

		if ( system( sys_string_ptr ) ){};

		sys_string_ptr =
		get_clone_application_remove_sql_sys_string(
			&create_clone_filename->directory_name,
			create_clone_filename->application_name,
			create_clone_filename->appaserver_data_directory,
			create_clone_filename->folder_name_list );

		if ( system( sys_string_ptr ) ){};

		sys_string_ptr =
		get_clone_application_remove_shell_script_sys_string(
			&create_clone_filename->directory_name,
			create_clone_filename->application_name,
			create_clone_filename->appaserver_data_directory,
			create_clone_filename->folder_name_list );

		if ( system( sys_string_ptr ) ){};
	}
	else
	{
		sys_string_ptr =
		get_clone_application_combine_sql_sys_string(
			&create_clone_filename->directory_name,
			create_clone_filename->application_name,
			create_clone_filename->appaserver_data_directory,
			create_clone_filename->folder_name_list,
			system_folders_yn );

		if ( system( sys_string_ptr ) ){};

		sys_string_ptr =
		get_clone_application_remove_sql_sys_string(
			&create_clone_filename->directory_name,
			create_clone_filename->application_name,
			create_clone_filename->appaserver_data_directory,
			create_clone_filename->folder_name_list );

		if ( system( sys_string_ptr ) ){};

	} /* if output ...sql.gz */

	printf( "<p>Created %s\n",
		create_clone_filename_get_clone_application_compressed_filename(
			&create_clone_filename->directory_name,
			create_clone_filename->application_name,
			create_clone_filename->appaserver_data_directory,
			system_folders_yn,
			export_output ) );

	document_close();

	exit( 0 );

} /* main() */

LIST *subtract_exclude_application_export(
			char *application_name,
			LIST *folder_name_list )
{
	char sys_string[ 1024 ];
	char *where;

	where = "exclude_application_export_yn = 'y'";

	sprintf( sys_string,
		 "get_folder_data	folder=folder			"
		 "			select=folder			"
		 "			application=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return list_subtract_list( folder_name_list, pipe2list( sys_string ) );
} /* subtract_exclude_application_export() */

char *get_clone_application_combine_shell_script_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list,
			char system_folders_yn )
{
	char buffer[ 65536 ];
	char *folder_name;
	char clone_folder_filename[ 1024 ];
	char *buffer_pointer = buffer;
	int first_time = 1;

	*directory_name =
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	if ( !list_rewind( folder_name_list ) ) return (char *)0;

	do {
		folder_name = list_get_pointer( folder_name_list );

		if ( first_time )
		{
			buffer_pointer += sprintf( buffer_pointer, "(cat " );
			first_time = 0;
		}
		else
		{
			buffer_pointer += sprintf( buffer_pointer, "&& cat " );

		}

		buffer_pointer += sprintf(
				buffer_pointer,
				" %s",
		create_clone_filename_get_shell_script_filename(
					clone_folder_filename,
					directory_name,
					application_name,
					appaserver_data_directory,
					folder_name ) );

		buffer_pointer += sprintf(
				buffer_pointer,
			" | sed 's/insert into %s_%s/insert into $table_name/'",
				application_name,
				folder_name );

		buffer_pointer += sprintf(
				buffer_pointer,
			" | sed 's/delete from %s_%s/delete from $table_name/'",
				application_name,
				folder_name );

		buffer_pointer += sprintf(
				buffer_pointer,
			" | sed 's/insert into %s/insert into $table_name/'",
				folder_name );

		buffer_pointer += sprintf(
				buffer_pointer,
			" | sed 's/delete from %s/delete from $table_name/'",
				folder_name );

	} while( list_next( folder_name_list ) );

	buffer_pointer += sprintf(
			buffer_pointer,
			") | sed 's/^exit 0$//' | gzip - > %s",
		create_clone_filename_get_clone_application_compressed_filename(
				directory_name,
				application_name,
				appaserver_data_directory,
				system_folders_yn,
				"shell_script" ) );

	return strdup( buffer );

} /* get_clone_application_combine_shell_script_sys_string() */

char *get_clone_application_combine_sql_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list,
			char system_folders_yn )
{
	char buffer[ 65536 ];
	char clone_folder_filename[ 1024 ];
	char *buffer_pointer = buffer;

	*directory_name =
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	buffer_pointer += sprintf( buffer_pointer, "zcat " );

	if ( !list_rewind( folder_name_list ) ) return (char *)0;

	do {
		buffer_pointer += sprintf(
			buffer_pointer,
			" %s",
		create_clone_filename_get_clone_folder_compressed_filename(
				clone_folder_filename,
				directory_name,
				application_name,
				appaserver_data_directory,
				list_get_string( folder_name_list ) ) );

	} while( list_next( folder_name_list ) );

	buffer_pointer += sprintf(
		buffer_pointer,
		" | gzip - > %s",
		create_clone_filename_get_clone_application_compressed_filename(
				directory_name,
				application_name,
				appaserver_data_directory,
				system_folders_yn,
				"gzip_file" ) );

	return strdup( buffer );
} /* get_clone_application_combine_sql_sys_string() */

char *get_clone_application_remove_sql_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list )
{
	char buffer[ 65536 ];
	char clone_folder_filename[ 1024 ];
	char *buffer_pointer = buffer;

	*directory_name =
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	buffer_pointer += sprintf( buffer_pointer, "rm -f " );

	if ( !list_rewind( folder_name_list ) ) return (char *)0;

	do {
		buffer_pointer += sprintf(
		buffer_pointer,
		" %s",
		create_clone_filename_get_clone_folder_compressed_filename(
					clone_folder_filename,
					directory_name,
					application_name,
					appaserver_data_directory,
					list_get_string( folder_name_list ) ) );

	} while( list_next( folder_name_list ) );

	return strdup( buffer );
} /* get_clone_application_remove_sql_sys_string() */

char *get_clone_application_remove_shell_script_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list )
{
	char buffer[ 65536 ];
	char clone_folder_filename[ 1024 ];
	char *buffer_pointer = buffer;

	*directory_name =
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	buffer_pointer += sprintf( buffer_pointer, "rm -f " );

	if ( !list_rewind( folder_name_list ) ) return (char *)0;

	do {
		buffer_pointer += sprintf(
		buffer_pointer,
		" %s",
		create_clone_filename_get_shell_script_filename(
					clone_folder_filename,
					directory_name,
					application_name,
					appaserver_data_directory,
					list_get_string( folder_name_list ) ) );

	} while( list_next( folder_name_list ) );

	return strdup( buffer );

} /* get_clone_application_remove_shell_script_sys_string() */

