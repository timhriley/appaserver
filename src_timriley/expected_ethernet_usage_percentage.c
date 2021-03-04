/* ---------------------------------------------------	*/
/* src_timriley/expected_ethernet_usage_percentage.c	*/
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
#include "appaserver_parameter_file.h"
#include "application.h"
#include "environ.h"
#include "date.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
/* #define TEST_COMPUTER 1 */
#define MEGABYTES_PER_TERABYTE	1000000.0

/* Prototypes */
/* ---------- */
char *get_current_month_year(	void );

int get_current_day(		void );

FILE *get_output_pipe(		char *output_medium,
				char *process_name );

double get_expected_percentage(	int current_megabytes,
				char *date_string,
				char *current_month_year,
				int current_day );

void parse_input_buffer(
				char *date_string,
				char *ethernet_address,
				char *receive_megabytes_string,
				char *transmit_megabytes_string,
				char *input_buffer );

FILE *get_input_pipe(		char *application_name );

void expected_ethernet_usage_percentage(
				FILE *output_pipe,
				char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *output_medium;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FILE *output_pipe;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s ignored process_name output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	output_medium=argv[ 3 ];

#ifndef TEST_COMPUTER
	add_relative_source_directory_to_path( application_name );
#endif
	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document = document_new( "", application_name );
/*
		document_set_output_content_type( document );
*/
	
#ifndef TEST_COMPUTER
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
#endif

	}

	expected_ethernet_usage_percentage(
			( output_pipe =
				get_output_pipe(
					output_medium,
					process_name ) ),
			application_name );

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

#ifndef TEST_COMPUTER
	process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );
#endif

	pclose( output_pipe );

	return 0;
} /* main() */

FILE *get_output_pipe(	char *output_medium,
			char *process_name )
{
	char sys_string[ 1024 ];

	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		sprintf( sys_string, "cat" );
	}
	else
	{
		char *heading;
		char *alignment;
		char buffer[ 128 ];

		heading =
"Year-Month,IP Address,Inbound Meg.,Outbound Meg.,Inbound Percentage,Outbound Percentage";

		alignment = "left,left,right";

		sprintf(	sys_string,
		"html_table.e '%s^Percentage of 1 Terabyte' '%s' '^' '%s'",
				format_initial_capital( buffer, process_name ),
				heading,
				alignment );
	}

	return popen( sys_string, "w" );

} /* get_output_pipe() */

FILE *get_input_pipe( char *application_name )
{
	FILE *input_pipe;

#ifdef TEST_COMPUTER
	input_pipe = popen( "cat snapshot.dat", "r" );
#else
	char sys_string[ 1024 ];
	char *table_name;

	table_name =
		get_table_name(
			application_name,
			"ethernet_device_snapshot" );

	sprintf( sys_string,
"echo \"select	substr( date, 1, 7 ) month,				 "
"		ip_address,						 "
"		sum( receive_daily_megabytes ),				 "
"		sum( transmit_daily_megabytes )				 "
"from %s								 "
"group by month, ip_address						 "
"order by month desc;\"							|"
"sql.e '^'								 ",
		 table_name );

	input_pipe = popen( sys_string, "r" );
#endif
	return input_pipe;
} /* get_input_pipe() */

void parse_input_buffer(
			char *date_string,
			char *ethernet_address,
			char *receive_megabytes_string,
			char *transmit_megabytes_string,
			char *input_buffer )
{
	piece( date_string, '^', input_buffer, 0 );
	piece( ethernet_address, '^', input_buffer, 1 );
	piece( receive_megabytes_string, '^', input_buffer, 2 );
	piece( transmit_megabytes_string, '^', input_buffer, 3 );

} /* parse_input_buffer() */

void expected_ethernet_usage_percentage(
			FILE *output_pipe,
			char *application_name )
{
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char date_string[ 128 ];
	char ethernet_address[ 128 ];
	char receive_megabytes_string[ 128 ];
	char transmit_megabytes_string[ 128 ];
	char receive_megabytes_commas_string[ 128 ];
	char transmit_megabytes_commas_string[ 128 ];
	double expected_receive_percentage;
	double expected_transmit_percentage;
	char *current_month_year;
	int current_day;

	current_month_year = get_current_month_year();
	current_day = get_current_day();

	input_pipe = get_input_pipe( application_name );

	while( get_line( input_buffer, input_pipe ) )
	{
		parse_input_buffer(
			date_string,
			ethernet_address,
			receive_megabytes_string,
			transmit_megabytes_string,
			input_buffer );

		expected_receive_percentage =
			get_expected_percentage(
				atoi( receive_megabytes_string ),
				date_string,
				current_month_year,
				current_day );

		expected_transmit_percentage =
			get_expected_percentage(
				atoi( transmit_megabytes_string ),
				date_string,
				current_month_year,
				current_day );

		strcpy(	receive_megabytes_commas_string,
			place_commas_in_number_string(
				receive_megabytes_string ) );

		strcpy(	transmit_megabytes_commas_string,
			place_commas_in_number_string(
				transmit_megabytes_string ) );

		fprintf( output_pipe,
			 "%s^%s^%s^%s^%.2lf^%.2lf\n",
			 date_string,
			 ethernet_address,
			 receive_megabytes_commas_string,
			 transmit_megabytes_commas_string,
			 expected_receive_percentage,
			 expected_transmit_percentage );
	}

	pclose( input_pipe );

} /* expected_ethernet_usage_percentage() */

char *get_current_month_year( void )
{
	char sys_string[ 128 ];

	sprintf( sys_string,
		 "date.e 0 | piece.e '-' 0,1" );

	return pipe2string( sys_string );
} /* get_current_month_year() */

int get_current_day( void )
{
	char sys_string[ 128 ];

	sprintf( sys_string,
		 "date.e 0 | piece.e '-' 2" );

	return atoi( pipe2string( sys_string ) );
} /* get_current_day() */

double get_expected_percentage(	int current_megabytes,
				char *date_string,
				char *current_month_year,
				int current_day )
{
	double expected_percentage = 0.0;
	int expected_month_consumed = 0;
	double month_ratio;

	if ( strcmp( date_string, current_month_year ) != 0 )
	{
		expected_month_consumed = current_megabytes;
	}
	else
	{
		int days_in_month;
		char current_year_string[ 16 ];
		char current_month_string[ 16 ];

		piece(	current_year_string,
			'-',
			current_month_year,
			0 );

		piece(	current_month_string,
			'-',
			current_month_year,
			1 );

		days_in_month =
			date_days_in_month(
				atoi( current_month_string ),
				atoi( current_year_string ) );

		month_ratio =	(double)current_day /
				(double)days_in_month;

		expected_month_consumed =
			(double)current_megabytes / month_ratio;
	}

	expected_percentage =
		(	(double)expected_month_consumed /
			MEGABYTES_PER_TERABYTE ) * 100.0;

	return expected_percentage;
} /* get_expected_percentage() */

