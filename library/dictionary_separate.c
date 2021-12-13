/* $APPASERVER_HOME/library/dictionary_separate.c	   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "element.h"
#include "appaserver_library.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

DICTIONARY *dictionary_separate_original_post_dictionary(
			char *dictionary_string )
{
	char escaped_delimiter_string[ 3 ];
	char delimiter_string[ 2 ];

	/* ---------------------------------------------------- */
	/* If from output_results, then this is executed by	*/
	/* window.open(). Therefore, the delimiters are escaped.*/
	/* ---------------------------------------------------- */
	sprintf( escaped_delimiter_string,
		 "\\%c",
		 DICTIONARY_ALTERNATIVE_DELIMITER );

	if ( string_exists(
			dictionary_string,
			escaped_delimiter_string ) )
	{
		sprintf( delimiter_string,
			 "%c",
			 DICTIONARY_ALTERNATIVE_DELIMITER );

		search_replace_string(
			dictionary_string,
			escaped_delimiter_string,
			delimiter_string );

		sprintf( escaped_delimiter_string,
		 	 "\\%c",
			 '&' );

		sprintf( delimiter_string,
			 "%c",
			 '&' );

		search_replace_string(
			dictionary_string,
			escaped_delimiter_string,
			delimiter_string );
	}

	return
	dictionary_string2dictionary( 
		dictionary_string );
}

