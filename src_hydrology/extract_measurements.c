/* -----------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/extract_measurements.c	*/
/* -----------------------------------------------------	*/
/* This process extracts the raw measurement data		*/
/* quickly and makes them available for download.		*/
/* -----------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define SELECT_LIST		"station,datatype,measurement_date,measurement_time,measurement_value"
#define ROWS_TO_SKIP		9

/* Prototypes */
/* ---------- */
void extract_measurements(	char *application_name,
				char *station_list_string,
				char *datatype_list_string,
				LIST *station_list,
				LIST *datatype_list,
				char *parameter_begin_date,
				char *parameter_end_date,
				APPASERVER_PARAMETER_FILE
					*appaserver_parameter_file,
				char *session,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *sort_process,
				char *process_name );

void extract_measurements_combined(
				char *application_name,
				char *station_list_string,
				char *datatype_list_string,
				LIST *station_list,
				LIST *datatype_list,
				char *parameter_begin_date,
				char *parameter_end_date,
				APPASERVER_PARAMETER_FILE
					*appaserver_parameter_file,
				char *session,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *sort_process,
				char *process_name );

char *get_heading_sys_string(
			char *application_name,
			char *datatype,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *output_filename );

char *get_datatype_heading_sys_string(
			char *application_name,
			char *datatype,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *output_filename );

char *get_combined_heading_sys_string(
			enum aggregate_level aggregate_level,
			char *output_filename );

char *get_sys_string(	char *application_name,
			char *table_name,
			char *datatype,
			char *where_clause,
			char *output_filename,
			char *sort_process,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *end_date );

char *get_combined_sys_string(
			char *application_name,
			char *table_name,
			LIST *station_list,
			LIST *datatype_list,
			char *date_where_clause,
			char *output_filename,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *end_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *parameter_begin_date, *parameter_end_date;
	char *station_list_string;
	char *datatype_list_string;
	LIST *station_list;
	LIST *datatype_list;
	char *aggregate_level_string;
	char *aggregate_statistic_string;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char *session;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *sort_process;
	char buffer[ 128 ];
	boolean combined_file;

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s application session process station datatype begin_date end_date aggregate_level aggregate_statistic combined_file_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	process_name = argv[ 3 ];
	station_list_string = argv[ 4 ];
	datatype_list_string = argv[ 5 ];
	parameter_begin_date = argv[ 6 ];
	parameter_end_date = argv[ 7 ];
	aggregate_level_string = argv[ 8 ];
	aggregate_statistic_string = argv[ 9 ];
	combined_file = (*argv[ 10 ] == 'y');

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

	aggregate_level =
		aggregate_level_get_aggregate_level( aggregate_level_string );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
				aggregate_statistic_string,
				aggregate_level );

	if ( aggregate_level == real_time || aggregate_level == daily )
	{
		sort_process = "sort -f";
	}
	else
	{
		sort_process = "sort -f -r";
	}

	if ( strcmp( station_list_string, "station" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: please choose a station/datatype.\n" );
		document_close();
		exit( 0 );
	}

	station_list = list_string2list( station_list_string, ',' );
	datatype_list = list_string2list( datatype_list_string, ',' );

	if ( list_length( station_list ) != list_length( datatype_list ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: station and datatype list don't match.\n" );
		document_close();
		exit( 0 );
	}

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

	printf( "<h1>%s<br></h1>\n",
		format_initial_capital( buffer, process_name ) );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	printf( "<table>\n" );

	if ( combined_file )
	{
		extract_measurements_combined(
				application_name,
				station_list_string,
				datatype_list_string,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				appaserver_parameter_file,
				session,
				aggregate_level,
				aggregate_statistic,
				sort_process,
				process_name );
	}
	else
	{
		extract_measurements(
				application_name,
				station_list_string,
				datatype_list_string,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				appaserver_parameter_file,
				session,
				aggregate_level,
				aggregate_statistic,
				sort_process,
				process_name );
	}

	printf( "</table>\n" );

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file->
					database_management_system );
	return 0;
} /* main() */

char *get_sys_string(	char *application_name,
			char *table_name,
			char *datatype,
			char *where_clause,
			char *output_filename,
			char *sort_process,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *end_date )
{
	static char sys_string[ 4096 ];
	char aggregation_process[ 256 ];
	enum aggregate_statistic local_aggregate_statistic;

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		local_aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype,
				aggregate_statistic );
	}
	else
	{
		local_aggregate_statistic = aggregate_statistic;
	}

	if ( aggregate_level == real_time )
	{
		strcpy( aggregation_process, "cat" );
	}
	else
	{
		sprintf( aggregation_process, 
			 "real_time2aggregate_value.e %s %d %d %d ',' %s n %s",
			 aggregate_statistic_get_string(
				local_aggregate_statistic ),
			 2 /* date_piece */,
			 3 /* time_piece */,
			 4 /* value_piece */,
			 aggregate_level_get_string( aggregate_level ),
			 end_date );
	}

	sprintf(sys_string,
	 	"echo \"select %s from %s where %s;\"		|"
		"sql_quick.e ','				|"
		"%s						|"
		"%s						|"
		"sort -f >> %s					 ",
		SELECT_LIST,
		table_name,
		where_clause,
		sort_process,
		aggregation_process,
		output_filename );

	return sys_string;
} /* get_sys_string() */

