/* list_usage.c */
/* ------------ */
/* ======================================================= */
/* These are functions that use the list package.	   */
/*                                                         */
/* Tim Riley                                               */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "list.h"
#include "list_usage.h"
#include "dictionary.h"
#include "column.h"
#include "timlib.h"
#include "piece.h"
#include "date_convert.h"

LIST *pipe2dictionary_list(	 	char *sys_string, 
					LIST *attribute_name_list,
					LIST *date_attribute_name_list,
					char field_delimiter,
					char *application_name,
					char *login_name )
{
	return list_usage_pipe2dictionary_list(
			sys_string, 
			attribute_name_list,
			date_attribute_name_list,
			field_delimiter,
			application_name,
			login_name );
}

LIST *list_usage_pipe2dictionary_list(
			char *sys_string, 
			LIST *attribute_name_list,
			LIST *date_attribute_name_list,
			char field_delimiter,
			char *application_name,
			char *login_name )
{
	char buffer[ 65536 ];
	char data[ 65536 ];
	LIST *list = list_new();
	DICTIONARY *dictionary;
	char *attribute_name;
	int i;
	FILE *p;
	DATE_CONVERT *date_convert = {0};

	if ( !list_length( attribute_name_list ) )
	{
		fprintf(stderr,
		"Warning: %s/%s()/%d got an empty attribute name list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (LIST *)0;
	}

	p = popen( sys_string, "r" );

	while( get_line( buffer, p ) )
	{
		list_rewind( attribute_name_list );

		dictionary = dictionary_small_dictionary_new();

		list_set( list, dictionary );

		for( i = 0; ; i++ )
		{
			if ( !piece( data, field_delimiter, buffer, i ) )
			{
				*data = '\0';
			}

			attribute_name =
				list_get(
					attribute_name_list );

			search_replace_string( data, "\\", "" );

			if ( login_name
			&&   date_attribute_name_list
			&&   list_exists_string(
				attribute_name,
				date_attribute_name_list ) )
			{
				if ( !date_convert )
				{
				      date_convert =
				      date_convert_new_user_format_date_convert(
						application_name,
						login_name,
						data );
	
					if ( !date_convert )
					{
						fprintf( stderr,
			 "Error in %s/%s(): cannot fetch from database\n",
				 		__FILE__,
				 		__FUNCTION__ );
						exit( 1 );
					}
				}
				else
				{
					date_convert->source_format =
					date_convert_populate_return_date(
						date_convert->return_date,
						date_convert->source_format,
						date_convert->
							destination_format,
						data );

				}

				if ( *date_convert->return_date )
				{
					strcpy( data,
						date_convert->return_date );
				}
			}

			dictionary_set_pointer(
				dictionary, 
				attribute_name, 
				strdup( data ) );

			if ( !next_item( attribute_name_list ) ) break;
		}
	}

	pclose( p );

	return list;
}

/* ------------------------------------------------------------- */
/* Sample dictionary_string: "datatype=salinity&station=station" */
/* ------------------------------------------------------------- */
LIST *dictionary_string2list( char *dictionary_string, 
			      int attribute_or_data )
{
	LIST *list = list_new();
	char buffer[ 65536 ];
	char data[ 65536 ];
	char attribute[ 256 ];
	char attribute_trimmed[ 256 ];
	int i;

	for( i = 0; piece( buffer, '&', dictionary_string, i ); i++ )
	{
		trim_index( attribute_trimmed,
			    piece( attribute, '=', buffer, 0 ) );

		if ( piece( data, '=', buffer, 1 ) )
		{
			if ( *data 
			&&   strcmp( attribute_trimmed, data ) != 0 )
			{
				if ( attribute_or_data == 
				     LIST_POPULATED_ATTRIBUTE )
					list_append_string( 
						list,
						strdup( attribute ) );
				else
					list_append_string( 
						list,
						strdup( data ) );
			}
		}
	}
	return list;
}

/* Sample: "first_name=tim&last_name=riley" */
/* ---------------------------------------- */
void list_subtract_dictionary_string( LIST *list, char *dictionary_string )
{
	char pair_string[ 1024 ];
	char attribute_string[ 1024 ];
	char data_string[ 1024 ];
	int i;

	for( i = 0; piece( pair_string, '&', dictionary_string, i ); i++ )
	{
		piece( attribute_string, '=', pair_string, 0 );

		if ( piece( data_string, '=', pair_string, 1 ) )
		{
			/* Don't delete if attribute=attribute */
			/* ----------------------------------- */
			if ( *data_string
			&&   strcmp( data_string, attribute_string ) != 0
			&&   item_exists( 	list, 
						attribute_string, 
						list_strcmp ) )
			{
				delete_current( list );
			}
		}
	}
}

char *two_lists_to_dictionary_string( LIST *list1, LIST *list2 )
{
	char return_buffer[ 65536 ];
	char *ptr_buffer = return_buffer;
	char *ptr_list1, *ptr_list2;
	register int first_time = 1;

	*ptr_buffer = '\0';

	if ( go_head( list1 ) && go_head( list2 ) )
	{
		do {
			ptr_list1 = retrieve_item_ptr( list1 );
			ptr_list2 = retrieve_item_ptr( list2 );

			if ( first_time )
			{
				first_time = 0;
				ptr_buffer += sprintf( 	ptr_buffer,
							"%s=%s",
							ptr_list1,
							ptr_list2 );
			}
			else
			{
				ptr_buffer += sprintf( 	ptr_buffer,
							"&%s=%s",
							ptr_list1,
							ptr_list2 );
			}
			if ( !next_item( list2 ) ) break;
		} while( next_item( list1 ) );
	}
	return strdup( return_buffer );
}


LIST *list_usage_get_column_list( char *string )
{
	int c;
	char buffer[ 1024 ];
	LIST *return_list = list_new();

	for( c = 0; column( buffer, c, string ); c++ )
		list_append_string( return_list, buffer );

	return return_list;

}

LIST *sys_string2list( char *sys_string )
{
	return pipe2list( sys_string );
}

char *search_replace_list( 	char *source_destination, 
				LIST *list, 
				DICTIONARY *dictionary )
{
	char *key;
	char *data;

	if ( list_reset( list ) )
	{
		do {
			key = list_get( list );

			data = dictionary_get( key, dictionary );

			if ( data )
			{
				search_replace_word(
					source_destination,
					key,
					data );
			}
		} while( list_next( list ) );
	}
	return source_destination;
}

char *search_replace_list_prepend( 	char *source_destination, 
					LIST *list, 
					DICTIONARY *dictionary,
					char replace_char_to_prepend )
{
	char *key;
	char *data;
	char data_to_replace[ 1024 ];

	if ( list_reset( list ) )
	{
		do {
			key = list_get( list );

			data = dictionary_get( key, dictionary );

			if ( data )
			{
				sprintf( data_to_replace,
					 "%c%s",
					 replace_char_to_prepend,
					 data );

				search_replace_word(
					source_destination,
					key,
					data_to_replace );
			}
		} while( list_next( list ) );
	}
	return source_destination;
}

char *search_replace_list_index_prepend_single_quoted(
				char *source_destination, 
				LIST *list,
				DICTIONARY *dictionary,
				int dictionary_key_offset,
				char *search_key_prepend,
				char search_char_prepend )
{
	char *key;
	char *data;
	char key_to_search[ 1024 ];
	char key_to_replace[ 1024 ];
	char data_to_replace[ 1024 ];

	if ( list_reset( list ) )
	{
		do {
			key = list_get( list );
			sprintf( key_to_search, 
				 "%s%s", 
				 search_key_prepend, 
				 key );

			if (  dictionary_get_index_data(		
					&data,
					dictionary,
					key_to_search,
					dictionary_key_offset ) > -1 )
			{
				sprintf( key_to_replace,
					 "%c%s",
					 search_char_prepend,
					 key );

				sprintf( data_to_replace,
					 "%c'%s'",
					 search_char_prepend,
					 data );

				search_replace_word(
					source_destination,
					key_to_replace,
					data_to_replace );
			}
		} while( list_next( list ) );
	}
	return source_destination;
}

char *search_replace_list_index_prepend_double_quoted(
				char *source_destination, 
				LIST *list,
				DICTIONARY *dictionary,
				int dictionary_key_offset,
				char *search_key_prepend,
				char search_char_prepend )
{
	char *key;
	char *data;
	char key_to_search[ 1024 ];
	char key_to_replace[ 1024 ];
	char data_to_replace[ 1024 ];

	if ( list_reset( list ) )
	{
		do {
			key = list_get( list );

			sprintf( key_to_search, 
				 "%s%s", 
				 search_key_prepend, 
				 key );

			if (  dictionary_get_index_data(		
					&data,
					dictionary,
					key_to_search,
					dictionary_key_offset ) > -1 )
			{
				sprintf( key_to_replace,
					 "%c%s",
					 search_char_prepend,
					 key );

				sprintf( data_to_replace,
					 "%c\"%s\"",
					 search_char_prepend,
					 data );

				search_replace_word(
					source_destination,
					key_to_replace,
					data_to_replace );
			}
		} while( list_next( list ) );
	} /* if */

	return source_destination;

}

void list_interpolate_string_record( LIST *list, char delimiter )
{
	double *double_array;
	double original_value;
	char buffer[ 1024 ];
	char double_string[ 512 ];
	char *s;
	int last_piece;
	int i = 0;

	double_array = (double *)
		calloc( list_length( list ), sizeof( double ) );
	
	if ( !list_rewind( list ) ) return;

	last_piece = character_count( delimiter, list_get_string( list ) );

	/* Build the double array */
	/* ---------------------- */
	do {
		s = list_get_string( list );
		double_array[ i++ ] = 
			atof( piece( buffer, delimiter, s, last_piece ) );
	} while( list_next( list ) );

	/* Fix the double array */
	/* -------------------- */
	linear_interpolate( double_array, 0, list_length( list ) - 1 );

	/* Append */
	/* ------ */
	list_reset( list );
	i = 0;
	do {
		s = list_get_string( list );
		original_value =
			atof( piece( buffer, delimiter, s, last_piece ) );
		if ( original_value != double_array[ i ] )
		{
			strcpy( buffer, s );
			sprintf( double_string, ",%lf", double_array[ i ] );
			strcat( buffer, double_string );
			list_set_current_pointer( list, strdup( buffer ) );
		}
		i++;
	} while( list_next( list ) );

	free( double_array );
}

int list_get_string_item_offset( LIST *list, char *search_string )
{
	char *item;
	int i = 0;

	if ( list && list_rewind( list ) )
	{
		do {
			item = list_get_string( list );
			if ( strcmp( item, search_string ) == 0 ) return i;
			i++;
		} while( list_next( list ) );
	}
	return -1;
}

void list_separate( LIST *list, int c )
{
	LIST *new_list;
	char buffer[ 128 ];
	char *ptr;
	int i;

	new_list = list_new();

	if ( list_rewind( list ) )
	{
		do {
			ptr = list_get_string( list );
			for( i = 0; piece( buffer, c, ptr, i ); i++ )
				list_append_string( new_list, buffer );
		} while( list_next( list ) );
	}
	list_append_list( list, new_list );
}

LIST *list_usage_piece_list( LIST *list, char delimiter, int offset )
{
	LIST *new_list;
	char buffer[ 1024 ];
	char *ptr;

	new_list = list_new();

	if ( list_rewind( list ) )
	{
		do {
			ptr = list_get_pointer( list );
			if ( piece( buffer, delimiter, ptr, offset ) )
			{
				list_append_pointer(
					new_list, strdup( buffer ) );
			}
		} while( list_next( list ) );
	}
	return new_list;
}

char *list_usage_attribute_data_list2where_clause(
				LIST *attribute_name_list,
				LIST *attribute_data_list )
{
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;
	char buffer[ 4096 ];

	where_ptr += sprintf( where_ptr, "1 = 1" );

	if ( !list_rewind( attribute_name_list )
	||   !list_rewind( attribute_data_list ) )
	{
		return "1 = 2";
	}

	do {

		where_ptr += sprintf(
				where_ptr,
				" and %s = '%s'",
				list_get_string( attribute_name_list ),
				escape_character(
					buffer,
					list_get_string(
						attribute_data_list ),
					'\'' ) );

		if ( !list_next( attribute_data_list ) ) break;
	} while( list_next( attribute_name_list ) );
	return strdup( where_clause );
}

LIST *list_usage_add_prefix(	LIST *source_list,
				char *prefix )
{
	char *data;
	char buffer[ 1024 ];
	LIST *return_list;

	return_list = list_new();
	if ( list_rewind( source_list ) )
	{
		do {
			data = list_get_string( source_list );
			sprintf( buffer, "%s%s", prefix, data );
			list_append_string( return_list, strdup( buffer ) );
		} while( list_next( source_list ) );
	}
	return return_list;
}

LIST *list_usage_remove_prefix(	LIST *source_list,
				char *prefix )
{
	char *data;
	int str_len;
	LIST *return_list;

	return_list = list_new();
	str_len = strlen( prefix );

	if ( list_rewind( source_list ) )
	{
		do {
			data = list_get_string( source_list );

			if ( timlib_strncmp( data, prefix ) == 0 )
				data = data + str_len;
			list_append_string( return_list, strdup( data ) );
		} while( list_next( source_list ) );
	}
	return return_list;
}

char *list_usage_concat( LIST *attribute_name_list )
{
	char *attribute_name;
	char buffer[ 1024 ];
	char *buffer_ptr = buffer;

	buffer_ptr += sprintf( buffer_ptr, "concat(" );

	if ( list_rewind( attribute_name_list ) )
	{
		do {
			attribute_name =
				list_get_pointer(
					attribute_name_list );

			if ( !list_at_head( attribute_name_list ) )
			{
				buffer_ptr +=
					sprintf(buffer_ptr,
						",'%c',",
						' ' );
			}

			buffer_ptr += sprintf(	buffer_ptr,
						"%s",
						attribute_name );
		} while( list_next( attribute_name_list ) );
	}

	buffer_ptr += sprintf( buffer_ptr, ")" );
	*buffer_ptr = '\0';
	return strdup( buffer );
}

char *list_usage_get_in_clause( LIST *column_string_list )
{
	char in_clause[ 65536 ];
	char *ptr = in_clause;
	char *column;
	boolean first_time = 1;

	if ( !list_rewind( column_string_list ) ) return "";

	ptr += sprintf( ptr, "(" );
	do {
		column = list_get_pointer( column_string_list );

		if ( first_time )
			first_time = 0;
		else
		{
			ptr += sprintf( ptr, "," );
		}

		ptr += sprintf( ptr, "'%s'", column );
	} while( list_next( column_string_list ) );
	ptr += sprintf( ptr, ")" );
	*ptr = '\0';
	return strdup( in_clause );
}

LIST *list_usage_attribute_data_dictionary_merge_list(
			LIST *primary_key_list,
			LIST *primary_data_list,
			char delimiter )
{
	DICTIONARY *dictionary;
	LIST *data_dictionary_merge_list;
	char *primary_data;
	char *primary_key;
	char piece_buffer[ 1024 ];
	int offset;

	data_dictionary_merge_list = list_new();

	if ( !list_length( primary_key_list ) )
		return data_dictionary_merge_list;

	if ( !list_rewind( primary_data_list ) )
		return data_dictionary_merge_list;

	do {
		primary_data = list_get_pointer( primary_data_list );
		
		dictionary = dictionary_small_dictionary_new();
		list_append_pointer( data_dictionary_merge_list, dictionary );

		list_rewind( primary_key_list );
		offset = 0;

		do {
			primary_key =
				list_get_pointer(
					primary_key_list );

			if ( !piece(	piece_buffer,
					delimiter,
					primary_data,
					offset++ ) )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: cannot piece(%d), delimiter = (%c), primary_data = (%s)\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 offset - 1,
					 delimiter,
					 primary_data );
				exit( 1 );
			}

			dictionary_set_pointer(	dictionary, 
						primary_key, 
						strdup( piece_buffer ) );
		} while( list_next( primary_key_list ) );
	} while( list_next( primary_data_list ) );

	return data_dictionary_merge_list;

}

