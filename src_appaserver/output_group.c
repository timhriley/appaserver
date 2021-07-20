/* $APPASERVER_HOME/src_appaserver/output_group.c		*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you select the group radio	*/
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
#include "role.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "decode_html_post.h"
#include "appaserver.h"
#include "column.h"
#include "dictionary_appaserver.h"
#include "attribute.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define	QUEUE_TOP_BOTTOM_LINES		50
#define FILENAME_STEM			"group_count"

/* Structures */
/* ---------- */
typedef struct{
	int total_count;
	double average;
	double total_sum;
	boolean exists_float_datatype;
	char *float_attribute_name;
} TOTAL_COUNT;

/* Prototypes */
/* ---------- */
TOTAL_COUNT *total_count_new(
			void );

char *get_total_delimiter_list(
			int attribute_list_length,
			char delimiter );

TOTAL_COUNT *get_total_count(
			char *application_name,
			char *folder_name,
			char *where_clause,
			LIST *attribute_float_list );

void output_related_folder(
			LIST *done_folder_name_list,
			char *application_name,
			RELATED_FOLDER *related_folder,
			pid_t process_id,
			char *document_root_directory,
			char *where_clause,
			char *folder_name,
			TOTAL_COUNT *total_count );

FILE *get_input_pipe(	char *application_name,
			LIST *foreign_attribute_name_list,
			char *folder_name,
			char *where_clause,
			TOTAL_COUNT *total_count );

