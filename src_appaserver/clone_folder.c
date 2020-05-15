/* --------------------------------------------------- 	*/
/* src_appaserver/clone_folder.c		       	*/
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
#include "piece.h"
#include "document.h"
#include "folder.h"
#include "attribute.h"
#include "list_usage.h"
#include "list.h"
#include "application.h"
#include "create_clone_filename.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "session.h"

/* Constants */
/* --------- */
#define TEMPLATE_APPLICATION		"template"
#define SESSION_PROCESS_NAME		"clone_folder"
#define MIN_COUNT_FOR_QUICK		100000

/* Prototypes */
/* ---------- */
void go_output2shell_script_insert_statements(
			char *application_name,
			char *shell_script_filename,
			char *insert_statement_filename,
			char *folder_name );

void go_output2shell_script_header(
			FILE *output_file,
			char *application_name,
			char *folder_name,
			char dont_output_if_statement_yn );

boolean go_output2shell_script(
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *folder_name,
			char dont_output_if_statement_yn );

char *get_quick_yes_no(	char *application_name, char *folder_name );

char *get_sys_string(	char *destination_table_name,
			char *application_name,
			char *folder_name,
			char *attribute_name, 
			char *old_data,
			char *new_data,
			int attribute_piece,
			char delete_yn,
			char really_yn,
			CREATE_CLONE_FILENAME *clone_filename );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *destination_application;
	char *folder_name;
	char *attribute_name;
	char *old_data;
	char *new_data;
	char *destination_table_name;
	char delete_yn;
	char really_yn;
	char shell_script_dont_output_if_statement_yn;
	int with_html;
	char *sys_string;
	DOCUMENT *document = {0};
	int attribute_piece = 0;
	LIST *attribute_name_list = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	CREATE_CLONE_FILENAME *create_clone_filename = {0};
	char *destination_database_management_system;
	char *database_string = {0};
	boolean output2shell_script = 0;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 14 )
	{
		fprintf(stderr,
"Usage: %s ignored shell_script_dont_output_if_statement_yn session login_name ignored destination_application folder attribute \"old_data\" \"new_data\" html|nohtml delete_yn really_yn destination_database_management_system [output2file_yn] [export_output]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	shell_script_dont_output_if_statement_yn = *argv[ 2 ];
	session = argv[ 3 ];
	login_name = argv[ 4 ];
	/* role_name = argv[ 5 ]; */
	destination_application = argv[ 6 ];
	folder_name = argv[ 7 ];
	attribute_name = argv[ 8 ];
	old_data = argv[ 9 ];
	new_data = argv[ 10 ];
	with_html = ( strcmp( argv[ 11 ], "html" ) == 0 );
	delete_yn = *argv[ 12 ];
	really_yn = *argv[ 13 ];
	destination_database_management_system = argv[ 14 ];

	if ( !*destination_application
	||   strcmp( destination_application, "destination_application" ) == 0 )
	{
		destination_application = application_name;
	}

	if ( !*destination_database_management_system
	||   strcmp(	destination_database_management_system,
			"database_management_system" ) == 0 )
	{
		destination_database_management_system = "mysql";
	}

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	/* ------------------------------------------------- */
	/* See $APPASERVER_HOME/library/appaserver_library.h */
	/* ------------------------------------------------- */

	if ( DIFFERENT_DESTINATION_APPLICATION_OK ) goto passed_security_check;

	if ( !session_access(	application_name,
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

	if ( timlib_strcmp(	application_name,
				destination_application ) == 0 )
	{
		goto passed_security_check;
	}

	if ( timlib_strcmp(	application_name,
				TEMPLATE_APPLICATION ) == 0 )
	{
		goto passed_security_check;
	}

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf(
"<h4> Error: For security, the destination application must match either the current application or be the template application.</h4>\n" );
	document_close();
	exit( 1 );

passed_security_check:

	/* argv[ 15 ] is output2file_yn */
	/* ---------------------------- */
	if ( argc >= 16 && *argv[ 15 ] == 'y' )
	{
		create_clone_filename =
			create_clone_filename_new(
				destination_application,
				appaserver_parameter_file->
					appaserver_data_directory );

		/* argv[ 16 ] is export_output */
		/* --------------------------- */
		if ( argc == 17 )
		{
			output2shell_script =
				( strcmp( argv[ 16 ], "shell_script" ) == 0 );
		}
	}

	if ( strcmp( folder_name, "application" ) == 0 )
	{
		static char local_table_name[ 128 ];
		sprintf( local_table_name,
			 "%s_%s",
			 destination_application,
			 folder_name );
		destination_table_name = local_table_name;
	}
	else
	{
		destination_table_name = folder_name;
	}

	if ( *attribute_name && strcmp( attribute_name, "attribute" ) != 0 )
	{
		attribute_name_list = 
			appaserver_library_get_attribute_name_list(
						application_name, 
						folder_name );

		attribute_piece = list_get_string_item_offset( 	
							attribute_name_list,
							attribute_name );
	
		if ( attribute_piece == -1  )
		{
			if ( with_html )
			{
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
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );

				document_output_body(
					document->application_name,
					document->onload_control_string );

			}
			printf( 
"<p>ERROR: The attribute = (%s) does not belong to folder = (%s). Should be in (%s).\n",
				attribute_name,
				folder_name,
				list_display( attribute_name_list ) );
			if ( with_html ) document_close();
			exit( 1 );
		}
	}

	if ( with_html )
	{
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

		document_output_body(	document->application_name,
					document->onload_control_string );
	}

	/* Do the work */
	/* ----------- */
	sys_string = get_sys_string(
				destination_table_name,
				application_name,
				folder_name,
				attribute_name, 
				old_data,
				new_data,
				attribute_piece,
				delete_yn,
				really_yn,
				create_clone_filename );

	fflush( stdout );
	if ( system( sys_string ) ) {};

	if ( create_clone_filename )
	{
		char filename[ 1024 ];

		if ( output2shell_script )
		{

			if ( !go_output2shell_script(
				create_clone_filename,
				application_name,
				folder_name,
				shell_script_dont_output_if_statement_yn ) )
			{
				printf(
				"<p>ERROR: cannot create %s for write\n",
				create_clone_filename_get_shell_script_filename(
				filename,
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				folder_name ) );

			}
			else
			{
				printf( "<p>Created %s\n",
			        create_clone_filename_get_shell_script_filename(
				filename,
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				folder_name ) );
			}
		}
		else
		{
			printf( "<p>Created %s\n",
		create_clone_filename_get_clone_folder_compressed_filename(
				filename,
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				folder_name ) );
		}
	}

	if ( toupper( really_yn ) == 'Y' )
		printf( "<p>Process complete\n" );
	else
		printf( "<p>Testing complete\n" );


	if ( with_html ) document_close();

	exit( 0 );

} /* main() */


char *get_sys_string(	char *destination_table_name,
			char *application_name,
			char *folder_name,
			char *attribute_name, 
			char *old_data,
			char *new_data,
			int attribute_piece,
			char delete_yn,
			char really_yn,
			CREATE_CLONE_FILENAME *create_clone_filename )
{
	char buffer[ 4096 ];
	char where_clause[ 512 ];
	LIST *attribute_name_list;
	char *mysql_process;
	char sql_statement_tee_process[ 1024 ];
	char search_replace_attribute_not_cloned_process[ 512 ];
	char output_delete_statement_process[ 512 ];
	char replace_date_process[ 512 ];
	char replace_special_characters_process[ 64 ];
	char *quick_yes_no;
	LIST *attribute_list;

	attribute_list =
		attribute_get_attribute_list(
		application_name,
		folder_name,
		(char *)0 /* attribute_name */,
		(LIST *)0 /* mto1_isa_related_folder_list */,
		(char *)0 /* role_name */ );

/*
	LIST *date_attribute_position_list = {0};
	date_attribute_position_list =
		attribute_get_date_attribute_position_list(
			attribute_list );
*/

	strcpy(	replace_special_characters_process,
		"sed \"s/'/''/g\"" );

	strcpy( replace_date_process, "cat" );

	attribute_name_list =
		attribute_get_attribute_name_list(
			attribute_list );

	/* Don't clone execution_count */
	/* --------------------------- */
	list_delete_string(
		attribute_name_list,
		"execution_count" );

	/* Don't clone password */
	/* -------------------- */
	/*
	list_delete_string(
		attribute_name_list,
		"password" );
	*/

	/* Don't clone grace_no_cycle_colors_yn */
	/* ------------------------------------ */
	list_delete_string(
		attribute_name_list,
		"grace_no_cycle_colors_yn" );

	if ( strcmp( old_data, "old_data" ) == 0
	&&   strcmp( new_data, "new_data" ) == 0 )
	{

		if ( delete_yn == 'y' )
		{
			sprintf( output_delete_statement_process,
		 		 "echo \"delete from %s;\"",
		 		 destination_table_name );
		}
		else
		{
			strcpy( output_delete_statement_process, 
				"echo '' > /dev/null" );
		}

		strcpy( where_clause, "1 = 1" );
		strcpy( search_replace_attribute_not_cloned_process, "cat" );
	}
	else
	{
		if ( *attribute_name )
		{
			sprintf(search_replace_attribute_not_cloned_process,
			 	"piece_replace '^' %d \'%s\'",
		 	 	attribute_piece,
		 	 	new_data );

			sprintf(where_clause,
		 		"%s = '%s'",
		 		attribute_name,
		 		old_data );
		}
		else
		{
			strcpy( where_clause, "1 = 1" );
			strcpy( search_replace_attribute_not_cloned_process,
				"cat" );
		}

		if ( delete_yn == 'y' )
		{
			sprintf( output_delete_statement_process,
		 		"echo \"delete from %s where %s = '%s';\"",
		 			destination_table_name,
		 			attribute_name,
		 			new_data );
		}
		else
		{
			strcpy( output_delete_statement_process, 
				"echo '' > /dev/null" );
		}
	}

	if ( toupper( really_yn ) == 'Y' )
		mysql_process = "sql.e 2>&1 | html_paragraph_wrapper";
	else
	{
		if ( create_clone_filename )
			mysql_process = "cat > /dev/null";
		else
			mysql_process = "html_paragraph_wrapper";
	}

	quick_yes_no = get_quick_yes_no(	application_name,
						folder_name );

	if ( create_clone_filename )
	{
		sprintf( sql_statement_tee_process,
		"%s",
		create_clone_filename_get_clone_folder_tee_process_string(
			&create_clone_filename->directory_name,
			create_clone_filename->application_name,
			create_clone_filename->
				appaserver_data_directory,
			folder_name ) );
	}
	else
	{
		strcpy( sql_statement_tee_process, "cat" );
	}

	sprintf( buffer,
		 "(%s						;"
		 "get_folder_data	application=%s		 "
		 "			folder=%s		 "
		 "			select=%s		 "
		 "			where=\"%s\"		 "
		 "			quick=%s		 "
		 "			order=none		|"
		 "%s						|"
		 "%s						|"
		 "%s						|"
		 "insert_statement.e %s '%s' '^')		|"
		 "%s						|"
		 "%s						 ",
		 output_delete_statement_process,
		 application_name,
		 folder_name,
		 list_display_delimited( attribute_name_list, ',' ),
		 where_clause,
		 quick_yes_no,
		 search_replace_attribute_not_cloned_process,
		 replace_date_process,
		 replace_special_characters_process,
		 destination_table_name,
		 list_display_delimited( attribute_name_list, ',' ),
		 sql_statement_tee_process,
		 mysql_process );

	return strdup( buffer );

} /* get_sys_string() */

char *get_quick_yes_no( char *application_name, char *folder_name )
{
	char sys_string[ 1024 ];
	int count;
	char *return_string;
	char *table_name;

	table_name = get_table_name( application_name, folder_name );

	sprintf( sys_string,
		 "echo \"select count(*) from %s;\" | sql.e",
		 table_name );

	return_string = pipe2string( sys_string );

	if ( !return_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot get folder count\n",
			__FILE__,
			__FUNCTION__ );
		exit( 1 );
	}
	else
	{
		count = atoi( return_string );
	}
	
	return (count > MIN_COUNT_FOR_QUICK) ? "yes" : "no";

} /* get_quick_yes_no() */

boolean go_output2shell_script(	CREATE_CLONE_FILENAME *create_clone_filename,
				char *application_name,
				char *folder_name,
				char dont_output_if_statement_yn )
{
	char shell_script_filename[ 512 ];
	char insert_statement_filename[ 512 ];
	FILE *output_file;
	char sys_string[ 1024 ];

	create_clone_filename_get_shell_script_filename(
				shell_script_filename,
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				folder_name );

	if ( ! ( output_file = fopen( shell_script_filename, "w" ) ) )
		return 0;

	go_output2shell_script_header(
				output_file,
				create_clone_filename->application_name,
				folder_name,
				dont_output_if_statement_yn );
	fclose( output_file );

	create_clone_filename_get_clone_folder_compressed_filename(
				insert_statement_filename,
				&create_clone_filename->directory_name,
				create_clone_filename->application_name,
				create_clone_filename->
					appaserver_data_directory,
				folder_name );

	go_output2shell_script_insert_statements(
				application_name,
				shell_script_filename,
				insert_statement_filename,
				folder_name );

	if ( ! ( output_file = fopen( shell_script_filename, "a" ) ) )
		return 0;

	fprintf( output_file,
		 "all_done\n) 2>&1 | grep -vi duplicate\nexit 0\n" );
	fclose( output_file );

	sprintf(	sys_string,
			"chmod +x,g+w %s 2>/dev/null",
			shell_script_filename );
	if ( system( sys_string ) ) {};

	return 1;

} /* go_output2shell_script() */

void go_output2shell_script_header(
			FILE *output_file,
			char *application_name,
			char *folder_name,
			char dont_output_if_statement_yn )
{
	fprintf( output_file,
	"#!/bin/sh\n" );

#ifdef NOT_DEFINED
	if ( strcmp( folder_name, "application" ) != 0 )
	{
		fprintf( output_file,
		"if [ \"$#\" -ne 1 ]\n" );
		fprintf( output_file,
		"then\n" );
		fprintf( output_file,
		"\techo \"Usage: $0 application\" 1>&2\n" );
		fprintf( output_file,
		"\texit 1\n" );
		fprintf( output_file,
		"fi\n" );
	
		fprintf( output_file, "application=$1\n\n" );

		if ( dont_output_if_statement_yn != 'y'
		&&   !appaserver_library_is_system_folder( folder_name ) )
		{
			fprintf( output_file,
			"if [ \"$application\" != %s ]\n", application_name );
			fprintf( output_file,
			"then\n" );
			fprintf( output_file,
			"\texit 0\n" );
			fprintf( output_file,
			"fi\n\n" );
		}

	}
#endif

	if ( dont_output_if_statement_yn != 'y'
	&&   !appaserver_library_is_system_folder( folder_name ) )
	{
		fprintf( output_file,
			"if [ \"$application\" != %s ]\n", application_name );
		fprintf( output_file,
			"then\n" );
		fprintf( output_file,
			"\texit 0\n" );
		fprintf( output_file,
			"fi\n\n" );
	}

	if ( strcmp( folder_name, "application" ) == 0 )
	{
		fprintf( output_file,
		"table_name=%s_application\n", application_name );
	}
	else
	{
		fprintf( output_file,
		"table_name=%s\n", folder_name );
	}

	fprintf( output_file, "(\nsql.e << all_done\n" );

} /* go_output2shell_script_header() */

void go_output2shell_script_insert_statements(
				char *application_name,
				char *shell_script_filename,
				char *insert_statement_filename,
				char *folder_name )
{
	char sys_string[ 1024 ];

/*
		 "sed 's/\\$/\\\\$/g'					|"
*/
	sprintf( sys_string,
		 "zcat %s						|"
		 "sed 's/^insert into %s/insert into \\$table_name/'	|"
		 "sed 's/^delete from %s/delete from \\$table_name/'	|"
		 "sed 's/^insert into %s_%s/insert into \\$table_name/'	|"
		 "sed 's/^delete from %s_%s/delete from \\$table_name/'	|"
		 "cat >> %s						 ",
		 insert_statement_filename,
		 folder_name,
		 folder_name,
		 application_name,
		 folder_name,
		 application_name,
		 folder_name,
		 shell_script_filename );

/*
fprintf( stderr, "%s\n", sys_string );
*/

	if ( system( sys_string ) ) {};

} /* go_output2shell_script_insert_statements() */

