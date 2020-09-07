/* src_appaserver/output_email.c				*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you put an email address	*/
/* in the lookup form.						*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
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

/* appaserver_link_file */

/* Constants */
/* --------- */
#define DELIMITER			','
#define ATTACHMENT_FILENAME_TEMPLATE	"%s/%s_%s_%d.csv"

/* Prototypes */
/* ---------- */
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
	char *login_name, *application_name, *session, *folder_name;
	char *role_name;
	char *email_address;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *post_dictionary;
	FOLDER *folder;
	ROLE *role;
	LIST *row_dictionary_list = {0};
	QUERY *query;
	LIST *ignore_attribute_name_list;
	LIST *attribute_name_list;
	FILE *file;
	DOCUMENT *document;
	char sys_string[ 1024 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	LIST *mto1_isa_related_folder_list;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	DICTIONARY *ignore_dictionary;
	DICTIONARY *query_dictionary;
	char attachment_filename[ 128 ];

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

		post_dictionary = 
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );

		if ( ! ( dictionary_appaserver =
				dictionary_appaserver_new(
					post_dictionary,
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

		post_dictionary = dictionary_appaserver->
					working_post_dictionary;

		ignore_dictionary = dictionary_appaserver->
					ignore_dictionary;

		query_dictionary = dictionary_appaserver->
					query_dictionary;
	}
	else
	{
		post_dictionary = (DICTIONARY *)0;
		ignore_dictionary = (DICTIONARY *)0;
		query_dictionary = (DICTIONARY *)0;
	}

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
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	email_address = dictionary_get_index_zero(
				query_dictionary,
				EMAIL_OUTPUT_NAME );

	if ( !email_address || !*email_address )
	{
		printf( "<p>Error in %s: cannot get email address from %s\n",
			 argv[ 0 ],
			 dictionary_display( post_dictionary ) );
		document_close();
		exit( 1 );
	}

	ignore_attribute_name_list = list_new();

	folder = folder_new_folder(
				application_name,
				session,
				folder_name );

	role = role_new_role(	application_name,
				role_name );

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
			application_name,
			folder->session,
			folder->folder_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	mto1_isa_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			session,
			folder_name,
			(char *)0 /* role_name */,
			1 /* isa_flag */,
			related_folder_recursive_all,
			0 /* dont override_row_restrictions */,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	folder->attribute_list =
		attribute_get_attribute_list(
		application_name,
		folder_name,
		(char *)0 /* attribute_name */,
		mto1_isa_related_folder_list,
		role_name );

	attribute_name_list =
		folder_get_attribute_name_list( folder->attribute_list );

	query = query_edit_table_new(
			query_dictionary,
			application_name,
			login_name,
			folder->folder_name,
			role );

	row_dictionary_list =
		query_row_dictionary_list(
				query->folder->application_name,
				query->query_output->select_clause,
				query->query_output->from_clause,
				query->query_output->where_clause,
				query->query_output->order_clause,
				query->max_rows,
				query->folder->append_isa_attribute_list,
				query->login_name );

	list_append_string_list(
		ignore_attribute_name_list,
		appaserver_library_get_no_display_pressed_attribute_name_list(
					ignore_dictionary, 
					attribute_name_list ) );

	attribute_name_list = 
		list_subtract( 	attribute_name_list, 
				ignore_attribute_name_list );

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

	output_dictionary_list_to_file(	file, 
					row_dictionary_list,
					attribute_name_list,
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
	exit( 0 );

} /* main() */


void output_dictionary_list_to_file(	FILE *file, 
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
} /* output_dictionary_list_to_file() */


void output_dictionary_to_file( FILE *file, 
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
} /* output_dictionary_to_file() */

void output_heading_to_file(	FILE *file, 
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
} /* output_heading_to_file() */

