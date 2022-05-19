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
#include "relation.h"
#include "dictionary_separate.h"
#include "element.h"
#include "relation.h"
#include "relation_join.h"
#include "appaserver.h"
#include "query.h"

char *query_system_string(
			char *select_string,
			char *from_string,
			char *where_string,
			char *order_string,
			int max_rows )
{
	char system_string[ STRING_FOUR_MEG ];

	if ( !select_string || !*select_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: select_string is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !from_string || !*from_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: from_string is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select	application=%s			"
		"	select=\"%s\"			"
		"	folder=%s			"
		"	where=\"%s\"			"
		"	order=\"%s\"			"
		"	maxrows=\"%d\"			",
		environment_application_name(),
		select_string,
		from_string,
		(where_string) ? where_string : "1 = 1",
		(order_string) ? order_string : "",
		max_rows );

	return strdup( system_string );
}

LIST *query_table_edit_row_dictionary_list(
			char *query_system_string,
			LIST *query_select_name_list,
			LIST *one_folder_primary_key_list,
			char *application_name,
			LIST *relation_join_one2m_list,
			char *one_folder_name )
{
	char buffer[ STRING_128K ];
	char data[ STRING_64K ];
	LIST *list = list_new();
	DICTIONARY *row_dictionary;
	int i;
	FILE *pipe;
	RELATION_JOIN *relation_join;

	if ( !list_length( query_select_name_list ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d query_select_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (LIST *)0;
	}

	pipe = popen( query_system_string, "r" );

	while( string_input( buffer, pipe, STRING_128K ) )
	{
		i = 0;

		list_set(
			list,
			( row_dictionary = dictionary_small() ) );

		list_rewind( query_select_name_list );

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

			dictionary_set(
				row_dictionary, 
				list_get( query_select_name_list ),
				strdup( data ) );

		} while ( list_next( query_select_name_list ) );

		if ( list_length( relation_join_one2m_list ) )
		{
			relation_join =
				relation_join_new(
					row_dictionary /* in/out */,
					application_name,
					relation_join_one2m_list,
					one_folder_name,
					one_folder_primary_key_list );

			relation_join_free( relation_join );
		}
	}

	pclose( pipe );
	return list;
}

LIST *query_delimited_list(
			char *select_string,
			char *from_string,
			char *where_string,
			char *order_string )
{
	return
	list_pipe_fetch(
		query_system_string(
			select_string,
			from_string,
			where_string,
			order_string,
			0 /* max_rows */ ) );
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
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
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
			SQL_DELIMITER,
			query_drop_down_row->
				data_list_string );

	query_drop_down_row->query_data_list =
		query_data_list(
			foreign_key_list,
			folder_attribute_list,
			query_drop_down_row->data_string_list );

	return query_drop_down_row;
}

