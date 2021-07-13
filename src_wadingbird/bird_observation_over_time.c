/* ---------------------------------------------------	*/
/* src_wadingbird/bird_observation_over_time.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "grace.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "hash_table.h"
#include "date.h"
#include "environ.h"
#include "date_convert.h"
#include "application.h"
#include "process.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char season_year_string[ 16 ];
	char initial_takeoff_date[ 16 ];
	int sum_bird_count;
	int sum_bird_estimation; 
} ANNUAL_HIGH_LOW;

/* Prototypes */
/* ---------- */
void output_average_text_file(	char *application_name,
				FILE *average_input_pipe,
				char *process_name );

void output_chart(		char *application_name,
				FILE *input_pipe,
				HASH_TABLE *annual_high_low_hash_table );

void output_text_file(		char *application_name,
				FILE *input_pipe,
				HASH_TABLE *annual_high_low_hash_table,
				char *process_name );

ANNUAL_HIGH_LOW *get_or_set_annual_high_low_hash_table(
				HASH_TABLE *annual_high_hash_table,
				char *season_year_string );

ANNUAL_HIGH_LOW *annual_high_low_new(
				char *season_year_string );

void parse_input_buffer(	char **season_year,
				char **initial_takeoff_date,
				int *sum_bird_count,
				int *sum_bird_estimation,
				char *input_buffer );

HASH_TABLE *get_annual_high_low_hash_table(
				char *application_name,
				char *common_name,
				char *begin_year_string,
				char *end_year_string,
				char *filter_sighting );

ANNUAL_HIGH_LOW *new_annual_high_low(
				char *year_string );

void output_table(		FILE *input_pipe,
				HASH_TABLE *annual_high_low_hash_table );

void output_average_table(	FILE *average_input_pipe );

char *get_latest_year_string(
				char *application_name );

char *get_earliest_year_string(
				char *application_name );

FILE *get_input_pipe(		char *application_name,
				char *common_name,
				char *begin_year_string,
				char *end_year_string );

FILE *get_average_input_pipe(	char *application_name,
				char *common_name,
				char *begin_year_string,
				char *end_year_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char buffer[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *begin_year_string;
	char *end_year_string;
	char *common_name;
	char *filter_sighting;
	char *output_medium;
	FILE *input_pipe = {0};
	FILE *average_input_pipe = {0};
	HASH_TABLE *annual_high_low_hash_table = {0};

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s application process begin_year end_year common_name highest|lowest|average output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	begin_year_string = argv[ 3 ];

	if ( 	!*begin_year_string
	|| 	strcmp( begin_year_string, "begin_year" ) == 0 )
	{
		begin_year_string =
			get_earliest_year_string(
				application_name );
	}

	end_year_string = argv[ 4 ];

	if ( 	!*end_year_string
	|| 	strcmp( end_year_string, "end_year" ) == 0 )
	{
		end_year_string =
			get_latest_year_string(
				application_name );
	}

	common_name = argv[ 5 ];
	filter_sighting = argv[ 6 ];
	output_medium = argv[ 7 ];

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
	add_local_bin_to_path();
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
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	if (	*filter_sighting
	&&	strcmp( filter_sighting, "filter_sighting" ) != 0 )
	{
		sprintf( buffer,
			 "%s (%s)",
			 process_name,
			 filter_sighting );
		format_initial_capital( buffer, buffer );
		printf( "<h1>%s</h1>\n", buffer );
	}
	else
	{
		printf( "<h1>%s</h1>\n",
			format_initial_capital( buffer, process_name ) );
	}

	printf( "<h2>From %s to %s<br>\n",
		begin_year_string,
		end_year_string );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*common_name || strcmp( common_name, "common_name" ) == 0 )
	{
		printf( "<h3>Please select a species.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( timlib_strncmp( filter_sighting, "average" ) == 0 )
	{
		average_input_pipe = get_average_input_pipe(
						application_name,
						common_name,
						begin_year_string,
						end_year_string );
	}
	else
	if ( timlib_strncmp( filter_sighting, "high" ) == 0
	||   timlib_strncmp( filter_sighting, "low" ) == 0 )
	{
		annual_high_low_hash_table =
			get_annual_high_low_hash_table(
						application_name,
						common_name,
						begin_year_string,
						end_year_string,
						filter_sighting );
	}
	else
	{
		input_pipe = get_input_pipe(	application_name,
						common_name,
						begin_year_string,
						end_year_string );
	}
					
	if ( strcmp( output_medium, "table" ) == 0
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		if ( !average_input_pipe )
		{
			output_table(	input_pipe,
					annual_high_low_hash_table );
		}
		else
		{
			output_average_table( average_input_pipe );
		}
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		if ( !average_input_pipe )
		{
			output_text_file(	application_name,
						input_pipe,
						annual_high_low_hash_table,
						process_name );
		}
		else
		{
			output_average_text_file(
						application_name,
						average_input_pipe,
						process_name );
		}
	}
	else
	if ( strcmp( output_medium, "chart" ) == 0
	||   strcmp( output_medium, "map" ) == 0 )
	{
		printf( "<h3>This function is not yet implemented.</h3>\n" );
/*
		output_chart(	application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				initial_takeoff_date,
				hydropattern_merge->cell_list,
				bird_count_estimation );
*/
	}

	if ( input_pipe ) pclose( input_pipe );
	if ( average_input_pipe ) pclose( average_input_pipe );
	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );
} /* main() */

char *get_earliest_year_string( char *application_name )
{
	char sys_string[ 1024 ];
	char *results_string;
	char *folder_name = "systematic_reconnaissance_flight";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"min(season_year)\"	"
		 "			folder=%s			",
		 application_name,
		 folder_name );

	results_string = pipe2string( sys_string );
	if ( !results_string )
		return "0000";
	else
		return results_string;

} /* get_earliest_year_string() */

