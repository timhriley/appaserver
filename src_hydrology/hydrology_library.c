/* $APPASERVER_HOME/src_hydrology/hydrology_library.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hydrology_library.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "aggregate_statistic.h"
#include "application.h"
#include "julian.h"
#include "date.h"
#include "piece.h"
#include "station.h"
#include "datatype.h"
#include "document.h"

char *hydrology_library_get_flow_units( char *application_name )
{
	return hydrology_library_get_datatype_units(
				application_name,
				"flow" );
}

char *hydrology_library_get_stage_units( char *application_name )
{
	return hydrology_library_get_datatype_units(
				application_name,
				"stage" );
}

char *hydrology_library_get_datatype_units(	char *application_name,
						char *datatype_name )
{
	boolean bar_graph;

	return hydrology_library_get_units_string(
						&bar_graph,
						application_name,
						datatype_name );
}

char *hydrology_library_get_expected_count_list_string(
					char *application,
					char *station,
					char *datatype,
					char delimiter )
{
	char sys_string[ 1024 ];
	char *table_name;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char list_string[ 2048 ];
	char *ptr = list_string;
	char *select =
"begin_measurement_date,begin_measurement_time,expected_count";

	table_name = get_table_name( application, "data_collection_frequency" );

	sprintf( sys_string,
		 "echo \"select %s					 "
		 "       from %s					 "
		 "	 where station = '%s'				 "
		 "	   and datatype = '%s'				 "
		 "	 order by %s;\"					|"
		 "sql.e '%c'						 ",
		 select,
		 table_name,
		 station,
		 datatype,
		 select,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		if ( ptr == list_string )
			ptr += sprintf( ptr, "%s", input_buffer );
		else
			ptr += sprintf( ptr, "%c%s", delimiter, input_buffer );
	}

	*ptr = '\0';
	pclose( input_pipe );
	return strdup( list_string );
} /* hydrology_library_get_expected_count_list_string() */

char *hydrology_library_get_units_string(	boolean *bar_graph,
						char *application_name,
						char *datatype_name )
{
	return datatype_get_units_string(
			bar_graph,
			application_name,
			datatype_name );

} /* hydrology_library_get_units_string() */

char hydrology_library_get_aggregation_sum(
				char *application_name,
				char *datatype_name )
{
	static LIST *datatype_list = {0};
	DATATYPE *datatype;

	if ( !datatype_list )
	{
		datatype_list = datatype_list_get( application_name );
	}

	if ( ( datatype = datatype_list_seek(
				datatype_list,
				datatype_name ) ) )
	{
		return datatype->aggregation_sum;
	}
	else
	{
		return 'n';
	}
} /* hydrology_library_get_aggregation_sum() */

LIST *based_on_datatype_get_aggregate_statistic_list(
				char *application_name,
				LIST *datatype_name_list )
{
	enum aggregate_statistic *aggregate_statistic;
	LIST *aggregate_statistic_list = list_new();
	char *datatype_name;

	if ( !list_rewind( datatype_name_list ) )
		return aggregate_statistic_list;

	do {
		datatype_name = list_get_pointer( datatype_name_list );

		aggregate_statistic =
			aggregate_statistic_new_aggregate_statistic();

		*aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype_name,
				aggregate_statistic_none );

		list_append_pointer(	aggregate_statistic_list,
					aggregate_statistic );
	} while( list_next( datatype_name_list ) );

	return aggregate_statistic_list;
} /* based_on_datatype_get_aggregate_statistic_list() */

enum aggregate_statistic based_on_datatype_get_aggregate_statistic(
				char *application_name,
				char *datatype,
				enum aggregate_statistic aggregate_statistic )
{
	char *aggregate_statistic_string;

	aggregate_statistic_string =
		aggregate_statistic_get_string(
			aggregate_statistic );

	if ( !aggregate_statistic_string
	||   !*aggregate_statistic_string
	||   strcmp( aggregate_statistic_string, "value" ) == 0
	||   strcmp( aggregate_statistic_string, "aggregate_statistic" ) == 0 )
	{
		if ( hydrology_library_get_aggregation_sum(
				application_name,
				datatype ) )
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
	}

	return aggregate_statistic;

} /* based_on_datatype_get_aggregate_statistic() */

