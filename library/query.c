/* $APPASERVER_LIBRARY/library/query.c					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "sql.h"
#include "String.h"
#include "column.h"
#include "date_convert.h"
#include "folder.h"
#include "folder_attribute.h"
#include "attribute.h"
#include "form.h"
#include "environ.h"
#include "prompt_recursive.h"
#include "relation.h"
#include "row_security.h"
#include "dictionary_separate.h"
#include "query.h"

char *query_system_string(
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows )
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
		max_rows );

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

char *query_drop_down_row_key(
			LIST *foreign_key_list,
			int index )
{
	char key[ 512 ];
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
				SQL_DELIMITER ) ),
		index );

	free( tmp );
	return strdup( key );
}

char *query_data_list_string(
			DICTIONARY *query_dictionary,
			char *query_key )
{
	return dictionary_fetch( query_key, query_dictionary );
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
			LIST *foreign_key_list,
			LIST *folder_attribute_list,
			DICTIONARY *dictionary,
			int index )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	char *data_list_string;
	char *key = {0};

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
			/* ------------------------------------------ */
			/* Returns dictionary->hash_table->other_data */
			/* ------------------------------------------ */
			query_drop_down_row_data_list_string(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				( key = query_drop_down_row_key(
					foreign_key_list,
					index ) ),
				dictionary ) ) )
	{
		return (QUERY_DROP_DOWN_ROW *)0;
	}

	query_drop_down_row = query_drop_down_row_calloc();
	query_drop_down_row->key = key;
	query_drop_down_row->data_list_string = data_list_string;

	query_drop_down_row->data_string_list =
		query_drop_down_row_data_string_list(
			query_drop_down_row->
				data_list_string );

	query_drop_down_row->query_data_list =
		query_data_list(
			foreign_key_list,
			folder_attribute_list,
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

char *query_drop_down_row_data_where(
			char *folder_table_name,
			char *attribute_name,
			char *data )
{
	static char where[ 512 ];

	*where = '\0';

	if ( string_strcmp( data, NULL_OPERATOR ) == 0 )
	{
		sprintf(where,
			" (%s = 'null' or %s = '' or %s is null)",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	attribute_full_attribute_name(
		 		folder_table_name,
		 		attribute_name ),
		 	attribute_full_attribute_name(
		 		folder_table_name,
		 		attribute_name ),
		 	attribute_full_attribute_name(
		 		folder_table_name,
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
		 		folder_table_name,
		 		attribute_name ),
		 	attribute_full_attribute_name(
		 		folder_table_name,
		 		attribute_name ),
		 	attribute_full_attribute_name(
		 		folder_table_name,
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
		 		folder_table_name,
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

QUERY_DATE_CONVERT *query_date_convert_new( char *login_name )
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

QUERY_DATA *query_data_calloc( void )
{
	QUERY_DATA *query_data;

	if ( ! ( query_data = calloc( 1, sizeof( QUERY_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	return query_data;
}

QUERY_DATA *query_data_new(
			char *folder_name,
			char *attribute_name,
			char *datatype_name,
			char *data )
{
	QUERY_DATA *query_data;

	if ( !data || !*data ) return (QUERY_DATA *)0;

	query_data = query_data_calloc();

	query_data->folder_name = folder_name;
	query_data->attribute_name = attribute_name;
	query_data->datatype_name = datatype_name;
	query_data->data = data;

	query_data->escaped_replaced_data =
		security_sql_injection_escape(
			security_replace_special_characters(
				/* --------------------------- */
				/* Returns heap memory or data */
				/* --------------------------- */
				query_data_date_international(
					string_trim_number_characters(
						data,
						datatype_name ),
					datatype_name ) ) );

	return query_data;
}

char *query_data_date_international(
			char *data,
			char *datatype_name )
{
	char date_string[ 64 ];
	char time_string[ 64 ];
	char return_date_time[ 128 ];
	enum date_convert_format date_convert_format;
	DATE_CONVERT *date;

	if ( !data || !*data ) return data;

	if ( !attribute_is_date( datatype_name )
	&&   !attribute_is_time( datatype_name )
	&&   !attribute_is_date_time( datatype_name ) )
	{
		return data;
	}

	date_convert_format =
		date_convert_date_time_evaluate(
			data /* date_time_string */ );

	if ( date_convert_format == date_convert_unknown )
		return data;

	*time_string = '\0';

	column( date_string, 0, data );
	column( time_string, 1, data );

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
			SQL_DELIMITER );
}

LIST *query_data_list(	LIST *foreign_key_list,
			LIST *folder_attribute_list,
			LIST *data_string_list )
{
	char *foreign_key;
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *data_list;

	if ( !list_length( data_string_list ) ) return (LIST *)0;

	/* If is_empty or not_empty */
	/* ------------------------ */
	if ( list_length( data_string_list ) == 1
	&&   list_length( foreign_key_list ) > 1 )
	{
		char *data_string = list_first( data_string_list );
		data_list = list_new();

		list_rewind( foreign_key_list );

		do {
			foreign_key = list_get( foreign_key_list );

			if ( ! ( folder_attribute =
					folder_attribute_list_seek( 
						foreign_key,
						folder_attribute_list ) ) )
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
				data_list,
				query_data_new(
					folder_attribute->folder_name,
					foreign_key /* attribute_name */,
					folder_attribute->
						attribute->
						datatype_name,
					data_string ) );

		} while ( list_next( foreign_key_list ) );

		return data_list;
	}
	else
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

	data_list = list_new();

	list_rewind( data_string_list );
	list_rewind( foreign_key_list );

	do {
		foreign_key =
			list_get(
				foreign_key_list );

		if ( ! ( folder_attribute =
				folder_attribute_list_seek( 
					foreign_key,
					folder_attribute_list ) ) )
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
			data_list,
			query_data_new(
				folder_attribute->folder_name,
				foreign_key /* attribute_name */,
				folder_attribute->attribute->datatype_name,
				(char *)list_get( data_string_list ) ) );

		list_next( data_string_list );

	} while ( list_next( foreign_key_list ) );

	return data_list;
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
		prefix_direction_attribute +
		strlen( DICTIONARY_SEPARATE_SORT_BUTTON_PREFIX ),
		0 );

	if ( string_strncmp(
		direction_attribute,
		FORM_SORT_DESCEND_LABEL ) == 0 )
	{
		string_strcpy(
			attribute_list_string,
			direction_attribute +
			strlen( FORM_SORT_DESCEND_LABEL ),
			0 );

		descending_label = " desc";
	}
	else
	{
		string_strcpy(
			attribute_list_string,
			direction_attribute +
			strlen( FORM_SORT_ASCEND_LABEL ),
			0 );
	}

	return query_attribute_name_list_order(
			mto1_folder_name,
			list_string_list(
				attribute_list_string,
				SQL_DELIMITER )
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

QUERY_ISA_WIDGET *query_isa_widget_calloc( void )
{
	QUERY_ISA_WIDGET *query_isa_widget;

	if ( ! ( query_isa_widget = calloc( 1, sizeof( QUERY_ISA_WIDGET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return query_isa_widget;
}

QUERY_ISA_WIDGET *query_isa_widget_new(
			char *one2m_isa_folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_primary_list,
			char *login_name,
			char *security_entity_where )
{
	QUERY_ISA_WIDGET *query_isa_widget = query_isa_widget_calloc();

	if ( ! ( query_isa_widget->query_widget_select_list =
			query_widget_select_list(
				primary_key_list,
				folder_attribute_primary_list,
				/* --------------- */
				/* Always succeeds */
				/* --------------- */
				query_date_convert_new(
					login_name ) ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_widget_select_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display_delimited( primary_key_list, ',' ) );
		exit( 1 );
	}

	if ( ! ( query_isa_widget->query_select_list_string =
			query_select_list_string(
				query_isa_widget->query_widget_select_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_select_list_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_isa_widget->query_from_clause =
		query_from_clause(
			one2m_isa_folder_name );

	query_isa_widget->query_where_clause =
		query_where_clause(
			security_entity_where );

	query_isa_widget->query_order_clause =
		query_order_clause(
			query_isa_widget->query_select_list_string,
			(DICTIONARY *)0 /* sort_dictionary */ );

	return query_isa_widget;
}

QUERY_SELECT *query_select_calloc( void )
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

	return query_select;
}

QUERY_SELECT *query_select_new(
			char *folder_name,
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			QUERY_DATE_CONVERT *query_date_convert )
{
	QUERY_SELECT *query_select = query_select_calloc();

	if ( attribute_is_date
	||   attribute_is_date_time )
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

		query_select->column_string =
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			query_select_column_string(
				folder_name,
				attribute_name,
				attribute_is_date,
				attribute_is_date_time,
				query_date_convert->
					date_convert_format );
	}
	else
	{
		query_select->column_string =
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			query_select_column_string(
				folder_name,
				attribute_name,
				0 /* not attribute_is_date */,
				0 /* not attribute_is_date_time */,
				date_convert_unknown );
	}

	return query_select;
}

char *query_select_column_string(
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

LIST *query_edit_table_select_list(
			LIST *folder_attribute_append_isa_list,
			LIST *ignore_select_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			int relation_mto1_isa_length,
			QUERY_DATE_CONVERT *query_date_convert,
			ROW_SECURITY_ROLE *row_security_role )
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

	if ( row_security_role )
	{
		list_set(
			select_list,
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			query_select_new(
				(char *)0 /* folder_name */,
				row_security_role->attribute_not_null,
				0 /* not attribute_is_date */,
				0 /* not attribute_is_date_time */,
				(QUERY_DATE_CONVERT *)0 ) );
	}

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

char *query_select_list_string( LIST *select_list )
{
	char list_string[ 2048 ];
	char *ptr = list_string;
	QUERY_SELECT *query_select;

	if ( !list_rewind( select_list ) ) return (char *)0;

	*ptr = '\0';

	do {
		query_select = list_get( select_list );

		if ( !*ptr ) ptr += sprintf( ptr, "," );

		if ( !query_select->column_string )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: column_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}


		ptr += sprintf( ptr, "%s", query_select->column_string );

	} while ( list_next( select_list ) );

	return strdup( list_string );
}

LIST *query_drop_down_list(
			char *many_folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *dictionary )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	RELATION *relation_mto1;
	QUERY_DROP_DOWN *query_drop_down;
	LIST *drop_down_list = {0};
	int highest_index;

	if ( !list_rewind( folder_attribute_list ) ) return (LIST *)0;

	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

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

		highest_index =
			dictionary_string_list_highest_index(
				relation_mto1->foreign_key_list,
				dictionary );

		if ( highest_index < 0 ) continue;

		query_drop_down =
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			query_drop_down_new(
				many_folder_name,
				folder_attribute_list,
				relation_mto1->foreign_key_list,
				dictionary,
				highest_index );

		if ( !list_length( query_drop_down->row_list ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: row_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !drop_down_list ) drop_down_list = list_new();

		list_set( drop_down_list, query_drop_down );

	} while ( list_next( folder_attribute_list ) );

	return drop_down_list;
}

QUERY_DROP_DOWN *query_drop_down_new(
			char *many_folder_name,
			LIST *folder_attribute_list,
			LIST *foreign_key_list,
			DICTIONARY *dictionary,
			int highest_index )
{
	QUERY_DROP_DOWN *query_drop_down;

	query_drop_down = query_drop_down_calloc();
	query_drop_down->many_folder_name = many_folder_name;

	query_drop_down->row_list =
		query_drop_down_row_list(
			foreign_key_list,
			folder_attribute_list,
			dictionary,
			highest_index );

	if ( !list_length( query_drop_down->row_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_drop_down_row_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
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
			DICTIONARY *dictionary )
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
		"Warning in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (LIST *)0;
	}

	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( ! ( operator_name =
				query_relation_operator_name(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					query_relation_operator_key(
						folder_attribute->
							attribute_name,
						QUERY_STARTING_LABEL ),
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
			query_attribute =
				query_attribute_new(
					folder_attribute->attribute_name,
					folder_attribute->folder_name,
					folder_attribute->
						attribute->
						datatype_name,
					relational_operator,
					(QUERY_DATA *)0 /* from_data */,
					(QUERY_DATA *)0 /* to_data */,
					folder_attribute->primary_key_index );

			if ( !query_attribute_list )
			{
				query_attribute_list = list_new();
			}

			list_set(
				query_attribute_list,
				query_attribute );

			continue;
		}

		from_data = (QUERY_DATA *)0;
		to_data = (QUERY_DATA *)0;

		if ( ! ( from_data =
				query_data_new(
					folder_attribute->folder_name,
					folder_attribute->attribute_name,
					folder_attribute->
						attribute->
						datatype_name,
					dictionary_starting_label_get(
						folder_attribute->
						     attribute_name,
						QUERY_FROM_STARTING_LABEL,
						dictionary ) ) ) )
		{
			continue;
		}

		to_data =
			query_data_new(
				folder_attribute->folder_name,
				folder_attribute->attribute_name,
				folder_attribute->
					attribute->
					datatype_name,
				dictionary_starting_label_get(
					folder_attribute->
						attribute_name,
					QUERY_TO_STARTING_LABEL,
					dictionary ) );

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
			char *application_name,
			LIST *query_drop_down_list,
			int relation_mto1_isa_list_length )
{
	char where[ STRING_TWO_MEG ];
	char *ptr = where;
	QUERY_DROP_DOWN *query_drop_down;
	boolean first_time = 1;
	char *tmp;

	if ( !list_rewind( query_drop_down_list ) ) return (char *)0;

	do {
		query_drop_down = list_get( query_drop_down_list );

		if ( !list_rewind( query_drop_down->row_list ) )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: row_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );

			continue;
		}

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " and" );
		}

		if ( ( tmp =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_drop_down_row_list_where(
				folder_table_name(
					application_name,
					query_drop_down->many_folder_name ),
				query_drop_down->row_list,
				relation_mto1_isa_list_length ) ) )
		{
			ptr += sprintf( ptr, "%s", tmp );
			free( tmp );
		}

	} while( list_next( query_drop_down_list ) );

	return strdup( where );
}

char *query_drop_down_row_list_where(
			char *folder_table_name,
			LIST *query_drop_down_row_list,
			int relation_mto1_isa_list_length )
{
	char where[ STRING_ONE_MEG ];
	char *ptr = where;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	boolean first_time = 1;
	char *tmp;

	if ( !list_rewind( query_drop_down_row_list ) ) return (char *)0;

	if ( list_length( query_drop_down_row_list ) > 1 )
	{
		ptr += sprintf( ptr, "(" );
	}

	do {
		query_drop_down_row =
			list_get(
				query_drop_down_row_list );

		if ( !list_length( query_drop_down_row->query_data_list ) )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: query_data_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " or" );
		}

		if ( ( tmp =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_drop_down_row_where(
				folder_table_name,
				query_drop_down_row->query_data_list,
				relation_mto1_isa_list_length ) ) )
		{
			ptr += sprintf( ptr, "%s", tmp );
			free( tmp );
		}

	} while( list_next( query_drop_down_row_list ) );

	if ( list_length( query_drop_down_row_list ) > 1 )
	{
		ptr += sprintf( ptr, ")" );
	}

	return strdup( where );
}

char *query_drop_down_row_where(
			char *folder_table_name,
			LIST *query_data_list,
			int relation_mto1_isa_list_length )
{
	char where[ 1024 ];
	char *ptr = where;
	QUERY_DATA *query_data;
	boolean first_time = 1;

	if ( !list_rewind( query_data_list ) ) return (char *)0;

	if ( list_length( query_data_list ) > 1 )
	{
		ptr += sprintf( ptr, "(" );
	}

	do {
		query_data = list_get( query_data_list );

		if ( first_time )
		{
			first_time = 0;
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
			query_drop_down_row_data_where(
				(relation_mto1_isa_list_length)
					? folder_table_name
					: (char *)0,
				query_data->attribute_name,
				query_data->escaped_replaced_data ) );

	} while( list_next( query_data_list ) );

	if ( list_length( query_data_list ) > 1 )
	{
		ptr += sprintf( ptr, ")" );
	}

	return strdup( where );
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

LIST *query_drop_down_row_list(
			LIST *foreign_key_list,
			LIST *folder_attribute_list,
			DICTIONARY *dictionary,
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
				foreign_key_list,
				folder_attribute_list,
				dictionary,
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

LIST *query_primary_delimited_list(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *foreign_key_list,
			LIST *foreign_data_list )
{
	char system_string[ STRING_INPUT_BUFFER ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		list_display_delimited(
			primary_key_list, ',' ),
		folder_table_name,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_data_where(
			(char *)0
				/* folder_name */,
			foreign_key_list
				/* where_attribute_name_list */,
			foreign_data_list
				/* where_attribute_data_list */,
			(LIST *)0
				/* folder_attribute_list */ ) );

	return list_pipe_fetch( system_string );
}

char *query_data_where(
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
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
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

#ifdef NOT_DEFINED
LIST *query_primary_delimited_fetch_list(
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_primary_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
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

		return (LIST *)0;
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

		return (LIST *)0;
	}

	query->from_clause = folder_name;

	query->query_widget_where =
		query_widget_where_new(
			folder_attribute_primary_list,
			relation_mto1_non_isa_list,
			security_entity_where,
			drillthru_dictionary );

	query->where_clause = query->query_widget_where->where_clause;

	query->order_clause = query->select_clause;

	return
	query_delimited_list(
		query->select_clause,
		query->from_clause,
		query->where_clause,
		query->order_clause,
		0 /* max_rows */ );
}
#endif

QUERY_EDIT_TABLE *query_edit_table_calloc( void )
{
	QUERY_EDIT_TABLE *query_edit_table;

	if ( ! ( query_edit_table = calloc( 1, sizeof( QUERY_EDIT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_edit_table;
}

QUERY_EDIT_TABLE *query_edit_table_new(
			char *application_name,
			char *folder_name,
			char *login_name,
			char *security_entity_where,
			LIST *relation_join_one2m_list,
			LIST *ignore_select_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			LIST *folder_attribute_append_isa_list,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			ROW_SECURITY_ROLE *row_security_role )
{
	QUERY_EDIT_TABLE *query_edit_table;

	if ( !list_length( folder_attribute_append_isa_list ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_attribute_append_isa_list empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (QUERY_EDIT_TABLE *)0;
	}

	query_edit_table = query_edit_table_calloc();

	query_edit_table->select_list =
		query_edit_table_select_list(
			folder_attribute_append_isa_list,
			ignore_select_attribute_name_list,
			exclude_lookup_attribute_name_list,
			list_length( relation_mto1_isa_list ),
			query_date_convert_new( login_name ),
			row_security_role );

	if ( !list_length( query_edit_table->select_list ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: query_edit_table_select_list() returned  empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (QUERY_EDIT_TABLE )0;
	}

	query_edit_table->query_select_string =
		query_select_string(
			query_edit_table->select_list );

	return query_edit_table;
}

QUERY_WIDGET_WHERE *query_widget_where_calloc( void )
{
	QUERY_WIDGET_WHERE *query_widget_where;

	if ( ! ( query_widget_where =
			calloc(
				1,
				sizeof( QUERY_WIDGET_WHERE ) ) ) )
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
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary )
{
	QUERY_WIDGET_WHERE *where =
		query_widget_where_calloc();

	where->query_drop_down_list =
		query_drop_down_list(
			(char *)0 /* application_name */,
			folder_attribute_list,
			relation_mto1_non_isa_list,
			drillthru_dictionary );

	if ( list_length( where->query_drop_down_list ) )
	{
		where->query_drop_down_list_where =
			query_drop_down_list_where(
				(char *)0 /* application_name */,
				where->query_drop_down_list,
				0 /* relation_mto1_isa_list_length */ );
	}

	where->query_attribute_list =
		query_attribute_list(
			folder_attribute_list,
			drillthru_dictionary );

	if ( list_length( where->query_attribute_list ) )
	{
		where->query_attribute_list_where =
			query_attribute_list_where(
				where->query_attribute_list );
	}

	where->query_widget_where =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		query_widget_where(
			where->query_drop_down_list_where,
			where->query_attribute_list_where,
			security_entity_where );

	where->clause =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		query_widget_where_clause(
			where->query_widget_where );

	return query_widget_where;
}

char *query_widget_where(
			char *query_drop_down_where,
			char *query_attribute_list_where,
			char *security_entity_where )
{
	char where[ STRING_TWO_MEG ];
	char *ptr = where;

	strcpy( where, "1 = 1" );

	if ( query_drop_down_where && *query_drop_down_where )
	{
		ptr += sprintf(
			ptr,
			"%s",
			query_drop_down_where );
	}

	if ( query_attribute_list_where && *query_attribute_list_where )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_attribute_list_where );
	}

	if ( security_entity_where && *security_entity_where )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			security_entity_where );
	}

	return strdup( where );
}

char *query_widget_where_clause(
			char *query_widget_where )
{
	char where[ STRING_TWO_MEG ];

	if ( !query_widget_where || !*query_widget_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: query_widget_where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( where, "where %s", query_widget_where );

	return strdup( where );
}

char *query_widget_where_string(
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary )
{
	QUERY_WIDGET_WHERE *query_widget_where =
		query_widget_where_new(
			folder_attribute_list,
			relation_mto1_non_isa_list,
			security_entity_where,
			drillthru_dictionary );

	if ( !query_widget_where )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_widget_where_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_widget_where->query_widget_where;
}

QUERY_WIDGET *query_widget_calloc( void )
{
	QUERY_WIDGET *query_widget;

	if ( ! ( query_widget = calloc( 1, sizeof( QUERY_WIDGET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_widget;
}

QUERY_WIDGET *query_widget_new(
			char *widget_folder_name,
			char *login_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary )
{
	QUERY_WIDGET *query_widget = query_widget_calloc();

	if ( ! ( query_widget->select_list =
			query_widget_select_list(
				folder_attribute_primary_key_list(
					folder_attribute_list ),
				folder_attribute_list,
				/* --------------- */
				/* Always succeeds */
				/* --------------- */
				query_date_convert_new(
					login_name ) ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_widget_select_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( query_widget->query_select_list_string =
			query_select_list_string(
				query_widget->select_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_select_list_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( query_widget->query_select_clause =
			query_select_clause(
				query_widget->select_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_select_clause() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_widget->query_from_clause =
		query_from_clause(
			widget_folder_name );

	if ( ! ( query_widget->where =
			query_widget_where_new(
				folder_attribute_list,
				relation_mto1_non_isa_list,
				security_entity_where,
				drillthru_dictionary ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_widget_where_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_widget->query_order_clause =
		query_order_clause(
			query_widget->query_select_list_string,
			(DICTIONARY *)0 /* sort_dictionary */ );

	return query_widget;
}

LIST *query_widget_select_list(
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

		if ( !folder_attribute->attribute )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
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

char *query_where_clause( char *where_string )
{
	char *where_clause;

	if ( !where_string || !*where_string )
	{
		return strdup( "where 1 = 1" );
	}
	else
	{
		if ( ! ( where_clause =
				calloc( strlen( where_string ) + 7, 
				sizeof( char ) ) ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		sprintf(where_clause,
			"where %s",
			where_string );

		return where_clause;
	}
}

char *query_from_clause( char *folder_name )
{
	char from_clause[ 128 ];

	if ( !folder_name || !*folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(from_clause,
		"from %s",
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		folder_table_name(
			environment_application_name(),
			folder_name ) );

	return strdup( from_clause );
}

char *query_edit_table_from_clause(
			char *folder_name,
			LIST *relation_mto1_isa_list,
			ROW_SECURITY_ROLE *row_security_role )
{
	char from_clause[ 1024 ];
	*ptr = from_clause;
	LIST *mto1_folder_name_list;

	if ( !folder_name || !*folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"from %s",
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		folder_table_name(
			environment_application_name(),
			folder_name ) );

	mto1_folder_name_list =
		relation_mto1_folder_name_list(
			relation_mto1_isa_list );

	if ( list_rewind( mto1_folder_name_list ) )
	{
		do {
			ptr += sprintf(
				ptr,
				",%s",
				list_get( mto1_folder_name_list ) );
		} while ( list_next( mto1_folder_name_list ) );
	}

	if ( row_security_role )
	{
		if ( !row_security_role->row_security_role_update )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: row_security_role_update is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		ptr += sprintf(
			ptr,
			",%s",
			row_security_role->
				row_security_role_update->
					check_folder_name );
	}

	return strdup( from_clause );
}

char *query_order_clause(
			char *query_select_list_string,
			DICTIONARY *sort_dictionary )
{
	char order_clause[ 1024 ];

	if ( dictionary_length( sort_dictionary ) )
	{
		return query_order_key_list_clause(
			dictionary_key_list(
				sort_dictionary ) );
	}
	else
	{
		if ( !query_select_list_string )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: query_select_list_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		sprintf(order_clause,
			"order by %s",
			query_select_list_string );
	}

	return strdup( order_clause );
}

char *query_order_key_list_clause( LIST *key_list )
{
	char order_clause[ 1024 ];
	char *key;
	int longest_key_length = 0;
	char *longest_key = (char *)0;
	char *found_key;

	if ( !list_rewind( key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_rewind() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		key = list_get( key_list );

		if ( strlen( key ) > longest_key_length )
		{
			longest_key = key;
			longest_key_length = strlen( key );
		}
	} while ( list_next( key_list ) );

	if ( !longest_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: longest_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	found_key =
		list_string_search(
			longest_key,
			key_list );

	if ( string_strncmp( found_key, FORM_SORT_ASSEND_LABEL ) == 0 )
	{
		sprintf(order_clause,
			"order by %s",
			string_search_replace_character(
				found_key +
				strlen( FORM_SORT_ASSEND_LABEL ),
				SQL_DELIMITER,
				',' ) );
	}
	else
	{
		sprintf(order_clause,
			"order by %s desc",
			string_search_replace_character(
				found_key +
				strlen( FORM_SORT_DESCEND_LABEL ),
				SQL_DELIMITER,
				',' ) );
	}
	return strdup( order_clause );
}

QUERY_EDIT_TABLE_WHERE *query_edit_table_where_calloc( void )
{
	QUERY_EDIT_TABLE_WHERE *query_edit_table_where;

	if ( ! ( query_edit_table_where =
			calloc( 1, sizeof( QUERY_EDIT_TABLE_WHERE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_edit_table_where;
}

QUERY_EDIT_TABLE_WHERE *query_edit_table_where_new(
			char *application_name,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			int relation_mto1_isa_list_length,
			PROMPT_RECURSIVE *prompt_recursive,
			ROW_SECURITY_ROLE *row_security_role,
			char *security_entity_where,
			DICTIONARY *query_dictionary )
{
	QUERY_EDIT_TABLE_WHERE *where = query_edit_table_where_calloc();

	where->query_drop_down_list =
		query_drop_down_list(
			folder_name /* many_folder_name */,
			folder_attribute_append_isa_list,
			relation_mto1_non_isa_list,
			query_dictionary );

	if ( list_length( where->query_drop_down_list ) )
	{
		where->query_drop_down_list_where =
			query_drop_down_list_where(
				application_name,
				where->query_drop_down_list,
				relation_mto1_isa_list_length );
	}

	if ( prompt_recursive )
	{
		where->prompt_recursive_drop_down_list =
			prompt_recursive_drop_down_list(
				folder_name /* many_folder_name */,
				prompt_recursive->
					prompt_recursive_folder_list );

		if ( list_length(
			where->
				prompt_recursive_drop_down_list ) )
		{
			where->prompt_recursive_drop_down_list_where =
				prompt_recursive_drop_down_list_where(
					application_name,
					where->prompt_recursive_drop_down_list,
					relation_mto1_isa_list_length );
		}
	}

	where->query_attribute_list =
		query_attribute_list(
			folder_attribute_append_isa_list,
			query_dictionary );

	if ( list_length( where->query_attribute_list ) )
	{
		where->query_attribute_list_where =
			query_attribute_list_where(
				application_name,
				where->query_attribute_list,
				relation_mto1_isa_list_length );
	}

	where->query_join_where =
		query_join_where(
			folder_name,
			folder_attribute_primary_key_list(
				folder_attribute_append_isa_list ),
			relation_mto1_isa_list,
			row_security_role );

	where->query_edit_table_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_edit_table_where(
			where->query_edit_table_drop_down_list_where,
			where->prompt_recursive_drop_down_list_where,
			where->query_attribute_list_where,
			where->query_join_where,
			security_entity_where );

	return where;
}

char *query_edit_table_where(
			char *query_edit_table_drop_down_list_where,
			char *prompt_recursive_drop_down_list_where,
			char *query_attribute_list_where,
			char *query_join_where,
			char *security_entity_where )
{
	char where[ STRING_FOUR_MEG ];
	char *ptr = where;

	if ( query_edit_table_drop_down_list_where )
	{
		ptr += sprintf(
			ptr,
			"%s",
			query_edit_table_drop_down_list_where );
	}

	if ( prompt_recursive_drop_down_list_where )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			prompt_recursive_drop_down_list_where );
	}

	if ( query_attribute_list_where )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_attribute_list_where );
	}

	if ( query_join_where )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_join_where );
	}

	if ( security_entity_where )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			security_entity_where );
	}

	if ( ptr == where )
		return (char *)0;
	else
		return strdup( where );
}

QUERY_RELATION *query_relation_calloc( void )
{
	QUERY_RELATION *query_relation;

	if ( ! ( query_relation = calloc( 1, sizeof( QUERY_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_relation;
}

QUERY_RELATION *query_relation_new(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary )
{
	QUERY_RELATION *query_relation;
	char *key;
	char *yes_no_key = {0};
	char *fetch;

	if ( ! ( key =
			query_relation_key(
				attribute_name,
				QUERY_RELATION_STARTING_LABEL ) ) )
	{
		yes_no_key = query_relation_yes_no_key( attribute_name );
	}

	if ( ! ( fetch =
			query_relation_fetch(
				key,
				yes_no_key,
				dictionary ) ) )
	{
		return (QUERY_RELATION *)0;
	}

	query_relation = query_relation_calloc();
	query_relation->key = key;
	query_relation->yes_no_key = yes_no_key;
	query_relation->fetch = fetch;

	query_relation->query_relation =
		query_relation(
			fetch,
			datatype_name );

	return query_relation;
}

char *query_relation_key(
			char *attribute_name,
			char *query_relation_starting_label )
{
	static char key[ 512 ];

	sprintf(key,
		"%s%s",
		query_relation_operator_starting_label,
		attribute_name );

	return key;
}

char *query_relation_yes_no_key(
			char *attribute_name )
{
	if ( attribute_is_yes_no( attribute_name ) )
	{
		return QUERY_EQUAL_OPERATOR;
	}
	else
	{
		return (char *)0;
	}
}

char *query_relation_fetch(
			char *query_relation_key,
			char *query_relation_yes_no_key,
			DICTIONARY *dictionary )
{
	if ( query_relation_key )
	{
		return dictionary_get( query_relation_key, dictionary );
	}
	else
	if ( query_relation_yes_no_key )
	{
		return dictionary_get( query_relation_yes_no_key, dictionary );
	}

	return (char *)0;
}

enum query_relation query_relation_enum(
			char *fetch,
			char *datatype_name )
{
	enum query_relation query_relation;

	if ( !query_relation_fetch )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: query_relation_fetch is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( fetch, QUERY_EQUAL ) == 0 )
		equal_relation = equal;
	else
	if ( strcmp( fetch, QUERY_NOT_EQUAL ) == 0 )
		query_relation = not_equal;
	else
	if ( strcmp( fetch, QUERY_NOT_EQUAL_OR_NULL ) == 0 )
		query_relation = not_equal_or_null;
	else
	if ( strcmp( fetch, QUERY_LESS_THAN ) == 0 )
		query_relation = less_than;
	else
	if ( strcmp( fetch, QUERY_LESS_THAN_EQUAL_TO ) == 0 )
		query_relation = less_than_equal_to;
	else
	if ( strcmp( fetch, QUERY_GREATER_THAN ) == 0 )
		query_relation = greater_than;
	else
	if ( strcmp( fetch, QUERY_GREATER_THAN_EQUAL_TO ) == 0 )
		query_relation = greater_than_equal_to;
	else
	if ( strcmp( fetch, QUERY_BETWEEN ) == 0 )
		query_relation = between;
	else
	if ( strcmp( fetch, QUERY_BEGINS ) == 0 )
		query_relation = begins;
	else
	if ( strcmp( fetch, QUERY_CONTAINS ) == 0 )
		query_relation = contains;
	else
	if ( strcmp( fetch, QUERY_NOT_CONTAINS ) == 0 )
		query_relation = not_contains;
	else
	if ( strcmp( fetch, QUERY_OR ) == 0 )
		query_relation = query_or;
	else
	if ( strcmp( fetch, QUERY_NULL ) == 0 )
		query_relation = is_null;
	else
	if ( strcmp( fetch, QUERY_NOT_NULL ) == 0 )
		query_relation = not_null;

	if ( query_relation == equals
	&&   attribute_is_date_time( datatype_name ) )
	{
		query_relation = begins;
	}

	return query_relation;
}

QUERY_BETWEEN_DATA *query_between_data_calloc( void )
{
	QUERY_BETWEEN_DATA *query_between_data;

	if ( ! ( query_between_data =
			calloc( 1, sizeof( QUERY_BETWEEN_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_between_data;
}

QUERY_BETWEEN_DATA *query_between_data_new(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary,
			char *folder_table_name,
			int relation_mto1_isa_list_length )
{
	QUERY_BETWEEN_DATA *query_between_data;
	char *date_attribute_base;
	char *time_attribute_name = {0};
	QUERY_DATA *from_date = {0};
	QUERY_DATA *from_time = {0};

	if ( !attribute_name || !datatype_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( dictionary ) ) return (QUERY_BETWEEN_DATA *)0;

	if ( ( date_attribute_base =
		query_between_data_date_attribute_base(
			attribute_name,
			attribute_is_date( datatype_name ) ||
			attribute_is_time( datatype_name ) ) ) )
	{
		time_attribute_name =
			query_between_data_time_attribute_name(
				date_attribute_base );

		if ( time_attribute_name )
		{
			from_date =
				query_between_data_from_date(
					attribute_name,
					datatype_name,
					dictionary );

			from_time =
				query_between_data_from_time(
					time_attribute_name,
					dictionary );
		}
	}

	if ( attribute_is_time( datatype_name )
	&&   from_date
	&&   from_time )
	{
		/* Ignore the time attribute */
		/* ------------------------- */
		return (QUERY_BETWEEN_DATA *)0;
	}

	query_between_data = query_between_data_calloc();
	query_between_data->date_attribute_base = date_attribute_base;
	query_between_data->time_attribute_name = time_attribute_name;
	query_between_data->from_date = from_date;
	query_between_data->from_time = from_time;

	if ( attribute_is_date( datatype_name ) )
	{
		query_between_data->to_date =
			query_between_data_to_date(
				attribute_name,
				datatype_name,
				dictionary );
	}

	return query_between_data;
}

char *query_between_data_date_attribute_base(
			char *attribute_name,
			boolean is_date_or_time )
{
	char attribute_base[ 128 ];
	int offset;

	if ( !is_date_or_time ) return (char *)0;

	offset =
		string_instr(
			"_date" /* substr */,
			attribute_name /* string */,
			1 /* occurrence */ );

	if ( offset < 0 ) return (char *)0;

	strcpy( attribute_base, attribute_name );
	*( attribute_base + offset ) = '\0';

	return strdup( attribute_base );
}

char *query_between_data_time_attribute_name(
			char *attribute_base )
{
	char time_attribute_name[ 128 ];

	if ( !attribute_base ) return (char *)0;

	sprintf( time_attribute_name, "%s_time", attribute_base );

	return strdup( time_attribute_name );
}

QUERY_DATA *query_between_data_from_date(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary )
{
	if ( attribute_is_date( datatype_name ) )
	{
		return
		query_data_fetch(
			(char *)0 /* folder_name */,
			attribute_name,
			datatype_name,
			dictionary );
	}

	return (QUERY_DATA *)0;
}

QUERY_DATA *query_between_data_from_time(
			char *time_attribute_name,
			DICTIONARY *dictionary )
{
	if ( time_attribute_name )
	{
		return
		query_data_fetch(
			(char *)0 /* folder_name */,
			time_attribute_name,
			(char *)0 /* datatype_name */,
			dictionary );
	}

	return (QUERY_DATA *)0;
}

QUERY_DATA *query_between_data_to_date(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary )
{
	char to_attribute_name[ 128 ];

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( dictionary ) ) return (QUERY_DATA *)0;
	if ( !attribute_is_date( datatype_name ) ) return (QUERY_DATA *)0;

	sprintf(to_attribute_name,
		"to_%s",
		attribute_name );

	return
	query_data_fetch(
		(char *)0 /* folder_name */,
		strdup( to_attribute_name ),
		datatype_name,
		dictionary );
}

QUERY_DATA *query_between_data_to_time(
			char *time_attribute_name,
			DICTIONARY *dictionary )
{
	char to_attribute_name[ 128 ];

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !time_attribute_name ) return (QUERY_DATA *)0;
	if ( !dictionary_length( dictionary ) ) return (QUERY_DATA *)0;

	sprintf(to_attribute_name,
		"to_%s",
		time_attribute_name );

	return
	query_data_fetch(
		(char *)0 /* folder_name */,
		strdup( to_attribute_name ),
		(char *)0 /* datatype_name */,
		dictionary );
}

QUERY_DATA *query_between_data_from(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary )
{
	return
	query_data_fetch(
		(char *)0 /* folder_name */,
		attribute_name,
		datatype_name,
		dictionary );
}

QUERY_DATA *query_between_data_to(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary )
{
	char to_attribute_name[ 128 ];

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( dictionary ) ) return (QUERY_DATA *)0;

	sprintf(to_attribute_name,
		"to_%s",
		attribute_name );

	return
	query_data_fetch(
		(char *)0 /* folder_name */,
		strdup( to_attribute_name ),
		datatype_name,
		dictionary );
}

char *query_between_data_date_time_where(
			QUERY_DATA *from_date,
			QUERY_DATA *from_time,
			QUERY_DATA *to_date,
			QUERY_DATA *to_time,
			char *folder_table_name,
			int relation_mto1_isa_list_length )
{
	static char where[ 1024 ];
	char *where_ptr = where;
	char *start_end_time;
	char *finish_begin_time;
	char *begin_date;
	char *end_date;
	char *begin_time;
	char *end_time;
	char *date_attribute_name;
	char *time_attribute_name;

	if ( !from_date || !from_time || !to_date || !to_time )
	{
		return (char *)0;
	}

	if ( !relation_mto1_isa_list_length )
	{
		folder_table_name = (char *)0;
	}

	begin_date = from_date->escaped_replaced_data;
	end_date = to_date->escaped_replaced_data;
	begin_time = from_time->escaped_replaced_data;
	end_time = to_time->escaped_replaced_data;
	date_attribute_name = from_date->attribute_name;
	time_attribute_name = from_time->attribute_name;

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

	return where;
}

char *query_between_data_non_date_time_where(
			QUERY_DATA *data_from,
			QUERY_DATA *data_to,
			char *datatype_name,
			char *folder_table_name,
			int relation_mto1_isa_list_length )
{
	static char where[ 1024 ];

	if ( !data_from || !data_to ) return (char *)0;

	if ( !datatype_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: datatype_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( relation_mto1_isa_list_length )
	{
		if ( !folder_name_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_table_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( attribute_is_number( datatype_name ) )
		{
			sprintf(where,
				"where %s.%s between %s and %s",
				folder_table_name,
				data_from->attribute_name,
				data_from->escaped_replaced_data,
				data_to->escaped_replaced_data );
		}
		else
		{
			sprintf(where,
				"where %s.%s between '%s' and '%s'",
				folder_table_name,
				data_from->attribute_name,
				data_from->escaped_replaced_data,
				data_to->escaped_replaced_data );
		}
	}
	else
	{
		if ( attribute_is_number( datatype_name ) )
		{
			sprintf(where,
				"where %s between %s and %s",
				data_from->attribute_name,
				data_from->escaped_replaced_data,
				data_to->escaped_replaced_data );
		}
		else
		{
			sprintf(where,
				"where %s between '%s' and '%s'",
				data_from->attribute_name,
				data_from->escaped_replaced_data,
				data_to->escaped_replaced_data );
		}
	}

	return where;
}

char *query_between_data_where(
			char *query_between_data_date_time_where,
			char *query_between_data_non_date_time_where )
{
	char where[ 1024 ];

	if ( !query_between_data_date_time_where
	&&   !query_between_data_non_date_time_where )
	{
		return (char *)0;
	}

	if ( query_between_data_date_time_where )
		strcpy( where, query_between_data_date_time_where );
	else
		strcpy( where, query_between_data_non_date_time_where );

	return strdup( where );
}

QUERY_DATA *query_data_fetch(
			char *folder_name,
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary )
{
	char *data;

	if ( ( data = dictionary_get( attribute_name, dictionary ) ) )
	{
		return
		query_data_new(
			folder_name,
			attribute_name,
			datatype_name,
			data );
	}

	return (QUERY_DATA *)0;
}