LIST *list_usage_file2list( char *filename )
{
	FILE *input_file;
	char input_buffer[ 65536 ];
	LIST *return_list;

	if ( ! ( input_file = fopen( filename, "r" ) ) ) return (LIST *)0;

	return_list = list_new();

	while( get_line( input_buffer, input_file ) )
	{
		list_append_pointer( return_list, strdup( input_buffer ) );
	}

	fclose( input_file );
	return return_list;

}

void list_usage_stdout_display( LIST *list )
{
	if ( list_rewind( list ) )
	{
		do {
			printf( "%s\n", (char *)list_get( list ) );

		} while( list_next( list ) );
	}

}

boolean list_usage_lists_equal(	LIST *list1,
				LIST *list2 )
{
	char *data1;
	char *data2;

	if ( !list_length( list1 ) ) return 0;

	if ( list_length( list1 ) != list_length( list2 ) ) return 0;

	list_rewind( list1 );
	list_rewind( list2 );

	do {
		data1 = list_get( list1 );
		data2 = list_get( list2 );

		if ( timlib_strcmp( data1, data2 ) != 0 ) return 0;

		list_next( list2 );
	} while( list_next( list1 ) );

	return 1;

}

/* Assume comma and dash delimited string. */
/* --------------------------------------- */
LIST *list_usage_expression2number_list( char *expression )
{
	LIST *number_list = list_new();
	char comma_buffer[ 16 ];
	char from_range_buffer[ 16 ];
	char to_range_buffer[ 16 ];
	int from_range;
	int to_range;
	int p;
	int range;
	int *number_ptr;

	for(	p = 0;
		piece( comma_buffer, ',', expression, p );
		p++ )
	{
		if ( character_exists( comma_buffer, '-' ) )
		{
			piece( from_range_buffer, '-', comma_buffer, 0 );
			from_range = atoi( from_range_buffer );

			piece( to_range_buffer, '-', comma_buffer, 1 );
			to_range = atoi( to_range_buffer );

			for(	range = from_range;
				range <= to_range;
				range++ )
			{
				number_ptr = calloc( 1, sizeof ( int ) );
				*number_ptr = range;
				list_append_pointer( number_list, number_ptr );
			}
			continue;
		}

		number_ptr = calloc( 1, sizeof ( int ) );
		*number_ptr = atoi( comma_buffer );
		list_append_pointer( number_list, number_ptr );
	}

	return number_list;

}

