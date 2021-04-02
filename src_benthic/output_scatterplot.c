/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_benthic/output_scatterplot.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "query.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hash_table.h"
#include "html_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "column.h"
#include "grace.h"
#include "regression.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
char *get_dependent_variable_heading(
				char *counts_measurements,
				char *dependent_variable_folder,
				char *vegetation_habitat );

void output_covariance(		FILE *input_pipe,
				char *independent_variable,
				char *counts_measurements,
				pid_t process_id,
				char *dependent_variable_folder,
				char *vegetation_habitat );

FILE *get_input_pipe(		char **independent_variable,
				char **dependent_variable_folder,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *datatype,
				char *vegetation_name,
				char *vegetation_habitat,
				char *substrate,
				char *counts_measurements );

void output_spreadsheet(	FILE *input_pipe,
				char *application_name,
				char *independent_variable,
				char *counts_measurements,
				char *sub_title,
				char *document_root_directory,
				char *dependent_variable_folder,
				char *vegetation_habitat,
				char *process_name );

void output_table(		FILE *input_pipe,
				char *independent_variable,
				char *counts_measurements,
				char *dependent_variable_folder,
				char *vegetation_habitat );

boolean output_scatterplot(	FILE *input_pipe,
				char *application_name,
				char *independent_variable,
				char *counts_measurements,
				char *sub_title,
				char *appaserver_mount_point,
				pid_t process_id,
				char *dependent_variable_folder,
				char *vegetation_habitat );

boolean get_title_and_sub_title(char *title,
				char *sub_title,
				char *process_name,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *datatype,
				char *vegetation_name,
				char *substrate,
				char *begin_date_string,
				char *end_date_string );

int main( int argc, char **argv )
{
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *application_name;
	char *database_string = {0};
	char *process_name;
	char *begin_date_string;
	char *end_date_string;
	char *family_name;
	char *genus_name;
	char *species_name;
	char *datatype;
	char *vegetation_name;
	char *vegetation_habitat;
	char *substrate;
	char *counts_measurements;
	char *output_medium;
	char title[ 128 ];
	char sub_title[ 128 ];
	FILE *input_pipe;
	char *independent_variable = "";
	char *dependent_variable_folder = {0};
	pid_t process_id = getpid();

	if ( argc != 14 )
	{
		fprintf( stderr, 
"Usage: %s application process_name begin_date end_date family genus species datatype vegetation_name vegetation_habitat substrate counts_measurements output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	begin_date_string = argv[ 3 ];
	end_date_string = argv[ 4 ];
	family_name = argv[ 5 ];
	genus_name = argv[ 6 ];
	species_name = argv[ 7 ];
	datatype = argv[ 8 ];
	vegetation_name = argv[ 9 ];
	vegetation_habitat = argv[ 10 ];
	substrate = argv[ 11 ];
	counts_measurements = argv[ 12 ];
	output_medium = argv[ 13 ];

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

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		printf( "<h3>ERROR: invalid date(s).</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( ! ( get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			family_name,
			genus_name,
			species_name,
			datatype,
			vegetation_name,
			substrate,
			begin_date_string,
			end_date_string ) ) )
	{
		printf(
"<h3>Please choose either a datatype, vegetation species, or substrate.</h3>\n" );
		document_close();
		exit( 0 );
	}

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( ! ( input_pipe = get_input_pipe(
				&independent_variable,
				&dependent_variable_folder,
				application_name,
				begin_date_string,
				end_date_string,
				family_name,
				genus_name,
				species_name,
				datatype,
				vegetation_name,
				vegetation_habitat,
				substrate,
				counts_measurements ) ) )
	{
		printf( "<h3>Error: invalid combination.</h3>\n" );
		document_close();
		exit( 0 );
	}


	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_table(	input_pipe,
				independent_variable,
				counts_measurements,
				dependent_variable_folder,
				vegetation_habitat );
	}
	else
	if ( strcmp( output_medium, "scatterplot" ) == 0 )
	{
		if ( !output_scatterplot(
				input_pipe,
				application_name,
				independent_variable,
				counts_measurements,
				sub_title,
				appaserver_parameter_file->
					appaserver_mount_point,
				process_id,
				dependent_variable_folder,
				vegetation_habitat ) )
		{
			printf(
		"<h3>There was nothing selected to display.</h3>\n" );
		}
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_spreadsheet(
				input_pipe,
				application_name,
				independent_variable,
				counts_measurements,
				sub_title,
				appaserver_parameter_file->
					document_root,
				dependent_variable_folder,
				vegetation_habitat,
				process_name );
	}
	else
	if ( strcmp( output_medium, "covariance" ) == 0
	||   strcmp( output_medium, "correlation" ) == 0 )
	{
		output_covariance(
				input_pipe,
				independent_variable,
				counts_measurements,
				process_id,
				dependent_variable_folder,
				vegetation_habitat );
	}
	else
	{
		output_table(	input_pipe,
				independent_variable,
				counts_measurements,
				dependent_variable_folder,
				vegetation_habitat );
	}

	pclose( input_pipe );
	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

