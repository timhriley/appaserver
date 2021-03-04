/* --------------------------------------------------- 	*/
/* src_hydrology/conductivity2salinity.c	       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "document.h"
#include "html_table.h"
#include "query.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application.h"
#include "hydrology_library.h"
#include "sed.h"
/*
#include "measurement_update_parameter.h"
*/

/* Constants */
/* --------- */
#define PROCESS_NAME			"conductivity2salinity"
#define MEASUREMENT_SELECT_LIST		"station,measurement_date,measurement_time,measurement_value,last_validation_date,last_person_validating,last_validation_process"

#define MEASUREMENT_INSERT_LIST		"station,measurement_date,measurement_time,datatype,measurement_value,measurement_update_method,last_validation_date,last_person_validating,last_validation_process"

#define MEASUREMENT_UPDATE_LIST		"station,measurement_date,measurement_time,datatype"

#define MEASUREMENT_STATION_PIECE			0
#define MEASUREMENT_DATE_PIECE				1
#define MEASUREMENT_TIME_PIECE				2
#define MEASUREMENT_VALUE_PIECE				3
#define MEASUREMENT_LAST_VALIDATION_DATE_PIECE		4
#define MEASUREMENT_LAST_PERSON_VALIDATING_PIECE	5
#define MEASUREMENT_LAST_VALIDATION_PROCESS_PIECE	6
#define ESTIMATION_METHOD				"from_conductivity"

/* Prototypes */
/* ---------- */
LIST *merge_conductivity_datatype_list(
			LIST *dictionary_conductivity_datatype_list,
			LIST *conductivity_salinity_datatype_list );

LIST *get_salinity_datatype_list(
			char *application_name,
			LIST *dictionary_conductivity_datatype_list );

LIST *get_conductivity_datatype_list(	char *application_name );

void get_datatype_lists(	LIST **dictionary_conductivity_datatype_list,
				LIST **salinity_datatype_list,
				char *application_name,
				DICTIONARY *parameter_dictionary );

void salinity2oxygen_solubility(
					char *application_name,
					char *station,
					char *where_clause,
					boolean html_document_ok );

void output_results(			DICTIONARY *parameter_dictionary,
					char *station,
					char really_yn,
					int count,
					char *salinity_datatype );

double conductivity2salinity(		double conductivity );