DICTIONARY_SEPARATE *dictionary_separate_calloc( void )
{
	DICTIONARY_SEPARATE *dictionary_separate;

	if ( ! ( dictionary_separate =
			calloc( 1, sizeof( DICTIONARY_SEPARATE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return dictionary_separate;
}

DICTIONARY_SEPARATE *dictionary_separate_folder_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *operation_name_list )
{
	DICTIONARY_SEPARATE *dictionary_separate =
		dictionary_separate_calloc();

	dictionary_separate->original_post_dictionary =
		original_post_dictionary;

	dictionary_separate->application_name = application_name;

	dictionary_separate->folder_attribute_date_name_list =
		folder_attribute_date_name_list;

	dictionary_separate->operation_name_list = operation_name_list;
	dictionary_separate->login_name = login_name;

	if ( !dictionary_length(
			dictionary_separate->
				original_post_dictionary ) )
	{
		return dictionary_separate;
	}

	dictionary_separate->working_post_dictionary =
		dictionary_separate_working_post(
			original_post_dictionary,
			application_name,
			folder_attribute_date_name_list,
			login_name );

	dictionary_separate->sort_dictionary =
		dictionary_separate_sort(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->query_dictionary =
		dictionary_separate_query(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->drillthru_dictionary =
		dictionary_separate_drillthru(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->ignore_dictionary =
		dictionary_separate_ignore(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->ignore_select_attribute_name_list =
		dictionary_separate_ignore_select_attribute_name_list(
			dictionary_separate->ignore_dictionary );

	dictionary_separate->pair_one2m_dictionary =
		dictionary_separate_pair_one2m(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			dictionary_separate->
				working_post_dictionary );

	return dictionary_separate;
}

DICTIONARY_SEPARATE *dictionary_separate_row_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *attribute_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *operation_name_list )
{
	DICTIONARY_SEPARATE *dictionary_separate =
		dictionary_separate_calloc();

	dictionary_separate->original_post_dictionary =
		original_post_dictionary;

	dictionary_separate->application_name = application_name;

	dictionary_separate->folder_attribute_date_name_list =
		folder_attribute_date_name_list;

	dictionary_separate->operation_name_list = operation_name_list;
	dictionary_separate->login_name = login_name;

	if ( !dictionary_length( original_post_dictionary ) )
		return dictionary_separate;

	dictionary_separate->working_post_dictionary =
		dictionary_separate_working_post(
			original_post_dictionary,
			application_name,
			folder_attribute_date_name_list,
			login_name );

	dictionary_separate->sort_dictionary =
		dictionary_separate_sort(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->query_dictionary =
		dictionary_separate_query(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->drillthru_dictionary =
		dictionary_separate_drillthru(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->ignore_dictionary =
		dictionary_separate_ignore(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->ignore_select_attribute_name_list =
		dictionary_separate_ignore_select_attribute_name_list(
			dictionary_separate->ignore_dictionary );

	dictionary_separate->pair_one2m_dictionary =
		dictionary_separate_pair_one2m(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->row_dictionary =
		dictionary_separate_row_dictionary_multi_row(
			dictionary_separate->
				non_prefixed_dictionary,
			attribute_name_list,
			operation_name_list );

	return dictionary_separate;
}

DICTIONARY_SEPARATE *dictionary_separate_string_new(
			DICTIONARY *original_post_dictionary )
{
	DICTIONARY_SEPARATE *dictionary_separate =
		dictionary_separate_calloc();

	dictionary_separate->original_post_dictionary =
		original_post_dictionary;

	return dictionary_separate;
}

DICTIONARY *dictionary_separate_working_post(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *folder_attribute_date_name_list,
			char *login_name )
{
	DICTIONARY *working_post_dictionary;

	working_post_dictionary =
		dictionary_large_copy(
			original_post_dictionary );

	dictionary_separate_parse_multi_attribute_keys(
		working_post_dictionary,
		DICTIONARY_SEPARATE_QUERY_PREFIX );

	dictionary_separate_parse_multi_attribute_keys(
		working_post_dictionary,
		DICTIONARY_SEPARATE_DRILLTHRU_PREFIX );

	dictionary_separate_parse_multi_attribute_keys(
		working_post_dictionary,
		DICTIONARY_SEPARATE_SORT_PREFIX );

	dictionary_separate_parse_multi_attribute_keys(
		working_post_dictionary,
		DICTIONARY_SEPARATE_PAIR_ONE2M_PREFIX );

	if ( list_length( folder_attribute_date_name_list ) )
	{
		dictionary_separate_date_convert(
			working_post_dictionary,
			application_name,
			folder_attribute_date_name_list,
			login_name );
	}

	/* Remove the index appended to the multi-select lookup widget. */
	/* ------------------------------------------------------------ */
	dictionary_separate_trim_multi_drop_down_index(
		working_post_dictionary,
		ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	/* Trim the [common_non_primary_attribute data] 		*/
	/* Note: this must follow					*/
	/* 	 dictionary_separate_trim_multi_drop_down_index()	*/
	/* ------------------------------------------------------------ */
	dictionary_separate_trim_double_bracked_data(
		working_post_dictionary );

	return working_post_dictionary;
}

DICTIONARY *dictionary_separate_sort(
			DICTIONARY *working_post_dictionary )
{
	return dictionary_separate_prefixed(
		working_post_dictionary,
		DICTIONARY_SEPARATE_SORT_PREFIX );

}

DICTIONARY *dictionary_separate_prefixed(
			DICTIONARY *post_dictionary,
			char *prefix )
{
	DICTIONARY *return_dictionary;

	if ( ! ( return_dictionary =
			dictionary_without_prefix(
				post_dictionary,
				prefix ) ) )
	{
		return_dictionary = dictionary_small();
	}

	return return_dictionary;
}

DICTIONARY *dictionary_separate_drillthru(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
			dictionary_without_prefix(
				working_post_dictionary,
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX ) ) )
	{
		dictionary = dictionary_small();
	}

	dictionary_add_suffix_zero( dictionary );

	dictionary_separate_remove_from_starting_label( dictionary );

	return dictionary;

}

DICTIONARY *dictionary_separate_ignore_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_without_prefix(
			working_post_dictionary,
			IGNORE_SELECT_PUSH_BUTTON_PREFIX ) ) )
	{
		dictionary = dictionary_small();
	}

	dictionary_append_dictionary(
		dictionary,
		dictionary_without_prefix(
			working_post_dictionary,
			NO_DISPLAY_PUSH_BUTTON_PREFIX ) );

	dictionary_separate_parse_multi_attribute_keys(
		dictionary,
		(char *)0 /* prefix */ );

	return dictionary;
}

DICTIONARY *dictionary_separate_pair_one2m_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_without_prefix(
			working_post_dictionary,
			DICTIONARY_SEPARATE_PAIR_ONE2M_PREFIX ) ) )
	{
		dictionary = dictionary_small();
	}

	return dictionary;
}

DICTIONARY *dictionary_separate_query(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
			dictionary_without_prefix(
				working_post_dictionary,
				DICTIONARY_SEPARATE_QUERY_PREFIX ) ) )
	{
		dictionary = dictionary_small();
	}

	dictionary_add_suffix_zero( dictionary );

	dictionary_separate_remove_from_starting_label( dictionary );

	dictionary_separate_parse_multi_attribute_keys(
		dictionary,
		RELATION_OPERATOR_PREFIX );

	return dictionary;

}

void dictionary_separate_parse_multi_attribute_keys(
			DICTIONARY *source_destination,
			char *prefix )
{
	/* ------------------------------------------------------------ */
	/* Sample:							*/
	/* From:  "station^datatype_1=BA^stage"				*/
	/* To:    "station_1=BA and datatype_1=stage"			*/
	/* ------------------------------------------------------------ */
	dictionary_parse_multi_attribute_keys(
		source_destination, 
		MULTI_ATTRIBUTE_KEY_DELIMITER,
		SQL_DELIMITER,
		prefix,
		1 /* dont_include_relational_operators */ );

	dictionary_parse_multi_attribute_keys(
		source_destination, 
		SQL_DELIMITER,
		SQL_DELIMITER,
		prefix,
		1 /* dont_include_relational_operators */ );
}

DICTIONARY *dictionary_separate_row_dictionary_row_zero(
			DICTIONARY *working_post_dictionary,
			LIST *attribute_name_list )
{
	char *attribute_name;
	char key[ 256 ];
	char *dictionary_row;
	DICTIONARY *row_dictionary;

	if ( !list_length( attribute_name_list ) ) return (DICTIONARY *)0;

	row_dictionary = dictionary_small();

	list_rewind( attribute_name_list );
	do {
		attribute_name =
			list_get( attribute_name_list );

		sprintf(	key,
				"%s_0",
				attribute_name );

		dictionary_row =
			dictionary_get(
				key,
				working_post_dictionary );

		if ( dictionary_row )
		{
			dictionary_set(
				row_dictionary,
				strdup( key ),
				dictionary_row );
		}

	} while( list_next( attribute_name_list ) );

	return row_dictionary;
}

DICTIONARY *dictionary_separate_row_dictionary_multi_row(
			DICTIONARY *non_prefixed_dictionary,
			LIST *attribute_name_list,
			LIST *operation_name_list )
{
	int index;
	char *full_attribute_name;
	char *operation_name;
	char key[ 256 ];
	char *data;
	DICTIONARY *row_dictionary;
	int highest_index;

	if ( !list_length( attribute_name_list ) ) return (DICTIONARY *)0;

	highest_index =
		dictionary_key_highest_index(
			non_prefixed_dictionary );

	row_dictionary = dictionary_large();

	for( index = 0; index <= highest_index; index++ )
	{
		list_rewind( attribute_name_list );

		do {
			full_attribute_name = list_get( attribute_name_list );

			sprintf(key,
				"%s_%d",
				full_attribute_name,
				index );

			data =
				dictionary_get(
					key,
					non_prefixed_dictionary );

			if ( data )
			{
				dictionary_set(
					row_dictionary,
					strdup( key ),
					data );
			}
		} while( list_next( attribute_name_list ) );

		if ( !list_rewind( operation_name_list ) ) continue;

		do {
			operation_name = 
				list_get(
					operation_name_list );

			sprintf(key,
				"%s_%d",
				operation_name,
				index );

			data =
				dictionary_get(
					key,
					non_prefixed_dictionary );

			if ( data )
			{
				dictionary_set(
					row_dictionary,
					strdup( key ),
					data );
			}
		} while( list_next( operation_name_list ) );

	} /* for each index */

	return row_dictionary;
}

DICTIONARY *dictionary_separate_row(
			DICTIONARY *row_dictionary,
			LIST *attribute_name_list,
			int row )
{
	DICTIONARY *return_dictionary;
	char *attribute_name;
	char key[ 128 ];
	char *data;

	if ( !row_dictionary ) return (DICTIONARY *)0;
	if ( !list_rewind( attribute_name_list ) ) return (DICTIONARY *)0;
	if ( !row ) return (DICTIONARY *)0;

	return_dictionary = dictionary_small();

	do {
		attribute_name = list_get( attribute_name_list );

		sprintf( key, "%s_%d", attribute_name, row );

		if ( ( data = dictionary_get( key, row_dictionary ) ) )
		{
			dictionary_set(
				return_dictionary,
				attribute_name,
				data );
		}
	} while( list_next( attribute_name_list ) );

	return return_dictionary;
}

char *dictionary_separate_escaped_dictionary_string(
			DICTIONARY_SEPARATE *dictionary_separate,
			boolean with_non_prefixed_dictionary )
{
	return dictionary_separate_escaped_send_dictionary_string(
				dictionary_separate,
				with_non_prefixed_dictionary );
}

char *dictionary_separate_escaped_send_dictionary_string(
				DICTIONARY_SEPARATE *dictionary_separate,
				boolean with_non_prefixed_dictionary )
{
	char escaped_dictionary_string[ STRING_INPUT_BUFFER ];
	DICTIONARY *non_prefixed_dictionary = (DICTIONARY *)0;

	if ( with_non_prefixed_dictionary )
	{
		non_prefixed_dictionary =
			dictionary_separate->
				non_prefixed_dictionary;
	}

	dictionary_separate->send_dictionary =
		dictionary_separate_send_dictionary(
			dictionary_separate->sort_dictionary,
			dictionary_separate->query_dictionary,
			dictionary_separate->drillthru_dictionary,
			dictionary_separate->ignore_dictionary,
			dictionary_separate->pair_one2m_dictionary,
			non_prefixed_dictionary );

	escape_character(
			escaped_dictionary_string,
			dictionary_display_delimited(
				dictionary_separate->send_dictionary,
				'&' ),
			'$' );

	return strdup( escaped_dictionary_string );
}

DICTIONARY *dictionary_separate_non_prefixed(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char *data;

	key_list = dictionary_key_list( working_post_dictionary );

	if ( !list_rewind( key_list ) ) return (DICTIONARY *)0;

	dictionary = dictionary_large();

	do {
		key = list_get( key_list );

		if ( string_strncmp(
				key,
				DICTIONARY_SEPARATE_SORT_PREFIX ) == 0
		||   string_strncmp(
				key,
				DICTIONARY_SEPARATE_QUERY_PREFIX ) == 0
		||   string_strncmp(
				key,
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX ) == 0
		||   string_strncmp(
				key,
				IGNORE_SELECT_PUSH_BUTTON_PREFIX ) == 0
		||   string_strncmp(
				key,
				DICTIONARY_SEPARATE_PAIR_ONE2M_PREFIX ) == 0
		||   string_strncmp(
				key,
				NO_DISPLAY_PUSH_BUTTON_PREFIX ) == 0 )
		{
			continue;
		}

		data =
			dictionary_get(
				key,
				working_post_dictionary );

		dictionary_set( 
			dictionary,
			key,
			data );

	} while( list_next( key_list ) );

	return dictionary;
}

void dictionary_separate_output_as_hidden(
			DICTIONARY_SEPARATE *dictionary_separate,
			boolean with_non_prefixed_dictionary )
{
	if ( dictionary_length( dictionary_separate->drillthru_dictionary ) )
	{
		dictionary_output_as_hidden(
			dictionary_prepend_key(
				dictionary_separate->drillthru_dictionary,
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX ) );
	}

	if ( dictionary_length( dictionary_separate->query_dictionary ) )
	{
		dictionary_output_as_hidden(
			dictionary_prepend_key(
				dictionary_separate->
					query_dictionary,
				DICTIONARY_SEPARATE_QUERY_PREFIX ) );
	}

	if ( dictionary_length( dictionary_separate->ignore_dictionary ) )
	{
		dictionary_output_as_hidden(
			dictionary_prepend_key(
				dictionary_separate->
					ignore_dictionary,
				IGNORE_SELECT_PUSH_BUTTON_PREFIX ) );
	}

	if ( dictionary_length( dictionary_separate->pair_one2m_dictionary ) )
	{
		dictionary_output_as_hidden(
			dictionary_prepend_key(
				dictionary_separate->
					pair_one2m_dictionary,
				DICTIONARY_SEPARATE_PAIR_ONE2M_PREFIX ) );
	}

	if ( with_non_prefixed_dictionary
	&&   dictionary_length( dictionary_separate->
					non_prefixed_dictionary ) )
	{
		dictionary_output_as_hidden(
				dictionary_separate->
					non_prefixed_dictionary );
	}

}

LIST *dictionary_separate_posted_attribute_name_list(
			DICTIONARY *query_dictionary, 
			LIST *attribute_name_list )
{
	LIST *return_list = list_new_list();
	char *attribute_name;
	char key[ 128 ];

	if ( !list_reset( attribute_name_list ) )
		return return_list;

	do {
		attribute_name =
			list_get( attribute_name_list );

		if ( dictionary_populated_key_exists_index_zero(
				query_dictionary,
				attribute_name )
		||   dictionary_populated_key_exists_index_one(
				query_dictionary,
				attribute_name ) )
		{
			list_append_pointer( 	return_list, 
						attribute_name );
		}
		else
		{
			sprintf( key,
				 "%s%s",
				 FROM_PREFIX,
				 attribute_name );
	
			if ( dictionary_populated_key_exists_index_zero(
					query_dictionary,
					key )
			||   dictionary_populated_key_exists_index_one(
					query_dictionary,
					key ) )
			{
				list_set(
					return_list, 
					attribute_name );
			}
		}

	} while( list_next( attribute_name_list ) );
	return return_list;
}

void dictionary_separate_remove_from_starting_label(
			DICTIONARY *dictionary )
{
	dictionary_add_elements_by_removing_prefix(
		    dictionary,
		    FROM_PREFIX );

}

void dictionary_separate_set_primary_data_list_string(
		DICTIONARY *row_dictionary,
		char *primary_data_list_string,
		LIST *primary_key_list,
		char delimiter )
{
	int highest_index;
	int index;
	LIST *primary_data_list;
	char *primary_key;
	char *primary_data;
	char key[ 128 ];

	if ( !list_length( primary_key_list) ) return;

	primary_data_list =
		list_string2list(
			primary_data_list_string,
			delimiter );

	if ( !list_length( primary_data_list) ) return;

	highest_index =
		dictionary_key_highest_index(
			row_dictionary );

	for( index = 1; index <= highest_index; index++ )
	{
		list_rewind( primary_key_list );
		list_rewind( primary_data_list );

		do {
			primary_data = list_get( primary_data_list );
			primary_key = list_get( primary_key_list );

			sprintf( key, "%s_%d", primary_key, index );

			dictionary_set(
					row_dictionary,
					strdup( key ),
					primary_data );

			if ( !list_next( primary_data_list ) ) break;

		} while( list_next( primary_key_list ) );

	}
}

LIST *dictionary_separate_ignore_select_attribute_name_list(
			DICTIONARY *ignore_dictionary )
{
	LIST *ignore_select_attribute_name_list;
	DICTIONARY *remove_index_zero_dictionary;

	remove_index_zero_dictionary =
		dictionary_remove_index_zero(
			ignore_dictionary );

	ignore_select_attribute_name_list =
		dictionary_extract_and_remove_prefixed_key_list(
			remove_index_zero_dictionary,
			IGNORE_SELECT_PUSH_BUTTON_PREFIX );

	list_set_list(
		ignore_select_attribute_name_list,
		dictionary_extract_and_remove_prefixed_key_list(
			remove_index_zero_dictionary,
			NO_DISPLAY_PUSH_BUTTON_PREFIX ) );

	return ignore_select_attribute_name_list;
}

void dictionary_separate_dictionary_date_convert(
			DICTIONARY *dictionary,
			char *application_name,
			LIST *folder_attribute_date_name_list,
			char *login_name )
{
	char *attribute_name;
	char key[ 128 ];
	char *date_string;
	int index;
	int highest_index;

	if ( !list_length( folder_attribute_date_name_list ) ) return;

	highest_index = dictionary_key_highest_index( dictionary );

	for( index = 0; index <= highest_index; index++ )
	{
		list_rewind( folder_attribute_date_name_list );

		do {
			attribute_name =
				list_get(
					folder_attribute_date_name_list );

			if ( index == 0 )
			{
				/* Do without prefix and suffix */
				/* ---------------------------- */
				strcpy(	key, attribute_name );

				if ( ( date_string =
					dictionary_get(
						key,
						dictionary ) ) )
				{
				     dictionary_separate_string_date_convert( 
					dictionary,
					application_name,
					date_string,
					key,
					login_name );
				}
			}

			/* Do without prefix */
			/* ----------------- */
			sprintf(key,
				"%s_%d",
				attribute_name,
				index );

			if ( ( date_string =
				dictionary_get(
					key,
					dictionary ) ) )
			{
				dictionary_separate_string_date_convert( 
					dictionary,
					application_name,
					date_string,
					key,
					login_name );
			}

			/* Do from date */
			/* ------------ */
			sprintf(key,
				"%s%s_%d",
				FROM_PREFIX,
				attribute_name,
				index );

			if ( ( date_string =
				dictionary_get(
					key,
					dictionary ) ) )
			{
				dictionary_separate_string_date_convert( 
					dictionary,
					application_name,
					date_string,
					key,
					login_name );
			}

			/* Do to date */
			/* ---------- */
			sprintf(key,
				"%s%s_%d",
				TO_PREFIX,
				attribute_name,
				index );
	
			if ( ( date_string =
				dictionary_get(
					key,
					dictionary ) ) )
			{
				dictionary_separate_string_date_convert( 
					dictionary,
					application_name,
					date_string,
					key,
					login_name );
			}

		} while( list_next( folder_attribute_date_name_list ) );

	} /* for each index */
}

void dictionary_separate_trim_multi_drop_down_index(
			DICTIONARY *working_post_dictionary,
			char multi_select_move_left_right_index_delimiter )
{
	char *ptr;
	char *data;
	char *key;
	LIST *key_list;
	char place_holder = multi_select_move_left_right_index_delimiter;

	key_list = dictionary_key_list( working_post_dictionary );

	if ( !list_rewind( key_list ) ) return;

	do {
		key = list_get( key_list );

		data = dictionary_data( working_post_dictionary, key );

		if ( !strlen( data ) ) continue;

		ptr = data + strlen( data ) - 1;

		while( ptr > data )
		{
			if ( isdigit( *ptr ) )
			{
				ptr--;
			}
			else
			if ( *ptr == place_holder )
			{
				*ptr = '\0';
				break;
			}
			else
			{
				break;
			}
		}
	} while( list_next( key_list ) );
}

void dictionary_separate_string_date_convert(
			DICTIONARY *dictionary,
			char *application_name,
			char *date_string,
			char *key,
			char *login_name )
{
	DATE_CONVERT *date_convert;
	char destination[ 1024 ];
	char date_piece_string[ 64 ];
	char date_half[ 32 ];
	char time_half[ 32 ];
	char *destination_ptr = destination;
	int i;
	boolean exists_time;

	if ( !date_string || !*date_string ) return;

	*destination = '\0';

	for( i = 0; piece( date_piece_string, ',', date_string, i ); i++ )
	{
		if ( string_character_count( ' ', date_piece_string ) == 0 )
		{
			strcpy( date_half, date_piece_string );
			exists_time = 0;
		}
		else
		if ( string_character_count( ' ', date_piece_string ) == 1 )
		{
			column( date_half, 0, date_piece_string );
			column( time_half, 1, date_piece_string );
			exists_time = 1;
		}
		else
		{
			continue;
		}

		date_convert =
			date_convert_international(
				application_name,
				login_name,
				date_half );

		if ( !date_convert )
		{
			fprintf( stderr,
			 "Error in %s/%s(): cannot fetch from database\n",
		 		__FILE__,
		 		__FUNCTION__ );
			exit( 1 );
		}

		if ( i ) destination_ptr += sprintf( destination_ptr, "," );

		if ( exists_time )
		{
			destination_ptr +=
				sprintf( destination_ptr,
					 "%s %s",
					 date_convert->return_date,
					 time_half );
		}
		else
		{
			destination_ptr +=
				sprintf( destination_ptr,
					 "%s",
					 date_convert->return_date );
		}

		date_convert_free( date_convert );

	} /* for each comma delimited date */

	dictionary_set(
		dictionary,
		key,
		strdup( destination ) );
}

void dictionary_separate_trim_double_bracked_data(
			DICTIONARY *working_post_dictionary )
{
	dictionary_trim_double_bracked_data( working_post_dictionary );
}

DICTIONARY *dictionary_separate_send_dictionary(
			DICTIONARY *sort_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *pair_one2m_dictionary,
			DICTIONARY *non_prefixed_dictionary )
{
	DICTIONARY *send_dictionary = dictionary_large();

	if ( dictionary_length( sort_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				sort_dictionary,
				DICTIONARY_SEPARATE_SORT_PREFIX ) );
	}

	if ( dictionary_length( query_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				query_dictionary,
				DICTIONARY_SEPARATE_QUERY_PREFIX ) );
	}

	if ( dictionary_length( drillthru_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				drillthru_dictionary,
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX ) );
	}

	if ( dictionary_length( ignore_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				ignore_dictionary,
				IGNORE_SELECT_PUSH_BUTTON_PREFIX ) );
	}

	if ( dictionary_length( pair_one2m_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				pair_one2m_dictionary,
				DICTIONARY_SEPARATE_PAIR_ONE2M_PREFIX ) );
	}

	if ( dictionary_length( non_prefixed_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			non_prefixed_dictionary );
	}

	/* ------------------------- */
	/* Forbidden characters: `\" */
	/* ------------------------- */
	dictionary_search_replace_special_characters( send_dictionary );

	return send_dictionary;
}

char *dictionary_separate_send_string(
			DICTIONARY *send_dictionary )
{
	char escaped_dictionary_string[ STRING_INPUT_BUFFER ];

	escape_character(
		escaped_dictionary_string,
		dictionary_display_delimiter(
			send_dictionary,
			'&' ),
		'$' );

	return strdup( escaped_dictionary_string );
}

char *dictionary_separate_hidden_html(
			char *prefix,
			DICTIONARY *dictionary )
{
	char html[ 65536 ];
	char *ptr = html;
	LIST *key_list;
	char *key;
	char *data;
	char input_name[ 128 ];

	key_list = dictionary_key_list( dictionary );

	if ( !list_rewind( key_list ) ) return (char *)0;

	do {
		key = list_get( key_list );
		data = dictionary_get( key, dictionary );

		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		sprintf(input_name,
			"%s%s",
			prefix,
			key );

		ptr += sprintf(
			ptr,
			"<input name=\"%s\" type=hidden value=\"%s\">",
			input_name,
			data );

	} while ( list_next( key_list ) );

	return strdup( html );
}