char *get_latest_year_string( char *application_name )
{
	char sys_string[ 1024 ];
	char *results_string;
	char *folder_name = "systematic_reconnaissance_flight";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"max(season_year)\"	"
		 "			folder=%s			",
		 application_name,
		 folder_name );

	results_string = pipe2string( sys_string );
	if ( !results_string )
		return "0000";
	else
		return results_string;

} /* get_latest_year_string() */

FILE *get_input_pipe(	char *application_name,
			char *common_name,
			char *begin_year_string,
			char *end_year_string )
{
	char sys_string[ 1024 ];
	char select[ 512 ];
	char *systematic_reconnaissance_flight_table_name;
	char *bird_observation_table_name;
	char where[ 512 ];
	char group_by[ 256 ];

	systematic_reconnaissance_flight_table_name =
		get_table_name(	application_name,
				"systematic_reconnaissance_flight" );

	bird_observation_table_name =
		get_table_name( application_name, "bird_observation" );

	sprintf( select,
"season_year,%s.initial_takeoff_date,sum(bird_count),sum(bird_estimation)",
		 systematic_reconnaissance_flight_table_name );

	sprintf( where,
		 "season_year between %s and %s and			"
		 "common_name = '%s' and				"
		 "%s.initial_takeoff_date = %s.initial_takeoff_date	",
		 begin_year_string,
		 end_year_string,
		 common_name,
		 systematic_reconnaissance_flight_table_name,
		 bird_observation_table_name );

	sprintf( group_by,
		 "%s.initial_takeoff_date",
		 systematic_reconnaissance_flight_table_name );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s group by %s;\"	|"
		 "sql.e							 ",
		 select,
		 systematic_reconnaissance_flight_table_name,
		 bird_observation_table_name,
		 where,
		 group_by );

/*
fprintf( stderr, "%s\n", sys_string );
*/

	return popen( sys_string, "r" );
} /* get_input_pipe() */