char *get_combined_sys_string(
			char *application_name,
			char *table_name,
			LIST *station_list,
			LIST *datatype_list,
			char *date_where_clause,
			char *output_filename,
			char *sort_process,
			enum aggregate_level aggregate_level,
			char *end_date )
{
	static char sys_string[ 65536 ];
	char *ptr = sys_string;
	char aggregation_process[ 256 ];
	enum aggregate_statistic aggregate_statistic;
	char *station_name;
	char *datatype_name;
	char where_clause[ 512 ];

	if ( !list_length( station_list )
	||   list_length( station_list ) != list_length( datatype_list ) )
	{
		return "";
	}

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station_name = list_get( station_list );
		datatype_name = list_get( datatype_list );

		if ( aggregate_level == real_time )
		{
			strcpy( aggregation_process, "cat" );
		}
		else
		{
			aggregate_statistic =
				based_on_datatype_get_aggregate_statistic(
					application_name,
					datatype_name,
					aggregate_statistic_none );

			sprintf( aggregation_process, 
			 "real_time2aggregate_value.e %s %d %d %d ',' %s n %s",
			 	aggregate_statistic_get_string(
					aggregate_statistic ),
			 	2 /* date_piece */,
			 	3 /* time_piece */,
			 	4 /* value_piece */,
			 	aggregate_level_get_string( aggregate_level ),
			 	end_date );
		}

		sprintf(where_clause,
		 	"station = '%s' and datatype = '%s' and %s",
		 	station_name,
		 	datatype_name,
		 	date_where_clause );

		if ( !list_at_head( datatype_list ) )
		{
			ptr += sprintf( ptr, "; " );
		}

		ptr += sprintf(	ptr,
	 	"echo \"select %s from %s where %s;\"		|"
		"sql_quick.e ','				|"
		"%s						|"
		"%s						|"
		"sort -f >> %s					 ",
				SELECT_LIST,
				table_name,
				where_clause,
				sort_process,
				aggregation_process,
				output_filename );

		list_next( station_list );

	} while( list_next( datatype_list ) );

	return sys_string;

} /* get_combined_sys_string() */

char *get_heading_sys_string(	char *application_name,
				char *datatype,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *output_filename )
{
	if ( datatype )
	{
		return get_datatype_heading_sys_string(
				application_name,
				datatype,
				aggregate_level,
				aggregate_statistic,
				output_filename );
	}
	else
	{
		return get_combined_heading_sys_string(
				aggregate_level,
				output_filename );
	}

} /* get_heading_sys_string() */

char *get_datatype_heading_sys_string(
				char *application_name,
				char *datatype,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *output_filename )
{
	static char sys_string[ 1024 ];
	boolean bar_graph;
	char value_label[ 32 ];
	char count_label[ 32 ];

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype,
				aggregate_statistic );
	}
	if ( aggregate_level == real_time )
	{
		strcpy( value_label, "value" );
		*count_label = '\0';
	}
	else
	{
		sprintf(value_label,
		 	"%s",
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
		strcpy( count_label, ",count" );
	}

	sprintf(sys_string,
	 	"echo \"station,datatype,date,time,%s(%s)%s\" >> %s",
		value_label,
		hydrology_library_get_units_string(
				&bar_graph,
				application_name,
				datatype ),
		count_label,
	 	output_filename );
	return sys_string;
} /* get_datatype_heading_sys_string() */

char *get_combined_heading_sys_string(
				enum aggregate_level aggregate_level,
				char *output_filename )
{
	static char sys_string[ 1024 ];
	char *count_label;

	if ( aggregate_level == real_time )
	{
		count_label = "";
	}
	else
	{
		count_label = ",count";
	}

	sprintf(sys_string,
	 	"echo \"station,datatype,date,time,value%s\" >> %s",
		count_label,
	 	output_filename );

	return sys_string;

} /* get_combined_heading_sys_string() */