double hydrology_library_get_measurement_value(
					boolean *is_null,
					char *application_name,
					char *station,
					char *datatype,
					char *measurement_date,
					char *measurement_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *results;

	*is_null = 0;

	sprintf( where,
		 "station = '%s' and "
		 "datatype = '%s' and "
		 "measurement_date = '%s' and "
		 "measurement_time = '%s'",
		 station,
		 datatype,
		 measurement_date,
		 measurement_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=measurement_value	"
		 "			folder=measurement		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results || timlib_strcmp( results, "null" ) == 0 )
	{
		*is_null = 1;
		return 0.0;
	}
	else
	{
		return atof( results );
	}
} /* hydrology_library_get_measurement_value() */

double hydrology_library_get_latest_before_measurement_value(
					char *application_name,
					char *station,
					char *datatype,
					char *begin_date )
{
	char *latest_date;
	char *latest_time;
	char sys_string[ 1024 ];
	char where[ 512 ];

	latest_date = hydrology_library_get_latest_measurement_date_time(
				&latest_time,
				application_name,
				station,
				datatype,
				begin_date );

	if ( !latest_date || !*latest_date ) return 0.0;

	sprintf( where,
		 "station = '%s' and "
		 "datatype = '%s' and "
		 "measurement_date = '%s' and "
		 "measurement_time = '%s'",
		 station,
		 datatype,
		 latest_date,
		 latest_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=measurement_value	"
		 "			folder=measurement		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return atof( pipe2string( sys_string ) );
} /* hydrology_library_get_latest_before_measurement_value() */

char *hydrology_library_get_latest_measurement_date_time(
			char **latest_measurement_time,
			char *application_name,
			char *station,
			char *datatype,
			char *begin_date )
{
	char *table_name;
	char *latest_date;
	char sys_string[ 1024 ];
	char where[ 512 ];

	table_name = get_table_name( application_name, "measurement" );

	sprintf( where,
		 "station = '%s' and "
		 "datatype = '%s' and "
		 "measurement_date < '%s'",
		 station,
		 datatype,
		 begin_date );

	sprintf( sys_string,
		 "echo \"select max( measurement_date )			 "
		 "	 from %s					 "
		 "	 where %s;\"					|"
		 "sql							 ",
		 table_name,
		 where );

	latest_date = pipe2string( sys_string );

	if ( !*latest_date ) return (char *)0;

	sprintf( where,
		 "station = '%s' and "
		 "datatype = '%s' and "
		 "measurement_date = '%s'",
		 station,
		 datatype,
		 latest_date );

	sprintf( sys_string,
		 "echo \"select max( measurement_time )			 "
		 "	 from %s					 "
		 "	 where %s;\"					|"
		 "sql							 ",
		 table_name,
		 where );

	*latest_measurement_time = pipe2string( sys_string );
	return latest_date;
} /* hydrology_library_get_latest_measurement_date_time() */

void hydrology_library_get_period_of_record_begin_end_dates(
				char **begin_date_string,
				char **end_date_string,
				char *application_name,
				char *station,
				char *datatype )
{
	*begin_date_string =
		hydrology_library_get_period_of_record_begin_date(
			application_name,
			station,
			datatype );

	*end_date_string =
		hydrology_library_get_period_of_record_end_date(
			application_name,
			station,
			datatype );

} /* hydrology_library_get_period_of_record_begin_end_dates() */

char *hydrology_library_get_period_of_record_begin_date(
				char *application_name,
				char *station,
				char *datatype )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *select;

	if ( datatype && *datatype && strcmp( datatype, "datatype" ) != 0 )
	{
		sprintf(	where_clause,
				"station in (%s) and datatype in (%s)",
				timlib_get_in_clause( station ),
				timlib_get_in_clause( datatype ) );
	}
	else
	{
		sprintf(	where_clause,
				"station in (%s)",
				timlib_get_in_clause( station ) );
	}

	select = "min(measurement_date)";

	sprintf(	sys_string,
			"get_folder_data	application=%s		"
			"			folder=measurement	"
			"			select=\"%s\"		"
			"			where=\"%s\"		"
			"			order=none		",
			application_name,
			select,
			where_clause );

	return pipe2string( sys_string );

} /* hydrology_library_get_period_of_record_begin_date() */

char *hydrology_library_get_period_of_record_end_date(
				char *application_name,
				char *station,
				char *datatype )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *select;

	if ( datatype && *datatype && strcmp( datatype, "datatype" ) != 0 )
	{
		sprintf(	where_clause,
				"station in (%s) and datatype in (%s)",
				timlib_get_in_clause( station ),
				timlib_get_in_clause( datatype ) );
	}
	else
	{
		sprintf(	where_clause,
				"station in (%s)",
				timlib_get_in_clause( station ) );
	}

	select = "max(measurement_date)";

	sprintf(	sys_string,
			"get_folder_data	application=%s		"
			"			folder=measurement	"
			"			select=\"%s\"		"
			"			where=\"%s\"		"
			"			order=none		",
			application_name,
			select,
			where_clause );

	return pipe2string( sys_string );

} /* hydrology_library_get_period_of_record_end_date() */

