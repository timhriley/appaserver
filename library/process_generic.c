/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "query.h"
#include "dictionary.h"
#include "attribute.h"
#include "appaserver.h"
#include "appaserver_user.h"
#include "application.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "accumulate_boolean.h"
#include "aggregate_level.h"
#include "document.h"
#include "folder.h"
#include "environ.h"
#include "units.h"
#include "grace.h"
#include "date_convert.h"
#include "lookup_statistic.h"
#include "process_generic.h"

char *process_generic_process_set_name( char *process_set_name )
{
	if ( !process_set_name )
		return "null";
	else
		return process_set_name;
}

char *process_generic_process_name(
		char *process_name,
		DICTIONARY *post_dictionary,
		char *process_generic_process_set_name )
{
	if ( string_strcmp(
		process_generic_process_set_name,
		"null" ) != 0 )
	{
		return
		dictionary_get(
			process_generic_process_set_name,
			post_dictionary );
	}
	else
	if ( !process_name )
		return "null";
	else
		return process_name;
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
			aggregate_statistic_string( aggregate_statistic ),
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
		char *begin_date_string,
		char *end_date_string,
		boolean date_is_date_time )
{
	static char where[ 128 ];
	char end_date_time[ 32 ];

	*end_date_time = '\0';

	if ( end_date_string && *end_date_string )
	{
		if ( date_is_date_time )
		{
			sprintf(end_date_time,
				"%s 23:59:59",
				end_date_string );
		}
		else
		{
			strcpy( end_date_time, end_date_string );
		}
	}

	if ( begin_date_string
	&&  *begin_date_string
	&&  *end_date_time )
	{
		sprintf(where,
			"%s >= '%s' and %s <= '%s'",
			date_attribute_name,
			begin_date_string,
			date_attribute_name,
			end_date_time );
	}
	else
	if ( begin_date_string && *begin_date_string )
	{
		sprintf(where,
			"%s >= '%s'",
			date_attribute_name,
			begin_date_string );
	}
	else
	if ( *end_date_time )
	{
		sprintf(where,
			"%s <= '%s'",
			date_attribute_name,
			end_date_time );
	}
	else
	{
		strcpy( where, "1 = 1" );
	}

	return where;
}

PROCESS_GENERIC_DATATYPE *process_generic_datatype_calloc( void )
{
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;

	if ( ! ( process_generic_datatype =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_DATATYPE ) ) ) )
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

PROCESS_GENERIC_DATATYPE *process_generic_datatype_parse( char *input )
{
	char piece_buffer[ 128 ];
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;

	if ( !input )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: input is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	process_generic_datatype = process_generic_datatype_calloc();

	/* See process_generic_datatype_select() */
	/* ------------------------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	process_generic_datatype->datatype_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	process_generic_datatype->aggregation_sum_boolean =
		*piece_buffer == 'y';

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	process_generic_datatype->bar_graph = *piece_buffer == 'y';

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	process_generic_datatype->scale_graph_zero = *piece_buffer == 'y';

	piece( piece_buffer, SQL_DELIMITER, input, 4 );

	if ( *piece_buffer )
		process_generic_datatype->units_name =
			strdup( piece_buffer );

	return process_generic_datatype;
}

PROCESS_GENERIC_DATATYPE *process_generic_datatype_fetch(
		DICTIONARY *post_dictionary,
		char *datatype_folder_name,
		char *datatype_attribute_name,
		boolean exists_aggregation_sum,
		boolean exists_bar_graph,
		boolean exists_scale_graph_zero,
		boolean datatype_exists_units )
{
	char *select;
	char *name;
	char *where;
	char *system_string;

	if ( !datatype_folder_name
	||   !datatype_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		process_generic_datatype_select(
			datatype_attribute_name,
			exists_aggregation_sum,
			exists_bar_graph,
			exists_scale_graph_zero,
			datatype_exists_units );

	name =
		/* ------------------------------- */
		/* Returns component of dictionary */
		/* ------------------------------- */
		process_generic_datatype_name(
			datatype_attribute_name,
			post_dictionary );

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_datatype_where(
			datatype_attribute_name,
			name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			datatype_folder_name,
			where );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	process_generic_datatype_parse(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_fetch( system_string ) );
}

