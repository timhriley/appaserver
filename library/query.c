/* $APPASERVER_LIBRARY/library/query.c					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "String.h"
#include "column.h"
#include "date_convert.h"
#include "folder.h"
#include "folder_attribute.h"
#include "attribute.h"
#include "form.h"
#include "environ.h"
#include "prompt_recursive.h"
#include "query.h"

QUERY *query_calloc( void )
{
	QUERY *query;

	if ( ! ( query = calloc( 1, sizeof( QUERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return query;
}

char *query_system_string(
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int maxrows )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	if ( !select_clause || !*select_clause )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: select_clause is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !from_clause || !*from_clause )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: from_clause is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select	application=%s			"
		"	select=\"%s\"			"
		"	folder=%s			"
		"	where_clause=\"%s\"		"
		"	order_clause=\"%s\"		"
		"	maxrows=\"%d\"			",
		environment_application_name(),
		select_clause,
		from_clause,
		(where_clause) ? where_clause : "",
		(order_clause) ? order_clause : "",
		maxrows );

	return strdup( system_string );
}

LIST *query_dictionary_list(
			char *select_clause,
			LIST *select_list,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows )
{
	char *system_string;

	system_string = 
		query_system_string(
			select_clause,
			from_clause,
			where_clause,
			order_clause,
			max_rows );

	return
		query_system_dictionary_list(
			system_string,
			select_list );
}

LIST *query_delimited_list(
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows )
{
	return
	list_pipe_fetch(
		query_system_string(
			select_clause,
			from_clause,
			where_clause,
			order_clause,
			max_rows ) );
}

char *query_attribute_between_date_time_where(
			char *date_attribute_name,
			char *time_attribute_name,
			QUERY_DATA *date_from_data,
			QUERY_DATA *time_from_data,
			QUERY_DATA *date_to_data,
			QUERY_DATA *time_to_data,
			char *folder_table_name )
{
	static char where_clause[ 1024 ];
	char *where_ptr;
	char *start_end_time;
	char *finish_begin_time;
	char *begin_date;
	char *end_date;
	char *begin_time;
	char *end_time;

	begin_date = date_from_data->escaped_replaced_data;
	end_date = date_to_data->escaped_replaced_data;
	begin_time = time_from_data->escaped_replaced_data;
	end_time = time_to_data->escaped_replaced_data;

	if ( string_strcmp( begin_date, end_date ) != 0 )
	{
		start_end_time = "2359";
		finish_begin_time = "0000";
	}
	else
	{
		start_end_time = end_time;
		finish_begin_time = begin_time;
	}

	where_ptr = where_clause;

	where_ptr +=
		sprintf(
		   where_ptr,
		  " ( ( %s = '%s' and %s >= '%s' and %s <= '%s' )",
			attribute_full_attribute_name(
				folder_table_name,
				date_attribute_name ),
		 	begin_date,
			attribute_full_attribute_name(
				folder_table_name,
				time_attribute_name ),
		 	begin_time,
			attribute_full_attribute_name(
				folder_table_name,
				time_attribute_name ),
			start_end_time );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s = 'null' )",
			attribute_full_attribute_name(
				folder_table_name,
				date_attribute_name ),
		 	begin_date,
			attribute_full_attribute_name(
				folder_table_name,
				time_attribute_name ) );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s = 'null' )",
			attribute_full_attribute_name(
				folder_table_name,
				date_attribute_name ),
		 	end_date,
			attribute_full_attribute_name(
				folder_table_name,
				time_attribute_name ) );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s > '%s' and %s < '%s' )",
			attribute_full_attribute_name(
				folder_table_name,
				date_attribute_name ),
		 	begin_date,
			attribute_full_attribute_name(
				folder_table_name,
				date_attribute_name ),
			end_date );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s >= '%s' and %s <= '%s' ) )",
			attribute_full_attribute_name(
				folder_table_name,
				date_attribute_name ),
		 	end_date,
			attribute_full_attribute_name(
				folder_table_name,
				time_attribute_name ),
			finish_begin_time,
			attribute_full_attribute_name(
				folder_table_name,
				time_attribute_name ),
		 	end_time );

	return where_clause;
}

char *query_attribute_operator_key(
			char *attribute_name )
{
	static char key[ 512 ];

	sprintf(key,
		"%s%s_0",
		QUERY_RELATION_OPERATOR_STARTING_LABEL,
		attribute_name );

	return key;
}

char *query_drop_down_operator_key(
			LIST *foreign_key_list )
{
	static char key[ 512 ];
	char *tmp;

	*key = '\0';

	if ( !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"%s%s_0",
		QUERY_RELATION_OPERATOR_STARTING_LABEL,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tmp =
			list_display_delimited(
				foreign_key_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) ) );

	free( tmp );
	return key;
}

char *query_drop_down_row_key(
			LIST *foreign_key_list,
			int index )
{
	static char key[ 512 ];
	char *tmp;

	*key = '\0';

	if ( !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"%s_%d",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tmp =
			list_display_delimited(
				foreign_key_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) ),
		index );

	free( tmp );
	return key;
}

char *query_data_list_string(
			DICTIONARY *query_dictionary,
			char *query_key )
{
	return dictionary_fetch( query_key, query_dictionary );
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
			LIST *one_primary_key_list,
			LIST *foreign_key_list,
			LIST *many_folder_attribute_list,
			DICTIONARY *drillthru_dictionary,
			int index )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	char *data_list_string;
	char *key = {0};

	if ( !list_length( one_primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( data_list_string =
			query_drop_down_row_data_list_string(
			( key = query_drop_down_row_key(
				foreign_key_list,
				index ) ),
				drillthru_dictionary ) ) )
	{
		return (QUERY_DROP_DOWN_ROW *)0;
	}

	query_drop_down_row = query_drop_down_row_calloc();

	query_drop_down_row->many_folder_attribute_list =
		many_folder_attribute_list;

	query_drop_down_row->key = key;
	query_drop_down_row->data_list_string = data_list_string;

	query_drop_down_row->data_string_list =
		query_drop_down_row_data_string_list(
			query_drop_down_row->
				data_list_string );

	query_drop_down_row->query_data_list =
		query_data_list(
			query_drop_down_row->foreign_key_list,
			query_drop_down_row->many_folder_attribute_list,
			query_drop_down_row->data_string_list );

	return query_drop_down_row;
}

QUERY_ATTRIBUTE *query_attribute_new(
			char *attribute_name,
			char *folder_name,
			char *datatype_name,
			enum relational_operator relational_operator,
			QUERY_DATA *from_data,
			QUERY_DATA *to_data,
			int primary_key_index )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( ! ( query_attribute = calloc( 1, sizeof( QUERY_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_attribute->attribute_name = attribute_name;
	query_attribute->folder_name = folder_name;
	query_attribute->datatype_name = datatype_name;
	query_attribute->relational_operator = relational_operator;
	query_attribute->primary_key_index = primary_key_index;
	query_attribute->from_data = from_data;
	query_attribute->to_data = to_data;

	return query_attribute;
}

enum relational_operator query_attribute_relational_operator(
			char *operator_name,
			char *datatype_name )
{
	enum relational_operator relational_operator;

	if ( !operator_name || !*operator_name )
		relational_operator = relational_operator_empty;
	else
	if ( strcmp( operator_name, EQUAL_OPERATOR ) == 0 )
		relational_operator = equals;
	else
	if ( strcmp( operator_name, NOT_EQUAL_OPERATOR ) == 0 )
		relational_operator = not_equal;
	else
	if ( strcmp( operator_name, NOT_EQUAL_OR_NULL_OPERATOR ) == 0 )
		relational_operator = not_equal_or_null;
	else
	if ( strcmp( operator_name, LESS_THAN_OPERATOR ) == 0 )
		relational_operator = less_than;
	else
	if ( strcmp( operator_name, LESS_THAN_EQUAL_TO_OPERATOR ) == 0 )
		relational_operator = less_than_equal_to;
	else
	if ( strcmp( operator_name, GREATER_THAN_OPERATOR ) == 0 )
		relational_operator = greater_than;
	else
	if ( strcmp( operator_name, GREATER_THAN_EQUAL_TO_OPERATOR ) == 0 )
		relational_operator = greater_than_equal_to;
	else
	if ( strcmp( operator_name, BETWEEN_OPERATOR ) == 0 )
		relational_operator = between;
	else
	if ( strcmp( operator_name, BEGINS_OPERATOR ) == 0 )
		relational_operator = begins;
	else
	if ( strcmp( operator_name, CONTAINS_OPERATOR ) == 0 )
		relational_operator = contains;
	else
	if ( strcmp( operator_name, NOT_CONTAINS_OPERATOR ) == 0 )
		relational_operator = not_contains;
	else
	if ( strcmp( operator_name, OR_OPERATOR ) == 0 )
		relational_operator = query_or;
	else
	if ( strcmp( operator_name, NULL_OPERATOR ) == 0 )
		relational_operator = is_null;
	else
	if ( strcmp( operator_name, NOT_NULL_OPERATOR ) == 0 )
		relational_operator = not_null;
	else
		relational_operator = relational_operator_unknown;

	if ( relational_operator == equals
	&&   attribute_is_date_time( datatype_name ) )
	{
		relational_operator = begins;
	}

	return relational_operator;
}

char *query_display_where(
			char *query_output_where,
			char *folder_name )
{
	char remove_folder_name[ 256 ];

	if ( !query_output_where ) return "";

	sprintf(remove_folder_name,
		"%s.",
		folder_name );

	search_replace_string(
		query_output_where,
		remove_folder_name,
		"" );

	search_replace_string(
		query_output_where,
		"1 = 1 and ",
		"" );

	search_replace_string(
		query_output_where,
		"'",
		"" );

	format_initial_capital( query_output_where, query_output_where );

	if ( !*query_output_where )
	{
		strcpy( query_output_where, "Entire folder" );
	}

	return query_output_where;
}

char *query_isa_related_join(
			char *folder_name,
			LIST *primary_key_list,
			char *isa_folder_name,
			LIST *isa_foreign_attribute_name_list )
{
	static char where[ 2048 ];
	char *ptr = where;
	char *primary_key;
	char *foreign_attribute_name;

	*ptr = '\0';

	if ( !list_length( primary_key_list ) )
		return where;

	if (	list_length( primary_key_list ) !=
		list_length( isa_foreign_attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: length = %d != length = %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( primary_key_list ),
			list_length( isa_foreign_attribute_name_list ) );
		exit( 1 );
	}

	list_rewind( primary_key_list );
	list_rewind( isa_foreign_attribute_name_list );

	do {
		primary_key =
			list_get(
				primary_key_list );

		foreign_attribute_name =
			list_get(
				isa_foreign_attribute_name_list );

		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
				ptr,
				"%s.%s = %s.%s",
				folder_name,
				primary_key,
				isa_folder_name,
				foreign_attribute_name );

		list_next( isa_foreign_attribute_name_list );

	} while( list_next( primary_key_list ) );

	return where;
}

char *query_relational_operator_display(
			enum relational_operator relational_operator )
{
	if ( relational_operator == equals )
		return EQUAL_OPERATOR;
	else
	if ( relational_operator == not_equal )
		return NOT_EQUAL_OPERATOR;
	else
	if ( relational_operator == not_equal_or_null )
		return NOT_EQUAL_OR_NULL_OPERATOR;
	else
	if ( relational_operator == less_than )
		return LESS_THAN_OPERATOR;
	else
	if ( relational_operator == less_than_equal_to )
		return LESS_THAN_EQUAL_TO_OPERATOR;
	else
	if ( relational_operator == greater_than )
		return GREATER_THAN_OPERATOR;
	else
	if ( relational_operator == greater_than_equal_to )
		return GREATER_THAN_EQUAL_TO_OPERATOR;
	else
	if ( relational_operator == between )
		return BETWEEN_OPERATOR;
	else
	if ( relational_operator == begins )
		return BEGINS_OPERATOR;
	else
	if ( relational_operator == contains )
		return CONTAINS_OPERATOR;
	else
	if ( relational_operator == not_contains )
		return NOT_CONTAINS_OPERATOR;
	else
	if ( relational_operator == query_or )
		return OR_OPERATOR;
	else
	if ( relational_operator == is_null )
		return NULL_OPERATOR;
	else
	if ( relational_operator == not_null )
		return NOT_NULL_OPERATOR;
	else
		return "Unknown";
}

boolean query_attribute_date_time_between(
			QUERY_ATTRIBUTE **date_between_attribute,
			QUERY_ATTRIBUTE **time_between_attribute,
			LIST *query_attribute_list )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( !list_rewind( query_attribute_list ) ) return 0;

	*date_between_attribute = (QUERY_ATTRIBUTE *)0;
	*time_between_attribute = (QUERY_ATTRIBUTE *)0;

	do {
		query_attribute = list_get( query_attribute_list );

		if (  	query_attribute->primary_key_index
		&&    	query_attribute->relational_operator == between
		&&	attribute_is_time(
				query_attribute->datatype_name ) )
		{
			*time_between_attribute = query_attribute;
			continue;
		}

		if (   	query_attribute->primary_key_index
		&&     	query_attribute->relational_operator == between
		&&	attribute_is_date(
				query_attribute->datatype_name ) )
		{
			*date_between_attribute = query_attribute;
			continue;
		}

	} while( list_next( query_attribute_list ) );

	return ( *date_between_attribute && *time_between_attribute );
}

char *query_drop_down_data_where(
			char *folder_name,
			char *attribute_name,
			char *data )
{
	static char where[ 1024 ];

	*where = '\0';

	if ( string_strcmp( data, NULL_OPERATOR ) == 0 )
	{
		sprintf(where,
			" (%s = 'null' or %s = '' or %s is null)",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	attribute_full_attribute_name(
		 		folder_name,
		 		attribute_name ),
		 	attribute_full_attribute_name(
		 		folder_name,
		 		attribute_name ),
		 	attribute_full_attribute_name(
		 		folder_name,
		 		attribute_name ) );
	}
	else
	if ( string_strcmp( data, NOT_NULL_OPERATOR ) == 0 )
	{
		sprintf(where,
			" (%s <> 'null' or %s <> '' or %s is not null)",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	attribute_full_attribute_name(
		 		folder_name,
		 		attribute_name ),
		 	attribute_full_attribute_name(
		 		folder_name,
		 		attribute_name ),
		 	attribute_full_attribute_name(
		 		folder_name,
		 		attribute_name ) );
	}
	else
	{
		sprintf(where,
			"%s = '%s'",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	attribute_full_attribute_name(
		 		folder_name,
		 		attribute_name ),
		 	data );
	}

	return where;
}

QUERY_OR_SEQUENCE *query_or_sequence_new( LIST *attribute_name_list )
{
	QUERY_OR_SEQUENCE *query_or_sequence;
	int length;
	int i;

	if ( ! ( length = list_length( attribute_name_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_length() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( query_or_sequence =
			calloc( 1, sizeof( QUERY_OR_SEQUENCE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_or_sequence->attribute_name_list = attribute_name_list;
	query_or_sequence->data_list_list = list_new();

	for ( i = 0; i < length; i++ )
	{
		list_set(
			query_or_sequence->data_list_list,
			list_new() );
	}

	return query_or_sequence;
}

boolean query_or_sequence_set_data_list_string(
			LIST *data_list_list,
			char *data_list_string )
{
	char delimiter;

	delimiter = string_delimiter( data_list_string );

	return
	query_or_sequence_set_data_list(
		data_list_list,
		list_string_list(
			data_list_string,
			delimiter ) );
}

boolean query_or_sequence_set_data_list(
			LIST *data_list_list,
			LIST *data_list )
{
	if ( !list_rewind( data_list_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: data_list_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( data_list ) ) return 0;

	do {
		list_set(
			list_get(
				data_list_list ),
			list_get( data_list ) );

		list_next( data_list_list );

	} while( list_next( data_list ) );

	return 1;
}

char *query_or_sequence_where_clause(
			LIST *attribute_name_list,
			LIST *data_list_list,
			boolean with_and_prefix )
{
	char *attribute_name;
	LIST *data_list;
	char *data;
	char where_clause[ STRING_WHERE_BUFFER ];
	char *where_ptr = where_clause;
	boolean first_time_and;
	boolean first_time_or;

	if ( !list_rewind( data_list_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: data_list_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*where_ptr = '\0';

	if ( with_and_prefix )
	{
		where_ptr += sprintf( where_ptr, "and ( " );
	}

	first_time_or = 1;

	do {
		if ( first_time_or )
		{
			first_time_or = 0;
		}
		else
		{
			where_ptr += sprintf( where_ptr, " or " );
		}

		data_list = list_get( data_list_list );

		list_rewind( attribute_name_list );
		list_rewind( data_list );

		where_ptr += sprintf( where_ptr, "(" );
		first_time_and = 1;

		do {
			attribute_name = list_get( attribute_name_list );
			data = list_get( data_list );

			if ( first_time_and )
			{
				first_time_and = 0;
			}
			else
			{
				where_ptr +=
					sprintf( where_ptr,
						 " and " );
			}

			where_ptr +=
				sprintf( where_ptr,
					 "%s = '%s'",
					 attribute_name,
					 data );

			list_next( data_list );

		} while( list_next( attribute_name_list ) );

		where_ptr += sprintf( where_ptr, ")" );

	} while( list_next( data_list_list ) );

	if ( with_and_prefix )
	{
		where_ptr += sprintf( where_ptr, " )" );
	}

	return strdup( where_clause );
}

QUERY_DATE_CONVERT *query_date_convert_calloc( void )
{
	QUERY_DATE_CONVERT *query_date_convert;

	if ( ! ( query_date_convert =
			calloc( 1, sizeof( QUERY_DATE_CONVERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}
	return query_date_convert;
}

QUERY_DATE_CONVERT *query_date_convert_new(
			char *login_name )
{
	QUERY_DATE_CONVERT *query_date_convert = query_date_convert_calloc();

	query_date_convert->date_convert_format =
		date_convert_user_date_format(
			environment_application_name(),
			login_name );

	return query_date_convert;
}

char *query_date_convert_select_string(
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			enum date_convert_format date_convert_format )
{
	static char select_string[ 64 ];

	if ( !attribute_name || !*attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( date_convert_format == american )
	{
		if ( attribute_is_date )
		{
			sprintf(select_string,
				"date_format(%s,'%cc/%ce/%cY')",
				attribute_name,
				'%',
				'%',
				'%' );
		}
		else
		if ( attribute_is_date_time )
		{
			sprintf(select_string,
				"date_format(%s,'%cc/%ce/%cY %cT')",
				attribute_name,
				'%',
				'%',
				'%',
				'%' );
		}
	}
	else
	if ( date_convert_format == military )
	{
		if ( attribute_is_date )
		{
			sprintf(select_string,
				"date_format(%s,'%cd-%cb-%cY')",
				attribute_name,
				'%',
				'%',
				'%' );
		}
		else
		if ( attribute_is_date_time )
		{
			sprintf(select_string,
				"date_format(%s,'%cd-%cb-%cY %cT')",
				attribute_name,
				'%',
				'%',
				'%',
				'%' );
		}
	}
	else
	{
		strcpy( select_string, attribute_name );
	}

	return select_string;
}

QUERY_DATA *query_data_new(
			char *attribute_name,
			char *datatype_name,
			char *escaped_replaced_data )
{
	QUERY_DATA *query_data;

	if ( !escaped_replaced_data || !*escaped_replaced_data )
		return (QUERY_DATA *)0;

	if ( ! ( query_data = calloc( 1, sizeof( QUERY_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	query_data->attribute_name = attribute_name;
	query_data->datatype_name = datatype_name;

	query_data->escaped_replaced_data =
		query_data_convert_date_international(
			query_data->datatype_name,
			escaped_replaced_data );

	return query_data;
}

char *query_data_convert_date_international(
			char *datatype_name,
			char *escaped_replaced_data )
{
	char date_string[ 64 ];
	char time_string[ 64 ];
	char return_date_time[ 128 ];
	enum date_convert_format date_convert_format;
	DATE_CONVERT *date;

	if ( !escaped_replaced_data || !*escaped_replaced_data )
		return escaped_replaced_data;

	if ( !attribute_is_date( datatype_name )
	&&   !attribute_is_time( datatype_name )
	&&   !attribute_is_date_time( datatype_name ) )
	{
		return escaped_replaced_data;
	}

	date_convert_format =
		date_convert_date_time_evaluate(
			escaped_replaced_data
				/* date_time_string */ );

	if ( date_convert_format == date_convert_unknown )
		return escaped_replaced_data;

	*time_string = '\0';

	column( date_string, 0, escaped_replaced_data );
	column( time_string, 1, escaped_replaced_data );

	date =
		date_convert_string_international(
			date_convert_format /* source_format */,
			date_string );

	if ( !date )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: date_convert_string_international() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	if ( *time_string )
	{
		sprintf(return_date_time,
			"%s %s",
			date->return_date,
			time_string );
	}
	else
	{
		strcpy( return_date_time, date->return_date );
	}

	date_convert_free( date );
	return strdup( return_date_time );
}