QUERY_ATTRIBUTE *query_attribute_calloc( void )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( ! ( query_attribute = calloc( 1, sizeof( QUERY_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_attribute;
}

QUERY_ATTRIBUTE *query_attribute_new(
			char *application_name,
			char *attribute_name,
			char *folder_name,
			char *datatype_name,
			DICTIONARY *dictionary,
			int relation_mto1_isa_list_length )
{
	QUERY_ATTRIBUTE *query_attribute;
	QUERY_RELATION *query_relation;

	if ( ! ( query_relation =
			query_relation_new(
				attribute_name,
				datatype_name,
				dictionary ) ) )
	{
		return (QUERY_ATTRIBUTE *)0;
	}

	query_attribute = query_attribute_calloc();
	query_attribute->application_name = application_name;
	query_attribute->folder_name = folder_name;

	query_attribute->relation_mto1_isa_list_length =
		relation_mto1_isa_list_length;

	if ( query_relation->query_relation_enum == between )
	{
		query_attribute->query_between_data =
			query_between_data_new(
				attribute_name,
				datatype_name,
				dictionary,
				application_name,
				folder_name,
				relation_mto1_isa_list_length );
	}
	else
	{
		query_attribute->query_data =
			query_data_fetch(
				folder_name,
				attribute_name,
				datatype_name,
				dictionary );
	}

	return query_attribute;
}

#ifdef NOT_DEFINED
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
#endif

char *query_drop_down_row_data_where(
			char *folder_table_name,
			char *attribute_name,
			char *data )
{
	static char where[ 512 ];

	*where = '\0';

	if ( string_strcmp( data, ELEMENT_NULL_OPERATOR ) == 0 )
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
	if ( string_strcmp( data, ELEMENT_NOT_NULL_OPERATOR ) == 0 )
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

char *query_or_sequence_where(
			LIST *attribute_name_list,
			LIST *data_list_list )
{
	char *attribute_name;
	LIST *data_list;
	char *data;
	char where[ STRING_WHERE_BUFFER ];
	char *where_ptr = where;
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

	where_ptr += sprintf( where_ptr, "(" );

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

	where_ptr += sprintf( where_ptr, ")" );

	return strdup( where );
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

	query_data->escaped_replaced_data =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
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

#ifdef NOT_DEFINED
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
#endif

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
		strlen( DICTIONARY_SEPARATE_SORT_PREFIX ),
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
			char *one_isa_folder_name,
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
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
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

	query_isa_widget->where =
		query_widget_where_new(
			(LIST *)0 /* folder_attribute_list */,
			(LIST *)0 /* relation_mto1_non_isa_list */,
			security_entity_where,
			(DICTIONARY *)0 /* drillthru dictionary */ );

	if ( !query_isa_widget->where )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: query_widget_where_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	query_isa_widget->query_order_string =
		query_order_string(
			query_isa_widget->query_select_list_string,
			(DICTIONARY *)0 /* sort_dictionary */ );

	query_isa_widget->delimited_list =
		query_delimited_list(
			query_isa_widget->query_select_list_string,
			one_isa_folder_name,
			query_isa_widget->where->string,
			query_isa_widget->query_order_string );

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
	enum date_convert_format date_convert_format;

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

		date_convert_format = query_date_convert->date_convert_format;
	}
	else
	{
		date_convert_format = date_convert_unknown;
	}

	query_select->column_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_column_string(
			folder_name,
			attribute_name,
			attribute_is_date,
			attribute_is_date_time,
			date_convert_format );

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

LIST *query_table_edit_select_list(
			LIST *folder_attribute_append_isa_list,
			LIST *no_display_name_list,
			LIST *exclude_lookup_attribute_name_list,
			int relation_mto1_isa_length,
			QUERY_DATE_CONVERT *query_date_convert,
			char *row_security_role_attribute_not_null )
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
			no_display_name_list ) )
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

	if ( row_security_role_attribute_not_null )
	{
		list_set(
			select_list,
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			query_select_new(
				(char *)0 /* folder_name */,
				row_security_role_attribute_not_null,
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

LIST *query_select_name_list( LIST *select_list )
{
	QUERY_SELECT *query_select;
	LIST *name_list;

	if ( !list_rewind( select_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		query_select = list_get( select_list );

		if ( !query_select->column_string )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: column_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set( name_list, query_select->column_string );

	} while ( list_next( select_list ) );

	return name_list;
}

char *query_select_list_string( LIST *select_list )
{
	char list_string[ 2048 ];
	char *ptr = list_string;
	QUERY_SELECT *query_select;

	if ( !list_rewind( select_list ) ) return (char *)0;

	do {
		query_select = list_get( select_list );

		if ( ptr != list_string ) ptr += sprintf( ptr, "," );

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

		relation_mto1->consumes_taken = 1;

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
			char *application_name,
			LIST *folder_attribute_append_isa_list,
			DICTIONARY *dictionary,
			int relation_mto1_isa_list_length )
{
	LIST *list = {0};
	QUERY_ATTRIBUTE *query_attribute;
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (LIST *)0;

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		query_attribute =
			query_attribute_new(
				application_name,
				folder_attribute->folder_name,
				folder_attribute->attribute_name,
				folder_attribute->attribute->datatype_name,
				dictionary,
				relation_mto1_isa_list_length );


		if ( query_attribute )
		{
			if ( !list ) list = list_new();

			list_set( list, query_attribute );
		}

	} while( list_next( folder_attribute_append_isa_list ) );

	return list;
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
			ptr += sprintf( ptr, " and " );
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

char *query_relation_character_string(
			enum query_relation query_relation_enum )
{
	if ( query_relation_enum == equal )
		return "=";
	else
	if ( query_relation_enum == not_equal )
		return "<>";
	else
	if ( query_relation_enum == greater_than )
		return ">";
	else
	if ( query_relation_enum == greater_than_equal_to )
		return ">=";
	else
	if ( query_relation_enum == less_than )
		return "<";
	else
	if ( query_relation_enum == less_than_equal_to )
		return "<=";
	else
	if ( query_relation_enum == begins )
		return QUERY_BEGINS;
	else
	if ( query_relation_enum == contains )
		return QUERY_CONTAINS;
	else
	if ( query_relation_enum == query_or )
		return QUERY_OR;
	else
	if ( query_relation_enum == not_contains )
		return QUERY_NOT_CONTAINS;
	else
	if ( query_relation_enum == between )
		return QUERY_BETWEEN;
	else
	if ( query_relation_enum == is_null )
		return QUERY_NULL;
	else
	if ( query_relation_enum == not_null )
		return QUERY_NOT_NULL;

	fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid query_relation_enum = %d.\n",
		__FILE__,
		__FUNCTION__,
		__LINE__,
		query_relation_enum );
	exit( 1 );
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

QUERY_TABLE_EDIT *query_table_edit_calloc( void )
{
	QUERY_TABLE_EDIT *query_table_edit;

	if ( ! ( query_table_edit = calloc( 1, sizeof( QUERY_TABLE_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_table_edit;
}

QUERY_TABLE_EDIT *query_table_edit_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			char *security_entity_where,
			LIST *relation_join_one2m_list,
			LIST *no_display_name_list,
			LIST *exclude_lookup_attribute_name_list,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_mto1_isa_list,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			char *row_security_role_folder_name,
			RELATION *row_security_role_relation,
			char *row_security_role_attribute_not_null )
{
	QUERY_TABLE_EDIT *query_table_edit;

	if ( !list_length( folder_attribute_append_isa_list ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_attribute_append_isa_list empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (QUERY_TABLE_EDIT *)0;
	}

	query_table_edit = query_table_edit_calloc();

	query_table_edit->select_list =
		query_table_edit_select_list(
			folder_attribute_append_isa_list,
			no_display_name_list,
			exclude_lookup_attribute_name_list,
			list_length( relation_mto1_isa_list ),
			query_date_convert_new( login_name ),
			row_security_role_attribute_not_null );

	if ( !list_length( query_table_edit->select_list ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: query_table_edit_select_list() returned  empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (QUERY_TABLE_EDIT *)0;
	}

	query_table_edit->query_select_list_string =
		query_select_list_string(
			query_table_edit->select_list );

	query_table_edit->query_select_name_list =
		query_select_name_list(
			query_table_edit->select_list );

	query_table_edit->from_string =
		query_table_edit_from_string(
			folder_name,
			relation_mto1_isa_list,
			row_security_role_folder_name );

	query_table_edit->where =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		query_table_edit_where_new(
			application_name,
			folder_name,
			folder_attribute_append_isa_list,
			relation_mto1_non_isa_list,
			relation_mto1_isa_list,
			security_entity_where,
			query_dictionary,
			row_security_role_relation );

	query_table_edit->query_order_string =
		query_order_string(
			query_table_edit->query_select_list_string,
			sort_dictionary );

	query_table_edit->query_system_string =
		query_system_string(
			query_table_edit->query_select_list_string,
			query_table_edit->from_string,
			query_table_edit->where->string,
			query_table_edit->query_order_string,
			APPASERVER_QUERY_TABLE_EDIT_MAX_ROWS );

	query_table_edit->row_dictionary_list =
		query_table_edit_row_dictionary_list(
			query_table_edit->query_system_string,
			query_table_edit->query_select_name_list,
			folder_attribute_primary_key_list(
				folder_attribute_append_isa_list )
					/* one_folder_primary_key_list */,
			application_name,
			relation_join_one2m_list,
			folder_name /* one_folder_name */ );

	return query_table_edit;
}

QUERY_WIDGET_WHERE *query_widget_where_new(
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary )
{
	QUERY_WIDGET_WHERE *where = query_widget_where_calloc();

	where->query_drop_down_list =
		query_drop_down_list(
			(char *)0 /* many_folder_name */,
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
			(char *)0 /* application_name */,
			folder_attribute_list,
			drillthru_dictionary,
			0 /* relation_mto1_isa_list_length */ );

	if ( list_length( where->query_attribute_list ) )
	{
		where->query_attribute_list_where =
			query_attribute_list_where(
				where->query_attribute_list );
	}

	where->string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_widget_where_string(
			where->query_drop_down_list_where,
			where->query_attribute_list_where,
			security_entity_where );

	return where;
}

char *query_widget_where_string(
			char *query_drop_down_where,
			char *query_attribute_list_where,
			char *security_entity_where )
{
	char string[ STRING_TWO_MEG ];
	char *ptr = string;

	strcpy( string, "1 = 1" );

	if ( query_drop_down_where && *query_drop_down_where )
	{
		ptr += sprintf(
			ptr,
			"%s",
			query_drop_down_where );
	}

	if ( query_attribute_list_where && *query_attribute_list_where )
	{
		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_attribute_list_where );
	}

	if ( security_entity_where && *security_entity_where )
	{
		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			security_entity_where );
	}

	return strdup( string );
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

QUERY_WIDGET *query_widget_fetch(
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
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
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

	query_widget->query_order_string =
		/* ----------------------------------------------- */
		/* Returns query_select_list_string or heap memory */
		/* ----------------------------------------------- */
		query_order_string(
			query_widget->query_select_list_string,
			(DICTIONARY *)0 /* sort_dictionary */ );

	query_widget->delimited_list =
		query_delimited_list(
			query_widget->query_select_list_string,
			folder_table_name(
				environment_application_name(),
				widget_folder_name )
					/* from_string */,
			query_widget->where->string,
			query_widget->query_order_string );

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

char *query_order_string(
			char *query_select_list_string,
			DICTIONARY *sort_dictionary )
{
	if ( dictionary_length( sort_dictionary ) )
	{
		/* Returns heap memory */
		/* ------------------- */
		return query_order_key_list_string(
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

		return query_select_list_string;
	}
}

char *query_order_key_list_string( LIST *key_list )
{
	char order_string[ 1024 ];
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

	if ( string_strncmp( found_key, FORM_SORT_ASCEND_LABEL ) == 0 )
	{
		strcpy(	order_string,
			string_search_replace_character(
				found_key +
				strlen( FORM_SORT_ASCEND_LABEL ),
				SQL_DELIMITER,
				',' ) );
	}
	else
	{
		sprintf(order_string,
			"%s desc",
			string_search_replace_character(
				found_key +
				strlen( FORM_SORT_DESCEND_LABEL ),
				SQL_DELIMITER,
				',' ) );
	}
	return strdup( order_string );
}

QUERY_TABLE_EDIT_WHERE *query_table_edit_where_calloc( void )
{
	QUERY_TABLE_EDIT_WHERE *query_table_edit_where;

	if ( ! ( query_table_edit_where =
			calloc( 1, sizeof( QUERY_TABLE_EDIT_WHERE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_table_edit_where;
}

QUERY_TABLE_EDIT_WHERE *query_table_edit_where_new(
			char *application_name,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_mto1_isa_list,
			char *security_entity_where,
			DICTIONARY *query_dictionary,
			RELATION *row_security_role_relation )
{
	QUERY_TABLE_EDIT_WHERE *where = query_table_edit_where_calloc();

	where->relation_mto1_isa_list_length =
		list_length(
			relation_mto1_isa_list );

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
				where->relation_mto1_isa_list_length );
	}

	where->query_attribute_list =
		query_attribute_list(
			application_name,
			folder_attribute_append_isa_list,
			query_dictionary,
			where->relation_mto1_isa_list_length );

	if ( list_length( where->query_attribute_list ) )
	{
		where->query_attribute_list_where =
			query_attribute_list_where(
				where->query_attribute_list );
	}

	where->query_join_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_join_where(
			application_name,
			folder_name,
			folder_attribute_primary_key_list(
				folder_attribute_append_isa_list ),
			relation_mto1_isa_list,
			row_security_role_relation );

	where->string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_table_edit_where_string(
			where->query_drop_down_list_where,
			where->query_attribute_list_where,
			where->query_join_where,
			security_entity_where );

	if ( !where->string )
	{
		where->string = "1 = 1";
	}

	return where;
}

char *query_table_edit_where_string(
			char *query_drop_down_list_where,
			char *query_attribute_list_where,
			char *query_join_where,
			char *security_entity_where )
{
	char string[ STRING_THREE_MEG ];
	char *ptr = string;

	*ptr = '\0';

	if ( query_drop_down_list_where )
	{
		ptr += sprintf(
			ptr,
			"%s",
			query_drop_down_list_where );
	}

	if ( query_attribute_list_where )
	{
		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_attribute_list_where );
	}

	if ( query_join_where )
	{
		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_join_where );
	}

	if ( security_entity_where )
	{
		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			security_entity_where );
	}

	if ( ptr == string )
		return (char *)0;
	else
		return strdup( string );
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
	char *operator_fetch;

	if ( ! ( key =
			query_relation_key(
				attribute_name,
				QUERY_RELATION_STARTING_LABEL ) ) )
	{
		yes_no_key = query_relation_yes_no_key( attribute_name );
	}

	if ( ! ( operator_fetch =
			query_relation_operator_fetch(
				key,
				yes_no_key,
				dictionary ) ) )
	{
		return (QUERY_RELATION *)0;
	}

	query_relation = query_relation_calloc();
	query_relation->key = key;
	query_relation->yes_no_key = yes_no_key;
	query_relation->operator_fetch = operator_fetch;

	query_relation->query_relation_enum =
		query_relation_enum(
			query_relation->operator_fetch,
			datatype_name );

	query_relation->character_string =
		query_relation_character_string(
			query_relation->query_relation_enum );

	return query_relation;
}

char *query_relation_key(
			char *attribute_name,
			char *query_relation_starting_label )
{
	static char key[ 512 ];

	sprintf(key,
		"%s%s",
		query_relation_starting_label,
		attribute_name );

	return key;
}

char *query_relation_yes_no_key(
			char *attribute_name )
{
	if ( attribute_is_yes_no( attribute_name ) )
	{
		return APPASERVER_EQUAL;
	}
	else
	{
		return (char *)0;
	}
}

char *query_relation_operator_fetch(
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
			char *operator_fetch,
			char *datatype_name )
{
	enum query_relation query_relation = {0};

	if ( !operator_fetch )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: operator_fetch is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( operator_fetch, APPASERVER_EQUAL ) == 0 )
		query_relation = equal;
	else
	if ( strcmp( operator_fetch, APPASERVER_NOT_EQUAL ) == 0 )
		query_relation = not_equal;
	else
	if ( strcmp( operator_fetch, APPASERVER_LESS_THAN ) == 0 )
		query_relation = less_than;
	else
	if ( strcmp( operator_fetch, APPASERVER_LESS_THAN_EQUAL_TO ) == 0 )
		query_relation = less_than_equal_to;
	else
	if ( strcmp( operator_fetch, APPASERVER_GREATER_THAN ) == 0 )
		query_relation = greater_than;
	else
	if ( strcmp( operator_fetch, APPASERVER_GREATER_THAN_EQUAL_TO ) == 0 )
		query_relation = greater_than_equal_to;
	else
	if ( strcmp( operator_fetch, APPASERVER_BETWEEN ) == 0 )
		query_relation = between;
	else
	if ( strcmp( operator_fetch, APPASERVER_BEGINS ) == 0 )
		query_relation = begins;
	else
	if ( strcmp( operator_fetch, APPASERVER_CONTAINS ) == 0 )
		query_relation = contains;
	else
	if ( strcmp( operator_fetch, APPASERVER_NOT_CONTAINS ) == 0 )
		query_relation = not_contains;
	else
	if ( strcmp( operator_fetch, APPASERVER_OR ) == 0 )
		query_relation = query_or;
	else
	if ( strcmp( operator_fetch, APPASERVER_NULL ) == 0 )
		query_relation = is_null;
	else
	if ( strcmp( operator_fetch, APPASERVER_NOT_NULL ) == 0 )
		query_relation = not_null;

	if ( query_relation == equal
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
			char *application_name,
			char *folder_name,
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
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_between_data_date_attribute_base(
			attribute_name,
			attribute_is_date( datatype_name ) ||
			attribute_is_time( datatype_name ) ) ) )
	{
		time_attribute_name =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
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

		query_between_data->to_time =
			query_between_data_to_time(
				time_attribute_name,
				dictionary );

		query_between_data->date_time_where =
			query_between_data_date_time_where(
				query_between_data->from_date,
				query_between_data->from_time,
				query_between_data->to_date,
				query_between_data->to_time,
				folder_table_name(
					application_name,
					folder_name ),
				relation_mto1_isa_list_length );
	}
	else
	{
		query_between_data->from =
			query_between_data_from(
				attribute_name,
				datatype_name,
				dictionary );

		query_between_data->to =
			query_between_data_to(
				attribute_name,
				datatype_name,
				dictionary );

		query_between_data->non_date_time_where =
			query_between_data_non_date_time_where(
				query_between_data->from,
				query_between_data->to,
				datatype_name,
				folder_table_name(
					application_name,
					folder_name ),
				relation_mto1_isa_list_length );
	}

	query_between_data->where_string =
		query_between_data_where_string(
			query_between_data->date_time_where,
			query_between_data->non_date_time_where );

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
		if ( !folder_table_name )
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

char *query_between_data_where_string(
			char *date_time_where,
			char *non_date_time_where )
{
	char where[ 1024 ];

	if ( !date_time_where
	&&   !non_date_time_where )
	{
		return (char *)0;
	}

	if ( date_time_where )
		strcpy( where, date_time_where );
	else
		strcpy( where, non_date_time_where );

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

char *query_attribute_list_where( LIST *query_attribute_list )
{
	char where[ STRING_64K ];
	char *ptr = where;
	QUERY_ATTRIBUTE *query_attribute;
	char *tmp;

	if ( !list_rewind( query_attribute_list ) ) return (char *)0;

	do {
		query_attribute =
			list_get(
				query_attribute_list );

		tmp =
			/* ------------------------------------------------ */
			/* Returns static memory, structure memory, or null */
			/* ------------------------------------------------ */
			query_attribute_where(
				/* ----------------------------- */
				/* Returns static memory or null */
				/* ----------------------------- */
				folder_table_name(
					query_attribute->application_name,
					query_attribute->folder_name ),
				query_attribute->relation_mto1_isa_list_length,
				query_attribute->query_relation,
				query_attribute->query_between_data,
				query_attribute->query_data );

		if ( !tmp ) continue;

		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			tmp );

	} while ( list_next( query_attribute_list ) );

	return strdup( where );
}

char *query_attribute_where(
			char *folder_table_name,
			int relation_mto1_isa_list_length,
			QUERY_RELATION *query_relation,
			QUERY_BETWEEN_DATA *query_between_data,
			QUERY_DATA *query_data )
{
	static char where[ 2048 ];

	if ( !query_between_data && !query_data ) return (char *)0;

	if ( query_between_data )
	{
		return query_between_data->where_string;
	}

	if ( !query_relation )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: query_relation is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !query_data )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: query_data is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !query_data->folder_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_data->folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !query_data->datatype_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_data->datatype_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !relation_mto1_isa_list_length )
	{
		folder_table_name = (char *)0;
	}

	if ( query_relation->query_relation_enum == is_null )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_is_null_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ) );
	}
	else
	if ( query_relation->query_relation_enum == not_null )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_not_null_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ) );
	}
	else
	if ( query_relation->query_relation_enum == query_or )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_or_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ),
			attribute_is_number(
				query_data->datatype_name ),
			query_data->escaped_replaced_data );
	}
	else
	if ( query_relation->query_relation_enum == begins )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_begins_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ),
			query_data->escaped_replaced_data );
	}
	else
	if ( query_relation->query_relation_enum == contains )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_contains_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ),
			query_data->escaped_replaced_data );
	}
	else
	if ( query_relation->query_relation_enum == not_contains )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_not_contains_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ),
			query_data->escaped_replaced_data );
	}
	else
	if ( query_relation->query_relation_enum == not_equal )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_not_equal_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ),
			attribute_is_number(
				query_data->datatype_name ),
			query_data->escaped_replaced_data );
	}
	else
	if ( attribute_is_number( query_data->datatype_name ) )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_number_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ),
			query_relation->character_string,
			query_data->escaped_replaced_data );
	}
	else
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_data_string_where(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			attribute_full_attribute_name(
				folder_table_name,
				query_data->attribute_name ),
			query_relation->character_string,
			query_data->escaped_replaced_data );
	}

	return where;
}