FILE *get_average_input_pipe(	char *application_name,
				char *common_name,
				char *begin_year_string,
				char *end_year_string )
{
	char sys_string[ 1024 ];
	int results;
	char select[ 512 ];
	char *systematic_reconnaissance_flight_table_name;
	char *bird_observation_table_name;
	char where[ 512 ];
	char group_by[ 256 ];
	char count_filename[ 128 ];
	char estimation_filename[ 128 ];
	pid_t process_id = getpid();

	sprintf( count_filename, "/tmp/count_%d", process_id );
	sprintf( estimation_filename, "/tmp/estimation_%d", process_id );

	systematic_reconnaissance_flight_table_name =
		get_table_name(	application_name,
				"systematic_reconnaissance_flight" );

	bird_observation_table_name =
		get_table_name( application_name, "bird_observation" );

	sprintf( where,
		 "season_year between %s and %s and			"
		 "common_name = '%s' and				"
		 "%s.initial_takeoff_date = %s.initial_takeoff_date	",
		 begin_year_string,
		 end_year_string,
		 common_name,
		 systematic_reconnaissance_flight_table_name,
		 bird_observation_table_name );

	sprintf( group_by,
		 "%s.initial_takeoff_date",
		 systematic_reconnaissance_flight_table_name );

	/* First, average the bird count */
	/* ----------------------------- */
	strcpy( select,
		"season_year,sum(bird_count)" );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s group by %s;\"	|"
		 "sql.e '|' 						|"
		 "statistics_on_group.e year				|"
		 "egrep 'year|Average'					|"
		 "sed 's/year://'					|"
		 "sed 's/Average://'					|"
		 "joinlines.e '%c' 2					|"
		 "cat > %s						 ",
		 select,
		 systematic_reconnaissance_flight_table_name,
		 bird_observation_table_name,
		 where,
		 group_by,
		 FOLDER_DATA_DELIMITER,
		 count_filename );

	results = system( sys_string );

	/* Second, average the bird estimation */
	/* ----------------------------------- */
	strcpy( select,
		"season_year,sum(bird_estimation)" );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s group by %s;\"	|"
		 "sql.e '|' 						|"
		 "statistics_on_group.e year				|"
		 "egrep 'Average'					|"
		 "sed 's/Average://'					|"
		 "cat > %s						 ",
		 select,
		 systematic_reconnaissance_flight_table_name,
		 bird_observation_table_name,
		 where,
		 group_by,
		 estimation_filename );

	results = system( sys_string );

	/* Third, join the files */
	/* --------------------- */
	sprintf( sys_string,
		 "joinfiles.e %s %s '%c'",
		 count_filename,
		 estimation_filename,
		 FOLDER_DATA_DELIMITER );

/*
fprintf( stderr, "%s\n", sys_string );
*/

	return popen( sys_string, "r" );
} /* get_average_input_pipe() */

HASH_TABLE *get_annual_high_low_hash_table(
					char *application_name,
					char *common_name,
					char *begin_year_string,
					char *end_year_string,
					char *filter_sighting )
{
	FILE *input_pipe;
	HASH_TABLE *annual_high_low_hash_table;
	ANNUAL_HIGH_LOW *annual_high_low;
	char *season_year_string;
	char *initial_takeoff_date;
	int sum_bird_count;
	int sum_bird_estimation;
	char input_buffer[ 1024 ];
	int new_extreme;

	annual_high_low_hash_table = hash_table_new( hash_table_small );

	input_pipe = get_input_pipe(
				application_name,
				common_name,
				begin_year_string,
				end_year_string );

	while( get_line( input_buffer, input_pipe ) )
	{
		parse_input_buffer(	&season_year_string,
					&initial_takeoff_date,
					&sum_bird_count,
					&sum_bird_estimation,
					input_buffer );

		annual_high_low =
			get_or_set_annual_high_low_hash_table(
				annual_high_low_hash_table,
				season_year_string );

		/* If first time */
		/* ------------- */
		if ( !*annual_high_low->initial_takeoff_date )
		{
			strcpy(	annual_high_low->initial_takeoff_date,
				initial_takeoff_date );
			annual_high_low->sum_bird_count = sum_bird_count;
			annual_high_low->sum_bird_estimation =
				sum_bird_estimation;
		}
		else
		{
			new_extreme = 0;

			if ( timlib_strncmp( filter_sighting, "high" ) == 0 )
			{
				if ( sum_bird_count > annual_high_low->
							sum_bird_count )
				{
					new_extreme = 1;
				}
			}

			if ( timlib_strncmp( filter_sighting, "low" ) == 0 )
			{
				if ( sum_bird_count < annual_high_low->
							sum_bird_count )
				{
					new_extreme = 1;
				}
			}

			if ( new_extreme )
			{
				strcpy(	annual_high_low->initial_takeoff_date,
					initial_takeoff_date );
				annual_high_low->sum_bird_count =
					sum_bird_count;
				annual_high_low->sum_bird_estimation =
					sum_bird_estimation;
			}
		}
	}

	pclose( input_pipe );
	return annual_high_low_hash_table;

} /* get_annual_high_hash_table() */

