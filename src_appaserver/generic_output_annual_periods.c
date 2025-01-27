/* --------------------------------------------------- 	*/
/* src_appaserver/generic_output_annual_periods.c      	*/
/* --------------------------------------------------- 	*/
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
#include "environ.h"
#include "hash_table.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "application.h"
#include "list_usage.h"
#include "aggregate_statistic.h"
#include "grace.h"
#include "session.h"
#include "process_generic_output.h"
#include "date_convert.h"
#include "appaserver_link.h"

/* Constants */
/* --------- */
#define EXCEEDANCE_DELIMITER		'|'
#define DEFAULT_OUTPUT_MEDIUM		"table"
#define GRACE_DATATYPE_ENTITY_PIECE	0
#define GRACE_DATATYPE_PIECE		1
#define GRACE_DATE_PIECE		2
#define GRACE_TIME_PIECE		-1
#define GRACE_VALUE_PIECE		3
#define KEY_DELIMITER			'/'
#define FILENAME_STEM			"annual_periods"

#define ROWS_BETWEEN_HEADING			20
#define DATE_PIECE		 		0
#define VALUE_PIECE		 		1
#define INPUT_DELIMITER				','
#define PIECE_DELIMITER				'|'

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void piece_exceedance_variables(
					char **value_string,
					char **date_string,
					char **count_below_string,
					char **percent_below_string,
					char **aggregation_count_string,
					char *input_buffer );

void annual_periods_output_table_exceedance_format(
					char *sys_string,
					enum aggregate_statistic,
					char *aggregate_period_string,
					char *period_string,
					char *value_folder,
					char *where_clause,
					char *application_name );

int annual_periods_output_chart_exceedance_format(
					char *application_name,
					char *datatype_name,
					char *begin_date_string,
					char *end_date_string,
					char *sys_string,
					char *document_root_directory,
					char *appaserver_mount_point,
					char *aggregate_period_string,
					char *period_string,
					enum aggregate_statistic,
					char *units,
					char *value_folder,
					char *where_clause );

int annual_periods_output_chart(
					char *application_name,
					char *datatype_name,
					char *begin_date_string,
					char *end_date_string,
					char *sys_string,
					char *document_root_directory,
					char *appaserver_mount_point,
					char *argv_0,
					char *aggregate_period_string,
					char *period_string,
					boolean datatype_bar_graph,
					boolean datatype_scale_graph_zero,
					char *units,
					char *value_folder,
					char *where_clause,
					LIST *foreign_attribute_data_list );

void annual_periods_output_table(
					char *sys_string,
					enum aggregate_statistic,
					char *aggregate_period_string,
					char *period_string,
					char *value_folder,
					char *where_clause,
					char *application_name );

void annual_periods_output_transmit(
					FILE *output_pipe,
					char *sys_string,
					enum aggregate_statistic );

void annual_periods_output_transmit_exceedance_format(
					FILE *output_pipe,
					char *sys_string );

void build_sys_string(
			char *sys_string,
			char *application_name,
			enum aggregate_statistic,
			char *aggregate_period_string,
			char *period_string,
			char *exceedance_format_yn,
			char *value_folder,
			char *date_attribute,
			char *value_attribute,
			char *where_clause );

