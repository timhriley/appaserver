/* ---------------------------------------------------	*/
/* src_creel/sport_measurement_report.c			*/
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
#include "appaserver_link.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, text_file, table, chart };

/* Constants */
/* --------- */
#define LOCAL_EASYCHARTS_WIDTH			730
#define ROWS_BETWEEN_HEADING			20
#define DEFAULT_OUTPUT_MEDIUM			table
#define FILENAME_STEM_DETAIL			"sport_measurement_detail"
#define FILENAME_STEM_SUMMARY			"sport_measurement_summary"

#define DETAIL_TEMPORARY_TEMPLATE "%s/sport_measurement_detail_temporary_%d.txt"
#define SUMMARY_TEMPORARY_TEMPLATE "%s/sport_measurement_summary_temporary_%d.txt"

/* Prototypes */
/* ---------- */
void sport_measurement_report_chart_get_sample_values(
				char **sample_values_measurement_list_string,
				char **sample_values_label_list_string,
				char *summary_temporary_filename );

void sport_measurement_report_generate_data(
				char *detail_temporary_filename,
				char *summary_temporary_filename,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string );

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_date_string,
				char *end_date_string,
				char *process_name );

enum output_medium get_output_medium(
				char *output_medium_string );

void sport_measurement_report_output_table(
				char *begin_date_string,
				char *end_date_string,
				char *detail_temporary_filename,
				char *summary_temporary_filename,
				char *process_name );

void sport_measurement_report_output_chart(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *summary_temporary_filename,
				char *document_root_directory,
				char *process_name );

void sport_measurement_report_output_text_file(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *document_root_directory,
				char *detail_temporary_filename,
				char *summary_temporary_filename,
				enum output_medium,
				char *process_name );

void get_total_fishing_hours_times_party_count(
				double *total_fishing_hours_times_party_count,
				int *fishing_trips_count,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area );