char *query_data_is_null_where(
			char *full_attribute_name )
{
	static char where[ 128 ];

	sprintf(where,
		"(%s = 'null' or %s is null)",
		full_attribute_name,
		full_attribute_name );

	return where;
}

char *query_data_not_null_where(
			char *full_attribute_name )
{
	static char where[ 128 ];

	sprintf(where,
		"(%s <> 'null' or %s is not null)",
		full_attribute_name,
		full_attribute_name );

	return where;
}

char *query_data_or_where(
			char *full_attribute_name,
			boolean attribute_is_number,
			char *escaped_replaced_data )
{
	static char where[ 1024 ];
	char *ptr = where;
	char piece_buffer[ 256 ];
	int i;
	char delimiter;
	char expression[ 256 ];

	delimiter = string_delimiter( escaped_replaced_data );

	ptr += sprintf( ptr, "(" );

	for(	i = 0;
		piece(	piece_buffer,
			delimiter,
			escaped_replaced_data,
			i );
		i++ )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " or " );

		if ( attribute_is_number )
		{
			sprintf(expression,
				"%s = %s",
				full_attribute_name,
				piece_buffer );
		}
		else
		{
			sprintf(expression,
				"%s = '%s'",
				full_attribute_name,
				piece_buffer );
		}

		ptr += sprintf(
	   		ptr,
			"%s",
			expression );
	}

	ptr += sprintf( ptr, ")" );

	return where;
}

