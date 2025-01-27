/* $APPASERVER_HOME/library/process_generic_output.c			*/
/* -------------------------------------------------------------------- */
/* This is being replace with process_generic.c				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "dictionary.h"
#include "attribute.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "aggregate_statistic.h"
#include "aggregate_level.h"
#include "document.h"
#include "folder.h"
#include "list_usage.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "process_generic_output.h"

PROCESS_GENERIC_OUTPUT *process_generic_output_new(
			char *application_name,
			char *process_name,
			char *process_set_name,
			boolean accumulate )
{
	PROCESS_GENERIC_OUTPUT *process_generic_output;

	process_generic_output =
		(PROCESS_GENERIC_OUTPUT *)
			calloc( 1, sizeof( PROCESS_GENERIC_OUTPUT ) );

	process_generic_output->process_name = process_name;
	process_generic_output->process_set_name = process_set_name;
	process_generic_output->accumulate = accumulate;

	process_generic_output->value_folder =
		process_generic_value_folder_new(
			application_name,
			process_generic_output->process_name,
			process_generic_output->process_set_name );

	return process_generic_output;
}

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_new(
				char *application_name,
				char *process_name,
				char *process_set_name )
{
	PROCESS_GENERIC_VALUE_FOLDER *value_folder;
	FOLDER *folder;
	char *datatype_folder_name;
	char *foreign_folder_name;

	value_folder =
		(PROCESS_GENERIC_VALUE_FOLDER *)
			calloc( 1, sizeof( PROCESS_GENERIC_VALUE_FOLDER ) );

	process_generic_value_folder_load(
		&value_folder->value_folder_name,
		&value_folder->date_attribute_name,
		&value_folder->time_attribute_name,
		&value_folder->value_attribute_name,
		&value_folder->units_folder_name,
		&datatype_folder_name,
		&foreign_folder_name,
		application_name,
		process_name,
		process_set_name );

	value_folder->datatype_folder =
		process_generic_datatype_folder_new(
			application_name,
			datatype_folder_name );

	value_folder->foreign_folder =
		process_generic_foreign_folder_new(
			application_name,
			foreign_folder_name );

	folder = folder_new(	application_name,
				BOGUS_SESSION,
				value_folder->value_folder_name );
	folder->attribute_list =
		folder_get_attribute_list(
			application_name,
			folder->folder_name );

	value_folder->primary_key_list =
		folder_attribute_primary_key_list(
			folder->folder_attribute_list );

	return value_folder;
}

PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_new(
			char *application_name,
			char *foreign_folder_name )
{
	PROCESS_GENERIC_FOREIGN_FOLDER *foreign_folder;
	FOLDER *folder;

	foreign_folder =
		(PROCESS_GENERIC_FOREIGN_FOLDER *)
			calloc( 1, sizeof( PROCESS_GENERIC_FOREIGN_FOLDER ) );

	folder = folder_new(	application_name,
				BOGUS_SESSION,
				foreign_folder_name );
	folder->attribute_list =
		folder_get_attribute_list(
			application_name,
			folder->folder_name );

	foreign_folder->foreign_attribute_name_list =
		folder_primary_key_list(
			folder->folder_attribute_list );

	foreign_folder->foreign_folder_name = foreign_folder_name;

	return foreign_folder;
}

void process_generic_value_folder_load(	char **value_folder_name,
					char **date_attribute_name,
					char **time_attribute_name,
					char **value_attribute_name,
					char **units_folder_name,
					char **datatype_folder_name,
					char **foreign_folder_name,
					char *application_name,
					char *process_name,
					char *process_set_name )
{
	char sys_string[ 1024 ];
	char piece_string[ 128 ];
	char where_clause[ 256 ];
	char *results;
	char *process_generic_output_table_name;
	char *process_generic_value_folder_table_name;
	char select[ 1024 ];
	char *select_template =
"%s.value_folder,date_attribute,time_attribute,value_attribute,process_generic_unit,datatype_folder,foreign_folder";

	process_generic_output_table_name =
		get_table_name(
			application_name,
			"process_generic_output" );

	process_generic_value_folder_table_name =
		get_table_name(
			application_name,
			"process_generic_value_folder" );

	sprintf(	select,
			select_template,
			process_generic_value_folder_table_name );

	if ( process_set_name
	&&   *process_set_name
	&&   strcmp( process_set_name, "null" ) != 0
	&&   strcmp( process_set_name, "process_set" ) != 0 )
	{
		sprintf( where_clause,
		 	 "process_set = '%s'",
			 process_set_name );
	}
	else
	if ( process_name
	&&   *process_name
	&&   strcmp( process_name, "null" ) != 0
	&&   strcmp( process_name, "process" ) != 0 )
	{
		sprintf( where_clause,
		 	 "process = '%s'",
			 process_name );
	}
	else
	{
		fprintf( stderr, 
			"ERROR in %s/%s()/%d: No process nor process set.",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( where_clause + strlen( where_clause ),
		 " and %s.value_folder = %s.value_folder",
		 process_generic_output_table_name,
		 process_generic_value_folder_table_name );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\" | sql.e '%c'",
		 select,
		 process_generic_output_table_name,
		 process_generic_value_folder_table_name,
		 where_clause,
		 FOLDER_DATA_DELIMITER );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: where clause failed [%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			where_clause );

		exit( 1 );
	}

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 0 );
	*value_folder_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 1 );
	*date_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 2 );
	*time_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 3 );
	*value_attribute_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 4 );
	*units_folder_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 5 );
	*datatype_folder_name = strdup( piece_string );

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 6 );
	*foreign_folder_name = strdup( piece_string );

	free( results );
}

PROCESS_GENERIC_DATATYPE_FOLDER *process_generic_datatype_folder_new(
					char *application_name,
					char *datatype_folder_name )
{
	PROCESS_GENERIC_DATATYPE_FOLDER *process_generic_datatype_folder;

	process_generic_datatype_folder =
		(PROCESS_GENERIC_DATATYPE_FOLDER *)
			calloc( 1, sizeof( PROCESS_GENERIC_DATATYPE_FOLDER ) );

	process_generic_datatype_folder->datatype_folder_name =
		datatype_folder_name;

	process_generic_datatype_folder_load(
		&process_generic_datatype_folder->primary_key_list,
		&process_generic_datatype_folder->exists_aggregation_sum,
		&process_generic_datatype_folder->exists_bar_graph,
		&process_generic_datatype_folder->exists_scale_graph_zero,
		application_name,
		process_generic_datatype_folder->datatype_folder_name );

	return process_generic_datatype_folder;
}

void process_generic_datatype_folder_load(
				LIST **primary_key_list,
				boolean *datatype_exists_aggregation_sum,
				boolean *datatype_exists_bar_graph,
				boolean *datatype_exists_scale_graph_zero,
				char *application_name,
				char *datatype_folder_name )
{
	char sys_string[ 1024 ];
	char piece_string[ 128 ];
	char where_clause[ 256 ];
	char *results;
	char *table_name;
	char *order_clause;
	char *select =
"datatype_aggregation_sum_yn,datatype_bar_graph_yn,datatype_scale_graph_zero_yn";

	table_name = get_table_name(
			application_name,
			"process_generic_datatype_folder" );

	sprintf( where_clause, "datatype_folder = '%s'", datatype_folder_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e '%c'",
		 select,
		 table_name,
		 where_clause,
		 FOLDER_DATA_DELIMITER );

	results = pipe2string( sys_string );
	if ( !results )
	{
		char msg[ 1024 ];
		sprintf( msg, 
		"ERROR in %s/%s()/%d: cannot find datatype folder = (%s)",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			datatype_folder_name );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 0 );
	*datatype_exists_aggregation_sum = (*piece_string == 'y');

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 1 );
	*datatype_exists_bar_graph = (*piece_string == 'y');

	piece( piece_string, FOLDER_DATA_DELIMITER, results, 2 );
	*datatype_exists_scale_graph_zero = (*piece_string == 'y');

	free( results );

	select = "attribute";

	table_name = get_table_name(
			application_name,
			"folder_attribute" );

	sprintf(	where_clause,
			"folder = '%s' and primary_key_index >= 1",
			datatype_folder_name );

	order_clause = "order by primary_key_index";

	sprintf( sys_string,
		 "echo \"select %s from %s where %s %s;\" | sql.e",
		 select,
		 table_name,
		 where_clause,
		 order_clause );

	*primary_key_list = pipe2list( sys_string );

}

LIST *process_generic_get_compare_datatype_list(
			char *application_name,
			LIST *foreign_attribute_name_list,
			char *datatype_folder_name,
			LIST *datatype_primary_key_list,
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
			datatype_primary_key_list,
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
			datatype_primary_key_list,
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
				process_generic_get_values_hash_table(
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

HASH_TABLE *process_generic_get_values_hash_table(
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


PROCESS_GENERIC_DATATYPE *process_generic_datatype_new(
			char *application_name,
			LIST *foreign_attribute_name_list,
			char *datatype_folder_name,
			LIST *datatype_primary_key_list,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			char *units_folder_name,
			DICTIONARY *post_dictionary,
			int dictionary_index )
{
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;

	process_generic_datatype =
		(PROCESS_GENERIC_DATATYPE *)
			calloc( 1, sizeof( PROCESS_GENERIC_DATATYPE ) );
	
	if ( ! ( process_generic_datatype->primary_attribute_data_list =
		process_generic_get_datatype_primary_attribute_data_list(
				post_dictionary,
				datatype_primary_key_list,
				(char *)0 /* compare_datatype_prefix */,
				dictionary_index /* dictionary_index */ ) ) )
	{
		return (PROCESS_GENERIC_DATATYPE *)0;
	}

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
		datatype_primary_key_list,
		process_generic_datatype->primary_attribute_data_list,
		units_folder_name );

	process_generic_datatype->foreign_attribute_data_list =
		dictionary_using_list_get_index_data_list(
			post_dictionary,
			foreign_attribute_name_list,
			dictionary_index );

	return process_generic_datatype;

}

