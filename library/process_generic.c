/* $APPASERVER_HOME/library/process_generic.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver process_generic ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "query.h"
#include "dictionary.h"
#include "attribute.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "aggregate_statistic.h"
#include "aggregate_level.h"
#include "document.h"
#include "folder.h"
#include "list_usage.h"
#include "appaserver.h"
#include "query.h"
#include "environ.h"
#include "appaserver_library.h"
#include "process_generic.h"

char *process_generic_process_name(
			char *process_set_name,
			DICTIONARY *post_dictionary )
{
	char key[ 128 ];

	sprintf( key, "%s_0", process_set_name );
	return dictionary_get_pointer( post_dictionary, key );
}

char *process_generic_units_label(
			char *units,
			char *units_converted,
			enum aggregate_statistic aggregate_statistic )
{
	static char units_label[ 128 ];

	*units_label = '\0';

	if ( aggregate_statistic == non_zero_percent )
		strcpy( units_label, "(percent)" );
	else
	if ( units_converted && *units_converted )
		sprintf( units_label, "(%s)", units_converted );
	else
	if (	units
	&& 	*units
	&&	( aggregate_statistic != aggregate_statistic_none ) )
	{
		sprintf(units_label,
			"%s (%s)",
			aggregate_statistic_get_string( aggregate_statistic ),
			units );
	}
	else
	if ( units && *units )
		sprintf( units_label, "(%s)", units );
	else
		*units_label = '\0';

	return units_label;
}

char *process_generic_date_where(
			char *date_attribute_name,
			char *begin_date,
			char *end_date )
{
	static char where[ 256 ];

	if ( begin_date && *begin_date
	&&   end_date && *end_date )
	{
		sprintf(where,
			"%s >= '%s' and %s <= '%s'",
			date_attribute_name,
			begin_date,
			date_attribute_name,
			end_date );
	}
	else
	if ( begin_date && *begin_date )
	{
		sprintf(where,
			"%s >= '%s'",
			date_attribute_name,
			begin_date );
	}
	else
	if ( end_date && *end_date )
	{
		sprintf(where,
			"%s <= '%s'",
			date_attribute_name,
			end_date );
	}
	else
	{
		strcpy( where, "1 = 1" );
	}

	return where;
}

PROCESS_GENERIC_DATATYPE *process_generic_datatype_calloc(
			void )
{
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;

	if ( ! ( process_generic_datatype =
			calloc( 1,
				sizeof( PROCESS_GENERIC_DATATYPE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_generic_datatype;
}

PROCESS_GENERIC_DATATYPE *process_generic_datatype_parse(
			char *input_buffer )
{
	char piece_buffer[ 128 ];
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;

	if ( !input_buffer || !*input_buffer )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: input_buffer is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_generic_datatype = process_generic_datatype_calloc();

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 0 );
	process_generic_datatype->datatype_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 1 );
	process_generic_datatype->aggregation_sum = *piece_buffer == 'y';

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 2 );
	process_generic_datatype->bar_graph = *piece_buffer == 'y';

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 3 );
	process_generic_datatype->scale_graph_zero = *piece_buffer == 'y';

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 4 );
	process_generic_datatype->unit = strdup( piece_buffer );

	return process_generic_datatype;
}

PROCESS_GENERIC_DATATYPE *process_generic_datatype_fetch(
			char *datatype_folder_name,
			char *datatype_attribute_name,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			boolean datatype_exists_unit,
			DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;

	if ( !datatype_folder_name
	||   !datatype_attribute_name )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: empty datatype_folder_name, datatype_attribute_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_generic_datatype = process_generic_datatype_calloc();

	process_generic_datatype->datatype_folder_name = datatype_folder_name;

	process_generic_datatype->datatype_attribute_name =
		datatype_attribute_name;

	process_generic_datatype->exists_aggregation_sum =
		exists_aggregation_sum;

	process_generic_datatype->exists_bar_graph =
		exists_bar_graph;

	process_generic_datatype->exists_scale_graph_zero =
		exists_scale_graph_zero;

	process_generic_datatype->datatype_exists_unit =
		datatype_exists_unit;
 
	process_generic_datatype->post_dictionary =
		post_dictionary;

	process_generic_datatype->process_generic_datatype_select =
		process_generic_datatype_select(
			process_generic_datatype->datatype_attribute_name,
			process_generic_datatype->exists_aggregation_sum,
			process_generic_datatype->exists_bar_graph,
			process_generic_datatype->exists_scale_graph_zero,
			process_generic_datatype->datatype_exists_unit );

	process_generic_datatype->datatype_name =
		dictionary_fetch(
			process_generic_datatype->post_dictionary,
			process_generic_datatype->datatype_attribute_name );

	if ( !process_generic_datatype->datatype_name )
	{
		return (PROCESS_GENERIC_DATATYPE *)0;
	}

	process_generic_datatype->process_generic_datatype_where =
		process_generic_datatype_where(
			process_generic_datatype->datatype_attribute_name,
			process_generic_datatype->datatype_name );

	process_generic_datatype->process_generic_datatype_system_string =
		process_generic_datatype_system_string(
			process_generic_datatype->
				process_generic_datatype_select,
			process_generic_datatype->datatype_folder_name,
			process_generic_datatype->
				process_generic_datatype_where );

	return process_generic_datatype_parse(
			string_pipe_fetch(
				process_generic_datatype->
				     process_generic_datatype_system_string ) );
}

char *process_generic_datatype_system_string(
			char *select,
			char *datatype_folder_name,
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		datatype_folder_name,
		where );

	return strdup( system_string );
}

char *process_generic_datatype_select(
			char *datatype_attribute_name,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			boolean unit_datatype_folder )
{
	char select[ 1024 ];
	char *select_ptr = select;

	select_ptr += sprintf( select_ptr, "%s", datatype_attribute_name );

	if ( exists_aggregation_sum )
	{
		select_ptr += sprintf( select_ptr, ",aggregation_sum_yn" );
	}
	else
	{
		select_ptr += sprintf( select_ptr, ",''" );
	}

	if ( exists_bar_graph )
	{
		select_ptr += sprintf( select_ptr, ",bar_graph_yn" );
	}
	else
	{
		select_ptr += sprintf( select_ptr, ",''" );
	}

	if ( exists_scale_graph_zero )
	{
		select_ptr += sprintf( select_ptr, ",scale_graph_to_zero_yn" );
	}
	else
	{
		select_ptr += sprintf( select_ptr, ",''" );
	}

	if ( unit_datatype_folder )
	{
		select_ptr += sprintf( select_ptr, ",units" );
	}
	else
	{
		select_ptr += sprintf( select_ptr, ",''" );
	}

	return strdup( select );
}

char *process_generic_where(
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			char *date_where )
{
	char where[ 1024 ];
	char *query_where;

	if ( !date_where ) return (char *)0;

	query_where =
		query_simple_where(
			(char *)0 /* folder_name */,
			foreign_attribute_name_list
				/* where_attribute_name_list */,
			foreign_attribute_data_list
				/* where_attribute_data_list */,
			(LIST *)0 /* append_isa_attribute_list */ );

	if ( !query_where ) return (char *)0;

	sprintf(where,
		"%s and %s",
		query_where,
		date_where );

	return strdup( where );
}

