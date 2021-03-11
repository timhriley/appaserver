/* $APPASERVER_HOME/library/process_generic_output.c			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver process_generic_output ADT.			*/
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
#include "process_generic_output.h"

#ifdef NOT_DEFINED
LIST *process_generic_get_compare_datatype_list(
			char *application_name,
			LIST *foreign_attribute_name_list,
			char *datatype_folder_name,
			LIST *datatype_primary_attribute_name_list,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			char *units_folder_name,
			DICTIONARY *post_dictionary,
			char *compare_datatype_prefix,
			boolean with_select_data,
			enum aggregate_level aggregate_level,
			char *begin_date_string,
			char *end_date_string,
			char *value_folder_name,
			char *date_attribute_name,
			char *time_attribute_name,
			char *value_attribute_name,
			boolean accumulate )
{
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;
	LIST *compare_datatype_list = list_new_list();
	LIST *datatype_primary_attribute_data_list;
	int index;
	char values_select_string[ 256 ];

	if ( with_select_data )
	{
		if ( time_attribute_name
		&&   *time_attribute_name
		&&   ( aggregate_level == aggregate_level_none
		||     aggregate_level == real_time ) )
		{
			sprintf(	values_select_string,
					"concat(%s,':',%s),%s",
					date_attribute_name,
					time_attribute_name,
					value_attribute_name );
		}
		else
		{
			sprintf(	values_select_string,
					"%s,%s",
					date_attribute_name,
					value_attribute_name );
		}
	}

	for(	index = 1;
		( datatype_primary_attribute_data_list =
		     process_generic_get_datatype_primary_attribute_data_list(
			post_dictionary,
			datatype_primary_attribute_name_list,
			compare_datatype_prefix,
			index ) );
		index++ )
	{

		process_generic_datatype =
			(PROCESS_GENERIC_DATATYPE *)
				calloc( 1, sizeof( PROCESS_GENERIC_DATATYPE ) );
	
		process_generic_datatype->
			primary_attribute_data_list =
				datatype_primary_attribute_data_list;

		process_generic_datatype_load(
			&process_generic_datatype->aggregation_sum,
			&process_generic_datatype->bar_graph,
			&process_generic_datatype->scale_graph_zero,
			&process_generic_datatype->units,
			exists_aggregation_sum,
			exists_bar_graph,
			exists_scale_graph_zero,
			application_name,
			datatype_folder_name,
			datatype_primary_attribute_name_list,
			process_generic_datatype->primary_attribute_data_list,
			units_folder_name );

		if ( ! ( process_generic_datatype->foreign_attribute_data_list =
				dictionary_get_prefixed_indexed_data_list(
				post_dictionary,
				compare_datatype_prefix,
				index,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) ) )
		{
			return (LIST *)0;
		}

		if ( with_select_data )
		{
			process_generic_datatype->values_hash_table =
				process_generic_values_hash_table(
					application_name,
					values_select_string,
					aggregate_level,
					foreign_attribute_name_list,
					process_generic_datatype->
						foreign_attribute_data_list,
					begin_date_string,
					end_date_string,
					value_folder_name,
					date_attribute_name,
					accumulate );
		}

		list_append_pointer(	compare_datatype_list,
					process_generic_datatype );
	}

	return compare_datatype_list;

}
#endif

/*
	if ( !delimiter )
	{
		sprintf(character_translate_process,
			"double_quote_comma_delimited.e '%c'",
			PROCESS_GENERIC_OUTPUT_DELIMITER );
		heading_delimiter = ',';
	}
	else
	{
		sprintf(	character_translate_process,
				"tr '%c' '%c'",
				PROCESS_GENERIC_OUTPUT_DELIMITER,
				delimiter );
		heading_delimiter = delimiter;
	}
*/

/*
		char append_low_high_flag;

		if ( append_low_high )
			append_low_high_flag = 'y';
		else
			append_low_high_flag = 'n';

		sprintf(aggregation_process, 
			"real_time2aggregate_value.e %s %d %d %d '%c' %s %c %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 local_date_piece,
			 local_time_piece,
			 local_value_piece,
			 PROCESS_GENERIC_OUTPUT_DELIMITER,
			 aggregate_level_get_string( aggregate_level ),
			 append_low_high_flag,
			 *end_date );

		strcpy( sort_process, "sort -r" );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0
	&&   process_generic_output->
		value_folder->datatype->units
	&&   *process_generic_output->
		value_folder->datatype->units )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e '%s' '%s' '%s' %d '%c'",
			 application_name,
			 process_generic_output->
				value_folder->
					datatype->units,
			 units_converted,
			 local_value_piece,
			 PROCESS_GENERIC_OUTPUT_DELIMITER );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}
*/

char *process_generic_output_process_name(
			char *process_set_name,
			DICTIONARY *post_dictionary )
{
	char key[ 128 ];

	sprintf( key, "%s_0", process_set_name );
	return dictionary_get_pointer( post_dictionary, key );
}

char *process_generic_output_get_units_label(
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

	/* format_initial_capital( units_label, units_label ); */

	return units_label;

}