int main( int argc, char **argv )
{
	char *application_name;
	char *parameter_dictionary_string;
	char *begin_date_string = {0};
	char *end_date_string = {0};
	char *output_medium;
	DOCUMENT *document = {0};
	char sys_string[ 1024 ];
	char *period_string = {0};
	enum aggregate_statistic aggregate_statistic;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *parameter_dictionary;
	char *aggregate_statistic_string;
	char *aggregate_period_string = {0};
	char *month_string;
	char *season_string;
	char *containing_date_string;
	char *exceedance_format_yn;
	char *process_name;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *where_clause;
	DATE_CONVERT *date_convert;
	char buffer[ 128 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s ignored ignored ignored ignored process parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	/* login_name = argv[ 1 ]; */
	/* role_name = argv[ 2 ]; */
	/* session = argv[ 3 ]; */

	process_name = argv[ 5 ];
	parameter_dictionary_string = argv[ 6 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( ! ( parameter_dictionary = 
		dictionary_index_string2dictionary(
			parameter_dictionary_string ) ) )
	{
		fprintf( stderr,
"Error in %s: invalid dictionary. It needs to contain both '&' and '='.\n",
			 parameter_dictionary_string );
		exit( 1 );
	}

	dictionary_add_elements_by_removing_prefix(
			    		parameter_dictionary,
			    		QUERY_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
			    		parameter_dictionary,
			    		QUERY_FROM_STARTING_LABEL );

	process_generic_output =
		process_generic_output_new(
			application_name,
			process_name,
			(char *)0 /* process_set_name */,
			0 /* accumulate_flag */ );


	process_generic_output->value_folder->datatype =
		process_generic_datatype_new(
			application_name,
			process_generic_output->
				value_folder->
					foreign_folder->
						foreign_attribute_name_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						datatype_folder_name,
			process_generic_output->
				value_folder->
					datatype_folder->
						primary_key_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_aggregation_sum,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_bar_graph,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_scale_graph_zero,
			process_generic_output->
				value_folder->
					units_folder_name,
			parameter_dictionary,
			0 /* dictionary_index */ );


	if ( !process_generic_output->value_folder->datatype )
	{
		document_basic_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot find datatype information for (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display(
				process_generic_output->
				value_folder->
				datatype_folder->
				primary_key_list ) );
		printf(
	"<h3>ERROR: cannot find datatype information for (%s).</h3>\n",
			list_display(
				process_generic_output->
				value_folder->
				datatype_folder->
				primary_key_list ) );
		document_close();
		exit( 0 );
	}

	if ( !	process_generic_output->
		value_folder->
		datatype->
		foreign_attribute_data_list )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot get foreign_attribute_data_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	dictionary_get_index_data( 	&output_medium,
					parameter_dictionary,
					"output_medium",
					0 );

	if (	!output_medium
	|| 	!*output_medium
	|| 	strcmp( output_medium, "select" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	if ( !process_generic_output_validate_begin_end_date(
			&begin_date_string /* in/out */,
			&end_date_string /* in/out */,
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			process_generic_output->
				value_folder->
				date_attribute_name,
			parameter_dictionary
				/* query_removed_post_dictionary */ ) )
	{
		document_basic_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n",
			format_initial_capital( buffer, process_name ) );

		printf( "<h3>ERROR: no data available from %s to %s.</h3>\n",
			begin_date_string,
			end_date_string );
		document_close();
		exit( 0 );
	}

	dictionary_get_index_data( 	&aggregate_statistic_string,
					parameter_dictionary,
					"aggregate_statistic",
					0 );

	dictionary_get_index_data( 	&aggregate_period_string,
					parameter_dictionary,
					"aggregate_period",
					0 );

	if ( !aggregate_period_string
	||   !*aggregate_period_string
	||   strcmp( aggregate_period_string, "aggregate_period" ) == 0 )
	{
		document_basic_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n",
			format_initial_capital( buffer, process_name ) );

		printf(
		"<h3>ERROR: please select an aggregate period.</h3>\n" );
		document_close();
		exit( 0 );
	}

	dictionary_get_index_data( 	&month_string,
					parameter_dictionary,
					"month",
					0 );

	dictionary_get_index_data( 	&season_string,
					parameter_dictionary,
					"season",
					0 );

	dictionary_get_index_data( 	&containing_date_string,
					parameter_dictionary,
					"containing_date",
					0 );

	/* Allow for "today", "yesterday", ... */
	/* ----------------------------------- */
	date_convert =
		date_convert_new_date_convert(
				date_convert_unknown,
				containing_date_string );
	containing_date_string = date_convert->return_date;

	dictionary_get_index_data( 	&exceedance_format_yn,
					parameter_dictionary,
					"exceedance_format_yn",
					0 );

	where_clause = process_generic_output_get_dictionary_where_clause(
			&begin_date_string,
			&end_date_string,
			application_name,
			process_generic_output,
			parameter_dictionary,
			1 /* with_set_dates */,
			process_generic_output->
				value_folder->
				value_folder_name );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
						aggregate_statistic_string,
						daily );

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic = average;
	}

	if ( strcmp( aggregate_period_string, "annually" ) == 0 )
		period_string = "annually";
	else
	if ( strcmp( aggregate_period_string, "seasonally" ) == 0 )
		period_string = season_string;
	else
	if ( strcmp( aggregate_period_string, "monthly" ) == 0 )
		period_string = month_string;
	else
	if ( strcmp( aggregate_period_string, "weekly" ) == 0 )
		period_string = containing_date_string;
	else
	if ( strcmp( aggregate_period_string, "daily" ) == 0 )
		period_string = containing_date_string;
	else
		period_string = (char *)0;

	if ( !period_string || !*period_string )
	{
		document_basic_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n",
			format_initial_capital( buffer, process_name ) );

		if ( strcmp( aggregate_period_string, "daily" ) == 0
		||   strcmp( aggregate_period_string, "weekly" ) == 0 )
		{
			printf(
			"<h3>ERROR: please enter a containing date.</h3>\n" );
		}
		else
		if ( strcmp( aggregate_period_string, "monthly" ) == 0 )
		{
			printf( "<h3>ERROR: please select a month.</h3>\n" );
		}
		else
		if ( strcmp( aggregate_period_string, "seasonally" ) == 0 )
		{
			printf( "<h3>ERROR: please select a season.</h3>\n" );
		}

		document_close();
		exit( 1 );
	}

	build_sys_string(	sys_string,
				application_name,
				aggregate_statistic,
				aggregate_period_string,
				period_string,
				exceedance_format_yn,
				process_generic_output->
					value_folder->
					value_folder_name,
				process_generic_output->
					value_folder->
					date_attribute_name,
				process_generic_output->
					value_folder->
					value_attribute_name,
				where_clause );

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   strcmp( output_medium, "chart" ) != 0 )
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
	}

	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		int results = 0;

		if ( *exceedance_format_yn == 'y' )
		{
			results =
			annual_periods_output_chart_exceedance_format(
					application_name,
					process_generic_get_datatype_name(
					   process_generic_output->
					   value_folder->
					   datatype->
					   primary_attribute_data_list,
					   ' ' /* delimiter */ ),
					begin_date_string,
					end_date_string,
					sys_string,
					appaserver_parameter_file->
						document_root,
					appaserver_parameter_file->
						appaserver_mount_point,
					aggregate_period_string,
					period_string,
					aggregate_statistic,
					process_generic_output->
						value_folder->
						datatype->
						units,
					process_generic_output->
						value_folder->
						value_folder_name,
					where_clause );
		}
		else
		{
			results =
			annual_periods_output_chart(
				application_name,
				process_generic_get_datatype_name(
					process_generic_output->
					value_folder->
					datatype->
					primary_attribute_data_list,
					' ' /* delimiter */ ),
				begin_date_string,
				end_date_string,
				sys_string,
				appaserver_parameter_file->
					document_root,
				appaserver_parameter_file->
					appaserver_mount_point,
				argv[ 0 ],
				aggregate_period_string,
				period_string,
				process_generic_output->
					value_folder->
					datatype->
					bar_graph,
				process_generic_output->
					value_folder->
					datatype->
					scale_graph_zero,
				process_generic_output->
					value_folder->
					datatype->
					units,
				process_generic_output->
					value_folder->
					value_folder_name,
				where_clause,
				process_generic_output->
					value_folder->
					datatype->
					foreign_attribute_data_list );
		}

		if ( !results )
		{
			printf( "<p>Warning: nothing selected to output.\n" );
			document_close();
			exit( 0 ); 
		}
	}
	else
	if ( strcmp( output_medium, "table" ) == 0 )
	{
		if ( *exceedance_format_yn == 'y' )
		{
			annual_periods_output_table_exceedance_format(
					sys_string,
					aggregate_statistic,
					aggregate_period_string,
					period_string,
					process_generic_output->
						value_folder->
						value_folder_name,
					where_clause,
					application_name );
		}
		else
		{
			annual_periods_output_table(
					sys_string,
					aggregate_statistic,
					aggregate_period_string,
					period_string,
					process_generic_output->
						value_folder->
						value_folder_name,
					where_clause,
					application_name );
		}
	}
	else
	if ( strcmp( output_medium, "transmit" ) == 0
	||   strcmp( output_medium, "text_file" ) == 0 )
	{
		char *output_filename;
		char *ftp_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		FILE *output_file;
		char output_sys_string[ 512 ];
		char buffer[ 128 ];
		APPASERVER_LINK *appaserver_link;

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				appaserver_parameter_file->
	 				document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"txt" );

		output_filename = appaserver_link->output->filename;

		ftp_filename = appaserver_link->prompt->filename;

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