boolean get_title_and_sub_title(	char *title,
					char *sub_title,
					char *process_name,
					char *family_name,
					char *genus_name,
					char *species_name,
					char *datatype,
					char *vegetation_name,
					char *substrate,
					char *begin_date_string,
					char *end_date_string )
{
	char buffer[ 256 ];

	sprintf( buffer,
		 "%s from %s to %s",
		 process_name,
		 begin_date_string,
		 end_date_string );

	format_initial_capital( title, buffer );

	if ( *datatype
	&&   strcmp( datatype, "datatype" ) != 0 )
	{
		if ( *vegetation_name
		&&   strcmp( vegetation_name, "vegetation_name" ) != 0 )
		{
			sprintf(	sub_title,
					"Correlate %s with %s",
					format_initial_capital(	buffer,
								datatype ),
					vegetation_name );
		}
		else
		{
			sprintf(	sub_title,
					"Correlate %s with %s %s %s",
					format_initial_capital(	buffer,
								datatype ),
					family_name,
					genus_name,
					species_name );
		}
	}
	else
	if ( *vegetation_name
	&&   strcmp( vegetation_name, "vegetation_name" ) != 0 )
	{
		sprintf(	sub_title,
				"Correlate %s with %s %s %s",
				vegetation_name,
				family_name,
				genus_name,
				species_name );
	}
	else
	if ( *substrate
	&&   strcmp( substrate, "substrate" ) != 0 )
	{
		sprintf(	sub_title,
				"Correlate %s with %s %s %s",
				substrate,
				family_name,
				genus_name,
				species_name );
	}
	else
	{
		return 0;
	}

	search_replace_character( sub_title, '_', ' ' );

	return 1;

} /* get_title_and_sub_title() */

void output_table(	FILE *input_pipe,
			char *independent_variable,
			char *counts_measurements,
			char *dependent_variable_folder,
			char *vegetation_habitat )
{
	char input_buffer[ 1024 ];
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char date_buffer[ 16 ];
	char time_buffer[ 16 ];
	char independent_value[ 32 ];
	char dependent_value[ 32 ];
	char *dependent_variable_heading;

	dependent_variable_heading =
		get_dependent_variable_heading(
			counts_measurements,
			dependent_variable_folder,
			vegetation_habitat );

	sprintf(sys_string,
		"html_table.e '' 'Date,Time,%s,%s' '^' left,left,right,right",
		independent_variable,
		dependent_variable_heading );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	date_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	time_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	independent_value, 
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	dependent_value, 
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		fprintf(	output_pipe,
				"%s^%s^%s^%.3lf\n",
				date_buffer,
				time_buffer,
				independent_value,
				atof( dependent_value ) );

	}
	pclose( output_pipe );

} /* output_table() */