void process_generic_datatype_load(
				boolean *aggregation_sum,
				boolean *bar_graph,
				boolean *scale_graph_zero,
				char **units,
				boolean exists_aggregation_sum,
				boolean exists_bar_graph,
				boolean exists_scale_graph_zero,
				char *application_name,
				char *datatype_folder_name,
				LIST *datatype_primary_key_list,
				LIST *datatype_primary_attribute_data_list,
				char *units_folder_name )
{
	char *where_clause;
	char sys_string[ 1024 ];
	char piece_buffer[ 16 ];
	char *results;
	int piece_int = 0;
	char select_string[ 1024 ];
	char *select_ptr = select_string;

	if ( exists_aggregation_sum )
	{
		select_ptr += sprintf( select_ptr, "aggregation_sum_yn" );
	}

	if ( exists_bar_graph )
	{
		if ( select_ptr != select_string )
			select_ptr += sprintf( select_ptr, "," );

		select_ptr += sprintf( select_ptr, "bar_graph_yn" );
	}

	if ( exists_scale_graph_zero )
	{
		if ( select_ptr != select_string )
			select_ptr += sprintf( select_ptr, "," );

		select_ptr += sprintf( select_ptr, "scale_graph_to_zero_yn" );
	}

	if ( units_folder_name
	&&   strcmp( units_folder_name, "datatype_folder" ) == 0 )
	{
		if ( select_ptr != select_string )
			select_ptr += sprintf( select_ptr, "," );

		select_ptr += sprintf( select_ptr, "units" );
	}

	if ( ! ( where_clause =
		process_generic_get_datatype_where_clause(
			datatype_primary_key_list,
			datatype_primary_attribute_data_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get where clause.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(	sys_string,
			"get_folder_data	application=%s		"
			"			select=\"%s\"		"
			"			folder=%s		"
			"			where=\"%s\"		",
			application_name,
			select_string,
			datatype_folder_name,
			where_clause );

	results = pipe2string( sys_string );

	if ( !results )
	{
		fprintf( stderr, 
"ERROR in %s/%s()/%d: cannot fetch datatype information for datatype = (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display( datatype_primary_attribute_data_list ) );
		exit( 1 );
	}

	if ( exists_aggregation_sum )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			results,
			piece_int );

		*aggregation_sum = (*piece_buffer == 'y' );
		piece_int++;
	}

	if ( exists_bar_graph )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			results,
			piece_int );

		*bar_graph = (*piece_buffer == 'y' );
		piece_int++;
	}
	else
	{
		*bar_graph = 1;
	}

	if ( exists_scale_graph_zero )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			results,
			piece_int );

		*scale_graph_zero = (*piece_buffer == 'y' );
		piece_int++;
	}

	if ( units_folder_name
	&&   strcmp( units_folder_name, "datatype_folder" ) == 0 )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			results,
			piece_int );

		*units = strdup( piece_buffer );
	}
	else
	{
		*units = "";
	}

	free( results );
}

