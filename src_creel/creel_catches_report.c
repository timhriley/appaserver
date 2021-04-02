/* ---------------------------------------------------	*/
/* src_creel/creel_catches_report.c			*/
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
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "easycharts.h"
#include "creel_library.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, text_file, table, stacked_chart };

/* Constants */
/* --------- */
/*
#define EASYCHARTS_WIDTH			750
#define EASYCHARTS_HEIGHT			470
*/
#define LOCAL_EASYCHARTS_WIDTH		730
#define ROWS_BETWEEN_HEADING		20
#define DEFAULT_OUTPUT_MEDIUM		table
#define FILENAME_STEM_DETAIL		"creel_catches_detail"
#define FILENAME_STEM_SUMMARY		"creel_catches_summary"
#define FILENAME_STEM_TOTAL		"creel_catches_total"

#define DETAIL_TEMPORARY_TEMPLATE "%s/creel_catches_detail_temporary_%d.csv"
#define SUMMARY_TEMPORARY_TEMPLATE "%s/creel_catches_summary_temporary_%d.csv"
#define TOTAL_TEMPORARY_TEMPLATE "%s/creel_catches_total_temporary_%d.csv"

/* Prototypes */
/* ---------- */
void creel_catches_report_stacked_chart_get_sample_values(
				char **sample_values_kept_list_string,
				char **sample_values_released_list_string,
				char **sample_values_label_list_string,
				char *summary_temporary_filename );

void creel_catches_report_generate_data(
				char *detail_temporary_filename,
				char *summary_temporary_filename,
				char *total_temporary_filename,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				boolean order_by_caught,
				boolean summary_only,
				char *fishing_purpose );

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_date_string,
				char *end_date_string,
				char *process_name,
				char *fishing_purpose );

enum output_medium get_output_medium(
				char *output_medium_string );

void creel_catches_report_output_table(
				char *begin_date_string,
				char *end_date_string,
				char *detail_temporary_filename,
				char *summary_temporary_filename,
				char *total_temporary_filename,
				boolean summary_only,
				char *process_name,
				char *fishing_purpose );

void creel_catches_report_output_stacked_chart(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *summary_temporary_filename,
				char *document_root_directory,
				char *process_name,
				char *fishing_purpose );

void creel_catches_report_output_text_file(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *document_root_directory,
				char *detail_temporary_filename,
				char *summary_temporary_filename,
				char *total_temporary_filename,
				enum output_medium output_medium,
				char *process_name,
				char *fishing_purpose );

void get_total_fishing_hours_times_party_count(
				double *total_fishing_hours_times_party_count,
				int *fishing_trips_count,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *fishing_purpose );