char *process_generic_datatype_name(
		char *datatype_attribute_name,
		DICTIONARY *post_dictionary )
{
	char *name;

	if ( ! ( name =
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				datatype_attribute_name,
				post_dictionary ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"dictionary_get(%s) returned empty.",
			datatype_attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return name;
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

PROCESS_GENERIC_WHERE *process_generic_where_calloc( void )
{
	PROCESS_GENERIC_WHERE *process_generic_where;

	if ( ! ( process_generic_where =
			calloc( 1, sizeof ( PROCESS_GENERIC_WHERE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_generic_where;
}

char *process_generic_select(
		LIST *folder_attribute_primary_key_list,
		char *value_attribute_name )
{
	char select[ 1024 ];
	char *tmp;

	if ( !list_length( folder_attribute_primary_key_list )
	||   !value_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(select,
		"%s,%s",
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		( tmp = list_display_delimited(
			folder_attribute_primary_key_list,
			',' ) ),
		value_attribute_name );

	free( tmp );

	return strdup( select );
}

char *process_generic_system_string(
		char *application_name,
		char *select,
		char *value_folder_name,
		char *where_string,
		enum aggregate_level aggregate_level,
		enum aggregate_statistic aggregate_statistic,
		int accumulate_piece,
		int date_piece,
		int time_piece,
		int value_piece,
		char *end_date_string,
		char *units_name,
		char *units_converted_name,
		boolean whisker_boolean )
{
	char system_string[ 2048 ];
	char real_time_process[ 512 ];
	char accumulation_process[ 512 ];
	char units_converted_process[ 512 ];

	if ( !application_name
	||   !select
	||   !value_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		strcpy( real_time_process, "cat" );
	}
	else
	{
		sprintf(real_time_process, 
	 "real_time2aggregate_value.e %s %d %d %d '%c' %s %c %s",
	 		aggregate_statistic_string(
				aggregate_statistic ),
	 		date_piece,
	 		time_piece,
	 		value_piece,
			SQL_DELIMITER,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
	 		aggregate_level_string( aggregate_level ),
			(whisker_boolean) ? 'y' : 'n',
			(end_date_string) ? end_date_string : "" );
	}

	if ( accumulate_piece == -1 )
	{
		strcpy( accumulation_process, "cat" );
	}
	else
	{
		sprintf(accumulation_process, 
			"accumulate.e %d '%c' append",
			value_piece,
			SQL_DELIMITER );
	}

	if ( !units_name
	||   !units_converted_name )
	{
		strcpy( units_converted_process, "cat" );
	}
	else
	{
		sprintf(units_converted_process,
			"measurement_convert_units.e '%s' '%s' %d '%c'",
			units_name,
			units_converted_name,
			value_piece,
			SQL_DELIMITER );
	}

	sprintf(system_string,
		"select		application=%s		    "
		"		folder=%s		    "
		"		select=\"%s\"		    "
		"		where=\"%s\"		    "
		"		order=select		   |"
		"%s					   |"
		"%s					   |"
		"%s					    ",
		application_name,
		value_folder_name,
		select,
		where_string,
		units_converted_process,
		real_time_process,
		accumulation_process );

	return strdup( system_string );
}

char *process_generic_value_units_name(
		char *datatype_units_name,
		char *foreign_folder_units_name )
{
	if ( datatype_units_name )
		return datatype_units_name;
	else
		return foreign_folder_units_name;
}

PROCESS_GENERIC_INPUT *process_generic_input_calloc( void )
{
	PROCESS_GENERIC_INPUT *process_generic_input;

	if ( ! ( process_generic_input =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return process_generic_input;
}

PROCESS_GENERIC_VALUE *process_generic_value_calloc( void )
{
	PROCESS_GENERIC_VALUE *process_generic_value;

	if ( ! ( process_generic_value =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_VALUE ) ) ) )
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

PROCESS_GENERIC_VALUE *process_generic_value_parse(
		char *value_folder_name,
		char *value_attribute_name,
		char *input )
{
	char piece_string[ 128 ];
	PROCESS_GENERIC_VALUE *process_generic_value;

	if ( !value_folder_name
	||   !value_attribute_name
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_value = process_generic_value_calloc();
	process_generic_value->value_folder_name = value_folder_name;
	process_generic_value->value_attribute_name = value_attribute_name;

	/* See PROCESS_GENERIC_VALUE_SELECT */
	/* -------------------------------- */
	piece( piece_string, SQL_DELIMITER, input, 0 );

	if ( *piece_string )
		process_generic_value->datatype_folder_name =
			strdup( piece_string );

	piece( piece_string, SQL_DELIMITER, input, 1 );

	if ( *piece_string )
		process_generic_value->foreign_folder_name =
			strdup( piece_string );

	piece( piece_string, SQL_DELIMITER, input, 2 );

	if ( *piece_string )
		process_generic_value->datatype_attribute_name =
			strdup( piece_string );

	piece( piece_string, SQL_DELIMITER, input, 3 );

	if ( *piece_string )
		process_generic_value->date_attribute_name =
			strdup( piece_string );

	piece( piece_string, SQL_DELIMITER, input, 4 );

	if ( *piece_string )
		process_generic_value->time_attribute_name =
			strdup( piece_string );

	piece( piece_string, SQL_DELIMITER, input, 5 );

	if ( *piece_string )
		process_generic_value->exists_aggregation_sum =
			(*piece_string == 'y');

	piece( piece_string, SQL_DELIMITER, input, 6 );

	if ( *piece_string )
		process_generic_value->exists_bar_graph =
			(*piece_string == 'y');

	piece( piece_string, SQL_DELIMITER, input, 7 );

	if ( *piece_string )
		process_generic_value->datatype_exists_units =
			(*piece_string == 'y');

	piece( piece_string, SQL_DELIMITER, input, 8 );

	if ( *piece_string )
		process_generic_value->foreign_exists_units =
			(*piece_string == 'y');

	return process_generic_value;
}

char *process_generic_value_primary_where(
		char *value_folder_name,
		char *value_attribute_name )
{
	static char where[ 128 ];

	if ( !value_folder_name
	||   !value_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"value_folder = '%s' and "
		"value_attribute = '%s'",
		value_folder_name,
		value_attribute_name );

	return where;
}

PROCESS_GENERIC_VALUE *process_generic_value_fetch(
		char *process_generic_value_table,
		DICTIONARY *post_dictionary,
		char *value_folder_name,
		char *value_attribute_name )
{
	char *system_string;
	char *where;
	char *input;
	PROCESS_GENERIC_VALUE *process_generic_value;

	if ( !process_generic_value_table
	||   !value_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_value_primary_where(
			value_folder_name,
			value_attribute_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			PROCESS_GENERIC_VALUE_SELECT,
			process_generic_value_table,
			where );
			
	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ! ( input = string_pipe_fetch( system_string ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"string_pipe_fetch(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_value =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_value_parse(
			value_folder_name,
			value_attribute_name,
			input );

	if ( !process_generic_value->foreign_folder_name
	||   !process_generic_value->date_attribute_name
	||   !process_generic_value->value_attribute_name )
	{
		char message[ 128 ];

		sprintf(message,
			"process_generic_value_parse(%s) returned incomplete.",
			input );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( input );

	process_generic_value->folder_attribute_list =
		folder_attribute_list(
			(char *)0 /* role_name */,
			value_folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_attribute */ );

	process_generic_value->folder_attribute_primary_key_list =
		folder_attribute_primary_key_list(
			value_folder_name,
			process_generic_value->folder_attribute_list );

	process_generic_value->primary_key_list_length =
		list_length(
			process_generic_value->
				folder_attribute_primary_key_list );

	if ( process_generic_value->datatype_folder_name )
	{
		process_generic_value->process_generic_datatype =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_generic_datatype_fetch(
				post_dictionary,
				process_generic_value->datatype_folder_name,
				process_generic_value->datatype_attribute_name,
				process_generic_value->exists_aggregation_sum,
				process_generic_value->exists_bar_graph,
				process_generic_value->exists_scale_graph_zero,
				process_generic_value->datatype_exists_units );
	}
	else
	{
		process_generic_value->process_generic_datatype =
			process_generic_datatype_calloc();
	}

	process_generic_value->process_generic_foreign_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_foreign_folder_fetch(
			post_dictionary,
			process_generic_value->foreign_folder_name,
			process_generic_value->foreign_exists_units );

	process_generic_value->units_name =
		/* -------------------------------- */
		/* Returns either parameter or null */
		/* -------------------------------- */
		process_generic_value_units_name(
			process_generic_value->
				process_generic_datatype->
				units_name,
			process_generic_value->
				process_generic_foreign_folder->
				units_name );

	return process_generic_value;
}

char *process_generic_primary_where(
		char *process_name,
		char *process_set_name )
{
	static char where[ 128 ];

	if ( !process_name
	||   !process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"process = '%s' and process_set = '%s'",
		process_name,
		process_set_name );

	return where;
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

PROCESS_GENERIC_INPUT *process_generic_input_new(
		char *application_name,
		char *login_name,
		char *output_medium_string,
		char *data_directory,
		DICTIONARY *post_dictionary,
		char *process_generic_process_name,
		boolean aggregation_sum_boolean,
		LIST *folder_attribute_list,
		LIST *folder_attribute_primary_key_list,
		int primary_key_list_length,
		char *date_attribute_name,
		char *time_attribute_name )
{
	PROCESS_GENERIC_INPUT *process_generic_input;

	if ( !process_generic_process_name
	||   !list_length( folder_attribute_list )
	||   !list_length( folder_attribute_primary_key_list )
	||   !primary_key_list_length
	||   !date_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_input = process_generic_input_calloc();

	process_generic_input->output_medium =
		output_medium_get(
			output_medium_string );

	if ( application_name
	&&   data_directory )
	{
		process_generic_input->process_generic_link_extension =
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			process_generic_link_extension(
				process_generic_input->output_medium );

		if ( process_generic_input->process_generic_link_extension )
		{
			process_generic_input->process_generic_link =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				process_generic_link_new(
					application_name,
					data_directory,
					process_generic_process_name
						/* filename_stem */,
					getpid() /* process_id */,
					process_generic_input->
					     process_generic_link_extension );
		}
	}

	if ( application_name
	&&   login_name )
	{
		process_generic_input->appaserver_user_date_format_string =
			/* ------------------------------------- */
			/* Returns heap memory or program memory */
			/* ------------------------------------- */
			appaserver_user_date_format_string(
				application_name,
				login_name );
	}

	process_generic_input->begin_date_string =
		/* ---------------------------------------------------------- */
		/* Returns component of post_dictionary, heap memory, or null */
		/* ---------------------------------------------------------- */
		process_generic_input_begin_date_string(
			post_dictionary );

	process_generic_input->end_date_string =
		/* ---------------------------------------------------------- */
		/* Returns component of post_dictionary, heap memory, or null */
		/* ---------------------------------------------------------- */
		process_generic_input_end_date_string(
			post_dictionary );

	process_generic_input->date_is_date_time =
		process_generic_input_date_is_date_time(
			folder_attribute_list,
			date_attribute_name );

	process_generic_input->aggregate_level =
		aggregate_level_dictionary_extract(
			post_dictionary );

	process_generic_input->aggregate_statistic =
		aggregate_statistic_dictionary_extract(
			post_dictionary,
			aggregation_sum_boolean,
			process_generic_input->aggregate_level );

	process_generic_input->accumulate_boolean =
		accumulate_boolean_dictionary_extract(
			post_dictionary );

	process_generic_input->whisker_boolean =
		process_generic_input_whisker_boolean(
			PROCESS_GENERIC_WHISKER_YN,
			post_dictionary );

	process_generic_input->units_converted_name =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		units_converted_dictionary_extract(
			post_dictionary );

	process_generic_input->date_piece =
		/* ----------------------- */
		/* Returns -1 if not found */
		/* ----------------------- */
		process_generic_input_date_piece(
			folder_attribute_primary_key_list,
			date_attribute_name );

	if ( process_generic_input->date_piece == -1 )
	{
		char message[ 128 ];

		sprintf(message,
			"process_generic_input_date_piece(%s) returned -1.",
			date_attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_input->time_piece =
		/* ----------------------- */
		/* Returns -1 if not found */
		/* ----------------------- */
		process_generic_input_time_piece(
			folder_attribute_primary_key_list,
			time_attribute_name );

	process_generic_input->value_piece =
		/* ------------------------------- */
		/* Returns primary_key_list_length */
		/* ------------------------------- */
		process_generic_input_value_piece(
			primary_key_list_length );

	process_generic_input->accumulate_piece =
		/* ------------------------- */
		/* Returns -1 or value_piece */
		/* ------------------------- */
		process_generic_input_accumulate_piece(
			process_generic_input->accumulate_boolean,
			process_generic_input->value_piece );

	return process_generic_input;
}

int process_generic_input_accumulate_piece(
		boolean accumulate_boolean,
		int value_piece )
{
	if ( !accumulate_boolean )
		return -1;
	else
		return value_piece;
}

int process_generic_input_value_piece( int primary_key_list_length )
{
	return primary_key_list_length;
}

int process_generic_input_time_piece(
		LIST *folder_attribute_primary_key_list,
		char *time_attribute_name )
{
	return
	/* ----------------------- */
	/* Returns -1 if not found */
	/* ----------------------- */
	list_seek_offset(
		time_attribute_name,
		folder_attribute_primary_key_list );
}

int process_generic_input_date_piece(
		LIST *folder_attribute_primary_key_list,
		char *date_attribute_name )
{
	return
	/* ----------------------- */
	/* Returns -1 if not found */
	/* ----------------------- */
	list_seek_offset(
		date_attribute_name,
		folder_attribute_primary_key_list );
}

char *process_generic_input_begin_date_string( DICTIONARY *post_dictionary )
{
	char *get;
	enum date_convert_format_enum date_convert_format_enum;

	get =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			"begin_date",
			post_dictionary );

	if ( !get || strcmp( get, "begin_date" ) == 0 ) return (char *)0;

	date_convert_format_enum =
		date_convert_string_evaluate(
			get );

	if ( date_convert_format_enum == date_convert_international )
	{
		return get;
	}

	return
	/* ----------------------------------------------- */
	/* Returns heap memory, source_date_string or null */
	/* ----------------------------------------------- */
	date_convert_return_date_string(
		date_convert_format_enum /* source_enum */,
		date_convert_international /* destination_enum */,
		get /* source_date_string */ );
}

char *process_generic_input_end_date_string( DICTIONARY *post_dictionary )
{
	char *get;
	enum date_convert_format_enum date_convert_format_enum;

	get =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			"end_date",
			post_dictionary );

	if ( !get || strcmp( get, "end_date" ) == 0 ) return (char *)0;

	date_convert_format_enum =
		date_convert_string_evaluate(
			get );

	if ( date_convert_format_enum == date_convert_international )
	{
		return get;
	}

	return
	/* ----------------------------------------------- */
	/* Returns heap memory, source_date_string or null */
	/* ----------------------------------------------- */
	date_convert_return_date_string(
		date_convert_format_enum /* source_enum */,
		date_convert_international /* destination_enum */,
		get /* source_date_string */ );
}

PROCESS_GENERIC *process_generic_parse(
		char *process_generic_process_name,
		char *process_generic_process_set_name,
		char *input )
{
	PROCESS_GENERIC *process_generic;
	char piece_buffer[ 128 ];

	if ( !process_generic_process_name
	||   !process_generic_process_set_name
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic = process_generic_calloc();
	process_generic->process_name = process_generic_process_name;
	process_generic->process_set_name = process_generic_process_set_name;

	/* See PROCESS_GENERIC_SELECT */
	/* -------------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );

	if ( !*piece_buffer )
	{
		char message[ 128 ];

		sprintf(message,
			"piece(%s,0) returned empty.",
			input );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic->value_folder_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );

	if ( !*piece_buffer )
	{
		char message[ 128 ];

		sprintf(message,
			"piece(%s,1) returned empty.",
			input );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic->value_attribute_name = strdup( piece_buffer );

	return process_generic;
}

PROCESS_GENERIC *process_generic_calloc( void )
{
	PROCESS_GENERIC *process_generic;

	if ( ! ( process_generic =
			calloc( 1, sizeof ( PROCESS_GENERIC ) ) ) )
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

PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_calloc(
		void )
{
	PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder;

	if ( ! ( process_generic_foreign_folder =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_FOREIGN_FOLDER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_foreign_folder;
}

PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_fetch(
		DICTIONARY *post_dictionary,
		char *foreign_folder_name,
		boolean foreign_exists_units )
{
	PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder;

	if ( !dictionary_length( post_dictionary )
	||   !foreign_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_foreign_folder =
		process_generic_foreign_folder_calloc();

	process_generic_foreign_folder->folder =
		folder_fetch(
			foreign_folder_name,
			(char *)0 /* role_name */,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	if ( !process_generic_foreign_folder->folder )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_fetch(%s) returned empty.",
			foreign_folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_foreign_folder->data_list =
		process_generic_foreign_folder_data_list(
			post_dictionary,
			process_generic_foreign_folder->
				folder->
				folder_attribute_primary_key_list
					/* foreign_key_list */ );

	if ( !list_length( process_generic_foreign_folder->data_list ) )
	{
		char message[ 128 ];

		sprintf(message,
	"process_generic_foreign_folder_data_list(%s) returned empty.",
			list_display(
				process_generic_foreign_folder->
					folder->
					folder_attribute_primary_key_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_foreign_folder->data_list_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		process_generic_foreign_folder_data_list_name(
			process_generic_foreign_folder->data_list,
			'_' /* delimiter */ );

	process_generic_foreign_folder->units_name =
		process_generic_foreign_folder_units_name(
			foreign_exists_units,
			list_last(
				process_generic_foreign_folder->
					data_list )
				/* last_data */ );

	return process_generic_foreign_folder;
}

char *process_generic_foreign_folder_data_list_name(
		LIST *foreign_folder_data_list,
		char delimiter )
{
	if ( !list_length( foreign_folder_data_list )
	||   !delimiter )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_delimited(
		foreign_folder_data_list,
		delimiter );
}

LIST *process_generic_foreign_folder_data_list(
		DICTIONARY *post_dictionary,
		LIST *foreign_key_list )
{
	if ( !dictionary_length( post_dictionary )
	||   !list_length( foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	dictionary_data_list(
		foreign_key_list /* attribute_name_list */,
		post_dictionary );
}

char *process_generic_foreign_folder_units_name(
		boolean foreign_exists_units,
		char *last_data )
{
	if ( !last_data )
	{
		char message[ 128 ];

		sprintf(message, "last_data is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	(foreign_exists_units)
		? last_data
		: (char *)0;
}

char *process_generic_heading(
		LIST *folder_attribute_primary_key_list,
		char *value_attribute_name,
		char *process_generic_value_units_name,
		char *units_converted_name,
		enum aggregate_level aggregate_level,
		enum aggregate_statistic aggregate_statistic,
		boolean accumulate_boolean )
{
	static char heading[ 512 ];
	char *ptr = heading;
	char *primary_key;
	char buffer[ 128 ];

	if ( !value_attribute_name
	||   !list_rewind( folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		primary_key =
			list_get(
				folder_attribute_primary_key_list );

		if ( !list_at_head( folder_attribute_primary_key_list ) )
			ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			string_initial_capital(
				buffer /* destination */,
				primary_key ) );

	} while( list_next( folder_attribute_primary_key_list ) );

	if ( process_generic_value_units_name )
	{
		ptr += sprintf( ptr,
			",%s %s",
			format_initial_capital(
				buffer,
				value_attribute_name ),
			process_generic_units_label(
				process_generic_value_units_name,
				units_converted_name,
				aggregate_statistic ) );
	}
	else
	{
		string_initial_capital( buffer, value_attribute_name );

		ptr += sprintf( ptr,
			",%s",
			string_initial_capital(
				buffer,
				value_attribute_name ) );
	}

	if ( accumulate_boolean )
	{
		ptr += sprintf( ptr, ",Accumulate" );
	}

	if ( aggregate_level >= half_hour )
	{
		ptr += sprintf( ptr, ",Count" );
	}

	return heading;
}

char *process_generic_sub_title(
			char *process_generic_value_folder_name,
			char *appaserver_user_date_format_string,
			char *begin_date_string,
			char *end_date_string,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic )
{
	static char sub_title[ 256 ];
	char folder_name_buffer[ 32 ];
	char aggregate_statistic_buffer[ 128 ];
	char aggregate_level_buffer[ 128 ];
	char begin_date_buffer[ 128 ];
	char end_date_buffer[ 128 ];
	char buffer[ 128 ];

	string_initial_capital(
		folder_name_buffer /* destination */,
		process_generic_value_folder_name );

	if ( aggregate_statistic != aggregate_statistic_none )
	{
		sprintf(aggregate_statistic_buffer,
			" %s",
			string_initial_capital(
				buffer,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				aggregate_statistic_string(
					aggregate_statistic ) ) );
	}
	else
	{
		*aggregate_statistic_buffer = '\0';
	}

	if ( aggregate_level != aggregate_level_none )
	{
		sprintf(aggregate_level_buffer,
			" %s",
			string_initial_capital(
				buffer /* destination */,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				aggregate_level_string(
					aggregate_level ) ) );
	}
	else
	{
		*aggregate_level_buffer = '\0';
	}

	if ( begin_date_string )
	{
		sprintf(begin_date_buffer,
			"From %s",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_user_date_convert_string(
				appaserver_user_date_format_string,
				begin_date_string
					/* international_date_string */ ) );
	}
	else
	{
		*begin_date_buffer = '\0';
	}

	if ( end_date_string )
	{
		sprintf(end_date_buffer,
			"To %s",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_user_date_convert_string(
				appaserver_user_date_format_string,
				end_date_string
					/* international_date_string */ ) );
	}
	else
	{
		*end_date_buffer = '\0';
	}

	sprintf(sub_title,
		"%s %s %s%s%s",
		folder_name_buffer,
		begin_date_buffer,
		end_date_buffer,
		aggregate_level_buffer,
		aggregate_statistic_buffer );

	return sub_title;
}

char *process_generic_datatype_where(
			char *datatype_attribute_name,
			char *datatype_name )
{
	static char where[ 256 ];

	if ( !datatype_attribute_name
	&&   !datatype_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"%s = '%s'",
		datatype_attribute_name,
		datatype_name );

	return where;
}

PROCESS_GENERIC *process_generic_new(
			char *application_name,
			char *login_name,
			char *process_generic_process_name,
			char *process_generic_process_set_name,
			char *output_medium_string,
			char *data_directory,
			DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC *process_generic;

	if ( !process_generic_process_name
	||   !process_generic_process_set_name
	||   !dictionary_length( post_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_fetch(
			PROCESS_GENERIC_TABLE,
			process_generic_process_name,
			process_generic_process_set_name );

	process_generic->process_generic_value =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_value_fetch(
			PROCESS_GENERIC_VALUE_TABLE,
			post_dictionary,
			process_generic->value_folder_name,
			process_generic->value_attribute_name );

	if ( !process_generic->
		process_generic_value->
		foreign_folder_name
	||   !process_generic->
		process_generic_value->
		date_attribute_name
	||   !process_generic->
		process_generic_value->
		value_attribute_name
	||   !process_generic->
		process_generic_value->
		primary_key_list_length
	||   !process_generic->
		process_generic_value->
		process_generic_datatype
	||   !process_generic->
		process_generic_value->
		process_generic_foreign_folder )
	{
		char message[ 128 ];

		sprintf(message,
		"process_generic_value_fetch(%s,%s) returned incomplete.",
			process_generic->value_folder_name,
			process_generic->value_attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic->process_generic_input =
		process_generic_input_new(
			application_name,
			login_name,
			output_medium_string,
			data_directory,
			post_dictionary,
			process_generic->process_name,
			process_generic->
				process_generic_value->
				process_generic_datatype->
				aggregation_sum_boolean,
			process_generic->
				process_generic_value->
				folder_attribute_list,
			process_generic->
				process_generic_value->
				folder_attribute_primary_key_list,
			process_generic->
				process_generic_value->
				primary_key_list_length,
			process_generic->
				process_generic_value->
				date_attribute_name,
			process_generic->
				process_generic_value->
				time_attribute_name );

	process_generic->select =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_select(
			process_generic->
				process_generic_value->
				folder_attribute_primary_key_list,
			process_generic->
				process_generic_value->
				value_attribute_name );

	process_generic->date_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_date_where(
			process_generic->
				process_generic_value->
				date_attribute_name,
			process_generic->
				process_generic_input->
				begin_date_string,
			process_generic->
				process_generic_input->
				end_date_string,
			process_generic->
				process_generic_input->
				date_is_date_time );

	process_generic->process_generic_where =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_where_new(
			process_generic->
				process_generic_value->
				process_generic_foreign_folder->
				folder->
				folder_attribute_primary_key_list
					/* where_attribute_name_list */,
			process_generic->
				process_generic_value->
				process_generic_foreign_folder->
				data_list
					/* where_attribute_data_list */,
			process_generic->
				process_generic_value->
				process_generic_foreign_folder->
				folder->
				folder_attribute_primary_list,
			process_generic->
				date_where );

	process_generic->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		process_generic_system_string(
			application_name,
			process_generic->select,
			process_generic->value_folder_name,
			process_generic->process_generic_where->string,
			process_generic->
				process_generic_input->
				aggregate_level,
			process_generic->
				process_generic_input->
				aggregate_statistic,
			process_generic->
				process_generic_input->
				accumulate_piece,
			process_generic->
				process_generic_input->
				date_piece,
			process_generic->
				process_generic_input->
				time_piece,
			process_generic->
				process_generic_input->
				value_piece,
			process_generic->
				process_generic_input->
				end_date_string,
			process_generic->
				process_generic_value->
				units_name,
			process_generic->
				process_generic_input->
				units_converted_name,
			process_generic->
				process_generic_input->
				whisker_boolean );

	process_generic->heading =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_heading(
			process_generic->
				process_generic_value->
				folder_attribute_primary_key_list,
			process_generic->
				process_generic_value->
				value_attribute_name,
			process_generic->
				process_generic_value->units_name,
			process_generic->
				process_generic_input->
				units_converted_name,
			process_generic->
				process_generic_input->
				aggregate_level,
			process_generic->
				process_generic_input->
				aggregate_statistic,
			process_generic->
				process_generic_input->
				accumulate_boolean );

	process_generic->title  = 
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_title(
			process_generic->process_set_name,
			process_generic->process_name );

	process_generic->sub_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_sub_title(
			process_generic->value_folder_name,
			process_generic->
				process_generic_input->
				appaserver_user_date_format_string,
			process_generic->
				process_generic_input->
				begin_date_string,
			process_generic->
				process_generic_input->
				end_date_string,
			process_generic->
				process_generic_input->
				aggregate_level,
			process_generic->
				process_generic_input->
				aggregate_statistic );

	return process_generic;
}

PROCESS_GENERIC *process_generic_fetch(
			char *process_generic_table,
			char *process_generic_process_name,
			char *process_generic_process_set_name )
{
	char *primary_where;
	char *system_string;
	char *pipe_fetch;

	if ( !process_generic_table
	||   !process_generic_process_name
	||   !process_generic_process_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		process_generic_primary_where(
			process_generic_process_name,
			process_generic_process_set_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			PROCESS_GENERIC_SELECT,
			process_generic_table,
			primary_where );

	if ( ! ( pipe_fetch =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_fetch(
				system_string ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"string_pipe_fetch(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	process_generic_parse(
		process_generic_process_name,
		process_generic_process_set_name,
		pipe_fetch /* input */ );
}

PROCESS_GENERIC_WHERE *process_generic_where_new(
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *folder_attribute_primary_list,
			char *process_generic_date_where )
{
	PROCESS_GENERIC_WHERE *process_generic_where;

	process_generic_where = process_generic_where_calloc();

	process_generic_where->query_data_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_data_where(
			(char *)0 /* folder_name */,
			where_attribute_name_list,
			where_attribute_data_list,
			folder_attribute_primary_list );

	process_generic_where->string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		process_generic_where_string(
			process_generic_where->query_data_where,
			process_generic_date_where );

	return process_generic_where;
}

char *process_generic_where_string(
			char *query_data_where,
			char *date_where )
{
	char string[ 1024 ];
	char *ptr = string;

	if ( !query_data_where
	&&   !date_where )
	{
		return (char *)0;
	}

	*ptr = '\0';

	if ( query_data_where )
	{
		ptr += sprintf( ptr, "%s", query_data_where );
	}

	if ( date_where )
	{
		if ( !*ptr ) ptr += sprintf( ptr, " and ");

		ptr += sprintf( ptr, "%s", date_where );
	}

	return strdup( string );
}

void process_generic_point_free( PROCESS_GENERIC_POINT *process_generic_point )
{
	if ( !process_generic_point ) return;

	if ( process_generic_point->primary_data_list_string )
		free( process_generic_point->primary_data_list_string );

	if ( process_generic_point->date_string )
		free( process_generic_point->date_string );

	if ( process_generic_point->time_string )
		free( process_generic_point->time_string );

	if ( process_generic_point->value_string )
		free( process_generic_point->value_string );

	free( process_generic_point );
}

PROCESS_GENERIC_POINT *process_generic_point_parse(
			char delimiter,
			int primary_key_list_length,
			int date_piece,
			int time_piece,
			int value_piece,
			int accumulate_piece,
			char *input )
{
	PROCESS_GENERIC_POINT *process_generic_point;

	if ( !delimiter || !input ) return (PROCESS_GENERIC_POINT *)0;

	process_generic_point = process_generic_point_calloc();

	if ( primary_key_list_length > 0 )
	{
		process_generic_point->primary_data_list_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			process_generic_point_primary_data_list_string(
				delimiter,
				primary_key_list_length,
				input );

		if ( !process_generic_point->primary_data_list_string )
			return (PROCESS_GENERIC_POINT *)0;
	}

	process_generic_point->date_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		process_generic_point_date_string(
			delimiter,
			date_piece,
			input );

	if ( !process_generic_point->date_string )
		return (PROCESS_GENERIC_POINT *)0;

	if ( time_piece != -1 )
	{
		process_generic_point->time_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			process_generic_point_time_string(
				delimiter,
				time_piece,
				input );

		if ( !process_generic_point->time_string )
			return (PROCESS_GENERIC_POINT *)0;
	}

	process_generic_point->value_null_boolean =
		process_generic_point_value_null_boolean(
			delimiter,
			value_piece,
			input );

	if ( !process_generic_point->value_null_boolean )
	{
		process_generic_point->value_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			process_generic_point_value_string(
				delimiter,
				value_piece,
				input );

		if ( !process_generic_point->value_string )
			return (PROCESS_GENERIC_POINT *)0;

		process_generic_point->value_double =
			process_generic_point_value_double(
				process_generic_point->value_string );
	}

	if ( accumulate_piece != -1 )
	{
		process_generic_point->accumulate_double =
			process_generic_point_accumulate_double(
				delimiter,
				accumulate_piece,
				input );
	}

	return process_generic_point;
}

char *process_generic_point_primary_data_list_string(
			char delimiter,
			int primary_key_list_length,
			char *input )
{
	char primary_data_list_string[ 128 ];

	if ( !delimiter
	||   primary_key_list_length < 1
	||   !input )
	{
		return (char *)0;
	}

	return
	strdup(
		piece_multiple(
			primary_data_list_string /* destination */,
			delimiter,
			input,
			primary_key_list_length ) );
}

char *process_generic_point_date_string(
			char delimiter,
			int date_piece,
			char *input )
{
	char date_string[ 128 ];

	if ( !delimiter
	||   date_piece == -1
	||   !input )
	{
		return (char *)0;
	}

	return
	strdup(
		piece(	date_string /* destination */,
			delimiter,
			input,
			date_piece ) );
}

char *process_generic_point_time_string(
			char delimiter,
			int time_piece,
			char *input )
{
	char time_string[ 128 ];

	if ( !delimiter
	||   time_piece == -1
	||   !input )
	{
		return (char *)0;
	}

	return
	strdup(
		piece(	time_string /* destination */,
			delimiter,
			input,
			time_piece ) );
}

boolean process_generic_point_value_null_boolean(
			char delimiter,
			int value_piece,
			char *input )
{
	char value_string[ 128 ];

	if ( !delimiter
	||   value_piece == -1
	||   !input )
	{
		return 0;
	}

	piece(	value_string /* destination */,
		delimiter,
		input,
		value_piece );

	if ( *value_string )
		return 0;
	else
		return 1;
}

char *process_generic_point_value_string(
			char delimiter,
			int value_piece,
			char *input )
{
	char value_string[ 128 ];

	if ( !delimiter
	||   value_piece == -1
	||   !input )
	{
		return (char *)0;
	}

	return
	strdup(
		piece(	value_string /* destination */,
			delimiter,
			input,
			value_piece ) );
}

double process_generic_point_value_double( char *value_string )
{
	if ( !value_string )
		return 0.0;
	else
		return atof( value_string );
}

double process_generic_point_accumulate_double(
			char delimiter,
			int accumulate_piece,
			char *input )
{
	char accumulate_string[ 128 ];

	if ( !delimiter
	||   accumulate_piece == -1
	||   !input )
	{
		return 0.0;
	}

	return
	atof(
		piece(	accumulate_string,
			delimiter,
			input,
			accumulate_piece ) );
}

char *process_generic_link_extension( enum output_medium output_medium )
{
	if ( output_medium == spreadsheet )
		return "csv";
	else
	if ( output_medium == gracechart )
		return "pdf";
	else
	if ( output_medium == text_file )
		return "txt";
	else
	if ( output_medium == googlechart )
		return "html";
	else
		return (char *)0;
}

PROCESS_GENERIC_LINK *process_generic_link_new(
			char *application_name,
			char *data_directory,
			char *filename_stem,
			pid_t process_id,
			char *process_generic_link_extension )
{
	PROCESS_GENERIC_LINK *process_generic_link;

	if ( !application_name
	||   !data_directory
	||   !filename_stem
	||   !process_generic_link_extension )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_link = process_generic_link_calloc();

	process_generic_link->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			application_server_address(),
			data_directory,
			filename_stem,
			application_name,
			process_id,
			(char *)0 /* session_key */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			process_generic_link_extension );

	if ( strcmp(
		process_generic_link_extension,
		"pdf" ) == 0 )
	{
		process_generic_link->agr_appaserver_link =
			appaserver_link_new(
				application_http_prefix(
					application_ssl_support_boolean(
						application_name ) ),
				application_server_address(),
				data_directory,
				filename_stem,
				application_name,
				process_id,
				(char *)0 /* session_key */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"agr" /* extension */ );
	}

	return process_generic_link;
}

PROCESS_GENERIC_LINK *process_generic_link_calloc( void )
{
	PROCESS_GENERIC_LINK *process_generic_link;

	if ( ! ( process_generic_link =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_LINK ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_link;
}

boolean process_generic_input_date_is_date_time(
			LIST *folder_attribute_list,
			char *date_attribute_name )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !date_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "date_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( folder_attribute =
			folder_attribute_seek(
				date_attribute_name,
				folder_attribute_list ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_attribute_seek(%s) returned empty.",
			date_attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !folder_attribute->attribute )
	{
		char message[ 128 ];

		sprintf(message, "folder_attribute->attribute is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	attribute_is_date_time(
		folder_attribute->
			attribute->
			datatype_name );
}

boolean process_generic_input_whisker_boolean(
			char *process_generic_whisker_yn,
			DICTIONARY *post_dictionary )
{
	char *get =
		dictionary_get(
			process_generic_whisker_yn,
			post_dictionary );

	return ( get && *get == 'y' );
}

char *process_generic_title(
			char *process_set_name,
			char *process_name )
{
	static char title[ 128 ];

	if ( string_strcmp(
		process_set_name,
		"null" ) != 0 )
	{
		string_initial_capital(
			title,
			process_set_name );
	}
	else
	{
		string_initial_capital(
			title,
			process_name );
	}

	return title;
}

LIST *process_generic_point_list(
			char delimiter,
			int primary_key_list_length,
			int date_piece,
			int time_piece,
			int value_piece,
			int accumulate_piece,
			char *system_string )
{
	LIST *list;
	FILE *input_pipe;
	char input[ 1024 ];
	PROCESS_GENERIC_POINT *process_generic_point;

	if ( !delimiter
	||   !system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		if ( ! ( process_generic_point =
				process_generic_point_parse(
					delimiter,
					primary_key_list_length,
					date_piece,
					time_piece,
					value_piece,
					accumulate_piece,
					input ) ) )
		{
			char message[ 2048 ];

			sprintf(message,
			"process_generic_point_parse(%s) returned empty.",
				input );

			pclose( input_pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			list,
			process_generic_point );
	}

	pclose( input_pipe );
	return list;
}

PROCESS_GENERIC_POINT *process_generic_point_calloc( void )
{
	PROCESS_GENERIC_POINT *process_generic_point;

	if ( ! ( process_generic_point =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_POINT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_point;
}

