/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/ethernet_usage_percent.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "application.h"
#include "environ.h"
#include "date.h"

#define MEGABYTES_PER_TERABYTE	1000000.0

char *current_month_year(
		void );

int current_day(
		void );

FILE *output_pipe(
		char *output_medium );

double expected_percent(
		int current_megabytes,
		char *date_string,
		char *current_month_year,
		int current_day );

void parse_input_buffer(
		char *date_string,
		char *ethernet_address,
		char *receive_megabytes_string,
		char *transmit_megabytes_string,
		char *input_buffer );

FILE *input_pipe(
		void );

void ethernet_usage_percent(
		FILE *output_pipe );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *output_medium;
	FILE *pipe;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf( stderr, 
			 "Usage: %s process_name output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	output_medium=argv[ 2 ];

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name );
	}

	ethernet_usage_percent(
		( pipe =
			output_pipe( output_medium ) ) );

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

	pclose( pipe );

	return 0;
}

FILE *output_pipe( char *output_medium )
{
	char system_string[ 1024 ];

	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		sprintf( system_string, "cat" );
	}
	else
	{
		char *heading;
		char *alignment;

		heading =
"Year-Month,IP Address,Inbound Meg.,Outbound Meg.,Inbound Percent,Outbound Percent";

		alignment = "left,left,right";

		sprintf(system_string,
		"html_table.e '^Percentage of 1 Terabyte' '%s' '^' '%s'",
			heading,
			alignment );
	}

	return popen( system_string, "w" );
}

FILE *input_pipe( void )
{
	char system_string[ 1024 ];

	sprintf(system_string,
"echo \"select	substr( date, 1, 7 ) month,				 "
"		ip_address,						 "
"		sum( receive_daily_megabytes ),				 "
"		sum( transmit_daily_megabytes )				 "
"from %s								 "
"group by month, ip_address						 "
"order by month desc;\"							|"
"sql.e '^'								 ",
		 "ethernet_device_snapshot" );

	return popen( system_string, "r" );
}

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
}

void ethernet_usage_percent( FILE *output_pipe )
{
	FILE *pipe;
	char input_buffer[ 1024 ];
	char date_string[ 128 ];
	char ethernet_address[ 128 ];
	char receive_megabytes_string[ 128 ];
	char transmit_megabytes_string[ 128 ];
	char receive_megabytes_commas_string[ 128 ];
	char transmit_megabytes_commas_string[ 128 ];
	double expected_receive_percent;
	double expected_transmit_percent;
	char *month_year;
	int day;

	month_year = current_month_year();
	day = current_day();

	pipe = input_pipe();

	while( string_input( input_buffer, pipe, 1024 ) )
	{
		parse_input_buffer(
			date_string,
			ethernet_address,
			receive_megabytes_string,
			transmit_megabytes_string,
			input_buffer );

		expected_receive_percent =
			expected_percent(
				atoi( receive_megabytes_string ),
				date_string,
				month_year,
				day );

		expected_transmit_percent =
			expected_percent(
				atoi( transmit_megabytes_string ),
				date_string,
				month_year,
				day );

		strcpy(	receive_megabytes_commas_string,
			/* --------------------------- */
			/* Returns static memory or "" */
			/* --------------------------- */
			string_commas_number_string(
				receive_megabytes_string ) );

		strcpy(	transmit_megabytes_commas_string,
			/* --------------------------- */
			/* Returns static memory or "" */
			/* --------------------------- */
			string_commas_number_string(
				transmit_megabytes_string ) );

		fprintf(output_pipe,
			"%s^%s^%s^%s^%.2lf^%.2lf\n",
			date_string,
			ethernet_address,
			receive_megabytes_commas_string,
			transmit_megabytes_commas_string,
			expected_receive_percent,
			expected_transmit_percent );
	}

	pclose( pipe );
}

char *current_month_year( void )
{
	char system_string[ 128 ];

	sprintf(system_string,
		"date.e 0 | piece.e '-' 0,1" );

	return string_pipe( system_string );
}

int current_day( void )
{
	char system_string[ 128 ];

	sprintf( system_string,
		 "date.e 0 | piece.e '-' 2" );

	return atoi( string_pipe( system_string ) );
}

double expected_percent(
		int current_megabytes,
		char *date_string,
		char *current_month_year,
		int current_day )
{
	double expected_percent = 0.0;
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

	expected_percent =
		(	(double)expected_month_consumed /
			MEGABYTES_PER_TERABYTE ) * 100.0;

	return expected_percent;
}