char *get_justify_comma_list( int attribute_list_length );

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name;
	char *login_name;
	char *role_name;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *original_post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	APPASERVER *appaserver;
	QUERY *query;
	char buffer[ 128 ];
	char title[ QUERY_WHERE_BUFFER ];
	DICTIONARY_APPASERVER *dictionary_appaserver;
	ROLE *role;
	boolean override_row_restrictions;
	LIST *exclude_attribute_name_list;
	RELATED_FOLDER *related_folder;
	LIST *done_folder_name_list;
	pid_t process_id = getpid();
	TOTAL_COUNT *total_count;
	char *full_name_only;
	char *street_address_only = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
			 "Usage: %s ignored folder login_name role\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	folder_name = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];

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
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	role = role_new( application_name, role_name );

	appaserver =
		appaserver_folder_load_new(
			application_name,
			folder_name );

	exclude_attribute_name_list =
		appaserver_get_exclude_attribute_name_list(
			appaserver->folder->attribute_list,
			"lookup" );

	if ( appaserver->folder->row_level_non_owner_forbid )
	{
		list_append_pointer(
			exclude_attribute_name_list,
			"login_name" );
	}

	appaserver->folder->mto1_related_folder_list =
	appaserver_remove_attribute_name_list_from_related_folder_list(
		appaserver->folder->mto1_related_folder_list,
		exclude_attribute_name_list );

	full_name_only =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_login_name_full_name(
			&street_address_only,
			login_name );

	query =
		query_simple_new(
			dictionary_appaserver->query_dictionary,
			login_name,
			full_name_only,
			street_address_only,
			appaserver->folder,
			(LIST *)0 /* ignore_attribute_name_list */ );

	if ( !query )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_simple_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !query->query_output )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: query_output is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( total_count =
			get_total_count(
				application_name,
				appaserver->folder->folder_name,
				query->query_output->where_clause,
				appaserver->folder->attribute_float_list ) ) )
	{
		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
		printf(
		"<h3>Warning: there are no rows selected.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !list_rewind( appaserver->folder->mto1_related_folder_list ) )
	{
		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
		printf(
		"<h3>Warning: there are no groups to be displayed.</h3>\n" );
		document_close();
		exit( 0 );
	}

	sprintf(title, 
		"Group %s: %s",
		format_initial_capital( buffer, folder_name ),
		query_get_display_where_clause(
			query->query_output->where_clause,
			application_name,
			folder_name,
			1 ) );

	document = document_new( title, application_name );
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

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	done_folder_name_list = list_new();

	do {
		related_folder =
			list_get_pointer(
				appaserver->folder->mto1_related_folder_list );

		output_related_folder(
			done_folder_name_list,
			application_name,
			related_folder,
			process_id,
			appaserver_parameter_file->document_root,
			query->query_output->where_clause,
			appaserver->folder->folder_name,
			total_count );

	} while( list_next( appaserver->folder->mto1_related_folder_list ) );

	document_close();
	return 0;
}

void output_related_folder(
			LIST *done_folder_name_list,
			char *application_name,
			RELATED_FOLDER *related_folder,
			pid_t process_id,
			char *document_root_directory,
			char *where_clause,
			char *folder_name,
			TOTAL_COUNT *total_count )
{
	LIST *foreign_attribute_name_list;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	FILE *table_output_pipe;
	FILE *file_output_pipe;
	FILE *output_file;
	char *output_filename;
	char *ftp_filename;
	char title[ 128 ];
	char *justify_comma_list;
	char sys_string[ 1024 ];
	char output_file_unique_key[ 128 ];
	int attribute_list_length;
	APPASERVER_LINK_FILE *appaserver_link_file;

	foreign_attribute_name_list =
		related_folder_foreign_attribute_name_list(
		   	folder_get_primary_attribute_name_list(
				related_folder->folder->
					attribute_list ),
		   	related_folder->related_attribute_name,
			related_folder->folder_foreign_attribute_name_list );

	if ( related_folder->related_attribute_name
	&&   *related_folder->related_attribute_name
	&&   strcmp(	related_folder->
				related_attribute_name,
			"null" ) != 0 )
	{
		sprintf( title,
			 "%s: %s (%s)",
			 folder_name,
		 	 related_folder->folder->folder_name,
			 related_folder->related_attribute_name );

		sprintf( output_file_unique_key,
			 "%s_%s_%s", 
			 FILENAME_STEM,
		 	 related_folder->folder->folder_name,
			 related_folder->related_attribute_name );
	}
	else
	{
		sprintf( title,
			 "%s: %s",
			 folder_name,
		 	 related_folder->folder->folder_name );

		sprintf( output_file_unique_key,
			 "%s_%s",
			 FILENAME_STEM,
		 	 related_folder->folder->folder_name );
	}

	if ( list_exists_string(
		output_file_unique_key,
		done_folder_name_list ) )
	{
		return;
	}
	else
	{
		list_append_pointer(
			done_folder_name_list,
			strdup( output_file_unique_key ) );
	}

	format_initial_capital( title, title );

	attribute_list_length = list_length( foreign_attribute_name_list );

	justify_comma_list = get_justify_comma_list( attribute_list_length );

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			output_file_unique_key /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 output_filename );
	}

	if ( !total_count->exists_float_datatype )
	{
		fprintf( output_file,
			"%s\n%s,Count,Percent\n",
		 	title,
		 	format_initial_capital(
				input_buffer,
				list_display( foreign_attribute_name_list ) ) );
	}
	else
	{
		fprintf( output_file,
			"%s\n%s,Count,Percent,Avg(%s),Sum(%s)\n",
		 	title,
		 	format_initial_capital(
				input_buffer,
				list_display( foreign_attribute_name_list ) ),
			 total_count->float_attribute_name,
			 total_count->float_attribute_name );

	}

	fclose( output_file );

	input_pipe =
		get_input_pipe(
			application_name,
			foreign_attribute_name_list,
		 	folder_name,
		 	where_clause,
			total_count );

	if ( !total_count->exists_float_datatype )
	{
		sprintf( sys_string,
		 "queue_top_bottom_lines.e %d				|"
		 "html_table.e '^^%s' '%s,count,percent' '%c' '%s'	 ", 
		 	QUEUE_TOP_BOTTOM_LINES,
		 	title,
		 	list_display( foreign_attribute_name_list ),
		 	FOLDER_DATA_DELIMITER,
		 	justify_comma_list );
	}
	else
	{
		char heading[ 256 ];

		sprintf( heading,
			 "%s,count,percent,average<br>%s,sum<br>%s",
		 	 list_display( foreign_attribute_name_list ),
			 total_count->float_attribute_name,
			 total_count->float_attribute_name );

		sprintf( sys_string,
		 "queue_top_bottom_lines.e %d		|"
		 "html_table.e '^^%s' '%s' '%c' '%s'	 ", 
		 	QUEUE_TOP_BOTTOM_LINES,
		 	title,
			heading,
		 	FOLDER_DATA_DELIMITER,
		 	justify_comma_list );
	}

	table_output_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
		 "double_quote_comma_delimited.e '%c' >> %s",
		 FOLDER_DATA_DELIMITER,
		 output_filename );

	file_output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( table_output_pipe, "%s\n", input_buffer );
		fprintf( file_output_pipe, "%s\n", input_buffer );
	}

	fprintf(	table_output_pipe,
			"%s%d\n",
			get_total_delimiter_list(
				attribute_list_length,
				FOLDER_DATA_DELIMITER ),
			total_count->total_count );

