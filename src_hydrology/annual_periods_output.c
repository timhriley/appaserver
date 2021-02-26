/* ------------------------------------------------------ 	*/
/* $APPASERVER_HOME/src_hydrology/annual_periods_output.c      	*/
/* ------------------------------------------------------ 	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------ 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "station_datatype.h"
#include "grace.h"
#include "column.h"
#include "session.h"
#include "query.h"
#include "boolean.h"
#include "period_wo_date.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
/* #define DEBUG_MODE			1 */
#define AGGREGATE_PERIOD_DEFAULT	"annually"
#define EXCEEDANCE_DELIMITER		'|'
#define DEFAULT_OUTPUT_MEDIUM		"table"
#define GRACE_DATATYPE_ENTITY_PIECE	0
#define GRACE_DATATYPE_PIECE		1
#define GRACE_DATE_PIECE		2
#define GRACE_TIME_PIECE		-1
#define GRACE_VALUE_PIECE		3
#define GRACE_TICKLABEL_ANGLE		90
#define PROCESS_NAME			"output_annual_periods"
#define KEY_DELIMITER			'/'
#define FILENAME_STEM			"annual_periods"

#define ROWS_BETWEEN_HEADING		20
#define SELECT_LIST			"measurement_date,measurement_value,station"

#define DATE_PIECE		 		0
#define VALUE_PIECE		 		1
#define STATION_PIECE		 		2
#define INPUT_DELIMITER				','
#define PIECE_DELIMITER				'|'

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
char *get_date_range_string(	char *period_string,
				char *ending_period_string );

char *get_delta_title(	char *delta_string );

void output_invalid_output_medium(
			char *output_medium );

void period_wo_date_output_transmit(
			PERIOD_WO_DATE_PERIOD **period_array,
			double sum_minimum,
			double sum_average,
			double sum_maximum,
			LIST *year_sum_list,
			boolean is_weekly,
			char *station_name,
			char *datatype_name,
			enum aggregate_statistic aggregate_statistic,
			char *begin_date_string,
			char *end_date_string,
			char *document_root_directory,
			char *application_name,
			char *output_medium,
			char *units_display_string );

void period_wo_date_output_table(
			PERIOD_WO_DATE_PERIOD **period_array,
			double sum_minimum,
			double sum_average,
			double sum_maximum,
			LIST *year_sum_list,
			boolean is_weekly,
			char *station_name,
			char *datatype_name,
			enum aggregate_statistic aggregate_statistic,
			char *begin_date_string,
			char *end_date_string,
			char *delta_string,
			char *units_display );

void period_wo_date_populate_year_value_list(
			PERIOD_WO_DATE_PERIOD **period_array,
			char **units_display,
			LIST *year_sum_list,
			boolean is_weekly,
			char *application_name,
			char *station_name,
			char *datatype_name,
			enum aggregate_statistic,
			char *appaserver_mount_point,
			char *begin_date_string,
			char *end_date_string,
			enum period_wo_date_delta,
			char *units_converted_string );

char *get_where_clause(	char *station_name,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string );

void annual_periods_monthly_missing_month_output_spreadsheet(
				char *application_name,
				char *station_name,
				char *datatype_name,
				enum aggregate_statistic aggregate_statistic,
				char *document_root_directory,
				char *begin_date_string,
				char *end_date_string,
				char *output_medium,
				char *units_converted_string );

void annual_periods_weekly_missing_week_output_spreadsheet(
				char *application_name,
				char *station_name,
				char *datatype_name,
				enum aggregate_statistic aggregate_statistic,
				char *document_root_directory,
				char *begin_date_string,
				char *end_date_string,
				char *output_medium,
				char *units_converted_string );

void annual_periods_weekly_missing_week_output_table(
				char *application_name,
				char *station_name,
				char *datatype_name,
				enum aggregate_statistic aggregate_statistic,
				char *appaserver_mount_point,
				char *begin_date_string,
				char *end_date_string,
				char *units_converted_string );

void annual_periods_monthly_missing_month_output_table(
				char *application_name,
				char *station_name,
				char *datatype_name,
				enum aggregate_statistic aggregate_statistic,
				char *appaserver_mount_point,
				char *begin_date_string,
				char *end_date_string,
				char *delta_string,
				char *units_converted_string );

void piece_exceedance_variables(
					char **value_string,
					char **date_string,
					char **count_below_string,
					char **percent_below_string,
					char **aggregation_count_string,
					char *input_buffer );

void annual_periods_output_table_exceedance_format(
					char *station_name,
					char *datatype_name,
					char *sys_string,
					enum aggregate_statistic,
					char *aggregate_period_string,
					char *begin_date_string,
					char *end_date_string,
					char *period_string,
					char *ending_period_string,
					char *delta_string,
					char *units_display );

int annual_periods_output_chart_exceedance_format(
					char *application_name,
					char *station_name,
					char *datatype_name,
					char *begin_date_string,
					char *end_date_string,
					char *sys_string,
					char *document_root_directory,
					char *aggregate_period_string,
					char *period_string,
					char *ending_period_string,
					enum aggregate_statistic,
					char *delta_string,
					char *units_display );

int annual_periods_output_chart(
					char *application_name,
					char *station,
					char *datatype_name,
					char *begin_date_string,
					char *end_date_string,
					char *sys_string,
					char *document_root_directory,
					char *argv_0,
					char *aggregate_period_string,
					char *period_string,
					char *ending_period_string,
					enum aggregate_statistic,
					char *output_medium,
					char *delta_string,
					char *units_display,
					boolean bar_graph );

void annual_periods_output_table(
					char *station_name,
					char *datatype_name,
					char *sys_string,
					enum aggregate_statistic,
					char *aggregate_period_string,
					char *begin_date_string,
					char *end_date_string,
					char *period_string,
					char *ending_period_string,
					char *delta_string,
					char *units_display );

void annual_periods_output_spreadsheet(
					FILE *output_pipe,
					char *sys_string,
					enum aggregate_statistic,
					char *aggregate_period_string,
					char *period_string,
					char *ending_period_string,
					char *begin_date_string,
					char *end_date_string,
					char *station_name,
					char *datatype_name,
					char *delta_string,
					char *units_display );

void annual_periods_output_transmit_exceedance_format(
				FILE *output_pipe,
				char *sys_string,
				enum aggregate_statistic aggregate_statistic,
				char *aggregate_period_string,
				char *period_string,
				char *ending_period_string,
				char *begin_date_string,
				char *end_date_string,
				char *station_name,
				char *datatype_name,
				char *delta_string,
				char *units_display );