void hydrology_library_with_list_get_clean_begin_end_date(
					char **begin_date,
					char **end_date,
					char *application_name,
					LIST *station_name_list,
					LIST *datatype_name_list )
{
	char *station_name;
	char *datatype_name;
	char *local_begin_date;
	char *local_end_date;

	if (	!list_length( station_name_list )
	||      ( list_length( station_name_list ) !=
		list_length( datatype_name_list ) ) )
	{
		return;
	}

	if ( !*begin_date
	||   !**begin_date
	||   strcmp( *begin_date, "begin_date" ) == 0 )
	{
		char minimum_begin_date[ 16 ];

		*minimum_begin_date = '\0';

		list_rewind( station_name_list );
		list_rewind( datatype_name_list );

		do {
			station_name = list_get_pointer( station_name_list );
			datatype_name = list_get_pointer( datatype_name_list );

			local_begin_date =
			     hydrology_library_get_period_of_record_begin_date(
				application_name,
				station_name,
				datatype_name );

			if ( local_begin_date
			&&   *local_begin_date )
			{
				if ( !*minimum_begin_date )
				{
					strcpy(	minimum_begin_date,
						local_begin_date );
				}
				else
				{
					if ( strcmp(local_begin_date,
						    minimum_begin_date ) < 0 )
					{
						strcpy(	minimum_begin_date,
							local_begin_date );
					}
				}

				free( local_begin_date );
			}

			list_next( datatype_name_list );

		} while( list_next( station_name_list ) );

		*begin_date = strdup( minimum_begin_date );
	}

	if ( !*end_date
	||   !**end_date
	||   strcmp( *end_date, "end_date" ) == 0 )
	{
		char maximum_end_date[ 16 ];

		*maximum_end_date = '\0';

		list_rewind( station_name_list );
		list_rewind( datatype_name_list );

		do {
			station_name = list_get_pointer( station_name_list );
			datatype_name = list_get_pointer( datatype_name_list );

			local_end_date =
				hydrology_library_get_period_of_record_end_date(
					application_name,
					station_name,
					datatype_name );

			if ( local_end_date
			&&   *local_end_date )
			{
				if ( !*maximum_end_date )
				{
					strcpy(	maximum_end_date,
						local_end_date );
				}
				else
				{
					if ( strcmp(local_end_date,
						    maximum_end_date ) > 0 )
					{
						strcpy(	maximum_end_date,
							local_end_date );
					}
				}

				free( local_end_date );
			}

			list_next( datatype_name_list );

		} while( list_next( station_name_list ) );

		*end_date = strdup( maximum_end_date );
	}

} /* hydrology_library_with_list_get_clean_begin_end_date() */

void hydrology_library_get_clean_begin_end_date(
					char **begin_date,
					char **end_date,
					char *application_name,
					char *station,
					char *datatype )
{
	int seeked_begin_date = 0;

	if ( !station || !*station ) return;

	if ( !*begin_date
	||   !**begin_date
	||   strcmp( *begin_date, "begin_date" ) == 0 )
	{
		*begin_date =
			hydrology_library_get_period_of_record_begin_date(
				application_name,
				station,
				datatype );

		seeked_begin_date = 1;
	}

	if ( !*end_date
	||   !**end_date
	||   strcmp( *end_date, "end_date" ) == 0 )
	{
		if ( seeked_begin_date )
		{
			*end_date =
				hydrology_library_get_period_of_record_end_date(
					application_name,
					station,
					datatype );
		}
		else
		{
			*end_date = *begin_date;
		}
	}

} /* hydrology_library_get_clean_begin_end_date() */

