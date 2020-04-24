/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/output_measurement_data_sets.c	*/
/* -------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "date.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "validation_level.h"
#include "aggregate_level.h"
#include "application.h"
#include "aggregate_statistic.h"
#include "julian.h"
#include "hydrology_library.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define DATE_TIME_DELIMITER		':'
#define PROCESS_NAME			"transmit_data_sets"
#define DATE_PIECE			0
#define TIME_PIECE			1
#define MEASUREMENT_VALUE_PIECE		2
#define SELECT_LIST			 "measurement_date,measurement_time,measurement_value"
#define REVERSE_ORDER_CLAUSE	 	"measurement_date desc,measurement_time desc"

#define PROMPT_REAL_TIME_TEMPLATE		 "For %d measurements per day, &lt;Left Click&gt; to view or &lt;Right Click&gt; save."

#define PROMPT_NON_REAL_TIME_TEMPLATE		 "&lt;Left Click&gt; to view or &lt;Right Click&gt; to save."

/* Type definitions */
/* ---------------- */
typedef struct
{
	char *date_colon_time;
	double measurement_value;
} MEASUREMENT_OUTPUT;

typedef struct
{
	char *datatype;
	char *begin_measurement_date;
	char *begin_measurement_time;
	JULIAN *begin_measurement_julian;
	HASH_TABLE *measurement_hash_table;
} DATATYPE;

typedef struct
{
	int expected_count;
	LIST *datatype_list;
	LIST *date_colon_time_slot_list;
} EXPECTED_COUNT;

typedef struct
{
	LIST *expected_count_list;
} TRANSMIT_MEASUREMENT_SETS;

/* Prototypes */
/* ---------- */
MEASUREMENT_OUTPUT *measurement_output_new(
				char *date_colon_time,
				double measurement_value );

char *expected_count_list_get_display(
				LIST *expected_count_list );

char *local_datatype_list_display(
				LIST *datatype_list );

char *date_colon_time_slot_list_display(
				LIST *date_colon_time_slot_list );

char *get_justify_column_list_string(
				char *heading_string,
				enum aggregate_level );

void output_measurement_data_sets_transmit(
				enum validation_level,
				LIST *expected_count_list,
				char *application_name,
				char *station,
				char *begin_date,
				int process_id,
				char *argv_0,
				enum aggregate_level,
				char *document_root_directory,
				boolean aggregate_level_changed_to_daily );

void output_measurement_data_sets_table(
				enum validation_level,
				LIST *expected_count_list,
				char *application_name,
				char *station,
				char *begin_date,
				int process_id,
				char *argv_0,
				enum aggregate_level,
				char *document_root_directory );

LIST *get_datatype_expected_count_list(
				LIST *expected_count_list,
				char *datatype_name );

void transmit_measurement_sets_populate_expected_count_list_datatype(
				LIST *expected_count_list,
				char *station,
				enum aggregate_level aggregate_level,
				char *begin_date,
				char *end_date );

HASH_TABLE *get_measurement_hash_table(
				LIST *expected_count_list,
				char *datatype_name,
				JULIAN *measurement_julian );

LIST *get_datatype_name_list(	LIST *expected_count_list );

void transmit_measurement_sets_populate_measurement_hash_table(
				LIST *expected_count_list,
				char *station,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				char *order_clause,
				int date_piece,
				int time_piece,
				int measurement_value_piece,
				enum validation_level,
				char *application_name );

EXPECTED_COUNT *get_or_set_expected_count(
				LIST *expected_count_list,
				int count,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level );

TRANSMIT_MEASUREMENT_SETS *transmit_measurement_sets_new(
				void );

LIST *get_date_colon_time_slot_list(
				int measurements_per_day,
				char *begin_date,
				char *end_date,
				enum aggregate_level );

void datatype_set_measurement_record(
				LIST *expected_count_list,
				char *measurement_record,
				int date_piece,
				int time_piece,
				int measurement_value_piece,
				char *datatype_name );

void output_measurement_data_sets_to_file(
				char *output_filename,
				char *station,
				EXPECTED_COUNT *expected_count,
				char *argv_0,
				enum aggregate_level );

EXPECTED_COUNT *expected_count_new(
				int expected_count,
				char *begin_date,
				char *end_date,
				enum aggregate_level );

void expected_count_append_datatype(
				LIST *datatype_list,
				char *datatype_string,
				char *begin_measurement_date,
				char *begin_measurement_time );

