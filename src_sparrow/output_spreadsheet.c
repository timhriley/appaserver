/* ---------------------------------------------------	*/
/* src_sparrow/output_spreadsheet.c			*/
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
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define DELIMITER		','

/* Prototypes */
/* ---------- */
char *get_site_visit_comments(	char *observer_code,
				char *visit_time,
				char *comments,
				char *reason_no_observations );

DICTIONARY *load_physical_observation_dictionary(
				char *parameter_where );

DICTIONARY *load_vegetation_dictionary(
				char *parameter_where );

DICTIONARY *load_site_visit_dictionary(
				char *parameter_where );

DICTIONARY *load_bird_count_dictionary(
				char *parameter_where );

char *get_physical_observation(	char *site_visit_key,
				DICTIONARY *physical_observation_dictionary,
				char *physical_parameter );

char *get_vegetation(		char *key,
				char *parameter_where );

char *get_site_visit(		char *key,
				char *parameter_where );

char *get_bird_count(		char *key,
				char *parameter_where );

char *get_site_visit_key(	char *quad_sheet,
				char *site_number,
				char *visit_date );

char *get_physical_observation_key(
				char *site_visit_key,
				char *quad_sheet,
				char *site_number,
				char *visit_date,
				char *physical_parameter,
				char *measurement_number );

DICTIONARY *get_bird_count_dictionary(
				char *parameter_where );

void output_spreadsheet_sys_string(
				char *output_filename,
				char *sys_string,
				char *parameter_where );

char *get_sys_string(		char *parameter_where );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				char *parameter_where );

void output_spreadsheet(
				char *application_name,
				char *appaserver_mount_point,
				int process_id,
				char *process_name,
				char *parameter_where );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *parameter_where;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr, 
"Usage: %s ignored process_name where\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	parameter_where = argv[ 3 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	output_spreadsheet(	application_name,
				appaserver_parameter_file->document_root,
				getpid(),
				process_name,
				parameter_where );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
}

void output_spreadsheet(
				char *application_name,
				char *document_root_directory,
				int process_id,
				char *process_name,
				char *parameter_where )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	char title[ 128 ];
	char sub_title[ 65536 ];
	char *sys_string;
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


	get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			parameter_where );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

/*
	sprintf(output_filename, 
	 	OUTPUT_TEMPLATE,
	 	appaserver_mount_point,
	 	application_name,
		process_name,
	 	process_id );
*/

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file,
"Site,Quad,SiteNumber,SubPopulation,Year,Survey,Date,Site.EastingNad83,Site.NorthingNad83,Site.Latitude,Site.Longitude,BC,Vegetation,VC,WD1,WD2,WD3,WD4,WD5,WD6,SD1,SD2,SD3,SD4,SD5,SD6,Comments\n" );


	fclose( output_file );

	sys_string = get_sys_string( parameter_where );

	output_spreadsheet_sys_string(
		output_filename,
		sys_string,
		parameter_where );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

}


void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			char *parameter_where )
{
	format_initial_capital( title, process_name );
	strcpy( sub_title, parameter_where );

	search_replace_word( 	sub_title,
				"1 = 1 and ", 
				"" );

	format_initial_capital( sub_title, sub_title );

}

char *get_sys_string(	char *parameter_where )
{
	static char sys_string[ 4096 ];
	char where_clause[ 2048 ];
	char join_where[ 2048 ];
	char *select;
	char *from;
	char utm_convert_process[ 128 ];

	sprintf( join_where,
	"site_visit.quad_sheet = observation_site.quad_sheet and	"
	"site_visit.site_number = observation_site.site_number and	"
	"site_visit.quad_sheet = quad_sheet.quad_sheet			" );

	sprintf( where_clause, "%s and %s", parameter_where, join_where );

	select =
"site_visit.quad_sheet, site_id, quad_sheet_code, site_visit.site_number, sub_population, survey_year, survey_number, site_visit.visit_date, observation_site.longitude, observation_site.latitude, observation_site.latitude, observation_site.longitude, '', '', total_vegetation_cover_percent";


	from = "site_visit, observation_site, quad_sheet";

	/* Also, use sed to make UTMs an integer. */
	/* -------------------------------------- */
	sprintf( utm_convert_process,
		 "location_convert.e WGS84_D.decimal UTM_NAD83 8 9 '%c'	|"
		 "sed 's/\\.[0-9][0-9]*,/,/'				|"
		 "sed 's/\\.[0-9][0-9]*,/,/'				 ",
		 DELIMITER );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\"	|"
		 "sql.e '%c'						|"
		 "%s							|"
		 "cat 							 ",
		 select,
		 from,
		 where_clause,
		 select,
		 DELIMITER,
		 utm_convert_process );

	return sys_string;

}