int main( int argc, char **argv )
{
	char *application_name;
	char *fishing_area;
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	enum output_medium output_medium;
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
	DOCUMENT *document;

	if ( argc != 11 )
	{
		fprintf( stderr, 
	"Usage: %s application process_name fishing_area begin_date end_date family genus species summary_only_yn output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	process_name = argv[ 2 ];
	fishing_area = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	end_date_string = argv[ 5 ];

	if ( !*end_date_string || strcmp( end_date_string, "end_date" ) == 0 )
		end_date_string = begin_date_string;

	family_list_string = argv[ 6 ];
	genus_list_string = argv[ 7 ];
	species_list_string = argv[ 8 ];
	summary_only = (*argv[ 9 ] == 'y');
	output_medium_string = argv[ 10 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	output_medium = get_output_medium( output_medium_string );

	if ( summary_only )
	{
		*detail_temporary_filename = '\0';
	}
	else
	{
		sprintf(detail_temporary_filename,
		 	DETAIL_TEMPORARY_TEMPLATE,
		 	appaserver_parameter_file->appaserver_data_directory,
		 	process_id );
	}

	sprintf( summary_temporary_filename,
		 SUMMARY_TEMPORARY_TEMPLATE,
		 appaserver_parameter_file->appaserver_data_directory,
		 process_id );

	sport_measurement_report_generate_data(
			detail_temporary_filename,
			summary_temporary_filename,
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area,
			family_list_string,
			genus_list_string,
			species_list_string );

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

		sport_measurement_report_output_table(
			begin_date_string,
			end_date_string,
			detail_temporary_filename,
			summary_temporary_filename,
			process_name );

		document_close();
	}
	else
	if ( output_medium == chart )
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

		sport_measurement_report_output_chart(
			application_name,
			begin_date_string,
			end_date_string,
			summary_temporary_filename,
			appaserver_parameter_file->
				document_root,
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

		sport_measurement_report_output_text_file(
			application_name,
			begin_date_string,
			end_date_string,
			appaserver_parameter_file->document_root,
			detail_temporary_filename,
			summary_temporary_filename,
			output_medium,
			process_name );

		document_close();
	}
	else
	if ( output_medium == output_medium_stdout )
	{
		sport_measurement_report_output_text_file(
			application_name,
			begin_date_string,
			end_date_string,
			appaserver_parameter_file->document_root,
			detail_temporary_filename,
			summary_temporary_filename,
			output_medium,
			process_name );
	}

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

void sport_measurement_report_output_table(
			char *begin_date_string,
			char *end_date_string,
			char *detail_temporary_filename,
			char *summary_temporary_filename,
			char *process_name )
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
			process_name );

	html_table = new_html_table( title, sub_title );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	
	html_table->number_left_justified_columns = 9;
	html_table->number_right_justified_columns = 4;

	if ( *detail_temporary_filename )
	{
		heading_list = list_new();
		list_append_pointer( heading_list, "Date" );
		list_append_pointer( heading_list, "Number" );
		list_append_pointer( heading_list, "Location" );
		list_append_pointer( heading_list, "Code" );
		list_append_pointer( heading_list, "Fishing Area" );
		list_append_pointer( heading_list, "Family" );
		list_append_pointer( heading_list, "Genus" );
		list_append_pointer( heading_list, "Species" );
		list_append_pointer( heading_list, "Common" );
		list_append_pointer( heading_list, "Length(MM)" );
		
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

/*
"%s.census_date,%s.interview_number,%s.interview_location,%s.florida_state_code,fishing_area,%s.family,%s.genus,%s.species,%s.common_name,length_millimeters",
*/
			/* Census Date */
			/* ----------- */
			piece( piece_buffer, '|', input_buffer, 0 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Interview Number */
			/* ---------------- */
			piece( piece_buffer, '|', input_buffer, 1 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Interview Location */
			/* ------------------ */
			piece( piece_buffer, '|', input_buffer, 2 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Florida State Code */
			/* ------------------ */
			piece( piece_buffer, '|', input_buffer, 3 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Fishing Area */
			/* ------------ */
			piece( piece_buffer, '|', input_buffer, 4 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Family */
			/* ------ */
			piece( piece_buffer, '|', input_buffer, 5 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Genus */
			/* ----- */
			piece( piece_buffer, '|', input_buffer, 6 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Species */
			/* ------- */
			piece( piece_buffer, '|', input_buffer, 7 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Common */
			/* ------ */
			piece( piece_buffer, '|', input_buffer, 8 );
			html_table_set_data(
				html_table->data_list,
				strdup( piece_buffer ) );

			/* Length(MM) */
			/* ---------- */
			piece( piece_buffer, '|', input_buffer, 9 );
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

	printf( "<h1>Summary</h1>\n" );

	heading_list = list_new();
	list_append_pointer( heading_list, "" );
	list_append_pointer( heading_list, "" );
	list_append_pointer( heading_list, "" );
	list_append_pointer( heading_list, "Code" );
	list_append_pointer( heading_list, "" );
	list_append_pointer( heading_list, "Family" );
	list_append_pointer( heading_list, "Genus" );
	list_append_pointer( heading_list, "Species" );
	list_append_pointer( heading_list, "Common" );
	list_append_pointer( heading_list, "AvgLength(MM)" );
	list_append_pointer( heading_list, "Count" );
		
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

		/* Date (which is empty) */
		/* --------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		/* Interview Number (which is empty) */
		/* --------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		/* Interview Location (which is empty) */
		/* ----------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		/* Florida State Code */
		/* ------------------ */
		piece( piece_buffer, '|', input_buffer, 4 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Fishing Area (which is empty) */
		/* ----------------------------- */
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

		/* Common */
		/* ------ */
		piece( piece_buffer, '|', input_buffer, 3 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Length(MM) */
		/* ---------- */
		piece( piece_buffer, '|', input_buffer, 5 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Count */
		/* ----- */
		piece( piece_buffer, '|', input_buffer, 6 );
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

	html_table_close();

} /* sport_measurement_report_output_table() */

void sport_measurement_report_output_text_file(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *document_root_directory,
			char *detail_temporary_filename,
			char *summary_temporary_filename,
			enum output_medium output_medium,
			char *process_name )
{
	char output_sys_string[ 1024 ];
	char *output_filename = {0};
	char *ftp_detail_filename = {0};
	char *ftp_summary_filename = {0};
	pid_t process_id = getpid();
	FILE *output_pipe;
	char input_buffer[ 128 ];
	FILE *input_file;
	char title[ 512 ];
	char sub_title[ 512 ];
	APPASERVER_LINK *appaserver_link;

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string,
			process_name );

	/* Output the detail */
	/* ----------------- */
	if ( *detail_temporary_filename )
	{
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
	
		if ( output_medium != output_medium_stdout )
		{
			FILE *output_file;

			appaserver_link =
				appaserver_link_new(
					application_http_prefix(
						application_name ),
					appaserver_library_server_address(),
					( application_prepend_http_protocol_yn(
						application_name ) == 'y' ),
					document_root_directory,
					FILENAME_STEM_DETAIL,
					application_name,
					process_id,
					(char *)0 /* session */,
					(char *)0 /* begin_date_string */,
					(char *)0 /* end_date_string */,
					"csv" );

			output_filename = appaserver_link->output->filename;

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
	
			sprintf( output_sys_string,
			 	 "sed 's/|/,/g' > %s",
			 	 output_filename );

			ftp_detail_filename = appaserver_link->prompt->filename;
		}
		else
		{
			strcpy( output_sys_string, "sed 's/|/,/g'" );
		}
	
		output_pipe = popen( output_sys_string, "w" );
	
		fprintf( output_pipe,
			 "%s Detail\n%s\n", title, sub_title );
	
		fprintf( output_pipe,
"Date,Number,Location,Code,FishingArea,Family,Genus,Species,Common,Length(MM)\n" );
	
		while( get_line( input_buffer, input_file ) )
		{
			fprintf( output_pipe,
				 "%s\n",
				 input_buffer );
		}
	
		fclose( input_file );
		pclose( output_pipe );
	}

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

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				document_root_directory,
				FILENAME_STEM_SUMMARY,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"csv" );

		output_filename = appaserver_link->output->filename;

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
		 	 "sed 's/|/,/g' > %s",
		 	 output_filename );

		ftp_summary_filename = appaserver_link->prompt->filename;
	}
	else
	{
		strcpy( output_sys_string, "sed 's/|/,/g'" );
	}

	output_pipe = popen( output_sys_string, "w" );

	fprintf( output_pipe,
		 "%s Summary\n%s\n", title, sub_title );

	fprintf( output_pipe,
		 "Family,Genus,Species,Common,Code,AvgLength(MM),Count\n" );

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
	"Summary: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	}

} /* sport_measurement_report_output_text_file() */


void get_total_fishing_hours_times_party_count(
			double *total_fishing_hours_times_party_count,
			int *fishing_trips_count,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_area )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
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
			fishing_area,
			begin_date_string,
			end_date_string,
			(char *)0 /* fishing_purpose */ );

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

void sport_measurement_report_output_chart(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *summary_temporary_filename,
			char *document_root_directory,
			char *process_name )
{
	char title[ 512 ];
	char sub_title[ 512 ];
	int easycharts_width;
	int easycharts_height;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;
	char applet_library_archive[ 128 ];
	char *sample_values_measurement_list_string;
	char *sample_values_label_list_string;

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
			process_name );

	fprintf(chart_file,
		"<param name=chartTitle value=\"%s\\n%s\">\n",
		title,
		sub_title );

	sport_measurement_report_chart_get_sample_values(
		&sample_values_measurement_list_string,
		&sample_values_label_list_string,
		summary_temporary_filename );

	fprintf(chart_file,
"<param name=seriesCount value=\"1\">\n"
"<param name=sampleValues_0 value=\"%s\">\n"
"<param name=sampleColors value=\"blue\">\n"
"<param name=multiColorOn value=false>\n"
"<param name=barLabelsOn value=true>\n"
"<param name=legendOn value=true>\n"
"<param name=sampleScrollerOn value=true>\n"
"<param name=rangeAdjusterOn value=true>\n"
"<param name=autoLabelSpacingOn value=true>\n"
"<param name=barLabelAngle value=\"90\">\n"
"<param name=sampleLabels value=\"%s\">\n"
"<param name=barType value=\"stacked\">\n"
"<param name=seriesLabels value=\"AvgMeasurement\">\n",
		sample_values_measurement_list_string,
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
} /* sport_measurement_report_output_chart() */

void sport_measurement_report_chart_get_sample_values(
		char **sample_values_measurement_list_string,
		char **sample_values_label_list_string,
		char *summary_temporary_filename )
{
	FILE *input_file;
	char input_buffer[ 1024 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species[ 128 ];
	char common[ 128 ];
	char code[ 16 ];
	char measurement[ 16 ];
	char local_sample_values_measurement_list_string[ 4096 ];
	char local_sample_values_label_list_string[ 65536 ];
	char *local_sample_values_measurement_ptr;
	char *local_sample_values_label_ptr;
	boolean first_time = 1;

	local_sample_values_measurement_ptr =
		local_sample_values_measurement_list_string;
	local_sample_values_label_ptr =
		local_sample_values_label_list_string;

	*local_sample_values_measurement_ptr = '\0';
	*local_sample_values_label_ptr = '\0';

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
		piece( common, '|', input_buffer, 3 );
		piece( code, '|', input_buffer, 4 );
		piece( measurement, '|', input_buffer, 5 );

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			local_sample_values_measurement_ptr +=
				sprintf(local_sample_values_measurement_ptr,
					"," );
			local_sample_values_label_ptr +=
				sprintf(local_sample_values_label_ptr,
					"," );
		}

		local_sample_values_measurement_ptr +=
			sprintf(local_sample_values_measurement_ptr,
				"%s",
				measurement );

		local_sample_values_label_ptr +=
			sprintf(local_sample_values_label_ptr,
				"%s %s %s\\n%s\\n%s",
				family,
				genus,
				species,
				common,
				code );
	}

	fclose( input_file );

	*sample_values_measurement_list_string =
		strdup( local_sample_values_measurement_list_string );
	*sample_values_label_list_string =
		strdup( local_sample_values_label_list_string );

} /* sport_measurement_report_chart_get_sample_values() */

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
	if ( strcmp( output_medium_string, "chart" ) == 0 )
	{
		return chart;
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
			char *process_name )
{
	strcpy( title, process_name );
	format_initial_capital( title, title );

	sprintf(	sub_title,
			"From %s To %s",
			begin_date_string,
			end_date_string );
} /* get_title_and_sub_title() */

void sport_measurement_report_generate_data(
			char *detail_temporary_filename,
			char *summary_temporary_filename,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_area,
			char *family_list_string,
			char *genus_list_string,
			char *species_list_string )
{
	char output_sys_string[ 1024 ];
	FILE *output_pipe = {0};
	FILE *output_file;
	CATCH_MEASUREMENTS catch_measurements;
	HASH_TABLE *catch_measurements_hash_table;
	CATCH_MEASUREMENTS *summary_catch_measurements;
	LIST *key_list;
	char *key;

	catch_measurements_hash_table =
		hash_table_new( hash_table_medium );

	/* Output the detail */
	/* ----------------- */
	if ( *detail_temporary_filename )
	{
		if ( ! ( output_file =
				fopen( detail_temporary_filename, "w" ) ) )
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
	
		sprintf( output_sys_string,
			 "cat > %s",
		 	 detail_temporary_filename );
	
		output_pipe = popen( output_sys_string, "w" );
	}

	while( creel_library_date_range_get_catch_measurements(
					&catch_measurements,
					application_name,
					begin_date_string,
					end_date_string,
					fishing_area,
					family_list_string,
					genus_list_string,
					species_list_string ) )
	{
		if ( output_pipe )
		{
			format_initial_capital(
				catch_measurements.interview_location,
				catch_measurements.interview_location );
				
			fprintf( output_pipe,
			 	"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
			 	catch_measurements.census_date,
			 	catch_measurements.interview_number,
			 	catch_measurements.interview_location,
			 	catch_measurements.florida_state_code,
			 	catch_measurements.fishing_area,
			 	catch_measurements.family,
			 	catch_measurements.genus,
			 	catch_measurements.species,
			 	catch_measurements.common_name,
			 	catch_measurements.length_millimeters );
		}

		if ( *catch_measurements.length_millimeters )
		{
			summary_catch_measurements =
			creel_library_get_summary_catch_measurements(
				catch_measurements_hash_table,
				catch_measurements.family,
				catch_measurements.genus,
				catch_measurements.species,
				catch_measurements.common_name,
				catch_measurements.florida_state_code );

			summary_catch_measurements->
				total_length_millimeters_integer +=
					catch_measurements.
					length_millimeters_integer;
			summary_catch_measurements->summary_count++;
		}
	}

	if ( *detail_temporary_filename )
	{
		pclose( output_pipe );
	}

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

	sprintf( output_sys_string,
		 "cat > %s",
	 	 summary_temporary_filename );

	output_pipe = popen( output_sys_string, "w" );

	key_list =
		hash_table_get_ordered_key_list(
			catch_measurements_hash_table );

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			summary_catch_measurements =
				hash_table_get_pointer(
					catch_measurements_hash_table,
					key );
	
			fprintf( output_pipe,
				 "%s|%s|%s|%s|%s|%.1lf|%d\n",
				 summary_catch_measurements->family,
				 summary_catch_measurements->genus,
				 summary_catch_measurements->species,
				 summary_catch_measurements->common_name,
				 summary_catch_measurements->florida_state_code,
				 (double)
				 (
				 summary_catch_measurements->
				 	total_length_millimeters_integer /
				 (double)summary_catch_measurements->
				 	summary_count
				 ),
				 summary_catch_measurements->summary_count
				 );

		} while( list_next( key_list ) );
	} /* if ( list_rewind( key_list ) ) */

	pclose( output_pipe );

} /* sport_measurement_report_generate_data() */