char *process_generic_select(
			LIST *primary_attribute_name_list,
			char *value_attribute_name )
{
	char select[ 1024 ];

	sprintf(select,
		"%s,%s",
		list_display_delimited(
			primary_attribute_name_list,
			',' ),
		value_attribute_name );

	return strdup( select );
}

char *process_generic_system_string(
			char *select,
			char *value_folder_name,
			char *where,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			boolean accumulate,
			int date_piece,
			int time_piece,
			int value_piece,
			char *end_date,
			char *datatype_unit,
			char *foreign_folder_unit,
			char *units_converted )
{
	char system_string[ 2048 ];
	char real_time_process[ 512 ];
	char accumulation_process[ 512 ];
	char units_converted_process[ 512 ];
	int accumulate_piece = -1;

	if ( !select || !*select ) return (char *)0;
	if ( !value_folder_name || !*value_folder_name ) return (char *)0;
	if ( !where || !*where ) return (char *)0;

	if ( accumulate )
	{
		accumulate_piece = value_piece;
	}

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		strcpy( real_time_process, "cat" );
	}
	else
	{
		sprintf( real_time_process, 
	 "real_time2aggregate_value.e %s %d %d %d '%c' %s n %s",
	 		aggregate_statistic_string(
				aggregate_statistic ),
	 		date_piece,
	 		time_piece,
	 		value_piece,
			SQL_DELIMITER,
	 		aggregate_level_string( aggregate_level ),
			(end_date) ? end_date : "" );
	}

	if ( accumulate )
	{
		sprintf( accumulation_process, 
			 "accumulate.e %d '%c' append",
			 accumulate_piece,
			 SQL_DELIMITER );
	}
	else
	{
		strcpy( accumulation_process, "cat" );
	}

	if ( units_converted
	&&   ( datatype_unit || foreign_folder_unit ) )
	{
		char *unit;

		if ( *datatype_unit )
		{
			unit = datatype_unit;
		}
		else
		{
			unit = foreign_folder_unit;
		}

		sprintf( units_converted_process,
			 "measurement_convert_units.e '%s' '%s' %d '%c'",
			 unit,
			 units_converted,
			 value_piece,
			 SQL_DELIMITER );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	sprintf(system_string,
		"get_folder_data	application=%s		    "
		"			folder=%s		    "
		"			select=\"%s\"		    "
		"			where=\"%s\"		    "
		"			order=select		   |"
		"%s						   |"
		"%s						   |"
		"%s						   |"
		"cat						    ",
		environment_application_name(),
		value_folder_name,
		select,
		where,
		units_converted_process,
		real_time_process,
		accumulation_process );

	return strdup( system_string );
}