void output_spreadsheet_sys_string(
		char *output_filename,
		char *sys_string,
		char *parameter_where )
{
	char output_sys_string[ 256 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char quad_sheet[ 32 ];
	char site_number[ 32 ];
	char visit_date[ 32 ];
	char visit_date_american[ 32 ];
	char latitude[ 32 ];
	char longitude[ 32 ];
	char *latitude_minutes;
	char *longitude_minutes;
	char *site_visit_key;
	DICTIONARY *physical_observation_dictionary;

	physical_observation_dictionary =
		load_physical_observation_dictionary(
			parameter_where );

	input_pipe = popen( sys_string, "r" );

	sprintf( output_sys_string,
		 "cat >> %s",
		 output_filename );

	output_pipe = popen( output_sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( quad_sheet, DELIMITER, input_buffer, 0 );
		piece( site_number, DELIMITER, input_buffer, 3 );
		piece( visit_date, DELIMITER, input_buffer, 7 );

		*visit_date_american = '\0';
		date_convert_source_international(
				visit_date_american,
				american /* destination_format */,
				visit_date );

		if ( *visit_date_american )
		{
			piece_replace(	input_buffer,
					DELIMITER,
					visit_date_american,
					7 );
		}

		piece( latitude, DELIMITER, input_buffer, 10 );

		if ( ( latitude_minutes =
			timlib_latitude_longitude_degrees_decimal_to_minutes(
					latitude ) )
		&&     *latitude_minutes )
		{
			piece_replace(	input_buffer,
					DELIMITER,
					latitude_minutes,
					10 );
		}

		piece( longitude, DELIMITER, input_buffer, 11 );

		if ( ( longitude_minutes =
			timlib_latitude_longitude_degrees_decimal_to_minutes(
					longitude ) )
		&&     *longitude_minutes )
		{
			piece_replace(	input_buffer,
					DELIMITER,
					longitude_minutes,
					11 );
		}

		site_visit_key =
			get_site_visit_key(
				quad_sheet,
				site_number,
				visit_date );

		/* Remove quad_sheet */
		/* ----------------- */
		piece_inverse( input_buffer, DELIMITER, 0 );

		/* Insert bird_count */
		/* ----------------- */
		piece_replace(	input_buffer,
				DELIMITER,
				get_bird_count(
					site_visit_key,
					parameter_where ),
				11 );

		/* Insert vegetation */
		/* ----------------- */
		piece_replace(	input_buffer,
				DELIMITER,
				get_vegetation(
					site_visit_key,
					parameter_where ),
				12 );

		/* Append water depth */
		/* ------------------ */
		strcat(	input_buffer,
			get_physical_observation(
				site_visit_key,
				physical_observation_dictionary,
				"water_depth" /* physical_parameter */ ) );

		/* Append soil depth */
		/* ----------------- */
		strcat(	input_buffer,
			get_physical_observation(
				site_visit_key,
				physical_observation_dictionary,
				"soil_depth" /* physical_parameter */ ) );

		/* ------------------------------------- */
		/* Append observer, visit_time,		 */
		/* reason_no_observations and comment.	 */
		/* ------------------------------------- */
		strcat(	input_buffer,
			get_site_visit(
				site_visit_key,
				parameter_where ) );

		/* Output */
		/* ------ */
		fprintf( output_pipe, "%s\n", input_buffer );

		free( site_visit_key );
	}

	pclose( output_pipe );
	pclose( input_pipe );

}

char *get_site_visit_key(	char *quad_sheet,
				char *site_number,
				char *visit_date )
{
	char key[ 128 ];

	sprintf( key,
		 "%s^%s^%s",
		 quad_sheet,
		 site_number,
		 visit_date );

	return strdup( key );

}

char *get_physical_observation_key(
				char *site_visit_key,
				char *quad_sheet,
				char *site_number,
				char *visit_date,
				char *physical_parameter,
				char *measurement_number )
{
	static char key[ 128 ];

	if ( !site_visit_key )
	{
		sprintf( key,
		 	"%s^%s^%s^%s^%s",
		 	quad_sheet,
		 	site_number,
		 	visit_date,
		 	physical_parameter,
		 	measurement_number );
	}
	else
	{
		sprintf( key,
			 "%s^%s^%s",
			 site_visit_key,
			 physical_parameter,
			 measurement_number );
	}

	return key;

}

char *get_bird_count(	char *key,
			char *parameter_where )
{
	static DICTIONARY *bird_count_dictionary = {0};
	char *data;

	if ( !bird_count_dictionary )
	{
		bird_count_dictionary =
			load_bird_count_dictionary(
				parameter_where );
	}

	if ( ! ( data = dictionary_fetch( key, bird_count_dictionary ) ) )
		return "";
	else
		return data;

}

DICTIONARY *load_bird_count_dictionary( char *parameter_where )
{
	DICTIONARY *bird_count_dictionary;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char where_clause[ 512 ];
	char input_buffer[ 1024 ];
	char quad_sheet[ 32 ];
	char site_number[ 32 ];
	char visit_date[ 32 ];
	char bird_count[ 32 ];
	char *select;
	char *from;
	char *group;
	char *key;

	bird_count_dictionary = dictionary_medium_new();

	select =
"site_visit.quad_sheet, site_visit.site_number, site_visit.visit_date, sum( bird_count )";
	group =
"site_visit.quad_sheet, site_visit.site_number, site_visit.visit_date";

	from = "site_visit, sparrow_observation";

	sprintf( where_clause,
	"%s and								"
	"sparrow_observation.quad_sheet = site_visit.quad_sheet and	"
	"sparrow_observation.site_number = site_visit.site_number and	"
	"sparrow_observation.visit_date = site_visit.visit_date		",
		 parameter_where );


	sprintf( sys_string,
		 "echo \"select %s from %s where %s group by %s;\" | sql '%c'",
		 select,
		 from,
		 where_clause,
		 group,
		 DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( quad_sheet, DELIMITER, input_buffer, 0 );
		piece( site_number, DELIMITER, input_buffer, 1 );
		piece( visit_date, DELIMITER, input_buffer, 2 );
		piece( bird_count, DELIMITER, input_buffer, 3 );

		key = get_site_visit_key( quad_sheet, site_number, visit_date );

		dictionary_set_pointer(
			bird_count_dictionary,
			key,
			strdup( bird_count ) );
	}

	pclose( input_pipe );

	return bird_count_dictionary;

}

char *get_site_visit(	char *key,
			char *parameter_where )
{
	static DICTIONARY *site_visit_dictionary = {0};
	char *data;

	if ( !site_visit_dictionary )
	{
		site_visit_dictionary =
			load_site_visit_dictionary(
				parameter_where );
	}

	if ( ! ( data = dictionary_fetch( key, site_visit_dictionary ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot dictionary_fetch(%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key );
		exit( 1 );
	}

	return data;

}

char *get_vegetation(	char *key,
			char *parameter_where )
{
	static DICTIONARY *vegetation_dictionary = {0};
	char *data;

	if ( !vegetation_dictionary )
	{
		vegetation_dictionary =
			load_vegetation_dictionary(
				parameter_where );
	}

	if ( ! ( data = dictionary_fetch( key, vegetation_dictionary ) ) )
		return "";
	else
		return data;

}

DICTIONARY *load_vegetation_dictionary( char *parameter_where )
{
	DICTIONARY *vegetation_dictionary;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char where_clause[ 512 ];
	char input_buffer[ 1024 ];
	char quad_sheet[ 32 ];
	char site_number[ 32 ];
	char visit_date[ 32 ];
	char vegetation_name[ 32 ];
	char vegetation_code[ 32 ];
	char *select;
	char *from;
	char *order_by;
	char *key;
	char *data;
	char new_data[ 128 ];

	vegetation_dictionary = dictionary_medium_new();

	select =
"site_visit.quad_sheet, site_visit.site_number, site_visit.visit_date, vegetation_observation.vegetation_name, vegetation_code";

	from = "site_visit, vegetation_observation, vegetation";

	sprintf( where_clause,
"%s and									"
"vegetation_observation.quad_sheet = site_visit.quad_sheet and		"
"vegetation_observation.site_number = site_visit.site_number and	"
"vegetation_observation.visit_date = site_visit.visit_date and		"
"vegetation_observation.vegetation_name = vegetation.vegetation_name	",
		 parameter_where );

	order_by = "predominance_rank";

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql '%c'",
		 select,
		 from,
		 where_clause,
		 order_by,
		 DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( quad_sheet, DELIMITER, input_buffer, 0 );
		piece( site_number, DELIMITER, input_buffer, 1 );
		piece( visit_date, DELIMITER, input_buffer, 2 );
		piece( vegetation_name, DELIMITER, input_buffer, 3 );
		piece( vegetation_code, DELIMITER, input_buffer, 4 );

		if ( !*vegetation_code )
		{
			strcpy( vegetation_code, vegetation_name );
		}

		key = get_site_visit_key( quad_sheet, site_number, visit_date );

		if ( !( data =
			dictionary_fetch( key, vegetation_dictionary ) ) )
		{
			dictionary_set_pointer(
				vegetation_dictionary,
				key,
				strdup( vegetation_code ) );
		}
		else
		{
			sprintf( new_data, "%s+%s", data, vegetation_code );

			dictionary_remove_key( vegetation_dictionary, key );

			dictionary_set_pointer(
				vegetation_dictionary,
				key,
				strdup( new_data ) );

			free( data );
		}
	}

	pclose( input_pipe );

	return vegetation_dictionary;

}

char *get_physical_observation(
			char *site_visit_key,
			DICTIONARY *physical_observation_dictionary,
			char *physical_parameter )
{
	char *data;
	int measurement_number; 
	char *physical_observation_key;
	char measurement_number_string[ 8 ];
	static char physical_observation[ 128 ];
	char *ptr = physical_observation;

	for (	measurement_number = 1;
		measurement_number <= 6;
		measurement_number++ )
	{
		sprintf(	measurement_number_string,
				"%d",
				measurement_number );

		physical_observation_key =
			get_physical_observation_key(
				site_visit_key,
				(char *)0 /* quad_sheet */,
				(char *)0 /* site_number */,
				(char *)0 /* visit_date */,
				physical_parameter,
				measurement_number_string );

		ptr += sprintf( ptr, "," );

		if ( ( data =
			dictionary_fetch(
				physical_observation_key,
				physical_observation_dictionary ) ) )
		{
			ptr += sprintf( ptr, "%s", data );
		}
	}

	return physical_observation;

}

DICTIONARY *load_physical_observation_dictionary( char *parameter_where )
{
	DICTIONARY *physical_observation_dictionary;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char where_clause[ 512 ];
	char input_buffer[ 1024 ];
	char quad_sheet[ 32 ];
	char site_number[ 32 ];
	char visit_date[ 32 ];
	char physical_parameter[ 32 ];
	char measurement_number[ 32 ];
	char measurement_value[ 32 ];
	char *select;
	char *from;
	char *physical_observation_key;

	physical_observation_dictionary = dictionary_large();

	select =
"site_visit.quad_sheet, site_visit.site_number, site_visit.visit_date, physical_parameter, measurement_number, measurement_value";

	from = "site_visit, physical_observation";

	sprintf( where_clause,
"%s and									"
"physical_observation.quad_sheet = site_visit.quad_sheet and		"
"physical_observation.site_number = site_visit.site_number and	"
"physical_observation.visit_date = site_visit.visit_date		",
		 parameter_where );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql '%c'",
		 select,
		 from,
		 where_clause,
		 DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( quad_sheet, DELIMITER, input_buffer, 0 );
		piece( site_number, DELIMITER, input_buffer, 1 );
		piece( visit_date, DELIMITER, input_buffer, 2 );
		piece( physical_parameter, DELIMITER, input_buffer, 3 );
		piece( measurement_number, DELIMITER, input_buffer, 4 );
		piece( measurement_value, DELIMITER, input_buffer, 5 );

		physical_observation_key =
			get_physical_observation_key(
				(char *)0 /* site_visit_key */,
				quad_sheet,
				site_number,
				visit_date,
				physical_parameter,
				measurement_number );

		dictionary_set_pointer(
			physical_observation_dictionary,
			strdup( physical_observation_key ),
			strdup( measurement_value ) );
	}

	pclose( input_pipe );

	return physical_observation_dictionary;

}

DICTIONARY *load_site_visit_dictionary( char *parameter_where )
{
	DICTIONARY *site_visit_dictionary;
	char sys_string[ 2048 ];
	FILE *input_pipe;
	char *left_join;
	char input_buffer[ 1024 ];
	char quad_sheet[ 32 ];
	char site_number[ 32 ];
	char visit_date[ 32 ];
	char observer[ 32 ];
	char observer_code[ 32 ];
	char visit_time[ 8 ];
	char reason_no_observations[ 32 ];
	char comments[ 2048 ];
	char *select;
	char *from;
	char *key;
	char *site_visit_comments;

	site_visit_dictionary = dictionary_medium_new();

	select =
"site_visit.quad_sheet, site_visit.site_number, site_visit.visit_date, site_visit.observer, observer_code, visit_time, reason_no_observations, comments";

	from = "site_visit";

	left_join =
	"left join observer on site_visit.observer = observer.observer";

	sprintf( sys_string,
		 "echo \"select %s from %s %s where %s;\" | sql.e",
		 select,
		 from,
		 left_join,
		 parameter_where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( quad_sheet, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( site_number, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		piece( visit_date, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		piece( observer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		piece( observer_code, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		piece( visit_time, FOLDER_DATA_DELIMITER, input_buffer, 5 );

		piece(	reason_no_observations,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );

		piece( comments, FOLDER_DATA_DELIMITER, input_buffer, 7 );

		key = get_site_visit_key( quad_sheet, site_number, visit_date );

		if ( !*observer_code )
		{
			strcpy( observer_code, observer );
		}

		site_visit_comments =
			get_site_visit_comments(
				observer_code,
				visit_time,
				comments,
				reason_no_observations );

		dictionary_set_pointer(
			site_visit_dictionary,
			key,
			strdup( site_visit_comments ) );
	}

	pclose( input_pipe );

	return site_visit_dictionary;

}

char *get_site_visit_comments(	char *observer_code,
				char *visit_time,
				char *comments,
				char *reason_no_observations )
{
	static char site_visit_data[ 4096 ];
	char visit_time_colon[ 6 ] = {0};
	char *ptr = site_visit_data;

	if ( strlen( visit_time ) == 3 )
	{
		sprintf( visit_time_colon,
			 "0%.1s:%s",
			 visit_time,
			 visit_time + 1 );
	}
	else
	if ( strlen( visit_time ) == 4 )
	{
		sprintf( visit_time_colon,
			 "%.2s:%s",
			 visit_time,
			 visit_time + 2 );
	}

	ptr += sprintf( ptr, "," );
	ptr += sprintf( ptr, "%c", '"' );

	if ( reason_no_observations && *reason_no_observations )
	{
		ptr += sprintf( ptr,
				"%s %s (%s) %s",
				observer_code,
				visit_time_colon,
				reason_no_observations,
				comments );
	}
	else
	{
		ptr += sprintf( ptr,
				"%s %s %s",
				observer_code,
				visit_time_colon,
				comments );
	}

	ptr += sprintf( ptr, "%c", '"' );

	return site_visit_data;

}