char *process_generic_get_datatype_where_clause(
			LIST *primary_key_list,
			LIST *primary_attribute_data_list )
{
	QUERY *query;

	query = query_new(	(char *)0 /* application_name */,
				(char *)0 /* login_name */,
				(char *)0 /* folder_name */,
				(LIST *)0 /* attribute_list */,
				(DICTIONARY *)0 /* dictionary */,
				(DICTIONARY *)0 /* sort_dictionary */,
				(ROLE *)0 /* role */,
				primary_key_list,
				primary_attribute_data_list,
				0 /* max_rows */,
				0 /* not include_root_folder */,
				(LIST *)0
					/* one2m_subquery_folder_name_list */,
				(RELATED_FOLDER *)0
					/* root_related_folder */ );

	return query->query_output->where_clause;
}

LIST *process_generic_get_datatype_primary_attribute_data_list(
			DICTIONARY *post_dictionary,
			LIST *datatype_primary_key_list,
			char *compare_datatype_prefix,
			int dictionary_index )
{
	char key[ 128 ];
	LIST *full_primary_attribute_data_list;
	LIST *primary_attribute_data_list;
	char *primary_key;
	char *primary_attribute_data;

	if ( compare_datatype_prefix )
	{
		if ( ! ( full_primary_attribute_data_list =
				dictionary_get_prefixed_indexed_data_list(
				     post_dictionary,
				     compare_datatype_prefix,
				     dictionary_index,
				     MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) ) )
		{
			return (LIST *)0;
		}

		if ( ! ( primary_attribute_data_list =
		     process_generic_get_prefixed_primary_attribute_data_list(
				full_primary_attribute_data_list,
				list_length(
					datatype_primary_key_list
			) ) ) )
		{
			return (LIST *)0;
		}

		return primary_attribute_data_list;
	}

	if ( !list_rewind( datatype_primary_key_list ) )
		return (LIST *)0;

	primary_attribute_data_list = list_new();

	do {
		primary_key =
			list_get_pointer(
				datatype_primary_key_list );

		sprintf(	key,
				"%s_%d",
				primary_key,
				dictionary_index );

		if ( ! ( primary_attribute_data =
				dictionary_get_pointer(
					post_dictionary,
					key ) ) )
		{
			primary_attribute_data =
				dictionary_get_pointer(
					post_dictionary,
					primary_key );
		}

		if ( !primary_attribute_data )
		{
			return (LIST *)0;
		}
		list_append_pointer(	primary_attribute_data_list,
					primary_attribute_data );
	} while( list_next( datatype_primary_key_list ) );
	return primary_attribute_data_list;
}

char *process_generic_output_get_begin_end_date_where(
			char **begin_date,
			char **end_date,
			DICTIONARY *dictionary,
			char *application_name,
			char *value_folder_name,
			char *date_attribute_name,
			char *original_where_clause )
{
	static char where_clause[ 256 ];

	if ( !*begin_date )
	{
		if ( !dictionary )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: empty dictionary to get begin_date from.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		*begin_date =
			dictionary_fetch_index_zero(
				dictionary,
				"begin_date" );
	}

	if ( !*end_date )
	{
		if ( !dictionary )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: empty dictionary to get end_date from.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		*end_date =
			dictionary_fetch_index_zero(
				dictionary,
				"end_date" );
	}

	if ( *begin_date && **begin_date
	&&   *end_date && **end_date )
	{
		sprintf(	where_clause,
				" and %s >= '%s' and %s <= '%s'",
				date_attribute_name,
				*begin_date,
				date_attribute_name,
				*end_date );
	}
	else
	if ( *begin_date && **begin_date )
	{
		sprintf(	where_clause,
				" and %s = '%s'",
				date_attribute_name,
				*begin_date );
	}
	else
	if ( *end_date && **end_date )
	{
		sprintf(	where_clause,
				" and %s = '%s'",
				date_attribute_name,
				*end_date );
		*begin_date = *end_date;
	}
	else
	{
		if ( !value_folder_name )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: empty value_folder_name to get period of record dates.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		process_generic_output_get_period_of_record_date(
				begin_date,
				application_name,
				value_folder_name,
				"min",
				date_attribute_name,
				original_where_clause );

		process_generic_output_get_period_of_record_date(
				end_date,
				application_name,
				value_folder_name,
				"max",
				date_attribute_name,
				original_where_clause );

		sprintf(	where_clause,
				" and %s >= '%s' and %s <= '%s'",
				date_attribute_name,
				*begin_date,
				date_attribute_name,
				*end_date );
	}

	return where_clause;
}

LIST *process_generic_output_get_concat_heading_list(
				PROCESS_GENERIC_VALUE_FOLDER *value_folder,
				enum aggregate_level aggregate_level )
{
	LIST *heading_list = list_new();
	char heading[ 128 ];
	char *ptr = heading;
	char *foreign_attribute_name;
	LIST *local_foreign_attribute_name_list;

	local_foreign_attribute_name_list =
		value_folder->foreign_folder->foreign_attribute_name_list;

	local_foreign_attribute_name_list =
		list_subtract_list(
			local_foreign_attribute_name_list,
			value_folder->
				datatype_folder->
				primary_key_list );

