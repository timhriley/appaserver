/* $APPASERVER_HOME/library/dictionary_appaserver.c	   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary_appaserver.h"
#include "timlib.h"
#include "piece.h"
#include "element.h"

DICTIONARY_APPASERVER *dictionary_appaserver_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *attribute_list,
			LIST *operation_name_list,
			char *login_name )
{
	DICTIONARY_APPASERVER *dictionary_appaserver;

	if ( ! ( dictionary_appaserver =
			calloc( 1, sizeof( DICTIONARY_APPASERVER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	dictionary_appaserver->original_post_dictionary =
		original_post_dictionary;

	dictionary_appaserver->application_name = application_name;
	dictionary_appaserver->attribute_list = attribute_list;
	dictionary_appaserver->operation_list = operation_list;
	dictionary_appaserver->login_name = login_name;

	if ( !dictionary_length( original_post_dictionary ) )
		return dictionary_appaserver;

	dictionary_appaserver->working_post_dictionary =
		dictionary_appaserver_working_post(
			original_post_dictionary,
			application_name,
			attribute_list,
			login_name );

	dictionary_appaserver->sort_dictionary =
		dictionary_appaserver_sort(
			dictionary_appaserver->
				working_post_dictionary );

	dictionary_appaserver->query_dictionary =
		dictionary_appaserver_query(
			dictionary_appaserver->
				working_post_dictionary );

	dictionary_appaserver->drilldown_dictionary =
		dictionary_appaserver_drilldown(
			dictionary_appaserver->
				working_post_dictionary );

	dictionary_appaserver->ignore_dictionary =
		dictionary_appaserver_ignore_dictionary(
			dictionary_appaserver->
				working_post_dictionary );

	dictionary_appaserver->ignore_select_attribute_name_list =
		dictionary_appaserver_ignore_select_attribute_name_list(
			dictionary_appaserver->ignore_dictionary );

	dictionary_appaserver->pair_one2m_dictionary =
		dictionary_appaserver_pair_one2m_dictionary(
			dictionary_appaserver->
				working_post_dictionary );

	dictionary_appaserver->non_prefixed_dictionary =
		dictionary_appaserver_non_prefixed_dictionary(
			dictionary_appaserver->
				working_post_dictionary,
			attribute_list,
			application_name );

	dictionary_appaserver->row_dictionary =
		dictionary_appaserver_row_dictionary_multi_row(
			dictionary_appaserver->
				non_prefixed_dictionary,
			attribute_name_list(
				attribute_list ),
			operation_name_list );

	return dictionary_appaserver;
}

DICTIONARY *dictionary_appaserver_send(
			DICTIONARY *sort_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *drilldown_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *pair_one2m_dictionary,
			DICTIONARY *non_prefixed_dictionary )
{
	DICTIONARY *send_dictionary;

	send_dictionary = dictionary_large_new();

	if ( dictionary_length( sort_dictionary ) )
	{
		dictionary_append(
			send_dictionary,
			dictionary_prefix(
				sort_dictionary,
				SORT_BUTTON_PREFIX ) );
	}

	if ( dictionary_length( query_dictionary ) )
	{
		dictionary_append(
			send_dictionary,
			dictionary_prefix(
				query_dictionary,
				QUERY_PREFIX ) );
	}

	if ( dictionary_length( drilldown_dictionary ) )
	{
		dictionary_append(
			send_dictionary,
			dictionary_prefix(
				drilldown_dictionary,
				DRILLDOWN_PREFIX ) );
	}

	if ( dictionary_length( ignore_dictionary ) )
	{
		dictionary_append(
			send_dictionary,
			dictionary_prefix(
				ignore_dictionary,
				IGNORE_SELECT_PUSH_BUTTON_PREFIX ) );
	}

	if ( dictionary_length( pair_one2m_dictionary ) )
	{
		dictionary_append(
			send_dictionary,
			dictionary_prefix(
				pair_one2m_dictionary,
				PAIR_ONE2M_PREFIX ) );
	}

	if ( dictionary_length( non_prefixed_dictionary ) )
	{
		dictionary_append(
			send_dictionary,
			non_prefixed_dictionary );
	}

	/* ------------------------- */
	/* Forbidden characters: `\" */
	/* ------------------------- */
	dictionary_search_replace_special_characters( send_dictionary );

	return send_dictionary;
}