/*
		if ( *exceedance_format_yn == 'y' )
		{
			sprintf( output_sys_string,
			 	"delimiter2padded_columns.e '|' 2 > %s",
			 	output_filename );
		}
		else
		{
			sprintf( output_sys_string,
			 	"delimiter2padded_columns.e '|' > %s",
			 	output_filename );
		}
*/
		sprintf(output_sys_string,
		 	"tr '|' '%c' > %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_filename );

		output_pipe = popen( output_sys_string, "w" );

		if ( *exceedance_format_yn == 'y' )
		{
			annual_periods_output_transmit_exceedance_format(
						output_pipe,
						sys_string );
		}
		else
		{
			annual_periods_output_transmit(
						output_pipe,
						sys_string,
						aggregate_statistic );
		}

		pclose( output_pipe );

		printf( "<h1>%s Annual Periods Transmission<br></h1>\n",
			format_initial_capital(	buffer,
						process_generic_output->
							value_folder->
							value_folder_name ) );
		printf( "<h1>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h1>\n" );

		printf( "<br>Search criteria: %s\n",
			query_get_display_where_clause(
				where_clause,
				application_name,
				process_generic_output->
					value_folder->value_folder_name,
				1 ) );

		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		char output_sys_string[ 512 ];

/*
		sprintf( output_sys_string,
			 "delimiter2padded_columns.e '|'" );
*/
		sprintf(output_sys_string,
		 	"tr '|' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( output_sys_string, "w" );

		if ( *exceedance_format_yn == 'y' )
		{
			annual_periods_output_transmit_exceedance_format(
						output_pipe,
						sys_string );
		}
		else
		{
			annual_periods_output_transmit(
						output_pipe,
						sys_string,
						aggregate_statistic );
		}

		pclose( output_pipe );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}
	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

void annual_periods_output_transmit(
				FILE *output_pipe,
				char *sys_string,
				enum aggregate_statistic aggregate_statistic )
{
	char buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	char count_string[ 128 ];
	FILE *input_pipe;

	fprintf(	output_pipe,
			"#Date|%s|Count\n",
			format_initial_capital(
				buffer,
				aggregate_statistic_get_string(
					aggregate_statistic ) ) );

	input_pipe = popen( sys_string, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, buffer, 0 );
		piece( value_string, INPUT_DELIMITER, buffer, 1 );
		piece( count_string, INPUT_DELIMITER, buffer, 2 );

		fprintf( output_pipe,
			 "%s|%s|%s\n",
			 date_string,
			 value_string,
			 count_string );
	}

	pclose( input_pipe );

} /* annual_periods_output_transmit() */