	if ( !list_rewind( local_foreign_attribute_name_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: empty local_foreign_attribute_name_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		foreign_attribute_name =
			list_get_pointer(
				local_foreign_attribute_name_list );

		if ( !list_at_head( local_foreign_attribute_name_list ) )
			ptr += sprintf( ptr, "/" );

		ptr += sprintf( ptr, "%s", foreign_attribute_name );
	} while( list_next( local_foreign_attribute_name_list ) );

	list_append_pointer(
			heading_list,
			strdup( format_initial_capital(
				heading,
				heading ) ) );

	list_append_pointer(
			heading_list,
			strdup( format_initial_capital(
				heading,
				value_folder->
				datatype_folder->
				datatype_folder_name ) ) );

	list_append_pointer(
			heading_list,
			strdup( format_initial_capital(
				heading,
				value_folder->date_attribute_name ) ) );

	if ( ( aggregate_level == aggregate_level_none
	||     aggregate_level == real_time
	||     aggregate_level == half_hour
	||     aggregate_level == hourly )
	&&   value_folder->time_attribute_name
	&&   *value_folder->time_attribute_name )
	{
		list_append_pointer(
				heading_list,
				strdup( format_initial_capital(
					heading,
					value_folder->time_attribute_name ) ) );
	}

	list_append_pointer(
			heading_list,
			strdup( format_initial_capital(
				heading,
				value_folder->value_attribute_name ) ) );

	return heading_list;

}

char *process_generic_output_get_heading_string(
				PROCESS_GENERIC_VALUE_FOLDER *value_folder,
				char heading_delimiter,
				enum aggregate_level aggregate_level )
{
	static char heading_string[ 1024 ];
	char *ptr = heading_string;
	char *select;
	char heading[ 128 ];
	LIST *local_foreign_attribute_name_list;

	local_foreign_attribute_name_list =
		list_copy_string_list(
				value_folder->
				foreign_folder->
				foreign_attribute_name_list );

	*ptr = '\0';

	if ( !list_rewind( local_foreign_attribute_name_list ) )
		return heading_string;

	do {
		select = list_get_pointer( local_foreign_attribute_name_list );
		if ( !list_at_head( local_foreign_attribute_name_list ) )
			ptr += sprintf( ptr, "%c", heading_delimiter );
		format_initial_capital( heading, select );
		ptr += sprintf( ptr, "%s", heading );
	} while( list_next( local_foreign_attribute_name_list ) );

	ptr += sprintf( ptr,
			"%c%s",
			heading_delimiter,
			format_initial_capital(
				heading,
				value_folder->date_attribute_name ) );

	if ( ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	&&   value_folder->time_attribute_name
	&&   *value_folder->time_attribute_name )
	{
		ptr += sprintf( ptr,
				"%c%s",
				heading_delimiter,
				format_initial_capital(
					heading,
					value_folder->time_attribute_name ) );
	}

	ptr += sprintf( ptr,
			"%c%s",
			heading_delimiter,
			format_initial_capital(
				heading,
			value_folder->value_attribute_name ) );