char *get_measurement_record_list_sys_string(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_date,
				char *end_date,
				enum aggregate_level,
				int date_piece,
				int time_piece,
				int measurement_value_piece,
				char *order_clause,
				enum validation_level );

int main( int argc, char **argv )
{
	char *application_name;
	char *aggregate_level_string;
	char *validation_level_string;
	char *station;
	char *begin_date, *end_date;
	TRANSMIT_MEASUREMENT_SETS *transmit_measurement_sets;
	int date_piece = DATE_PIECE;
	int time_piece = -1;
	int measurement_value_piece = MEASUREMENT_VALUE_PIECE;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *order_clause = "select";
	int process_id = getpid();
	enum validation_level validation_level;
	enum aggregate_level aggregate_level;
	char *output_medium;
	boolean aggregate_level_changed_to_daily = 0;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s ignored ignored station begin_date end_date aggregate_level validation_level output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	/* application_name = argv[ 1 ]; */
	/* session = argv[ 2 ]; */
	station = argv[ 3 ];
	begin_date = argv[ 4 ];
	end_date = argv[ 5 ];
	aggregate_level_string = argv[ 6 ];
	validation_level_string = argv[ 7 ];
	output_medium = argv[ 8 ];

	aggregate_level =
		aggregate_level_get_aggregate_level(
			aggregate_level_string );

	if ( ( ( !*begin_date || strcmp( begin_date, "begin_date" ) == 0 )
	&&   ( !*end_date || strcmp( end_date, "end_date" ) == 0 ) )
	||   ( date_years_between( begin_date, end_date ) > 2 ) )
	{
		if ( aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			aggregate_level = daily;
			aggregate_level_changed_to_daily = 1;
		}
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(	application_name,
					appaserver_parameter_file->
					appaserver_mount_point );

	hydrology_library_get_clean_begin_end_date(
		&begin_date,
		&end_date,
		application_name,
		station,
		(char *)0 /* datatype */ );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					appaserver_parameter_file->
						database_management_system,
					(PROCESS_GENERIC_OUTPUT *)0,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	validation_level =
		validation_level_get_validation_level(
			validation_level_string);

	if ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		time_piece = TIME_PIECE;

		if ( aggregate_level == hourly )
			order_clause = REVERSE_ORDER_CLAUSE;
	}
	else
	{
		if ( aggregate_level != daily )
			order_clause = REVERSE_ORDER_CLAUSE;
	}

	transmit_measurement_sets = transmit_measurement_sets_new();

	transmit_measurement_sets_populate_expected_count_list_datatype(
			transmit_measurement_sets->expected_count_list,
			station,
			aggregate_level,
			begin_date,
			end_date );

	if ( !list_length( transmit_measurement_sets->expected_count_list ) )
	{
		printf( "<h1>ERROR</h1>\n" );
		printf(
"<p>There is no data collection frequency assigned to the datatypes for this station.\n");
		document_close();
		exit( 1 );
	}

/*
fprintf( stderr, "%s/%s()/%d: got expected_count_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
expected_count_list_get_display(
	transmit_measurement_sets->expected_count_list ) );
*/

	transmit_measurement_sets_populate_measurement_hash_table(
			transmit_measurement_sets->expected_count_list,
			station,
			begin_date,
			end_date,
			aggregate_level,
			order_clause,
			date_piece,
			time_piece,
			measurement_value_piece,
			validation_level,
			application_name );

	/* ------------- */
	/* Do the output */
	/* ------------- */

	if ( strcmp( output_medium, "transmit" ) == 0
	||   strcmp( output_medium, "text_file" ) == 0 )
	{
		output_measurement_data_sets_transmit(
			validation_level,
			transmit_measurement_sets->expected_count_list,
			application_name,
			station,
			begin_date,
			process_id,
			argv[ 0 ],
			aggregate_level,
			 appaserver_parameter_file->
			 	document_root,
			aggregate_level_changed_to_daily );
	}
	else
	{
		hydrology_library_output_station_table(
			application_name,
			station );

		output_measurement_data_sets_table(
			validation_level,
			transmit_measurement_sets->expected_count_list,
			application_name,
			station,
			begin_date,
			process_id,
			argv[ 0 ],
			aggregate_level,
			appaserver_parameter_file->
			 	document_root );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

void output_measurement_data_sets_transmit(
			enum validation_level validation_level,
			LIST *expected_count_list,
			char *application_name,
			char *station,
			char *begin_date,
			int process_id,
			char *argv_0,
			enum aggregate_level aggregate_level,
			char *document_root_directory,
			boolean aggregate_level_changed_to_daily )
{
	EXPECTED_COUNT *expected_count;
	char *output_filename;
	char *ftp_filename;
	char end_date_string[ 16 ];
	char prompt_string[ 128 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			PROCESS_NAME /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date;

	printf( "<h1>Transmit Measurement Data Sets " );

	if ( validation_level == provisional
	||   validation_level == validated )
	{
		printf( "(%s) ",
			validation_level_get_string( validation_level ) );
	}

	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	fflush( stdout );
	printf( "</h1>\n" );

	printf( "<hr>\n" );

	if ( aggregate_level_changed_to_daily )
	{
		printf( "<h2>Aggregate Level Change to Daily</h2>\n" );
	}

	list_rewind( expected_count_list );
	do {
		expected_count =
			list_get_pointer(
				expected_count_list );

		sprintf( end_date_string,
			 "%d",
			 expected_count->expected_count );

		appaserver_link_file->end_date_string =
			end_date_string;

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

		if ( aggregate_level == real_time )
		{
			sprintf(prompt_string,
			 	PROMPT_REAL_TIME_TEMPLATE,
			 	expected_count->expected_count );
		}
		else
		{
			strcpy( prompt_string, PROMPT_NON_REAL_TIME_TEMPLATE );
		}

		appaserver_library_output_ftp_prompt(
				ftp_filename, 
				prompt_string, 
				(char *)0 /* target */,
				(char *)0 /* application_type */ );

		hydrology_library_output_station_text_filename(
					output_filename,
					application_name,
					station,
					0 /* not with_zap_file */ );

		output_measurement_data_sets_to_file(
				output_filename,
				station,
				expected_count,
				argv_0,
				aggregate_level );

	} while( list_next( expected_count_list ) );

} /* output_measurement_data_sets_transmit() */

void output_measurement_data_sets_table(
			enum validation_level validation_level,
			LIST *expected_count_list,
			char *application_name,
			char *station,
			char *begin_date,
			int process_id,
			char *argv_0,
			enum aggregate_level aggregate_level,
			char *document_root_directory )
{
	EXPECTED_COUNT *expected_count;
	char *output_filename;
	char prompt_string[ 128 ];
	FILE *output_pipe;
	FILE *input_file;
	char sys_string[ 1024 ];
	char title[ 128 ];
	char input_buffer[ 1024 ];
	char heading_string[ 1024 ];
	char end_date_string[ 16 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			PROCESS_NAME /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date;

	list_rewind( expected_count_list );
	do {
		expected_count =
			list_get_pointer(
				expected_count_list );

/*
		sprintf( output_filename, 
			 OUTPUT_FILE_TEMPLATE,
			 appaserver_mount_point,
			 application_name, 
			 station,
			 expected_count->expected_count,
			 begin_date,
			 process_id );
*/

		sprintf( end_date_string,
			 "%d",
			 expected_count->expected_count );

		appaserver_link_file->end_date_string =
			end_date_string;

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

		if ( aggregate_level == real_time )
		{
			sprintf(prompt_string,
			 	PROMPT_REAL_TIME_TEMPLATE,
			 	expected_count->expected_count );
		}
		else
		{
			strcpy( prompt_string, PROMPT_NON_REAL_TIME_TEMPLATE );
		}

		if ( aggregate_level == aggregate_level_none
		||   aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly
		||   aggregate_level == daily )
		{
			sprintf(	title,
					"Measurement Data Sets: %d Per Day",
					expected_count->expected_count );
		}
		else
		{
			strcpy(	title, "Measurement Data Sets" );
		}

		if ( validation_level == provisional
		||   validation_level == validated )
		{
			sprintf(title + strlen( title ),
				" (%s)",
				validation_level_get_string(
					validation_level ) );
		}

		output_measurement_data_sets_to_file(
				output_filename,
				station,
				expected_count,
				argv_0,
				aggregate_level );

		input_file = fopen( output_filename, "r" );
		if ( !input_file )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot open file for read: %s.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				output_filename );
			exit( 1 );
		}

		if ( !get_line( input_buffer, input_file ) )
		{
			fclose( input_file );
			fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot get heading in file: %s.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				output_filename );
			exit( 1 );
		}

		strcpy( heading_string, input_buffer );

		sprintf(sys_string,
			"html_table.e \"%s\" \"%s\" ',' '%s'",
			title,
	        	heading_string,
			get_justify_column_list_string(
				heading_string,
				aggregate_level ) );

		output_pipe = popen( sys_string, "w" );
		while( get_line( input_buffer, input_file ) )
		{
			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( output_pipe );
		fclose( input_file );

	} while( list_next( expected_count_list ) );

} /* output_measurement_data_sets_table() */

char *get_measurement_record_list_sys_string(
			char *application_name,
			char *station,
			char *datatype,
			char *begin_date,
			char *end_date,
			enum aggregate_level aggregate_level,
			int date_piece,
			int time_piece,
			int measurement_value_piece,
			char *order_clause,
			enum validation_level validation_level )
{
	char sys_string[ 4096 ];
	char aggregate_process[ 1024 ];
	char *round_down_hourly_process = "cat";
	char where_clause[ 4096 ];
	char *date_time_where;
	enum aggregate_statistic aggregate_statistic;

	if ( hydrology_library_get_aggregation_sum(
		application_name, datatype ) )
	{
		aggregate_statistic =
			aggregate_statistic_get_aggregate_statistic(
				"sum",
				daily );
	}
	else
	{
		aggregate_statistic =
			aggregate_statistic_get_aggregate_statistic(
				"average",
				daily );
	}

	date_time_where = query_get_between_date_time_where(
					"measurement_date",
					"measurement_time",
					begin_date,
					"0000",
					end_date,
					"2359",
					(char *)0 /* application */,
					(char *)0 /* folder_name */ );

	sprintf(	where_clause, 
			"station = '%s' and datatype = '%s' and %s",
			station,
			datatype,
			date_time_where );

	strcat( where_clause,
		/* ----------------------- */
		/* Returns program memory. */
		/* ----------------------- */
		hydrology_library_provisional_where(
			validation_level ) );

	if ( aggregate_level == real_time )
	{
		strcpy( aggregate_process, "cat" );
	}
	else
	{
		sprintf( aggregate_process, 
			 "real_time2aggregate_value.e %s %d %d %d '%c' %s n %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 date_piece,
			 time_piece,
			 measurement_value_piece,
			 FOLDER_DATA_DELIMITER,
			 aggregate_level_get_string( aggregate_level ),
			 end_date );

		if ( aggregate_level == hourly )
		{
			round_down_hourly_process = "sed 's/50\\^/00^/'";
		}
	}

	sprintf( sys_string,
		"get_folder_data	application=%s			 "
		"			folder=measurement		 "
		"			select='%s'			 "
		"			where=\"%s\"			 "
		"			order='%s'			|"
		"%s 							|"
		"%s							|"
		"sort							 ",
		application_name,
		SELECT_LIST,
		where_clause,
		order_clause,
		aggregate_process,
		round_down_hourly_process );

	return strdup( sys_string );

} /* get_measurement_record_list_sys_string() */

EXPECTED_COUNT *expected_count_new(	int expected_count,
					char *begin_date,
					char *end_date,
					enum aggregate_level aggregate_level )
{
	EXPECTED_COUNT *a;

	a = (EXPECTED_COUNT *)calloc( 1, sizeof( EXPECTED_COUNT ) );
	a->expected_count = expected_count;
	a->datatype_list = list_new_list();
	a->date_colon_time_slot_list = 
		get_date_colon_time_slot_list(	expected_count,
						begin_date,
					   	end_date,
						aggregate_level );

	return a;
} /* expected_count_new() */

void expected_count_append_datatype(	LIST *datatype_list,
					char *datatype_string,
					char *begin_measurement_date,
					char *begin_measurement_time )
{
	DATATYPE datatype;

	datatype.datatype = datatype_string;

	if ( !*begin_measurement_date ) begin_measurement_date = "0000-00-00";
	begin_measurement_time =
		julian_get_clean_hhmm( begin_measurement_time );
	datatype.begin_measurement_date = begin_measurement_date;
	datatype.begin_measurement_time = begin_measurement_time;

	if ( ! ( datatype.begin_measurement_julian =
		julian_yyyy_mm_dd_hhmm_new(	begin_measurement_date,
						begin_measurement_time ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get julian date for (%s:%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 begin_measurement_date,
			 begin_measurement_time );
		exit( 1 );
	}

	datatype.measurement_hash_table = hash_table_new( hash_table_small );
	list_append( datatype_list, &datatype, sizeof( DATATYPE ) );
} /* expected_count_append_datatype() */

void datatype_set_measurement_record(
				LIST *expected_count_list,
				char *measurement_record,
				int date_piece,
				int time_piece,
				int measurement_value_piece,
				char *datatype_name )
{
	char date[ 16 ];
	char time[ 16 ];
	char measurement_string[ 32 ];
	char date_colon_time[ 32 ];
	double measurement_value;
	HASH_TABLE *measurement_hash_table;
	MEASUREMENT_OUTPUT *measurement;
	static JULIAN *measurement_julian = {0};

	piece( 	measurement_string, 
		FOLDER_DATA_DELIMITER, 
		measurement_record, 
		measurement_value_piece );

	if ( !*measurement_string )
	{
		return;
	}
	else
	{
		measurement_value = atof( measurement_string );
	}

	piece( date, FOLDER_DATA_DELIMITER, measurement_record, date_piece );

	if ( time_piece == -1 )
	{
		strcpy( time, "0000" );
	}
	else
	{
		piece(	time, 
			FOLDER_DATA_DELIMITER, 
			measurement_record, 
			time_piece );
	}

	strcpy( time, julian_get_clean_hhmm( time ) );

	if ( !measurement_julian )
		measurement_julian = julian_new( 0.0 );

	julian_set_yyyy_mm_dd_hhmm(	measurement_julian,
					date,
					time );

	sprintf( date_colon_time, "%s:%s", date, time );

	measurement = measurement_output_new(
				strdup( date_colon_time ),
				measurement_value );

	measurement_hash_table =
		get_measurement_hash_table(
			expected_count_list,
			datatype_name,
			measurement_julian );

	if ( !measurement_hash_table )
	{
		return;
	}

	hash_table_set( measurement_hash_table,
			measurement->date_colon_time,
			measurement );

} /* datatype_set_measurement_record() */


void output_measurement_data_sets_to_file(
					char *output_filename,
					char *station,
					EXPECTED_COUNT *expected_count,
					char *argv_0,
					enum aggregate_level aggregate_level )
{
	FILE *f;
	DATATYPE *datatype;
	char *date_colon_time_slot;
	MEASUREMENT_OUTPUT *measurement;
	double time_percent_of_day;
	char date_colon_time_slot_display[ 128 ];
	char time_string[ 128 ];

	f = fopen( output_filename, "a" );
	if ( !f )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot open %s for write.\n",
			 argv_0,
			 output_filename );
		exit( 1 );
	}

	/* Output the heading */
	/* ------------------ */
	if ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly
	||   aggregate_level == aggregate_level_none )
	{
		fprintf( f, 
			"station,date:time,time/2400" );
	}
	else
	{
		fprintf( f, 
			"station,date(aggregation %s)",
			aggregate_level_get_string(
				aggregate_level ) );
	}

	if ( !list_rewind( expected_count->datatype_list ) ) return;
	do {
		datatype = list_get_pointer( expected_count->datatype_list );

		if ( !hash_table_length( datatype->measurement_hash_table ) )
		{
			continue;
		}

		fprintf( f, ",%s", datatype->datatype );
	} while( list_next( expected_count->datatype_list ) );
	fprintf( f, "\n" );

	if ( !list_rewind( expected_count->date_colon_time_slot_list ) )
	{
		fclose( f );
		return;
	}

	do {
		date_colon_time_slot = 
			list_get_pointer(
				expected_count->date_colon_time_slot_list );

		strcpy( date_colon_time_slot_display, date_colon_time_slot );

		if ( aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			piece(	time_string,
				DATE_TIME_DELIMITER,
				date_colon_time_slot_display, 1 );

			time_percent_of_day = get_percent_of_day( time_string );

			fprintf( f, 
				 "%s,%s,%.3lf",
			 	 station, 
				 date_colon_time_slot_display, 
				 time_percent_of_day );
		}
		else
		{
			/* Zap the time which is "0000" */
			/* ---------------------------- */
			*(date_colon_time_slot_display + 10) = '\0';
			fprintf(f,
				"%s,%s",
				station,
				date_colon_time_slot_display );
		}

		if ( !list_rewind( expected_count->datatype_list ) ) return;
		do {
			datatype =
				list_get_pointer(
					expected_count->datatype_list );

			if ( !hash_table_length(
				datatype->measurement_hash_table ) )
			{
				continue;
			}

			measurement =
				hash_table_fetch(
					datatype->measurement_hash_table,
					date_colon_time_slot );

			if ( !measurement )
			{
				fprintf( f, "," );
			}
			else
			{
				fprintf(f,
					",%.3lf",
					measurement->measurement_value );
			}

		} while( list_next( expected_count->datatype_list ) );

		fprintf( f, "\n" );

	} while( list_next( expected_count->date_colon_time_slot_list ) );

	fclose( f );

} /* output_measurement_data_sets_to_file() */

LIST *get_date_colon_time_slot_list(
				int measurements_per_day,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "time_ticker.e '%c' %s %s %s %d | sed 's/null/0000/'",
		 DATE_TIME_DELIMITER,
		 aggregate_level_get_string( aggregate_level ),
		 begin_date,
		 end_date,
		 measurements_per_day );

	return pipe2list( sys_string );

} /* get_date_colon_time_slot_list() */

