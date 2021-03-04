/* ---------------------------------------------------	*/
/* src_creel/guide_compliance_mail_merge.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "html_table.h"
#include "hashtbl.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "creel_library.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, text_file, table, labels };

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING			20
#define DEFAULT_OUTPUT_MEDIUM			table
#define DEFAULT_DAYS_OFFSET			"10"

/* Prototypes */
/* ---------- */
void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *days_offset,
				char *process_name,
				char *mail_merge_letter );

enum output_medium get_output_medium(
				char *output_medium_string );

void guide_compliance_mail_merge_output_table(
				char *application_name,
				char *days_offset,
				char *mail_merge_letter,
				char *process_name );

void guide_compliance_mail_merge_output_text_file(
				char *application_name,
				char *document_root_directory,
				char *days_offset,
				char *mail_merge_letter,
				char *process_name );

void guide_compliance_mail_merge_output_labels(
				char *application_name,
				char *document_root_directory,
				char *days_offset,
				char *mail_merge_letter,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	enum output_medium output_medium;
	char *days_offset;
	char *output_medium_string;
	char *mail_merge_letter;
	DOCUMENT *document;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name output_medium mail_merge_letter days_offset\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	output_medium_string = argv[ 3 ];
	mail_merge_letter = argv[ 4 ];
	days_offset = argv[ 5 ];

	if ( !*days_offset ) days_offset = DEFAULT_DAYS_OFFSET;

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( !*mail_merge_letter
	||   strcmp( mail_merge_letter, "select" ) == 0
	||   strcmp( mail_merge_letter, "mail_merge_letter" ) == 0 )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );
		printf( "<h3>Please select a mail merge letter.</h3>\n" );
		document_close();
		exit( 0 );
	}

	output_medium = get_output_medium( output_medium_string );

	if ( output_medium == table )
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

		guide_compliance_mail_merge_output_table(
			application_name,
			days_offset,
			mail_merge_letter,
			process_name );

		document_close();
	}
	else
	if ( output_medium == text_file )
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

		guide_compliance_mail_merge_output_text_file(
			application_name,
			appaserver_parameter_file->
				document_root,
			days_offset,
			mail_merge_letter,
			process_name );

		document_close();
	}
	else
	if ( output_medium == labels )
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

		guide_compliance_mail_merge_output_labels(
			application_name,
			appaserver_parameter_file->
				document_root,
			days_offset,
			mail_merge_letter,
			process_name );

		document_close();
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void guide_compliance_mail_merge_output_table(
			char *application_name,
			char *days_offset,
			char *mail_merge_letter,
			char *process_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char title[ 512 ];
	char sub_title[ 512 ];
	int count = 0;
	GUIDE_ANGLER guide_angler;

	get_title_and_sub_title(
			title,
			sub_title,
			days_offset,
			process_name,
			mail_merge_letter );

	html_table = new_html_table( title, sub_title );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	
	heading_list = list_new();
	list_append_pointer( heading_list, "Angler Name" );
	list_append_pointer( heading_list, "City" );
	list_append_pointer( heading_list, "Phone" );
	list_append_pointer( heading_list, "Email" );
	list_append_pointer( heading_list, "Permit" );
	list_append_pointer( heading_list, "Issued" );
	list_append_pointer( heading_list, "Expired" );
		
	html_table_set_heading_list( html_table, heading_list );
	
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	while( creel_library_get_expired_guide_angler(
				&guide_angler,
				application_name,
				days_offset,
				mail_merge_letter,
				1 /* yes, order_by_expiration_date */ ) )
	{
		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
					html_table->heading_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->justify_list );
		}

		html_table_set_data(
			html_table->data_list,
			strdup( guide_angler.guide_angler_name ) );

		html_table_set_data(
			html_table->data_list,
			strdup( guide_angler.city ) );

		html_table_set_data(
			html_table->data_list,
			strdup( guide_angler.phone_number ) );

		html_table_set_data(
			html_table->data_list,
			strdup( guide_angler.email_address ) );

		html_table_set_data(
			html_table->data_list,
			strdup( guide_angler.permit_code ) );

		html_table_set_data(
			html_table->data_list,
			strdup( guide_angler.issued_date ) );

		html_table_set_data(
			html_table->data_list,
			strdup( guide_angler.expiration_date ) );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new_list();
	}

	printf( "<tr><td colspan=99>\n" );
	html_table_close();

} /* guide_compliance_mail_merge_output_table() */