void build_sys_string(
				char *sys_string,
				char **units_display,
				boolean *bar_graph,
				char *application_name,
				char *station_name,
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *appaserver_mount_point,
				enum aggregate_statistic,
				char *aggregate_period_string,
				char *period_string,
				char *ending_period_string,
				char *delta_string,
				char *exceedance_format_yn,
				char *units_converted_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *parameter_dictionary_string;
	char *station_name = {0};
	char *datatype_name = {0};
	char *begin_date_string = {0};
	char *end_date_string = {0};
	char *output_medium;
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *parameter_dictionary;
	char sys_string[ 2048 ];
	char *aggregate_statistic_string = {0};
	enum aggregate_statistic aggregate_statistic;
	char *aggregate_period_string = {0};
	char *period_string = {0};
	char *ending_period_string = {0};
	char *month_string = {0};
	char *season_string = {0};
	char *containing_date_string = {0};
	char *dynamic_ending_date_string = {0};
	char *delta_string = {0};
	char *exceedance_format_yn = {0};
	char *units_converted_string = {0};
	char *units_display = {0};
	boolean bar_graph = 0;
	char *database_string = {0};

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s ignored ignored ignored application parameter_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	/* login_name = argv[ 1 ]; */
	/* role_name = argv[ 2 ]; */
	/* session = argv[ 3 ]; */
	application_name = argv[ 4 ];
	parameter_dictionary_string = argv[ 5 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
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

	parameter_dictionary = 
		dictionary_index_string2dictionary(
			parameter_dictionary_string );

	dictionary_add_elements_by_removing_prefix(
			    		parameter_dictionary,
			    		QUERY_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
			    		parameter_dictionary,
			    		QUERY_FROM_STARTING_LABEL );

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

	if ( strcmp( output_medium, "text_file") == 0 )
		output_medium = "transmit";

	dictionary_get_index_data( 	&station_name,
					parameter_dictionary,
					"station",
					0 );

	if (	!station_name
	|| 	!*station_name
	||	timlib_strcmp( station_name, "station" ) == 0 )
	{
		if ( timlib_strcmp( output_medium, "stdout" ) != 0 )
		{
			document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
		}

		printf( "<p>ERROR: Please choose a station/datatype.\n" );

		if ( timlib_strcmp( output_medium, "stdout" ) != 0 )
			document_close();

		exit( 0 );
	}

	dictionary_get_index_data( 	&datatype_name,
					parameter_dictionary,
					"datatype",
					0 );

	dictionary_get_index_data( 	&aggregate_statistic_string,
					parameter_dictionary,
					"aggregate_statistic",
					0 );

	dictionary_get_index_data(	&aggregate_period_string,
					parameter_dictionary,
					"aggregate_period",
					0 );

	if (	!aggregate_period_string
	|| 	!*aggregate_period_string
	||	timlib_strcmp(	aggregate_period_string,
				"aggregate_period" ) == 0 )
	{
		aggregate_period_string = AGGREGATE_PERIOD_DEFAULT;
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

	if ( !containing_date_string )
	{
		dictionary_get_index_data(
					&containing_date_string,
					parameter_dictionary,
					"week_containing_date",
					0 );
	}

	dictionary_get_index_data( 	&dynamic_ending_date_string,
					parameter_dictionary,
					"dynamic_ending_date",
					0 );

	dictionary_get_index_data( 	&delta_string,
					parameter_dictionary,
					"delta",
					0 );

	dictionary_get_index_data( 	&exceedance_format_yn,
					parameter_dictionary,
					"exceedance_format_yn",
					0 );

	dictionary_get_index_data( 	&units_converted_string,
					parameter_dictionary,
					"units_converted",
					0 );

	{
		int position;

		if ( ( position = character_position(
				units_converted_string,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) ) != -1 )
		{
			strcpy(	units_converted_string,
				units_converted_string + ( position + 1 ) );
		}
	}

	dictionary_get_index_data(	&begin_date_string,
					parameter_dictionary,
					"begin_date",
					0 );

	dictionary_get_index_data(	&end_date_string,
					parameter_dictionary,
					"end_date",
					0 );

	hydrology_library_get_clean_begin_end_date(
					&begin_date_string,
					&end_date_string,
					application_name,
					station_name,
					datatype_name );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		if ( strcmp( output_medium, "stdout" ) != 0 )
		{
			document_quick_output_body(
						application_name,
						appaserver_parameter_file->
						appaserver_mount_point );
		}

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			aggregate_statistic_string,
			daily );

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype_name,
				aggregate_statistic_none );
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
	if ( strcmp( aggregate_period_string, "dynamically" ) == 0 )
	{
		period_string = containing_date_string;
		ending_period_string = dynamic_ending_date_string;
	}
	else
		period_string = (char *)0;

	if (	!period_string
	||	!*period_string
	||	strcasecmp( period_string, "period" ) == 0 )
	{
		if ( strcmp( aggregate_period_string, "weekly" ) == 0 )
		{
			if ( strcmp( output_medium, "stdout" ) != 0 )
			{
				document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
			}

			if ( strcmp( output_medium, "table" ) == 0 )
			{
				hydrology_library_output_station_table(
						application_name,
						station_name );

				annual_periods_weekly_missing_week_output_table(
					application_name,
					station_name,
					datatype_name,
					aggregate_statistic,
					appaserver_parameter_file->
						appaserver_mount_point,
					begin_date_string,
					end_date_string,
					units_converted_string );
			}
			else
			if ( strcmp( output_medium, "transmit" ) == 0
			||   strcmp( output_medium, "spreadsheet" ) == 0
			||   strcmp( output_medium, "stdout" ) == 0 )
			{
			  annual_periods_weekly_missing_week_output_spreadsheet(
					application_name,
					station_name,
					datatype_name,
					aggregate_statistic,
					appaserver_parameter_file->
						document_root,
					begin_date_string,
					end_date_string,
					output_medium,
					units_converted_string );

			}
			else
			{
				output_invalid_output_medium( output_medium );
			}

			if ( strcmp( output_medium, "stdout" ) != 0 )
			{
				document_close();
			}

			exit( 0 );
		}
		else
		if ( strcmp( aggregate_period_string, "monthly" ) == 0 )
		{
			if ( strcmp( output_medium, "stdout" ) != 0 )
			{
				document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
			}

			if ( strcmp( output_medium, "table" ) == 0 )
			{
				hydrology_library_output_station_table(
						application_name,
						station_name );

			      annual_periods_monthly_missing_month_output_table(
					application_name,
					station_name,
					datatype_name,
					aggregate_statistic,
					appaserver_parameter_file->
						appaserver_mount_point,
					begin_date_string,
					end_date_string,
					delta_string,
					units_converted_string );
			}
			else
			if ( strcmp( output_medium, "transmit" ) == 0
			||   strcmp( output_medium, "spreadsheet" ) == 0
			||   strcmp( output_medium, "stdout" ) == 0 )
			{
			annual_periods_monthly_missing_month_output_spreadsheet(
					application_name,
					station_name,
					datatype_name,
					aggregate_statistic,
					appaserver_parameter_file->
						document_root,
					begin_date_string,
					end_date_string,
					output_medium,
					units_converted_string );
			}
			else
			{
				output_invalid_output_medium( output_medium );
			}

			if ( strcmp( output_medium, "stdout" ) != 0 )
			{
				document_close();
			}
			exit( 0 );
		}

		if ( strcmp( output_medium, "stdout" ) != 0
		&&   strcmp( output_medium, "chart" ) != 0 )
		{
			document_quick_output_body(
						application_name,
						appaserver_parameter_file->
						appaserver_mount_point );
		}

		printf(
	"<h3>ERROR: please select a corresponding aggregate period.</h3>\n" );
		document_close();
		exit( 1 );
	}

	build_sys_string(	sys_string,
				&units_display,
				&bar_graph,
				application_name,
				station_name,
				datatype_name,
				begin_date_string,
				end_date_string,
				appaserver_parameter_file->
					appaserver_mount_point,
				aggregate_statistic,
				aggregate_period_string,
				period_string,
				ending_period_string,
				delta_string,
				exceedance_format_yn,
				units_converted_string );

#ifdef DEBUG_MODE
fprintf( stderr, "%s/%s()/%d: got sys_string = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
#endif

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
		int results;

		if ( *exceedance_format_yn == 'y' )
		{
			results = annual_periods_output_chart_exceedance_format(
					application_name,
					station_name,
					datatype_name,
					begin_date_string,
					end_date_string,
					sys_string,
					appaserver_parameter_file->
						document_root,
					aggregate_period_string,
					period_string,
					ending_period_string,
					aggregate_statistic,
					delta_string,
					units_display );
		}
		else
		{
			results = annual_periods_output_chart(
					application_name,
					station_name,
					datatype_name,
					begin_date_string,
					end_date_string,
					sys_string,
					appaserver_parameter_file->
						document_root,
					argv[ 0 ],
					aggregate_period_string,
					period_string,
					ending_period_string,
					aggregate_statistic,
					output_medium,
					delta_string,
					units_display,
					bar_graph );
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
		hydrology_library_output_station_table(
				application_name,
				station_name );

		if ( *exceedance_format_yn == 'y' )
		{
			annual_periods_output_table_exceedance_format(
					station_name,
					datatype_name,
					sys_string,
					aggregate_statistic,
					aggregate_period_string,
					begin_date_string,
					end_date_string,
					period_string,
					ending_period_string,
					delta_string,
					units_display );
		}
		else
		{
			annual_periods_output_table(
					station_name,
					datatype_name,
					sys_string,
					aggregate_statistic,
					aggregate_period_string,
					begin_date_string,
					end_date_string,
					period_string,
					ending_period_string,
					delta_string,
					units_display );
		}
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		char *output_filename;
		char *ftp_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		FILE *output_file;
		char output_sys_string[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
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
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		hydrology_library_output_station_text_filename(
				output_filename,
				application_name,
				station_name,
				1 /* with_zap_file */ );

/*
		if ( *exceedance_format_yn == 'y' )
		{
			sprintf( output_sys_string,
			 	"delimiter2padded_columns.e '|' 99 >> %s",
			 	output_filename );
		}
		else
		{
			sprintf( output_sys_string,
			 	"delimiter2padded_columns.e '|' 2 >> %s",
			 	output_filename );
		}
*/
		sprintf(output_sys_string,
		 	"tr '|' '%c' >> %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_filename );

		output_pipe = popen( output_sys_string, "w" );

		if ( *exceedance_format_yn == 'y' )
		{
			annual_periods_output_transmit_exceedance_format(
					output_pipe,
					sys_string,
					aggregate_statistic,
					aggregate_period_string,
					period_string,
					ending_period_string,
					begin_date_string,
					end_date_string,
					station_name,
					datatype_name,
					delta_string,
					units_display );
		}
		else
		{
			annual_periods_output_spreadsheet(
						output_pipe,
						sys_string,
						aggregate_statistic,
						aggregate_period_string,
						period_string,
						ending_period_string,
						begin_date_string,
						end_date_string,
						station_name,
						datatype_name,
						delta_string,
						units_display );
		}

		pclose( output_pipe );

		printf( "<h1>Annual Periods Spreadsheet<br></h1>\n" );
		printf( "<h2>\n" );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
		fflush( stdout );
		printf( "</h2>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "transmit" ) == 0 )
	{
		char *output_filename;
		char *ftp_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		FILE *output_file;
		char output_sys_string[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
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

		hydrology_library_output_station_text_filename(
				output_filename,
				application_name,
				station_name,
				1 /* with_zap_file */ );

/*
		if ( *exceedance_format_yn == 'y' )
		{
			sprintf( output_sys_string,
			 	"delimiter2padded_columns.e ',' 99 >> %s",
			 	output_filename );
		}
		else
		{
			sprintf( output_sys_string,
			 	"delimiter2padded_columns.e ',' 2 >> %s",
			 	output_filename );
		}
*/
		sprintf(output_sys_string,
		 	"tr ',' '%c' >> %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_filename );

		output_pipe = popen( output_sys_string, "w" );

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


		if ( *exceedance_format_yn == 'y' )
		{
			annual_periods_output_transmit_exceedance_format(
					output_pipe,
					sys_string,
					aggregate_statistic,
					aggregate_period_string,
					period_string,
					ending_period_string,
					begin_date_string,
					end_date_string,
					station_name,
					datatype_name,
					delta_string,
					units_display );
		}
		else
		{
			annual_periods_output_spreadsheet(
						output_pipe,
						sys_string,
						aggregate_statistic,
						aggregate_period_string,
						period_string,
						ending_period_string,
						begin_date_string,
						end_date_string,
						station_name,
						datatype_name,
						delta_string,
						units_display );
		}

		pclose( output_pipe );

		printf( "<h1>Annual Periods Text File<br></h1>\n" );
		printf( "<h2>\n" );
		fflush( stdout );
		if ( system( "date '+%x %H:%M'" ) ){};
		fflush( stdout );
		printf( "</h2>\n" );
	
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

		hydrology_library_output_station_text_file(
				stdout,
				application_name,
				station_name );

/*
		if ( *exceedance_format_yn == 'y' )
		{
			sprintf( output_sys_string,
			 	"delimiter2padded_columns.e ',' 99" );
		}
		else
		{
			sprintf( output_sys_string,
			 	"delimiter2padded_columns.e ',' 2" );
		}
*/
		sprintf(output_sys_string,
		 	"tr ',' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( output_sys_string, "w" );

		if ( *exceedance_format_yn == 'y' )
		{
			annual_periods_output_transmit_exceedance_format(
					output_pipe,
					sys_string,
					aggregate_statistic,
					aggregate_period_string,
					period_string,
					ending_period_string,
					begin_date_string,
					end_date_string,
					station_name,
					datatype_name,
					delta_string,
					units_display );
		}
		else
		{
			annual_periods_output_spreadsheet(
						output_pipe,
						sys_string,
						aggregate_statistic,
						aggregate_period_string,
						period_string,
						ending_period_string,
						begin_date_string,
						end_date_string,
						station_name,
						datatype_name,
						delta_string,
						units_display );
		}

		pclose( output_pipe );
	}
	else
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid output medium = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 output_medium );
		exit( 1 );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
		application_name,
		PROCESS_NAME,
		appaserver_parameter_file_get_dbms() );

	return 0;
}

void annual_periods_output_spreadsheet(
				FILE *output_pipe,
				char *sys_string,
				enum aggregate_statistic aggregate_statistic,
				char *aggregate_period_string,
				char *period_string,
				char *ending_period_string,
				char *begin_date_string,
				char *end_date_string,
				char *station_name,
				char *datatype_name,
				char *delta_string,
				char *units_display )
{
	char buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	char count_string[ 128 ];
	FILE *input_pipe;
	char *delta_title;
	char title[ 256 ];
	char value_heading_string[ 128 ];

	delta_title = get_delta_title( delta_string );

	sprintf(	title,
		"Annual %s%s %s for %s from: %s to: %s for %s/%s",
			( strcmp( aggregate_period_string, "annually" ) == 0 )
				? ""
				: aggregate_period_string,
			delta_title,
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
			get_date_range_string(
				period_string,
				ending_period_string ),
			begin_date_string,
			end_date_string,
			station_name,
			datatype_name );

	fprintf(	output_pipe,
			"#%s\n",
			format_initial_capital( title, title ) );

	sprintf( value_heading_string,
		 "%s(%s)",
		 aggregate_statistic_get_string(
			aggregate_statistic ),
		 units_display );

	format_initial_capital( value_heading_string, value_heading_string );

	fprintf(	output_pipe,
			"#Date,%s,Count\n",
			value_heading_string );

	input_pipe = popen( sys_string, "r" );
	while( get_line( buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, buffer, 0 );
		piece( value_string, INPUT_DELIMITER, buffer, 1 );
		piece( count_string, INPUT_DELIMITER, buffer, 2 );

		fprintf( output_pipe,
			 "%s,%s,%s\n",
			 date_string,
			 value_string,
			 count_string );
	}

	pclose( input_pipe );

} /* annual_periods_output_spreadsheet() */

void annual_periods_output_table(
				char *station_name,
				char *datatype_name,
				char *sys_string,
				enum aggregate_statistic aggregate_statistic,
				char *aggregate_period_string,
				char *begin_date_string,
				char *end_date_string,
				char *period_string,
				char *ending_period_string,
				char *delta_string,
				char *units_display )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char title[ 512 ];
	char title_buffer[ 512 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	char count_string[ 128 ];
	char value_heading_string[ 128 ];
	int count = 0;
	char *delta_title;

	delta_title = get_delta_title( delta_string );

	sprintf(	title,
		"Annual %s%s %s for %s<br>from: %s to: %s<br>for %s/%s",
			( strcmp( aggregate_period_string, "annually" ) == 0 )
				? ""
				: aggregate_period_string,
			delta_title,
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
			get_date_range_string(
				period_string,
				ending_period_string ),
			begin_date_string,
			end_date_string,
			station_name,
			datatype_name );

	html_table = new_html_table(	format_initial_capital(
						title_buffer,
						title ),
			(char *)0 /* sub_title */ );
	heading_list = new_list();

	list_append_pointer( heading_list, "Date" );

	sprintf( value_heading_string,
		 "%s(%s)",
		 aggregate_statistic_get_string(
			aggregate_statistic ),
		 units_display );

	format_initial_capital( value_heading_string, value_heading_string );
	list_append_pointer(	heading_list,
				strdup( value_heading_string ) );

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

/*
		if ( count % 2 )
			html_table_set_background_shaded( html_table );
		else
			html_table_set_background_unshaded( html_table );
*/

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
		char **units_display,
		boolean *bar_graph,
		char *application_name,
		char *station_name,
		char *datatype_name,
		char *begin_date_string,
		char *end_date_string,
		char *appaserver_mount_point,
		enum aggregate_statistic aggregate_statistic,
		char *aggregate_period_string,
		char *period_string,
		char *ending_period_string,
		char *delta_string,
		char *exceedance_format_yn,
		char *units_converted_string )
{
	char *where_clause;
	char aggregation_process[ 1024 ];
	char delta_process[ 1024 ];
	char exceedance_process[ 1024 ];
	char *units;
	char units_converted_process[ 256 ];

	units = hydrology_library_get_units_string(
					bar_graph,
					application_name,
					datatype_name );

	*units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype_name,
			units,
			units_converted_string,
			aggregate_statistic );

	if ( !*units_display )
	{
		document_quick_output_body(	application_name,
						appaserver_mount_point );
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype_name,
				units,
				units_converted_string ) );
		document_close();
		exit( 0 );
	}

	if ( units_converted_string
	&&   *units_converted_string
	&&   strcmp( units_converted_string, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s %d ',' %d",
			 application_name,
			 units,
			 units_converted_string,
			 VALUE_PIECE,
			 STATION_PIECE );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype_name,
				aggregate_statistic_none );
	}

	if ( exceedance_format_yn && *exceedance_format_yn == 'y' )
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

	if ( delta_string && *delta_string )
	{
		char *first_previous;

		if ( strcmp( delta_string, "first" ) == 0 )
			first_previous = "first";
		else
		if ( strcmp( delta_string, "previous" ) == 0 )
			first_previous = "previous";
		else
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid delta_string = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				delta_string );
			exit( 1 );
		}

		sprintf(delta_process,
			"delta_values.e %d %s '%c'",
	 		VALUE_PIECE,
			first_previous,
			INPUT_DELIMITER );
	}
	else
	{
		strcpy( delta_process, "cat" );
	}

	where_clause = get_where_clause(
			station_name,
			datatype_name,
			begin_date_string,
			end_date_string );

/*
	sprintf( 	aggregation_process, 
	"real_time2aggregate_value.e %s %d -1 %d ',' daily n %s |"
	"real_time2annual_period.e %s %s %d %d '%c' %s %s '%s'   ",
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
	 		DATE_PIECE,
	 		VALUE_PIECE,
			(ending_period_string) ? ending_period_string : "",
	 		application_name,
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
	 		DATE_PIECE,
	 		VALUE_PIECE,
			INPUT_DELIMITER,
			aggregate_period_string,
			period_string,
			(ending_period_string) ? ending_period_string : "" );
*/

	sprintf( 	aggregation_process, 
	"real_time2annual_period.e %s %s %d %d '%c' %s %s '%s'   ",
	 		application_name,
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
	 		DATE_PIECE,
	 		VALUE_PIECE,
			INPUT_DELIMITER,
			aggregate_period_string,
			period_string,
			(ending_period_string) ? ending_period_string : "" );
/*
	sys_string += sprintf( sys_string,
	"get_folder_data	application=%s				    "
	"			folder=measurement			    "
	"			select=\"%s\"				    "
	"			where=\"%s\"				    "
	"			order=select				    "
	"			quick=y					   |"
	"tr '%c' '%c' 							   |"
	"pad_missing_times.e ',' 0,-1,1 real_time %s 0000 %s 2359 0 '%s'   |"
	"%s								   |"
	"%s								   |"
	"%s								   |"
	"%s								   |"
	"cat								    ",
		application_name,
		SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		begin_date_string,
		end_date_string,
		hydrology_library_get_expected_count_list_string(
			application_name,
			station_name,
			datatype_name,
			'|' ),
		aggregation_process,
		units_converted_process,
		delta_process,
		exceedance_process );
*/

	sys_string += sprintf( sys_string,
	"get_folder_data	application=%s				    "
	"			folder=measurement			    "
	"			select=\"%s\"				    "
	"			where=\"%s\"				    "
	"			order=select				    "
	"			quick=y					   |"
	"tr '%c' '%c' 							   |"
	"%s								   |"
	"%s								   |"
	"%s								   |"
	"%s								   |"
	"cat								    ",
		application_name,
		SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		aggregation_process,
		units_converted_process,
		delta_process,
		exceedance_process );

} /* build_sys_string() */