void extract_measurements(	char *application_name,
				char *station_list_string,
				char *datatype_list_string,
				LIST *station_list,
				LIST *datatype_list,
				char *parameter_begin_date,
				char *parameter_end_date,
				APPASERVER_PARAMETER_FILE
					*appaserver_parameter_file,
				char *session,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *sort_process,
				char *process_name )
{
	char *station;
	char *datatype;
	char *begin_date;
	char *end_date;
	char end_date_suffix[ 128 ];
	char *output_filename;
	char *ftp_filename;
	char where_clause[ 4096 ];
	int record_count;
	char date_where_clause[ 128 ];
	char *sys_string;
	char *table_name;
	APPASERVER_LINK_FILE *appaserver_link_file;
	char filename_stem[ 128 ];

	if ( !list_length( station_list )
	||   list_length( station_list ) != list_length( datatype_list ) )
	{
		return;
	}

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			(char *)0 /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			"csv" );

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		begin_date = parameter_begin_date;
		end_date = parameter_end_date;

		hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station_list_string,
					datatype_list_string );

		if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
		{
			printf(
		"<p>Warning: no data available for %s/%s.\n",
				station,
				datatype );
			continue;
		}

		if ( strcmp( end_date, "end_date" ) != 0 )
			sprintf( end_date_suffix, "%s_", end_date );
		else
			*end_date_suffix = '\0';

		appaserver_link_file->begin_date_string = begin_date;
		appaserver_link_file->end_date_string = end_date;

		sprintf( filename_stem,
			 "%s_%s_%s",
			 process_name,
			 station,
			 datatype );

		output_filename =
			appaserver_link_get_output_filename(
				appaserver_link_file->
					output_file->
					document_root_directory,
				appaserver_link_file->application_name,
				filename_stem,
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
				filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

		if ( strcmp( end_date, "end_date" ) != 0 )
		{
			sprintf(date_where_clause,
				"measurement_date between '%s' and '%s'	",
				begin_date,
				end_date );
		}
		else
		{
			sprintf(date_where_clause,
				"measurement_date = '%s'		",
				begin_date );
		}

		sprintf(where_clause,
		 	"station = '%s' and datatype = '%s' and %s",
		 	station,
		 	datatype,
		 	date_where_clause );

		table_name = get_table_name( application_name, "measurement" );

		hydrology_library_output_station_text_filename(
					output_filename,
					application_name,
					station,
					1 /* with_zap_file */ );

		sys_string = get_heading_sys_string(
				application_name,
				datatype,
				aggregate_level,
				aggregate_statistic,
				output_filename );

		system( sys_string );

		sys_string = get_sys_string(
				application_name,
				table_name,
				datatype,
				where_clause,
				output_filename,
				sort_process,
				aggregate_level,
				aggregate_statistic,
				end_date );

		system( sys_string );

		sprintf( sys_string,
		 	"wc -l %s",
		 	output_filename );
		record_count = atoi( pipe2string( sys_string ) ) - ROWS_TO_SKIP;

		printf( "<tr><td>For %s/%s generated %d measurements:\n",
			station,
			datatype,
			record_count );
		
		printf( "<td><a href=\"%s\">%s</a></p>\n",
			ftp_filename,
			TRANSMIT_PROMPT );

		list_next( datatype_list );
	} while( list_next( station_list ) );

} /* extract_measurements() */

void extract_measurements_combined(
				char *application_name,
				char *station_list_string,
				char *datatype_list_string,
				LIST *station_list,
				LIST *datatype_list,
				char *parameter_begin_date,
				char *parameter_end_date,
				APPASERVER_PARAMETER_FILE
					*appaserver_parameter_file,
				char *session,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				char *sort_process,
				char *process_name )
{
	char *begin_date;
	char *end_date;
	char end_date_suffix[ 128 ];
	char *output_filename;
	char *ftp_filename;
	int record_count;
	char date_where_clause[ 128 ];
	char *sys_string;
	char *table_name;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( !list_length( station_list )
	||   list_length( station_list ) != list_length( datatype_list ) )
	{
		return;
	}

	begin_date = parameter_begin_date;
	end_date = parameter_end_date;

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station_list_string,
					datatype_list_string );


	if ( !appaserver_library_validate_begin_end_date(
				&begin_date,
				&end_date,
				(DICTIONARY *)0 /* post_dictionary */) )
	{
		return;
	}

	if ( strcmp( end_date, "end_date" ) != 0 )
		sprintf( end_date_suffix, "%s_", end_date );
	else
		*end_date_suffix = '\0';

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			process_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

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

	if ( strcmp( end_date, "end_date" ) != 0 )
	{
		sprintf(date_where_clause,
			"measurement_date between '%s' and '%s'	",
			begin_date,
			end_date );
	}
	else
	{
		sprintf(date_where_clause,
			"measurement_date = '%s'		",
			begin_date );
	}

	table_name = get_table_name( application_name, "measurement" );

	hydrology_library_output_station_text_filename(
				output_filename,
				application_name,
				list_get_first_pointer( station_list ),
				1 /* with_zap_file */ );

	sys_string = get_heading_sys_string(
			application_name,
			(char *)0 /* datatype */,
			aggregate_level,
			aggregate_statistic,
			output_filename );

	system( sys_string );

	sys_string = get_combined_sys_string(
			application_name,
			table_name,
			station_list,
			datatype_list,
			date_where_clause,
			output_filename,
			sort_process,
			aggregate_level,
			end_date );

	system( sys_string );

	sprintf( sys_string,
	 	"wc -l %s",
	 	output_filename );
	record_count = atoi( pipe2string( sys_string ) ) - ROWS_TO_SKIP;

	printf( "<tr><td>Generated %d measurements:\n",
		record_count );
	
	printf( "<td><a href=\"%s\">%s</a></p>\n",
		ftp_filename,
		TRANSMIT_PROMPT );

} /* extract_measurements_combined() */