char *process_generic_output_get_exceedance_html_table_justify_string(
					int list_length,
					enum aggregate_level aggregate_level )
{
	char justify_string[ 1024 ];
	char *ptr;
	int i;
	int number_significant_columns;

	ptr = justify_string;

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		number_significant_columns = list_length - 1;
	}
	else
	{
		number_significant_columns = list_length - 2;
	}

	ptr += sprintf( ptr, "right" );
	for( i = 0; i < number_significant_columns; i++ )
		ptr += sprintf( ptr, ",left" );

	/* One more for the aggregation count */
	/* ---------------------------------- */
	if ( aggregate_level != real_time ) sprintf( ptr, ",right" );

	sprintf( ptr, ",right,right" );
	return strdup( justify_string );
}

LIST *process_generic_output_get_exceedance_heading_list(
				LIST *select_list,
				char *value_attribute_name,
				char *units_label,
				enum aggregate_level aggregate_level )
{
	char value_label[ 128 ];
	LIST *heading_list;

	heading_list = list_new_list();

	if ( !list_rewind( select_list ) ) return heading_list;

	sprintf(value_label,
	 	"%s %s",
	 	value_attribute_name,
	 	units_label );

	list_append_pointer( heading_list, strdup( value_label ) );

	/* Value is the first element in select_list */
	/* ----------------------------------------- */
	list_next( select_list );

	do {
		list_append_pointer(
			heading_list,
			list_get_pointer( select_list ) );

	} while( list_next( select_list ) );

	if ( aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		list_append_pointer( heading_list, "aggregation_count" );
	}

	list_append_pointer( heading_list, "count_below" );
	list_append_pointer( heading_list, "percent_below" );

	return heading_list;

}

#ifdef NOT_DEFINED
char *process_generic_output_get_row_exceedance_stdout_sys_string(
				char **begin_date,
				char **end_date,
				int *percent_below_piece,
				char **where_clause,
				char *application_name,
				PROCESS_GENERIC_OUTPUT *process_generic_output,
				DICTIONARY *post_dictionary,
				char *login_name,
				int row )
{
	LIST *attribute_list;
	LIST *select_list;
	char sys_string[ 2048 ];
	char units_converted_process[ 128 ];
	char *units_converted;
	char date_convert_process[ 512 ];
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char aggregate_process[ 256 ];
	int value_piece = 0;
	int date_piece = 1;
	int time_piece = -1;

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	sprintf(	date_convert_process,
			"date_convert.e '%c' %d %s",
			FOLDER_DATA_DELIMITER,
			date_piece,
			date_convert_get_date_format_string(
				date_convert_get_user_date_format(
					application_name,
					login_name ) ) );

	select_list = list_new_list();

	attribute_list =
		attribute_get_attribute_list(
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	*where_clause =
		process_generic_output_get_row_dictionary_where_clause(
			begin_date,
			end_date,
			application_name,
			process_generic_output,
			post_dictionary,
			1 /* with_set_dates */,
			row );

	units_converted = dictionary_get_index_zero(
				post_dictionary,
				"units_converted" );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0
	&&   process_generic_output->
		value_folder->datatype->units
	&&   *process_generic_output->
		value_folder->datatype->units )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s %d '%c'",
			 application_name,
			 process_generic_output->
				value_folder->datatype->units,
			 units_converted,
			 value_piece,
			 FOLDER_DATA_DELIMITER );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_statistic" ),
			aggregate_level );

	list_append_pointer(	select_list,
	 			process_generic_output->
					value_folder->
						value_attribute_name );

	list_append_pointer(
		select_list,
		process_generic_output->
			value_folder->
				date_attribute_name );

	if ( process_generic_output->
		value_folder->
		time_attribute_name
	&&   *process_generic_output->
		value_folder->
		time_attribute_name )
	{
		list_append_pointer(
				select_list,
				process_generic_output->
					value_folder->
					time_attribute_name );
		time_piece = 2;
	}

	if ( aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		sprintf( aggregate_process,
		"real_time2aggregate_value.e %s %d %d %d '%c' '%s' n %s	 |"
		"piece_inverse.e %d '%c'				 |"
		"cat							  ",
		aggregate_statistic_get_string( aggregate_statistic ),
		date_piece,
		time_piece,
		value_piece,
		FOLDER_DATA_DELIMITER,
		aggregate_level_get_string( aggregate_level ),
		*end_date,
		time_piece,
		FOLDER_DATA_DELIMITER );
	}
	else
	{
		strcpy( aggregate_process, "cat" );
	}

	select_list =
		process_generic_output_append_select_list(
				select_list,
				attribute_list,
				post_dictionary );

	sprintf(sys_string,
		"get_folder_data	application=%s			  "
		"			folder=%s			  "
		"			select=\"%s\"			  "
		"			where=\"%s\"			  "
		"			order=\"%s\"			 |"
		"%s							 |"
		"%s							 |"
		"piece_exceedance.e '%c'				 |"
		"%s							 |"
		"cat							  ",
		application_name,
		process_generic_output->value_folder->value_folder_name,
		list_display( select_list ),
		*where_clause,
		process_generic_output->value_folder->date_attribute_name,
		units_converted_process,
		aggregate_process,
		FOLDER_DATA_DELIMITER,
		date_convert_process );

	*percent_below_piece = list_length( select_list ) + 1;

	return strdup( sys_string );

}