TRANSMIT_MEASUREMENT_SETS *transmit_measurement_sets_new( void )
{
	TRANSMIT_MEASUREMENT_SETS *transmit_measurement_sets;

	transmit_measurement_sets =
		calloc( 1, sizeof( TRANSMIT_MEASUREMENT_SETS ) );

	transmit_measurement_sets->expected_count_list = list_new_list();
	return transmit_measurement_sets;
}

void transmit_measurement_sets_populate_expected_count_list_datatype(
			LIST *expected_count_list,
			char *station,
			enum aggregate_level aggregate_level,
			char *begin_date,
			char *end_date )
{
	LIST *temp_list;
	char *count_datatype;
	char sys_string[ 1024 ];
	int count;
	char count_string[ 16 ];
	char datatype[ 128 ];
	char begin_measurement_date[ 16 ];
	char begin_measurement_time[ 16 ];
	EXPECTED_COUNT *expected_count = {0};

	if ( aggregate_level == real_time )
	{
		sprintf(	sys_string, 
				"expected_count_list.sh %s '^'", 
				station );
	}
	else
	if ( aggregate_level == half_hour )
	{
		/* Change the expected count to 48 */
		/* ------------------------------- */
		sprintf(	sys_string, 
				"expected_count_list.sh %s '^'	|"
				"sed 's/[0-9][0-9]*^/48^/'	|"
				"piece_inverse.e 2,3 '^'	|"
				"sort -u			 ",
				station );
	}
	else
	if ( aggregate_level == hourly )
	{
		/* Change the expected count to 24 */
		/* ------------------------------- */
		sprintf(	sys_string, 
				"expected_count_list.sh %s '^'	|"
				"sed 's/[0-9][0-9]*^/24^/'	|"
				"piece_inverse.e 2,3 '^'	|"
				"sort -u			 ",
				station );
	}
	else
	{
		/* Change the expected count to one */
		/* -------------------------------- */
		sprintf(	sys_string, 
				"expected_count_list.sh %s '^'	|"
				"sed 's/[0-9][0-9]*^/1^/'	|"
				"piece_inverse.e 2,3 '^'	|"
				"sort -u			 ",
				station );
	}

	temp_list = pipe2list( sys_string );

	if ( !list_rewind( temp_list ) ) return;

	/* If aggregating, then ignore the DATA_COLLECTION_FREQUENCY */
	/* --------------------------------------------------------- */
	*begin_measurement_date = '\0';
	*begin_measurement_time = '\0';

	do {
		/* Looks like: 24^rain^2002-12-19^0000 */
		/* ----------------------------------- */
		count_datatype = list_get_pointer( temp_list );

		piece( count_string, '^', count_datatype, 0 );
		piece( datatype, '^', count_datatype, 1 );

		if ( !piece( begin_measurement_date, '^', count_datatype, 2 )
		||   !*begin_measurement_date )
			strcpy( begin_measurement_date, "0000-00-00" );


		if ( !piece( begin_measurement_time, '^', count_datatype, 3 )
		||   !*begin_measurement_time )
			strcpy( begin_measurement_time, "0000" );

		count = atoi( count_string );
		expected_count = get_or_set_expected_count(
					expected_count_list,
					count,
					begin_date,
					end_date,
					aggregate_level );

		if ( !expected_count )
		{
			fprintf(stderr,
			"ERROR in %s/%s(): invalid expected_count_list\n",
				__FILE__,
				__FUNCTION__ );
			exit( 1 );
		}

		expected_count_append_datatype(
					expected_count->datatype_list,
					strdup( datatype ),
					strdup( begin_measurement_date ),
					strdup( begin_measurement_time ) );

	} while( list_next( temp_list ) );
	list_free_string_list( temp_list );
} /* transmit_measurement_sets_populate_expected_count_list_datatype() */