void hydrology_library_get_clean_begin_end_time(
					char **begin_time_string,
					char **end_time_string )
{
	if ( !*begin_time_string
	||   !**begin_time_string
	||   strcmp( *begin_time_string, "begin_time" ) == 0 )
	{
		*begin_time_string = "0000";
	}

	if ( !*end_time_string
	||   !**end_time_string
	||   strcmp( *end_time_string, "end_time" ) == 0 )
	{
		*end_time_string = "2359";
	}

} /* hydrology_library_get_clean_begin_end_time() */

boolean hydrology_library_get_bar_graph(	char *application_name,
						char *datatype_name )

{
	char sys_string[ 1024 ];
	char *table_name;
	char *results;

	table_name = get_table_name( application_name, "datatype" );

	sprintf( sys_string,
		 "echo \"select bar_graph_yn			 "
		 "	 from %s				 "
		 "	 where datatype = '%s';\"		|"
		 "sql						 ",
		 table_name,
		 datatype_name );

	results = pipe2string( sys_string );
	if ( !results || !*results )
		return 0;
	else
		return (*results == 'y' );
} /* hydrology_library_get_bar_graph() */

void hydrology_library_get_title(
				char *title,
				char *sub_title,
				enum validation_level validation_level,
				enum aggregate_statistic aggregate_statistic,
				enum aggregate_level aggregate_level,
				char *station_name,
				char *datatype,
				char *begin_date_string,
				char *end_date_string,
				char accumulate_yn )
{
	char title_validation_string[ 256 ];
	char title_accumulate_string[ 16 ];
	char buffer[ 256 ];
	char title_aggregate_statistic_string[ 256 ];
	char station_title[ 256 ];

	if ( !station_name ) station_name = "";

	if ( datatype && *datatype )
	{
		sprintf( station_title, "%s/%s", station_name, datatype );
		format_initial_capital( station_title, station_title );
	}
	else
	{
		strcpy( station_title, station_name );
	}

	if ( accumulate_yn == 'y' )
		strcpy( title_accumulate_string, "(Accumulated)" );
	else
		*title_accumulate_string = '\0';

	if ( validation_level == provisional
	||   validation_level == validated )
	{
		sprintf(	title_validation_string,
				"(%s)",
				validation_level_get_string(
					validation_level ) );
	}
	else
	{
		*title_validation_string = '\0';
	}

	if ( aggregate_statistic != aggregate_statistic_none )
	{
		sprintf(	title_aggregate_statistic_string,
				"%s",
				format_initial_capital(
					buffer,
					aggregate_statistic_get_string(
						aggregate_statistic ) ) );
	}
	else
	{
		*title_aggregate_statistic_string = '\0';
	}

	if ( aggregate_level != aggregate_level_none )
	{
		sprintf(title, "%s %s %s%s Values %s", 
			station_title,
			format_initial_capital(	buffer,
					aggregate_level_get_string(
						aggregate_level ) ),
			title_aggregate_statistic_string,
			title_accumulate_string,
			title_validation_string );
	}
	else
	{
		sprintf(title, "%s %s %s%s Values %s", 
			station_title,
			format_initial_capital(	buffer,
					aggregate_level_get_string(
						aggregate_level ) ),
			title_aggregate_statistic_string,
			title_accumulate_string,
			title_validation_string );
	}

	sprintf( sub_title, "Beginning: %s Ending: %s",
		 begin_date_string,
		 end_date_string );

} /* hydrology_library_get_title() */

int get_latest_measurements_per_day(	char *application,
					char *station,
					char *datatype )
{
	char buffer[ 1024 ];
	char *table_name;
	char *results;
	char latest_begin_measurement_date[ 16 ];
	char latest_begin_measurement_time[ 16 ];
	char measurement_time_where[ 128 ];

	table_name = get_table_name( application, "data_collection_frequency" );

	sprintf( buffer,
		 "echo \"	select	begin_measurement_date,		 "
		 "			begin_measurement_time		 "
		 "		from %s					 "
		 "		where station = '%s'			 "
		 "		  and datatype = '%s';\"		|"
		 "sql '^'						|"
		 "sort							|"
		 "tail -1						 ",
		 table_name,
		 station,
		 datatype );
	if ( ! ( results = pipe2string( buffer ) ) ) return 0;

	piece( latest_begin_measurement_date, '^', results, 0 );
	piece( latest_begin_measurement_time, '^', results, 1 );

	if ( !*latest_begin_measurement_time )
	{
		strcpy( measurement_time_where,
			"(begin_measurement_time = ''"
			"or begin_measurement_time is null)" );
	}
	else
	{
		sprintf(measurement_time_where,
			"begin_measurement_time = '%s'",
			latest_begin_measurement_time );
	}

	sprintf( buffer,
		 "echo \"	select expected_count 			 "
		 "		from %s					 "
		 "		where station = '%s'			 "
		 "		  and datatype = '%s'			 "
		 "		  and begin_measurement_date = '%s'	 "
		 "and %s;\"						|"
		 "sql							 ",
		 table_name,
		 station,
		 datatype,
		 latest_begin_measurement_date,
		 measurement_time_where );
	results = pipe2string( buffer );
	if ( !results )
		return 0;
	else
		return atoi( results );

} /* get_latest_measurements_per_day() */