char *process_generic_output_get_exceedance_stdout_sys_string(
				char **begin_date,
				char **end_date,
				int *percent_below_piece,
				char **where_clause,
				char *application_name,
				PROCESS_GENERIC_OUTPUT *process_generic_output,
				DICTIONARY *post_dictionary,
				boolean with_html_table,
				char *login_name )
{
	LIST *attribute_list;
	LIST *select_list;
	char sys_string[ 2048 ];
	char html_table_process[ 512 ];
	char pad_columns_process[ 128 ];
	char units_converted_process[ 128 ];
	char *units_converted;
	char *units_label;
	char date_convert_process[ 512 ];
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char aggregate_process[ 256 ];
	int value_piece = 0;
	int date_piece = 1;
	int time_piece = -1;
	char order_clause[ 128 ];

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	/* Build select_list */
	/* ----------------- */
	select_list = list_new_list();

	list_append_pointer(
			select_list,
	 		process_generic_output->
				value_folder->
				value_attribute_name );

	list_append_pointer(
			select_list,
			process_generic_output->
				value_folder->
				date_attribute_name );

	if ( ( aggregate_level == aggregate_level_none
	||     aggregate_level == real_time
	||     aggregate_level == hourly
	||     aggregate_level == half_hour )
	&&   process_generic_output->value_folder->time_attribute_name
	&&   *process_generic_output->value_folder->time_attribute_name )
	{
		list_append_pointer(
				select_list,
				process_generic_output->
					value_folder->
					time_attribute_name );
		time_piece = 2;
	}

	attribute_list =
		attribute_get_attribute_list(
			application_name,
			process_generic_output->
				value_folder->
				value_folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	select_list =
		process_generic_output_append_select_list(
				select_list,
				attribute_list,
				post_dictionary );

	/* Build where_clause */
	/* ------------------ */
	*where_clause =
		process_generic_output_get_dictionary_where_clause(
			begin_date,
			end_date,
			application_name,
			process_generic_output,
			post_dictionary,
			1 /* with_set_dates */,
			process_generic_output->
				value_folder->
				value_folder_name );

	/* Build order_clause */
	/* ------------------ */
	if ( process_generic_output->value_folder->time_attribute_name
	&&   *process_generic_output->value_folder->time_attribute_name )
	{
		sprintf( order_clause,
			 "%s,%s",
			 process_generic_output->
				value_folder->
				date_attribute_name,
			 process_generic_output->
				value_folder->
				time_attribute_name );
	}
	else
	{
		strcpy(	order_clause,
			process_generic_output->
				value_folder->
				date_attribute_name );
	}

	/* Populate processes */
	/* ------------------ */
	sprintf(	date_convert_process,
			"date_convert.e '%c' %d %s",
			FOLDER_DATA_DELIMITER,
			date_piece,
			date_convert_get_date_format_string(
				date_convert_get_user_date_format(
					application_name,
					login_name ) ) );

	units_converted = dictionary_get_index_zero(
				post_dictionary,
				"units_converted" );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0
	&&   process_generic_output->
		value_folder->datatype->units
	&&   *process_generic_output->
		value_folder->datatype->units )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s %d '%c'",
			 application_name,
			 process_generic_output->
				value_folder->datatype->units,
			 units_converted,
			 value_piece,
			 FOLDER_DATA_DELIMITER );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_statistic" ),
			aggregate_level );

	units_label =
		process_generic_output_get_units_label(
			process_generic_output->
				value_folder->datatype->units,
			units_converted,
			aggregate_statistic );

	if ( aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		sprintf( aggregate_process,
		"real_time2aggregate_value.e %s %d %d %d '%c' '%s' n %s	 |"
		"cat							  ",
		aggregate_statistic_get_string( aggregate_statistic ),
		date_piece,
		time_piece,
		value_piece,
		FOLDER_DATA_DELIMITER,
		aggregate_level_get_string( aggregate_level ),
		*end_date );
	}
	else
	{
		strcpy( aggregate_process, "cat" );
	}

	if ( with_html_table )
	{
		char title[ 128 ];
		LIST *heading_list;

		heading_list =
			process_generic_output_get_exceedance_heading_list(
				select_list,
				process_generic_output->
					value_folder->
					value_attribute_name,
				units_label,
				aggregate_level );

		sprintf( title,
			 "%s Exceedance Table",
			 process_generic_output->
				value_folder->
				value_folder_name );

		if ( aggregate_level != aggregate_level_none
		&&   aggregate_level != real_time )
		{
			sprintf( title + strlen( title ),
				 " (%s %s)",
			 	 aggregate_level_get_string(
					aggregate_level ),
			 	 aggregate_statistic_get_string(
					aggregate_statistic ) );
		}

		format_initial_capital( title, title );

		sprintf(
		html_table_process,
		"queue_top_bottom_lines.e %d | html_table.e '%s' '%s' '%c' %s",
		HTML_TABLE_QUEUE_TOP_BOTTOM,
	 	title,
		list_display( heading_list ),
		FOLDER_DATA_DELIMITER,
		process_generic_output_get_exceedance_html_table_justify_string(
			list_length( select_list ),
			aggregate_level ) );

		strcpy( pad_columns_process, "cat" );
	}
	else
	{
		strcpy( html_table_process, "cat" );
		sprintf(pad_columns_process,
		 	"tr '^' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );
	}

	sprintf(sys_string,
		"get_folder_data	application=%s			  "
		"			folder=%s			  "
		"			select=\"%s\"			  "
		"			where=\"%s\"			  "
		"			order=%s			 |"
		"%s							 |"
		"%s							 |"
		"piece_exceedance.e '%c'				 |"
		"%s							 |"
		"%s							 |"
		"%s							 |"
		"cat							  ",
		application_name,
		process_generic_output->value_folder->value_folder_name,
		list_display( select_list ),
		*where_clause,
		order_clause,
		units_converted_process,
		aggregate_process,
		FOLDER_DATA_DELIMITER,
		date_convert_process,
		pad_columns_process,
		html_table_process );

	*percent_below_piece = list_length( select_list ) + 1;

	if ( aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		(*percent_below_piece)++;
	}

	return strdup( sys_string );

}