int annual_periods_output_chart(
				char *application_name,
				char *station_name,
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string,
				char *document_root_directory,
				char *argv_0,
				char *aggregate_period_string,
				char *period_string,
				char *ending_period_string,
				enum aggregate_statistic aggregate_statistic,
				char *output_medium,
				char *delta_string,
				char *units_display,
				boolean bar_graph )
{
	char input_buffer[ 512 ];
	char station_datatype_input_buffer[ 512 ];
	char title[ 512 ];
	char sub_title[ 512 ];
	char buffer1[ 512 ];
	char buffer2[ 512 ];
	GRACE *grace;
	char graph_identifier[ 128 ];
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
	STATION_DATATYPE *station_datatype;
	char delta_title[ 128 ];

	strcpy( delta_title, get_delta_title( delta_string ) );

	sprintf( title,
		 "%s%s Annual Periods for %s",
		 format_initial_capital(
			 buffer1,
			( strcmp( aggregate_period_string, "annually" ) == 0 )
				? ""
				: aggregate_period_string ),
		 format_initial_capital( delta_title, delta_title ),
		 format_initial_capital(
			 buffer2,
			 get_date_range_string( period_string,
				 		ending_period_string ) ) );

	sprintf(sub_title,
		"%s/%s %s from %s to %s",
			station_name,
			format_initial_capital( buffer1, datatype_name ),
			format_initial_capital( buffer2,
				aggregate_statistic_get_string(
					aggregate_statistic ) ),
			begin_date_string,
			end_date_string );

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
				strdup( title ),
				strdup( sub_title ),
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	if ( !grace_set_begin_end_date(grace,
					begin_date_string,
					end_date_string ) )
	{
		if ( strcmp( output_medium, "stdout" ) != 0
		&&   strcmp( output_medium, "chart" ) != 0 )
		{
			document_quick_output_body(
				application_name,
				(char *)0 /* appaserver_mount_point */ );
		}

		printf(
		"<h3>ERROR: Invalid date format format.</h3>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

/*
	grace_graph->units = 
		hydrology_library_get_units_string(
					&bar_graph,
					application_name,
					datatype_name );
*/
	grace_graph->units = units_display;

	grace_datatype = grace_new_grace_datatype(
					strdup( station_name ),
					strdup( datatype_name ) );

	station_datatype =
		station_datatype_get_station_datatype(
			application_name,
			(char *)0 /* station */,
			datatype_name );

	grace_graph_set_scale_to_zero(
			grace_graph,
			(station_datatype->scale_graph_zero_yn == 'y') );

	grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;

	strcpy( legend, datatype_name );

	strcpy(	legend,
		format_initial_capital( buffer1, legend ) );

	grace_datatype->legend = strdup( legend );

	if ( bar_graph )
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
		grace_datatype->line_linestyle = 0;
	}
	else
	{
		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	}

/*
	grace_datatype->datatype_type_bar_xy_xyhilo =
				(bar_graph) ? "bar" : "xy";
*/

	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	list_append_pointer( grace->graph_list, grace_graph );

	grace->grace_output =
		application_grace_output( application_name );

	sprintf(	graph_identifier,
			"%s_%s_%d",
			station_name,
			datatype_name,
			getpid() );

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
			 "%s|%s|%s",
			 station_name,
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
				1 /* dataset_no_cycle_color */,
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
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

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
				(char *)0 /* appaserver_mount_point */,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
	}
	return 1;
} /* annual_periods_output_chart() */