char *query_data_begins_where(
			char *full_attribute_name,
			char *escaped_replaced_data )
{
	static char where[ 256 ];

	sprintf(where,
		"%s like '%s%c'",
		full_attribute_name,
		escaped_replaced_data,
		'%' );

	return where;
}

char *query_data_contains_where(
			char *full_attribute_name,
			char *escaped_replaced_data )
{
	static char where[ 256 ];

	sprintf(where,
		"%s like '%c%s%c'",
		full_attribute_name,
		'%',
		escaped_replaced_data,
		'%' );

	return where;
}

char *query_data_not_contains_where(
			char *full_attribute_name,
			char *escaped_replaced_data )
{
	static char where[ 256 ];

	sprintf(where,
		"%s not like '%c%s%c'",
		full_attribute_name,
		'%',
		escaped_replaced_data,
		'%' );

	return where;
}

char *query_data_not_equal_where(
			char *full_attribute_name,
			boolean attribute_is_number,
			char *escaped_replaced_data )
{
	static char where[ 512 ];
	char expression[ 256 ];

	if ( attribute_is_number )
	{
		sprintf(expression,
			"%s <> %s",
			full_attribute_name,
			escaped_replaced_data );
	}
	else
	{
		sprintf(expression,
			"%s <> '%s'",
			full_attribute_name,
			escaped_replaced_data );
	}

	sprintf(where,
	   	"(%s or %s is null)",
		expression,
		full_attribute_name );

	return where;
}