void process_generic_output_get_period_of_record_date(
				char **date_string,
				char *application_name,
				char *value_folder_name,
				char *function,
				char *date_attribute,
				char *where_clause )
{
	char sys_string[ 1024 ];
	char *return_pointer;
	char select[ 128 ];

	sprintf( select, "%s(%s)", function, date_attribute );

	sprintf(	sys_string,
			"get_folder_data	application=%s		"
			"			folder=%s		"
			"			select=\"%s\"		"
			"			where=\"%s\"		"
			"			order=none		",
			application_name,
			value_folder_name,
			select,
			where_clause );

	if ( ! ( return_pointer = pipe2string( sys_string ) ) )
		return;
	*date_string = return_pointer;
}

enum aggregate_statistic process_generic_output_get_aggregate_statistic(
				boolean datatype_aggregation_sum )
{
	if ( datatype_aggregation_sum )
		return sum;
	else
		return average;
}

char *process_generic_output_get_highest_delimiter_compare_key(
				DICTIONARY *post_dictionary,
				int dictionary_offset )
{
	LIST *key_list;
	char *key;
	char *data;
	int highest_delimiters = 0;
	int number_delimiters;
	char *highest_compare_key = {0};

	key_list = dictionary_get_key_list( post_dictionary );

	if ( !list_rewind( key_list ) ) return (char *)0;

	do {
		key = list_get_pointer( key_list );

		if ( strncmp( key, "compare_", 8 ) == 0
		&&   timlib_get_index( key ) == dictionary_offset )
		{
			data = dictionary_get_data(	post_dictionary,
							key );
			number_delimiters =
				timlib_count_delimiters(
					MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
					data );

			if ( number_delimiters > highest_delimiters )
			{
				highest_delimiters = number_delimiters;
				highest_compare_key = key;
			}
		}

	} while( list_next( key_list ) );
	return highest_compare_key;
}

LIST *process_generic_output_get_compare_data_list(
				DICTIONARY *post_dictionary,
				int dictionary_offset )
{
	char *compare_key;
	char *compare_data;
	LIST *compare_data_list;
	char piece_buffer[ 128 ];
	int piece_offset;

	if ( ! ( compare_key =
		process_generic_output_get_highest_delimiter_compare_key(
				post_dictionary,
				dictionary_offset ) ) )
	{
		return (LIST *)0;
	}

	compare_data = dictionary_get_data(
					post_dictionary,
					compare_key );

	compare_data_list = list_new_list();

	for( piece_offset = 0; ; piece(	piece_buffer,
					MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
					compare_data,
					piece_offset ) )
	{
		list_append_pointer(	compare_data_list,
					strdup( piece_buffer ) );
	}

	return compare_data_list;
}