LIST *query_data_string_list(
			char *data_list_string )
{
	return list_string_list(
			data_list_string,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );
}

LIST *query_data_list(	LIST *foreign_key_list,
			LIST *many_folder_attribute_list,
			LIST *data_string_list )
{
	char *foreign_key;
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *query_data_list;

	if ( !list_length( data_string_list ) ) return (LIST *)0;

	if (	list_length( data_string_list ) !=
		list_length( foreign_key_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: data_string_list length = %d != foreign_key_list length = %d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( data_string_list ),
			list_length( foreign_key_list ) );
		exit( 1 );
	}

	query_data_list = list_new();

	list_rewind( data_string_list );
	list_rewind( foreign_key_list );

	do {
		foreign_key =
			list_get(
				foreign_key_list );

		if ( ! ( folder_attribute =
				folder_attribute_list_seek( 
					foreign_key,
					many_folder_attribute_list ) ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: folder_attribute_seek(%s) returned empty.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	foreign_key );
			exit( 1 );
		}

		list_set(
			query_data_list,
			query_data_new(
				foreign_key /* attribute_name */,
				folder_attribute->attribute->datatype_name,
				security_sql_injection_escape(
					security_replace_special_characters(
					     string_trim_number_characters(
						  (char *)list_get(
						  data_string_list ),
						  folder_attribute->
							attribute->
							datatype_name ) ) ) ) );

		list_next( data_string_list );

	} while ( list_next( foreign_key_list ) );

	return query_data_list;
}