void parse_input_buffer(	char **season_year,
				char **initial_takeoff_date,
				int *sum_bird_count,
				int *sum_bird_estimation,
				char *input_buffer )
{
	static char local_season_year[ 16 ];
	static char local_initial_takeoff_date[ 16 ];
	char local_sum_bird_count[ 16 ];
	char local_sum_bird_estimation[ 16 ];

	piece(	local_season_year,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		0 );

	piece(	local_initial_takeoff_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		1 );

	piece(	local_sum_bird_count,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		2 );

	piece(	local_sum_bird_estimation,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		3 );

	*season_year = local_season_year;
	*initial_takeoff_date = local_initial_takeoff_date;
	*sum_bird_count = atoi( local_sum_bird_count );
	*sum_bird_estimation = atoi( local_sum_bird_estimation );

} /* parse_input_buffer() */

ANNUAL_HIGH_LOW *get_or_set_annual_high_low_hash_table(
				HASH_TABLE *annual_high_low_hash_table,
				char *season_year_string )
{
	ANNUAL_HIGH_LOW *annual_high_low;

	if ( ( annual_high_low = hash_table_get_pointer(
				annual_high_low_hash_table,
				season_year_string ) ) )
	{
		return annual_high_low;
	}
	else
	{
		annual_high_low = annual_high_low_new( season_year_string );

		hash_table_set_pointer(
			annual_high_low_hash_table,
			annual_high_low->season_year_string,
			annual_high_low );

		return annual_high_low;
	}

} /* get_or_set_annual_high_low_hash_table() */

ANNUAL_HIGH_LOW *annual_high_low_new( char *season_year_string )
{
	ANNUAL_HIGH_LOW *annual_high_low;

	if ( ! ( annual_high_low = calloc( 1, sizeof( ANNUAL_HIGH_LOW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !timlib_strcpy(	annual_high_low->season_year_string,
				season_year_string,
				16 ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: year string of (%s) is too big.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			season_year_string );
		exit( 1 );
	}
	return annual_high_low;
} /* annual_high_low_new() */

void output_table(	FILE *input_pipe,
			HASH_TABLE *annual_high_low_hash_table )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char heading[ 512 ];
	char justification[ 512 ];
	char input_buffer[ 1024 ];

	strcpy(	heading,
	"season_year,initial_takeoff_date,bird_count,bird_estimation" );

	strcpy( justification,
		"left,left,right,right" );

	sprintf( sys_string,
		 "html_table.e '' %s '%c' %s",
		 heading,
		 FOLDER_DATA_DELIMITER,
		 justification );

	output_pipe = popen( sys_string, "w" );

	if ( input_pipe )
	{
		while( get_line( input_buffer, input_pipe ) )
		{
			fprintf( output_pipe, "%s\n", input_buffer );
		}
	}
	else
	if ( annual_high_low_hash_table )
	{
		LIST *season_year_key_list;
		char *season_year;
		ANNUAL_HIGH_LOW *annual_high_low;

		season_year_key_list =
			hash_table_get_ordered_key_list(
				annual_high_low_hash_table );

		if ( !list_rewind( season_year_key_list ) ) return;

		do {
			season_year =
				list_get_pointer( 
					season_year_key_list );

			annual_high_low =
				hash_table_get_pointer(
					annual_high_low_hash_table,
					season_year );

			fprintf( output_pipe,
				 "%s^%s^%d^%d\n",
				 annual_high_low->season_year_string,
				 annual_high_low->initial_takeoff_date,
				 annual_high_low->sum_bird_count,
				 annual_high_low->sum_bird_estimation );

		} while( list_next( season_year_key_list ) );
	}

} /* output_table() */

void output_average_table( FILE *average_input_pipe )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char heading[ 512 ];
	char justification[ 512 ];
	char input_buffer[ 1024 ];

	strcpy(	heading,
	"season_year,bird_count,bird_estimation" );

	strcpy( justification,
		"left,right,right" );

	sprintf( sys_string,
		 "html_table.e '' %s '%c' %s",
		 heading,
		 FOLDER_DATA_DELIMITER,
		 justification );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, average_input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}

} /* output_average_table() */