void annual_periods_output_table(
				char *sys_string,
				enum aggregate_statistic aggregate_statistic,
				char *aggregate_period_string,
				char *period_string,
				char *value_folder,
				char *where_clause,
				char *application_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char buffer[ 2048 ];
	char title[ 2048 ];
	char title_buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	char count_string[ 128 ];
	int count = 0;

	sprintf(title,
		"%s Annual %s %s for %s<br>Search criteria: %s",
		format_initial_capital( buffer, value_folder ),
		aggregate_period_string,
	 	aggregate_statistic_get_string(
			aggregate_statistic ),
		period_string,
		query_get_display_where_clause(
			where_clause,
			application_name,
			value_folder,
			1 ) );

	html_table = new_html_table(
			format_initial_capital(
				title_buffer,
				title ),
			(char *)0 /* sub_title */ );
	heading_list = new_list();

	list_append_pointer( heading_list, "Date" );

	list_append_pointer(	heading_list,
				strdup( format_initial_capital(
					buffer,
					aggregate_statistic_get_string(
						aggregate_statistic ) ) ) );

	list_append_pointer( heading_list, "Count" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 2;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, input_buffer, 0 );
		piece( value_string, INPUT_DELIMITER, input_buffer, 1 );
		piece( count_string, INPUT_DELIMITER, input_buffer, 2 );

		html_table_set_data(	html_table->data_list,
					strdup( date_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( value_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( count_string ) );

		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
		}

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

	}

	pclose( input_pipe );
	html_table_close();

} /* annual_periods_output_table() */