int hydrology_library_insert_null_measurements(
				char *station,
				char *datatype,
				char *begin_date,
				char *begin_time,
				char *end_date,
				char *end_time,
				char *reason_value_missing,
				int minutes_offset,
				char really_yn,
				char *process_title,
				boolean with_table_output,
				char *application_name )
{
	char sys_string[ 1024 ];
	char buffer[ 256 ];
	char input_buffer[ 128 ];
	char date_string[ 16 ];
	char time_string[ 16 ];
	FILE *input_pipe;
	FILE *insert_pipe = {0};
	int count = 0;
	FILE *display_pipe = {0};
	char *status_string;
	char *output_process_string;
	char *table_name;
	DATE *minutes_offset_date;

	if ( !reason_value_missing )
	{
		reason_value_missing = "";
	}
	else
	if ( strcmp(	reason_value_missing,
			"reason_value_missing" ) == 0 )
	{
		*reason_value_missing = '\0';
	}

	table_name = get_table_name( application_name, "measurement" );

	sprintf(sys_string,
		"data_collection_frequency_list %s %s %s real_time %s:%s %s:%s",
		application_name,
		station,
		datatype,
		begin_date,
		(begin_time) ? begin_time : "",
		end_date,
		(end_time) ? end_time : "" );

	input_pipe = popen( sys_string, "r" );

	if ( with_table_output )
		output_process_string = "html_paragraph_wrapper";
	else
		output_process_string = "cat >/dev/null";

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"insert_statement.e %s %s | sql.e 2>&1 | %s",
		 	table_name,
		 	NULL_VALUE_COLUMNS_TO_INSERT,
		 	output_process_string );
		insert_pipe = popen( sys_string, "w" );
	}

	if ( with_table_output )
	{
		char *column_list_string =
			"Station,Datatype,Date,Time,Reason,Status";

		sprintf(sys_string,
			"queue_top_bottom_lines.e 50	|"
			"html_table.e \"%s\" %s '|'	 ",
		 	process_title,
			column_list_string );
		display_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, '^', input_buffer, 0 );
		piece( time_string, '^', input_buffer, 1 );

		if ( minutes_offset )
		{
			minutes_offset_date =
				date_yyyy_mm_dd_hhmm_new(
					date_string,
					time_string );

			date_increment_minutes(
				minutes_offset_date,
				minutes_offset );

			date_get_yyyy_mm_dd(
				date_string,
				minutes_offset_date );

			date_get_hhmm(
				time_string,
				minutes_offset_date );

			date_free( minutes_offset_date );
		}

		sprintf(buffer,
		 	"%s|%s|%s|%s|%s",
		 	station,
		 	datatype,
		 	date_string,
		 	time_string,
		 	reason_value_missing );

		if ( really_yn == 'y' )
		{
			fprintf( insert_pipe, "%s\n", buffer );
		}
		else
		{
			if ( with_table_output )
			{
				fprintf( display_pipe, "%s", buffer );
				fprintf(display_pipe, "|NOT inserted\n" );
			}
		}

		count++;

	} /* while() */

	pclose( input_pipe );

	if ( really_yn == 'y' ) pclose( insert_pipe );

	if ( with_table_output )
	{
		status_string = (really_yn == 'y') ? "Complete" : "";
		fprintf(display_pipe,
			"Count: %d|||||%s\n",
			count,
			status_string );
		pclose( display_pipe );
	}
	return count;

} /* hydrology_library_insert_null_measurements() */

char *hydrology_library_get_datatype_units_display(
				char *application_name,
				char *datatype,
				char *datatype_units,
				char *datatype_units_converted,
				enum aggregate_statistic aggregate_statistic )
{
	char *units_display;