void output_text_file(	char *application_name,
			FILE *input_pipe,
			HASH_TABLE *annual_high_low_hash_table,
			char *process_name )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	FILE *output_file;
	char *output_filename;
	char *ftp_filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char input_buffer[ 1024 ];
	int process_id = getpid();
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_parameter_file = appaserver_parameter_file_new();

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

/*
	sprintf( output_filename, 
		 OUTPUT_TEMPLATE,
		 appaserver_parameter_file->appaserver_mount_point,
		 application_name, 
		 process_id );
*/

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf(
		"<h3>ERROR: Cannot open output file %s</h3>\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	sprintf( sys_string,
		 "cat > %s",
		 output_filename );
	output_pipe = popen( sys_string, "w" );



	fprintf( output_pipe,
	"Season Year,Initial Takeoff_date,Bird Count,Bird Estimation\n" );

	if ( input_pipe )
	{
		while( get_line( input_buffer, input_pipe ) )
		{
			search_replace_character(
				input_buffer,
				FOLDER_DATA_DELIMITER,
				',' );

			fprintf( output_pipe, "%s\n", input_buffer );
		}
	}
	else
	if ( annual_high_low_hash_table )
	{
		LIST *season_year_key_list;
		char *season_year;
		ANNUAL_HIGH_LOW *annual_high_low;

		season_year_key_list =
			hash_table_get_ordered_key_list(
				annual_high_low_hash_table );

		if ( !list_rewind( season_year_key_list ) ) return;

		do {
			season_year =
				list_get_pointer( 
					season_year_key_list );

			annual_high_low =
				hash_table_get_pointer(
					annual_high_low_hash_table,
					season_year );

			fprintf( output_pipe,
				 "%s,%s,%d,%d\n",
				 annual_high_low->season_year_string,
				 annual_high_low->initial_takeoff_date,
				 annual_high_low->sum_bird_count,
				 annual_high_low->sum_bird_estimation );

		} while( list_next( season_year_key_list ) );
	}

	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
		ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
		(char *)0 /* target */,
		(char *)0 /* application_type */ );

} /* output_text_file() */

void output_average_text_file(	char *application_name,
				FILE *average_input_pipe,
				char *process_name )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	FILE *output_file;
	char *output_filename;
	char *ftp_filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char input_buffer[ 1024 ];
	int process_id = getpid();
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_parameter_file = appaserver_parameter_file_new();

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

/*
	sprintf( output_filename, 
		 OUTPUT_TEMPLATE,
		 appaserver_parameter_file->appaserver_mount_point,
		 application_name, 
		 process_id );
*/

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf(
		"<h3>ERROR: Cannot open output file %s</h3>\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	sprintf( sys_string,
		 "cat > %s",
		 output_filename );
	output_pipe = popen( sys_string, "w" );


	fprintf( output_pipe,
	"Season Year,Bird Count,Bird Estimation\n" );

	while( get_line( input_buffer, average_input_pipe ) )
	{
		search_replace_character(
			input_buffer,
			FOLDER_DATA_DELIMITER,
			',' );

		fprintf( output_pipe, "%s\n", input_buffer );
	}

	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
		ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
		(char *)0 /* target */,
		(char *)0 /* application_type */ );

} /* output_average_text_file() */