int main( int argc, char **argv )
{
	char *application_name;
	char *fishing_area_list_string;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	enum output_medium output_medium;
	char *fishing_purpose;
	boolean order_by_caught;
	boolean summary_only;
	char *begin_date_string;
	char *end_date_string;
	char *family_list_string;
	char *genus_list_string;
	char *species_list_string;
	char *output_medium_string;
	pid_t process_id = getpid();
	char detail_temporary_filename[ 128 ];
	char summary_temporary_filename[ 128 ];
	char total_temporary_filename[ 128 ];

	if ( argc != 13 )
	{
		fprintf( stderr,
"Usage: %s application process_name fishing_area begin_date end_date family genus species order_by_caught_yn summary_only_yn fishing_purpose output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	fishing_area_list_string = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	end_date_string = argv[ 5 ];

	if ( !*end_date_string || strcmp( end_date_string, "end_date" ) == 0 )
		end_date_string = begin_date_string;

	family_list_string = argv[ 6 ];
	genus_list_string = argv[ 7 ];
	species_list_string = argv[ 8 ];
	order_by_caught = (*argv[ 9 ] == 'y');
	summary_only = (*argv[ 10 ] == 'y');
	fishing_purpose = argv[ 11 ];
	output_medium_string = argv[ 12 ];

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

	appaserver_parameter_file = appaserver_parameter_file_new();

	output_medium = get_output_medium( output_medium_string );

	sprintf( detail_temporary_filename,
		 DETAIL_TEMPORARY_TEMPLATE,
		 appaserver_parameter_file->appaserver_data_directory,
		 process_id );

	sprintf( summary_temporary_filename,
		 SUMMARY_TEMPORARY_TEMPLATE,
		 appaserver_parameter_file->appaserver_data_directory,
		 process_id );

	sprintf( total_temporary_filename,
		 TOTAL_TEMPORARY_TEMPLATE,
		 appaserver_parameter_file->appaserver_data_directory,
		 process_id );

	creel_catches_report_generate_data(
			detail_temporary_filename,
			summary_temporary_filename,
			total_temporary_filename,
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area_list_string,
			family_list_string,
			genus_list_string,
			species_list_string,
			order_by_caught,
			summary_only,
			fishing_purpose );

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

		creel_catches_report_output_table(
			begin_date_string,
			end_date_string,
			detail_temporary_filename,
			summary_temporary_filename,
			total_temporary_filename,
			summary_only,
			process_name,
			fishing_purpose );

		document_close();
	}
	else
	if ( output_medium == stacked_chart )
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

		creel_catches_report_output_stacked_chart(
			application_name,
			begin_date_string,
			end_date_string,
			summary_temporary_filename,
			appaserver_parameter_file->
				document_root,
			process_name,
			fishing_purpose );

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

		creel_catches_report_output_text_file(
			application_name,
			begin_date_string,
			end_date_string,
			appaserver_parameter_file->
				document_root,
			detail_temporary_filename,
			summary_temporary_filename,
			total_temporary_filename,
			output_medium,
			process_name,
			fishing_purpose );

		document_close();
	}
	else
	if ( output_medium == output_medium_stdout )
	{
		creel_catches_report_output_text_file(
			application_name,
			begin_date_string,
			end_date_string,
			appaserver_parameter_file->
				document_root,
			detail_temporary_filename,
			summary_temporary_filename,
			total_temporary_filename,
			output_medium,
			process_name,
			fishing_purpose );
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void creel_catches_report_output_table(
			char *begin_date_string,
			char *end_date_string,
			char *detail_temporary_filename,
			char *summary_temporary_filename,
			char *total_temporary_filename,
			boolean summary_only,
			char *process_name,
			char *fishing_purpose )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char title[ 512 ];
	char sub_title[ 512 ];
	char input_buffer[ 128 ];
	char piece_buffer[ 128 ];
	FILE *input_file;
	int count = 0;

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string,
			process_name,
			fishing_purpose );

	html_table = new_html_table( title, sub_title );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	
	html_table->number_left_justified_columns = 9;
	html_table->number_right_justified_columns = 99;

	if ( !summary_only )
	{
		heading_list = list_new();
		list_append_pointer( heading_list, "Fishing Purpose" );
		list_append_pointer( heading_list, "Date" );
		list_append_pointer( heading_list, "Interview" );
		list_append_pointer( heading_list, "Family" );
		list_append_pointer( heading_list, "Genus" );
		list_append_pointer( heading_list, "Species" );
		list_append_pointer( heading_list, "Fishing Area" );
		list_append_pointer( heading_list, "Common" );
		list_append_pointer( heading_list, "Code" );
		list_append_pointer( heading_list, "Caught" );
		list_append_pointer( heading_list, "Kept" );
		list_append_pointer( heading_list, "Released" );
		list_append_pointer( heading_list, "Hours Fishing" );
		list_append_pointer( heading_list, "Party Count" );
		list_append_pointer( heading_list, "Effort" );
		
		html_table_set_heading_list( html_table, heading_list );
	
		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

		if ( ! ( input_file =
				fopen( detail_temporary_filename, "r" ) ) )
		{
			fprintf( stderr,
			 	"ERROR in %s/%s()/%d: cannot open %s\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	detail_temporary_filename );
			exit( 1 );
		}

		while( get_line( input_buffer, input_file ) )
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

			/* Fishing Purpose */
			/* --------------- */
			piece( piece_buffer, '|', input_buffer, 0 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Census Date */
			/* ----------- */
			piece( piece_buffer, '|', input_buffer, 1 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Interview Number */
			/* ---------------- */
			piece( piece_buffer, '|', input_buffer, 2 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Family */
			/* ------ */
			piece( piece_buffer, '|', input_buffer, 3 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Genus */
			/* ----- */
			piece( piece_buffer, '|', input_buffer, 4 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Species */
			/* ------- */
			piece( piece_buffer, '|', input_buffer, 5 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Fishing_Area */
			/* ------------ */
			piece( piece_buffer, '|', input_buffer, 6 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Common */
			/* ------ */
			piece( piece_buffer, '|', input_buffer, 7 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Florida State Code */
			/* ------------------ */
			piece( piece_buffer, '|', input_buffer, 8 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Caught */
			/* ------ */
			piece( piece_buffer, '|', input_buffer, 9 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Kept */
			/* ---- */
			piece( piece_buffer, '|', input_buffer, 10 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Released */
			/* -------- */
			piece( piece_buffer, '|', input_buffer, 11 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Hours Fishing */
			/* ------------- */
			piece( piece_buffer, '|', input_buffer, 12 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Party Count */
			/* ----------- */
			piece( piece_buffer, '|', input_buffer, 13 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Effort */
			/* -------- */
			piece( piece_buffer, '|', input_buffer, 14 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

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
		fclose( input_file );
	}

	if ( ! ( input_file = fopen( summary_temporary_filename, "r" ) ) )
	{
		fprintf( stderr,
		 	"ERROR in %s/%s()/%d: cannot open %s\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	summary_temporary_filename );
		exit( 1 );
	}

	printf( "<h1>Per Species Per Area</h1>\n" );

	heading_list = list_new();
	list_append_pointer( heading_list, "" );
	list_append_pointer( heading_list, "" );
	list_append_pointer( heading_list, "" );
	list_append_pointer( heading_list, "Family" );
	list_append_pointer( heading_list, "Genus" );
	list_append_pointer( heading_list, "Species" );
	list_append_pointer( heading_list, "Fishing Area" );
	list_append_pointer( heading_list, "Common" );
	list_append_pointer( heading_list, "Code" );
	list_append_pointer( heading_list, "Caught" );
	list_append_pointer( heading_list, "Kept" );
	list_append_pointer( heading_list, "Released" );
		
	html_table_set_heading_list( html_table, heading_list );
	
	html_table_output_data_heading(
			html_table->heading_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->justify_list );

	count = 0;

	while( get_line( input_buffer, input_file ) )
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

		/* Fishing Purpose (which is empty) */
		/* -------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		/* Interview Number (which is empty) */
		/* --------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		/* Date (which is empty) */
		/* --------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		/* Family */
		/* ------ */
		piece( piece_buffer, '|', input_buffer, 0 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Genus */
		/* ----- */
		piece( piece_buffer, '|', input_buffer, 1 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Species */
		/* ------- */
		piece( piece_buffer, '|', input_buffer, 2 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Fishing Area */
		/* ------------ */
		piece( piece_buffer, '|', input_buffer, 3 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Common */
		/* ------ */
		piece( piece_buffer, '|', input_buffer, 4 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Florida State Code */
		/* ------------------ */
		piece( piece_buffer, '|', input_buffer, 5 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Caught */
		/* ------ */
		piece( piece_buffer, '|', input_buffer, 6 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Kept */
		/* ---- */
		piece( piece_buffer, '|', input_buffer, 7 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Released */
		/* -------- */
		piece( piece_buffer, '|', input_buffer, 8 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new_list();
	}
	fclose( input_file );

	if ( ! ( input_file = fopen( total_temporary_filename, "r" ) ) )
	{
		fprintf( stderr,
		 	"ERROR in %s/%s()/%d: cannot open %s\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	total_temporary_filename );
		exit( 1 );
	}

	html_table_close();

	printf( "<h1>Total</h1>\n" );

	printf( "<tr><td colspan=99>\n" );
	printf( "<table border=1>\n" );
	printf( "<tr>\n" );
	printf( "<th align=left></th>\n" );
	printf( "<th align=left>Caught</th>\n" );
	printf( "<th align=right>Kept</th>\n" );
	printf( "<th align=right>Released</th>\n" );
	printf( "<th align=right>Effort</th>\n" );
	printf( "<th align=right>Catch Per Unit Effort<sup>*</sup></th>\n" );
	printf( "<th align=right>Minutes Between Catch<sup>**</sup></th>\n" );
	printf( "<th align=right>Trips Count</th>\n" );
	printf( "<th align=right>Catch Records Count</th>\n" );

	if( get_line( input_buffer, input_file ) )
	{
		printf( "<tr>\n" );
		printf( "<td>Total</td>\n" );

		/* Caught */
		/* ------ */
		piece( piece_buffer, '|', input_buffer, 0 );
		printf( "<td align=right>%s</td>\n", piece_buffer );

		/* Kept */
		/* ---- */
		piece( piece_buffer, '|', input_buffer, 1 );
		printf( "<td align=right>%s</td>\n", piece_buffer );

		/* Released */
		/* -------- */
		piece( piece_buffer, '|', input_buffer, 2 );
		printf( "<td align=right>%s</td>\n", piece_buffer );

		/* Total Fishing Hours X Party Count */
		/* --------------------------------- */
		piece( piece_buffer, '|', input_buffer, 3 );
		printf( "<td align=right>%s</td>\n", piece_buffer );

		/* Unit Effort */
		/* ----------- */
		piece( piece_buffer, '|', input_buffer, 4 );
		printf( "<td align=right>%s</td>\n", piece_buffer );

		/* Minutes Between Catch */
		/* --------------------- */
		piece( piece_buffer, '|', input_buffer, 5 );
		printf( "<td align=right>%s</td>\n", piece_buffer );

		/* Trip Count */
		/* ---------- */
		piece( piece_buffer, '|', input_buffer, 6 );
		printf( "<td align=right>%s</td>\n", piece_buffer );

		/* Catch Record Count */
		/* ------------------ */
		piece( piece_buffer, '|', input_buffer, 7 );
		printf( "<td align=right>%s</td>\n", piece_buffer );

	}

	fclose( input_file );

	printf(
"<tr><td colspan=99><sup>*</sup>The Catch Per Unit Effort is the number of fish caught per human fishing hour. Catch Per Unit Effort = Caught / (Fishing Hours X Party Count).</td>\n" );

	printf(
"<tr><td colspan=99><sup>**</sup>The Minutes Between Catch is the average number of minutes between catches. Minutes Between Catch = (1 / Catch Per Unit Effort) X 60.</td>\n" );

	html_table_close();

} /* creel_catches_report_output_table() */


void creel_catches_report_output_text_file(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *document_root_directory,
			char *detail_temporary_filename,
			char *summary_temporary_filename,
			char *total_temporary_filename,
			enum output_medium output_medium,
			char *process_name,
			char *fishing_purpose )
{
	char output_sys_string[ 1024 ];
	char *ftp_detail_filename = {0};
	char *ftp_summary_filename = {0};
	char *ftp_total_filename = {0};
	char *output_filename;
	pid_t process_id = getpid();
	FILE *output_pipe;
	char input_buffer[ 128 ];
	FILE *input_file;
	char title[ 512 ];
	char sub_title[ 512 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			(char *)0 /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string,
			process_name,
			fishing_purpose );

	/* Output the detail */
	/* ----------------- */
	if ( ! ( input_file = fopen( detail_temporary_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 detail_temporary_filename );
		exit( 1 );
	}

	if ( output_medium != output_medium_stdout )
	{
		FILE *output_file;

		appaserver_link_file->filename_stem = FILENAME_STEM_DETAIL;

/*
		sprintf( output_filename, 
			 OUTPUT_DETAIL_TEMPLATE,
			 appaserver_mount_point,
			 application_name, 
			 process_id );
*/

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

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		sprintf( output_sys_string,
			 "tr '|' ',' > %s",
			 output_filename );

		ftp_detail_filename =
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

	}
	else
	{
		sprintf( output_sys_string,
			 "tr '|' ','" );
	}

	output_pipe = popen( output_sys_string, "w" );

	fprintf( output_pipe,
		 "#%s Detail\n#%s\n", title, sub_title );

	fprintf( output_pipe,
"Purpose|Date|Interview|Family|Genus|Species|FishingArea|Common|Code|Caught|Kept|Released|HoursFishing|PartyCount|Effort\n" );

	while( get_line( input_buffer, input_file ) )
	{
		fprintf( output_pipe,
			 "%s\n",
			 input_buffer );
	}

	fclose( input_file );
	pclose( output_pipe );

	/* Output the summary */
	/* ------------------ */
	if ( ! ( input_file = fopen( summary_temporary_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 summary_temporary_filename );
		exit( 1 );
	}

	if ( output_medium != output_medium_stdout )
	{
		FILE *output_file;

/*
		sprintf( output_filename, 
			 OUTPUT_SUMMARY_TEMPLATE,
			 appaserver_mount_point,
			 application_name, 
			 process_id );
*/

		appaserver_link_file->filename_stem = FILENAME_STEM_SUMMARY;

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

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		sprintf( output_sys_string,
			 "tr '|' ',' > %s",
			 output_filename );

		ftp_summary_filename =
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

	}
	else
	{
		sprintf( output_sys_string,
			 "tr '|' ','" );
	}

	output_pipe = popen( output_sys_string, "w" );

	fprintf( output_pipe,
		 "#%s Per Species Per Area\n#%s\n", title, sub_title );

	fprintf( output_pipe,
	"Family|Genus|Species|FishingArea|Common|Code|Caught|Kept|Released\n" );

	while( get_line( input_buffer, input_file ) )
	{
		fprintf( output_pipe,
			 "%s\n",
			 input_buffer );
	}

	fclose( input_file );
	pclose( output_pipe );

	/* Output the total */
	/* ---------------- */
	if ( ! ( input_file = fopen( total_temporary_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 total_temporary_filename );
		exit( 1 );
	}

	if ( output_medium != output_medium_stdout )
	{
		FILE *output_file;

		appaserver_link_file->filename_stem = FILENAME_STEM_TOTAL;
/*
		sprintf( output_filename, 
			 OUTPUT_TOTAL_TEMPLATE,
			 appaserver_mount_point,
			 application_name, 
			 process_id );
*/
		
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

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		sprintf( output_sys_string,
			 "tr '|' ',' > %s",
			 output_filename );

		ftp_total_filename =
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

	}
	else
	{
		sprintf( output_sys_string,
			 "tr '|' ','" );
	}

	output_pipe = popen( output_sys_string, "w" );

	fprintf( output_pipe,
		 "#%s Total\n#%s\n", title, sub_title );

	fprintf( output_pipe,
"Caught|Kept|Released|Effort|CatchPerUnitEffort|MinutesBetweenCatch|TripCount|RecordCount\n" );

	while( get_line( input_buffer, input_file ) )
	{
		fprintf( output_pipe,
			 "%s\n",
			 input_buffer );
	}

	fclose( input_file );
	pclose( output_pipe );

	if ( output_medium != output_medium_stdout )
	{
		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h2>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );

		appaserver_library_output_ftp_prompt(
			ftp_detail_filename,
	"Detail: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

		printf( "<br>\n" );

		appaserver_library_output_ftp_prompt(
			ftp_summary_filename,
	"Per Species Per Area: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

		printf( "<br>\n" );

		appaserver_library_output_ftp_prompt(
			ftp_total_filename,
	"Total: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	}

} /* creel_catches_report_output_text_file() */


void get_total_fishing_hours_times_party_count(
			double *total_fishing_hours_times_party_count,
			int *fishing_trips_count,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_area_list_string,
			char *fishing_purpose )
{
	char sys_string[ 4096 ];
	char where[ 4096 ];
	char *where_ptr;
	char input_buffer[ 128 ];
	char piece_buffer[ 128 ];
	double party_count;
	double hours_fishing;
	FILE *input_pipe;
	char *select = "number_of_people_fishing,hours_fishing";

	*fishing_trips_count = 0;
	*total_fishing_hours_times_party_count = 0.0;

	/* ------------------------------------------------------------ */
	/* creel_library_get_fishing_trips_census_date_where_clause()   */
	/* begins with " and"						*/ 
	/* ------------------------------------------------------------ */
	where_ptr = where;
	where_ptr += sprintf( where_ptr, "1 = 1" );

	creel_library_get_fishing_trips_census_date_where_clause(
			where_ptr,
			application_name,
			fishing_area_list_string,
			begin_date_string,
			end_date_string,
			fishing_purpose );

	sprintf(sys_string,
		"get_folder_data	application=%s		"
		"			folder=fishing_trips	"
		"			select=%s		"
		"			where=\"%s\"		",
		application_name,
		select,
		where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		party_count = atof( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		hours_fishing = atof( piece_buffer );

		*total_fishing_hours_times_party_count +=
			( party_count * hours_fishing );

		(*fishing_trips_count)++;
	}

	pclose( input_pipe );

} /* get_total_fishing_hours_times_party_count() */

enum output_medium get_output_medium( char *output_medium_string )
{
	if ( strcmp( output_medium_string, "output_medium" ) == 0
	||   !*output_medium_string )
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
	else
	if ( strcmp( output_medium_string, "text_file" ) == 0
	||   strcmp( output_medium_string, "transmit" ) == 0 )
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
	if ( strcmp( output_medium_string, "stacked_chart" ) == 0 )
	{
		return stacked_chart;
	}
	else
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
} /* get_output_medium_string() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *begin_date_string,
			char *end_date_string,
			char *process_name,
			char *fishing_purpose )
{
	format_initial_capital( title, process_name );

	if ( fishing_purpose
	&&   *fishing_purpose
	&&   strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
	{
		sprintf(	sub_title,
				"Purpose: %s From %s To %s",
				fishing_purpose,
				begin_date_string,
				end_date_string );
	}
	else
	{
		sprintf(	sub_title,
				"From %s To %s",
				begin_date_string,
				end_date_string );
	}
	format_initial_capital( sub_title, sub_title );
} /* get_title_and_sub_title() */

void creel_catches_report_generate_data(
			char *detail_temporary_filename,
			char *summary_temporary_filename,
			char *total_temporary_filename,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_area_list_string,
			char *family_list_string,
			char *genus_list_string,
			char *species_list_string,
			boolean order_by_caught,
			boolean summary_only,
			char *fishing_purpose )
{
	char output_sys_string[ 1024 ];
	FILE *output_pipe;
	FILE *output_file;
	CATCHES catches;
	int fishing_trips_count;
	int catch_records_count = 0;
	double total_fishing_hours_times_party_count = 0.0;
	int caught;
	int total_kept = 0;
	int total_released = 0;
	int total_caught = 0;
	HASH_TABLE *catches_hash_table;
	CATCHES *summary_catches;
	LIST *key_list;
	char *key;

	get_total_fishing_hours_times_party_count(
			&total_fishing_hours_times_party_count,
			&fishing_trips_count,
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area_list_string,
			fishing_purpose );

	catches_hash_table =
		hash_table_new( hash_table_medium );

	/* Output the detail */
	/* ----------------- */
	if ( ! ( output_file = fopen( detail_temporary_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR: Cannot open output file %s\n",
			 detail_temporary_filename );
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	if ( order_by_caught )
	{
		sprintf( output_sys_string,
			 "sort -t\\| -k10 -n -r > %s",
	 		 detail_temporary_filename );
	}
	else
	{
		sprintf( output_sys_string,
			 "cat > %s",
	 		 detail_temporary_filename );
	}

	output_pipe = popen( output_sys_string, "w" );

	while( creel_library_date_range_get_catches(
					&catches,
					application_name,
					begin_date_string,
					end_date_string,
					fishing_area_list_string,
					family_list_string,
					genus_list_string,
					species_list_string,
					fishing_purpose ) )
	{
		caught = catches.kept_integer + catches.released_integer;

		if ( !summary_only )
		{
			fprintf( output_pipe,
			 "%s|%s|%d|%s|%s|%s|%s|%s|%s|%d|%s|%s|%d|%d|%d\n",
			 	catches.fishing_purpose,
			 	catches.census_date,
			 	catches.interview_number,
			 	catches.family,
			 	catches.genus,
			 	catches.species,
			 	catches.fishing_area,
			 	catches.common_name,
			 	catches.florida_state_code,
			 	caught,
			 	catches.kept_count,
			 	catches.released_count,
				catches.hours_fishing,
				catches.number_of_people_fishing,
				catches.hours_fishing *
				catches.number_of_people_fishing );
		}

		total_caught += caught;
		total_kept += catches.kept_integer;
		total_released +=
			catches.released_integer;
		catch_records_count++;

		summary_catches =
			creel_library_get_fishing_area_summary_catches(
				catches_hash_table,
				catches.family,
				catches.genus,
				catches.species,
				catches.fishing_area,
				catches.common_name,
				catches.florida_state_code );

		summary_catches->caught_integer += caught;
		summary_catches->kept_integer += catches.kept_integer;
		summary_catches->released_integer +=
			catches.released_integer;
	}

	pclose( output_pipe );

	/* Output the summary */
	/* ------------------ */
	if ( ! ( output_file = fopen( summary_temporary_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR: Cannot open output file %s\n",
			 summary_temporary_filename );
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	if ( order_by_caught )
	{
		sprintf( output_sys_string,
			 "sort -t\\| -k7 -n -r > %s",
	 		 summary_temporary_filename );
	}
	else
	{
		sprintf( output_sys_string,
			 "cat > %s",
	 		 summary_temporary_filename );
	}

	output_pipe = popen( output_sys_string, "w" );

	key_list =
		hash_table_get_ordered_key_list(
			catches_hash_table );

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			summary_catches =
				hash_table_get_pointer(
					catches_hash_table,
					key );
	
			caught =	summary_catches->kept_integer +
					summary_catches->released_integer;

			fprintf( output_pipe,
				 "%s|%s|%s|%s|%s|%s|%d|%d|%d\n",
				 summary_catches->family,
				 summary_catches->genus,
				 summary_catches->species,
				 summary_catches->fishing_area,
				 summary_catches->common_name,
				 summary_catches->florida_state_code,
				 caught,
				 summary_catches->kept_integer,
				 summary_catches->released_integer );

		} while( list_next( key_list ) );
	} /* if ( list_rewind( key_list ) ) */

	pclose( output_pipe );

	/* Output the total */
	/* ---------------- */
	if ( ! ( output_file = fopen( total_temporary_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR: Cannot open output file %s\n",
			 total_temporary_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	sprintf( output_sys_string,
	 	 "cat > %s",
	 	 total_temporary_filename );

	output_pipe = popen( output_sys_string, "w" );

	fprintf( output_pipe,
		 "%d|%d|%d",
		 total_caught,
		 total_kept,
		 total_released );

	if ( total_fishing_hours_times_party_count
	&& ( ( !*family_list_string
	||     strcmp( family_list_string, "family" ) == 0 ) ) )
	{
		double unit_effort =
			(double)
			((double)total_caught /
			total_fishing_hours_times_party_count );

		fprintf( output_pipe,
			 "|%.2lf|%.2lf|%.0lf",
		 	 total_fishing_hours_times_party_count,
			 unit_effort,
			 1.0 / unit_effort * 60.0 );

		fprintf( output_pipe,
		 	"|%d",
		 	fishing_trips_count );
	}
	else
	{
		fprintf( output_pipe, "||||" );
	}

	fprintf( output_pipe,
		 "|%d\n",
		 catch_records_count );

	pclose( output_pipe );

} /* creel_catches_report_generate_data() */

void creel_catches_report_output_stacked_chart(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *summary_temporary_filename,
			char *document_root_directory,
			char *process_name,
			char *fishing_purpose )
{
	char title[ 512 ];
	char sub_title[ 512 ];
	int easycharts_width;
	int easycharts_height;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;
	char applet_library_archive[ 128 ];
	char *sample_values_kept_list_string = "";
	char *sample_values_released_list_string = "";
	char *sample_values_label_list_string = "";

	easycharts_get_chart_filename(
			&chart_filename,
			&prompt_filename,
			application_name,
			document_root_directory,
			getpid() );

	chart_file = fopen( chart_filename, "w" );

	if ( !chart_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			chart_filename );
		exit( 1 );
	}

	application_constants_get_easycharts_width_height(
			&easycharts_width,
			&easycharts_height,
			application_name );

	if ( !easycharts_width )
		easycharts_width = LOCAL_EASYCHARTS_WIDTH;

	if ( !easycharts_height )
		easycharts_height = EASYCHARTS_HEIGHT;

	sprintf(applet_library_archive,
		"/appaserver/%s/%s",
		application_name,
		EASYCHARTS_JAR_FILE );

	fprintf(chart_file,
		"<applet code=%s archive=%s width=%d height=%d",
		EASYCHARTS_APPLET_LIBRARY_BAR_CHART,
		applet_library_archive,
		easycharts_width,
		easycharts_height );

	fprintf( chart_file, " VIEWASTEXT id=Applet1>\n" );

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string,
			process_name,
			fishing_purpose );

	fprintf(chart_file,
		"<param name=chartTitle value=\"%s\\n%s\">\n",
		title,
		sub_title );

	creel_catches_report_stacked_chart_get_sample_values(
		&sample_values_kept_list_string,
		&sample_values_released_list_string,
		&sample_values_label_list_string,
		summary_temporary_filename );

	fprintf(chart_file,
"<param name=seriesCount value=\"2\">\n"
"<param name=sampleValues_0 value=\"%s\">\n"
"<param name=sampleValues_1 value=\"%s\">\n"
"<param name=sampleColors value=\"red,blue\">\n"
"<param name=multiColorOn value=true>\n"
"<param name=barLabelsOn value=true>\n"
"<param name=legendOn value=true>\n"
"<param name=sampleScrollerOn value=true>\n"
"<param name=rangeAdjusterOn value=true>\n"
"<param name=autoLabelSpacingOn value=true>\n"
"<param name=barLabelAngle value=\"90\">\n"
"<param name=sampleLabels value=\"%s\">\n"
"<param name=barType value=\"stacked\">\n"
"<param name=seriesLabels value=\"Kept,Released\">\n",
		sample_values_kept_list_string,
		sample_values_released_list_string,
		sample_values_label_list_string );

	fprintf(chart_file, "</applet>\n" );
	easycharts_output_html( chart_file );

	fclose( chart_file );

	easycharts_output_graph_window(
				application_name,
				(char *)0 /* appaserver_mount_point */,
				0 /* not with_document_output */,
				process_name,
				prompt_filename,
				(char *)0 /* where_clause */ );

} /* creel_catches_report_output_stacked_chart() */

void creel_catches_report_stacked_chart_get_sample_values(
		char **sample_values_kept_list_string,
		char **sample_values_released_list_string,
		char **sample_values_label_list_string,
		char *summary_temporary_filename )
{
	FILE *input_file;
	char input_buffer[ 1024 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species[ 128 ];
	char fishing_area[ 128 ];
	char common[ 128 ];
	char code[ 16 ];
	char kept[ 16 ];
	char released[ 16 ];
	char local_sample_values_kept_list_string[ 1024 ];
	char local_sample_values_released_list_string[ 1024 ];
	char local_sample_values_label_list_string[ 65536 ];
	char *local_sample_values_kept_ptr;
	char *local_sample_values_released_ptr;
	char *local_sample_values_label_ptr;
	boolean first_time = 1;

	local_sample_values_kept_ptr =
		local_sample_values_kept_list_string;
	local_sample_values_released_ptr =
		local_sample_values_released_list_string;
	local_sample_values_label_ptr =
		local_sample_values_label_list_string;

	*local_sample_values_kept_ptr = '\0';
	*local_sample_values_released_ptr = '\0';
	*local_sample_values_label_ptr = '\0';
/*
	*sample_values_kept_list_string = "20,20,20,20,20";
	*sample_values_released_list_string = "-10,-10,-10,-10,-10";
	*sample_values_label_list_string =
		"Ariidae Arius felis\\nSea Catfish\\n0801,Ariidae Bagre marinus\\nGafftopsail catfish\\n0802,Clinidae Elops saurus\\nLadyfish\\n3901,Clinidae Pamatomus saltrtrix\\nBluefish\\n7801,Sauridae Unknown unknown\\nlizardfish\\n9500";
*/

	if ( ! ( input_file = fopen( summary_temporary_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 summary_temporary_filename );
		exit( 1 );
	}

	while( get_line( input_buffer, input_file ) )
	{
		piece( family, '|', input_buffer, 0 );
		piece( genus, '|', input_buffer, 1 );
		piece( species, '|', input_buffer, 2 );
		piece( fishing_area, '|', input_buffer, 3 );
		piece( common, '|', input_buffer, 4 );
		piece( code, '|', input_buffer, 5 );
		piece( kept, '|', input_buffer, 7 );
		piece( released, '|', input_buffer, 8 );

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			local_sample_values_kept_ptr +=
				sprintf(local_sample_values_kept_ptr,
					"," );
			local_sample_values_released_ptr +=
				sprintf(local_sample_values_released_ptr,
					"," );
			local_sample_values_label_ptr +=
				sprintf(local_sample_values_label_ptr,
					"," );
		}

		local_sample_values_kept_ptr +=
			sprintf(local_sample_values_kept_ptr,
				"%s",
				kept );

		local_sample_values_released_ptr +=
			sprintf(local_sample_values_released_ptr,
				"%s",
				released );

		local_sample_values_label_ptr +=
			sprintf(local_sample_values_label_ptr,
				"%s: %s %s %s\\n%s\\n%s",
				fishing_area,
				family,
				genus,
				species,
				common,
				code );
	}

	fclose( input_file );
	*sample_values_kept_list_string =
		strdup( local_sample_values_kept_list_string );
	*sample_values_released_list_string =
		strdup( local_sample_values_released_list_string );
	*sample_values_label_list_string =
		strdup( local_sample_values_label_list_string );

} /* creel_catches_report_stacked_chart_get_sample_values() */