void annual_periods_output_transmit_exceedance_format(
				FILE *output_pipe,
				char *sys_string,
				enum aggregate_statistic aggregate_statistic,
				char *aggregate_period_string,
				char *period_string,
				char *ending_period_string,
				char *begin_date_string,
				char *end_date_string,
				char *station_name,
				char *datatype_name,
				char *delta_string,
				char *units_display )
{
	char buffer[ 512 ];
	char title[ 256 ];
	FILE *input_pipe;
	char *delta_title;
	char value_heading_string[ 128 ];

	delta_title = get_delta_title( delta_string );

	sprintf( value_heading_string,
		 "%s(%s)",
		 aggregate_statistic_get_string(
			aggregate_statistic ),
		 units_display );

	format_initial_capital( value_heading_string, value_heading_string );

	sprintf(	title,
	"Annual %s%s %s Exceedance Format for %s from: %s to: %s for %s/%s",
			aggregate_period_string,
			delta_title,
			value_heading_string,
			get_date_range_string(
				period_string,
				ending_period_string ),
			begin_date_string,
			end_date_string,
			station_name,
			datatype_name );

	fprintf(	output_pipe,
			"%s\n",
			format_initial_capital( title, title ) );

	fprintf(	output_pipe,
			"Value,Count,Date,CountBelow,%cBelow\n",
			'%' );

	input_pipe = popen( sys_string, "r" );
	while( get_line( buffer, input_pipe ) )
	{
		search_replace_character( buffer, '|', ',' );
		fprintf( output_pipe, "%s\n", buffer );
	}

	pclose( input_pipe );

} /* annual_periods_output_transmit_exceedance_format() */