void output_spreadsheet(FILE *input_pipe,
			char *application_name,
			char *independent_variable,
			char *counts_measurements,
			char *sub_title,
			char *document_root_directory,
			char *dependent_variable_folder,
			char *vegetation_habitat,
			char *process_name )
{
	char *ftp_filename;
	char *output_filename;
	FILE *output_file;
	char input_buffer[ 1024 ];
	pid_t process_id = getpid();
	char buffer1[ 128 ];
	char buffer2[ 128 ];
	char *dependent_variable_heading;
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

	dependent_variable_heading =
		get_dependent_variable_heading(
			counts_measurements,
			dependent_variable_folder,
			vegetation_habitat );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file, "#%s\n", sub_title );

	fprintf( output_file,
		"Date,Time,\"%s\",\"%s\"\n",
		format_initial_capital( buffer1, independent_variable ),
		format_initial_capital( buffer2, dependent_variable_heading ) );

	while( get_line( input_buffer, input_pipe ) )
	{
		search_replace_character( input_buffer, '^', ',' );
		fprintf( output_file, "%s\n", input_buffer );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_spreadsheet() */

void output_covariance(	FILE *input_pipe,
			char *independent_variable,
			char *counts_measurements,
			pid_t process_id,
			char *dependent_variable_folder,
			char *vegetation_habitat )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *output_pipe;
	char temporary_filename[ 128 ];
	char label_buffer[ 1024 ];
	char format_buffer[ 1024 ];
	char covariance_buffer[ 128 ];
	char correlation_buffer[ 128 ];
	char determination_buffer[ 128 ];
	char count_buffer[ 128 ];
	char average_buffer[ 128 ];
	char standard_deviation_buffer[ 128 ];
	HTML_TABLE *html_table = {0};
	LIST *heading_list;
	LIST *input_list;
	char *input_record;
	char date_buffer[ 16 ];
	char time_buffer[ 16 ];
	char independent_value[ 32 ];
	char dependent_value[ 32 ];
	char *dependent_variable_heading;

	dependent_variable_heading =
		get_dependent_variable_heading(
			counts_measurements,
			dependent_variable_folder,
			vegetation_habitat );

	sprintf( temporary_filename,
		 "/tmp/benthic_scatterplot_%d.dat",
		 process_id );

	/* ----------------------------------------------------------- */
	/* covariance.e receives:				       */
	/* ----------------------------------------------------------- */
	/* Sample input: "2006-04-20/1217^bottom_salinity_PPT^24.6"    */
	/* Sample input: "2006-04-20/1217^measurements^12.2"	       */
	/* ----------------------------------------------------------- */
	sprintf( sys_string,
		 "covariance.e '%s' 0 1 2 '^'			|"
		 "cat > %s					 ",
		 independent_variable,
		 temporary_filename );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	date_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	time_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	independent_value, 
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	dependent_value, 
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		fprintf(	output_pipe,
				"%s/%s^%s^%s\n",
				date_buffer,
				time_buffer,
				independent_variable,
				independent_value );

		fprintf(	output_pipe,
				"%s/%s^%s^%s\n",
				date_buffer,
				time_buffer,
				dependent_variable_heading,
				dependent_value );
	}

	pclose( output_pipe );

	sprintf( sys_string, "cat %s", temporary_filename );
	input_list = pipe2list( sys_string );

	if ( !list_rewind( input_list ) )
	{
		printf( "<h2>ERROR: input error for sys_string = (%s)</h2>\n",
			sys_string );
		return;
	}

 	html_table = new_html_table(
			(char *)0 /* title */,
			(char *)0 /* sub_title */ );

	heading_list = new_list();

	list_append_pointer( heading_list, "Variables" );
	list_append_pointer( heading_list, "Correlation (r)" );
	list_append_pointer( heading_list, "Determination (r^2)" );
	list_append_pointer( heading_list, "Covariance" );
	list_append_pointer( heading_list, "Average" );
	list_append_pointer( heading_list, "Count" );
	list_append_pointer( heading_list, "Standard Deviation" );

	html_table_set_heading_list( html_table, heading_list );

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 6;

	html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	do {
		input_record = list_get_pointer( input_list );

		piece( label_buffer, ':', input_record, 0 );
		piece( covariance_buffer, ':', input_record, 1 );
		piece( count_buffer, ':', input_record, 2 );
		piece( average_buffer, ':', input_record, 3 );
		piece( standard_deviation_buffer, ':', input_record, 4 );
		piece( correlation_buffer, ':', input_record, 5 );

		html_table_set_data(	html_table->data_list,
					strdup( format_initial_capital(
						format_buffer,
						label_buffer ) ) );

		html_table_set_data(	html_table->data_list,
					strdup( correlation_buffer ) );

		sprintf(	determination_buffer,
				"%lf",
				atof( correlation_buffer ) *
				atof( correlation_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( determination_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( covariance_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( average_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( count_buffer ) );

		html_table_set_data(	html_table->data_list,
					strdup( standard_deviation_buffer ) );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();
	} while( list_next( input_list ) );

	sprintf( sys_string, "rm -f %s", temporary_filename );
	system( sys_string );

	html_table_close();
	document_close();

} /* output_covariance() */

FILE *get_input_pipe(		char **independent_variable,
				char **dependent_variable_folder,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *datatype,
				char *vegetation_name,
				char *vegetation_habitat,
				char *substrate,
				char *counts_measurements )
{
	char sys_string[ 1024 ];

	if ( *datatype
	&&   strcmp( datatype, "datatype" ) != 0
	&&   strcmp( vegetation_name, "vegetation_name" ) != 0 )
	{
		*independent_variable = datatype;
		*dependent_variable_folder = "vegetation_species";

		if ( strcmp( vegetation_habitat, "height" ) == 0 )
		{
			sprintf( sys_string,
"select_environment_cross_height.sh %s %s %s \"%s\" \"%s\"",
			 	application_name,
			 	begin_date_string,
			 	end_date_string,
			 	datatype,
			 	vegetation_name );
		}
		else
		{
			sprintf( sys_string,
"select_environment_cross_abundance.sh %s %s %s \"%s\" \"%s\"",
			 	application_name,
			 	begin_date_string,
			 	end_date_string,
			 	datatype,
			 	vegetation_name );
		}

		return popen( sys_string, "r" );
	}
	else
	if ( *datatype
	&&   strcmp( datatype, "datatype" ) != 0
	&&   strcmp( counts_measurements, "measurements" ) == 0 )
	{
		*independent_variable = datatype;
		sprintf( sys_string,
"select_environment_cross_measurement.sh %s %s %s \"%s\" \"%s\" \"%s\" \"%s\"",
			 application_name,
			 begin_date_string,
			 end_date_string,
			 datatype,
			 family_name,
			 genus_name,
			 species_name );
		return popen( sys_string, "r" );
	}
	else
	if ( *datatype
	&&   strcmp( datatype, "datatype" ) != 0
	&&   strcmp( counts_measurements, "counts" ) == 0 )
	{
		*independent_variable = datatype;
		sprintf( sys_string,
"select_environment_cross_count.sh %s %s %s \"%s\" \"%s\" \"%s\" \"%s\"",
			 application_name,
			 begin_date_string,
			 end_date_string,
			 datatype,
			 family_name,
			 genus_name,
			 species_name );
		return popen( sys_string, "r" );
	}
	else
	if ( *vegetation_name
	&&   strcmp( vegetation_name, "vegetation_name" ) != 0
	&&   strcmp( counts_measurements, "measurements" ) == 0 )
	{
		*independent_variable = vegetation_name;

		if ( strcmp( vegetation_habitat, "height" ) == 0 )
		{
			sprintf( sys_string,
"select_seagrass_height_cross_measurement.sh %s %s %s \"%s\" \"%s\" \"%s\" \"%s\"",
			 	application_name,
			 	begin_date_string,
			 	end_date_string,
			 	vegetation_name,
			 	family_name,
				genus_name,
				species_name );
		}
		else
		{
			char *total_indicator;

			if ( instr( "total", vegetation_name, 1 ) >= 0 )
			{
				total_indicator = "total";
			}
			else
			{
				total_indicator = "";
			}

			sprintf( sys_string,
"select_seagrass_abundance_cross_measurement.sh %s %s %s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
		 		application_name,
		 		begin_date_string,
		 		end_date_string,
		 		vegetation_name,
		 		family_name,
		 		genus_name,
		 		species_name,
				total_indicator );
		}
		return popen( sys_string, "r" );
	}
	else
	if ( *vegetation_name
	&&   strcmp( vegetation_name, "vegetation_name" ) != 0
	&&   strcmp( counts_measurements, "counts" ) == 0 )
	{
		*independent_variable = vegetation_name;

		if ( strcmp( vegetation_habitat, "height" ) == 0 )
		{
			sprintf( sys_string,
"select_seagrass_height_cross_count.sh %s %s %s \"%s\" \"%s\" \"%s\" \"%s\"",
			 	application_name,
			 	begin_date_string,
			 	end_date_string,
			 	vegetation_name,
			 	family_name,
				genus_name,
				species_name );
		}
		else
		{
			char *total_indicator;

			if ( instr( "total", vegetation_name, 1 ) >= 0 )
			{
				total_indicator = "total";
			}
			else
			{
				total_indicator = "";
			}

			sprintf( sys_string,
"select_seagrass_abundance_cross_count.sh %s %s %s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
			 	application_name,
			 	begin_date_string,
			 	end_date_string,
			 	vegetation_name,
			 	family_name,
				genus_name,
				species_name,
				total_indicator );
		}
		return popen( sys_string, "r" );
	}
	else
	if ( *substrate
	&&   strcmp( substrate, "substrate" ) != 0
	&&   strcmp( counts_measurements, "measurements" ) == 0 )
	{
		*independent_variable = substrate;
		sprintf( sys_string,
"select_substrate_cross_measurement.sh %s %s %s \"%s\" \"%s\" \"%s\" \"%s\"",
			 application_name,
			 begin_date_string,
			 end_date_string,
			 substrate,
			 family_name,
			 genus_name,
			 species_name );
		return popen( sys_string, "r" );
	}
	else
	if ( *substrate
	&&   strcmp( datatype, "substrate" ) != 0
	&&   strcmp( counts_measurements, "counts" ) == 0 )
	{
		*independent_variable = substrate;
		sprintf( sys_string,
"select_substrate_cross_count.sh %s %s %s \"%s\" \"%s\" \"%s\" \"%s\"",
			 application_name,
			 begin_date_string,
			 end_date_string,
			 substrate,
			 family_name,
			 genus_name,
			 species_name );
		return popen( sys_string, "r" );
	}
	else
	{
		return (FILE *)0;
	}
} /* get_input_pipe() */

boolean output_scatterplot(
			FILE *input_pipe,
			char *application_name,
			char *independent_variable,
			char *counts_measurements,
			char *sub_title,
			char *appaserver_mount_point,
			pid_t process_id,
			char *dependent_variable_folder,
			char *vegetation_habitat )
{
	char input_buffer[ 1024 ];
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char independent_value[ 128 ];
	char dependent_value[ 128 ];
	char grace_scatterplot_output_file[ 128 ];
	char statistics_filename[ 128 ];
	FILE *input_file;
	char ftp_agr_filename[ 128 ];
	char ftp_output_filename[ 128 ];
	char output_filename[ 128 ];
	char buffer1[ 128 ];
	char buffer2[ 128 ];
	char *dependent_variable_heading;

	dependent_variable_heading =
		get_dependent_variable_heading(
			counts_measurements,
			dependent_variable_folder,
			vegetation_habitat );

	sprintf(grace_scatterplot_output_file,
		"/tmp/output_scatterplot_%d",
		process_id );

	sprintf(	sys_string,
			"grace_scatterplot.e %s %d '%s' '' '%s' '%s' > %s",
			application_name,
			process_id,
			sub_title,
			format_initial_capital( buffer1, independent_variable ),
			format_initial_capital(	buffer2,
						dependent_variable_heading ),
			grace_scatterplot_output_file );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	independent_value, 
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	dependent_value, 
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		fprintf(output_pipe,
			"%s %s\n",
			independent_value,
			dependent_value );

	}
	pclose( output_pipe );

	if ( ! ( input_file = fopen( grace_scatterplot_output_file, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 grace_scatterplot_output_file );
		exit( 1 );
	}

	if ( !get_line( input_buffer, input_file ) ) return 0;

	column( ftp_agr_filename, 0, input_buffer );
	column( ftp_output_filename, 1, input_buffer );
	column( output_filename, 2, input_buffer );
	column( statistics_filename, 3, input_buffer );

	fclose( input_file );

	if ( strcmp( ftp_agr_filename, "null" ) == 0 )
	{
		printf(
	"<h3>ERROR: could not generate a plot. Try a smaller dataset.</h3>\n" );
	}
	else
	{
		grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				appaserver_mount_point,
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );
	}

	/* Output the statistics */
	/* --------------------- */
	regression_output_statistics_table( statistics_filename );

	sprintf( sys_string,
		 "rm -f %s",
		 grace_scatterplot_output_file );
	system( sys_string );

	sprintf( sys_string,
		 "rm -f %s",
		 statistics_filename );
	system( sys_string );

	return 1;
} /* output_scatterplot() */

char *get_dependent_variable_heading(
			char *counts_measurements,
			char *dependent_variable_folder,
			char *vegetation_habitat )
{
	char *dependent_variable_heading;

	if ( timlib_strcmp(	dependent_variable_folder,
				"vegetation_species" ) == 0 )
	{
		if ( timlib_strcmp( vegetation_habitat, "height" ) == 0 )
			dependent_variable_heading = "height";
		else
			dependent_variable_heading = "abundance_rating";
	}
	else
	{
		dependent_variable_heading =
			(strcmp( counts_measurements, "counts" ) == 0 )
				? "Counts" : "Length";
	}
	return dependent_variable_heading;

} /* get_dependent_variable_heading() */

