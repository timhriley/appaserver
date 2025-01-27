/* generic_annual_comparison.c				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdlib.h>
#include "generic_annual_comparison.h"
#include "timlib.h"
#include "piece.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "date.h"
#include "list.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "process_generic_output.h"

/* Prototypes */
/* ---------- */
void parse_input_buffer(	char *measurement_date,
				char *measurement_value,
				char *input_buffer );

/* Operations */
/* ---------- */
ANNUAL_COMPARISON *annual_comparison_new(
				char *begin_month_day,
				char *end_month_day,
				int begin_year,
				int end_year )
{
	ANNUAL_COMPARISON *annual_comparison;

	if ( ! ( annual_comparison =
			(ANNUAL_COMPARISON *)
			calloc( 1, sizeof( ANNUAL_COMPARISON ) ) ) )
	{
		fprintf( stderr,
			 "Memory allocation error in %s/%s()/%d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	annual_comparison->begin_month_day = begin_month_day;
	annual_comparison->end_month_day = end_month_day;
	annual_comparison->begin_year = begin_year;
	annual_comparison->end_year = end_year;

	annual_comparison->day_range =
		annual_comparison_get_day_range(
				&annual_comparison->begin_date,
				begin_month_day,
				end_month_day );

	if ( !annual_comparison->day_range )
		return (ANNUAL_COMPARISON *)0;

	annual_comparison->year_range = (end_year + 1) - begin_year;

	annual_comparison->value_array =
		annual_comparison_value_array_new(
			annual_comparison->year_range,
			annual_comparison->day_range );

	return annual_comparison;
} /* annual_comparison_new() */

int annual_comparison_get_day_range(
				DATE **begin_date,
				char *begin_month_day,
				char *end_month_day )
{
	DATE *end_date;
	char date_string[ 16 ];
	int day_range;

	sprintf(date_string,
		"%d-%s",
		ANNUAL_COMPARISON_DEFAULT_YEAR,
		begin_month_day );

	*begin_date =
		date_yyyy_mm_dd_new(
			strdup( date_string ) );

	sprintf(date_string,
		"%d-%s",
		ANNUAL_COMPARISON_DEFAULT_YEAR,
		end_month_day );

	end_date =
		date_yyyy_mm_dd_new(
			strdup( date_string ) );

	day_range = date_subtract_days( end_date, *begin_date ) + 1;

	if ( day_range < 0 )
		return 0;
	else
		return day_range;

} /* annual_comparison_get_day_range() */

char ***annual_comparison_value_array_new(
			int year_range,
			int day_range )
{
	char ***value_array;
	int day;

	value_array = (char ***) calloc( day_range, sizeof( void ** ) );
	if ( !value_array )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	for ( day = 0; day < day_range; day++ )
	{
		value_array[ day ] =
			(char **) calloc( year_range, sizeof( char * ) );
		if ( !value_array[ day ] )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}
	return value_array;
} /* annual_comparison_value_array_new() */

boolean annual_comparison_set_value(
			char ***value_array,
			int *day_array_offset,
			int *year_array_offset,
			DATE *begin_date,
			int begin_year,
			int day_range,
			int year_range,
			char *date_yyyymmdd,
			char *value_string )
{
	int year;

	if ( !value_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: value_string is null. Perhaps memory allocation error in strdup() function.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*day_array_offset =
		annual_comparison_get_day_array_offset(
			begin_date,
			date_yyyymmdd );

	if ( *day_array_offset < 0 || *day_array_offset >= day_range )
	{
		return 0;
	}

	year = atoi( date_yyyymmdd );
	*year_array_offset = year - begin_year;

	if ( *year_array_offset < 0 || *year_array_offset >= year_range )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid year_array_offset = %d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			*year_array_offset );
		exit( 1 );
	}

	value_array[ *day_array_offset ][ *year_array_offset ] = value_string;

	return 1;
} /* annual_comparison_set_value() */

int annual_comparison_get_day_array_offset(
			DATE *begin_date,
			char *date_yyyymmdd )
{
	char date_string[ 16 ];
	char end_month_day[ 16 ];
	DATE *local_date;
	DATE *end_date;
	int array_offset;

	local_date =
		date_yyyy_mm_dd_new(
			date_yyyymmdd );

	sprintf( end_month_day,
		 "%d-%d",
		 date_get_month_number( local_date ),
		 date_get_month_day_number( local_date ) );

	date_free( local_date );

	sprintf(date_string,
		"%d-%s",
		ANNUAL_COMPARISON_DEFAULT_YEAR,
		end_month_day );

	end_date =
		date_yyyy_mm_dd_new(
			date_string );

	array_offset = date_subtract_days( end_date, begin_date );

	date_free( end_date );
	return array_offset;
} /* annual_comparison_get_day_array_offset() */

void annual_comparison_output(
				char ***value_array,
				int day_range,
				int year_range,
				DATE *begin_date,
				int begin_year )
{
	int day;
	int year;

	for( day = 0; day < day_range; day++ )
	{
		for( year = 0; year < year_range; year++ )
		{
			if ( value_array[ day ][ year ] )
			{
				printf( ".day %d-%d\n", 
					date_get_month_number( begin_date ),
					date_get_month_day_number(
						begin_date ) );

				printf( ".year %d,%s\n",
					begin_year + year,
					value_array[ day ][ year ] );
				fflush( stdout );
			}
		}
		date_increment( begin_date );
	}
} /* annual_comparison_output() */

void parse_input_buffer(	char *measurement_date,
				char *measurement_value,
				char *input_buffer )
{
	piece(	measurement_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		0 );

	piece(	measurement_value,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		1 );
}

int main( int argc, char **argv )
{
	ANNUAL_COMPARISON *annual_comparison;
	char *application_name;
	char *begin_month_day;
	char *end_month_day;
	char *begin_year;
	char *end_year;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char measurement_date[ 16 ];
	char measurement_value[ 16 ];
	char *where_clause;
	char begin_date[ 16 ];
	char *begin_date_ptr;
	char end_date[ 16 ];
	char *end_date_ptr;
	char select[ 128 ];
	char *process_name;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *post_dictionary_string;
	DICTIONARY *post_dictionary;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char aggregation_process[ 1024 ];
	char sort_process[ 128 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf(stderr,
"Usage: %s ignored begin_day_month end_day_month begin_year end_year process_name aggregate_level aggregate_statistic post_dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	begin_month_day = argv[ 2 ];
	end_month_day = argv[ 3 ];
	begin_year = argv[ 4 ];
	end_year = argv[ 5 ];
	process_name = argv[ 6 ];

	aggregate_level =
		aggregate_level_get_aggregate_level(
				argv[ 7 ] );

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		aggregate_level = daily;
	}

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
				argv[ 8 ],
				aggregate_level );

	post_dictionary_string = argv[ 9 ];

	if ( ! ( post_dictionary = 
		dictionary_index_string2dictionary(
			post_dictionary_string ) ) )
	{
		fprintf( stderr,
"Error in %s: invalid dictionary. It needs to contain both '&' and '='.\n",
			 post_dictionary_string );
		exit( 1 );
	}

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_STARTING_LABEL );

	process_generic_output =
		process_generic_output_new(
			application_name,
			process_name,
			(char *)0 /* process_set_name */,
			0 /* accumulate_flag */ );

	process_generic_output->value_folder->datatype =
		process_generic_datatype_new(
			application_name,
			process_generic_output->
				value_folder->
					foreign_folder->
						foreign_attribute_name_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						datatype_folder_name,
			process_generic_output->
				value_folder->
					datatype_folder->
						primary_key_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_aggregation_sum,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_bar_graph,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_scale_graph_zero,
			process_generic_output->
				value_folder->
					units_folder_name,
			post_dictionary,
			0 /* dictionary_index */ );

	if ( ! ( annual_comparison =
		annual_comparison_new(
			begin_month_day,
			end_month_day,
			atoi( begin_year ),
			atoi( end_year ) ) ) )
	{
		exit( 0 );
	}

	sprintf( begin_date, "%s-%s", begin_year, begin_month_day );
	begin_date_ptr = begin_date;
	sprintf( end_date, "%s-%s", end_year, end_month_day );
	end_date_ptr = end_date;

	where_clause = process_generic_output_get_dictionary_where_clause(
			&begin_date_ptr,
			&end_date_ptr,
			application_name,
			process_generic_output,
			post_dictionary,
			1 /* with_set_dates */,
			process_generic_output->
				value_folder->
				value_folder_name );

	sprintf( select,
		 "%s,%s",
		 process_generic_output->
			value_folder->
			date_attribute_name,
		 process_generic_output->
			value_folder->
			value_attribute_name );

	if ( aggregate_level == real_time )
	{
		strcpy( aggregation_process, "cat" );
		strcpy( sort_process, "sort" );
	}
	else
	{
		sprintf(aggregation_process, 
			"real_time2aggregate_value.e %s %d %d %d '%c' %s %c %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 0 /* date_piece */,
			 -1 /* time_piece */,
			 1 /* value_piece */,
			 FOLDER_DATA_DELIMITER,
			 aggregate_level_get_string( aggregate_level ),
			 'n' /* append_low_high_flag */,
			 end_date );

		strcpy( sort_process, "sort -r" );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		|"
		 "%s						|"
		 "%s						|"
		 "cat						 ",
		 application_name,
		 select,
		 process_generic_output->value_folder->value_folder_name,
		 where_clause,
		 sort_process,
		 aggregation_process );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		parse_input_buffer(
				measurement_date,
				measurement_value,
				input_buffer );

		annual_comparison_set_value(
			annual_comparison->value_array,
			&annual_comparison->day_array_offset,
			&annual_comparison->year_array_offset,
			annual_comparison->begin_date,
			annual_comparison->begin_year,
			annual_comparison->day_range,
			annual_comparison->year_range,
			measurement_date,
			strdup( measurement_value ) );
	}

	pclose( input_pipe );

	annual_comparison_output(
			annual_comparison->value_array,
			annual_comparison->day_range,
			annual_comparison->year_range,
			annual_comparison->begin_date,
			annual_comparison->begin_year );

	return 0;
} /* main() */