int conductivity2salinity_for_datatype(	char *conductivity_datatype,
					char *salinity_datatype,
					char *application_name,
					char *where_clause,
					char really_yn,
					boolean html_document_ok );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *where_clause;
	DOCUMENT *document;
	int really_yn;
	DICTIONARY *parameter_dictionary;
	char *parameter_dictionary_string;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	boolean html_document_ok;
	char *database_string = {0};
	char *salinity_datatype;
	char *conductivity_datatype;
	LIST *dictionary_conductivity_datatype_list = {0};
	LIST *salinity_datatype_list = {0};
	int count;

	if ( argc < 7 )
	{
		fprintf(stderr,
	"Usage: %s ignored application station parameter_dictionary \"where_clause\" really_yn [nohtml]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	/* login_name = argv[ 1 ]; */
	application_name = argv[ 2 ];

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

	station = argv[ 3 ];
	parameter_dictionary_string = argv[ 4 ];
	where_clause = argv[ 5 ];

	search_replace_string(
		where_clause,
		"conductivity_datatype",
		"datatype" );

	really_yn = *argv[ 6 ];

	html_document_ok = ( argc != 8 || strcmp( argv[ 7 ], "nohtml" ) != 0);

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	parameter_dictionary = 
		dictionary_index_string2dictionary(
			parameter_dictionary_string );

	parameter_dictionary = dictionary_remove_prefix(
						parameter_dictionary,
						QUERY_STARTING_LABEL );

	get_datatype_lists(	&dictionary_conductivity_datatype_list,
				&salinity_datatype_list,
				application_name,
				parameter_dictionary );

	parameter_dictionary =
		dictionary_remove_index(
			parameter_dictionary );

	if ( html_document_ok )
	{
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
	}

	if ( !station || strcmp( station, "station" ) == 0 )
	{
		if ( html_document_ok )
		{
			printf( "<h3>Please select a station.</h3>\n" );
			document_close();
		}
		else
		{
			fprintf( stderr, "Please select a station.\n" );
		}
		exit( 0 );
	}

	if ( !dictionary_exists_key(
			parameter_dictionary,
			"relation_operator_measurement_date" ) )
	{
		if ( html_document_ok )
		{
			printf( "<h3>Please build a date clause.</h3>\n" );
			document_close();
		}
		else
		{
			fprintf( stderr,
"Must include dictionary variable relation_operator_measurement_date\n" );
		}
		exit( 0 );
	}

	if ( !list_length( dictionary_conductivity_datatype_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty dictionary_conductivity_datatype_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		if ( html_document_ok ) document_close();

		exit( 0 );
	}

	if ( list_length( dictionary_conductivity_datatype_list ) !=
	      list_length( salinity_datatype_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: The conductivity to salinity MAP is not synchronized.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		if ( html_document_ok ) document_close();

		exit( 1 );
	}

	list_rewind( dictionary_conductivity_datatype_list );
	list_rewind( salinity_datatype_list );

	do {
		conductivity_datatype =
			list_get_pointer(
				dictionary_conductivity_datatype_list );

		salinity_datatype =
			list_get_pointer(
				salinity_datatype_list );

		count = conductivity2salinity_for_datatype(
						conductivity_datatype,
						salinity_datatype,
						application_name,
						where_clause,
						really_yn,
						html_document_ok );
	
		if ( html_document_ok )
		{
			output_results(	parameter_dictionary,
					station,
					really_yn,
					count,
					salinity_datatype );
		}

		if ( really_yn == 'y'
		&&   strcmp( salinity_datatype, "salinity" ) == 0 )
		{
			salinity2oxygen_solubility(
				application_name,
				station,
				where_clause,
				html_document_ok );
		}

		list_next( salinity_datatype_list );

	} while( list_next( dictionary_conductivity_datatype_list ) );

	if ( really_yn == 'y' )
	{
#ifdef NOT_DEFINED
		if ( parameter_dictionary
		&&   dictionary_length( parameter_dictionary ) )
		{
			measurement_update_parameter =
					measurement_update_parameter_new(
							application_name,
							station,
							"salinity",
							ESTIMATION_METHOD,
							login_name,
							parameter_dictionary,
							(char *)0 /* notes */ );
	
			measurement_update_parameter_save(
					measurement_update_parameter );
		}
#endif
		process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	}

	if ( html_document_ok ) document_close();

	exit( 0 );
} /* main() */

double conductivity2salinity( double conductivity )
{
	if ( conductivity < 0.0 ) return 0.0;

	return 	0.008 
		- ( 0.16920 * pow ( conductivity / 53.0648, 0.5 ) )
		+ ( 25.3851 * pow ( conductivity / 53.0648, 1.0 ) )
		+ ( 14.0941 * pow ( conductivity / 53.0648, 1.5 ) )
		- (  7.0261 * pow ( conductivity / 53.0648, 2.0 ) )
		+ (  2.7081 * pow ( conductivity / 53.0648, 2.5 ) );
} /* conductivity2salinity() */

int conductivity2salinity_for_datatype(	char *conductivity_datatype,
					char *salinity_datatype,
					char *application_name,
					char *where_clause,
					char really_yn,
					boolean html_document_ok )
{
	char *table_name;
	char full_where_clause[ 1024 ];
	char buffer[ 4096 ];
	FILE *input_pipe;
	FILE *insert_pipe;
	FILE *update_pipe;
	char title[ 128 ];
	char conductivity_datatype_label[ 128 ];
	char salinity_datatype_label[ 128 ];
	int count = 0;
	HTML_TABLE *html_table;
	LIST *heading_list;
	double results = 0.0;
	char measurement_station_string[ 128 ];
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	char measurement_value_string[ 128 ];
	char measurement_last_validation_date_string[ 128 ];
	char measurement_last_person_validating_string[ 128 ];
	char measurement_last_validation_process_string[ 128 ];

	table_name = get_table_name( application_name, "measurement" );

	sprintf( full_where_clause, 
		 "%s and datatype = '%s'",
		 where_clause,
		 conductivity_datatype );

	sprintf(buffer,
	"get_folder_data	application=%s				  "
	"			folder=measurement			  "
	"			select='%s'				  "
	"			where=\"%s\"				 |"
	"tr '%c' '|'							  ",
		application_name,
		MEASUREMENT_SELECT_LIST,
		full_where_clause,
		FOLDER_DATA_DELIMITER );

	input_pipe = popen( buffer, "r" );

	sprintf(buffer,
"insert_statement.e table=%s field=%s replace=y delimiter='^' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
		table_name,
		MEASUREMENT_INSERT_LIST );

	insert_pipe = popen( buffer, "w" );

	sprintf(buffer,
		"update_statement.e %s %s '^^' | sql.e",
		table_name,
		MEASUREMENT_UPDATE_LIST );

	update_pipe = popen( buffer, "w" );

	sprintf(	title,
			"Specific Conductance to Salinity (%s -> %s)",
			conductivity_datatype,
			salinity_datatype );

	html_table = new_html_table(
			title,
			(char *)0 /* sub_title */ );

	if ( really_yn != 'y' )
	{
		format_initial_capital(	conductivity_datatype_label,
					conductivity_datatype );

		format_initial_capital(	salinity_datatype_label,
					salinity_datatype );

		heading_list = list_new_list();
		list_append_string( heading_list, "Station" );
		list_append_string( heading_list, "Measurement Date" );
		list_append_string( heading_list, "Measurement Time" );
		list_append_string( heading_list, conductivity_datatype_label );
		list_append_string( heading_list, salinity_datatype_label );
	
		html_table_set_heading_list( html_table, heading_list );
		html_table_output_table_heading(
						html_table->title,
						html_table->sub_title );
		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
	}
	else
	{
		if ( html_document_ok )
		{
			html_table_output_table_heading(
						html_table->title,
						html_table->sub_title );
		}
	}

	/* Sample input: "BD|1999-01-01|1000|5.00" */
	/* --------------------------------------- */
	while( get_line( buffer, input_pipe ) )
	{
		count++;

		piece(	measurement_station_string, 
			'|', 
			buffer, 
			MEASUREMENT_STATION_PIECE );

		piece(	measurement_date_string, 
			'|', 
			buffer, 
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time_string, 
			'|', 
			buffer, 
			MEASUREMENT_TIME_PIECE );

		piece(	measurement_value_string, 
			'|', 
			buffer, 
			MEASUREMENT_VALUE_PIECE );

		piece(	measurement_last_validation_date_string,
			'|',
			buffer,
			MEASUREMENT_LAST_VALIDATION_DATE_PIECE );

		piece(	measurement_last_person_validating_string,
			'|',
			buffer,
			MEASUREMENT_LAST_PERSON_VALIDATING_PIECE );

		piece(	measurement_last_validation_process_string,
			'|',
			buffer,
			MEASUREMENT_LAST_VALIDATION_PROCESS_PIECE );

		if ( *measurement_value_string )
		{
			results = conductivity2salinity(
					atof( measurement_value_string ) );
		}

		if ( really_yn == 'y' )
		{
			if ( *measurement_value_string )
			{
				fprintf(insert_pipe,
				"%s^%s^%s^%s^%lf^%s^%s^%s^%s\n",
				measurement_station_string,
				measurement_date_string,
				measurement_time_string,
				salinity_datatype,
				results,
				ESTIMATION_METHOD,
				measurement_last_validation_date_string,
				measurement_last_person_validating_string,
				measurement_last_validation_process_string );

				fprintf(update_pipe,
				"%s^%s^%s^%s^measurement_value^%lf\n",
				measurement_station_string,
				measurement_date_string,
				measurement_time_string,
				salinity_datatype,
				results );
			}
			else
			{
				fprintf(insert_pipe,
				"%s^%s^%s^%s^^%s^%s^%s^%s\n",
				measurement_station_string,
				measurement_date_string,
				measurement_time_string,
				salinity_datatype,
				ESTIMATION_METHOD,
				measurement_last_validation_date_string,
				measurement_last_person_validating_string,
				measurement_last_validation_process_string );

				fprintf(update_pipe,
				"%s^%s^%s^%s^measurement_value^\n",
				measurement_station_string,
				measurement_date_string,
				measurement_time_string,
				salinity_datatype );
			}
			fprintf(update_pipe,
				"%s^%s^%s^%s^last_validation_date^%s\n",
				measurement_station_string,
				measurement_date_string,
				measurement_time_string,
				salinity_datatype,
				measurement_last_validation_date_string );
			fprintf(update_pipe,
				"%s^%s^%s^%s^last_person_validating^%s\n",
				measurement_station_string,
				measurement_date_string,
				measurement_time_string,
				salinity_datatype,
				measurement_last_person_validating_string );
			fprintf(update_pipe,
				"%s^%s^%s^%s^last_validation_process^%s\n",
				measurement_station_string,
				measurement_date_string,
				measurement_time_string,
				salinity_datatype,
				measurement_last_validation_process_string );
		}
		else
		{
			if ( *measurement_value_string )
			{
				sprintf(buffer,
					"%s|%s|%s|%s|%.4lf",
					measurement_station_string,
					measurement_date_string,
					measurement_time_string,
					measurement_value_string,
					results );
			}
			else
			{
				sprintf(buffer,
					"%s|%s|%s||",
					measurement_station_string,
					measurement_date_string,
					measurement_time_string );
			}

			if ( count == 50 )
			{
				html_table_set_record_data(	html_table,
								"Skipping",
								'|' );
				html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );

				list_free_string_list( html_table->data_list );
				html_table->data_list = list_new();
			}
			else
			if ( count < 50 )
			{
				html_table_set_record_data(	html_table,
								buffer,
								'|' );
				html_table_output_data(
					html_table->data_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->background_shaded,
					html_table->justify_list );

				list_free_string_list( html_table->data_list );
				html_table->data_list = list_new();
			}
		}
	}

	pclose( input_pipe );
	pclose( insert_pipe );
	pclose( update_pipe );
	if ( html_document_ok ) html_table_close();
	return count;
} /* conductivity2salinity_for_datatype() */

void output_results(	DICTIONARY *parameter_dictionary,
			char *station,
			char really_yn,
			int count,
			char *salinity_datatype )
{

	if ( really_yn == 'y' )
	{
		if ( count )
		{
			char count_string[ 128 ];
	
			printf( "<p>%s %s records processed: %d</p>\n",
				station,
				salinity_datatype,
				count );
	
			if ( parameter_dictionary
			&&   dictionary_length( parameter_dictionary ) )
			{
				sprintf( count_string, "%d", count );
				dictionary_add_pointer(
						parameter_dictionary,
						salinity_datatype,
						strdup( count_string ) );
			}
		}
	}
	else
	{
		if ( count )
		{
			printf(
			"<p>Not processed %s/%s. Records to process: %d</p>\n",
				station,
				salinity_datatype,
				count );
		}
	}
} /* output_results() */

void get_datatype_lists(	LIST **dictionary_conductivity_datatype_list,
				LIST **salinity_datatype_list,
				char *application_name,
				DICTIONARY *parameter_dictionary )
{

	*dictionary_conductivity_datatype_list =
		dictionary_get_index_data_list(
				parameter_dictionary,
				"conductivity_datatype" );

	if ( !list_length( *dictionary_conductivity_datatype_list ) )
	{
		*dictionary_conductivity_datatype_list =
			get_conductivity_datatype_list(
				application_name );
	}

	*salinity_datatype_list =
		get_salinity_datatype_list(
			application_name,
			*dictionary_conductivity_datatype_list );

} /* get_datatype_lists() */

LIST *get_salinity_datatype_list(
			char *application_name,
			LIST *dictionary_conductivity_datatype_list )
{
	char sys_string[ 1024 ];
	char *in_clause;
	char *select;
	char where[ 1024 ];
	LIST *conductivity_salinity_datatype_list;

	select = "conductivity_datatype,salinity_datatype";

	in_clause =
		timlib_with_list_get_in_clause(
			dictionary_conductivity_datatype_list );

	sprintf( where, "conductivity_datatype in (%s)", in_clause );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=conductivity_salinity_map "
		 "			where=\"%s\"			 "
		 "			order=\"%s\"			 ",
		 application_name,
		 select,
		 where,
		 in_clause );

	conductivity_salinity_datatype_list = pipe2list( sys_string );

	return merge_conductivity_datatype_list(
			dictionary_conductivity_datatype_list,
			conductivity_salinity_datatype_list );

} /* get_salinity_datatype_list() */

void salinity2oxygen_solubility(
			char *application_name,
			char *station,
			char *where_clause,
			boolean html_document_ok )
{
	char sys_string[ 1024 ];
	char *report_title;
	char local_where_clause[ 1024 ];
	char delete_where_clause[ 1024 ];
	SED *sed;
	char *table_name;

	if ( html_document_ok )
	{
		report_title = "Salinity to Oxygen Solubility";
		printf( "<h1>%s</h1>\n", report_title );
	}

	sed = sed_new( " and datatype in .*", "" );
	strcpy( local_where_clause, where_clause );
	sed_search_replace( local_where_clause, sed );

	search_replace_string(	local_where_clause,
				" and datatype = 'conductivity'",
				"" );

	sprintf( delete_where_clause,
		 "%s and datatype = '%s'",
		 local_where_clause,
		 OXYGEN_SOLUBILITY_DATATYPE );

	table_name = get_table_name( application_name, "measurement" );

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 table_name,
		 delete_where_clause );

	system( sys_string );

	sprintf(sys_string,
		"calculate_oxygen_solubility %s %s \"%s\" sql 		|"
		"sql.e							 ",
		application_name,
		station,
		local_where_clause );

	system( sys_string );

} /* salinity2oxygen_solubility() */

