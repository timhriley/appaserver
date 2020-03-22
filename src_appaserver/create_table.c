/* --------------------------------------------------- 	*/
/* src_appaserver/create_table.c		       	*/
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
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "appaserver.h"
#include "boolean.h"
#include "dictionary2file.h"
#include "application.h"
#include "session.h"

/* Constants */
/* --------- */
#define CREATE_TABLE_FILENAME_TEMPLATE		 "%s/create_table.sh"

/* Prototypes */
/* ---------- */
void make_executable(	char *create_table_filename );

char *get_sys_string(	char *folder_name,
			char *table_name,
			LIST *attribute_list,
			char *create_table_filename,
			char build_shell_script_yn,
			char *data_directory,
			char *index_directory,
			char *create_view_statement );

int main( int argc, char **argv )
{
	char *current_application;
	char *session;
	char *login_name;
	char *role_name;
	char *destination_application;
	char *folder_name;
	char *table_name;
	char *really_yn;
	char build_shell_script_yn;
	char *sys_string;
	boolean html_ok;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FOLDER *folder;
	char create_table_filename[ 512 ] = {0};

	current_application = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		current_application );

	if ( argc < 10 )
	{
		fprintf(stderr,
"Usage: %s ignored build_shell_script_yn session login_name role destination_application folder really_yn ignored [nohtml]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	build_shell_script_yn = *argv[ 2 ];
	session = argv[ 3 ];
	login_name = argv[ 4 ];
	role_name = argv[ 5 ];
	destination_application = argv[ 6 ];
	folder_name = argv[ 7 ];
	really_yn = argv[ 8 ];
	/* destination_database_management_system = argv[ 9 ]; */

	if ( build_shell_script_yn == 'y' ) really_yn = "n";

	html_ok = (argc != 11 || strcmp( argv[ 10 ], "nohtml" ) != 0 );

	if ( !*destination_application
	||   strcmp(	destination_application,
			"destination_application" ) == 0 )
	{
		destination_application = current_application;
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	/* ------------------------------------------------- */
	/* See $APPASERVER_HOME/library/appaserver_library.h */
	/* ------------------------------------------------- */

	if ( DIFFERENT_DESTINATION_APPLICATION_OK ) goto passed_security_check;

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

	if ( timlib_strcmp(	current_application,
				TEMPLATE_APPLICATION ) != 0
	&&   timlib_strcmp(	current_application,
				destination_application ) != 0 )
	{
		if ( html_ok )
		{
			document_quick_output_body(
				current_application,
				appaserver_parameter_file->
					appaserver_mount_point );
		}
		printf(
"<h4> Error in %s: For security, the destination application must match the current application.</h4>\n",
			__FILE__ );
		if ( html_ok ) document_close();
		exit( 1 );
	}

passed_security_check:

	if ( *really_yn != 'y' )
	{
		sprintf(create_table_filename,
		 	CREATE_TABLE_FILENAME_TEMPLATE,
		 	appaserver_parameter_file->appaserver_data_directory );
	}

	application_reset();
	table_name = get_table_name(	destination_application, 
					folder_name );
	application_reset();

	folder = folder_new_folder(
				current_application,
				BOGUS_SESSION,
				folder_name );

	folder_load(	&folder->insert_rows_number,
			&folder->lookup_email_output,
			&folder->row_level_non_owner_forbid,
			&folder->row_level_non_owner_view_only,
			&folder->populate_drop_down_process,
			&folder->post_change_process,
			&folder->folder_form,
			&folder->notepad,
			&folder->html_help_file_anchor,
			&folder->post_change_javascript,
			&folder->lookup_before_drop_down,
			&folder->data_directory,
			&folder->index_directory,
			&folder->no_initial_capital,
			&folder->subschema_name,
			&folder->create_view_statement,
			folder->application_name,
			BOGUS_SESSION,
			folder->folder_name,
			0 /* override_row_restrictions */,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	folder->attribute_list =
		attribute_get_attribute_list(
		folder->application_name,
		folder->folder_name,
		(char *)0 /* attribute_name */,
		(LIST *)0 /* mto1_isa_related_folder_list */,
		(char *)0 /* role_name */ );

	if ( !list_length( folder->attribute_list ) )
	{
		if ( html_ok )
		{
			document_quick_output_body(
				folder->application_name,
				appaserver_parameter_file->
					appaserver_mount_point );
		}
		printf( 
	"<h4> Error: there are no attributes assigned folder = %s.</h4>\n",
			folder->folder_name );
		if ( html_ok ) document_close();
		exit( 1 );
	}

	if ( html_ok )
	{
		document_quick_output_body(
			folder->application_name,
			appaserver_parameter_file->
				appaserver_mount_point );
	}

	/* Display the work */
	/* ---------------- */
	sys_string = get_sys_string(
				folder_name,
				table_name,
				folder->attribute_list,
				(char *)0 /* create_table_filename */,
				build_shell_script_yn,
				folder->data_directory,
				folder->index_directory,
				folder->create_view_statement );

	printf( "<p>%s\n", sys_string );

	/* Do the work */
	/* ----------- */
	sys_string = get_sys_string(
				folder_name,
				table_name,
				folder->attribute_list,
				create_table_filename,
				build_shell_script_yn,
				folder->data_directory,
				folder->index_directory,
				folder->create_view_statement );

	if ( *create_table_filename )
	{
		FILE *output_file;

		output_file = fopen( create_table_filename, "w" );

		if ( !output_file )
		{
			printf( "<p>ERROR: cannot write to %s\n",
				create_table_filename );
		}
		else
		{
			fprintf( output_file, "%s\n", sys_string );
			fclose( output_file );
			make_executable( create_table_filename );

			if ( html_ok )
			{
				printf(
				"<p>Created: %s\n", create_table_filename );
			}
		}
	}
	else
	if ( toupper( *really_yn ) == 'Y' )
	{
		fflush( stdout );
		if ( system( sys_string ) ) {};

		if ( html_ok ) printf( "<h3>Process complete</h3>\n" );
	}
		

	if ( html_ok ) document_close();

	return 0;

} /* main() */

char *get_sys_string(	char *folder_name,
			char *table_name,
			LIST *attribute_list,
			char *create_table_filename,
			char build_shell_script_yn,
			char *data_directory,
			char *index_directory,
			char *create_view_statement )
{
	char buffer[ 65536 ];
	char *buffer_ptr;
	ATTRIBUTE *attribute;
	boolean create_first_time = 1;
	boolean additional_unique_first_time = 1;

	buffer_ptr = buffer;

	if ( ( create_table_filename && *create_table_filename )
	||     build_shell_script_yn == 'y' )
	{
		buffer_ptr += sprintf( buffer_ptr,
		"#!/bin/sh\n" );
		buffer_ptr += sprintf( buffer_ptr,
		"#create %s\n\n", folder_name );

		buffer_ptr += sprintf( buffer_ptr,
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
		"   echo \"Error in `basename.e $0 n`: you must first:\" 1>&2\n"
		"   echo \"$ . set_database\" 1>&2\n"
		"   exit 1\n"
		"fi\n" );

		if ( strcmp( folder_name, "application" ) == 0 )
		{
			buffer_ptr += sprintf(
				buffer_ptr,
				"table_name=${application}_%s\n",
			 	folder_name );
		}
		else
		{
			buffer_ptr += sprintf(
				buffer_ptr,
				"table_name=%s\n",
			 	folder_name );
		}
	} /* if ( create_table_filename && *create_table_filename ) */

	/* If creating a view */
	/* ------------------ */
	if ( create_view_statement && *create_view_statement )
	{
		buffer_ptr += sprintf( buffer_ptr,
"echo \"%s\" | sql.e\n",
				create_view_statement );

		buffer_ptr += sprintf( buffer_ptr, "\n" );
		return strdup( buffer );
	}

	if ( ( create_table_filename && *create_table_filename )
	||     build_shell_script_yn == 'y' )
	{
		buffer_ptr += sprintf(	buffer_ptr,
"echo \"drop table if exists $table_name;\" | sql.e\n" );
	}
	else
	{
		buffer_ptr += sprintf(
	 		buffer_ptr,
"echo \"drop table if exists %s;\" |"
" sql.e 2>&1 | html_paragraph_wrapper;", 
	 		table_name );
	}

	/* Create the create table statement */
	/* --------------------------------- */
	if ( ( create_table_filename && *create_table_filename )
	||     build_shell_script_yn == 'y' )
	{
		buffer_ptr += sprintf( 
		 	buffer_ptr,
		 	"echo \"create table $table_name (" );
	}
	else
	{
		buffer_ptr += sprintf( 
		 	buffer_ptr,
		 	"echo \"create table %s (", table_name );
	}

	if ( !list_rewind( attribute_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty attribute_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		attribute = list_get_pointer( attribute_list );

		if ( create_first_time )
			create_first_time = 0;
		else
			buffer_ptr += sprintf( buffer_ptr, "," );

		if ( !attribute->datatype || !*attribute->datatype )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: attribute = (%s) has empty attribute_datatype.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 attribute->attribute_name );
			exit( 1 );
		}

		buffer_ptr += sprintf(
			buffer_ptr,
			"%s %s",
			attribute->attribute_name,
			attribute_get_database_datatype(
				attribute->datatype,
				attribute->width,
				attribute->float_decimal_places,
				attribute->primary_key_index ) );

	} while( list_next( attribute_list ) );

	buffer_ptr += sprintf(
		 	buffer_ptr,
		 	")" );

	if ( data_directory && *data_directory )
	{
		buffer_ptr += sprintf(
		 		buffer_ptr,
		 		" data directory = '%s',",
				data_directory );
	}

	if ( index_directory && *index_directory )
	{
		buffer_ptr += sprintf(
		 		buffer_ptr,
		 		" index directory = '%s',",
				index_directory );
	}

	buffer_ptr += sprintf(
		 	buffer_ptr,
		 	" engine MyISAM" );

	if ( ( create_table_filename && *create_table_filename )
	||     build_shell_script_yn == 'y' )
	{
		buffer_ptr += sprintf(
		 	buffer_ptr,
		 	";\" | sql.e\n" );
	}
	else
	{
		buffer_ptr += sprintf(
		 	buffer_ptr,
		 	";\" | sql.e 2>&1 | html_paragraph_wrapper;" );
	}

	if ( ( create_table_filename && *create_table_filename )
	||     build_shell_script_yn == 'y' )
	{
		buffer_ptr += sprintf(
		 	buffer_ptr,
"echo \"create unique index $table_name on $table_name (%s)",
		 	list_display(
				folder_get_primary_attribute_name_list(
				attribute_list ) ) );
	}
	else
	{
		buffer_ptr += sprintf(
		 	buffer_ptr,
"echo \"create unique index %s on %s (%s)",
		 	table_name,
		 	table_name,
		 	list_display(
				folder_get_primary_attribute_name_list(
				attribute_list ) ) );
	}

	if ( ( create_table_filename && *create_table_filename )
	||     build_shell_script_yn == 'y' )
	{
		buffer_ptr += sprintf(
		 	buffer_ptr,
";\" | sql.e\n" );
	}
	else
	{
		buffer_ptr += sprintf(
		 	buffer_ptr,
";\" | sql.e 2>&1 | html_paragraph_wrapper" );
	}

	/* Create additional indexes */
	/* ------------------------- */
	list_rewind( attribute_list ); 
	do {
		attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

		if ( attribute->additional_index )
		{
			if ( ( create_table_filename && *create_table_filename )
			||     build_shell_script_yn == 'y' )
			{
				buffer_ptr += sprintf(
		 			buffer_ptr,
"echo \"create index ${table_name}_%s on $table_name (%s);\" | sql.e\n",
					attribute->attribute_name,
					attribute->attribute_name );
			}
			else
			{
				buffer_ptr += sprintf(
		 			buffer_ptr,
";echo \"create index %s_%s on %s (%s);\" | sql.e 2>&1 | html_paragraph_wrapper",
		 			table_name,
					attribute->attribute_name,
		 			table_name,
					attribute->attribute_name );
			}
		}

	} while( list_next( attribute_list ) );

	/* Create additional unique indexes */
	/* -------------------------------- */
	list_rewind( attribute_list ); 
	do {
		attribute = list_get_pointer( attribute_list );

		if ( attribute->additional_unique_index )
		{
			if ( additional_unique_first_time )
			{
				additional_unique_first_time = 0;

				if ( ( create_table_filename
				&&     *create_table_filename )
				||     build_shell_script_yn == 'y' )
				{
					buffer_ptr += sprintf(
		 				buffer_ptr,
"echo \"create unique index ${table_name}_additional_unique on $table_name (%s",
						attribute->attribute_name );
				}
				else
				{
					buffer_ptr += sprintf(
		 				buffer_ptr,
";echo \"create unique index %s_additional_unique on %s (%s",
		 				table_name,
		 				table_name,
						attribute->attribute_name );
				}
			}
			else
			{
				buffer_ptr += sprintf(
		 			buffer_ptr,
					",%s",
					attribute->attribute_name );
			}
		}

	} while( list_next( attribute_list ) );

	if ( !additional_unique_first_time )
	{
		if ( ( create_table_filename
		&&     *create_table_filename )
		||     build_shell_script_yn == 'y' )
		{
			buffer_ptr += sprintf(
		 		buffer_ptr,
");\" | sql.e\n" );
		}
		else
		{
			buffer_ptr += sprintf(
		 		buffer_ptr,
");\" | sql.e 2>&1 | html_paragraph_wrapper" );
		}
	}

	buffer_ptr += sprintf( buffer_ptr, "\n" );
	return strdup( buffer );

} /* get_sys_string() */

void make_executable( char *create_table_filename )
{
	char sys_string[ 512 ];

	sprintf(sys_string,
		"chmod -x,ug+x %s 2>/dev/null",
		create_table_filename );
	if ( system( sys_string ) ) {};
}