	if ( aggregate_statistic == non_zero_percent )
	{
		return "Percent";
	}
	else
	if ( !datatype_units_converted
	||   !*datatype_units_converted )
	{
		return datatype_units;
	}

	if ( strcmp( datatype_units_converted, "units_converted" ) != 0
	&&   !hydrology_library_can_convert_to_units(
			application_name,
			datatype,
			datatype_units_converted ) )
	{
		return (char *)0;
	}

	if ( datatype_units_converted
	&&   *datatype_units_converted
	&&   strcmp( datatype_units_converted, "units_converted" ) != 0 )
	{
		units_display = datatype_units_converted;
	}
	else
	{
		units_display = datatype_units;
	}
	return units_display;
} /* hydrology_library_get_datatype_units_display() */

char *hydrology_library_get_output_invalid_units_error(
					char *datatype,
					char *units,
					char *units_converted )
{
	static char message[ 128 ];
	sprintf( message,
"<h3>Error: Datatype of %s (stored in %s) cannot convert to units of %s</h3>",
		 datatype,
		 units,
		 units_converted );
	return message;
} /* hydrology_library_get_output_invalid_units_error() */

boolean hydrology_library_can_convert_to_units(
			char *application_name,
			char *datatype,
			char *units_converted )
{
	char sys_string[ 1024 ];
	char *datatype_table_name;
	char *units_converted_table_name;

	datatype_table_name =
		get_table_name( application_name, "datatype" );
	units_converted_table_name =
		get_table_name( application_name, "units_converted" );

	sprintf(sys_string,
		"echo \"	select count(*)				 "
		"		from %s,%s				 "
		"		where %s.units = %s.units		 "
		"		  and %s.datatype = '%s'		 "
		"		  and %s.units_converted = '%s';\"	|"
		"sql.e							 ",
		datatype_table_name,
		units_converted_table_name,
		datatype_table_name,
		units_converted_table_name,
		datatype_table_name,
		datatype,
		units_converted_table_name,
		units_converted );
	return atoi( pipe2string( sys_string ) );
} /* hydrology_library_can_convert_to_units() */

boolean hydrology_library_some_measurements_validated(
			char *application_name,
			char *where_clause,
			char *station,
			char *datatype,
			char *begin_measurement_date,
			char *end_measurement_date )
{
	char sys_string[ 2048 ];
	char local_where_clause[ 2048 ];
	char *provisional_where;
	char *results;

	provisional_where =
		/* ----------------------- */
		/* Returns program memory. */
		/* ----------------------- */
		hydrology_library_provisional_where(
				provisional );

	sprintf(local_where_clause,
		"station = '%s' and				"
		"datatype = '%s' and				"
		"measurement_date between '%s' and '%s' 	"
		"%s						",
		station,
		datatype,
		begin_measurement_date,
		end_measurement_date,
		provisional_where );

	sprintf( sys_string,
		 "get_folder_data		application=%s		"
		 "				select=count		"
		 "				folder=measurement	"
		 "				where=\"%s\"		",
		 application_name,
		 local_where_clause );

	results = pipe2string( sys_string );

	return (boolean)atoi( results );

} /* hydrology_library_some_measurements_validated() */