DICTIONARY *dictionary_appaserver_working_post(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *attribute_list,
			char *login_name )
{
	DICTIONARY *working_post_dictionary;

	working_post_dictionary = dictionary_copy( original_post_dictionary );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		QUERY_PREFIX );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		DRILLDOWN_PREFIX );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		SORT_BUTTON_PREFIX );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		PAIR_ONE2M_PREFIX );

	/* Remove the index appended to the multi-select lookup widget. */
	/* ------------------------------------------------------------ */
	dictionary_appaserver_trim_multi_drop_down_index(
		working_post_dictionary,
		ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	if ( attribute_list )
	{
		dictionary_appaserver_dictionary_convert_date(
			working_post_dictionary,
			application_name,
			attribute_list,
			login_name );
	}

	/* Trim the [common_non_primary_attribute data] 		*/
	/* Note: this must follow					*/
	/* 	 dictionary_appaserver_trim_multi_drop_down_index()	*/
	/* ------------------------------------------------------------ */
	dictionary_trim_double_bracked_data( working_post_dictionary );

	return working_post_dictionary;
}

DICTIONARY *dictionary_appaserver_sort_dictionary(
			DICTIONARY *working_post_dictionary )
{
	return dictionary_appaserver_prefixed_dictionary(
		working_post_dictionary,
		SORT_BUTTON_PREFIX );

}

DICTIONARY *dictionary_appaserver_prefixed_dictionary(
			DICTIONARY *post_dictionary,
			char *prefix )
{
	DICTIONARY *return_dictionary;

	if ( ! ( return_dictionary =
		dictionary_without_prefix(
			post_dictionary,
			prefix ) ) )
	{
		return_dictionary = dictionary_small_new();
	}

	return return_dictionary;
}

DICTIONARY *dictionary_appaserver_preprompt_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
			dictionary_without_prefix(
				working_post_dictionary,
				PREPROMPT_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	dictionary_add_suffix_zero( dictionary );

	dictionary_appaserver_remove_from_starting_label( dictionary );

	return dictionary;

}

DICTIONARY *dictionary_appaserver_ignore_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_without_prefix(
			working_post_dictionary,
			IGNORE_SELECT_PUSH_BUTTON_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	dictionary_append_dictionary(
		dictionary,
		dictionary_without_prefix(
			working_post_dictionary,
			NO_DISPLAY_PUSH_BUTTON_PREFIX ) );

	dictionary_appaserver_parse_multi_attribute_keys(
		dictionary,
		(char *)0 /* prefix */ );

	return dictionary;
}

DICTIONARY *dictionary_appaserver_pair_one2m_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_without_prefix(
			working_post_dictionary,
			PAIR_ONE2M_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	return dictionary;
}

DICTIONARY *dictionary_appaserver_lookup_before_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_without_prefix(
			working_post_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	return dictionary;
}

DICTIONARY *dictionary_appaserver_query_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
			dictionary_without_prefix(
				working_post_dictionary,
				QUERY_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	dictionary_add_suffix_zero( dictionary );

	dictionary_appaserver_remove_from_starting_label( dictionary );

	dictionary_appaserver_parse_multi_attribute_keys(
		dictionary,
		RELATION_OPERATOR_PREFIX );

	return dictionary;

}

void dictionary_appaserver_parse_multi_attribute_keys(
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
		MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
		prefix,
		1 /* dont_include_relational_operators */ );

	dictionary_parse_multi_attribute_keys(
		source_destination, 
		MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
		MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
		prefix,
		1 /* dont_include_relational_operators */ );
}

DICTIONARY *dictionary_appaserver_row_dictionary_row_zero(
			DICTIONARY *working_post_dictionary,
			LIST *attribute_name_list )
{
	char *attribute_name;
	char key[ 256 ];
	char *dictionary_row;
	DICTIONARY *row_dictionary;

	if ( !list_length( attribute_name_list ) ) return (DICTIONARY *)0;

	row_dictionary = dictionary_small_new();

	list_rewind( attribute_name_list );
	do {
		attribute_name =
			list_get( attribute_name_list );

		sprintf(	key,
				"%s_0",
				attribute_name );

		dictionary_row =
			dictionary_fetch(
				key,
				working_post_dictionary );

		if ( dictionary_row )
		{
			dictionary_set_pointer(
				row_dictionary,
				strdup( key ),
				dictionary_row );
		}

	} while( list_next( attribute_name_list ) );

	return row_dictionary;
}

DICTIONARY *dictionary_appaserver_row_dictionary_multi_row(
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
		dictionary_highest_index(
			non_prefixed_dictionary );

	row_dictionary = dictionary_large_new();

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
				dictionary_fetch(
					key,
					non_prefixed_dictionary );

			if ( data )
			{
				dictionary_set_pointer(
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
				dictionary_fetch(
					key,
					non_prefixed_dictionary );

			if ( data )
			{
				dictionary_set_pointer(
					row_dictionary,
					strdup( key ),
					data );
			}
		} while( list_next( operation_name_list ) );

	} /* for each index */

	return row_dictionary;
}

DICTIONARY *dictionary_appaserver_row_dictionary_row(
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

	return_dictionary = dictionary_small_new();

	do {
		attribute_name = list_get( attribute_name_list );

		sprintf( key, "%s_%d", attribute_name, row );

		if ( ( data = dictionary_fetch( key, row_dictionary ) ) )
		{
			dictionary_set_pointer(
				return_dictionary,
				attribute_name,
				data );
		}
	} while( list_next( attribute_name_list ) );

	return return_dictionary;

}

