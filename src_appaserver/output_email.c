/* $APPASERVER_HOME/src_appaserver/output_email.c		*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you put an email address	*/
/* in the lookup form.						*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "folder.h"
#include "operation.h"
#include "dictionary.h"
#include "query.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "decode_html_post.h"
#include "appaserver.h"
#include "role.h"
#include "dictionary_appaserver.h"

#define DELIMITER			','
#define ATTACHMENT_FILENAME_TEMPLATE	"%s/%s_%s_%d.csv"

void output_heading_to_file(	FILE *file, 
				LIST *attribute_name_list,
				char delimiter );

void output_dictionary_list_to_file( 
				FILE *file, 
				LIST *data_dictionary_list,
				LIST *attribute_name_list,
				char delimiter );

void output_dictionary_to_file( FILE *file, 
				DICTIONARY *data_dictionary,
				LIST *attribute_name_list,
				char delimiter );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *session;
	char *folder_name;
	char *role_name;
	char *email_address;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *post_dictionary;
	LIST *row_dictionary_list = {0};
	QUERY *query;
	FILE *file;
	DOCUMENT *document;
	char sys_string[ 1024 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	char attachment_filename[ 128 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder role ignored dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	/* state = argv[ 6 ]; */

	decode_html_post(
		decoded_dictionary_string, 
		argv[ 7 ] );

	post_dictionary = 
		dictionary_index_string2dictionary( 
			decoded_dictionary_string );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				post_dictionary,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_name_list */,
				(LIST *)0 /* attribute_date_name_list */,
				(LIST *)0 /* operation_name_list */,
				(char *)0 /* login_name */ ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: dictionary_appaserver_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_dictionary =
		dictionary_appaserver->
			working_post_dictionary;

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	email_address =
		dictionary_get_index_zero(
			dictionary_appaserver->query_dictionary,
			EMAIL_OUTPUT_NAME );

	if ( !email_address || !*email_address )
	{
		printf( "<p>Error in %s: cannot get email address from %s\n",
			 argv[ 0 ],
			 dictionary_display( post_dictionary ) );
		document_close();
		exit( 1 );
	}

	query =
		query_simple_new(
			query_dictionary,
			login_name,
			folder_name,
			role_name,
			dictionary_appaserver->
				ignore_select_attribute_name_list );

	if ( !query )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_simple_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_dictionary_list =
		query_dictionary_list(
			query->query_select_display,
			query->query_select_name_list,
			query->query_from,
			query->query_where,
			query->query_order,
			0 /* max_rows */,
			query->query_date_convert );

	sprintf(attachment_filename,
	 	ATTACHMENT_FILENAME_TEMPLATE, 
		appaserver_parameter_file->appaserver_data_directory,
	 	application_name,
		folder_name,
	 	getpid() );

	if ( ! ( file = fopen( attachment_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 attachment_filename );
		exit( 1 );
	}

	output_heading_to_file( file, attribute_name_list, DELIMITER );

	output_dictionary_list_to_file(
		file, 
		row_dictionary_list,
		query->select_name_list,
		DELIMITER );

	fclose( file );

	printf( "<H1>Emailing %d records.</H1>\n",
		list_length( row_dictionary_list ) );

	sprintf( sys_string,
	"echo \"See the attached.\" | mutt -s \"Output %s\" -a %s -- %s",
		 folder_name,
		 attachment_filename,
		 email_address );

	system( sys_string );
	document_close();

	return 0;
}

void output_dictionary_list_to_file(
			FILE *file, 
			LIST *data_dictionary_list,
			LIST *attribute_name_list,
			char delimiter )
{
	DICTIONARY *data_dictionary;

	if ( !data_dictionary_list 
	||   !list_reset( data_dictionary_list ) )
	{
		return;
	}

	
	do {
		data_dictionary = (DICTIONARY *)
				list_get( data_dictionary_list );

		output_dictionary_to_file(	file,
						data_dictionary,
						attribute_name_list,
						delimiter );
	} while( list_next( data_dictionary_list ) );
}


void output_dictionary_to_file(
			FILE *file, 
			DICTIONARY *data_dictionary,
			LIST *attribute_name_list,
			char delimiter )
{
	char current_value[ MAX_INPUT_LINE ];
	char *attribute_name;
	int first_time = 1;
	int row = 1;

	if ( !data_dictionary ) return;

	if ( !list_rewind( attribute_name_list ) ) return;

	do {
		attribute_name = list_get_string( attribute_name_list );

		dictionary_get_index_data_multi(
				current_value,
				data_dictionary,
				attribute_name,
				row++,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

		if ( first_time )
			first_time = 0;
		else
			fprintf( file, "%c", delimiter );

		fprintf( file, "\"%s\"", current_value );

	} while( next_item( attribute_name_list ) );
	fprintf( file, "\n" );
}

void output_heading_to_file(
			FILE *file, 
			LIST *attribute_name_list,
			char delimiter )
{
	char *attribute_name;
	int first_time = 1;

	if ( !list_rewind( attribute_name_list ) ) return;

	do {
		attribute_name = list_get_string( attribute_name_list );

		if ( first_time )
			first_time = 0;
		else
			fprintf( file, "%c", delimiter );

		fprintf( file, "\"%s\"", attribute_name );

	} while( next_item( attribute_name_list ) );

	fprintf( file, "\n" );
}