char *query_isa_join_where(
			char *folder_name,
			LIST *primary_key_list,
			LIST *relation_mto1_isa_list )
{
	RELATION *mto1_isa;
	char where[ STRING_WHERE_BUFFER ];
	char *ptr = where;

	*ptr = '\0';

	if ( !list_rewind( relation_mto1_isa_list ) ) return (char *)0;

	do {
		mto1_isa = list_get( relation_mto1_isa_list );

		if ( !list_length(
			mto1_isa->
				foreign_key_list ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_isa_related_join(
				folder_name,
				primary_key_list,
				mto1_isa->
					many_folder->
					folder_name,
				mto1_isa->
					foreign_key_list ) );

	} while( list_next( relation_mto1_isa_list ) );

	return strdup( where );
}

char *query_output_where(
			char *query_drop_down_data_where,
			char *query_attribute_where,
			char *query_isa_join_where,
			char *attribute_not_null_join )
{
	char where[ STRING_WHERE_BUFFER ];
	char *ptr = where;

	*ptr = '\0';

	if ( string_strlen( query_drop_down_data_where ) )
	{
		ptr += sprintf(
			ptr,
			"%s",
			query_drop_down_data_where );
	}

	if ( string_strlen( query_attribute_where ) )
	{
		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_attribute_where );
	}

	if ( string_strlen( query_isa_join_where ) )
	{
		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_isa_join_where );
	}

	if ( string_strlen( attribute_not_null_join ) )
	{
		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			attribute_not_null_join );
	}

	return strdup( where );
}