boolean process_generic_output_exists_compare_multi_select(
				DICTIONARY *post_dictionary )
{
	if ( process_generic_output_get_compare_data_list(
				post_dictionary,
				1 /* dictionary_offset */ ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
#endif

char *process_generic_output_date_where(
			char *date_attribute_name,
			char *begin_date,
			char *end_date )
{
	static char where[ 256 ];

	if ( begin_date && *begin_date
	&&   end_date && *end_date )
	{
		sprintf(where,
			" and %s >= '%s' and %s <= '%s'",
			date_attribute_name,
			begin_date,
			date_attribute_name,
			end_date );
	}
	else
	if ( begin_date && *begin_date )
	{
		sprintf(where,
			" and %s >= '%s'",
			date_attribute_name,
			begin_date );
	}
	else
	if ( end_date && *end_date )
	{
		sprintf(where,
			" and %s <= '%s'",
			date_attribute_name,
			end_date );
	}
	else
	{
		strcpy( where, "and 1 = 1" );
	}

	return where;
}

/* Returns heap memory. */
/* -------------------- */
char *process_generic_output_drop_down_where(
			char *application_name,
			char *value_folder_name,
			DICTIONARY *query_removed_post_dictionary )
{
	QUERY *query;

	query = query_calloc();

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				value_folder_name,
				(ROLE *)0 /* role */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->dictionary = query_removed_post_dictionary;
	query->max_rows = 0;

	query->query_output =
		process_generic_query_output(
				application_name,
				query->folder->folder_name,
				query->folder->mto1_related_folder_list,
				query_removed_post_dictionary );

	if ( !query->query_output )
		return (char *)0;
	else
		return query->query_output->where_clause;

}

boolean process_generic_output_validate_begin_end_date(
				char **begin_date_string /* in/out */,
				char **end_date_string /* in/out */,
				char *application_name,
				char *value_folder_name,
				char *date_attribute_name,
				DICTIONARY *query_removed_post_dictionary )
{
	char buffer[ 128 ];
	int i;
	char new_begin_date[ 128 ];
	char new_end_date[ 128 ];

	if ( !begin_date_string && !end_date_string ) return 0;

	if ( character_exists( *begin_date_string, '/' ) ) return 1;
	if ( character_exists( *end_date_string, '/' ) ) return 1;

	if ( !*begin_date_string
	&&   dictionary_length( query_removed_post_dictionary ) )
	{
		dictionary_get_index_data(
			begin_date_string,
			query_removed_post_dictionary,
			"begin_date",
			0 );
	}

	if ( !*end_date_string
	&&   dictionary_length( query_removed_post_dictionary ) )
	{
		dictionary_get_index_data(
			end_date_string,
			query_removed_post_dictionary,
			"end_date",
			0 );
	}

	if ( ( !*begin_date_string || !**begin_date_string )
	&&   ( !*end_date_string || !**end_date_string ) )
	{
		char *where_clause;

		where_clause =
			process_generic_output_where_clause(
				application_name,
				value_folder_name,
				date_attribute_name,
				*begin_date_string,
				*end_date_string,
				query_removed_post_dictionary );

		if ( !where_clause || !*where_clause ) return 0;

		process_generic_output_get_period_of_record_date(
				begin_date_string,
				application_name,
				value_folder_name,
				"min",
				date_attribute_name,
				where_clause );

		if ( !**begin_date_string ) return 0;

		process_generic_output_get_period_of_record_date(
				end_date_string,
				application_name,
				value_folder_name,
				"max",
				date_attribute_name,
				where_clause );

			return 1;
	}
	
	strcpy( new_begin_date, *begin_date_string );
	strcpy( new_end_date, *end_date_string );

	piece( buffer, '-', new_begin_date, 0 );
	if ( strlen( buffer ) != 4 ) return 0;

	/* Don't allow for 0000-00-00 */
	/* -------------------------- */
	if ( !atoi( buffer ) ) return 0;

	if ( !piece( buffer, '-', new_begin_date, 1 ) )
	{
		strcat( new_begin_date, "-01-01" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 12 ) return 0;
	}

	if ( !piece( buffer, '-', new_begin_date, 2 ) )
	{
		strcat( new_begin_date, "-01" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 31 ) return 0;
	}

	if ( !*new_end_date || strcmp( new_end_date, "end_date" ) == 0 )
	{
		strcpy( new_end_date, new_begin_date );
	}

	piece( buffer, '-', new_end_date, 0 );
	if ( strlen( buffer ) != 4 ) return 0;
	if ( !atoi( buffer ) ) return 0;

	if ( !piece( buffer, '-', new_end_date, 1 ) )
	{
		strcat( new_end_date, "-12-31" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 12 ) return 0;
	}

	if ( !piece( buffer, '-', new_end_date, 2 ) )
	{
		strcat( new_end_date, "-31" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 31 ) return 0;
	}

	*begin_date_string = new_begin_date;
	*end_date_string = new_end_date;

	if ( strcmp( new_begin_date, new_end_date ) > 0 )
		return 0;
	else
		return 1;

}

/* PROCESS_GENERIC_DATATYPE */
/* ------------------------ */
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
			boolean datatype_exists_units,
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

	process_generic_datatype->datatype_exists_units =
		datatype_exists_units;

	process_generic_datatype->post_dictionary =
		post_dictionary;

	process_generic_datatype->process_generic_datatype_select =
		process_generic_datatype_select(
			process_generic_datatype->datatype_attribute_name,
			process_generic_datatype->exists_aggregation_sum,
			process_generic_datatype->exists_bar_graph,
			process_generic_datatype->exists_scale_graph_zero,
			process_generic_datatype->datatype_exists_units );

	process_generic_datatype->datatype_name =
		dictionary_fetch(
			process_generic_datatype->post_dictionary,
			process_generic_datatype->datatype_attribute_name );

	if ( !process_generic_datatype->datatype_name )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: dictionary_fetch(%s) of %s returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			dictionary_display( post_dictionary ),
			process_generic_datatype->datatype_attribute_name );
		exit( 1 );
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

char *process_generic_output_where(
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

char *process_generic_datatype_measurement_where(
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			char *date_where )
{
	char *query_where;
	static char where[ 1024 ];

	query_where =
		query_simple_where(
			(char *)0 /* folder_name */,
			foreign_attribute_name_list
				/* where_attribute_name_list */,
			foreign_attribute_data_list
				/* where_attribute_data_list */,
			(LIST *)0 /* append_isa_attribute_list */ );

	if ( !query_where )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_simple_where() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !date_where )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty date_where.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"%s and %s",
		query_where,
		date_where );

	return where;
}

char *process_generic_output_select(
			LIST *foreign_attribute_name_list,
			int foreign_attribute_name_list_length )
{
	return
		list_display_limited(
			foreign_attribute_name_list,
			',',
			foreign_attribute_name_list_length );
}

char *process_generic_system_string(
			char *select,
			char *value_folder_name,
			char *where,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *end_date,
			boolean accumulate,
			int foreign_attribute_name_list_length )
{
	char system_string[ 2048 ];
	char real_time_process[ 512 ];
	char accumulation_process[ 512 ];
	int date_piece;
	int time_piece;
	int value_piece;
	int accumulate_piece;

	if ( !select || !*select ) return (char *)0;
	if ( !value_folder_name || !*value_folder_name ) return (char *)0;
	if ( !where || !*where ) return (char *)0;

	date_piece = foreign_attribute_name_list_length;

	if ( time_attribute && *time_attribute )
	{
		time_piece = length_foreign_attribute_name_list + 1;
		value_piece = length_foreign_attribute_name_list + 2;
	}
	else
	{
		time_piece = -1;
		value_piece = length_foreign_attribute_name_list + 1;
	}

	if ( accumulate )
	{
		if ( time_piece != -1 )
			accumulate_piece = date_piece + 2;
		else
			accumulate_piece = date_piece + 1;
	}
	else
	{
		accumulate_piece = -1;
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
			(end_date_string) ? end_date_string : "" );
	}

	if ( accumulate )
	{
		sprintf( accumulation_process, 
			 "sort | accumulate.e %d '%c' append",
			 accumulate_piece,
			 SQL_DELIMITER );
	}
	else
	{
		strcpy( accumulation_process, "cat" );
	}

	sprintf(system_string,
		"get_folder_data	application=%s		    "
		"			folder=%s		    "
		"			select=\"%s\"		    "
		"			where=\"%s\"		   |"
		"%s						   |"
		"%s						   |"
		"cat						    ",
		environment_application_name(),
		value_folder,
		select,
		where,
		accumulation_process,
		real_time_process );

	return strdup( system_string );
}

