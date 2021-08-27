/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/fix_orphans.c       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "application.h"
#include "folder.h"
#include "related_folder.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"

/* Constants */
/* --------- */
#define OUTPUT_COUNT		100000
#define TEXT_OUTPUT_TEMPLATE	"%s/fix_orphans_%d.txt"

/* Prototypes */
/* ---------- */
void check_row_dictionary(
				FILE *table_output_pipe,
				FILE *text_output_pipe,
				FILE *insert_delete_pipe,
				DICTIONARY *row_dictionary,
				LIST *mto1_append_isa_related_folder_list,
				char *application_name,
				char *folder_name,
				char delete_yn );

void check_row_dictionary_with_related_folder(
				FILE *table_output_pipe,
				FILE *text_output_pipe,
				FILE *insert_delete_pipe,
				DICTIONARY *row_dictionary,
				RELATED_FOLDER *related_folder,
				char *application_name,
				char *folder_name,
				char delete_yn );

void output_insert_statement(
				FILE *table_output_pipe,
				FILE *text_output_pipe,
				FILE *insert_delete_pipe,
				RELATED_FOLDER *related_folder,
				char *foreign_data_key_list_string,
				char *application_name );

void output_delete_statement(
				FILE *table_output_pipe,
				FILE *text_output_pipe,
				FILE *insert_delete_pipe,
				char *folder_name,
				LIST *foreign_attribute_name_list,
				char *foreign_data_key_list_string,
				char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *folder_name;
	char execute_yn;
	char delete_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FOLDER *folder;
	char sys_string[ 1024 ];
	char text_output_filename[ 128 ];
	char input_buffer[ 65536 ];
	FILE *input_pipe;
	char *table_name;
	LIST *attribute_name_list;
	DICTIONARY *row_dictionary;
	char title[ 256 ];
	FILE *table_output_pipe = {0};
	FILE *text_output_pipe;
	FILE *insert_delete_pipe = {0};
	long count = 0;
	boolean output_medium_stdout = 0;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 5 )
	{
		fprintf(stderr,
"Usage: %s process_name folder_name delete_yn execute_yn [stdout]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	folder_name = argv[ 2 ];
	delete_yn = *argv[ 3 ];
	execute_yn = *argv[ 4 ];

	if ( argc == 6 )
	{
		output_medium_stdout = ( strcmp( argv[ 5 ], "stdout" ) == 0 );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !output_medium_stdout )
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
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
		document_output_body(
				document->application_name,
				document->onload_control_string );
	
		sprintf( title,
			 "%s for %s",
			 process_name,
			 folder_name );
	
		sprintf(sys_string,
		"sort | html_table.e '%s' 'Folder,Attribute,New Data' '|'",
			format_initial_capital( title, title ) );

		table_output_pipe = popen( sys_string, "w" );
	
		sprintf( text_output_filename,
			 TEXT_OUTPUT_TEMPLATE,
			 appaserver_parameter_file->appaserver_data_directory,
			 getpid() );

		sprintf( sys_string,
			 "sort | cat > %s",
			 text_output_filename );

		text_output_pipe = popen( sys_string, "w" );
	}
	else
	{
		text_output_pipe = popen( "sort | sed 's/|/=/'", "w" );
	}

	if ( execute_yn == 'y' )
	{
		char sys_string[ 1024 ];

		sprintf(sys_string,
			"sort -u | sql.e 2>&1 | grep -vi duplicate 1>&2" );

		insert_delete_pipe = popen( sys_string, "w" );
	}

	folder =
		folder_with_load_new(
			application_name,
			BOGUS_SESSION,
			folder_name,
			(ROLE *)0 /* role */ );

	if ( !folder ) exit( 0 );

	if ( delete_yn != 'y' )
	{
		related_folder_mto1_append_unique(
			folder->mto1_append_isa_related_folder_list,
			folder->mto1_recursive_related_folder_list );
	}

	related_folder_populate_primary_data_dictionary(
			folder->mto1_append_isa_related_folder_list,
			application_name );

	table_name = get_table_name( application_name, folder->folder_name );

	attribute_name_list = 
		folder_get_attribute_name_list(
			folder->attribute_list );

	sprintf(sys_string,
		"echo \"select %s from %s;\" | sql_quick.e",
		list_display( attribute_name_list ),
		table_name );

	input_pipe = popen( sys_string, "r" );
	row_dictionary = dictionary_duper();

	while( get_line( input_buffer, input_pipe ) )
	{
		dictionary_set_delimited_string(
					row_dictionary,
					attribute_name_list,
					input_buffer,
					'^' );
		check_row_dictionary(
				table_output_pipe,
				text_output_pipe,
				insert_delete_pipe,
				row_dictionary,
				folder->mto1_append_isa_related_folder_list,
				application_name,
				folder_name,
				delete_yn );

		dictionary_free_data( row_dictionary, attribute_name_list );

		if ( count && !(count % OUTPUT_COUNT ) )
		{
			char msg[ 128 ];

			sprintf(msg,
				"%s: %12s",
				argv[ 0 ],
				place_commas_in_long( count ) );

			appaserver_output_error_message(
				application_name, msg, "" );
		}
		count++;
	}

	pclose( input_pipe );

	if ( table_output_pipe )
	{
		pclose( table_output_pipe );
	}

	pclose( text_output_pipe );

	if ( insert_delete_pipe ) pclose( insert_delete_pipe );

	if ( !output_medium_stdout )
	{
		if ( execute_yn == 'y' )
			printf( "<h3>Process Complete</h3>\n" );
		else
			printf( "<h3>Process Not Executed</h3>\n" );

		printf( "<h3>Created %s</h3>\n",
			text_output_filename );

		document_close();
	}

	if ( execute_yn == 'y' )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}

	exit( 0 );
} /* main() */