char *dictionary_appaserver_escaped_dictionary_string(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean with_non_prefixed_dictionary )
{
	return dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				with_non_prefixed_dictionary );
}

char *dictionary_appaserver_escaped_send_dictionary_string(
				DICTIONARY_APPASERVER *dictionary_appaserver,
				boolean with_non_prefixed_dictionary )
{
	char escaped_dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *non_prefixed_dictionary = (DICTIONARY *)0;

	if ( with_non_prefixed_dictionary )
	{
		non_prefixed_dictionary =
			dictionary_appaserver->
				non_prefixed_dictionary;
	}

	dictionary_appaserver->send_dictionary =
		dictionary_appaserver_send_dictionary(
			dictionary_appaserver->sort_dictionary,
			dictionary_appaserver->query_dictionary,
			dictionary_appaserver->preprompt_dictionary,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			dictionary_appaserver->ignore_dictionary,
			dictionary_appaserver->pair_one2m_dictionary,
			non_prefixed_dictionary );

	escape_character(
			escaped_dictionary_string,
			dictionary_display_delimited(
				dictionary_appaserver->send_dictionary,
				'&' ),
			'$' );

	return strdup( escaped_dictionary_string );

}

DICTIONARY *dictionary_appaserver_non_prefixed_dictionary(
			DICTIONARY *working_post_dictionary,
			LIST *attribute_list,
			char *application_name )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char *data;
	int strlen_sort;
	int strlen_lookup;
	int strlen_query;
	int strlen_preprompt;
	int strlen_ignore;
	int strlen_pair_1tom;
	int strlen_no_display;

	key_list = dictionary_key_list( working_post_dictionary );
	if ( !list_rewind( key_list ) ) return (DICTIONARY *)0;

	dictionary = dictionary_large_new();

	strlen_sort = strlen( SORT_BUTTON_PREFIX );
	strlen_lookup = strlen( LOOKUP_BEFORE_DROP_DOWN_PREFIX );
	strlen_query = strlen( QUERY_PREFIX );
	strlen_preprompt = strlen( PREPROMPT_PREFIX );
	strlen_ignore = strlen( IGNORE_SELECT_PUSH_BUTTON_PREFIX );
	strlen_pair_1tom = strlen( PAIR_ONE2M_PREFIX );
	strlen_no_display = strlen( NO_DISPLAY_PUSH_BUTTON_PREFIX );

	do {
		key = list_get( key_list );

		if ( strncmp(	key,
				SORT_BUTTON_PREFIX,
				strlen_sort ) == 0
		||   strncmp(	key,
				PREPROMPT_PREFIX,
				strlen_preprompt ) == 0
		||   strncmp(	key,
				LOOKUP_BEFORE_DROP_DOWN_PREFIX,
				strlen_lookup ) == 0
		||   strncmp(	key,
				QUERY_PREFIX,
				strlen_query ) == 0
		||   strncmp(	key,
				IGNORE_SELECT_PUSH_BUTTON_PREFIX,
				strlen_ignore ) == 0
		||   strncmp(	key,
				PAIR_ONE2M_PREFIX,
				strlen_pair_1tom ) == 0
		||   strncmp(	key,
				NO_DISPLAY_PUSH_BUTTON_PREFIX,
				strlen_no_display ) == 0 )
		{
			continue;
		}

		data =
			dictionary_get(
				working_post_dictionary, 
				key );

		dictionary_set_pointer( 
			dictionary,
			key,
			data );

	} while( list_next( key_list ) );

	dictionary_appaserver_parse_multi_attribute_keys(
		dictionary,
		(char *)0 /* prefix */ );

	dictionary_appaserver_remove_from_starting_label(
		dictionary );

	dictionary_add_elements_by_removing_index_zero(
	    	dictionary );

	if ( attribute_list )
	{
		dictionary_appaserver_post_dictionary_database_convert_dates(
				dictionary,
				application_name,
				attribute_list );
	}

	return dictionary;

}

void dictionary_appaserver_output_as_hidden(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean with_non_prefixed_dictionary )
{
	if ( dictionary_length( dictionary_appaserver->preprompt_dictionary ) )
	{
		dictionary_appaserver_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->preprompt_dictionary,
				PREPROMPT_PREFIX ) );
	}

	if ( dictionary_length( dictionary_appaserver->
					lookup_before_drop_down_dictionary ) )
	{
		dictionary_appaserver_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					lookup_before_drop_down_dictionary,
				LOOKUP_BEFORE_DROP_DOWN_PREFIX ) );
	}

	if ( dictionary_length( dictionary_appaserver->query_dictionary ) )
	{
		dictionary_appaserver_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					query_dictionary,
				QUERY_PREFIX ) );
	}

	if ( dictionary_length( dictionary_appaserver->ignore_dictionary ) )
	{
		dictionary_appaserver_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					ignore_dictionary,
				IGNORE_SELECT_PUSH_BUTTON_PREFIX ) );
	}

	if ( dictionary_length( dictionary_appaserver->pair_one2m_dictionary ) )
	{
		dictionary_appaserver_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					pair_one2m_dictionary,
				PAIR_ONE2M_PREFIX ) );
	}