void annual_periods_output_table_exceedance_format(
				char *station_name,
				char *datatype_name,
				char *sys_string,
				enum aggregate_statistic aggregate_statistic,
				char *aggregate_period_string,
				char *begin_date_string,
				char *end_date_string,
				char *period_string,
				char *ending_period_string,
				char *delta_string,
				char *units_display )
{
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char title[ 512 ];
	char title_buffer[ 512 ];
	int count = 0;
	char *value_string;
	char *date_string;
	char *count_below_string;
	char *percent_below_string;
	char *aggregation_count_string;
	char *delta_title;
	char value_heading_string[ 128 ];

	delta_title = get_delta_title( delta_string );

	sprintf(	title,
"Annual %s%s %s for %s <br>from: %s to: %s<br>for %s/%s Exceedance Format",
			aggregate_period_string,
			delta_title,
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
			get_date_range_string(
				period_string,
				ending_period_string ),
			begin_date_string,
			end_date_string,
			station_name,
			datatype_name );

	html_table = new_html_table(	format_initial_capital(
						title_buffer,
						title ),
			(char *)0 /* sub_title */ );
	heading_list = new_list();

/*
	list_append_pointer(	heading_list,
				strdup( format_initial_capital(
					buffer,
					aggregate_statistic_get_string(
						aggregate_statistic ) ) ) );
*/
	sprintf( value_heading_string,
		 "%s(%s)",
		 aggregate_statistic_get_string(
			aggregate_statistic ),
		 units_display );

	format_initial_capital( value_heading_string, value_heading_string );
	list_append_pointer(	heading_list,
				strdup( value_heading_string ) );


	list_append_pointer( heading_list, "Count Below" );
	list_append_pointer( heading_list, "Percent Below" );
	list_append_pointer( heading_list, "Representative<br>Date" );
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
				char *station_name,
				char *datatype_name,
				char *begin_date_string,
				char *end_date_string,
				char *sys_string,
				char *document_root_directory,
				char *aggregate_period_string,
				char *period_string,
				char *ending_period_string,
				enum aggregate_statistic aggregate_statistic,
				char *delta_string,
				char *units_display )
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
	char delta_title[ 128 ];

	strcpy( delta_title, get_delta_title( delta_string ) );

	sprintf( title,
		 "%s%s Annual Periods for %s",
		 format_initial_capital( buffer1, aggregate_period_string ),
		 format_initial_capital( delta_title, delta_title ),
		 format_initial_capital( buffer2,
			 get_date_range_string(
				 period_string,
				 ending_period_string ) ) );

	sprintf(sub_title,
		"%s/%s %s from %s to %s Exceedance Format",
			station_name,
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
				units_display,
				/*
				hydrology_library_get_units_string(
					&bar_chart,
					application_name,
					datatype_name ),
				*/
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
				(char *)0 /* appaserver_mount_point */,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
	return 1;
} /* annual_periods_output_chart_exceedance_format() */

void annual_periods_monthly_missing_month_output_table(
			char *application_name,
			char *station_name,
			char *datatype_name,
			enum aggregate_statistic aggregate_statistic,
			char *appaserver_mount_point,
			char *begin_date_string,
			char *end_date_string,
			char *delta_string,
			char *units_converted_string )
{
	PERIOD_WO_DATE *period_wo_date;
	char *units_display = {0};

	period_wo_date =
		period_wo_date_new(
			0 /* not is_weekly */,
			aggregate_statistic,
			delta_string,
			period_wo_date_get_year_int( begin_date_string ),
			period_wo_date_get_year_int( end_date_string ) );

	period_wo_date->period_array =
		period_wo_date_get_monthly_period_array(
			begin_date_string,
			end_date_string );

	period_wo_date_populate_year_value_list(
		period_wo_date->period_array,
		&units_display,
		period_wo_date->year_sum_list,
		period_wo_date->is_weekly,
		application_name,
		station_name,
		datatype_name,
		aggregate_statistic,
		appaserver_mount_point,
		begin_date_string,
		end_date_string,
		period_wo_date->period_wo_date_delta,
		units_converted_string );

	period_wo_date_populate_minimum_average_maximum(
		&period_wo_date->sum_minimum,
		&period_wo_date->sum_average,
		&period_wo_date->sum_maximum,
		period_wo_date->period_array );

	period_wo_date_populate_year_sum_list_null_count(
		period_wo_date->year_sum_list,
		period_wo_date->period_array );

	period_wo_date_output_table(
		period_wo_date->period_array,
		period_wo_date->sum_minimum,
		period_wo_date->sum_average,
		period_wo_date->sum_maximum,
		period_wo_date->year_sum_list,
		period_wo_date->is_weekly,
		station_name,
		datatype_name,
		aggregate_statistic,
		begin_date_string,
		end_date_string,
		delta_string,
		units_display );

} /* annual_periods_monthly_missing_month_output_table() */

