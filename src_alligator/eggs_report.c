/* ---------------------------------------------------	*/
/* src_alligator/eggs_report.c				*/
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
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "appaserver_link_file.h"
#include "alligator.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, text_file, table };

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING			10
#define DEFAULT_OUTPUT_MEDIUM			table

#define DATA_TEMPLATE		"%s/eggs_report_temporary_%d.csv"

#define SELECT_TEMPLATE		"%s.nest_number,transect_number,habitat,basin,materials,observation_date,nest_status,nest_visit_number,%s.total_eggs,%s.banded_eggs,%s.not_banded_eggs,hatched_eggs,flooded_eggs,predated_eggs,crushed_eggs,other_egg_mortality"

/* Prototypes */
/* ---------- */
void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_discovery_date_string,
				char *end_discovery_date_string,
				char *transect_number,
				char *habitat,
				char *basin,
				char *materials,
				char *nest_status,
				char *process_name );

enum output_medium get_output_medium(
				char *output_medium_string );

boolean generate_data(
				char *data_filename,
				char *application_name,
				char *begin_discovery_date_string,
				char *end_discovery_date_string,
				char *transect_number,
				char *habitat,
				char *basin,
				char *materials,
				char *nest_status );

void output_table(
				char *data_filename,
				char *begin_discovery_date_string,
				char *end_discovery_date_string,
				char *transect_number,
				char *habitat,
				char *basin,
				char *materials,
				char *nest_status,
				char *process_name );

void output_text_file(
				char *data_filename,
				char *application_name,
				char *document_root_directory,
				char *begin_discovery_date_string,
				char *end_discovery_date_string,
				char *transect_number,
				char *habitat,
				char *basin,
				char *materials,
				char *nest_status,
				enum output_medium output_medium,
				char *process_name,
				pid_t process_id );