/*
Can't output the sort dictionary.
	if ( dictionary_length( dictionary_appaserver->sort_dictionary ) )
	{
		dictionary_appaserver_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					sort_dictionary,
				SORT_BUTTON_PREFIX ) );
	}
*/

	if ( with_non_prefixed_dictionary
	&&   dictionary_length( dictionary_appaserver->
					non_prefixed_dictionary ) )
	{
		dictionary_appaserver_output_dictionary_as_hidden(
				dictionary_appaserver->
					non_prefixed_dictionary );
	}

}

LIST *dictionary_appaserver_posted_attribute_name_list(
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

void dictionary_appaserver_remove_from_starting_label(
			DICTIONARY *dictionary )
{
	dictionary_add_elements_by_removing_prefix(
		    dictionary,
		    FROM_PREFIX );

}

void dictionary_appaserver_set_primary_data_list_string(
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
		dictionary_highest_index(
			row_dictionary );

	for( index = 1; index <= highest_index; index++ )
	{
		list_rewind( primary_key_list );
		list_rewind( primary_data_list );

		do {
			primary_data = list_get( primary_data_list );
			primary_key = list_get( primary_key_list );

			sprintf( key, "%s_%d", primary_key, index );

			dictionary_set_pointer(
					row_dictionary,
					strdup( key ),
					primary_data );

			if ( !list_next( primary_data_list ) ) break;

		} while( list_next( primary_key_list ) );

	}
}

LIST *dictionary_appaserver_ignore_select_attribute_name_list(
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

void dictionary_appaserver_dictionary_convert_date(
			DICTIONARY *post_dictionary,
			char *application_name,
			LIST *attribute_list,
			char *login_name )
{
	LIST *date_attribute_name_list;
	char *attribute_name;
	char key[ 128 ];
	char *date_string;
	int index;
	int highest_index;

	date_attribute_name_list =
		attribute_date_attribute_name_list(
			attribute_list );

	if ( !list_length( date_attribute_name_list ) ) return;

	highest_index = dictionary_highest_index( post_dictionary );

	for( index = 0; index <= highest_index; index++ )
	{
		list_rewind( date_attribute_name_list );

		do {
			attribute_name =
				list_get(
					date_attribute_name_list );

			if ( index == 0 )
			{
				/* Do without prefix and suffix */
				/* ---------------------------- */
				strcpy(	key, attribute_name );

				date_string =
					dictionary_fetch(
						key,
						post_dictionary );

				dictionary_appaserver_date_string_convert_date( 
					post_dictionary,
					application_name,
					date_string,
					key,
					login_name );
			}

			/* Do without prefix */
			/* ----------------- */
			sprintf(	key,
					"%s_%d",
					attribute_name,
					index );

			date_string =
				dictionary_fetch(
					key,
					post_dictionary );

			dictionary_appaserver_date_string_convert_date( 
				post_dictionary,
				application_name,
				date_string,
				key,
				login_name );

			/* Do from date */
			/* ------------ */
			sprintf(	key,
					"%s%s_%d",
					FROM_PREFIX,
					attribute_name,
					index );

			date_string =
				dictionary_fetch(
					key,
					post_dictionary );

			dictionary_appaserver_date_string_convert_date( 
				post_dictionary,
				application_name,
				date_string,
				key,
				login_name );

			/* Do to date */
			/* ---------- */
			sprintf(	key,
					"%s%s_%d",
					TO_PREFIX,
					attribute_name,
					index );
	
			date_string =
				dictionary_fetch(
					key,
					post_dictionary );

			dictionary_appaserver_date_string_convert_date( 
				post_dictionary,
				application_name,
				date_string,
				key,
				login_name );

		} while( list_next( date_attribute_name_list ) );

	} /* for each index */
}

void dictionary_appaserver_trim_multi_drop_down_index(
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

void dictionary_appaserver_date_string_convert_date(
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
		if ( timlib_character_count( ' ', date_piece_string ) == 0 )
		{
			strcpy( date_half, date_piece_string );
			exists_time = 0;
		}
		else
		if ( timlib_character_count( ' ', date_piece_string ) == 1 )
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
			date_convert_database(
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

	dictionary_set_pointer(
		dictionary,
		key,
		strdup( destination ) );
}