void guide_compliance_mail_merge_output_text_file(
			char *application_name,
			char *document_root_directory,
			char *days_offset,
			char *mail_merge_letter,
			char *process_name )
{
	char *output_filename;
	char *ftp_filename;
	pid_t process_id = getpid();
	FILE *output_file;
	char title[ 512 ];
	char sub_title[ 512 ];
	GUIDE_ANGLER guide_angler;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

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


	get_title_and_sub_title(
			title,
			sub_title,
			days_offset,
			process_name,
			mail_merge_letter );

/*
	sprintf( output_filename, 
		 OUTPUT_TEMPLATE,
		 appaserver_mount_point,
		 application_name, 
		 process_id );
*/
	
	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf(
			"<H2>ERROR: Cannot open output file %s\n",
				output_filename );
		document_close();
		exit( 1 );
	}

	while( creel_library_get_expired_guide_angler(
				&guide_angler,
				application_name,
				days_offset,
				mail_merge_letter,
				0 /* no, order_by_expiration_date */ ) )
	{
		fprintf( output_file,
			 "guide_angler_name=%s\n"
			 "street_address=%s\n"
			 "city=%s\n"
			 "state=%s\n"
			 "zip_code=%s\n"
			 "permit=%s\n"
			 "issued_date=%s\n"
			 "expiration_date=%s\n",
			 guide_angler.guide_angler_name,
			 guide_angler.street_address,
			 guide_angler.city,
			 guide_angler.state_code,
			 guide_angler.zip_code,
			 guide_angler.permit_code,
			 guide_angler.issued_date,
			 guide_angler.expiration_date );
	}

	fclose( output_file );

	printf( "<h1>%s<br></h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	fflush( stdout );
	printf( "</h2>\n" );
		
	appaserver_library_output_ftp_prompt(
			ftp_filename,
	"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* guide_compliance_mail_merge_output_text_file() */

void guide_compliance_mail_merge_output_labels(
			char *application_name,
			char *document_root_directory,
			char *days_offset,
			char *mail_merge_letter,
			char *process_name )
{
	char *output_filename;
	char *ftp_filename;
	pid_t process_id = getpid();
	FILE *output_file;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	GUIDE_ANGLER guide_angler;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

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


	get_title_and_sub_title(
			title,
			sub_title,
			days_offset,
			process_name,
			mail_merge_letter );

/*
	sprintf( output_filename, 
		 OUTPUT_TEMPLATE,
		 appaserver_mount_point,
		 application_name, 
		 process_id );
*/
	
	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf(
			"<H2>ERROR: Cannot open output file %s\n",
				output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	sprintf( sys_string,
		 "label.sh > %s",
		 output_filename );

	output_pipe = popen( sys_string, "w" );

	while( creel_library_get_expired_guide_angler(
				&guide_angler,
				application_name,
				days_offset,
				mail_merge_letter,
				0 /* no, order_by_expiration_date */ ) )
	{
		fprintf( output_pipe,
			 "%s||%s||%s|%s|%s\n",
			 guide_angler.guide_angler_name,
			 guide_angler.street_address,
			 guide_angler.city,
			 guide_angler.state_code,
			 guide_angler.zip_code );
	}

	pclose( output_pipe );

	printf( "<h1>%s Labels<br></h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	fflush( stdout );
	printf( "</h2>\n" );
		
	appaserver_library_output_ftp_prompt(
			ftp_filename,
	"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* guide_compliance_mail_merge_output_labels() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *days_offset,
			char *process_name,
			char *mail_merge_letter )
{
	char *begin_date_string;
	char *end_date_string;

	sprintf( title, "%s %s", mail_merge_letter, process_name );
	format_initial_capital( title, title );

	creel_library_get_begin_end_dates(
				&begin_date_string,
				&end_date_string,
				days_offset,
				mail_merge_letter );

	if ( strcmp( mail_merge_letter, "about_to_expire" ) == 0 )
	{
		sprintf(	sub_title,
				"Up Until: %s",
				end_date_string );
	}
	else
	{
		sprintf(	sub_title,
				"As Of: %s",
				begin_date_string );
	}
} /* get_title_and_sub_title() */

enum output_medium get_output_medium( char *output_medium_string )
{
	if ( strcmp( output_medium_string, "output_medium" ) == 0
	||   !*output_medium_string )
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
	else
	if ( strcmp( output_medium_string, "text_file" ) == 0 )
	{
		return text_file;
	}
	else
	if ( strcmp( output_medium_string, "table" ) == 0 )
	{
		return table;
	}
	else
	if ( strcmp( output_medium_string, "stdout" ) == 0 )
	{
		return output_medium_stdout;
	}
	else
	if ( strcmp( output_medium_string, "labels" ) == 0 )
	{
		return labels;
	}
	else
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
} /* get_output_medium_string() */