LIST *get_conductivity_datatype_list(
					char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=conductivity_datatype	 "
		 "			folder=conductivity_salinity_map ",
		 application_name );

	return pipe2list( sys_string );

} /* get_conductivity_datatype_list() */

LIST *merge_conductivity_datatype_list(
			LIST *dictionary_conductivity_datatype_list,
			LIST *conductivity_salinity_datatype_list )
{
	LIST *salinity_datatype_list = list_new();
	char *dictionary_conductivity_datatype;
	char *record;
	char conductivity_datatype[ 128 ];
	char salinity_datatype[ 128 ];

	if ( !list_rewind( dictionary_conductivity_datatype_list ) )
		return salinity_datatype_list;

	do {
		dictionary_conductivity_datatype =
			list_get(
				dictionary_conductivity_datatype_list );

		list_rewind( conductivity_salinity_datatype_list );

		do {
			record =
				list_get(
					conductivity_salinity_datatype_list );

			piece(	conductivity_datatype,
				FOLDER_DATA_DELIMITER,
				record,
				0 );

			if ( strcmp(
				conductivity_datatype,
				dictionary_conductivity_datatype ) == 0 )
			{
				piece(	salinity_datatype,
					FOLDER_DATA_DELIMITER,
					record,
					1 );

				list_append_pointer(
					salinity_datatype_list,
					strdup( salinity_datatype ) );

				break;
			}

		} while( list_next( conductivity_salinity_datatype_list ) );

	} while( list_next( dictionary_conductivity_datatype_list ) );

	return salinity_datatype_list;

} /* merge_conductivity_datatype_list() */