EXPECTED_COUNT *get_or_set_expected_count(
					LIST *expected_count_list,
					int count,
					char *begin_date,
					char *end_date,
					enum aggregate_level aggregate_level )
{
	EXPECTED_COUNT *expected_count;

	if ( !list_rewind( expected_count_list ) )
	{
		expected_count =
			expected_count_new(
				count,
				begin_date,
				end_date,
				aggregate_level );
		list_append_pointer( expected_count_list, expected_count );
		return expected_count;
	}

	do {
		expected_count = list_get_pointer( expected_count_list );

		if ( expected_count->expected_count == count )
			return expected_count;

	} while( list_next( expected_count_list ) ) ;

	expected_count =
		expected_count_new(
				count,
				begin_date,
				end_date,
				aggregate_level );
	list_append_pointer( expected_count_list, expected_count );
	return expected_count;

} /* get_or_set_expected_count() */

void transmit_measurement_sets_populate_measurement_hash_table(
				LIST *expected_count_list,
				char *station,
				char *begin_date,
				char *end_date,
				enum aggregate_level aggregate_level,
				char *order_clause,
				int date_piece,
				int time_piece,
				int measurement_value_piece,
				enum validation_level validation_level,
				char *application_name )
{
	LIST *datatype_name_list;
	char *datatype_name;
	char *sys_string;
	LIST *measurement_record_list;
	char *measurement_record;

	datatype_name_list = get_datatype_name_list( expected_count_list );

	if ( !datatype_name_list || !list_rewind( datatype_name_list ) ) return;

	do {
		datatype_name = list_get_pointer( datatype_name_list );

		sys_string = get_measurement_record_list_sys_string(	
				     application_name,
				     station,
				     datatype_name,
				     begin_date,
				     end_date,
				     aggregate_level,
				     date_piece,
				     time_piece,
				     measurement_value_piece,
				     order_clause,
				     validation_level );

/*
fprintf( stderr, "%s/%s()/%d: for datatype = (%s), got sys_string = (%s)\n", 
__FILE__,
__FUNCTION__,
__LINE__,
datatype_name,
sys_string );
*/

		measurement_record_list = pipe2list( sys_string );
		free( sys_string );

		if ( !list_rewind( measurement_record_list ) )
			continue;

		do {
			measurement_record =
				list_get_pointer(
					measurement_record_list );

			datatype_set_measurement_record(
				expected_count_list,
				measurement_record,
				date_piece,
				time_piece,
				measurement_value_piece,
				datatype_name );

		} while( list_next( measurement_record_list ) );

		list_free_string_list( measurement_record_list );

	} while( list_next( datatype_name_list ) );

} /* transmit_measurement_sets_populate_measurement_hash_table() */

