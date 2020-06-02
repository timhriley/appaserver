/* --------------------------------------------------- 	*/
/* estimation_linear_interpolation_malfunction.c	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "query.h"
#include "timlib.h"
#include "document.h"
#include "piece.h"
#include "date.h"
#include "measurement_backup.h"
#include "appaserver_parameter_file.h"
#include "environ.h"

/* Constants */
/* --------- */
#define MEASUREMENT_SELECT_LIST		 "station,datatype,measurement_date,measurement_time,measurement_value"

#define MEASUREMENT_KEY_LIST		 "station,datatype,measurement_date,measurement_time"

#define MEASUREMENT_DATE_PIECE		2
#define MEASUREMENT_TIME_PIECE		3
#define MEASUREMENT_VALUE_PIECE		4
#define INPUT_DELIMITER			','
#define ESTIMATION_METHOD		"malfunction"

/* Prototypes */
/* ---------- */
char *get_where_clause(		char *station,
				char *datatype,
				char *from_date_string,
				char *to_date_string,
				char *from_measurement_time,
				char *to_measurement_time );

double get_rise_double( 	char *application_name, 
				char *where_clause,
				double first_value_double,
				double last_value_double );

int main( int argc, char **argv )
{
	register int counter = 0;
	char *login_name;
	char *application_name;
	char *station;
	char *datatype;
	char *expanded_where_clause;
	char *where_clause;
	char buffer[ 1024 ];
	char comma_delimited_record[ 1024 ];
	char *table_name;
	FILE *input_pipe;
	FILE *output_pipe = {0};
	MEASUREMENT_BACKUP *measurement_backup = {0};
	double results_double;
	double first_value_double;
	double last_value_double;
	double rise_double;
	int really_yn;
	DICTIONARY *parameter_dictionary;
	char *parameter_dictionary_string;
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	char measurement_value_string[ 128 ];
	char *expanded_begin_date_string, *expanded_end_date_string;
	char *begin_date_string, *end_date_string;
	char *begin_time_string, *end_time_string;
	DATE *begin_date, *end_date;
	DATE *measurement_date = {0};
	char *notes;
	char *database_string = {0};

	if ( argc != 16 )
	{
		fprintf(stderr,
"Usage: %s person application_name station datatype first_value last_value begin_date begin_time end_date end_time expanded_begin_date expanded_end_date dictionary notes really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}


	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	station = argv[ 3 ];
	datatype = argv[ 4 ];
	first_value_double = atof( argv[ 5 ] );
	last_value_double = atof( argv[ 6 ] );
	begin_date_string = argv[ 7 ];
	begin_time_string = argv[ 8 ];
	end_date_string = argv[ 9 ];
	end_time_string = argv[ 10 ];
	expanded_begin_date_string = argv[ 11 ];
	expanded_end_date_string = argv[ 12 ];
	parameter_dictionary_string = argv[ 13 ];
	notes = argv[ 14 ];
	really_yn = *argv[ 15 ];

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

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	begin_date =
		date_new_yyyy_mm_dd_hhmm(
			begin_date_string,
			begin_time_string );

	end_date =
		date_new_yyyy_mm_dd_hhmm(
			end_date_string,
			end_time_string );

	expanded_where_clause =
		get_where_clause(	station,
					datatype,
					expanded_begin_date_string,
					expanded_end_date_string,
					begin_time_string,
					end_time_string );

	where_clause =
		get_where_clause(	station,
					datatype,
					begin_date_string,
					end_date_string,
					begin_time_string,
					end_time_string );

	parameter_dictionary = 
		dictionary_index_string2dictionary(
				parameter_dictionary_string );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_FROM_STARTING_LABEL );

	parameter_dictionary = dictionary_remove_index(
						parameter_dictionary );

	measurement_update_parameter = measurement_update_parameter_new(
					application_name,
					station,
					datatype,
					ESTIMATION_METHOD,
					login_name,
					parameter_dictionary,
					notes );

	table_name = get_table_name( application_name, "measurement" );

	rise_double = get_rise_double(	application_name, 
					where_clause,
					first_value_double,
					last_value_double );

	sprintf(buffer, 
	"get_folder_data	application=%s				  "
	"			folder=%s				  "
	"			select='%s'				  "
	"			where=\"%s\"				 |"
		"tr '%c' '%c'						  ",
		application_name,
		"measurement",
		MEASUREMENT_SELECT_LIST,
		expanded_where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER );

/*
{
char msg[ 128 ];
sprintf( msg, "%s/%s()/%d\n",
__FILE__,
__FUNCTION__,
__LINE__ );
m2( "hydrology", msg );
}
*/
	input_pipe = popen( buffer, "r" );

	if ( really_yn == 'y' )
	{
		sprintf(buffer,
			"update_statement.e %s %s '|' | sql",
			table_name,
			MEASUREMENT_KEY_LIST );
	
		output_pipe = popen( buffer, "w" );
	
		measurement_backup = 
			new_measurement_backup(
				application_name, 
				ESTIMATION_METHOD,
				login_name,
				measurement_update_parameter->now_date,
				measurement_update_parameter->now_time );
	
		measurement_backup->insert_pipe =
			measurement_backup_open_insert_pipe(
						application_name );
	}

	results_double = first_value_double;

	/* Sample input: "BD,1999-01-01,1000,bottom_temperature,10" */
	/* -------------------------------------------------------- */
	while( get_line( comma_delimited_record, input_pipe ) )
	{
		piece(	measurement_date_string,
			',',
			comma_delimited_record,
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time_string,
			',',
			comma_delimited_record,
			MEASUREMENT_TIME_PIECE );

		if ( measurement_date ) date_free( measurement_date );

		measurement_date =
			date_new_yyyy_mm_dd_hhmm(
				measurement_date_string,
				measurement_time_string );

		if ( !measurement_date )
		{
			fprintf( stderr, 
		"ERROR in %s/%s(): cannot get measurement_date\n",
				 __FILE__,
				 __FUNCTION__ );
			exit( 1 );
		}

		piece( 	measurement_value_string,
			INPUT_DELIMITER,
			comma_delimited_record,
			MEASUREMENT_VALUE_PIECE );

		if ( measurement_date->current < begin_date->current
		||   measurement_date->current > end_date->current )
		{
			printf( "%s,%s\n",
				comma_delimited_record,
				measurement_value_string );
		}
		else
		{
			counter++;
	
			if ( really_yn == 'y' )
			{
				fprintf(output_pipe, 
					"%s|measurement_value=%lf\n",
					comma_delimited_record,
					results_double );
		
				fprintf(output_pipe, 
					"%s|measurement_update_method=%s\n",
					comma_delimited_record,
					ESTIMATION_METHOD );
	
				measurement_backup_insert(
					measurement_backup->insert_pipe,
					measurement_backup->
						measurement_update_date,
					measurement_backup->
						measurement_update_time,
					measurement_backup->
						measurement_update_method,
					measurement_backup->login_name,
					comma_delimited_record,
					really_yn,
					',' );
	
			}
	
			printf( "%s,%lf\n",
				comma_delimited_record,
				results_double );
	
			results_double += rise_double;
		}
	}
	pclose( input_pipe );

	if ( really_yn == 'y' )
	{
		char counter_string[ 128 ];

		pclose( output_pipe );
		measurement_backup_close( measurement_backup->insert_pipe );

		sprintf( counter_string, "%d", counter );
		dictionary_add_string( 	parameter_dictionary,
					"count",
					counter_string );

		measurement_update_parameter_save(
				measurement_update_parameter );
	}
	printf( "%d\n", counter );
	exit( 0 );
} /* main() */


double get_rise_double( 	char *application_name, 
				char *where_clause,
				double first_value_double,
				double last_value_double )
{
	char buffer[ 1024 ];
	int count;
	char *results;

	sprintf(buffer, 
	"get_folder_data	application=%s		"
	"			folder=measurement	"
	"			select=count		"
	"			where=\"%s\"		",
		application_name,
		where_clause );

	results = pipe2string( buffer );
	if ( !results )
		count = 0;
	else
		count = atoi( results );

	return (last_value_double - first_value_double) / (double)(count - 1);
} /* get_rise_double() */

char *get_where_clause(	char *station,
			char *datatype,
			char *from_date_string,
			char *to_date_string,
			char *from_measurement_time,
			char *to_measurement_time )
{
	char where_clause[ 4096 ];

	sprintf( where_clause,
		 "station = '%s' and datatype = '%s' and %s",
		 station,
		 datatype,
		 query_get_between_date_time_where(
					"measurement_date",
					"measurement_time",
					from_date_string,
					from_measurement_time,
					to_date_string,
					to_measurement_time,
					(char *)0 /* application */,
					(char *)0 /* folder_name */ ) );
	return strdup( where_clause );
} /* get_where_clause() */