/*
	fprintf(	file_output_pipe,
			"%s%d\n",
			get_total_delimiter_list(
				attribute_list_length,
				',' ),
			total_count->total_count );
*/

	pclose( input_pipe );
	pclose( table_output_pipe );
	pclose( file_output_pipe );

	fflush( stdout );
	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	fflush( stdout );

}

char *get_total_delimiter_list(	int attribute_list_length,
				char delimiter )
{
	static char total_delimiter_list[ 1024 ];
	char *ptr = total_delimiter_list;
	int i;

	*total_delimiter_list = '\0';

	for( i = 0; i < attribute_list_length; i++ )
	{
		if ( i == 0 )
		{
			ptr += sprintf( ptr, "Row Count%c", delimiter );
		}
		else
		{
			ptr += sprintf( ptr, "%c", delimiter );
		}
	}

	return total_delimiter_list;
}

char *get_justify_comma_list( int attribute_list_length )
{
	static char justify_comma_list[ 1024 ];
	char *ptr = justify_comma_list;
	int i;

	*justify_comma_list = '\0';

	for( i = 0; i < attribute_list_length; i++ )
	{
		if ( i ) ptr += sprintf( ptr, "," );
		ptr += sprintf( ptr, "left" );
	}

	ptr += sprintf( ptr, ",right,right" );
	return justify_comma_list;
}

FILE *get_input_pipe(	char *application_name,
			LIST *foreign_attribute_name_list,
			char *folder_name,
			char *where_clause,
			TOTAL_COUNT *total_count )
{
	char sys_string[ QUERY_WHERE_BUFFER ];
	char select[ 512 ];

	if ( !total_count->exists_float_datatype )
	{
		sprintf( select,
		 	"%s,count(*),(count(*) / %d) * 100.0",
		 	list_display( foreign_attribute_name_list ),
		 	total_count->total_count );
	}
	else
	{
		sprintf(select,
		 	"%s,count(*),(count(*) / %d) * 100.0,	"
			"avg(%s),sum(%s)			",
		 	list_display( foreign_attribute_name_list ),
		 	total_count->total_count,
			total_count->float_attribute_name,
			total_count->float_attribute_name );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			group=\"%s\"		 "
		 "			order=\"count(*) desc\"	 ",
		 application_name,
		 select,
		 folder_name,
		 where_clause,
		 list_display( foreign_attribute_name_list ) );

/*
fprintf( stderr, "%s\n",sys_string );
*/

	return popen( sys_string, "r" );

}

TOTAL_COUNT *get_total_count(
			char *application_name,
			char *folder_name,
			char *where_clause,
			LIST *attribute_float_list )
{
	char sys_string[ QUERY_WHERE_BUFFER ];
	ATTRIBUTE *first_attribute = {0};
	TOTAL_COUNT *total_count;
	char select[ 256 ];
	char *input_record;
	char piece_buffer[ 128 ];

	total_count = total_count_new();

	if ( list_length( attribute_float_list ) )
	{
		first_attribute =
				list_get_first_pointer(
					attribute_float_list );

		sprintf( select,
			 "count(*),avg(%s),sum(%s)",
			 first_attribute->attribute_name,
			 first_attribute->attribute_name );

		total_count->exists_float_datatype = 1;

		total_count->float_attribute_name =
			first_attribute->attribute_name;
	}
	else
	{
		strcpy( select, "count(*)" );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder_name,
		 where_clause );

	input_record = pipe2string( sys_string );

	if ( !input_record || !*input_record )
	{
		return total_count;
	}

	total_count->total_count = atoi( piece(	piece_buffer,
						FOLDER_DATA_DELIMITER,
						input_record,
						0 ) );

	if ( total_count->exists_float_datatype )
	{
		total_count->average =
				atof( piece(	piece_buffer,
						FOLDER_DATA_DELIMITER,
						input_record,
						1 ) );

		total_count->total_sum =
				atof( piece(	piece_buffer,
						FOLDER_DATA_DELIMITER,
						input_record,
						2 ) );

	}

	return total_count;

}

TOTAL_COUNT *total_count_new( void )
{
	TOTAL_COUNT *total_count;

	total_count = (TOTAL_COUNT *)calloc( 1, sizeof( TOTAL_COUNT ) );
	return total_count;
}