LIST *get_datatype_name_list( LIST *expected_count_list )
{
	EXPECTED_COUNT *expected_count;
	LIST *datatype_name_list;
	DATATYPE *datatype;

	if ( !expected_count_list || !list_rewind( expected_count_list ) )
		return (LIST *)0;

	datatype_name_list = list_new_list();
	do {
		expected_count = list_get_pointer( expected_count_list );
		if ( !list_rewind( expected_count->datatype_list ) )
			continue;

		do {
			datatype =
				list_get_pointer(
					expected_count->datatype_list );
			list_append_unique_string(
					datatype_name_list,
					datatype->datatype );
		} while( list_next( expected_count->datatype_list ) );
	} while( list_next( expected_count_list ) );
	return datatype_name_list;
} /* get_datatype_name_list() */

HASH_TABLE *get_measurement_hash_table(
				LIST *expected_count_list,
				char *datatype_name,
				JULIAN *measurement_julian )
{
	LIST *datatype_expected_count_list;
	EXPECTED_COUNT *expected_count;
	DATATYPE *datatype;

	datatype_expected_count_list =
		get_datatype_expected_count_list(
			expected_count_list,
			datatype_name );

	if ( !datatype_expected_count_list
	||   !list_go_tail( datatype_expected_count_list ) )
	{
		return (HASH_TABLE *)0;
	}

	do {
		expected_count =
			list_get_pointer(
				datatype_expected_count_list );

		if ( !list_go_tail( expected_count->datatype_list ) )
		{
			return (HASH_TABLE *)0;
		}

		do {
			datatype =
				list_get_pointer(
					expected_count->datatype_list );

			if ( strcmp(	datatype_name,
					datatype->datatype ) == 0
			&&   measurement_julian->current >=
			     datatype->begin_measurement_julian->current )
			{
				list_free_container(
					datatype_expected_count_list );
				return datatype->measurement_hash_table;
			}
		} while( list_previous( expected_count->datatype_list ) );
	} while( list_previous( datatype_expected_count_list ) );

	list_free_container( datatype_expected_count_list );
	return (HASH_TABLE *)0;

} /* get_measurement_hash_table() */