void build_sys_string(
		char *sys_string,
		char *application_name,
		enum aggregate_statistic aggregate_statistic,
		char *aggregate_period_string,
		char *period_string,
		char *exceedance_format_yn,
		char *value_folder,
		char *date_attribute,
		char *value_attribute,
		char *where_clause )
{
	char aggregation_process[ 1024 ];
	char exceedance_process[ 1024 ];

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic = average;
/*
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				appaserver_mount_point,
				datatype_name,
				aggregate_statistic_none );
*/
	}

	if ( *exceedance_format_yn == 'y' )
	{
		sprintf(	exceedance_process,
				"piece_shift_left.e '%c'	|"
				"piece_exceedance.e '%c'	|"
				"sed 's/%c/%c/g'		|"
				"cat				 ",
				INPUT_DELIMITER,
				INPUT_DELIMITER,
				INPUT_DELIMITER,
				EXCEEDANCE_DELIMITER );
	}
	else
	{
		strcpy( exceedance_process, "cat" );
	}

	sprintf( 	aggregation_process, 
	 "real_time2annual_period.e %s %s %d %d '%c' %s %s",
	 		application_name,
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
	 		DATE_PIECE,
	 		VALUE_PIECE,
			INPUT_DELIMITER,
			aggregate_period_string,
			period_string );

	sys_string += sprintf( sys_string,
	"get_folder_data	application=%s		    "
	"			folder=%s		    "
	"			select=\"%s,%s\"	    "
	"			where=\"%s\"		   |"
	"tr '%c' '%c' 					   |"
	"%s						   |"
	"%s						   |"
	"cat						    ",
		application_name,
		value_folder,
		date_attribute,
		value_attribute,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		aggregation_process,
		exceedance_process );

} /* build_sys_string() */