/* PROCESS_GENERIC_VALUE */
/* --------------------- */
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

/*
PROCESS_GENERIC_VALUE *process_generic_value_parse(
			double *accumulate_value,
			char *input,
			char *time_attribute,
			boolean unit_value_folder,
			boolean accumulate,
			enum aggregate_level aggregate_level )
{
	PROCESS_GENERIC_VALUE *process_generic_value;

	if ( !input || !*input ) return (PROCESS_GENERIC_VALUE *)0;
}
*/

HASH_TABLE *process_generic_values_hash_table(
			char *application_name,
			char *values_select_string,
			enum aggregate_level aggregate_level,
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			char *begin_date_string,
			char *end_date_string,
			char *value_folder_name,
			char *date_attribute_name,
			boolean accumulate )
{
	char sys_string[ 1024 ];
	char real_time_process[ 512 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char date_string[ 16 ];
	char value_string[ 128 ];
	double accumulate_so_far = 0.0;
	PROCESS_GENERIC_VALUE *value;
	HASH_TABLE *values_hash_table = hash_table_new( hash_table_medium );
	char *where_clause;

	where_clause =
		process_generic_output_get_foreign_folder_where_clause(
			&begin_date_string,
			&end_date_string,
			foreign_attribute_name_list,
			foreign_attribute_data_list,
			/* ------------------------------------------------ */
			/* Need to assume both dates are already populated. */
			/* ------------------------------------------------ */
			(DICTIONARY *)0 /* dictionary */,
	       		(char *)0 /* application_name */,
			(char *)0 /* value_folder_name */,
			date_attribute_name );

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		strcpy( real_time_process, "cat" );
	}
	else
	{
		sprintf( real_time_process, 
	 "real_time2aggregate_value.e %s %d %d %d '%c' %s n %s",
	 		aggregate_statistic_get_string(
				aggregate_statistic_none ),
	 		 0 /* date_piece */,
	 		-1 /* time_piece */,
	 		 1 /* value_piece */,
			FOLDER_DATA_DELIMITER,
	 		aggregate_level_get_string(
				aggregate_level ),
			end_date_string );
	}

	sprintf(sys_string,
		"get_folder_data	application=%s		    "
		"			folder=%s		    "
		"			select=\"%s\"		    "
		"			where=\"%s\"		   |"
		"%s						   |"
		"cat						    ",
		application_name,
		value_folder_name,
		values_select_string,
		where_clause,
		real_time_process );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( value_string, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		value = process_generic_value_new();

		if ( *value_string && strncmp( value_string, "null", 4 ) != 0 )
		{
			value->value = atof( value_string );

			if ( accumulate )
			{
				accumulate_so_far += value->value;
				value->accumulate = accumulate_so_far;
			}
		}
		else
		{
			value->is_null = 1;
		}

		hash_table_add_pointer(
			values_hash_table,
			strdup( date_string ),
			value );
	}

	pclose( input_pipe );

	return values_hash_table;
}