LIST *get_datatype_expected_count_list(
			LIST *expected_count_list,
			char *datatype_name )
{
	EXPECTED_COUNT *expected_count;
	DATATYPE *datatype;
	LIST *datatype_list;
	LIST *datatype_expected_count_list;

	if ( !list_rewind( expected_count_list ) ) return (LIST *)0;

	datatype_expected_count_list = list_new_list();
	do {
		expected_count = list_get_pointer( expected_count_list );

		datatype_list = expected_count->datatype_list;
		if ( !list_rewind( datatype_list ) ) continue;

		do {
			datatype = list_get_pointer( datatype_list );
			if ( strcmp( datatype->datatype, datatype_name ) == 0 )
			{
				list_append_pointer(
					datatype_expected_count_list,
					expected_count );
				break;
			}
		} while( list_next( datatype_list ) );

	} while( list_next( expected_count_list ) ) ;

	return datatype_expected_count_list;
} /* get_datatype_expected_count_list() */

char *get_justify_column_list_string(	char *heading_string,
					enum aggregate_level aggregate_level )
{
	int comma_count;
	static char string[ 1024 ];
	char *ptr = string;

	comma_count = character_count( ',', heading_string );

	if ( aggregate_level == real_time
	||   aggregate_level == aggregate_level_none
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		ptr += sprintf( ptr, "left,left,left,right" );
		comma_count -= 3;
	}
	else
	{
		ptr += sprintf( ptr, "left,left" );
		comma_count -= 1;
	}