char *query_data_number_where(
			char *full_attribute_name,
			char *character_string,
			char *escaped_replaced_data )
{
	static char where[ 256 ];

	sprintf(where,
	   	"%s %s %s",
		full_attribute_name,
		character_string,
		escaped_replaced_data );

	return where;
}

char *query_data_string_where(
			char *full_attribute_name,
			char *character_string,
			char *escaped_replaced_data )
{
	static char where[ 256 ];

	sprintf(where,
	   	"%s %s '%s'",
		full_attribute_name,
		character_string,
		escaped_replaced_data );

	return where;
}

char *query_table_edit_from_string(
			char *folder_name,
			LIST *relation_mto1_isa_list,
			char *row_security_role_folder_name )
{
	char from_string[ 1024 ];
	char *ptr = from_string;

	if ( !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "%s", folder_name );

	if ( list_length( relation_mto1_isa_list ) )
	{
		ptr += sprintf(
			ptr,
			",%s",
			list_display_delimited(
				relation_mto1_folder_name_list(
					relation_mto1_isa_list ),
				',' ) );
	}

	if ( row_security_role_folder_name )
	{
		ptr += sprintf(
			ptr,
			",%s",
			row_security_role_folder_name );
	}

	return strdup( from_string );
}

char *query_join_where(
			char *application_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *relation_mto1_isa_list,
			RELATION *row_security_role_relation )
{
	char where[ STRING_8K ];
	char *ptr = where;
	char *primary_folder_table_name = {0};

	if ( !application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( relation_mto1_isa_list ) )
	{
		primary_folder_table_name =
			strdup(
				/* ----------------------------- */
				/* Returns static memory or null */
				/* ----------------------------- */
				folder_table_name(
					application_name,
					folder_name ) );

		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_join_relation_where(
				application_name,
				primary_folder_table_name,
				primary_key_list,
				relation_mto1_isa_list ) );
	}

	if ( row_security_role_relation )
	{
		if ( !row_security_role_relation->many_folder )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: many_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !primary_folder_table_name )
		{
			primary_folder_table_name =
				strdup(
					/* ----------------------------- */
					/* Returns static memory or null */
					/* ----------------------------- */
					folder_table_name(
						application_name,
						folder_name ) );
		}

		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_string_join_where(
				primary_folder_table_name,
				folder_table_name(
					application_name,
					row_security_role_relation->
						many_folder->
						folder_name ),
				primary_key_list,
				row_security_role_relation->
					foreign_key_list ) );
	}

	if ( primary_folder_table_name )
	{
		free( primary_folder_table_name );
	}

	if ( where == ptr )
		return (char *)0;
	else
		return strdup( where );
}