boolean hydrology_library_get_drift_ratio_variables(
					DICTIONARY *parameter_dictionary,
					double *first_multiplier_double,
					double *last_multiplier_double,
					char *application_name,
					double first_value_double,
					double last_value_double,
					char *station,
					char *datatype,
					char *begin_date_string,
					char *begin_time_string,
					char *end_date_string,
					char *end_time_string )
{
	char value_string[ 16 ];
	double measurement_value;
	boolean is_null;

	if ( !parameter_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: null parameter_dictionary.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Get first_multiplier_double */
	/* --------------------------- */
	measurement_value =
		hydrology_library_get_measurement_value(
					&is_null,
					application_name,
					station,
					datatype,
					begin_date_string,
					begin_time_string );

	if ( is_null )
	{
		return 0;
	}
	else
	{
		if ( measurement_value < DRIFT_MEASUREMENT_DETECTION_LIMIT )
		{
			 measurement_value = DRIFT_MEASUREMENT_DETECTION_LIMIT;
		}

		*first_multiplier_double =
			first_value_double / measurement_value;

		sprintf( value_string, "%.3lf", *first_multiplier_double );

		dictionary_set_pointer(	parameter_dictionary,
					"first_multiplier",
					strdup( value_string ) );

		sprintf( value_string, "%.3lf", measurement_value );

		dictionary_set_pointer(	parameter_dictionary,
					"original_first_value",
					strdup( value_string ) );
	}

	/* Get last_multiplier_double */
	/* --------------------------- */
	measurement_value =
		hydrology_library_get_measurement_value(
					&is_null,
					application_name,
					station,
					datatype,
					end_date_string,
					end_time_string );

	if ( is_null )
	{
		return 0;
	}
	else
	{
		if ( measurement_value < DRIFT_MEASUREMENT_DETECTION_LIMIT )
		{
			measurement_value = DRIFT_MEASUREMENT_DETECTION_LIMIT;
		}

		*last_multiplier_double =
			last_value_double / measurement_value;

		sprintf( value_string, "%.3lf", *last_multiplier_double );

		dictionary_set_pointer(	parameter_dictionary,
					"last_multiplier",
					strdup( value_string ) );

		sprintf( value_string, "%.3lf", measurement_value );

		dictionary_set_pointer(	parameter_dictionary,
					"original_last_value",
					strdup( value_string ) );
	}

	return 1;

} /* hydrology_library_get_drift_ratio_variables() */

char *hydrology_library_get_ground_surface_elevation_string(
			double ground_surface_elevation_ft,
			boolean ground_surface_elevation_null )
{
	static char elevation_string[ 16 ];

	if ( ground_surface_elevation_null )
		return HYDROLOGY_LIBRARY_MISSING;
	else
	{
		sprintf( elevation_string,
			 "%.2lf",
			 ground_surface_elevation_ft );

		return elevation_string;
	}

} /* hydrology_library_get_ground_surface_elevation_string() */

void hydrology_library_output_station_table(
				char *application_name,
				char *station_name )
{
	STATION *station;
	char station_sys_string[ 1024 ];
	char *station_heading;
	FILE *output_pipe;
	char *justify_column_list_string;

	station = station_new( station_name );

	if ( !station_load(	
				&station->lat_nad83,
				&station->long_nad83,
				&station->agency,
				&station->ground_surface_elevation_ft,
				&station->ground_surface_elevation_null,
				&station->vertical_datum,
				&station->ngvd29_navd88_conversion,
				&station->station_type,
				&station->comments,
				application_name,
				station->station_name ) )
	{
		printf( "<p>Error: cannot find station = (%s)\n",
			station_name );
		document_close();
		exit( 0 );
	}

	station_heading =
"agency,ground_surface_elevation_ft,latitude,longitude,vertical_datum,NGVD29_NAVD88_conversion";

	justify_column_list_string = "left,right,right,right,left,right";

	sprintf( station_sys_string,
		 "html_table.e '^^Station %s' %s ',' %s",
		 station_name,
		 station_heading,
		 justify_column_list_string );

	output_pipe = popen( station_sys_string, "w" );

	fflush( stdout );

	fprintf( output_pipe,
		 "%s,%s,%.5lf,%.5lf,%s,%.3lf\n",
		 station->agency,
		 hydrology_library_get_ground_surface_elevation_string(
			station->ground_surface_elevation_ft,
			station->ground_surface_elevation_null ),
		 station->lat_nad83,
		 station->long_nad83,
		 (station->vertical_datum) ?
			station->vertical_datum : "",
		 station->ngvd29_navd88_conversion );

	pclose( output_pipe );

	fflush( stdout );
	printf( "<br>\n" );
	fflush( stdout );

} /* hydrology_library_output_station_table() */

void hydrology_library_output_station_text_filename(
				char *output_filename,
				char *application_name,
				char *station_name,
				boolean with_zap_file )
{
	FILE *output_file;
	char *mode;

	if ( with_zap_file )
		mode = "w";
	else
		mode = "a";

	if ( strcmp( output_filename, "stdout" ) == 0 )
	{
		output_file = stdout;
	}
	else
	if ( ! ( output_file = fopen( output_filename, mode ) ) )
	{
		printf( "<p>Error: cannot open file = (%s)\n",
			output_filename );
		document_close();
		exit( 0 );
	}

	hydrology_library_output_station_text_file(
				output_file,
				application_name,
				station_name );

	if ( strcmp( output_filename, "stdout" ) != 0 )
		fclose( output_file );

} /* hydrology_library_output_station_text_filename() */

void hydrology_library_output_station_text_file(
				FILE *output_file,
				char *application_name,
				char *station_name )
{
	STATION *station;

	station = station_new( station_name );

	if ( !station_load(	
				&station->lat_nad83,
				&station->long_nad83,
				&station->agency,
				&station->ground_surface_elevation_ft,
				&station->ground_surface_elevation_null,
				&station->vertical_datum,
				&station->ngvd29_navd88_conversion,
				&station->station_type,
				&station->comments,
				application_name,
				station->station_name ) )
	{
		printf( "<p>Error: cannot find station = (%s)\n",
			station_name );
		document_close();
		exit( 0 );
	}

	fprintf( output_file,
"#station: %s\n#agency: %s\n#ground surface elevation ft: %s\n#latitude: %.5lf\n#longitude: %.5lf\n#vertical datum: %s\n#conversion: %.3lf\n\n",
		 station_name,
		 station->agency,
		 hydrology_library_get_ground_surface_elevation_string(
			station->ground_surface_elevation_ft,
			station->ground_surface_elevation_null ),
		 station->lat_nad83,
		 station->long_nad83,
		 (station->vertical_datum) ?
			station->vertical_datum : "",
		 station->ngvd29_navd88_conversion );

	fflush( output_file );

} /* hydrology_library_output_station_text_file() */

void hydrology_library_output_data_collection_frequency_table(
				char *application_name,
				char *station_name,
				char *datatype_name )
{
	char sys_string[ 1024 ];
	char *heading;
	FILE *output_pipe;
	char *justify_column_list_string;
	char *data_collection_frequency_list_string;
	char delimiter = '|';
	char piece_buffer[ 128 ];
	int p;

	data_collection_frequency_list_string =
		hydrology_library_get_expected_count_list_string(
				application_name,
				station_name,
				datatype_name,
				delimiter );

	heading =
"begin_measurement_date,begin_measurement_time,expected_count_per_day";

	justify_column_list_string = "left,left,right";

	sprintf( sys_string,
		 "html_table.e '^^Data Collection Frequency %s/%s' %s '%c' %s",
		 station_name,
		 datatype_name,
		 heading,
		 FOLDER_DATA_DELIMITER,
		 justify_column_list_string );

	output_pipe = popen( sys_string, "w" );

	for(	p = 0;
		piece(	piece_buffer,
			delimiter,
			data_collection_frequency_list_string,
			p );
		p++ )
	{
		fprintf( output_pipe, "%s\n", piece_buffer );
	}

	pclose( output_pipe );

	fflush( stdout );
	printf( "<br>\n" );
	fflush( stdout );

} /* hydrology_library_output_data_collection_frequency_table() */

void hydrology_library_output_data_collection_frequency_text_file(
				FILE *output_file,
				char *application_name,
				char *station_name,
				char *datatype_name )
{

	char *data_collection_frequency_list_string;
	char delimiter = '|';
	char piece_buffer[ 128 ];
	int p;

	data_collection_frequency_list_string =
		hydrology_library_get_expected_count_list_string(
				application_name,
				station_name,
				datatype_name,
				delimiter );

	for(	p = 0;
		piece(	piece_buffer,
			delimiter,
			data_collection_frequency_list_string,
			p );
		p++ )
	{
		fprintf( output_file, "#%s\n", piece_buffer );
	}
	fprintf( output_file, "\n" );

} /* hydrology_library_output_data_collection_frequency_text_file() */

boolean hydrology_library_get_begin_end_year(
		char **begin_year,
		char **end_year,
		char *year_expression )
{
	LIST *number_list;
	int *number_ptr;
	char buffer[ 8 ];

	number_list =
		list_usage_expression2number_list(
			year_expression );

	if ( !list_length( number_list ) ) return 0;

	number_ptr = list_get_first_pointer( number_list );
	sprintf( buffer, "%d", *number_ptr );
	*begin_year = strdup( buffer );

	number_ptr = list_get_last_pointer( number_list );
	sprintf( buffer, "%d", *number_ptr );
	*end_year = strdup( buffer );

	return 1;

} /* hydrology_library_get_begin_end_year() */

/* Returns program memory. */
/* ----------------------- */
char *hydrology_library_provisional_where(
				enum validation_level validation_level )
{
	if ( validation_level == provisional )
	{
		return " and last_validation_date is null";
	}
	else
	if ( validation_level == validated )
	{
		return " and last_validation_date is not null";
	}
	else
	{
		return " and 1 = 1";
	}

} /* hydrology_library_provisional_where() */