	if ( comma_count > 0 )
	{
		while( comma_count-- )
		{
			ptr += sprintf( ptr, ",right" );
		}
	}
	return string;
} /* get_justify_column_list_string() */

char *expected_count_list_get_display( LIST *expected_count_list )
{
	EXPECTED_COUNT *expected_count;
	char return_string[ 65536 ];
	char *ptr = return_string;

	if ( !list_rewind( expected_count_list ) ) return "";

	do {
		expected_count = list_get_pointer( expected_count_list );

		ptr += sprintf(	ptr,
				"\nExpected count: %d\n",
				expected_count->expected_count );

		ptr += sprintf( ptr,
				"Datatype list: %s\n",
				local_datatype_list_display(
					expected_count->datatype_list ) );

		ptr += sprintf( ptr, "\n" );

	} while( list_next( expected_count_list ) );

	return strdup( return_string );

} /* expected_count_list_get_display() */

char *local_datatype_list_display( LIST *datatype_list )
{
	DATATYPE *datatype;
	static char return_string[ 4096 ];
	char *ptr = return_string;

	if ( !list_rewind( datatype_list ) ) return "";

	do {
		datatype = list_get_pointer( datatype_list );

		ptr += sprintf( ptr,
				"\nDatatype: %s\n",
				datatype->datatype );
		ptr += sprintf( ptr,
				"Begin measurement date: %s\n",
				datatype->begin_measurement_date );
		ptr += sprintf( ptr,
				"Begin measurement time: %s\n",
				datatype->begin_measurement_time );
		ptr += sprintf( ptr,
				"Julian representation: %s\n",
				julian_display_yyyy_mm_dd_hhmm(
					datatype->
						begin_measurement_julian->
						current ) );
	} while( list_next( datatype_list ) );

	return return_string;

} /* local_datatype_list_display() */

char *date_colon_time_slot_list_display(
				LIST *date_colon_time_slot_list )
{
date_colon_time_slot_list = (LIST *)0;
	return "";
} /* date_colon_time_slot_list_display() */

MEASUREMENT_OUTPUT *measurement_output_new(
				char *date_colon_time,
				double measurement_value )
{
	MEASUREMENT_OUTPUT *m;

	if ( ! ( m = (MEASUREMENT_OUTPUT *)
			calloc( 1,
				sizeof( MEASUREMENT_OUTPUT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->date_colon_time = date_colon_time;
	m->measurement_value = measurement_value;

	return m;

} /* measurement_output_new() */

