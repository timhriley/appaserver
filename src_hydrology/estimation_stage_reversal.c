/* --------------------------------------------------- 	*/
/* estimation_stage_reversal.c			       	*/
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
#include "application.h"
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
#define MEASUREMENT_VALUE_PIECE		4
#define ESTIMATION_METHOD		"stage_reversal"

#define HEADING_LIST "station,datatype,date,time,original_value,new_value"
#define HTML_TABLE_JUSTIFY_LIST "left,left,left,left,right,right"

/* Prototypes */
/* ---------- */
char *get_where_clause(			char *station,
					char *datatype,
					char *from_date_string,
					char *to_date_string,
					char *from_measurement_time,
					char *to_measurement_time );

int main( int argc, char **argv )
{
	register int counter = 0;
	char *login_name;
	char *application_name;
	char *station;
	char *datatype;
	char *where_clause;
	char delimited_record[ 4096 ];
	char buffer[ 4096 ];
	char *table_name;
	FILE *input_pipe;
	FILE *html_pipe = {0};
	FILE *update_pipe = {0};
	MEASUREMENT_BACKUP *measurement_backup = {0};
	char measurement_date_string[ 128 ];
	char measurement_value_string[ 128 ];
	double results_double;
	double difference;
	double current_value;
	char really_yn;
	char results_string[ 128 ];
	char *process_name;
	DATE *measurement_date = {0};
	DATE *begin_date, *end_date;
	DICTIONARY *parameter_dictionary = {0};
	DOCUMENT *document;
	MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *begin_date_string, *end_date_string;
	char *begin_time_string, *end_time_string;
	double first_value = 0.0;
	boolean first_time = 1;
	char *notes;
	char *database_string = {0};

	if ( argc < 11 )
	{
		fprintf(stderr,
"Usage: %s application_name login_name process station datatype begin_date begin_time end_date end_time really_yn [dictionary]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];
	process_name = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	begin_date_string = argv[ 6 ];
	begin_time_string = argv[ 7 ];
	end_date_string = argv[ 8 ];
	end_time_string = argv[ 9 ];
	really_yn = *argv[ 10 ];

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

		printf( "<p>ERROR: invalid date format.\n" );
		document_close();
		exit( 0 );
	}

	hydrology_library_get_clean_begin_end_time(
					&begin_time_string,
					&end_time_string );

	begin_date =
		date_new_yyyy_mm_dd_date(
			begin_date_string );

	end_date =
		date_new_yyyy_mm_dd_date(
			end_date_string );

	where_clause =
		get_where_clause(	station,
					datatype,
					begin_date_string,
					end_date_string,
					begin_time_string,
					end_time_string );

	if ( argc == 12 && *argv[ 11 ] )
	{
		parameter_dictionary = 
			dictionary_index_string2dictionary(
					argv[ 11 ] );
	
		parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_STARTING_LABEL );
	
		parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_FROM_STARTING_LABEL );
	
		parameter_dictionary = dictionary_remove_index(
						parameter_dictionary );
	
		dictionary_get_index_data(
					&notes,
					parameter_dictionary,
					"notes",
					0 );

/*
		dictionary_get_index_data(
					&expanded_begin_date_string,
					parameter_dictionary,
					"expanded_begin_date",
					0 );

		dictionary_get_index_data(
					&expanded_end_date_string,
					parameter_dictionary,
					"expanded_end_date",
					0 );
*/

		measurement_update_parameter =
			measurement_update_parameter_new(
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

/*
		expanded_where_clause =
			get_where_clause(	station,
						datatype,
						expanded_begin_date_string,
						expanded_end_date_string,
						begin_time_string,
						end_time_string );
*/
	}

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	if ( really_yn != 'y' )
	{
		char sys_string[ 1024 ];

		sprintf(sys_string,
		 	"html_table.e \"%s\" \"%s\" '%c' %s",
		 	"Estimation Stage Reversal",
		 	HEADING_LIST,
		 	FOLDER_DATA_DELIMITER,
		 	HTML_TABLE_JUSTIFY_LIST );
		html_pipe = popen( sys_string, "w" );
	}

	table_name = 
		get_table_name( application_name, "measurement" );

/* Used for future grace graph.
	if ( expanded_where_clause )
	{
		sprintf(
		buffer, 
		"get_folder_data	application=%s			  "
		"			folder=%s			  "
		"			select='%s'			  "
		"			where=\"%s\"			 |"
		"cat 							  ",
		application_name,
		table_name,
		MEASUREMENT_SELECT_LIST,
		expanded_where_clause );
	}
	else
	{
		sprintf(
		buffer, 
		"get_folder_data	application=%s			  "
		"			folder=%s			  "
		"			select='%s'			  "
		"			where=\"%s\"			 |"
		"cat 							  ",
		application_name,
		table_name,
		MEASUREMENT_SELECT_LIST,
		where_clause );
	}
*/

	sprintf(
		buffer, 
		"get_folder_data	application=%s			  "
		"			folder=%s			  "
		"			select='%s'			  "
		"			where=\"%s\"			 |"
		"cat 							  ",
		application_name,
		table_name,
		MEASUREMENT_SELECT_LIST,
		where_clause );

	input_pipe = popen( buffer, "r" );

	if ( really_yn == 'y' )
	{
		sprintf(buffer,
			"update_statement.e table=%s key=%s carrot=y | sql.e",
			table_name,
			MEASUREMENT_KEY_LIST );

		update_pipe = popen( buffer, "w" );
	}

	/* Sample input: "BD^stage^1999-01-01^1000^10" */
	/* ------------------------------------------- */
	while( get_line( delimited_record, input_pipe ) )
	{
		piece(	measurement_date_string,
			FOLDER_DATA_DELIMITER,
			delimited_record,
			MEASUREMENT_DATE_PIECE );

		piece( 	measurement_value_string,
			FOLDER_DATA_DELIMITER,
			delimited_record,
			MEASUREMENT_VALUE_PIECE );

		if ( measurement_date ) date_free( measurement_date );

		measurement_date =
			date_new_yyyy_mm_dd_date(
				measurement_date_string );

		if ( !measurement_date )
		{
			fprintf( stderr, 
		"ERROR in %s/%s(): cannot get measurement_date\n",
				 __FILE__,
				 __FUNCTION__ );
			exit( 1 );
		}

		if ( measurement_date->current < begin_date->current
		||   measurement_date->current > end_date->current )
		{
			if ( html_pipe )
			{
				fprintf(html_pipe,
					"%s%c%s\n",
					delimited_record,
					FOLDER_DATA_DELIMITER,
					measurement_value_string );
			}
			continue;
		}

		if ( *measurement_value_string )
		{
			counter++;

			current_value = atof( measurement_value_string );

			if ( first_time )
			{
				first_value = current_value;
				first_time = 0;

				if ( html_pipe )
				{
					fprintf(html_pipe,
						"%s%c%s\n",
						delimited_record,
						FOLDER_DATA_DELIMITER,
						measurement_value_string );
				}
				continue;
			}


			difference = current_value - first_value;
			results_double = first_value - difference;

			if ( really_yn == 'y' && update_pipe )
			{
				char local_delimited_record[ 1024 ];

				strcpy(	local_delimited_record,
					delimited_record );

				piece_delete(
					local_delimited_record,
					FOLDER_DATA_DELIMITER,
					MEASUREMENT_VALUE_PIECE );

				fprintf(update_pipe, 
					"%s^measurement_value^%lf\n",
				   	local_delimited_record,
				   	results_double );

/*
				printf(
					"<p>%s^measurement_value^%lf\n",
				   	local_delimited_record,
				   	results_double );
*/

				fprintf(update_pipe, 
					"%s^measurement_update_method^%s\n",
					local_delimited_record,
					ESTIMATION_METHOD );

				if ( measurement_backup )
				{
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
					delimited_record,
					really_yn,
					FOLDER_DATA_DELIMITER );
				}
			}

			sprintf(results_string,
				"%.3lf", 
				results_double );
		}
		else
		{
			strcpy( results_string, "null" );
		}

		if ( html_pipe )
		{
			fprintf(html_pipe,
				"%s%c%s\n",
				delimited_record,
				FOLDER_DATA_DELIMITER,
				results_string );
		}
	}

	pclose( input_pipe );

	if ( measurement_backup )
		measurement_backup_close( measurement_backup->insert_pipe );

	if ( update_pipe ) pclose( update_pipe );
	if ( html_pipe ) pclose( html_pipe );

	if ( really_yn == 'y'
	&&   parameter_dictionary
	&&   measurement_update_parameter )
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

	if ( really_yn == 'y' )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
		printf( "<p>Process complete.\n" );
	}

	document_close();

	exit( 0 );
} /* main() */

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

