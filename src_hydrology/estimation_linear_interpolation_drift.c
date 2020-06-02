/* --------------------------------------------------- 	*/
/* estimation_linear_interpolation_drift.c	       	*/
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
#include "timlib.h"
#include "measurement_update_parameter.h"
#include "query.h"
#include "document.h"
#include "piece.h"
#include "date.h"
#include "measurement_backup.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define MEASUREMENT_SELECT_LIST		 "station,datatype,measurement_date,measurement_time,measurement_value"

#define MEASUREMENT_KEY_LIST		 "station,datatype,measurement_date,measurement_time"

#define MEASUREMENT_DATE_PIECE		2
#define MEASUREMENT_TIME_PIECE		3
#define MEASUREMENT_VALUE_PIECE		4
#define INPUT_DELIMITER			','
#define ESTIMATION_METHOD		"drift"

/* Prototypes */
/* ---------- */
int get_count(			char *application_name,
				char *where_clause );

int interpolation_drift_add(	DICTIONARY *parameter_dictionary,
				char *application_name,
				char *where_clause,
				char *expanded_where_clause,
				double first_value_double,
				double last_value_double,
				char *table_name,
				char really_yn,
				MEASUREMENT_BACKUP *measurement_backup,
				DATE *begin_date,
				DATE *end_date );

int interpolation_drift_multiply(
				char *application_name,
				char *where_clause,
				char *expanded_where_clause,
				double first_multiplier_double,
				double last_multiplier_double,
				char *table_name,
				char really_yn,
				MEASUREMENT_BACKUP *measurement_backup,
				DATE *begin_date,
				DATE *end_date,
				double last_value_double );

char *get_where_clause(			char *station,
					char *datatype,
					char *from_date_string,
					char *to_date_string,
					char *from_measurement_time,
					char *to_measurement_time );

double get_first_value_in_database(	char *application_name,
					char *where_clause );
double get_last_value_in_database(	char *application_name,
					char *where_clause );