void annual_periods_weekly_missing_week_output_table(
			char *application_name,
			char *station_name,
			char *datatype_name,
			enum aggregate_statistic aggregate_statistic,
			char *appaserver_mount_point,
			char *begin_date_string,
			char *end_date_string,
			char *units_converted_string )
{
	PERIOD_WO_DATE *period_wo_date;
	char *units_display = {0};

	period_wo_date = period_wo_date_new(
			1 /* is_weekly */,
			aggregate_statistic,
			(char *)0 /* delta_string */,
			period_wo_date_get_year_int( begin_date_string ),
			period_wo_date_get_year_int( end_date_string ) );

	period_wo_date->period_array =
		period_wo_date_get_weekly_period_array(
			begin_date_string,
			end_date_string );

	period_wo_date_populate_year_value_list(
		period_wo_date->period_array,
		&units_display,
		period_wo_date->year_sum_list,
		period_wo_date->is_weekly,
		application_name,
		station_name,
		datatype_name,
		aggregate_statistic,
		appaserver_mount_point,
		begin_date_string,
		end_date_string,
		period_wo_date->period_wo_date_delta,
		units_converted_string );

	period_wo_date_populate_minimum_average_maximum(
		&period_wo_date->sum_minimum,
		&period_wo_date->sum_average,
		&period_wo_date->sum_maximum,
		period_wo_date->period_array );

	period_wo_date_populate_year_sum_list_null_count(
		period_wo_date->year_sum_list,
		period_wo_date->period_array );

	period_wo_date_output_table(
		period_wo_date->period_array,
		period_wo_date->sum_minimum,
		period_wo_date->sum_average,
		period_wo_date->sum_maximum,
		period_wo_date->year_sum_list,
		period_wo_date->is_weekly,
		station_name,
		datatype_name,
		aggregate_statistic,
		begin_date_string,
		end_date_string,
		(char *)0 /* delta_string */,
		units_display );

} /* annual_periods_weekly_missing_week_output_table() */

void annual_periods_monthly_missing_month_output_spreadsheet(
			char *application_name,
			char *station_name,
			char *datatype_name,
			enum aggregate_statistic aggregate_statistic,
			char *document_root_directory,
			char *begin_date_string,
			char *end_date_string,
			char *output_medium,
			char *units_converted_string )
{
	PERIOD_WO_DATE *period_wo_date;
	char *units_display = {0};

	period_wo_date = period_wo_date_new(
			0 /* not is_weekly */,
			aggregate_statistic,
			(char *)0 /* delta_string */,
			period_wo_date_get_year_int( begin_date_string ),
			period_wo_date_get_year_int( end_date_string ) );

	period_wo_date->period_array =
		period_wo_date_get_monthly_period_array(
			begin_date_string,
			end_date_string );

	period_wo_date_populate_year_value_list(
		period_wo_date->period_array,
		&units_display,
		period_wo_date->year_sum_list,
		period_wo_date->is_weekly,
		application_name,
		station_name,
		datatype_name,
		aggregate_statistic,
		document_root_directory,
		begin_date_string,
		end_date_string,
		period_wo_date->period_wo_date_delta,
		units_converted_string );

	period_wo_date_populate_minimum_average_maximum(
		&period_wo_date->sum_minimum,
		&period_wo_date->sum_average,
		&period_wo_date->sum_maximum,
		period_wo_date->period_array );

	period_wo_date_populate_year_sum_list_null_count(
		period_wo_date->year_sum_list,
		period_wo_date->period_array );

	period_wo_date_output_transmit(
		period_wo_date->period_array,
		period_wo_date->sum_minimum,
		period_wo_date->sum_average,
		period_wo_date->sum_maximum,
		period_wo_date->year_sum_list,
		period_wo_date->is_weekly,
		station_name,
		datatype_name,
		aggregate_statistic,
		begin_date_string,
		end_date_string,
		document_root_directory,
		application_name,
		output_medium,
		units_display );

} /* annual_periods_monthly_missing_month_output_spreadsheet() */

void annual_periods_weekly_missing_week_output_spreadsheet(
			char *application_name,
			char *station_name,
			char *datatype_name,
			enum aggregate_statistic aggregate_statistic,
			char *document_root_directory,
			char *begin_date_string,
			char *end_date_string,
			char *output_medium,
			char *units_converted_string )
{
	PERIOD_WO_DATE *period_wo_date;
	char *units_display = {0};

	period_wo_date = period_wo_date_new(
			1 /* is_weekly */,
			aggregate_statistic,
			(char *)0 /* delta_string */,
			period_wo_date_get_year_int( begin_date_string ),
			period_wo_date_get_year_int( end_date_string ) );

	period_wo_date->period_array =
		period_wo_date_get_weekly_period_array(
			begin_date_string,
			end_date_string );

	period_wo_date_populate_year_value_list(
		period_wo_date->period_array,
		&units_display,
		period_wo_date->year_sum_list,
		period_wo_date->is_weekly,
		application_name,
		station_name,
		datatype_name,
		aggregate_statistic,
		document_root_directory,
		begin_date_string,
		end_date_string,
		period_wo_date->period_wo_date_delta,
		units_converted_string );

	period_wo_date_populate_minimum_average_maximum(
		&period_wo_date->sum_minimum,
		&period_wo_date->sum_average,
		&period_wo_date->sum_maximum,
		period_wo_date->period_array );

	period_wo_date_populate_year_sum_list_null_count(
		period_wo_date->year_sum_list,
		period_wo_date->period_array );

	period_wo_date_output_transmit(
		period_wo_date->period_array,
		period_wo_date->sum_minimum,
		period_wo_date->sum_average,
		period_wo_date->sum_maximum,
		period_wo_date->year_sum_list,
		period_wo_date->is_weekly,
		station_name,
		datatype_name,
		aggregate_statistic,
		begin_date_string,
		end_date_string,
		document_root_directory,
		application_name,
		output_medium,
		units_display );

} /* annual_periods_weekly_missing_week_output_spreadsheet() */

char *get_where_clause(	char *station_name,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string )
{
	char where_clause[ 1024 ];

	sprintf( where_clause,
 	"station = '%s' and 				      "
 	"datatype = '%s' and				      "
	"measurement_date between '%s' and '%s' 	      ",
		station_name,
		datatype_name,
		begin_date_string,
		end_date_string );
	return strdup( where_clause );
} /* get_where_clause() */

int period_wo_date_get_week_int( char *date_string )
{
	static JULIAN *julian = {0};
	int week_int;

	if ( !julian )
	{
		julian = julian_yyyy_mm_dd_new( date_string );
	}
	else
	{
		julian_set_yyyy_mm_dd(
				julian,
				date_string );
	}
	week_int = julian_get_week_number( julian->current );

	/* Comingle the last partial week into the last full week. */
	/* ------------------------------------------------------- */
	if ( week_int == 53 ) week_int = 52;

	return week_int;
} /* period_wo_date_get_week_int() */

int period_wo_date_get_month_int( char *date_string )
{
	static JULIAN *julian = {0};
	int month_int;

	if ( !julian )
	{
		julian = julian_yyyy_mm_dd_new( date_string );
	}
	else
	{
		julian_set_yyyy_mm_dd(
				julian,
				date_string );
	}

	month_int = julian_get_month_number( julian->current );

	return month_int;
} /* period_wo_date_get_month_int() */

int period_wo_date_get_year_int( char *date_string )
{
	return atoi( date_string );
}