	*ptr = '\0';
	return heading_string;

}

char *process_generic_output_get_text_file_sys_string(
				char **begin_date,
				char **end_date,
				char **where_clause,
				char **units_label,
				int *datatype_entity_piece,
				int *datatype_piece,
				int *date_piece,
				int *time_piece,
				int *value_piece,
				int *length_select_list,
				char *application_name,
				PROCESS_GENERIC_OUTPUT *
					process_generic_output,
				DICTIONARY *post_dictionary,
				char delimiter,
				enum aggregate_level aggregate_level,
				enum aggregate_statistic aggregate_statistic,
				boolean append_low_high,
				boolean concat_datatype_entity,
				boolean concat_datatype,
				boolean accumulate )
{
	static char sys_string[ 65536 ] = {0};
	static char local_where_clause[ 65536 ];
	LIST *select_list;
	char *heading_string;
	int local_date_piece = 0;
	int local_time_piece = 0;
	int local_value_piece = 0;
	int accumulate_piece;
	char aggregation_process[ 256 ];
	char sort_process[ 128 ];
	char units_converted_process[ 128 ];
	char *units_converted;
	char character_translate_process[ 128 ];
	int right_justified_columns_from_right;
	char heading_delimiter;
	char accumulation_process[ 256 ];
	char accumulate_label[ 32 ];

	if ( aggregate_level == real_time )
	{
		right_justified_columns_from_right = 1;
	}
	else
	{
		right_justified_columns_from_right = 2;
	}

	strcpy(	local_where_clause,
		process_generic_output_get_dictionary_where_clause(
			begin_date,
			end_date,
			application_name,
			process_generic_output,
			post_dictionary,
			1 /* with_set_dates */,
			process_generic_output->
				value_folder->
				value_folder_name ) );


	select_list = process_generic_output_get_select_list(
				datatype_entity_piece,
				datatype_piece,
				&local_date_piece,
				&local_time_piece,
				&local_value_piece,
				length_select_list,
				process_generic_output->value_folder,
				concat_datatype_entity,
				concat_datatype );

	if ( date_piece ) *date_piece = local_date_piece;
	if ( time_piece ) *time_piece = local_time_piece;
	if ( value_piece ) *value_piece = local_value_piece;

	if ( time_piece && *time_piece != -1 )
		accumulate_piece = local_date_piece + 2;
	else
		accumulate_piece = local_date_piece + 1;

	if ( accumulate ) right_justified_columns_from_right++;

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

	if ( accumulate )
	{
		sprintf( accumulation_process, 
			 "sort | accumulate.e %d '%c' append",
			 accumulate_piece,
			 PROCESS_GENERIC_OUTPUT_DELIMITER );

		sprintf( accumulate_label, "%cAccumulate", heading_delimiter );
	}
	else
	{
		strcpy( accumulation_process, "cat" );
		*accumulate_label = '\0';
	}

	heading_string =
		process_generic_output_get_heading_string(
			process_generic_output->value_folder,
			heading_delimiter,
			aggregate_level );

	if ( aggregate_level == real_time )
	{
		strcpy( aggregation_process, "cat" );
		strcpy( sort_process, "sort" );
	}
	else
	{
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
	}

	units_converted =
		dictionary_get_index_zero(
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
			 "measurement_convert_units.e '%s' '%s' %d '%c'",
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

	*units_label =
		process_generic_output_get_units_label(
			process_generic_output->
				value_folder->
					datatype->units,
			units_converted,
			aggregate_statistic );

	sprintf( sys_string,
		 "(echo \"#%s%c%s%s%s\"					 ;"
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select=\"%s\"			  "
		 "			where=\"%s\"			 |"
		 "%s							 |"
		 "tr '%c' '%c'						 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "sort) | cat				 		  ",
		 heading_string,
		 ' ' /* heading_delimiter */,
		 *units_label,
		 accumulate_label,
		 aggregate_level_get_summation_heading(
			aggregate_level,
			heading_delimiter ),
		 application_name,
		 process_generic_output->value_folder->value_folder_name,
		 list_display_delimited( select_list, ',' ),
		 local_where_clause,
		 sort_process,
		 FOLDER_DATA_DELIMITER,
		 PROCESS_GENERIC_OUTPUT_DELIMITER,
		 units_converted_process,
		 aggregation_process,
		 accumulation_process,
		 character_translate_process );

	*where_clause = local_where_clause;

	return sys_string;
}

LIST *process_generic_output_get_select_list(
				int *datatype_entity_piece,
				int *datatype_piece,
				int *date_piece,
				int *time_piece,
				int *value_piece,
				int *length_select_list,
				PROCESS_GENERIC_VALUE_FOLDER *value_folder,
				boolean concat_datatype_entity,
				boolean concat_datatype )
{
	LIST *select_list = list_new();
	LIST *local_foreign_attribute_name_list;
	char *datatype_entity_concat_select;
	char *datatype_concat_select;

	if ( datatype_piece ) *datatype_piece = 0;
	if ( date_piece ) *date_piece = 0;
	if ( time_piece ) *time_piece = 0;
	if ( value_piece ) *value_piece = 0;
	if ( length_select_list ) *length_select_list = 0;

	local_foreign_attribute_name_list =
		list_copy_string_list(
				value_folder->
				foreign_folder->
				foreign_attribute_name_list );

	local_foreign_attribute_name_list =
		list_subtract_list(
			local_foreign_attribute_name_list,
			value_folder->
			datatype_folder->
			primary_key_list );

	if ( concat_datatype_entity )
	{
		datatype_entity_concat_select =
			list_usage_concat(
				local_foreign_attribute_name_list );

		list_append_pointer(
			select_list,
			datatype_entity_concat_select );

		if ( datatype_entity_piece ) *datatype_entity_piece = 0;
		if ( datatype_piece ) (*datatype_piece)++;
		if ( date_piece ) (*date_piece)++;
		if ( time_piece ) (*time_piece)++;
		if ( value_piece ) (*value_piece)++;
		if ( length_select_list ) (*length_select_list)++;
	}
	else
	{
		int length;

		length = list_length( local_foreign_attribute_name_list );

		list_append_list(
			select_list,
			local_foreign_attribute_name_list );


		if ( datatype_entity_piece ) *datatype_entity_piece = 0;
		if ( datatype_piece ) (*datatype_piece) += length;
		if ( date_piece ) (*date_piece) += length;
		if ( time_piece ) (*time_piece) += length;
		if ( value_piece ) (*value_piece) += length;
		if ( length_select_list ) (*length_select_list) += length;
	}

	if ( concat_datatype )
	{
		datatype_concat_select =
			list_usage_concat(
				value_folder->
				datatype_folder->
				primary_key_list );

		list_append_pointer(
			select_list,
			datatype_concat_select );

		if ( date_piece ) (*date_piece)++;
		if ( time_piece ) (*time_piece)++;
		if ( value_piece ) (*value_piece)++;
		if ( length_select_list ) (*length_select_list)++;
	}
	else
	{
		int length;

		length = list_length(
				value_folder->
				datatype_folder->
				primary_key_list );

		list_append_list(
			select_list,
			value_folder->
			datatype_folder->
			primary_key_list );

		if ( date_piece ) (*date_piece) += length;
		if ( time_piece ) (*time_piece) += length;
		if ( value_piece ) (*value_piece) += length;
		if ( length_select_list ) (*length_select_list) += length;
	}

	list_append_pointer(
		select_list,
		value_folder->date_attribute_name );

	if ( time_piece ) (*time_piece)++;
	if ( value_piece ) (*value_piece)++;
	if ( length_select_list ) (*length_select_list)++;

	if ( value_folder->time_attribute_name
	&&   *value_folder->time_attribute_name )
	{
		list_append_pointer(
			select_list,
			value_folder->time_attribute_name );

		if ( value_piece ) (*value_piece)++;
		if ( length_select_list ) (*length_select_list)++;
	}
	else
	{
		if ( time_piece ) *time_piece = -1;
	}

	list_append_pointer(
		select_list,
		value_folder->value_attribute_name );
	if ( length_select_list ) (*length_select_list)++;

	return select_list;

}

LIST *process_generic_output_append_select_list(
					LIST *select_list,
					LIST *attribute_list,
					DICTIONARY *dictionary )
{
	ATTRIBUTE *attribute;
	char *select_string;

	if ( !list_rewind( attribute_list ) ) return select_list;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( dictionary_get_index_data_if_populated(
					&select_string,
					dictionary,
					attribute->attribute_name,
					0 /* index */ ) )
		{
			list_append_pointer(
				select_list,
				attribute->attribute_name );
		}
	} while( list_next( attribute_list ) );
	return select_list;
}

char *process_generic_output_get_process_name(
			char *process_set_name,
			DICTIONARY *post_dictionary )
{
	char key[ 128 ];

	sprintf( key, "%s_0", process_set_name );
	return dictionary_get_pointer( post_dictionary, key );
}