int annual_periods_output_chart(
				char *application_name,
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string,
				char *document_root_directory,
				char *appaserver_mount_point,
				char *argv_0,
				char *aggregate_period_string,
				char *period_string,
				boolean datatype_bar_graph,
				boolean datatype_scale_graph_zero,
				char *units,
				char *value_folder,
				char *where_clause,
				LIST *foreign_attribute_data_list )
{
	char input_buffer[ 512 ];
	char station_datatype_input_buffer[ 512 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	char buffer1[ 512 ];
	GRACE *grace;
	char graph_identifier[ 16 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	FILE *input_pipe;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char legend[ 128 ];

	if ( strcmp( period_string, "annually" ) == 0 )
	{
		sprintf( title,
		 	"%s Annually",
		 	value_folder );
	}
	else
	{
		sprintf( title,
		 	"%s %s Annual Periods for %s",
		 	value_folder,
		 	aggregate_period_string,
		 	period_string );
	}

	format_initial_capital( title, title );

	sprintf(	sub_title,
			"%s From: %s To: %s",
			list_display_delimited(
				foreign_attribute_data_list,
				'/' ),
			begin_date_string,
			end_date_string );

	format_initial_capital( sub_title, sub_title );

	grace = grace_new_unit_graph_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				title,
				sub_title,
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	if ( !grace_set_begin_end_date(grace,
					begin_date_string,
					end_date_string ) )
	{
		document_basic_output_body(
			application_name,
			appaserver_mount_point );

		printf(
		"<h3>ERROR: Invalid date format format.</h3>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

	grace_graph->units = units;

	grace_datatype = grace_new_grace_datatype(
					(char *)0 /* datatype_entity_name */,
					strdup( datatype_name ) );

	grace_datatype->nodisplay_legend = 1;

	grace_graph_set_scale_to_zero(
			grace_graph,
			datatype_scale_graph_zero );

	grace_graph->xaxis_ticklabel_angle = 45;

	strcpy( legend, datatype_name );

	strcpy(	legend,
		format_initial_capital( buffer1, legend ) );

	grace_datatype->legend = strdup( legend );

	if ( datatype_bar_graph )
	{
		grace_datatype->line_linestyle = 0;
		grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
	}
	else
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	}

	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	list_append_pointer( grace->graph_list, grace_graph );

	grace->grace_output =
		application_grace_output( application_name );

	sprintf( graph_identifier, "%d", getpid() );

	grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		search_replace_string( input_buffer, ",", "|" );

		sprintf( station_datatype_input_buffer,
			 "|%s|%s",
			 datatype_name,
			 input_buffer );

		grace_set_string_to_point_list(
				grace->graph_list, 
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				station_datatype_input_buffer,
				unit_graph,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );
	}
	pclose( input_pipe );

	if ( !grace_set_structures(
				&page_width_pixels,
				&page_length_pixels,
				&distill_landscape_flag,
				&grace->landscape_mode,
				grace,
				grace->graph_list,
				grace->anchor_graph_list,
				grace->begin_date_julian,
				grace->end_date_julian,
				grace->number_of_days,
				grace->grace_graph_type,
				0 /* not force_landscape_mode */ ) )
	{
		document_basic_output_body(
			application_name,
			appaserver_mount_point );

		printf( "<h3>Warning: no graphs to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	grace->symbols = 1;

	if ( !grace_output_charts(
				output_filename, 
				postscript_filename,
				agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				page_width_pixels,
				page_length_pixels,
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_ghost_script_directory(
					application_name ),
				(LIST *)0 /* quantum_datatype_name_list */,
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list ) )
	{
		printf( "<h3>No data for selected parameters.</h3>\n" );
		document_close();
		exit( 0 );
	}
	else
	{
		grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				appaserver_mount_point,
				1 /* with_document_output */,
				query_get_display_where_clause(
					where_clause,
					application_name,
					value_folder,
					1 ) );
	}
	return 1;
} /* annual_periods_output_chart() */

void annual_periods_output_transmit_exceedance_format(
				FILE *output_pipe,
				char *sys_string )
{
	char buffer[ 512 ];
	FILE *input_pipe;

	fprintf(	output_pipe,
			"#Value|Count|Date|CountBelow|%cBelow\n",
			'%' );

	input_pipe = popen( sys_string, "r" );
	while( get_line( buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", buffer );
	}

	pclose( input_pipe );

} /* annual_periods_output_transmit_exceedance_format() */

void annual_periods_output_table_exceedance_format(
				char *sys_string,
				enum aggregate_statistic aggregate_statistic,
				char *aggregate_period_string,
				char *period_string,
				char *value_folder,
				char *where_clause,
				char *application_name )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char buffer[ 512 ];
	char title[ 512 ];
	char title_buffer[ 512 ];
	int count = 0;
	char *value_string;
	char *date_string;
	char *count_below_string;
	char *percent_below_string;
	char *aggregation_count_string;

	sprintf(	title,
"%s Annual %s %s Exceedance Format for %s <br>Search %s",
			format_initial_capital( buffer, value_folder ),
			aggregate_period_string,
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
			period_string,
			query_get_display_where_clause(
				where_clause,
				application_name,
				value_folder,
				1 ) );

	html_table = new_html_table(
			format_initial_capital(
				title_buffer,
				title ),
			(char *)0 /* sub_title */ );
	heading_list = new_list();

	list_append_pointer(	heading_list,
				strdup( format_initial_capital(
					buffer,
					aggregate_statistic_get_string(
						aggregate_statistic ) ) ) );

	list_append_pointer( heading_list, "Count Below" );
	list_append_pointer( heading_list, "Percent Below" );
	list_append_pointer( heading_list, "Representative Date" );
	list_append_pointer( heading_list, "Aggregation Count" );

	html_table->number_left_justified_columns = 0;
	html_table->number_right_justified_columns = 5;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece_exceedance_variables(
				&value_string,
				&date_string,
				&count_below_string,
				&percent_below_string,
				&aggregation_count_string,
				input_buffer );

		html_table_set_data(	html_table->data_list,
					strdup( value_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( count_below_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( percent_below_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( date_string ) );

		html_table_set_data(	html_table->data_list,
					strdup( aggregation_count_string ) );

		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
		}

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

	}

	pclose( input_pipe );
	html_table_close();

} /* annual_periods_output_table_exceedance_format() */

void piece_exceedance_variables(
				char **value_string,
				char **date_string,
				char **count_below_string,
				char **percent_below_string,
				char **aggregation_count_string,
				char *input_buffer )
{
	static char local_value_string[ 128 ];
	static char local_date_string[ 128 ];
	static char local_count_below_string[ 128 ];
	static char local_percent_below_string[ 128 ];
	static char local_aggregation_count_string[ 128 ];

	piece(	local_value_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		0 );
	piece(	local_aggregation_count_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		1 );
	piece(	local_date_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		2 );
	piece(	local_count_below_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		3 );
	piece(	local_percent_below_string,
		EXCEEDANCE_DELIMITER,
		input_buffer,
		4 );
	*value_string = local_value_string;
	*date_string = local_date_string;
	*count_below_string = local_count_below_string;
	*percent_below_string = local_percent_below_string;
	*aggregation_count_string = local_aggregation_count_string;
} /* piece_exceedance_variables() */

int annual_periods_output_chart_exceedance_format(
				char *application_name,
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string,
				char *document_root_directory,
				char *appaserver_mount_point,
				char *aggregate_period_string,
				char *period_string,
				enum aggregate_statistic aggregate_statistic,
				char *units,
				char *value_folder,
				char *where_clause )
{
	GRACE *grace;
	char title[ 512 ];
	char sub_title[ 512 ];
	char input_buffer[ 512 ];
	char buffer1[ 512 ];
	char buffer2[ 512 ];
	char legend[ 128 ];
	char graph_identifier[ 16 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	GRACE_GRAPH *grace_graph;
	char *value_string;
	char *date_string;
	char *count_below_string;
	char *percent_below_string;
	char *aggregation_count_string;
	FILE *input_pipe;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;

	sprintf( title,
		 "%s %s Annual Periods for %s",
		 value_folder,
		 aggregate_period_string,
		 period_string );

	format_initial_capital( title, title );

	sprintf(sub_title,
		"%s %s from %s to %s Exceedance Format",
			format_initial_capital( buffer1, datatype_name ),
			format_initial_capital( buffer2,
				aggregate_statistic_get_string(
					aggregate_statistic ) ),
			begin_date_string,
			end_date_string );

	grace = grace_new_xy_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				title,
				sub_title,
				units,
				format_initial_capital(
					legend, datatype_name ) );

	grace->xaxis_ticklabel_precision = 0;
	grace->world_min_x = 0.0;
	grace->world_max_x = 100.0;

	sprintf( graph_identifier, "%d", getpid() );

	grace->grace_output =
			application_grace_output( application_name );

	grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece_exceedance_variables(
				&value_string,
				&date_string,
				&count_below_string,
				&percent_below_string,
				&aggregation_count_string,
				input_buffer );

		grace_set_xy_to_point_list(
				grace->graph_list, 
				atof( percent_below_string ),
				strdup( value_string ),
				(char *)0 /* optional_label */,
				grace->dataset_no_cycle_color );

	}

	pclose( input_pipe );

	grace_graph =
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list );
	grace_graph->xaxis_label = "Percent Below";

	if ( !grace_set_structures(
				&page_width_pixels,
				&page_length_pixels,
				&distill_landscape_flag,
				&grace->landscape_mode,
				grace,
				grace->graph_list,
				grace->anchor_graph_list,
				grace->begin_date_julian,
				grace->end_date_julian,
				grace->number_of_days,
				grace->grace_graph_type,
				0 /* not force_landscape_mode */ ) )
	{
		return 0;
	}

	grace->symbols = 1;

	if ( !grace_output_charts(
				output_filename, 
				postscript_filename,
				agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				page_width_pixels,
				page_length_pixels,
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_ghost_script_directory(
					application_name ),
				(LIST *)0 /* quantum_datatype_name_list */,
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list ) )
	{
		return 0;
	}

	grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				appaserver_mount_point,
				1 /* with_document_output */,
				query_get_display_where_clause(
					where_clause,
					application_name,
					value_folder,
					1 ) );
	return 1;
}