void period_wo_date_populate_year_value_list(
		PERIOD_WO_DATE_PERIOD **period_array,
		char **units_display,
		LIST *year_sum_list,
		boolean is_weekly,
		char *application_name,
		char *station_name,
		char *datatype_name,
		enum aggregate_statistic aggregate_statistic,
		char *appaserver_mount_point,
		char *begin_date_string,
		char *end_date_string,
		enum period_wo_date_delta period_wo_date_delta,
		char *units_converted_string )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char *where_clause;
	char aggregation_process[ 1024 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	char count_string[ 128 ];
	int year_int;
	int array_offset;
	PERIOD_WO_DATE_PERIOD *period_wo_date_period;
	PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value;
	PERIOD_WO_DATE_YEAR_SUM *period_wo_date_year_sum;
	char *aggregate_level_string;
	char delta_process[ 128 ];
	char units_converted_process[ 256 ];
	char *units;
	boolean bar_graph = 0;

	units = hydrology_library_get_units_string(
					&bar_graph,
					application_name,
					datatype_name );

	*units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype_name,
			units,
			units_converted_string,
			aggregate_statistic );

	if ( !*units_display )
	{
		document_quick_output_body(	application_name,
						appaserver_mount_point );
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype_name,
				units,
				units_converted_string ) );
		document_close();
		exit( 0 );
	}


	if ( units_converted_string
	&&   *units_converted_string
	&&   strcmp( units_converted_string, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s %d ',' %d",
			 application_name,
			 units,
			 units_converted_string,
			 VALUE_PIECE,
			 STATION_PIECE );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype_name,
				aggregate_statistic_none );
	}

	if ( is_weekly )
		aggregate_level_string = "weekly";
	else
		aggregate_level_string = "monthly";

	where_clause = get_where_clause(
			station_name,
			datatype_name,
			begin_date_string,
			end_date_string );

	sprintf( 	aggregation_process, 
			"real_time2aggregate_value.e %s %d %d %d '%c' %s n %s",
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
	 		DATE_PIECE,
	 		-1 /* time_piece */,
	 		VALUE_PIECE,
			INPUT_DELIMITER,
			aggregate_level_string,
			end_date_string );

	if ( period_wo_date_delta != delta_none )
	{
/* Needs work.
		sprintf(delta_process,
			"delta_values.e %d %s '%c'",
	 		VALUE_PIECE,
			period_wo_date_get_delta_string( period_wo_date_delta ),
			INPUT_DELIMITER );
*/
		strcpy( delta_process, "cat" );
	}
	else
	{
		strcpy( delta_process, "cat" );
	}

	sprintf(sys_string,
	"get_folder_data	application=%s		    "
	"			folder=measurement	    "
	"			select=\"%s\"		    "
	"			where=\"%s\"		   |"
	"tr '%c' '%c' 					   |"
	"%s						   |"
	"%s						   |"
	"%s						   |"
	"cat						    ",
		application_name,
		SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		aggregation_process,
		units_converted_process,
		delta_process );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, INPUT_DELIMITER, input_buffer, 0 );
		piece( value_string, INPUT_DELIMITER, input_buffer, 1 );
		piece( count_string, INPUT_DELIMITER, input_buffer, 2 );

		if ( is_weekly )
		{
			array_offset =
				period_wo_date_get_week_int( date_string );

		}
		else
		{
			array_offset =
				period_wo_date_get_month_int( date_string );
		}

		/* Note: array_offset is one based */
		/* ------------------------------- */
		period_wo_date_period = period_array[ array_offset - 1 ];

		year_int = period_wo_date_get_year_int( date_string );

		if ( ! ( period_wo_date_year_value =
			period_wo_date_seek_period_wo_date_year_value(
				period_wo_date_period->year_value_list,
				year_int ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot get period_wo_date_year_value.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ! ( period_wo_date_year_sum =
			period_wo_date_seek_period_wo_date_year_sum(
				year_sum_list,
				year_int ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot get period_wo_date_year_sum.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !*value_string || strcasecmp( value_string, "null" ) == 0 )
		{
			period_wo_date_year_value->is_null = 1;
		}
		else
		{
			period_wo_date_year_value->value = atof( value_string );
			period_wo_date_year_value->is_null = 0;
			period_wo_date_year_sum->sum +=
				period_wo_date_year_value->value;
		}
	}
	pclose( input_pipe );

} /* period_wo_date_populate_year_value_list() */

void period_wo_date_output_table(
		PERIOD_WO_DATE_PERIOD **period_array,
		double sum_minimum,
		double sum_average,
		double sum_maximum,
		LIST *year_sum_list,
		boolean is_weekly,
		char *station_name,
		char *datatype_name,
		enum aggregate_statistic aggregate_statistic,
		char *begin_date_string,
		char *end_date_string,
		char *delta_string,
		char *units_display_string )
{
	PERIOD_WO_DATE_PERIOD *period_wo_date_period;
	PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value;
	PERIOD_WO_DATE_YEAR_SUM *period_wo_date_year_sum;
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char title[ 512 ];
	char *aggregate_period_string;
	int count = 0;
	char output_buffer[ 16 ];
	char *delta_title;
	char value_heading_string[ 128 ];

	delta_title = get_delta_title( delta_string );

	if ( is_weekly )
		aggregate_period_string = "Weekly";
	else
		aggregate_period_string = "Monthly";

	if ( units_display_string )
	{
		sprintf(value_heading_string,
		 	"%s(%s)",
		 	aggregate_statistic_get_string(
				aggregate_statistic ),
		 	units_display_string );
	}
	else
	{
		sprintf(value_heading_string,
		 	"%s",
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
	}

	format_initial_capital( value_heading_string, value_heading_string );

	sprintf(	title,
			"Annual %s %s%s<br>from: %s to: %s<br>for %s/%s",
			aggregate_period_string,
			value_heading_string,
			delta_title,
			begin_date_string,
			end_date_string,
			station_name,
			datatype_name );

	html_table = new_html_table(
			format_initial_capital(
					title,
					title ),
			(char *)0 /* sub_title */ );

	heading_list = list_new();

	list_append_pointer( heading_list, "Period" );

	if ( is_weekly )
	{
		list_append_pointer( heading_list, "Representative<br>Date" );
	}

	period_wo_date_period = *period_array;

	if ( !list_rewind( period_wo_date_period->year_value_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty year_value_list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Output Heading */
	/* -------------- */
	do {
		period_wo_date_year_value =
			list_get_pointer(
				period_wo_date_period->year_value_list );

		sprintf(output_buffer,
			"%d",
			period_wo_date_year_value->year );

		list_append_pointer(
				heading_list,
				strdup( output_buffer ) );
	} while( list_next( period_wo_date_period->year_value_list ) );

	list_append_pointer( heading_list, "Minimum" );
	list_append_pointer( heading_list, "Average" );
	list_append_pointer( heading_list, "Maximum" );

	html_table->number_left_justified_columns = 0;
	html_table->number_right_justified_columns = 99;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	/* Output data */
	/* ----------- */
	while( ( period_wo_date_period = *period_array++ ) )
	{
		html_table_set_data(
				html_table->data_list,
				strdup( period_wo_date_period->period_label ) );

		if ( is_weekly )
		{
			html_table_set_data(
				html_table->data_list,
				strdup( period_wo_date_period->
				     	representative_week_date_string ) );
		}

		if ( !list_rewind( period_wo_date_period->year_value_list ) )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty year_value_list\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			period_wo_date_year_value =
				list_get_pointer(
					period_wo_date_period->
						year_value_list );

			if ( period_wo_date_year_value->is_null )
			{
				sprintf(output_buffer,
					"null" );
			}
			else
			{
				sprintf(output_buffer,
					"%.2lf",
					period_wo_date_year_value->
						value );
			}
	
			html_table_set_data(
				html_table->data_list,
				strdup( output_buffer ) );

		} while( list_next( period_wo_date_period->
						year_value_list ) );

		sprintf(output_buffer,
			"%.2lf",
			period_wo_date_period->minimum );

		html_table_set_data(
			html_table->data_list,
			strdup( output_buffer ) );

		sprintf(output_buffer,
			"%.2lf",
			period_wo_date_period->average );

		html_table_set_data(
			html_table->data_list,
			strdup( output_buffer ) );

		sprintf(output_buffer,
			"%.2lf",
			period_wo_date_period->maximum );

		html_table_set_data(
			html_table->data_list,
			strdup( output_buffer ) );

		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
		}

		if ( count % 2 )
			html_table_set_background_shaded( html_table );
		else
			html_table_set_background_unshaded( html_table );

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();
	}

	/* Output aggregation */
	/* ------------------ */
	if ( aggregate_statistic == sum )
	{
		html_table_set_data(
				html_table->data_list,
				strdup( "Total" ) );
	}
	else
	{
		html_table_set_data(
				html_table->data_list,
				strdup( "Average" ) );
	}

	if ( is_weekly )
	{
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );
	}

	if ( !list_rewind( year_sum_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty year_sum_list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		period_wo_date_year_sum =
			list_get_pointer(
				year_sum_list );

		sprintf(output_buffer,
			"%s",
			period_wo_date_get_aggregation_value(
				period_wo_date_year_sum->sum,
				aggregate_statistic,
				is_weekly,
				period_wo_date_year_sum->null_count ) );

		html_table_set_data(
			html_table->data_list,
			strdup( output_buffer ) );

	} while( list_next( year_sum_list ) );

	sprintf(output_buffer,
		"%s",
		period_wo_date_get_aggregation_average_value(
			sum_minimum,
			is_weekly ) );

	html_table_set_data(
		html_table->data_list,
		strdup( output_buffer ) );

	sprintf(output_buffer,
		"%s",
		period_wo_date_get_aggregation_average_value(
			sum_average,
			is_weekly ) );

	html_table_set_data(
		html_table->data_list,
		strdup( output_buffer ) );

	sprintf(output_buffer,
		"%s",
		period_wo_date_get_aggregation_average_value(
			sum_maximum,
			is_weekly ) );

	html_table_set_data(
		html_table->data_list,
		strdup( output_buffer ) );

	if ( !(++count % ROWS_BETWEEN_HEADING ) )
	{
		html_table_output_data_heading(
			html_table->heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );
	}

	if ( count % 2 )
		html_table_set_background_shaded( html_table );
	else
		html_table_set_background_unshaded( html_table );

	html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	list_free_string_list( html_table->data_list );

	html_table_close();

} /* period_wo_date_output_table() */

void period_wo_date_output_transmit(
		PERIOD_WO_DATE_PERIOD **period_array,
		double sum_minimum,
		double sum_average,
		double sum_maximum,
		LIST *year_sum_list,
		boolean is_weekly,
		char *station_name,
		char *datatype_name,
		enum aggregate_statistic aggregate_statistic,
		char *begin_date_string,
		char *end_date_string,
		char *document_root_directory,
		char *application_name,
		char *output_medium,
		char *units_display_string )
{
	char *output_filename;
	char *ftp_filename;
	pid_t process_id = getpid();
	FILE *output_pipe;
	FILE *output_file;
	char output_sys_string[ 512 ];
	PERIOD_WO_DATE_PERIOD *period_wo_date_period;
	PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value;
	PERIOD_WO_DATE_YEAR_SUM *period_wo_date_year_sum;
	char *aggregate_period_string;
	char value_heading_string[ 128 ];

	if ( !output_medium || strcmp( output_medium, "stdout" ) != 0 )
	{
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				document_root_directory,
				FILENAME_STEM,
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

/*
		sprintf( output_filename,
			 OUTPUT_FILE_SPREADSHEET,
			 appaserver_mount_point,
			 application_name, 
			 process_id );
*/
		
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

	}
	else
	{
/*
		sprintf( output_sys_string,
		 	"delimiter2padded_columns.e '|' 99" );
*/
		sprintf(output_sys_string,
		 	"tr '|' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );
	}

	output_pipe = popen( output_sys_string, "w" );

	if ( is_weekly )
		aggregate_period_string = "Weekly";
	else
		aggregate_period_string = "Monthly";

	if ( units_display_string )
	{
		sprintf(value_heading_string,
		 	"%s(%s)",
		 	aggregate_statistic_get_string(
				aggregate_statistic ),
		 	units_display_string );
	}
	else
	{
		sprintf(value_heading_string,
		 	"%s",
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
	}

	format_initial_capital( value_heading_string, value_heading_string );

	fprintf(	output_pipe,
			"Annual %s %s from: %s to: %s for %s/%s\n",
			aggregate_period_string,
			value_heading_string,
			begin_date_string,
			end_date_string,
			station_name,
			datatype_name );

	fprintf( output_pipe, "Period" );

	if ( is_weekly )
	{
		fprintf( output_pipe, "|Representative_Date" );
	}

	period_wo_date_period = *period_array;

	if ( !list_rewind( period_wo_date_period->year_value_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty year_value_list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		period_wo_date_year_value =
			list_get_pointer(
				period_wo_date_period->year_value_list );

		fprintf(output_pipe,
			"|%d",
			period_wo_date_year_value->year );
	} while( list_next( period_wo_date_period->year_value_list ) );

	fprintf( output_pipe, "|Minimum|Average|Maximum\n" );

	while( ( period_wo_date_period = *period_array++ ) )
	{
		fprintf(output_pipe,
			"%s",
			period_wo_date_period->period_label );

		if ( is_weekly )
		{
			fprintf(output_pipe,
				"|%s",
				period_wo_date_period->
				     	representative_week_date_string );
		}

		if ( !list_rewind( period_wo_date_period->year_value_list ) )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty year_value_list\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			period_wo_date_year_value =
				list_get_pointer(
					period_wo_date_period->
						year_value_list );

			if ( period_wo_date_year_value->is_null )
			{
				fprintf( output_pipe, "|null" );
			}
			else
			{
				fprintf(output_pipe,
					"|%.2lf",
					period_wo_date_year_value->value );
			}

		} while( list_next( period_wo_date_period->year_value_list ) );

		fprintf(output_pipe,
			"|%.2lf|%.2lf|%.2lf\n",
			period_wo_date_period->minimum,
			period_wo_date_period->average,
			period_wo_date_period->maximum );
	}

	if ( aggregate_statistic == sum )
	{
		fprintf(output_pipe,
			"Total" );
	}
	else
	{
		fprintf(output_pipe,
			"Average" );
	}

	if ( is_weekly )
	{
		fprintf(output_pipe,
			"|" );
	}

	if ( !list_rewind( year_sum_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty year_sum_list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		period_wo_date_year_sum =
			list_get_pointer( year_sum_list );

		fprintf(output_pipe,
			"|%s",
			period_wo_date_get_aggregation_value(
				period_wo_date_year_sum->sum,
				aggregate_statistic,
				is_weekly,
				period_wo_date_year_sum->null_count ) );

	} while( list_next( year_sum_list ) );

	fprintf(output_pipe,
		"|%s|%s|%s\n",
		period_wo_date_get_aggregation_average_value(
				sum_minimum,
				is_weekly ),
		period_wo_date_get_aggregation_average_value(
				sum_average,
				is_weekly ),
		period_wo_date_get_aggregation_average_value(
				sum_maximum,
				is_weekly ) );

	pclose( output_pipe );

	if ( !output_medium || strcmp( output_medium, "stdout" ) != 0 )
	{
		printf( "<h1>Annual Periods Transmission<br></h1>\n" );
		printf( "<h2>\n" );
		fflush( stdout );
		if ( system( "date '+%x %H:%M'" ) ){};
		fflush( stdout );
		printf( "</h2>\n" );
		
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
} /* period_wo_date_output_transmit() */

void output_invalid_output_medium( char *output_medium )
{
	printf(
"<h3>ERROR: the output medium of %s is not available for this function.</h3>\n",
		output_medium );
}

char *get_delta_title( char *delta_string )
{
	char delta_title[ 128 ];

	if ( delta_string && *delta_string )
		sprintf( delta_title, " delta %s", delta_string );
	else
		*delta_title = '\0';

	return strdup( delta_title );
} /* get_delta_title() */

char *get_date_range_string(	char *period_string,
				char *ending_period_string )
{
	char date_range_string[ 128 ];

	if ( ending_period_string && *ending_period_string )
	{
		sprintf(	date_range_string,
				"%s/%s",
				period_string,
				ending_period_string );
	}
	else
	{
		strcpy( date_range_string, period_string );
	}

	return strdup( date_range_string );

} /* get_date_range_string() */