char *query_join_relation_where(
			char *application_name,
			char *primary_folder_table_name,
			LIST *primary_key_list,
			LIST *relation_mto1_isa_list )
{
	static char where[ 2048 ];
	char *ptr = where;
	RELATION *relation;

	if ( !list_rewind( relation_mto1_isa_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: relation_mto1_isa_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		relation = list_get( relation_mto1_isa_list );

		if (	list_length( primary_key_list ) !=
			list_length( relation->foreign_key_list ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: length primary_key_list:%d != length foreign_key_list:%d.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				list_length( primary_key_list ),
				list_length( relation->foreign_key_list ) );
			exit( 1 );
		}

		if ( !relation->one_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !relation->one_folder->folder_name )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: one_folder->folder_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_string_join_where(
				primary_folder_table_name,
				/* ----------------------------- */
				/* Returns static memory or null */
				/* ----------------------------- */
				folder_table_name(
					application_name,
					relation->one_folder->folder_name )
					/* relation_folder_table_name */,
				primary_key_list,
				relation->foreign_key_list ) );

	} while ( list_next( relation_mto1_isa_list ) );

	return where;
}

char *query_string_join_where(
			char *primary_folder_table_name,
			char *relation_folder_table_name,
			LIST *primary_key_list,
			LIST *foreign_key_list )
{
	static char where[ 1024 ];
	char *ptr = where;

	if ( !list_length( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if (	list_length( primary_key_list ) !=
		list_length( foreign_key_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: length primary_key_list:%d != length foreign_key_list:%d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( primary_key_list ),
			list_length( foreign_key_list ) );
		exit( 1 );
	}

	if ( !primary_folder_table_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: primary_folder_table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !relation_folder_table_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: relation_folder_table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	while ( 1 )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s.%s = %s.%s",
			primary_folder_table_name,
			(char *)list_get( primary_key_list ),
			relation_folder_table_name,
			(char *)list_get( foreign_key_list ) );

		if ( !list_next( primary_key_list ) ) break;
		list_next( foreign_key_list );
	}

	return where;
}

char *query_drop_down_row_data_list_string(
			char *key,
			DICTIONARY *dictionary )
{
	return dictionary_get( key, dictionary );
}

LIST *query_drop_down_row_data_string_list(
			char sql_delimiter,
			char *data_list_string )
{
	return
	list_string_list(
		data_list_string,
		sql_delimiter );

}

