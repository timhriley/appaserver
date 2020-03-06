/* ----------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/output_merged_waterquality.c 	*/
/* ----------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "date.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "session.h"
#include "hydrology_library.h"
#include "application_constants.h"
#include "appaserver_link_file.h"
#include "google_chart.h"
#include "dictionary_appaserver.h"
#include "merged.h"

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING	20

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void output_merged_waterquality_table(
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_space_time_key_list );

void output_merged_waterquality(
				char *application_name,
				char *begin_date,
				char *end_date,
				char *output_medium,
				MERGED_INPUT *merged_input );

DICTIONARY *output_merged_parameter_dictionary(
				char *parameter_dictionary_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *begin_date;
	char *end_date;
	char *output_medium;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *parameter_dictionary;
	MERGED_INPUT *merged_input;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
	"Usage: %s process begin_date end_date output_medium dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	begin_date = argv[ 2 ];
	end_date = argv[ 3 ];
	output_medium = argv[ 4 ];

	parameter_dictionary =
		output_merged_parameter_dictionary(
			argv[ 5 ]
				/* parameter_dictionary_string */ );


	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		char buffer[ 128 ];

		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n<h2>",
			format_initial_capital(
				buffer,
				process_name ) );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
		printf( "</h2>\n" );
		fflush( stdout );
	}

	if ( ! ( merged_input = merged_input_new( parameter_dictionary ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: merged_input_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	output_merged_waterquality(
				application_name,
				begin_date,
				end_date,
				output_medium,
				merged_input );

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

DICTIONARY *output_merged_parameter_dictionary(
				char *parameter_dictionary_string )
{
	DICTIONARY *parameter_dictionary;

	parameter_dictionary = 
		dictionary_index_string2dictionary(
			parameter_dictionary_string );

	dictionary_appaserver_parse_multi_attribute_keys(
		parameter_dictionary,
		(char *)0 /* prefix */ );

	dictionary_add_elements_by_removing_prefix(
		parameter_dictionary,
		QUERY_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
		parameter_dictionary,
		QUERY_FROM_STARTING_LABEL );

	return parameter_dictionary;

} /* output_merged_parameter_dictionary() */

void output_merged_waterquality(
				char *application_name,
				char *begin_date,
				char *end_date,
				char *output_medium,
				MERGED_INPUT *merged_input )
{
	MERGED *merged;
	HASH_TABLE *key_hash_table;
	LIST *date_space_time_key_list;

	merged = merged_new( merged_input );

	merged_measurement_hash_table(
		application_name,
		begin_date,
		end_date,
		merged->hydrology_station_datatype_list );

	merged_results_hash_table(
		begin_date,
		end_date,
		merged->waterquality_station_datatype_list );

	/* Get the time sequence from waterquality */
	/* --------------------------------------- */
	key_hash_table =
		merged_key_hash_table(
			merged->waterquality_station_datatype_list );

	date_space_time_key_list =
		 hash_table_get_ordered_key_list(
			key_hash_table );

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_merged_waterquality_table(
			begin_date,
			end_date,
			merged->waterquality_station_datatype_list,
			merged->hydrology_station_datatype_list,
			date_space_time_key_list );
	}

} /* output_merged_waterquality() */

void output_merged_waterquality_table(
			char *begin_date,
			char *end_date,
			LIST *waterquality_station_datatype_list,
			LIST *hydrology_station_datatype_list,
			LIST *date_space_time_key_list )
{
	char *date_space_time;
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char buffer[ 512 ];
	char initial_capital_buffer[ 512 ];
	int count = 0;
	char title[ 512 ];

	sprintf(title,
		"Merged from %s to %s",
		begin_date,
		end_date );

	html_table = html_table_new( title, "", "" );
	heading_list = list_new();
	list_append_pointer( heading_list, "Date" );

	/* Set the waterquality headings */
	/* ----------------------------- */
	if ( list_rewind( waterquality_station_datatype_list ) )
	{
		do {
			station_datatype =
				list_get_pointer(
					waterquality_station_datatype_list );

			sprintf(buffer,
				"%s %s %s",
				station_datatype->station,
				format_initial_capital(
					initial_capital_buffer,
					station_datatype->
						datatype ),
				station_datatype->units );

			list_append_pointer(
				heading_list,
				strdup( buffer ) );

		} while( list_next( waterquality_station_datatype_list ) );

	} /* If */

	/* Set the hydrology headings */
	/* -------------------------- */
	if ( list_rewind( hydrology_station_datatype_list ) )
	{
		do {
			station_datatype =
				list_get_pointer(
					hydrology_station_datatype_list );

			sprintf(buffer,
				"%s %s",
				station_datatype->station,
				format_initial_capital(
					initial_capital_buffer,
					station_datatype->
						datatype ) );

			list_append_pointer(
				heading_list,
				strdup( buffer ) );

		} while( list_next( hydrology_station_datatype_list ) );

	} /* If */

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns =
		list_length( heading_list ) - 1;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	if ( !list_rewind( date_space_time_key_list ) )
	{
		printf( "</table>\n" );
		return;
	}

	do {
		date_space_time =
			list_get_pointer(
				date_space_time_key_list );

		html_table_set_data(	html_table->data_list,
					strdup( date_space_time ) );

		/* Output waterquality */
		/* ------------------- */
		if ( list_rewind( waterquality_station_datatype_list ) )
		{
			do {
				station_datatype =
					list_get_pointer(
					 waterquality_station_datatype_list );

				measurement =
					hash_table_get_pointer(
						station_datatype->
							measurement_hash_table,
						date_space_time );

				if ( measurement && !measurement->is_null )
				{
					sprintf(buffer,
						"%.3lf",
						measurement->
							measurement_value );
				}
				else
				if ( measurement )
				{
					strcpy( buffer, "null" );
				}
				else
				{
					strcpy( buffer, "missing" );
				}

				html_table_set_data(
					html_table->data_list,
					strdup( buffer ) );

			} while( list_next(
					waterquality_station_datatype_list ) );

		} /* If */

		/* Output hydrology */
		/* ---------------- */
		if ( list_rewind( hydrology_station_datatype_list ) )
		{
			do {
				station_datatype =
					list_get_pointer(
					 hydrology_station_datatype_list );

				measurement =
					hash_table_get_pointer(
						station_datatype->
							measurement_hash_table,
						date_space_time );

				if ( measurement && !measurement->is_null )
				{
					sprintf(buffer,
						"%.3lf",
						measurement->
							measurement_value );
				}
				else
				if ( measurement )
				{
					strcpy( buffer, "null" );
				}
				else
				{
					strcpy( buffer, "missing" );
				}

				html_table_set_data(
					html_table->data_list,
					strdup( buffer ) );

			} while( list_next(
					hydrology_station_datatype_list ) );

		} /* If */

		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
		}

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

	} while( list_next( date_space_time_key_list ) );	

	html_table_close();

} /* output_merged_waterquality_table() */