/* PROCESS_GENERIC_PARAMETER */
/* ------------------------- */
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

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_fetch(
			char *value_folder_name,
			DICTIONARY *post_dictionary )
{
	char system_string[ 1024 ];
	char piece_string[ 128 ];
	char where[ 256 ];
	char *order;
	char *results;

	PROCESS_GENERIC_VALUE_FOLDER *value_folder =
		process_generic_value_folder_calloc();

	value_folder->post_dictionary = post_dictionary;

	sprintf(where,
		"value_folder = '%s'",
		value_folder_name );

	sprintf(system_string,
		"select.sh '*' process_generic_value_folder \"%s\"",
		select,
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

	/* See attribute_list process_generic_value_folder */
	/* ----------------------------------------------- */
	piece( piece_string, FOLDER_DATA_DELIMITER, results, 0 );
	value_folder->value_folder_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 1 );
	value_folder->datatype_folder = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 2 );
	value_folder->foreign_folder = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 3 );
	value_folder->datatype_attribute = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 4 );
	value_folder->date_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 5 );
	value_folder->time_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 6 );
	value_folder->value_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 7 );
	value_folder->exists_aggregation_sum = (*piece_string == 'y');

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 8 );
	value_folder->exists_bar_graph = (*piece_string == 'y');

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 9 );
	value_folder->datatype_exists_unit = (*piece_string == 'y');

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 10 );
	value_folder->foreign_exists_unit = (*piece_string == 'y');

	free( results );

	value_folder->datatype =
		process_generic_datatype_fetch(
			value_folder->datatype_folder_name,
			value_folder->datatype_attribute_name,
			value_folder->datatype_attribute_name,
			value_folder->datatype_attribute_name,
			value_folder->datatype_attribute_name,
			value_folder->datatype_attribute_name,
			value_folder->post_dictionary );

	return value_folder;
}

char *process_generic_output_value_folder_name(
			char *process_name,
			char *process_set_name )
{
	char system_string[ 1024 ];
	char where[ 128 ];

	sprintf(where,
		"process = '%s' and process_set = '%s'",
		process_name,
		process_set_name );

	sprintf(system_string,
		"select.sh value_folder process_generic_output \"%s\"",
		where );

	return string_pipe_fetch( system_string );
}

