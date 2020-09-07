/* $APPASERVER_HOME/src_appaserver/output_histogram.c		*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you select the histogram radio	*/
/* button in some of the lookup forms.				*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "folder.h"
#include "operation.h"
#include "dictionary.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "grace.h"
#include "application.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "decode_html_post.h"
#include "appaserver.h"
#include "column.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *original_post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	LIST *query_record_list;
	char *query_record;
	FOLDER *folder;
	QUERY *query;
	LIST *select_attribute_name_list;
	char *select_attribute_name;
	char buffer[ 256 ];
	char title[ 256 ];
	char sub_title[ 1024 ];
	FILE *histogram_pipe;
	FILE *input_file;
	char input_buffer[ 256 ];
	char grace_histogram_filename[ 256 ];
	char sys_string[ 1024 ];
	char ftp_agr_filename[ 256 ];
	char ftp_output_filename[ 256 ];
	char output_filename[ 256 ];
	DICTIONARY_APPASERVER *dictionary_appaserver;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder role ignored [dictionary_stdin]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	/* state = argv[ 6 ]; */

	if ( argc == 8 && strcmp( argv[ 7 ], "dictionary_stdin" ) == 0 )
	{
		get_line( dictionary_string, stdin );
		decode_html_post(	decoded_dictionary_string, 
					dictionary_string );

		original_post_dictionary = 
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );

		if ( ! ( dictionary_appaserver =
				dictionary_appaserver_new(
					original_post_dictionary,
					(char *)0 /* application_name */,
					(LIST *)0 /* attribute_list */,
					(LIST *)0 /* operation_name_list */) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: exiting early.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}
	}
	else
	{
		dictionary_appaserver =
				dictionary_appaserver_new(
					(DICTIONARY *)0,
					(char *)0 /* application_name */,
					(LIST *)0 /* attribute_list */,
					(LIST *)0 /* operation_name_list */ );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	folder = folder_new_folder( 	application_name,
					session,
					folder_name );

	folder->attribute_list =
	attribute_get_attribute_list(
		folder->application_name,
		folder->folder_name,
		(char *)0 /* attribute_name */,
		(LIST *)0 /* mto1_isa_related_folder_list */,
		role_name );

	sprintf(	title,
			"%s Histogram",
			format_initial_capital(
				buffer,
				folder_name ) );

	document = document_new( title, application_name );
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

	printf( "<h1>Histogram</h1>\n" );
	fflush( stdout );

	select_attribute_name_list =
		attribute_get_histogram_attribute_name_list(
			folder->attribute_list );

	if ( list_rewind( select_attribute_name_list ) )
	{
		LIST *query_select_attribute_name_list;

		do {
			select_attribute_name =
				list_get_pointer(
					select_attribute_name_list );
	
			sprintf( grace_histogram_filename,
		 		"%s/grace_histogram_%s_%d.dat",
		 		appaserver_parameter_file->
					appaserver_data_directory,
		 		select_attribute_name,
		 		getpid() );

			query_select_attribute_name_list = list_new();

			list_append_pointer(
				query_select_attribute_name_list,
				select_attribute_name );

			query = query_edit_table_new(
					dictionary_appaserver->
						query_dictionary,
					application_name,
					login_name,
					folder_name,
					role_new(
						application_name,
						role_name ) );

			query_record_list =
				query_get_record_list(
					application_name,
					query->query_output,
					list_display(
					     query_select_attribute_name_list ),
					query->query_output->order_clause );

			strcpy( sub_title,
				query_get_display_where_clause(
				query->query_output->where_clause,
				application_name,
				folder_name,
				application_get_is_primary_application(
					application_name ) ) );

			if ( strcmp( sub_title, "1 = 1" ) == 0 )
				*sub_title = '\0';

			strcpy( buffer, "'" );
			search_replace_string( sub_title, buffer, "" );

			strcpy(	sub_title,
				format_initial_capital( buffer, sub_title ) );

			if ( !query_record_list
			||   !list_rewind( query_record_list ) )
			{
				printf(
			"<h2>No data for selected parameters.</h2>\n" );
				document_close();
				exit( 0 );
			}

			sprintf(	sys_string,
"grace_histogram.e %s %s \"%s\" \"%s\" \"%s\" \"%s\" | grep '\\.agr'> %s",
					application_name,
					session,
					title,
					sub_title,
					"",
					select_attribute_name,
					grace_histogram_filename );

			histogram_pipe = popen( sys_string, "w" );

			do {
				query_record =
					list_get_pointer(
						query_record_list );

				fprintf( histogram_pipe, "%s\n", query_record );

			} while( list_next( query_record_list ) );

			pclose( histogram_pipe );

			input_file = fopen( grace_histogram_filename, "r" );

			if ( !input_file )
			{
				fprintf( stderr,
			 		"%s/%s(): cannot open %s for read.\n",
			 		__FILE__,
			 		__FUNCTION__,
			 		grace_histogram_filename );
				exit( 1 );
			}

			if ( !get_line( input_buffer, input_file ) )
			{
				fprintf( stderr,
			 	"%s/%s(): unexpected end of file in %s\n",
			 		__FILE__,
			 		__FUNCTION__,
			 		grace_histogram_filename );
				exit( 1 );
			}

			column( ftp_agr_filename, 0, input_buffer );
			column( ftp_output_filename, 1, input_buffer );
			column( output_filename, 2, input_buffer );
		
			fclose( input_file );
			sprintf( sys_string,
				 "rm -f %s",
				 grace_histogram_filename );
			system( sys_string );

			printf(
		"<p><a href=\"%s\" target=_new>&lt;Left Click&gt; %s</a>\n",
				ftp_output_filename,
				select_attribute_name );

			appaserver_library_output_ftp_prompt(
				ftp_agr_filename, 
				grace_get_agr_prompt(),
				(char *)0    /* target */,
				"application/agr" );

		} while( list_next( select_attribute_name_list ) );
	}

	document_close();
	return 0;

} /* main() */