char *process_generic_output_get_datatype_entity_data(
			DICTIONARY *post_dictionary,
			char *datatype_entity_attribute )
{
	char key[ 128 ];

	sprintf( key, "%s_0", datatype_entity_attribute );
	return dictionary_get_pointer( post_dictionary, key );
}

char *process_generic_output_get_units(
				char *application_name,
				char *datatype_folder_name,
				LIST *datatype_primary_key_list,
				DICTIONARY *post_dictionary )
{
	char sys_string[ 1024 ];
	LIST *datatype_primary_attribute_data_list;
	char *where_clause;

	if ( ! ( datatype_primary_attribute_data_list =
		process_generic_get_datatype_primary_attribute_data_list(
				post_dictionary,
				datatype_primary_key_list,
				(char *)0 /* compare_datatype_prefix */,
				0 /* dictionary_index */ ) ) )
	{
		return (char *)0;
	}

	if ( ! ( where_clause =
		process_generic_get_datatype_where_clause(
			datatype_primary_key_list,
			datatype_primary_attribute_data_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get where clause.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=units			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 datatype_folder_name,
		 where_clause );
	return pipe2string( sys_string );
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
			 "measurement_convert_units.e %s %s %d '%c'",
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
			 "measurement_convert_units.e %s %s %d '%c'",
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

LIST *process_generic_output_get_primary_attribute_data_list(
				LIST *primary_key_list,
				DICTIONARY *post_dictionary,
				int index_offset )
{
	LIST *primary_attribute_data_list = {0};
	char key[ 128 ];
	char *data;

	if ( post_dictionary && list_rewind( primary_key_list ) )
	{
		primary_attribute_data_list = list_new_list();
		do {
			sprintf(	key,
					"%s_%d",
					(char *)list_get_pointer(
						primary_key_list ),
					index_offset );

			if ( ! ( data = dictionary_get_pointer(
						post_dictionary,
						key ) ) )
			{
				primary_attribute_data_list = (LIST *)0;
				break;
			}

			list_append_pointer(
				primary_attribute_data_list,
				data );

		} while( list_next( primary_key_list ) );
	}

	return primary_attribute_data_list;
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

char *process_generic_output_get_row_dictionary_where_clause(
			char **begin_date,
			char **end_date,
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			DICTIONARY *dictionary,
			boolean with_set_dates,
			int row )
{
	static char where_clause[ 4096 ];
	char *drop_down_where_clause;
	QUERY *query;
	LIST *attribute_data_list;

	if ( ! ( attribute_data_list =
			dictionary_get_data_list(
				process_generic_output->
					value_folder->
					foreign_folder->
					foreign_attribute_name_list,
				dictionary,
				row ) ) )
	{
		return (char*)0;
	}

	query = query_new(	application_name,
				(char *)0 /* login_name */,
				(char *)0 /* folder_name */,
				(LIST *)0 /* attribute_list */,
				(DICTIONARY *)0 /* dictionary */,
				(DICTIONARY *)0 /* sort_dictionary */,
				(ROLE *)0 /* role */,
				process_generic_output->
					value_folder->
					foreign_folder->
					foreign_attribute_name_list,
				attribute_data_list,
				0 /* max_rows */,
				0 /* not include_root_folder */,
				(LIST *)0
					/* one2m_subquery_folder_name_list */,
				(RELATED_FOLDER *)0
					/* root_related_folder */ );

	drop_down_where_clause =
		query->query_output->where_clause;

	*where_clause = '\0';

	if ( drop_down_where_clause && *drop_down_where_clause )
	{
		strcpy( where_clause, drop_down_where_clause );
	}

	if ( with_set_dates )
	{
		strcat(	where_clause,
			process_generic_output_get_begin_end_date_where(
				begin_date,
				end_date,
				dictionary,
				application_name,
				process_generic_output->
					value_folder->value_folder_name,
				process_generic_output->
					value_folder->date_attribute_name,
				where_clause ) );
	}

	return where_clause;

}

char *process_generic_output_get_drop_down_where_clause(
			char *application_name,
			char *value_folder_name,
			DICTIONARY *dictionary )
{
	QUERY *query;

	query = query_process_drop_down_new(
			application_name,
			value_folder_name,
			dictionary /* query_dictionary */ );

	if ( !query->query_output )
		return (char *)0;
	else
		return query->query_output->drop_down_where_clause;

}

char *process_generic_output_get_dictionary_where_clause(
			char **begin_date,
			char **end_date,
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			DICTIONARY *dictionary,
			boolean with_set_dates,
			char *value_folder_name )
{
	static char where_clause[ 4096 ];
	char *drop_down_where_clause;

	drop_down_where_clause =
		process_generic_output_get_drop_down_where_clause(
			application_name,
			value_folder_name,
			dictionary );

	if ( drop_down_where_clause && *drop_down_where_clause )
	{
		strcpy( where_clause, drop_down_where_clause );
	}

	if ( with_set_dates )
	{
		strcat(	where_clause,
			process_generic_output_get_begin_end_date_where(
				begin_date,
				end_date,
				dictionary,
				application_name,
				value_folder_name,
				process_generic_output->
					value_folder->date_attribute_name,
				where_clause ) );
	}

	return where_clause;

}

char *process_generic_compare_datatype_list_display(
					LIST *compare_datatype_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;

	if ( !compare_datatype_list
	||   !list_rewind( compare_datatype_list ) )
	{
		return "";
	}

	*ptr = '\0';

	do {
		process_generic_datatype =
			list_get_pointer(
				compare_datatype_list );

		ptr += sprintf(	ptr,
				"%s\n",
				process_generic_datatype_display(
					process_generic_datatype ) );

	} while( list_next( compare_datatype_list ) );
	return strdup( buffer );
}

char *process_generic_datatype_display(
		PROCESS_GENERIC_DATATYPE *process_generic_datatype )
{
	static char buffer[ 512 ];

	sprintf(buffer,
		"primary_attribute_data_list = \"%s\","
		"units = '%s',"
		"aggregation_sum = %d,"
		"bar_graph = %d,"
		"scale_graph_zero = %d\n"
		"foreign_attribute_data_list = %s\n"
		"length hash_table = %d\n",
		list_display(
			process_generic_datatype->
				primary_attribute_data_list ),
		process_generic_datatype->
			units,
		process_generic_datatype->
			aggregation_sum,
		process_generic_datatype->
			bar_graph,
		process_generic_datatype->
			scale_graph_zero,
	        list_display( process_generic_datatype->
				foreign_attribute_data_list ),
		hash_table_length(	process_generic_datatype->
					values_hash_table ) );
	return buffer;
}

char *process_generic_output_get_foreign_folder_where_clause(
			char **begin_date,
			char **end_date,
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			DICTIONARY *dictionary,
	       		char *application_name,
			char *value_folder_name,
			char *date_attribute_name )
{
	char *foreign_attribute_name;
	char *foreign_attribute_data;
	static char where_clause[ 1024 ];
	char escaped_data[ 1024 ];
	char *where_ptr = where_clause;

	if ( !list_length( foreign_attribute_name_list )
	&&    list_length( foreign_attribute_name_list ) !=
	      list_length( foreign_attribute_data_list ) )
	{
		return (char *)0;
	}

	*where_clause = '\0';

	list_rewind( foreign_attribute_name_list );
	list_rewind( foreign_attribute_data_list );
	do {
		foreign_attribute_name =
			list_get_pointer( foreign_attribute_name_list );

		foreign_attribute_data =
			list_get_pointer( foreign_attribute_data_list );

		if ( !list_at_head( foreign_attribute_name_list ) )
			where_ptr += sprintf( where_ptr, " and" );

		strcpy( escaped_data,
			foreign_attribute_data );
		escape_single_quotes( escaped_data );

		where_ptr += sprintf(	where_ptr,
					" %s = '%s'",
					foreign_attribute_name,
					escaped_data );
		list_next( foreign_attribute_data_list );
	} while( list_next( foreign_attribute_name_list ) );

	where_ptr += sprintf(
			where_ptr, "%s",
			process_generic_output_get_begin_end_date_where(
				begin_date,
				end_date,
				dictionary,
				application_name,
				value_folder_name,
				date_attribute_name,
				where_clause ) );

	return where_clause;
}

PROCESS_GENERIC_VALUE *process_generic_value_new( void )
{
	PROCESS_GENERIC_VALUE *process_generic_value;

	if ( ! ( process_generic_value =
		(PROCESS_GENERIC_VALUE *)
			calloc( 1, sizeof( PROCESS_GENERIC_VALUE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation failed.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_generic_value;
}

char *process_generic_get_datatype_name(
				LIST *datatype_primary_attribute_data_list,
				char delimiter )
{
	return list_display_delimited(
			datatype_primary_attribute_data_list,
			delimiter );
}

LIST *process_generic_get_prefixed_primary_attribute_data_list(
			LIST *full_primary_attribute_data_list,
			int length_datatype_primary_key_list )
{
	int number_to_skip;
	LIST *primary_attribute_data_list;

	number_to_skip =
		list_length( full_primary_attribute_data_list ) -
		length_datatype_primary_key_list;

	if ( number_to_skip <= 0 )
	{
		return (LIST *)0;
	}

	primary_attribute_data_list = list_new();

	list_rewind( full_primary_attribute_data_list );
	do {
		if ( --number_to_skip < 0 )
		{
			list_append_pointer(
				primary_attribute_data_list,
				list_get_pointer(
					full_primary_attribute_data_list ) );
		}
	} while( list_next( full_primary_attribute_data_list ) );
	return primary_attribute_data_list;
}

char *process_generic_get_datatype_entity(
				LIST *foreign_attribute_data_list,
				LIST *datatype_primary_key_list,
				char delimiter )
{
	char datatype_entity[ 512 ];
	char *datatype_entity_ptr = datatype_entity;
	char *foreign_attribute_data;
	boolean first_time = 1;
	int number_of_elements_to_include;

	if ( !delimiter ) delimiter = PROCESS_GENERIC_ENTITY_DELIMITER;

	number_of_elements_to_include =
		list_length( foreign_attribute_data_list ) -
		list_length( datatype_primary_key_list );

	*datatype_entity = '\0';
	if ( !list_rewind( foreign_attribute_data_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty foreign_attribute_data_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 0 );
	}

	do {
		if ( first_time )
			first_time = 0;
		else
			datatype_entity_ptr +=
				sprintf(datatype_entity_ptr,
					"%c",
					delimiter );

		foreign_attribute_data =
				(char *)list_get_pointer(
					foreign_attribute_data_list );

		datatype_entity_ptr +=
			sprintf(datatype_entity_ptr,
				"%s",
				foreign_attribute_data );

		if ( !--number_of_elements_to_include ) break;

	} while( list_next( foreign_attribute_data_list ) );
	return strdup( datatype_entity );
}

LIST *process_generic_output_get_compare_datatype_name_list(
				LIST *compare_datatype_list )
{
	PROCESS_GENERIC_DATATYPE *compare_datatype;
	LIST *compare_datatype_name_list;

	if ( !list_rewind( compare_datatype_list ) )
	{
		return (LIST *)0;
	}

	compare_datatype_name_list = list_new();

	do {
		compare_datatype =
			list_get_pointer( compare_datatype_list );

		list_append_pointer(
			compare_datatype_name_list,
			process_generic_get_datatype_name(
				compare_datatype->
					primary_attribute_data_list,
				' ' /* delimiter */ ) );

	} while( list_next( compare_datatype_list ) );

	return compare_datatype_name_list;
	
}

LIST *process_generic_output_get_compare_entity_name_list(
				LIST *compare_datatype_list,
				LIST *primary_key_list )
{
	PROCESS_GENERIC_DATATYPE *compare_datatype;
	LIST *compare_entity_name_list;

	if ( !list_rewind( compare_datatype_list ) )
	{
		return (LIST *)0;
	}

	compare_entity_name_list = list_new();

	do {
		compare_datatype =
			list_get_pointer( compare_datatype_list );

		list_append_pointer(
			compare_entity_name_list,
			process_generic_get_datatype_entity(
				compare_datatype->
					foreign_attribute_data_list,
				primary_key_list,
				' ' /* delimiter */ ) );

	} while( list_next( compare_datatype_list ) );

	return compare_entity_name_list;
	
}

LIST *process_generic_output_get_compare_datatype_units_list(
				LIST *compare_datatype_list )
{
	PROCESS_GENERIC_DATATYPE *compare_datatype;
	LIST *compare_datatype_units_list;

	if ( !list_rewind( compare_datatype_list ) )
	{
		return (LIST *)0;
	}

	compare_datatype_units_list = list_new();

	do {
		compare_datatype =
			list_get_pointer( compare_datatype_list );

		list_append_pointer(
			compare_datatype_units_list,
			compare_datatype->units );

	} while( list_next( compare_datatype_list ) );

	return compare_datatype_units_list;
	
}

enum aggregate_statistic
		process_generic_output_get_database_aggregate_statistic(
				char *application_name,
				char *appaserver_mount_point,
				LIST *primary_attribute_data_list,
				boolean exists_aggregation_sum )
{
	char *datatype_name;
	enum aggregate_statistic aggregate_statistic;

	if ( !exists_aggregation_sum ) return sum;

	if ( !list_length( primary_attribute_data_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty primary_attribute_data_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype_name =
		process_generic_get_datatype_name(
				primary_attribute_data_list,
				'^' );

	aggregate_statistic =
		appaserver_based_on_datatype_get_aggregate_statistic(
			application_name,
			appaserver_mount_point,
			datatype_name );

	return aggregate_statistic;

}

char *process_generic_output_get_datatype_heading_string(
			PROCESS_GENERIC_FOREIGN_FOLDER *foreign_folder,
			PROCESS_GENERIC_DATATYPE *datatype,
			char heading_delimiter )
{
	static char heading_string[ 1024 ];
	char *ptr = heading_string;
	char *foreign_name;
	char *foreign_data;
	char heading[ 128 ];

	if ( !list_rewind( foreign_folder->foreign_attribute_name_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty foreign_attribute_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if (	list_length( foreign_folder->foreign_attribute_name_list ) !=
		list_length( datatype->foreign_attribute_data_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: length( primary_attribute_data_list ) <> length( foreign_attribute_data_list ): %s <> %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_display(
				foreign_folder->foreign_attribute_name_list ),
			 list_display(
				datatype->foreign_attribute_data_list ) );
		exit( 1 );
	}

	list_rewind( datatype->foreign_attribute_data_list );

	do {
		foreign_name =
			list_get_pointer(
				foreign_folder->foreign_attribute_name_list );

		foreign_data =
			list_get_pointer(
				datatype->foreign_attribute_data_list );

		if ( !list_at_head(
			foreign_folder->foreign_attribute_name_list ) )
		{
			ptr += sprintf( ptr, "%c", heading_delimiter );
		}

		sprintf( heading, "%s: %s", foreign_name, foreign_data );

		format_initial_capital( heading, heading );

		ptr += sprintf( ptr, "%s", heading );

		list_next( foreign_folder->foreign_attribute_name_list );

	} while( list_next( datatype->foreign_attribute_data_list ) );

	return heading_string;

}

#ifdef NOT_DEFINED
QUERY_OUTPUT *process_generic_query_output(
				char *application_name,
				char *value_folder_name,
				LIST *mto1_related_folder_list,
				DICTIONARY *query_removed_post_dictionary )
{
	QUERY_OUTPUT *query_output;

	query_output = query_output_calloc();

	query_output->
		query_drop_down_list =
			query_process_generic_drop_down_list(
				value_folder_name,
				mto1_related_folder_list,
				query_removed_post_dictionary );

	query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			application_name,
			value_folder_name,
			1 /* combine_date_time */ );

	return query_output;

}
#endif

/* ------------------------------- */
/* Returns static (program) memory */
/* ------------------------------- */
char *process_generic_output_begin_end_date_where_clause(
			char *date_attribute_name,
			char *begin_date_string,
			char *end_date_string )
{
	static char where_clause[ 256 ];

	if ( begin_date_string && *begin_date_string
	&&   end_date_string && *end_date_string )
	{
		sprintf(	where_clause,
				" and %s >= '%s' and %s <= '%s'",
				date_attribute_name,
				begin_date_string,
				date_attribute_name,
				end_date_string );
	}
	else
	if ( begin_date_string && *begin_date_string )
	{
		sprintf(	where_clause,
				" and %s = '%s'",
				date_attribute_name,
				begin_date_string );
	}
	else
	if ( end_date_string && *end_date_string )
	{
		sprintf(	where_clause,
				" and %s <= '%s'",
				date_attribute_name,
				end_date_string );
	}
	else
	{
		strcpy( where_clause, "and 1 = 1" );
	}

	return where_clause;

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

/* Returns heap memory. */
/* -------------------- */
char *process_generic_output_where(
			char *application_name,
			char *value_folder_name,
			char *date_attribute_name,
			char *begin_date_string,
			char *end_date_string,
			DICTIONARY *query_removed_post_dictionary )
{
	char where_clause[ 65536 ];
	char *drop_down_where;
	char *begin_end_date_where;

	if ( ! ( drop_down_where =
			/* -------------------- */
			/* Returns heap memory. */
			/* -------------------- */
			process_generic_output_drop_down_where(
				application_name,
				value_folder_name,
				query_removed_post_dictionary ) ) )
	{
		return (char *)0;
	}

	begin_end_date_where =
		/* ------------------------------- */
		/* Returns static (program) memory */
		/* ------------------------------- */
		process_generic_output_begin_end_date_where_clause(
			date_attribute_name,
			begin_date_string,
			end_date_string );

	sprintf( where_clause,
		 "%s %s",
		 drop_down_where,
		 begin_end_date_where );

	return strdup( where_clause );

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
			process_generic_output_where(
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

	*begin_date_string = strdup( new_begin_date );
	*end_date_string = strdup( new_end_date );

	if ( strcmp( new_begin_date, new_end_date ) > 0 )
		return 0;
	else
		return 1;

}