PROCESS_GENERIC_DATATYPE_FOLDER *process_generic_datatype_folder_calloc(
			void )
{
	PROCESS_GENERIC_DATATYPE_FOLDER *process_generic_datatype_folder;

	if ( ! ( process_generic_datatype_folder =
			calloc( 1,
				sizeof( PROCESS_GENERIC_DATATYPE_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_generic_datatype_folder;
}

LIST *process_generic_datatype_folder_primary_attribute_name_list(
			char *datatype_folder_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *order;

	char *select = "attribute";

	sprintf(where,
		"folder = '%s' and primary_key_index >= 1",
		datatype_folder_name );

	order = "primary_key_index";

	sprintf(sys_string,
		"select.sh %s folder_attribute \"%s\" %s",
		 select,
		 where,
		 order );

	return string_pipe_list( sys_string );
}

LIST *process_generic_datatype_folder_primary_attribute_data_list(
			LIST *primary_attribute_name_list,
			DICTIONARY *post_dictionary )
{
	char key[ 128 ];
	LIST *primary_attribute_data_list;
	char *primary_attribute_name;
	char *primary_attribute_data;

	if ( !list_rewind( primary_attribute_name_list ) )
		return (LIST *)0;

	primary_attribute_data_list = list_new();

	do {
		primary_attribute_name =
			list_get(
				primary_attribute_name_list );

		sprintf(	key,
				"%s_0",
				primary_attribute_name );

		if ( ! ( primary_attribute_data =
				dictionary_get(
					post_dictionary,
					key ) ) )
		{
			primary_attribute_data =
				dictionary_get(
					post_dictionary,
					primary_attribute_name );
		}

		if ( !primary_attribute_data )
		{
			return (LIST *)0;
		}

		list_set(
			primary_attribute_data_list,
			primary_attribute_data );

	} while( list_next( primary_attribute_name_list ) );

	return primary_attribute_data_list;
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
			char *argv_0 )
{
	char *accumulate_yn;

	PROCESS_GENERIC_PARAMETER *process_generic_parameter =
		process_generic_parameter_calloc();

	process_generic_parameter->output_medium_string = output_medium_string;

	/* Note: 'n' used to mean no-output-to-stdout */
	/* ------------------------------------------ */
	if ( strcmp( output_medium_string, "text_file" ) == 0
	||   strcmp( output_medium_string, "n" ) == 0 )
	{
		process_generic_parameter->output_medium =
			text_file;
	}
	else
	/* ------------------------------------------- */
	/* Note: 'y' used to mean yes-output-to-stdout */
	/* ------------------------------------------- */
	if ( strcmp( output_medium_string, "stdout" ) == 0
	||   strcmp( output_medium_string, "y" ) == 0 )
	{
		process_generic_parameter->output_medium =
			output_medium_stdout;
	}
	else
	if ( strcmp( output_medium_string, "spreadsheet" ) == 0 )
	{
		process_generic_parameter->output_medium =
			spreadsheet;
	}
	else
	if ( strcmp( output_medium_string, "table" ) == 0 )
	{
		process_generic_parameter->output_medium =
			table;
	}
	else
	{
		fprintf(stderr,
"ERROR in %s: output_medium must be either 'stdout','text_file','spreadsheet',or 'table'.\n",
			argv_0 );
		exit( 1 );
	}

	if ( process_generic_parameter->output_medium == table
	||   process_generic_parameter->output_medium == text_file
	||   process_generic_parameter->output_medium == output_medium_stdout )
	{
		process_generic_parameter->delimiter = FOLDER_DATA_DELIMITER;
	}
	else
	/* Pipe to double_quote_comma_delimited.e */
	/* -------------------------------------- */
	{
		process_generic_parameter->delimiter = '\0';
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

	if ( process_generic_parameter->end_date
	&&  *process_generic_parameter->end_date )
	{
		char buffer[ 128 ];

		sprintf( buffer, "_%s", process_generic_parameter->end_date );
		process_generic_parameter->end_date_suffix =
			strdup( buffer );
	}

	process_generic_parameter->process_id = getpid();

	return process_generic_parameter;
}

PROCESS_GENERIC_OUTPUT *process_generic_output_calloc( void )
{
	PROCESS_GENERIC_OUTPUT *process_generic_output;

	if ( ! ( process_generic_output =
			calloc( 1, sizeof( PROCESS_GENERIC_OUTPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return process_generic_output;
}

PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_fetch(
			char *foreign_folder_name,
			DICTIONARY *post_dictionary )
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
		foreign_folder->primary_attribute_name_list;

	process_generic_foreign_folder->foreign_attribute_data_list =
		dictionary_key_list_fetch(
			process_generic_foreign_folder->
				foreign_attribute_name_list,
			process_generic_foreign_folder->post_dictionary );

	if ( !list_length( process_generic_foreign_folder->
				foreign_attribute_data_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: dictionary_key_list_fetch(%s) for [%s] returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			dictionary_display(
				process_generic_foreign_folder->
					post_dictionary ),
			list_display(
				process_generic_foreign_folder->
					foreign_attribute_name_list ) );
		exit( 1 );
	}

	process_generic_foreign_folder->foreign_attribute_name_list_length =
		list_length(
			process_generic_foreign_folder->
				foreign_attribute_name_list );

	return process_generic_foreign_folder;
}

char *process_generic_output_heading(
			LIST *foreign_attribute_name_list,
			char *datatype_unit,
			char *foreign_folder_unit,
			enum aggregate_level aggregate_level,
			char *time_attribute_name,
			boolean accumulate )
{
	static char heading_line[ 1024 ];
	char *ptr = heading_line;
	char *attribute_name;
	char heading[ 128 ];
	LIST *local_foreign_attribute_name_list;
	char *unit;

	if ( datatype_unit )
		unit = datatype_unit;
	else
		unit = foreign_folder_unit;

	if ( time_attribute_name
	&&   *time_attribute_name
	&&   aggregate_level >= daily )
	{
		local_foreign_attribute_name_list =
			list_copy_string_list(
				foreign_attribute_name_list );

		list_subtract_string(
			local_foreign_attribute_name_list,
			time_attribute_name );

		foreign_attribute_name_list =
			local_foreign_attribute_name_list;
	}

	*ptr = '\0';

	if ( !list_rewind( foreign_attribute_name_list ) )
		return heading_line;

	do {
		attribute_name =
			list_get(
				foreign_attribute_name_list );

		if ( !list_at_head( foreign_attribute_name_list ) )
			ptr += sprintf( ptr, "," );

		format_initial_capital( heading, attribute_name );

		if ( unit && list_at_end( foreign_attribute_name_list ) )
		{
			sprintf(heading + strlen( heading ),
				" (%s)",
				unit );
		}

		ptr += sprintf( ptr, "%s", heading );

	} while( list_next( foreign_attribute_name_list ) );

	if ( accumulate )
	{
		sprintf( heading_line + strlen( heading_line ) )
			 ",Accumulate" );
	}

	return heading_line;
}

char *process_generic_output_subtitle(
			char *value_folder_name,
			char *begin_date,
			char *end_date,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic )
{
	static char subtitle[ 256 ];
	char title_aggregate_statistic[ 128 ];
	char title_aggregate_level[ 128 ];
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

	sprintf(subtitle,
		"Beginning: %s Ending: %s%s%s",
		(begin_date) ? begin_date : "",
		(end_date) ? end_date : "",
		title_aggregate_statistic,
		title_aggregate_level );

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