int main( int argc, char **argv )
{
	char *application_name;
	char *transect_number;
	char *habitat;
	char *basin;
	char *materials;
	char *nest_status;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	enum output_medium output_medium;
	char *begin_discovery_date_string;
	char *end_discovery_date_string;
	char data_filename[ 128 ];
	char *output_medium_string;
	pid_t process_id = getpid();

	if ( argc != 11 )
	{
		fprintf( stderr, 
	"Usage: %s application process_name begin_discovery_date end_discovery_date transect_number habitat basin materials nest_status output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];

	begin_discovery_date_string = argv[ 3 ];
	if ( strcmp(	begin_discovery_date_string,
			"begin_discovery_date" ) == 0 )
	{
		*begin_discovery_date_string = '\0';
	}

	end_discovery_date_string = argv[ 4 ];
	if ( !*end_discovery_date_string
	||   strcmp( end_discovery_date_string, "end_discovery_date" ) == 0 )
	{
		*end_discovery_date_string = '\0';
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	transect_number = argv[ 5 ];
	if ( strcmp(	transect_number,
			"transect_number" ) == 0 )
	{
		*transect_number = '\0';
	}

	habitat = argv[ 6 ];
	if ( strcmp(	habitat,
			"habitat" ) == 0 )
	{
		*habitat = '\0';
	}

	basin = argv[ 7 ];
	if ( strcmp(	basin,
			"basin" ) == 0 )
	{
		*basin = '\0';
	}

	materials = argv[ 8 ];
	if ( strcmp(	materials,
			"materials" ) == 0 )
	{
		*materials = '\0';
	}

	nest_status = argv[ 9 ];
	if ( strcmp(	nest_status,
			"nest_status" ) == 0 )
	{
		*nest_status = '\0';
	}

	output_medium_string = argv[ 10 ];

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

	output_medium = get_output_medium( output_medium_string );

	sprintf( data_filename,
		 DATA_TEMPLATE,
		 appaserver_parameter_file->
			appaserver_data_directory,
		 process_id );

	if ( !generate_data(
			data_filename,
			application_name,
			begin_discovery_date_string,
			end_discovery_date_string,
			transect_number,
			habitat,
			basin,
			materials,
			nest_status ) )
	{
		if ( output_medium != output_medium_stdout )
		{
			char title[ 128 ];
			char sub_title[ 65536 ];

			document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

			get_title_and_sub_title(
				title,
				sub_title,
				begin_discovery_date_string,
				end_discovery_date_string,
				transect_number,
				habitat,
				basin,
				materials,
				nest_status,
				process_name );

			printf( "<h2>%s</h2>\n", title );
			printf( "<h2>%s</h2>\n", sub_title );
			printf( "<h2>ERROR: no data to output.</h2>\n" );
			document_close();
		}
		else
		{
			fprintf( stderr, "ERROR: no data to output.\n" );
		}
		exit( 0 );
	}

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

		output_table(
				data_filename,
				begin_discovery_date_string,
				end_discovery_date_string,
				transect_number,
				habitat,
				basin,
				materials,
				nest_status,
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

		output_text_file(
				data_filename,
				application_name,
				appaserver_parameter_file->
					document_root,
				begin_discovery_date_string,
				end_discovery_date_string,
				transect_number,
				habitat,
				basin,
				materials,
				nest_status,
				output_medium,
				process_name,
				process_id );

		document_close();
	}
	else
	if ( output_medium == output_medium_stdout )
	{
		output_text_file(
				data_filename,
				application_name,
				appaserver_parameter_file->
					document_root,
				begin_discovery_date_string,
				end_discovery_date_string,
				transect_number,
				habitat,
				basin,
				materials,
				nest_status,
				output_medium,
				process_name,
				process_id );
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_table(
			char *data_filename,
			char *begin_discovery_date_string,
			char *end_discovery_date_string,
			char *transect_number,
			char *habitat,
			char *basin,
			char *materials,
			char *nest_status,
			char *process_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char title[ 128 ];
	char sub_title[ 65536 ];
	char input_buffer[ 1024 ];
	char piece_buffer[ 512 ];
	char nest_visit_number_string[ 128 ];
	int nest_visit_number;
	FILE *input_file;
	int count = 0;

	if ( ! ( input_file = fopen( data_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 data_filename );
		exit( 1 );
	}

	get_title_and_sub_title(
				title,
				sub_title,
				begin_discovery_date_string,
				end_discovery_date_string,
				transect_number,
				habitat,
				basin,
				materials,
				nest_status,
				process_name );

	html_table = new_html_table( title, sub_title );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	
	html_table->number_left_justified_columns = 7;
	html_table->number_right_justified_columns = 9;

	heading_list = list_new();
	list_append_pointer( heading_list, "Nest Number" );
	list_append_pointer( heading_list, "Transect" );
	list_append_pointer( heading_list, "Habitat" );
	list_append_pointer( heading_list, "Basin" );
	list_append_pointer( heading_list, "Materials" );
	list_append_pointer( heading_list, "Observation" );
	list_append_pointer( heading_list, "Status" );
	list_append_pointer( heading_list, "Visit Number" );
	list_append_pointer( heading_list, "Total" );
	list_append_pointer( heading_list, "Banded" );
	list_append_pointer( heading_list, "Not Banded" );
	list_append_pointer( heading_list, "Hatched" );
	list_append_pointer( heading_list, "Flooded" );
	list_append_pointer( heading_list, "Predated" );
	list_append_pointer( heading_list, "Crushed" );
	list_append_pointer( heading_list, "Other Egg Mortality" );

	html_table_set_heading_list( html_table, heading_list );
	
	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

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

		/* Nest Number */
		/* ----------- */
		piece( piece_buffer, '|', input_buffer, 0 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Transect */
		/* -------- */
		piece( piece_buffer, '|', input_buffer, 1 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Habitat */
		/* ------- */
		piece( piece_buffer, '|', input_buffer, 2 );
		format_initial_capital( piece_buffer, piece_buffer );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Basin */
		/* ----- */
		piece( piece_buffer, '|', input_buffer, 3 );
		format_initial_capital( piece_buffer, piece_buffer );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Materials */
		/* --------- */
		piece( piece_buffer, '|', input_buffer, 4 );
		format_initial_capital( piece_buffer, piece_buffer );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Observation Date */
		/* ---------------- */
		piece( piece_buffer, '|', input_buffer, 5 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Nest Status */
		/* ----------- */
		piece( piece_buffer, '|', input_buffer, 6 );
		format_initial_capital( piece_buffer, piece_buffer );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Nest Visit Number */
		/* ----------------- */
		piece( nest_visit_number_string, '|', input_buffer, 7 );
		html_table_set_data(
			html_table->data_list,
			strdup( nest_visit_number_string ) );
		nest_visit_number = atoi( nest_visit_number_string );

		/* Total Eggs */
		/* ---------- */
		if ( nest_visit_number == 1 )
			piece( piece_buffer, '|', input_buffer, 8 );
		else
			*piece_buffer = '\0';

		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Banded Eggs */
		/* ----------- */
		if ( nest_visit_number == 1 )
			piece( piece_buffer, '|', input_buffer, 9 );
		else
			*piece_buffer = '\0';

		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Not-Banded Eggs */
		/* --------------- */
		if ( nest_visit_number == 1 )
			piece( piece_buffer, '|', input_buffer, 10 );
		else
			*piece_buffer = '\0';

		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Hatched Eggs */
		/* ------------ */
		piece( piece_buffer, '|', input_buffer, 11 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Flooded Eggs */
		/* ------------ */
		piece( piece_buffer, '|', input_buffer, 12 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Predated Eggs */
		/* ------------- */
		piece( piece_buffer, '|', input_buffer, 13 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Crushed Eggs */
		/* ------------ */
		piece( piece_buffer, '|', input_buffer, 14 );
		html_table_set_data(
			html_table->data_list,
			strdup( piece_buffer ) );

		/* Other Egg Mortality */
		/* ------------------- */
		piece( piece_buffer, '|', input_buffer, 15 );
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

	html_table_close();

} /* output_table() */

void output_text_file(
				char *data_filename,
				char *application_name,
				char *document_root_directory,
				char *begin_discovery_date_string,
				char *end_discovery_date_string,
				char *transect_number,
				char *habitat,
				char *basin,
				char *materials,
				char *nest_status,
				enum output_medium output_medium,
				char *process_name,
				pid_t process_id )
{
	char sys_string[ 1024 ];
	char *output_filename;
	char *ftp_filename;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	FILE *input_file;
	char title[ 512 ];
	char sub_title[ 65536 ];
	char piece_buffer[ 128 ];
	char nest_visit_number_string[ 128 ];
	int nest_visit_number;

	get_title_and_sub_title(
				title,
				sub_title,
				begin_discovery_date_string,
				end_discovery_date_string,
				transect_number,
				habitat,
				basin,
				materials,
				nest_status,
				process_name );

	if ( ! ( input_file = fopen( data_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 data_filename );
		exit( 1 );
	}

	if ( output_medium == text_file )
	{
		FILE *output_file;
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			document_root_directory,
				process_name /* filename_stem */,
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

		/* Test the output file. */
		/* --------------------- */
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

		sprintf( sys_string,
		 "sed 's/^/%c/' | sed 's/|/\\%c,\\%c/g' | sed 's/$/%c/' > %s",
			 '"',
			 '"',
			 '"',
			 '"',
		 	 output_filename );
	}
	else
	/* ------------------------------------------------- */
	/* Must be ( output_medium == output_medium_stdout ) */
	/* ------------------------------------------------- */
	{
		sprintf( sys_string,
		 	 "cat > %s",
		 	 output_filename );
	}

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s\n%s\n", title, sub_title );

	fprintf( output_pipe,
"Nest|Transect|Habitat|Basin|Materials|Observation|Status|Visit|Total|Banded|Not-Banded|Hatched|Flooded|Predated|Crushed|Other-EggMortality\n" );

	while( get_line( input_buffer, input_file ) )
	{
		/* Nest Number */
		/* ----------- */
		piece( piece_buffer, '|', input_buffer, 0 );
		fprintf( output_pipe,
			 "%s",
			 piece_buffer );

		/* Transect */
		/* -------- */
		piece( piece_buffer, '|', input_buffer, 1 );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Habitat */
		/* ------- */
		piece( piece_buffer, '|', input_buffer, 2 );
		format_initial_capital( piece_buffer, piece_buffer );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Basin */
		/* ----- */
		piece( piece_buffer, '|', input_buffer, 3 );
		format_initial_capital( piece_buffer, piece_buffer );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Materials */
		/* --------- */
		piece( piece_buffer, '|', input_buffer, 4 );
		format_initial_capital( piece_buffer, piece_buffer );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Observation Date */
		/* ---------------- */
		piece( piece_buffer, '|', input_buffer, 5 );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Nest Status */
		/* ----------- */
		piece( piece_buffer, '|', input_buffer, 6 );
		format_initial_capital( piece_buffer, piece_buffer );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Nest Visit Number */
		/* ----------------- */
		piece( nest_visit_number_string, '|', input_buffer, 7 );
		fprintf( output_pipe,
			 "|%s",
			 nest_visit_number_string );
		nest_visit_number = atoi( nest_visit_number_string );

		/* Total Eggs */
		/* ---------- */
		if ( nest_visit_number == 1 )
			piece( piece_buffer, '|', input_buffer, 8 );
		else
			*piece_buffer = '\0';

		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Banded Eggs */
		/* ----------- */
		if ( nest_visit_number == 1 )
			piece( piece_buffer, '|', input_buffer, 9 );
		else
			*piece_buffer = '\0';

		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Not-Banded Eggs */
		/* --------------- */
		if ( nest_visit_number == 1 )
			piece( piece_buffer, '|', input_buffer, 10 );
		else
			*piece_buffer = '\0';

		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Hatched Eggs */
		/* ------------ */
		piece( piece_buffer, '|', input_buffer, 11 );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Flooded Eggs */
		/* ------------ */
		piece( piece_buffer, '|', input_buffer, 12 );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Predated Eggs */
		/* ------------- */
		piece( piece_buffer, '|', input_buffer, 13 );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Crushed Eggs */
		/* ------------ */
		piece( piece_buffer, '|', input_buffer, 14 );
		fprintf( output_pipe,
			 "|%s",
			 piece_buffer );

		/* Other Egg Mortality */
		/* ------------------- */
		piece( piece_buffer, '|', input_buffer, 15 );
		fprintf( output_pipe,
			 "|%s\n",
			 piece_buffer );
	}

	fclose( input_file );
	pclose( output_pipe );

	if ( output_medium == text_file )
	{
		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h2>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );
		
		appaserver_library_output_ftp_prompt(
			ftp_filename,
	"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	}

} /* output_text_file() */

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
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
} /* get_output_medium() */

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_discovery_date_string,
				char *end_discovery_date_string,
				char *transect_number,
				char *habitat,
				char *basin,
				char *materials,
				char *nest_status,
				char *process_name )
{
	char *sub_title_pointer;

	strcpy( title, process_name );
	format_initial_capital( title, title );
	sub_title_pointer = sub_title;

	*sub_title_pointer = '\0';

	if ( *begin_discovery_date_string )
	{
		sub_title_pointer +=
			sprintf(
				sub_title_pointer,
				"From %s To %s",
				begin_discovery_date_string,
				end_discovery_date_string );
	}
	else
	{
		sub_title_pointer +=
			sprintf(
				sub_title_pointer,
				"Date: Period of Record" );
	}

	if ( *transect_number )
	{
		if ( !*sub_title_pointer )
		{
			sub_title_pointer +=
				sprintf(sub_title_pointer, "; " );
		}
		sub_title_pointer +=
			sprintf(
				sub_title_pointer,
				"Transect: %s",
				transect_number );
	}

	if ( *habitat )
	{
		if ( !*sub_title_pointer )
		{
			sub_title_pointer +=
				sprintf(sub_title_pointer, "; " );
		}
		sub_title_pointer +=
			sprintf(
				sub_title_pointer,
				"Habitat: %s",
				habitat );
	}

	if ( *basin )
	{
		if ( !*sub_title_pointer )
		{
			sub_title_pointer +=
				sprintf(sub_title_pointer, "; " );
		}
		sub_title_pointer +=
			sprintf(
				sub_title_pointer,
				"Basin: %s",
				basin );
	}

	if ( *materials )
	{
		if ( !*sub_title_pointer )
		{
			sub_title_pointer +=
				sprintf(sub_title_pointer, "; " );
		}
		sub_title_pointer +=
			sprintf(
				sub_title_pointer,
				"Materials: %s",
				materials );
	}

	if ( *nest_status )
	{
		if ( !*sub_title_pointer )
		{
			sub_title_pointer +=
				sprintf(sub_title_pointer, "; " );
		}
		sub_title_pointer +=
			sprintf(
				sub_title_pointer,
				"Nest Status: %s",
				nest_status );
	}

	format_initial_capital( sub_title, sub_title );

} /* get_title_and_sub_title() */

boolean generate_data(
				char *data_filename,
				char *application_name,
				char *begin_discovery_date_string,
				char *end_discovery_date_string,
				char *transect_number,
				char *habitat,
				char *basin,
				char *materials,
				char *nest_status )
{
	char sys_string[ 65536 ];
	char *nest_table_name;
	char *nest_observation_table_name;
	char where_clause[ 65536 ];
	char *where_clause_pointer;
	char join_where_clause[ 1024 ];
	char order_clause[ 256 ];
	char select[ 1024 ];
	char *sort_command;
	FILE *output_file;

	if ( ! ( output_file = fopen( data_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR: Cannot open output file %s\n",
			 data_filename );
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	nest_table_name = get_table_name( application_name, "nest" );

	nest_observation_table_name =
		get_table_name( application_name, "nest_visit" );

	sprintf( select,
		 SELECT_TEMPLATE,
		 nest_table_name,
		 nest_table_name,
		 nest_table_name,
		 nest_table_name );

	sprintf( join_where_clause,
		 "%s.nest_number = %s.nest_number			",
		 nest_table_name,
		 nest_observation_table_name );

	sprintf( order_clause,
		 "%s.nest_number,observation_date",
		 nest_table_name );

	where_clause_pointer = where_clause;
	*where_clause_pointer = '\0';

	if ( *begin_discovery_date_string )
	{
		where_clause_pointer +=
			sprintf( where_clause_pointer,
				 " and %s.discovery_date between '%s' and '%s'",
				 nest_table_name,
				 begin_discovery_date_string,
				 end_discovery_date_string );
	}

	if ( *transect_number )
	{
		where_clause_pointer +=
			sprintf( where_clause_pointer,
				 " and transect_number in (%s)",
				 timlib_get_in_clause( transect_number ) );
	}

	if ( *habitat )
	{
		where_clause_pointer +=
			sprintf( where_clause_pointer,
				 " and habitat in (%s)",
				 timlib_get_in_clause( habitat ) );
	}

	if ( *basin )
	{
		where_clause_pointer +=
			sprintf( where_clause_pointer,
				 " and basin in (%s)",
				 timlib_get_in_clause( basin ) );
	}

	if ( *materials )
	{
		where_clause_pointer +=
			sprintf( where_clause_pointer,
				 " and materials in (%s)",
				 timlib_get_in_clause( materials ) );
	}

	if ( *nest_status )
	{
		where_clause_pointer +=
			sprintf( where_clause_pointer,
				 " and nest_status in (%s)",
				 timlib_get_in_clause( nest_status ) );
	}

	sort_command = "sort -n -k2 -t'-'";

	sprintf( sys_string,
		 "echo	\"select %s		 "
		 "	  from %s,%s		 "
		 "	  where %s %s		 "
		 "	  order by %s;\"	|"
		 "sql.e '|'			|"
		 "%s > %s			 ",
		 select,
		 nest_table_name,
		 nest_observation_table_name,
		 join_where_clause,
		 where_clause,
		 order_clause,
		 sort_command,
		 data_filename );

	system( sys_string );
	return timlib_file_populated( data_filename );

} /* generate_data() */