double get_rise_double( 		char *application_name, 
					char *where_clause,
					double first_value_double,
					double last_value_double,
					double first_value_in_database,
					double last_value_in_database );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *station;
	char *datatype;
	char *expanded_where_clause;
	char *where_clause;
	char *table_name;
	MEASUREMENT_BACKUP *measurement_backup;
	double first_value_double;
	double last_value_double;
	double first_multiplier_double;
	double last_multiplier_double;
	char really_yn;
	DICTIONARY *parameter_dictionary;
	char *parameter_dictionary_string;
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *expanded_begin_date_string, *expanded_end_date_string;
	char *begin_date_string, *end_date_string;
	char *begin_time_string, *end_time_string;
	DATE *begin_date, *end_date;
	char *notes;
	char *process_name;
	int counter;
	char *database_string = {0};

	if ( argc != 19 )
	{
		fprintf(stderr,
"Usage: %s person application_name station datatype first_value last_value first_multiplier last_multiplier begin_date begin_time end_date end_time expanded_begin_date expanded_end_date dictionary notes process really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	station = argv[ 3 ];
	datatype = argv[ 4 ];
	first_value_double = atof( argv[ 5 ] );
	last_value_double = atof( argv[ 6 ] );
	first_multiplier_double = atof( argv[ 7 ] );
	last_multiplier_double = atof( argv[ 8 ] );
	begin_date_string = argv[ 9 ];
	begin_time_string = argv[ 10 ];
	end_date_string = argv[ 11 ];
	end_time_string = argv[ 12 ];
	expanded_begin_date_string = argv[ 13 ];
	expanded_end_date_string = argv[ 14 ];
	parameter_dictionary_string = argv[ 15 ];
	notes = argv[ 16 ];
	process_name = argv[ 17 ];
	really_yn = *argv[ 18 ];

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

	if ((!(	first_value_double +
		last_value_double +
		first_multiplier_double +
		last_multiplier_double))
	|| ( ( first_value_double + last_value_double )
	&&   ( first_multiplier_double + last_multiplier_double ) ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
"<p>ERROR: either first value and last value are used or first multiplier and last multiplier are used.\n" );
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

	if ( strcmp( process_name, DRIFT_RATIO_PROCESS_NAME ) == 0 )
	{
		if ( !hydrology_library_get_drift_ratio_variables(
						parameter_dictionary,
						&first_multiplier_double,
						&last_multiplier_double,
						application_name,
						first_value_double,
						last_value_double,
						station,
						datatype,
						begin_date_string,
						begin_time_string,
						end_date_string,
						end_time_string ) )
		{
			printf(
			"<h3>Cannot get drift ratio variables.</h3>\n" );
			exit( 0 );
		}
	}

	measurement_update_parameter = measurement_update_parameter_new(
					application_name,
					station,
					datatype,
					ESTIMATION_METHOD,
					login_name,
					parameter_dictionary,
					notes );
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

	table_name = 
		get_table_name( application_name, "measurement" );

	if ( !( first_multiplier_double + last_multiplier_double )
	&&  strcmp( process_name, DRIFT_RATIO_PROCESS_NAME ) != 0 )
	{
		counter = interpolation_drift_add(
				parameter_dictionary,
				application_name,
				where_clause,
				expanded_where_clause,
				first_value_double,
				last_value_double,
				table_name,
				really_yn,
				measurement_backup,
				begin_date,
				end_date );
	}
	else
	{
		counter = interpolation_drift_multiply(
				application_name,
				where_clause,
				expanded_where_clause,
				first_multiplier_double,
				last_multiplier_double,
				table_name,
				really_yn,
				measurement_backup,
				begin_date,
				end_date,
				last_value_double );
	}

	measurement_backup_close( measurement_backup->insert_pipe );

	if ( really_yn == 'y' )
	{
		char tmp_buffer[ 128 ];

		/* Add additional parameters to dictionary */
		/* --------------------------------------- */
		sprintf( tmp_buffer, "%d", counter );
		dictionary_add_string( 	parameter_dictionary,
					"count",
					tmp_buffer );

		measurement_update_parameter_save(
				measurement_update_parameter );
	}

	printf( "%d\n", counter );

	exit( 0 );
} /* main() */

double get_rise_double( 	char *application_name, 
				char *where_clause,
				double first_value_double,
				double last_value_double,
				double first_value_in_database,
				double last_value_in_database )
{
	int count;

	count = get_count(application_name, where_clause );

	if ( count < 2 )
	{
		return 0.0;
	}
	else
	{
		return ( ( ( last_value_double - last_value_in_database ) - 
		 	( first_value_double - first_value_in_database ) ) / 
			(double)(count - 1) );
	}
} /* get_rise_double() */

int get_count(	char *application_name,
		char *where_clause )
{
	char buffer[ 1024 ];
	char *results;
	int count;

	sprintf(buffer,
		"get_folder_data	application=%s			"
		"			folder=measurement		"
		"			select=count			"
		"			where=\"%s\"			",
		application_name,
		where_clause );

	if ( ! ( results = pipe2string( buffer ) ) )
		count = 0;
	else
		count = atoi( results );

	free( results );
	return count;
} /* get_count() */

double get_last_value_in_database(	char *application_name,
					char *where_clause )
{
	char buffer[ 4096 ];
	double results;
	char *results_string;

	sprintf(buffer, 
	"get_folder_data	application=%s			 "
	"			folder=measurement		 "
	"			select=%s			 "
	"			where=\"%s\"	 	        |"
	"tail -1						|"
	"piece '^' %d						 ",
		application_name,
		MEASUREMENT_SELECT_LIST,
		where_clause,
		MEASUREMENT_VALUE_PIECE );
	results_string = pipe2string( buffer );
	if ( !results_string )
		results = 0.0;
	else
		results = atof( results_string );
	return results;
} /* get_last_value_in_database() */

double get_first_value_in_database(	char *application_name,
					char *where_clause )
{
	char buffer[ 4096 ];
	double results;
	char *results_string;

	sprintf(buffer, 
	"get_folder_data	application=%s			 "
	"			folder=measurement		 "
	"			select=%s			 "
	"			where=\"%s\"			|"
	"piece '^' %d						|"
	"head -1					 	 ",
		application_name,
		MEASUREMENT_SELECT_LIST,
		where_clause,
		MEASUREMENT_VALUE_PIECE );

	results_string = pipe2string( buffer );
	if ( !results_string )
		results = 0.0;
	else
		results = atof( results_string );
	return results;
} /* get_first_value_in_database() */

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

int interpolation_drift_add(	DICTIONARY *parameter_dictionary,
				char *application_name,
				char *where_clause,
				char *expanded_where_clause,
				double first_value_double,
				double last_value_double,
				char *table_name,
				char really_yn,
				MEASUREMENT_BACKUP *measurement_backup,
				DATE *begin_date,
				DATE *end_date )
{
	double rise_double;
	double results_double;
	FILE *input_pipe;
	FILE *update_pipe;
	char comma_delimited_record[ 4096 ];
	char buffer[ 4096 ];
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	DATE *measurement_date = {0};
	char measurement_value_string[ 128 ];
	char results_string[ 128 ];
	int counter = 0;
	double first_value_in_database;
	double last_value_in_database;

	first_value_in_database = get_first_value_in_database(	
					application_name,
					where_clause );

	last_value_in_database = get_last_value_in_database(	
					application_name,
					where_clause );

	rise_double = get_rise_double(	application_name, 
					where_clause,
					first_value_double,
					last_value_double,
					first_value_in_database,
					last_value_in_database );

	sprintf(buffer,
		"get_folder_data	application=%s			  "
		"			folder=%s			  "
		"			select='%s'			  "
		"			where=\"%s\"			 |"
		"tr '%c' '%c'						  ",
		application_name,
		table_name,
		MEASUREMENT_SELECT_LIST,
		expanded_where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER );

	input_pipe = popen( buffer, "r" );

	sprintf(buffer,
		"update_statement.e %s %s '|' | sql",
		table_name,
		MEASUREMENT_KEY_LIST );

	update_pipe = popen( buffer, "w" );

	results_double = first_value_double - first_value_in_database;

	/* Sample input: "BD,stage,1999-01-01,1000,10" */
	/* ------------------------------------------- */
	while( get_line( comma_delimited_record, input_pipe ) )
	{
		piece(	measurement_date_string,
			INPUT_DELIMITER,
			comma_delimited_record,
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time_string,
			INPUT_DELIMITER,
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
			if ( *measurement_value_string )
			{
				counter++;
	
				if ( really_yn == 'y' )
				{
					fprintf(update_pipe, 
					"%s|measurement_value=%lf\n",
					   comma_delimited_record,
					   results_double + 
					   atof( measurement_value_string ) );
		
					fprintf(update_pipe, 
					"%s|measurement_update_method=%s\n",
					comma_delimited_record,
					ESTIMATION_METHOD );
	
					measurement_backup_insert(
					measurement_backup->
						insert_pipe,
					measurement_backup->
						measurement_update_date,
					measurement_backup->
						measurement_update_time,
					measurement_backup->
					      measurement_update_method,
					measurement_backup->
						login_name,
					comma_delimited_record,
					really_yn,
					',' );
				}
				sprintf( results_string, "%lf", 
					results_double + 
					atof( measurement_value_string ) );
			}
			else
			{
				strcpy( results_string, "null" );
			}
	
			printf( "%s,%s\n",
				comma_delimited_record,
				results_string );

			results_double += rise_double;
		}
	}

	pclose( input_pipe );
	pclose( update_pipe );

	if ( really_yn == 'y' )
	{
		char tmp_buffer[ 128 ];

		/* Add additional parameters to dictionary */
		/* --------------------------------------- */
		sprintf( tmp_buffer, "%lf", first_value_in_database );
		dictionary_add_string( 	parameter_dictionary,
					"first_db_value",
					tmp_buffer );


		sprintf( tmp_buffer, "%lf", last_value_in_database );
		dictionary_add_string( 	parameter_dictionary,
					"last_db_value",
					tmp_buffer );
	}

	return counter;
} /* interpolation_drift_add() */

int interpolation_drift_multiply(
				char *application_name,
				char *where_clause,
				char *expanded_where_clause,
				double first_multiplier_double,
				double last_multiplier_double,
				char *table_name,
				char really_yn,
				MEASUREMENT_BACKUP *measurement_backup,
				DATE *begin_date,
				DATE *end_date,
				double last_value_double )
{
	double multiplier_double;
	double results_double;
	double multiplier_increment;
	FILE *input_pipe;
	FILE *update_pipe;
	char comma_delimited_record[ 4096 ];
	char buffer[ 4096 ];
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	DATE *measurement_date = {0};
	char measurement_value_string[ 128 ];
	char results_string[ 128 ];
	int counter = 0;
	int count;

	count = get_count( application_name, where_clause );

	if ( count < 3 ) return 0;

	multiplier_increment =
		( last_multiplier_double - first_multiplier_double ) /
		(double)(count - 1);

	multiplier_double = first_multiplier_double;

	sprintf(buffer,
		"get_folder_data	application=%s			  "
		"			folder=%s			  "
		"			select='%s'			  "
		"			where=\"%s\"			 |"
		"tr '%c' '%c'						  ",
		application_name,
		table_name,
		MEASUREMENT_SELECT_LIST,
		expanded_where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER );

	input_pipe = popen( buffer, "r" );

	sprintf(buffer,
		"update_statement.e %s %s '|' | sql",
		table_name,
		MEASUREMENT_KEY_LIST );

	update_pipe = popen( buffer, "w" );

	/* Sample input: "BD,stage,1999-01-01,1000,10" */
	/* ------------------------------------------- */
	while( get_line( comma_delimited_record, input_pipe ) )
	{
		piece(	measurement_date_string,
			INPUT_DELIMITER,
			comma_delimited_record,
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time_string,
			INPUT_DELIMITER,
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
			if ( *measurement_value_string )
			{
				counter++;
	
				results_double =
					multiplier_double *
					atof( measurement_value_string );

				if ( double_virtually_same(
						results_double,
						last_value_double ) )
				{
					results_double = last_value_double;
				}

				if ( really_yn == 'y' )
				{
					fprintf(update_pipe, 
					"%s|measurement_value=%lf\n",
					   comma_delimited_record,
					   results_double );
		
					fprintf(update_pipe, 
					"%s|measurement_update_method=%s\n",
					comma_delimited_record,
					ESTIMATION_METHOD );
	
					measurement_backup_insert(
					measurement_backup->
						insert_pipe,
					measurement_backup->
						measurement_update_date,
					measurement_backup->
						measurement_update_time,
					measurement_backup->
					      measurement_update_method,
					measurement_backup->
						login_name,
					comma_delimited_record,
					really_yn,
					',' );
				}
				sprintf(results_string,
					"%lf", 
					results_double );
			}
			else
			{
				strcpy( results_string, "null" );
			}
	
			printf( "%s,%s\n",
				comma_delimited_record,
				results_string );

			multiplier_double += multiplier_increment;
		}
	}

	pclose( input_pipe );
	pclose( update_pipe );
	return counter;
} /* interpolation_drift_multiply() */