char *query_attribute_name_list_order(
			char *folder_name,
			LIST *attribute_name_list,
			char *descending_clause )
{
	char order[ 1024 ];
	char *ptr = order;
	char *attribute_name;
	char *table_name =
		get_table_name(
			environment_application_name(),
			folder_name );

	if ( !folder_name || !*folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	
	*ptr = '\0';

	do {
		attribute_name = list_get( attribute_name_list );

		if ( !*ptr ) ptr += sprintf( ptr, "," );

		ptr += sprintf(	ptr,
				"%s.%s%s",
				table_name,
				attribute_name,
				descending_clause );

	} while ( list_next( attribute_name_list ) );

	return strdup( order );
}

char *query_sort_prefix_direction_attribute_index(
			DICTIONARY *sort_dictionary )
{
	LIST *key_list;

	key_list = dictionary_key_list( sort_dictionary );

	if ( !list_rewind( key_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: dictionary_key_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* List of one */
	/* ----------- */
	return list_get( key_list );
}

char *query_output_order(
			char *mto1_folder_name,
			LIST *primary_key_list,
			DICTIONARY *sort_dictionary )
{
	char *prefix_direction_attribute_index;
	char prefix_direction_attribute[ 512 ];
	char direction_attribute[ 512 ];
	char attribute_list_string[ 512 ];
	char *descending_label = "";

	if ( !dictionary_length( sort_dictionary ) )
	{
		return query_attribute_name_list_order(
			mto1_folder_name,
			primary_key_list,
			descending_label );
	}

	prefix_direction_attribute_index =
		/* ------------------------------------------------------- */
		/* Sample: ssort_button_descend_full_name^street_address_0 */
		/* ------------------------------------------------------- */
		query_sort_prefix_direction_attribute_index(
			sort_dictionary );

	trim_index(
		prefix_direction_attribute,
		prefix_direction_attribute_index );

	string_strcpy(
		direction_attribute,
		prefix_direction_attribute + strlen( SORT_BUTTON_PREFIX ),
		0 );

	if ( string_strncmp(
		direction_attribute,
		FORM_DESCENDING_LABEL ) == 0 )
	{
		string_strcpy(
			attribute_list_string,
			direction_attribute +
			strlen( FORM_DESCENDING_LABEL ),
			0 );

		descending_label = " desc";
	}
	else
	{
		string_strcpy(
			attribute_list_string,
			direction_attribute +
			strlen( FORM_SORT_LABEL ),
			0 );
	}

	return query_attribute_name_list_order(
			mto1_folder_name,
			list_string_list(
				attribute_list_string,
				FOLDER_DATA_DELIMITER )
					/* primary_key_list */,
			descending_label );
}

char *query_output_select_display(
			char *mto1_folder_name,
			LIST *query_output_select_name_list,
			int mto1_isa_related_folder_list_length )
{
	char *select_name;
	char display_string[ STRING_WHERE_BUFFER ];
	char *ptr = display_string;

	if ( !list_rewind( query_output_select_name_list ) )
		return (char *)0;

	*ptr = '\0';

	do {
		select_name =
			list_get(
				query_output_select_name_list );

		if ( mto1_isa_related_folder_list_length )
		{
			ptr += sprintf( ptr, "%s.", mto1_folder_name );
		}

		if ( *ptr ) ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", select_name );

	} while ( list_next( query_output_select_name_list ) );

	return strdup( display_string );
}

LIST *query_system_dictionary_list(
			char *system_string,
			LIST *select_name_list )
{
	char buffer[ STRING_WHERE_BUFFER ];
	char data[ 65536 ];
	LIST *list = list_new();
	DICTIONARY *dictionary;
	char *attribute_name;
	int i;
	FILE *pipe;

	if ( !list_length( select_name_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d select_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pipe = popen( system_string, "r" );

	while( string_input( buffer, pipe, STRING_WHERE_BUFFER ) )
	{
		dictionary = dictionary_small();
		list_set( list, dictionary );
		i = 0;

		list_rewind( select_name_list );

		do {
			if ( !piece( data, SQL_DELIMITER, buffer, i++ ) )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: piece(%d) returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					i - 1 );

				pclose( pipe );
				exit( 1 );
			}

			attribute_name = list_get( select_name_list );

			dictionary_set_pointer(
				dictionary, 
				attribute_name, 
				strdup( data ) );

		} while ( list_next( select_name_list ) );
	}

	pclose( pipe );
	return list;
}

QUERY *query_isa_widget_new(
			char *one2m_isa_folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_primary_list,
			char *login_name,
			char *security_entity_where )
{
	QUERY *query = query_calloc();

	if ( ! ( query->select_list =
			query_primary_select_list(
				primary_key_list,
				folder_attribute_primary_list,
				/* --------------- */
				/* Always succeeds */
				/* --------------- */
				query_date_convert_new(
					login_name ) ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: query_primary_select_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return query;
	}

	if ( ! ( query->select_clause =
			query_select_clause(
				query->select_list ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: query_select_clause() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return query;
	}

	query->from_clause = one2m_isa_folder_name;
	query->where_clause = security_entity_where;
	query->order_clause = query->select_clause;

	return query;
}

QUERY_SELECT *query_select_new(
			char *folder_name,
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			QUERY_DATE_CONVERT *query_date_convert )
{
	QUERY_SELECT *query_select;

	if ( ! ( query_select = calloc( 1, sizeof( QUERY_SELECT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_select->folder_name = folder_name;
	query_select->attribute_name = attribute_name;
	query_select->attribute_is_date = attribute_is_date;
	query_select->attribute_is_date_time = attribute_is_date_time;

	if ( query_select->attribute_is_date
	||   query_select->attribute_is_date_time )
	{
		if ( !query_date_convert )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: query_date_convert is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		query_select->query_select_string =
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			query_select_string(
				folder_name,
				attribute_name,
				query_select->attribute_is_date,
				query_select->attribute_is_date_time,
				query_select->
					query_date_convert->
					date_convert_format );
	}
	else
	{
		query_select->query_select_string =
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			query_select_string(
				folder_name,
				attribute_name,
				0 /* not attribute_is_date */,
				0 /* not attribute_is_date_time */,
				date_convert_unknown );
	}

	return query_select;
}

char *query_select_string(
			char *folder_name,
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			enum date_convert_format date_convert_format )
{
	char select_string[ 128 ];

	*select_string = '\0';

	if ( folder_name && *folder_name )
	{
		if ( date_convert_format != date_convert_unknown )
		{
			sprintf(select_string,
				"%s.%s",
				folder_name,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				query_date_convert_select_string(
					attribute_name,
					attribute_is_date,
					attribute_is_date_time,
					date_convert_format ) );
		}
		else
		{
			sprintf(select_string,
				"%s.%s",
				folder_name,
				attribute_name );
		}
	}
	else
	{
		if ( date_convert_format != date_convert_unknown )
		{
			strcpy( select_string,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				query_date_convert_select_string(
					attribute_name,
					attribute_is_date,
					attribute_is_date_time,
					date_convert_format ) );
		}
		else
		{
			strcpy( select_string, attribute_name );
		}
	}

	return strdup( select_string );
}

LIST *query_select_list(
			LIST *folder_attribute_append_isa_list,
			LIST *ignore_select_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			int relation_mto1_isa_length,
			QUERY_DATE_CONVERT *query_date_convert )
{
	LIST *select_list;
	FOLDER_ATTRIBUTE *folder_attribute;
	char *folder_name = {0};

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (LIST *)0;

	select_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( list_string_exists(
			folder_attribute->attribute_name,
			ignore_select_attribute_name_list ) )
		{
			continue;
		}

		if ( list_string_exists(
			folder_attribute->attribute_name,
			exclude_lookup_attribute_name_list ) )
		{
			continue;
		}

		if ( relation_mto1_isa_length )
			folder_name =
				folder_attribute->folder_name;

		list_set(
			select_list,
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			query_select_new(
				folder_name,
				folder_attribute->attribute_name,
				attribute_is_date(
					folder_attribute->
						attribute->
						datatype_name ),
				attribute_is_date_time(
					folder_attribute->
						attribute->
						datatype_name ),
				query_date_convert ) );

	} while ( list_next( folder_attribute_append_isa_list ) );

	return select_list;
}

LIST *query_primary_select_list(
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			QUERY_DATE_CONVERT *query_date_convert )
{
	char *primary_key;
	LIST *select_list;
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !list_rewind( primary_key_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	select_list = list_new();

	do {
		primary_key = list_get( primary_key_list );

		if ( ! ( folder_attribute =
				folder_attribute_seek(
					primary_key,
					folder_attribute_list ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_seek(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				primary_key );
			exit( 1 );
		}


		list_set(
			select_list,
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			query_select_new(
				(char *)0 /* folder_name */,
				folder_attribute->attribute_name,
				attribute_is_date(
					folder_attribute->
						attribute->
						datatype_name ),
				attribute_is_date_time(
					folder_attribute->
						attribute->
						datatype_name ),
				query_date_convert ) );

	} while ( list_next( primary_key_list ) );

	return select_list;
}

char *query_select_clause( LIST *select_list )
{
	char clause[ 2048 ];
	char *ptr = clause;
	QUERY_SELECT *query_select;

	if ( !list_rewind( select_list ) ) return (char *)0;

	*ptr = '\0';

	do {
		query_select = list_get( select_list );

		if ( !*ptr ) ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", query_select->query_select_string );

	} while ( list_next( select_list ) );

	return strdup( clause );
}

LIST *query_drop_down_list(
			LIST *exclude_attribute_name_list /* out */,
			char *many_folder_name,
			LIST *many_folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	RELATION *relation_mto1;
	QUERY_DROP_DOWN *query_drop_down;
	LIST *drop_down_list = {0};
	enum relational_operator relational_operator;
	int highest_index;

	if ( !list_rewind( many_folder_attribute_list ) ) return (LIST *)0;

	do {
		folder_attribute =
			list_get(
				many_folder_attribute_list );

		if ( ! ( relation_mto1 =
				relation_consumes(
					folder_attribute->attribute_name,
					relation_mto1_non_isa_list ) ) )
		{
			continue;
		}

		if ( !relation_mto1->one_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !list_length(
			relation_mto1->
				one_folder->folder_attribute_list ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: one_folder = %s has empty folder_attribute_list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				relation_mto1->one_folder->folder_name );
			exit( 1 );
		}

		relation_mto1->consumes_taken = 1;

		highest_index =
			dictionary_string_list_highest_index(
				relation_mto1->foreign_key_list,
				drillthru_dictionary );

		if ( highest_index < 0 ) continue;

		relational_operator =
			query_relational_operator(
				query_operator_name(
					query_drop_down_operator_key(
						relation_mto1->
							foreign_key_list ),
					drillthru_dictionary ),
				(char *)0 /* datatype_name */ );

		if ( relational_operator == is_null
		||   relational_operator == not_null )
		{
			query_drop_down = query_drop_down_calloc();

			query_drop_down->many_folder_name = many_folder_name;

			query_drop_down->one_folder_name =
				relation_mto1->one_folder->folder_name;

			query_drop_down->foreign_key_list =
				relation_mto1->foreign_key_list;

			query_drop_down->relational_operator =
				relational_operator;

			if ( !drop_down_list )
				drop_down_list =
					list_new();

			/* Need to execute string_delimiter_repeat() */
			/* ----------------------------------------- */
			list_set( drop_down_list, query_drop_down );

			continue;
		}

		query_drop_down =
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			query_drop_down_new(
				many_folder_name,
				many_folder_attribute_list,
				relation_mto1->one_folder->folder_name
					/* one_folder_name */,
				relation_mto1->one_folder->primary_key_list
					/* one_primary_key_list */,
				relation_mto1->foreign_key_list,
				drillthru_dictionary,
				highest_index );

		query_drop_down->relational_operator = relational_operator;

		if ( !drop_down_list ) drop_down_list = list_new();

		list_set( drop_down_list, query_drop_down );

		list_set_list(
			exclude_attribute_name_list,
			relation_mto1->foreign_key_list );

	} while ( list_next( many_folder_attribute_list ) );

	return drop_down_list;
}

QUERY_DROP_DOWN *query_drop_down_new(
			char *many_folder_name,
			LIST *many_folder_attribute_list,
			char *one_folder_name,
			LIST *one_primary_key_list,
			LIST *foreign_key_list,
			DICTIONARY *drillthru_dictionary,
			int highest_index )
{
	QUERY_DROP_DOWN *query_drop_down;

	query_drop_down = query_drop_down_calloc();

	query_drop_down->many_folder_name = many_folder_name;
	query_drop_down->one_folder_name = one_folder_name;
	query_drop_down->one_primary_key_list = one_primary_key_list;
	query_drop_down->foreign_key_list = foreign_key_list;
	query_drop_down->drillthru_dictionary = drillthru_dictionary;
	query_drop_down->highest_index = highest_index;

	query_drop_down->row_list =
		query_drop_down_row_list(
			one_primary_key_list,
			foreign_key_list,
			many_folder_attribute_list,
			drillthru_dictionary,
			highest_index );

	if ( !list_length( query_drop_down->row_list ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: query_drop_down_row_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (QUERY_DROP_DOWN *)0;
	}

	return query_drop_down;
}

QUERY_DROP_DOWN *query_drop_down_calloc( void )
{
	QUERY_DROP_DOWN *query_drop_down;

	if ( ! ( query_drop_down = calloc( 1, sizeof( QUERY_DROP_DOWN ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return query_drop_down;
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc( void )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;

	if ( ! ( query_drop_down_row =
			calloc( 1, sizeof( QUERY_DROP_DOWN_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		exit( 1 );
	}
	return query_drop_down_row;
}

LIST *query_attribute_list(
			LIST *folder_attribute_list,
			DICTIONARY *dictionary,
			LIST *exclude_attribute_name_list )
{
	LIST *query_attribute_list = {0};
	QUERY_ATTRIBUTE *query_attribute;
	FOLDER_ATTRIBUTE *folder_attribute;
	QUERY_DATA *from_data;
	QUERY_DATA *to_data;
	char *operator_name;
	enum relational_operator relational_operator;

	if ( !list_rewind( folder_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( list_exists_string(
			folder_attribute->attribute_name,
			exclude_attribute_name_list ) )
		{
			continue;
		}

		if ( ! ( operator_name =
				query_operator_name(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					query_attribute_operator_key(
						folder_attribute->
							attribute_name ),
					dictionary ) ) )
		{
			if ( ! ( operator_name =
					query_yes_no_operator_name(
						folder_attribute->
							attribute_name ) ) )
			{
				continue;
			}
		}

		relational_operator =
			query_relational_operator(
				operator_name,
				folder_attribute->attribute->datatype_name );

		if ( relational_operator == is_null
		||   relational_operator == not_null )
		{
			continue;
		}

		from_data = (QUERY_DATA *)0;
		to_data = (QUERY_DATA *)0;

		if ( ! ( from_data =
				query_data_new(
					folder_attribute->attribute_name,
					folder_attribute->
						attribute->
						datatype_name,
					security_sql_injection_escape(
					    security_replace_special_characters(
					     string_trim_number_characters(
						dictionary_starting_label_get(
						    folder_attribute->
								attribute_name,
						    QUERY_FROM_STARTING_LABEL,
						    dictionary ),
						folder_attribute->
						     attribute->
						     datatype_name ) ) ) ) ) )
		{
			continue;
		}

		to_data =
			query_data_new(
				folder_attribute->attribute_name,
				folder_attribute->
					attribute->
					datatype_name,
				security_sql_injection_escape(
				    security_replace_special_characters(
					dictionary_starting_label_get(
						folder_attribute->
							attribute_name,
						QUERY_TO_STARTING_LABEL,
						dictionary ) ) ) );

		query_attribute =
			query_attribute_new(
				folder_attribute->attribute_name,
				folder_attribute->folder_name,
				folder_attribute->attribute->datatype_name,
				relational_operator,
				from_data,
				to_data,
				folder_attribute->primary_key_index );

		if ( !query_attribute_list )
		{
			query_attribute_list = list_new();
		}

		list_set(
			query_attribute_list,
			query_attribute );

	} while( list_next( folder_attribute_list ) );

	return query_attribute_list;
}

char *query_yes_no_operator_name(
			char *attribute_name )
{
	if ( attribute_is_yes_no( attribute_name ) )
	{
		return EQUAL_OPERATOR;
	}
	else
	{
		return (char *)0;
	}
}

char *query_drop_down_list_where(
			LIST *drop_down_list,
			int relation_mto1_isa_list_length )
{
	QUERY_DROP_DOWN *drop_down;
	QUERY_DROP_DOWN_ROW *drop_down_row;
	QUERY_DATA *data;
	char where_clause[ STRING_WHERE_BUFFER ];
	char *ptr = where_clause;
	boolean outter_first_time;
	boolean inner_first_time;
	boolean inner_inner_first_time;

	*ptr = '\0';

	if ( !list_rewind( drop_down_list ) ) return (char *)0;

	outter_first_time = 1;

	ptr += sprintf( ptr, " (" );

	do {
		drop_down = list_get( drop_down_list );

		if ( outter_first_time )
		{
			outter_first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " and (" );
		}

		if ( !list_rewind( drop_down->row_list ) ) continue;

		inner_first_time = 1;

		ptr += sprintf( ptr, " (" );

		do {
			drop_down_row =
				list_get(
					drop_down->row_list );

			if ( !list_rewind( drop_down_row->query_data_list ) )
				continue;

			if ( inner_first_time )
			{
				inner_first_time = 0;
			}
			else
			{
				ptr += sprintf( ptr, " or (" );
			}

			inner_inner_first_time = 1;

			do {
				data =
					list_get(
						drop_down_row->
							query_data_list );

				if ( inner_inner_first_time )
				{
					inner_inner_first_time = 0;
				}
				else
				{
					ptr += sprintf( ptr, " and" );
				}

				ptr += sprintf(
					ptr,
					"%s",
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					query_drop_down_data_where(
						(relation_mto1_isa_list_length)
						? drop_down->
							many_folder_name
						: (char *)0,
						data->attribute_name,
						data->escaped_replaced_data ) );

			} while( list_next( drop_down_row->query_data_list ) );

			if ( !inner_inner_first_time )
				ptr += sprintf( ptr, " )" );

		} while( list_next( drop_down->row_list ) );

		ptr += sprintf( ptr, " )" );

	} while( list_next( drop_down_list ) );

	return strdup( where_clause );
}

char *query_attribute_list_where(
			LIST *query_attribute_list )
{
	char where_clause[ STRING_WHERE_BUFFER ];
	char *ptr = where_clause;
	QUERY_ATTRIBUTE *date_between_attribute = {0};
	QUERY_ATTRIBUTE *time_between_attribute = {0};
	QUERY_ATTRIBUTE *query_attribute;
	boolean first_time = 1;
	char *operator_character_string;
	char *application_name = environment_application_name();
	LIST *exclude_attribute_name_list = list_new();

	*ptr = '\0';

	if ( !list_length( query_attribute_list ) ) return (char *)0;

	if ( query_attribute_date_time_between(
			&date_between_attribute,
			&time_between_attribute,
			query_attribute_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_attribute_between_date_time_where(
				date_between_attribute->attribute_name,
				time_between_attribute->attribute_name,
				date_between_attribute->from_data,
				time_between_attribute->from_data,
				date_between_attribute->to_data,
				time_between_attribute->to_data,
				/* ----------------------------- */
				/* Returns static memory or null */
				/* ----------------------------- */
				folder_table_name(
					application_name,
					date_between_attribute->
						folder_name ) ) );

		list_set(
			exclude_attribute_name_list,
			date_between_attribute->attribute_name );

		list_set(
			exclude_attribute_name_list,
			time_between_attribute->attribute_name );

		first_time = 0;
	}

	list_rewind( query_attribute_list );

	do {
		query_attribute = list_get( query_attribute_list );

		if ( list_exists_string(
			query_attribute->attribute_name,
			exclude_attribute_name_list ) )
		{
			continue;
		}

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " and" );

		operator_character_string = 
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			query_operator_character_string(
				query_relational_operator_display(
					query_attribute->
						relational_operator ) );

		if ( attribute_is_number( query_attribute->datatype_name )
		&&   query_attribute->relational_operator == between )
		{
			ptr += sprintf(
				ptr,
				" %s >= %s and %s <= %s",
				attribute_full_attribute_name(
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				query_attribute->
					from_data->
					escaped_replaced_data,
				attribute_full_attribute_name(
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				query_attribute->
					to_data->
					escaped_replaced_data );
		}
		else
		if ( query_attribute->relational_operator == between )
		{
			ptr += sprintf(
				ptr,
				" %s %s '%s' and '%s'",
				attribute_full_attribute_name(
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				operator_character_string,
				query_attribute->
					from_data->
					escaped_replaced_data,
				query_attribute->
					to_data->
					escaped_replaced_data );
		}
		else
		if ( query_attribute->relational_operator == is_null )
		{
			ptr += sprintf(
			   	ptr,
			 	" (%s = 'null' or %s = '' or %s is null)",
				attribute_full_attribute_name(
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				attribute_full_attribute_name(
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				attribute_full_attribute_name(
			   		query_attribute->folder_name,
					query_attribute->attribute_name ) );
		}
		else
		if ( query_attribute->relational_operator == not_null )
		{
			ptr += sprintf(
			   	ptr,
			 	" (%s <> 'null' or %s <> '' or %s is not null)",
				attribute_full_attribute_name(
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				attribute_full_attribute_name(
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				attribute_full_attribute_name(
			   		query_attribute->folder_name,
					query_attribute->attribute_name ) );
		}
		else
		if ( query_attribute->relational_operator == query_or )
		{
			char piece_buffer[ 512 ];
			int i;
			char delimiter;

			delimiter =
				string_delimiter( 
					query_attribute->
						from_data->
						escaped_replaced_data );

			for(	i = 0;
				piece(	piece_buffer,
					delimiter,
					query_attribute->
						from_data->
						escaped_replaced_data,
					i );
				i++ )
			{
				if ( i == 0 )
				{
					ptr += sprintf( ptr, " (" );
				}
				else
				{
					ptr += sprintf( ptr, " or" );
				}

				ptr += sprintf(
			   		ptr,
			   		" %s = '%s'",
					attribute_full_attribute_name(
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
			   		strdup( piece_buffer ) );
			}
			ptr += sprintf( ptr, " )" );
		}
		else
		if ( query_attribute->relational_operator == begins )
		{
			ptr += sprintf( ptr,
					" %s like '%s%c'",
					attribute_full_attribute_name(
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					query_attribute->
						from_data->
						escaped_replaced_data,
					'%' );
		}
		else
		if ( query_attribute->relational_operator == contains )
		{
			ptr += sprintf( ptr,
					" %s like '%c%s%c'",
					attribute_full_attribute_name(
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					'%',
					query_attribute->
						from_data->
						escaped_replaced_data,
					'%' );
		}
		else
		if ( query_attribute->relational_operator == not_contains )
		{
			ptr += sprintf( ptr,
					" %s not like '%c%s%c'",
					attribute_full_attribute_name(
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					'%',
					query_attribute->
						from_data->
						escaped_replaced_data,
					'%' );
		}
		else
		if ( query_attribute->relational_operator == not_equal_or_null )
		{
			ptr +=
				sprintf(
			   	ptr,
			   	" (%s <> '%s' or %s is null)",
				attribute_full_attribute_name(
					query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				query_attribute->
					from_data->
					escaped_replaced_data,
				attribute_full_attribute_name(
					query_attribute->folder_name,
					query_attribute->attribute_name ) );
		}
		else
		if ( attribute_is_number( query_attribute->datatype_name ) )
		{
			ptr +=
				sprintf(
		   		ptr,
			   	" %s %s %s",
				attribute_full_attribute_name(
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				operator_character_string,
				query_attribute->
					from_data->
					escaped_replaced_data );
		}
		else
		if ( attribute_is_boolean( query_attribute->attribute_name ) )
		{
			if ( string_strcmp(
					query_attribute->
						from_data->
						escaped_replaced_data,
					NOT_NULL_OPERATOR ) == 0 )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s is not null",
					attribute_full_attribute_name(
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
			else
			if ( *query_attribute->
				from_data->
				escaped_replaced_data == 'n' )
			{
				char *full_attribute_name;

				full_attribute_name =
					attribute_full_attribute_name(
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name );

				ptr +=
					sprintf(
			   		ptr,
			   		" (%s = 'n' or %s is null)",
					full_attribute_name,
					full_attribute_name );
			}
			else
			if ( *query_attribute->
				from_data->
				escaped_replaced_data == 'y' )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s = 'y'",
					attribute_full_attribute_name(
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
			else
			if ( string_strcmp(
					query_attribute->
						from_data->
						escaped_replaced_data,
					NULL_OPERATOR ) == 0 )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s is null",
					attribute_full_attribute_name(
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
		}
		else
		{
			ptr +=
				sprintf( ptr,
				" %s %s '%s'",
				attribute_full_attribute_name(
			   		query_attribute->folder_name,
			   		query_attribute->
						attribute_name ),
				operator_character_string,
				query_attribute->
					from_data->
					escaped_replaced_data );
		}

	} while( list_next( query_attribute_list ) );

	return strdup( where_clause );
}

char *query_operator_character_string(
			char *operator_string )
{
	if ( strcmp( operator_string, EQUAL_OPERATOR ) == 0 )
		return "=";
	else
	if ( strcmp( operator_string, NOT_EQUAL_OPERATOR ) == 0 )
		return "<>";
	else
	if ( strcmp( operator_string, GREATER_THAN_OPERATOR ) == 0 )
		return ">";
	else
	if ( strcmp( operator_string, GREATER_THAN_EQUAL_TO_OPERATOR ) == 0 )
		return ">=";
	else
	if ( strcmp( operator_string, LESS_THAN_OPERATOR ) == 0 )
		return "<";
	else
	if ( strcmp( operator_string, LESS_THAN_EQUAL_TO_OPERATOR ) == 0 )
		return "<=";
	else
	if ( strcmp( operator_string, BEGINS_OPERATOR ) == 0 )
		return BEGINS_OPERATOR;
	else
	if ( strcmp( operator_string, CONTAINS_OPERATOR ) == 0 )
		return CONTAINS_OPERATOR;
	else
	if ( strcmp( operator_string, OR_OPERATOR ) == 0 )
		return OR_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_CONTAINS_OPERATOR ) == 0 )
		return NOT_CONTAINS_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_EQUAL_OR_NULL_OPERATOR ) == 0 )
		return NOT_EQUAL_OR_NULL_OPERATOR;
	else
	if ( strcmp( operator_string, BETWEEN_OPERATOR ) == 0 )
		return BETWEEN_OPERATOR;
	else
	if ( strcmp( operator_string, NULL_OPERATOR ) == 0 )
		return NULL_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_NULL_OPERATOR ) == 0 )
		return NOT_NULL_OPERATOR;
	else
		return "unknown operator";
}

QUERY *query_widget_new(
			char *widget_folder_name,
			char *login_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary )
{
	QUERY *query = query_calloc();

	if ( ! ( query->select_list =
			query_primary_select_list(
				primary_key_list,
				folder_attribute_list,
				/* --------------- */
				/* Always succeeds */
				/* --------------- */
				query_date_convert_new(
					login_name ) ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: query_primary_select_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return query;
	}

	if ( ! ( query->select_clause =
			query_select_clause(
				query->select_list ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: query_select_clause() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return query;
	}

	query->from_clause = widget_folder_name;

	query->query_widget_where =
		query_widget_where_new(
			widget_folder_name,
			folder_attribute_list,
			relation_mto1_non_isa_list,
			security_entity_where,
			drillthru_dictionary );

	query->order_clause = query->select_clause;

	return query;
}

QUERY_WIDGET_WHERE *query_widget_where_calloc( void )
{
	QUERY_WIDGET_WHERE *query_widget_where;

	if ( ! ( query_widget_where =
			calloc( 1, sizeof( QUERY_WIDGET_WHERE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_widget_where;
}

QUERY_WIDGET_WHERE *query_widget_where_new(
			char *widget_folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary )
{
	QUERY_WIDGET_WHERE *query_widget_where =
		query_widget_where_calloc();

	query_widget_where->exclude_attribute_name_list = list_new();

	query_widget_where->query_drop_down_list =
		query_drop_down_list(
			query_widget_where->
				exclude_attribute_name_list
					/* out */,
			widget_folder_name /* many_folder_name */,
			folder_attribute_list /* many_folder_attribute_list */,
			relation_mto1_non_isa_list,
			drillthru_dictionary );

	if ( list_length( query_widget_where->query_drop_down_list ) )
	{
		query_widget_where->query_drop_down_list_where =
			query_drop_down_list_where(
				query_widget_where->query_drop_down_list,
				0 /* relation_mto1_isa_list_length */ );
	}

	query_widget_where->query_attribute_list =
		query_attribute_list(
			folder_attribute_list,
			drillthru_dictionary,
			query_widget_where->exclude_attribute_name_list );

	if ( list_length( query_widget_where->query_attribute_list ) )
	{

		query_widget_where->query_attribute_list_where =
			query_attribute_list_where(
				query_widget_where->query_attribute_list );
	}

	query_widget_where->where_clause =
		query_widget_where_clause(
			query_widget_where->query_drop_down_list_where,
			query_widget_where->query_attribute_list_where,
			security_entity_where );

	return query_widget_where;
}

LIST *query_drop_down_row_list(
			LIST *one_primary_key_list,
			LIST *foreign_key_list,
			LIST *many_folder_attribute_list,
			DICTIONARY *drillthru_dictionary,
			int highest_index )
{
	LIST *drop_down_row_list = {0};
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	int index;

	for(	index = 0;
		index <= highest_index;
		index++ )
	{
		if ( ( query_drop_down_row =
			query_drop_down_row_new(
				one_primary_key_list,
				foreign_key_list,
				many_folder_attribute_list,
				drillthru_dictionary,
				index ) ) )
		{
			if ( !drop_down_row_list )
				drop_down_row_list =
					list_new();

			list_set(
				drop_down_row_list,
				query_drop_down_row );
		}
	}

	return drop_down_row_list;
}

char *query_widget_where_clause(
			char *drop_down_where,
			char *attribute_list_where,
			char *security_entity_where )
{
	char where_clause[ STRING_WHERE_BUFFER ];
	char *ptr = where_clause;
	boolean got_one = 0;

	if ( !drop_down_where
	&&   !attribute_list_where
	&&   !security_entity_where )
	{
		return (char *)0;
	}

	if ( drop_down_where )
	{
		ptr += sprintf( ptr, "%s", drop_down_where );
		got_one = 1;
	}

	if ( attribute_list_where )
	{
		if ( got_one ) ptr += sprintf( ptr, " and " );
		ptr += sprintf( ptr, "%s", attribute_list_where );
		got_one = 1;
	}

	if ( security_entity_where )
	{
		if ( got_one ) ptr += sprintf( ptr, " and " );
		ptr += sprintf( ptr, "%s", security_entity_where );
	}

	return strdup( where_clause );
}


char *query_data_where_clause(
			char *folder_name,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *folder_attribute_list )
{
	char where_clause[ STRING_WHERE_BUFFER ];
	char *ptr = where_clause;
	char *attribute_name;
	char full_attribute_name[ 128 ];
	char *data;
	char *table_name = {0};
	FOLDER_ATTRIBUTE *folder_attribute = {0};

	if ( folder_name )
	{
		table_name =
			folder_table_name(
				environment_application_name(),
				folder_name );
	}

	list_rewind( where_attribute_name_list );
	list_rewind( where_attribute_data_list );
	*ptr = '\0';

	do {
		attribute_name =
			list_get(
				where_attribute_name_list );

		if ( list_length( folder_attribute_list ) )
		{
			folder_attribute =
				folder_attribute_seek(
					attribute_name,
					folder_attribute_list );
		}

		if ( !list_at_head( where_attribute_name_list ) )
		{
			ptr += sprintf( ptr, " and " );
		}

		if ( table_name )
		{
			sprintf(full_attribute_name,
				"%s.%s",
				table_name,
				attribute_name );
		}
		else
		{
			strcpy( full_attribute_name, attribute_name );
		}

		if ( folder_attribute
		&&   attribute_is_date_time(
				folder_attribute->
					attribute->
					datatype_name )
		&&   folder_attribute->attribute->width == 16 )
		{
			ptr += sprintf( ptr,
					"substr(%s,1,16) = '%s'",
					full_attribute_name,
					data );
		}
		else
		{
			ptr += sprintf( ptr,
			 		"%s = '%s'",
			 		full_attribute_name,
			 		data );
		}

		if ( !list_next( where_attribute_data_list ) ) break;

	} while( list_next( where_attribute_name_list ) );

	return strdup( where_clause );
}