PROCESS_GENERIC_VALUE *process_generic_value_calloc( void )
{
	PROCESS_GENERIC_VALUE *process_generic_value;

	if ( ! ( process_generic_value =
			calloc( 1,
				sizeof( PROCESS_GENERIC_VALUE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_generic_value;
}

PROCESS_GENERIC_PARAMETER *process_generic_parameter_calloc( void )
{
	PROCESS_GENERIC_PARAMETER *process_generic_parameter;

	if ( ! ( process_generic_parameter =
			calloc( 1,
				sizeof( PROCESS_GENERIC_PARAMETER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_generic_parameter;
}

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_calloc( void )
{
	PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder;

	if ( ! ( process_generic_value_folder =
			calloc( 1,
				sizeof( PROCESS_GENERIC_VALUE_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_generic_value_folder;
}

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_parse(
			char *input )
{
	char piece_string[ 128 ];
	PROCESS_GENERIC_VALUE_FOLDER *value_folder =
		process_generic_value_folder_calloc();

	/* See attribute_list process_generic_value_folder */
	/* ----------------------------------------------- */
	piece( piece_string, FOLDER_DATA_DELIMITER, input, 0 );
	value_folder->value_folder_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 1 );
	value_folder->datatype_folder_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 2 );
	value_folder->foreign_folder_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 3 );
	value_folder->datatype_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 4 );
	value_folder->date_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 5 );
	value_folder->time_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 6 );
	value_folder->value_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 7 );
	value_folder->exists_aggregation_sum = (*piece_string == 'y');

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 8 );
	value_folder->exists_bar_graph = (*piece_string == 'y');

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 9 );
	value_folder->datatype_exists_unit = (*piece_string == 'y');

	piece( piece_string, FOLDER_DATA_DELIMITER, input, 10 );
	value_folder->foreign_exists_unit = (*piece_string == 'y');

	return value_folder;
}

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_fetch(
			char *value_folder_name,
			DICTIONARY *post_dictionary )
{
	char system_string[ 1024 ];
	char where[ 256 ];
	char *results;
	PROCESS_GENERIC_VALUE_FOLDER *value_folder;

	sprintf(where,
		"value_folder = '%s'",
		value_folder_name );

	sprintf(system_string,
		"select.sh '*' process_generic_value \"%s\"",
		where );

	if ( ! ( results = string_pipe_fetch( system_string ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: string_pipe_fetch(%s) returned empty.",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			system_string );
		exit( 1 );
	}

	value_folder =
		process_generic_value_folder_parse(
			results /* input */ );

	free( results );

	value_folder->primary_attribute_name_list =
		folder_fetch_primary_attribute_name_list(
			value_folder->value_folder_name );

	value_folder->post_dictionary = post_dictionary;

	value_folder->datatype =
		process_generic_datatype_fetch(
			value_folder->datatype_folder_name,
			value_folder->datatype_attribute_name,
			value_folder->exists_aggregation_sum,
			value_folder->exists_bar_graph,
			value_folder->exists_scale_graph_zero,
			value_folder->datatype_exists_unit,
			value_folder->post_dictionary );

	if ( !value_folder->datatype )
	{
		return (PROCESS_GENERIC_VALUE_FOLDER *)0;
	}

	if ( ! ( value_folder->foreign_folder =
			process_generic_foreign_folder_fetch(
				value_folder->foreign_folder_name,
				value_folder->post_dictionary,
				value_folder->foreign_exists_unit ) ) )
	{
		return (PROCESS_GENERIC_VALUE_FOLDER *)0;
	}

	return value_folder;
}

char *process_generic_value_folder_name(
			char *process_name,
			char *process_set_name )
{
	char system_string[ 1024 ];
	char where[ 128 ];

	sprintf(where,
		"process = '%s' or process_set = '%s'",
		process_name,
		process_set_name );

	sprintf(system_string,
		"select.sh value_folder process_generic \"%s\"",
		where );

	return string_pipe_fetch( system_string );
}

char *process_generic_datatype_folder_select(
			char *datatype_attribute_name,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			boolean datatype_exists_unit )
{
	char select[ 1024 ];
	char *ptr = select;

	ptr += sprintf( ptr, "%s", datatype_attribute_name );

	if ( exists_aggregation_sum )
	{
		ptr += sprintf( ptr, ",aggregation_sum_yn" );
	}
	else
	{
		ptr += sprintf( ptr, ",''" );
	}

	if ( exists_bar_graph )
	{
		ptr += sprintf( ptr, ",bar_graph_yn" );
	}
	else
	{
		ptr += sprintf( ptr, ",''" );
	}

	if ( exists_scale_graph_zero )
	{
		ptr += sprintf( ptr, "scale_graph_to_zero_yn" );
	}
	else
	{
		ptr += sprintf( ptr, ",''" );
	}

	if ( datatype_exists_unit )
	{
		ptr += sprintf( ptr, ",units" );
	}
	else
	{
		ptr += sprintf( ptr, ",''" );
	}

	return strdup( select );
}


char *process_generic_datatype_folder_where(
			LIST *primary_attribute_name_list,
			LIST *primary_attribute_data_list )
{
	return query_simple_where(
			(char *)0 /* folder_name */,
			primary_attribute_name_list
				/* where_attribute_name_list */,
			primary_attribute_data_list
				/* where_attribute_data_list */,
			(LIST *)0 /* append_isa_attribute_list */ );
}

PROCESS_GENERIC_PARAMETER *process_generic_parameter_parse(
			char *output_medium_string,
			DICTIONARY *post_dictionary,
			boolean aggregation_sum,
			LIST *value_folder_primary_attribute_name_list,
			char *value_folder_date_attribute_name,
			char *value_folder_time_attribute_name )
{
	char *accumulate_yn;

	PROCESS_GENERIC_PARAMETER *process_generic_parameter =
		process_generic_parameter_calloc();

	process_generic_parameter->output_medium_string =
		output_medium_string;

	if ( string_strcmp( output_medium_string, "text_file" ) == 0 )
	{
		process_generic_parameter->output_medium =
			text_file;
	}
	else
	if ( string_strcmp( output_medium_string, "spreadsheet" ) == 0 )
	{
		process_generic_parameter->output_medium =
			spreadsheet;
	}
	else
	if ( string_strcmp( output_medium_string, "table" ) == 0 )
	{
		process_generic_parameter->output_medium =
			table;
	}
	else
	if ( string_strcmp( output_medium_string, "gracechart" ) == 0 )
	{
		process_generic_parameter->output_medium =
			gracechart;
	}
	else
	{
		process_generic_parameter->output_medium =
			output_medium_unknown;
	}

	process_generic_parameter->begin_date =
		dictionary_fetch_index_zero(
			post_dictionary,
			"begin_date" );

	process_generic_parameter->end_date =
		dictionary_fetch_index_zero(
			post_dictionary,
			"end_date" );

	process_generic_parameter->aggregate_level =
		aggregate_level_extract(
			dictionary_fetch_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	process_generic_parameter->aggregate_statistic =
		aggregate_statistic_extract(
			dictionary_fetch_index_zero(
				post_dictionary,
				"aggregate_statistic" ),
			process_generic_parameter->aggregate_level );

	if ( process_generic_parameter->aggregate_statistic ==
		aggregate_statistic_none
	&&   process_generic_parameter->aggregate_level != aggregate_level_none
	&&   process_generic_parameter->aggregate_level != real_time )
	{
		process_generic_parameter->aggregate_statistic =
			aggregate_statistic_infer(
				aggregation_sum );
	}

	accumulate_yn =
		dictionary_fetch_index_zero(
			post_dictionary,
			"accumulate_yn" );

	process_generic_parameter->accumulate =
		(accumulate_yn && *accumulate_yn == 'y' );

	process_generic_parameter->email_address =
			dictionary_fetch_index_zero(
				post_dictionary,
				"email_address" );

	process_generic_parameter->units_converted =
			dictionary_fetch_index_zero(
				post_dictionary,
				"units_converted" );

	if ( process_generic_parameter->units_converted
	&&   strcmp(
		process_generic_parameter->units_converted,
		"units_converted" ) == 0 )
	{
		process_generic_parameter->units_converted = (char *)0;
	}

	process_generic_parameter->process_id = getpid();

	process_generic_parameter->date_piece =
		/* ----------------------- */
		/* Returns -1 if not found */
		/* ----------------------- */
		list_seek(	value_folder_date_attribute_name,
				value_folder_primary_attribute_name_list );

	process_generic_parameter->time_piece =
		list_seek(	value_folder_time_attribute_name,
				value_folder_primary_attribute_name_list );

	process_generic_parameter->value_piece =
		list_length(
			value_folder_primary_attribute_name_list );

	return process_generic_parameter;
}

PROCESS_GENERIC *process_generic_calloc( void )
{
	PROCESS_GENERIC *process_generic;

	if ( ! ( process_generic =
			calloc( 1, sizeof( PROCESS_GENERIC ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return process_generic;
}

PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_fetch(
			char *foreign_folder_name,
			DICTIONARY *post_dictionary,
			boolean foreign_exists_unit )
{
	PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder;

	process_generic_foreign_folder =
		(PROCESS_GENERIC_FOREIGN_FOLDER *)
			calloc( 1, sizeof( PROCESS_GENERIC_FOREIGN_FOLDER ) );

	process_generic_foreign_folder->foreign_folder_name =
		foreign_folder_name;

	process_generic_foreign_folder->post_dictionary =
		post_dictionary;

	process_generic_foreign_folder->foreign_folder =
		folder_fetch(
			process_generic_foreign_folder->foreign_folder_name,
			1 /* fetch_attribute_list */,
			0 /* not fetch_one2m_relation_list */,
			0 /* not fetch_one2m_recursive_relation_list */,
			0 /* not fetch_mto1_isa_recursive_relation_list */,
			0 /* not fetch_mto1_relation_list */ );

	process_generic_foreign_folder->foreign_attribute_name_list =
		process_generic_foreign_folder->
			foreign_folder->
			primary_attribute_name_list;

	process_generic_foreign_folder->foreign_attribute_data_list =
		dictionary_key_list_fetch(
			process_generic_foreign_folder->
				foreign_attribute_name_list,
			process_generic_foreign_folder->post_dictionary );

	if ( !list_length( process_generic_foreign_folder->
				foreign_attribute_data_list ) )
	{
		return (PROCESS_GENERIC_FOREIGN_FOLDER *)0;
	}

	process_generic_foreign_folder->unit =
		(foreign_exists_unit)
			? list_last( process_generic_foreign_folder->
					foreign_attribute_data_list )
			: (char *)0;

	return process_generic_foreign_folder;
}

char *process_generic_heading(
			LIST *primary_attribute_name_list,
			char *value_attribute_name,
			char *datatype_unit,
			char *foreign_folder_unit,
			char *units_converted,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			boolean accumulate )
{
	static char heading_line[ 1024 ];
	char *ptr = heading_line;
	char *attribute_name;
	char heading[ 128 ];

	*ptr = '\0';

	if ( !list_rewind( primary_attribute_name_list ) )
		return heading_line;

	do {
		attribute_name =
			list_get(
				primary_attribute_name_list );

		if ( !list_at_head( primary_attribute_name_list ) )
			ptr += sprintf( ptr, "," );

		format_initial_capital( heading, attribute_name );

		ptr += sprintf( ptr, "%s", heading );

	} while( list_next( primary_attribute_name_list ) );

	if ( datatype_unit && *datatype_unit )
	{
		sprintf(heading_line + strlen( heading_line ),
			",%s %s",
			format_initial_capital(
				heading,
				value_attribute_name ),
			process_generic_units_label(
				datatype_unit,
				units_converted,
				aggregate_statistic ) );
	}
	else
	if ( foreign_folder_unit && *foreign_folder_unit )
	{
		sprintf(heading_line + strlen( heading_line ),
			",%s %s",
			format_initial_capital(
				heading,
				value_attribute_name ),
			process_generic_units_label(
				foreign_folder_unit,
				units_converted,
				aggregate_statistic ) );
	}
	else
	{
		format_initial_capital( heading, value_attribute_name );

		sprintf(heading_line + strlen( heading_line ),
			",%s",
			format_initial_capital(
				heading,
				value_attribute_name ) );
	}

	if ( accumulate )
	{
		sprintf( heading_line + strlen( heading_line ),
			 ",Accumulate" );
	}

	if ( aggregate_level >= half_hour )
	{
		sprintf(heading_line + strlen( heading_line ),
			",Count" );
	}

	return heading_line;
}

char *process_generic_subtitle(
			char *value_folder_name,
			char *begin_date,
			char *end_date,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *additional_message )
{
	static char subtitle[ 256 ];
	char title_aggregate_statistic[ 128 ];
	char title_aggregate_level[ 128 ];
	char begin_date_buffer[ 128 ];
	char end_date_buffer[ 128 ];
	char buffer[ 128 ];

	format_initial_capital( subtitle, value_folder_name );

	if ( aggregate_statistic != aggregate_statistic_none )
	{
		sprintf(title_aggregate_statistic,
			" %s",
			format_initial_capital(
				buffer,
				aggregate_statistic_string(
					aggregate_statistic ) ) );
	}
	else
	{
		*title_aggregate_statistic = '\0';
	}

	if ( aggregate_level != aggregate_level_none )
	{
		sprintf(title_aggregate_level,
			" %s",
			format_initial_capital(
				buffer,
				aggregate_level_string(
					aggregate_level ) ) );
	}
	else
	{
		*title_aggregate_level = '\0';
	}

	if ( begin_date && *begin_date )
	{
		sprintf(begin_date_buffer,
			"Begin=%s",
			begin_date );
	}
	else
	{
		*begin_date_buffer = '\0';
	}

	if ( end_date && *end_date )
	{
		sprintf(end_date_buffer,
			"End=%s",
			end_date );
	}
	else
	{
		*end_date_buffer = '\0';
	}

	sprintf(subtitle + strlen( subtitle ),
		" %s %s%s%s %s",
		begin_date_buffer,
		end_date_buffer,
		title_aggregate_level,
		title_aggregate_statistic,
		(additional_message)
			? additional_message
			: "" );

	return subtitle;
}

char *process_generic_datatype_where(
			char *datatype_attribute_name,
			char *datatype_name )
{
	char where[ 256 ];
	char destination[ 128 ];

	sprintf(where,
		"%s = '%s'",
		datatype_attribute_name,
		string_escape_quote(
			destination,
			datatype_name ) );

	return strdup( where );
}

PROCESS_GENERIC *process_generic_fetch(
			char *process_set_name,
			char *process_name,
			char *login_name,
			char *output_medium_string,
			DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC *process_generic;

	process_generic = process_generic_calloc();

	process_generic->process_set_name = process_set_name;
	process_generic->login_name = login_name;
	process_generic->output_medium_string = output_medium_string;
	process_generic->post_dictionary = post_dictionary;

	if ( strcmp( process_set_name, "null" ) != 0 )
	{
		if ( ! ( process_generic->process_name =
				process_generic_process_name(
					process_generic->process_set_name,
					process_generic->post_dictionary ) ) )
		{
			fprintf(stderr,
"Warning in %s/%s()/%d: process_generic_process_name() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			return (PROCESS_GENERIC *)0;
		}
	}
	else
	{
		process_generic->process_name = process_name;
	}

	if ( ! ( process_generic->value_folder_name =
			process_generic_value_folder_name(
				process_generic->process_name,
				process_generic->process_set_name ) ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: process_generic_value_folder_name(%s/%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			process_generic->process_name,
			process_generic->process_set_name );
		return (PROCESS_GENERIC *)0;
	}

	if ( ! ( process_generic->value_folder =
			process_generic_value_folder_fetch(
				process_generic->value_folder_name,
				process_generic->post_dictionary ) ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: process_generic_value_folder_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PROCESS_GENERIC *)0;
	}

	process_generic->parameter =
		process_generic_parameter_parse(
			process_generic->output_medium_string,
			process_generic->post_dictionary,
			process_generic->
				value_folder->
				datatype->
				aggregation_sum,
			process_generic->
				value_folder->
				primary_attribute_name_list,
			process_generic->
				value_folder->
				date_attribute_name,
			process_generic->
				value_folder->
				time_attribute_name );

	if ( !process_generic->parameter )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: process_generic_parameter_parse() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (PROCESS_GENERIC *)0;
	}

	process_generic->process_generic_date_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_date_where(
			process_generic->
				value_folder->
				date_attribute_name,
			process_generic->
				parameter->
				begin_date,
			process_generic->
				parameter->
				end_date );

	process_generic->process_generic_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		process_generic_where(
			process_generic->
				value_folder->
				foreign_folder->
				foreign_attribute_name_list,
			process_generic->
				value_folder->
				foreign_folder->
				foreign_attribute_data_list,
			process_generic->
				process_generic_date_where );

	if ( !process_generic->process_generic_where )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: process_generic_where() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PROCESS_GENERIC *)0;
	}

	process_generic->process_generic_system_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		process_generic_system_string(
			process_generic_select(
				process_generic->
					value_folder->
					primary_attribute_name_list,
				process_generic->
					value_folder->
					value_attribute_name ),
			process_generic->value_folder_name,
			process_generic->process_generic_where,
			process_generic->
				parameter->
				aggregate_level,
			process_generic->
				parameter->
				aggregate_statistic,
			process_generic->
				parameter->
				accumulate,
			process_generic->
				parameter->
				date_piece,
			process_generic->
				parameter->
				time_piece,
			process_generic->
				parameter->
				value_piece,
			process_generic->
				parameter->
				end_date,
			process_generic->
				value_folder->
				datatype->
				unit,
			process_generic->
				value_folder->
				foreign_folder->
				unit,
			process_generic->
				parameter->
				units_converted );

	if ( !process_generic->process_generic_system_string )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: process_generic_system_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PROCESS_GENERIC *)0;
	}

	process_generic->
		process_generic_heading =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			process_generic_heading(
				process_generic->
					value_folder->
					primary_attribute_name_list,
				process_generic->
					value_folder->
					value_attribute_name,
				process_generic->
					value_folder->
					datatype->
					unit /* datatype_unit */,
				process_generic->
					value_folder->
					foreign_folder->
					unit /* foreign_folder_unit */,
				process_generic->
					parameter->
					units_converted,
				process_generic->
					parameter->
					aggregate_level,
				process_generic->
					parameter->
					aggregate_statistic,
				process_generic->
					parameter->
					accumulate );

	process_generic->
		process_generic_subtitle =
			/* Returns static memory */
			/* --------------------- */
			process_generic_subtitle(
				process_generic->value_folder_name,
				process_generic->
					parameter->
					begin_date,
				process_generic->
					parameter->
					end_date,
				process_generic->
					parameter->
					aggregate_level,
				process_generic->
					parameter->
					aggregate_statistic,
				(char *)0 /* additional_message */ );

	return process_generic;
}