void check_row_dictionary(
				FILE *table_output_pipe,
				FILE *text_output_pipe,
				FILE *insert_delete_pipe,
				DICTIONARY *row_dictionary,
				LIST *mto1_append_isa_related_folder_list,
				char *application_name,
				char *folder_name,
				char delete_yn )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_append_isa_related_folder_list ) ) return;

	do {
		related_folder =
			list_get_pointer(
				mto1_append_isa_related_folder_list );

		check_row_dictionary_with_related_folder(
				table_output_pipe,
				text_output_pipe,
				insert_delete_pipe,
				row_dictionary,
				related_folder,
				application_name,
				folder_name,
				delete_yn );

	} while( list_next( mto1_append_isa_related_folder_list ) );

} /* check_row_dictionary() */

void check_row_dictionary_with_related_folder(
				FILE *table_output_pipe,
				FILE *text_output_pipe,
				FILE *insert_delete_pipe,
				DICTIONARY *row_dictionary,
				RELATED_FOLDER *related_folder,
				char *application_name,
				char *folder_name,
				char delete_yn )
{
	char *foreign_attribute_name;
	LIST *foreign_attribute_name_list;
	char foreign_data_key_list_string[ 1024 ];
	char *foreign_data_key_ptr;
	char *row_data;
	static DICTIONARY *guarantee_output_unique_dictionary = {0};

	foreign_attribute_name_list =
		related_folder->foreign_attribute_name_list;

	if ( !guarantee_output_unique_dictionary )
	{
		guarantee_output_unique_dictionary = dictionary_huge();
	}

	foreign_data_key_ptr = foreign_data_key_list_string;
	*foreign_data_key_ptr = '\0';

	if ( !list_rewind( foreign_attribute_name_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s/%d: empty foreign_attribute_name_list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		foreign_attribute_name =
			list_get_pointer(
				foreign_attribute_name_list );

		if ( !list_at_first( foreign_attribute_name_list ) )
		{
			foreign_data_key_ptr +=
				sprintf( foreign_data_key_ptr, "^" );
		}

		row_data =
			dictionary_get_data(
				row_dictionary,
				foreign_attribute_name );

		/* Don't insert nulls */
		/* ------------------ */
		if ( !row_data || !*row_data )
		{
			return;
		}

		foreign_data_key_ptr +=
			sprintf(	foreign_data_key_ptr,
					"%s",
					row_data );

	} while( list_next( foreign_attribute_name_list ) );

	if ( dictionary_exists_key(	related_folder->
						folder->
						primary_data_dictionary,
					foreign_data_key_list_string ) )
	{
		return;
	}

	if ( dictionary_exists_key(	guarantee_output_unique_dictionary,
					foreign_data_key_list_string ) )
	{
		return;
	}

	if ( delete_yn == 'y' )
	{
		output_delete_statement(
				table_output_pipe,
				text_output_pipe,
				insert_delete_pipe,
				folder_name,
				related_folder->
					foreign_attribute_name_list,
				foreign_data_key_list_string,
				application_name );
	}
	else
	{
		output_insert_statement(
				table_output_pipe,
				text_output_pipe,
				insert_delete_pipe,
				related_folder,
				foreign_data_key_list_string,
				application_name );
	}

	dictionary_set_pointer(
			guarantee_output_unique_dictionary,
			strdup( foreign_data_key_list_string ),
			"" );

} /* check_row_dictionary_with_related_folder() */

void output_insert_statement(
				FILE *table_output_pipe,
				FILE *text_output_pipe,
				FILE *insert_delete_pipe,
				RELATED_FOLDER *related_folder,
				char *foreign_data_key_list_string,
				char *application_name )
{
	LIST *foreign_data_key_list;
	char data_buffer[ 4096 ];
	char *data;
	char *table_name;

	foreign_data_key_list =
		list_string2list(
			foreign_data_key_list_string,
			'^' );

	if ( insert_delete_pipe )
	{
		table_name = get_table_name(
				application_name,
				related_folder->folder->folder_name );

		fprintf(insert_delete_pipe,
			"insert into %s (%s) values (",
			table_name,
			list_display( related_folder->
					folder->
					primary_key_list ) );
	}

	if ( table_output_pipe )
	{
		fprintf(	table_output_pipe,
				"%s|%s|",
				related_folder->folder->folder_name,
				list_display( related_folder->
						folder->
						primary_key_list ) );
	}

	fprintf(	text_output_pipe,
			"%s|%s|",
			related_folder->folder->folder_name,
			list_display( related_folder->
					folder->
					primary_key_list ) );

	list_rewind( related_folder->folder->primary_key_list );
	list_rewind( foreign_data_key_list );

	do {
		data = list_get_pointer( foreign_data_key_list );

		if ( !list_at_head( related_folder->
					folder->
					primary_key_list ) )
		{
			if ( table_output_pipe )
			{
				fprintf( table_output_pipe, "," );
			}

			fprintf( text_output_pipe, "," );

			if ( insert_delete_pipe )
			{
				fprintf( insert_delete_pipe, "," );
			}
		}

		if ( table_output_pipe )
		{
			fprintf( table_output_pipe, "%s", data );
		}

		fprintf( text_output_pipe, "%s", data );

		if ( insert_delete_pipe )
		{
			strcpy( data_buffer, data );
			escape_single_quotes( data_buffer );

			fprintf(insert_delete_pipe,
				"'%s'",
				data_buffer );
		}

		list_next( foreign_data_key_list );

	} while( list_next( related_folder->
				folder->
				primary_key_list ) );

	if ( table_output_pipe )
	{
		fprintf( table_output_pipe, "\n" );
	}

	fprintf( text_output_pipe, "\n" );

	if ( insert_delete_pipe ) fprintf( insert_delete_pipe, ");\n" );

} /* output_insert_statement() */

void output_delete_statement(
				FILE *table_output_pipe,
				FILE *text_output_pipe,
				FILE *insert_delete_pipe,
				char *folder_name,
				LIST *foreign_attribute_name_list,
				char *foreign_data_key_list_string,
				char *application_name )
{
	char *attribute_name;
	char *data;
	char *table_name;
	LIST *foreign_data_key_list;
	char data_buffer[ 4096 ];

	foreign_data_key_list =
		list_string2list(
			foreign_data_key_list_string,
			'^' );

	if ( insert_delete_pipe )
	{
		table_name = get_table_name(
				application_name,
				folder_name );

		fprintf(insert_delete_pipe,
			"delete from %s where 1 = 1",
			table_name );
	}

	if ( table_output_pipe )
	{
		fprintf(	table_output_pipe,
				"%s|%s|",
				folder_name,
				list_display( foreign_attribute_name_list ) );
	}

	fprintf(	text_output_pipe,
			"%s|%s|",
			folder_name,
			list_display( foreign_attribute_name_list ) );

	list_rewind( foreign_attribute_name_list );
	list_rewind( foreign_data_key_list );

	do {
		attribute_name =
			list_get_pointer(
				foreign_attribute_name_list );

		data = list_get_pointer( foreign_data_key_list );

		if ( !list_at_head( foreign_attribute_name_list ) )
		{
			if ( table_output_pipe )
			{
				fprintf( table_output_pipe, "," );
			}

			fprintf( text_output_pipe, "," );
		}

		if ( table_output_pipe )
		{
			fprintf( table_output_pipe, "%s", data );
		}

		fprintf( text_output_pipe, "%s", data );

		if ( insert_delete_pipe )
		{
			strcpy( data_buffer, data );
			escape_single_quotes( data_buffer );

			fprintf( insert_delete_pipe,
				 " and %s = '%s'",
				 attribute_name,
				 data_buffer );
		}

		list_next( foreign_data_key_list );

	} while( list_next( foreign_attribute_name_list ) );

	if ( table_output_pipe )
	{
		fprintf( table_output_pipe, "\n" );
	}
	fprintf( text_output_pipe, "\n" );

	if ( insert_delete_pipe ) fprintf( insert_delete_pipe, ";\n" );

} /* output_delete_statement() */

