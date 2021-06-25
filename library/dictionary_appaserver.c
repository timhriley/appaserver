/* library/dictionary_appaserver.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary_appaserver.h"
#include "timlib.h"
#include "piece.h"
#include "query.h"

DICTIONARY_APPASERVER *dictionary_appaserver_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *attribute_list,
			LIST *operation_name_list )
{
	DICTIONARY_APPASERVER *dictionary_appaserver;

	if ( ! ( dictionary_appaserver =
		(DICTIONARY_APPASERVER *)
			calloc( 1, sizeof( DICTIONARY_APPASERVER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocation memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (DICTIONARY_APPASERVER *)0;
	}

	if ( !dictionary_length( original_post_dictionary ) )
	{
		dictionary_appaserver->working_post_dictionary =
			dictionary_small_new();

		dictionary_appaserver->sort_dictionary =
			dictionary_small_new();

		dictionary_appaserver->query_dictionary =
			dictionary_small_new();

		dictionary_appaserver->preprompt_dictionary =
			dictionary_small_new();

		dictionary_appaserver->ignore_dictionary =
			dictionary_small_new();

		dictionary_appaserver->pair_one2m_dictionary =
			dictionary_small_new();

		/* ------------------------------------------------------- */
		/* Don't initialize the lookup_before_drop_down_dictionary */
		/* ------------------------------------------------------- */
	}
	else
	{
		dictionary_appaserver->original_post_dictionary =
			original_post_dictionary;

		dictionary_appaserver->working_post_dictionary =
			dictionary_appaserver_get_working_post_dictionary(
				dictionary_appaserver->
					original_post_dictionary,
					application_name,
					attribute_list );

		dictionary_appaserver->sort_dictionary =
			dictionary_appaserver_get_sort_dictionary(
				dictionary_appaserver->
					working_post_dictionary );

		dictionary_appaserver->query_dictionary =
			dictionary_appaserver_get_query_dictionary(
				dictionary_appaserver->
					working_post_dictionary );

		dictionary_appaserver->preprompt_dictionary =
			dictionary_appaserver_get_preprompt_dictionary(
				dictionary_appaserver->
					working_post_dictionary );

		dictionary_appaserver->lookup_before_drop_down_dictionary =
			dictionary_appaserver_get_lookup_before_dictionary(
				dictionary_appaserver->
					working_post_dictionary );

		/* ------------------------------------------------------- */
		/* Don't initialize the lookup_before_drop_down_dictionary */
		/* ------------------------------------------------------- */
		if ( !dictionary_length(
			dictionary_appaserver->
				lookup_before_drop_down_dictionary ) )
		{
			dictionary_appaserver->
				lookup_before_drop_down_dictionary =
					(DICTIONARY *)0;
		}

		dictionary_appaserver->ignore_dictionary =
			dictionary_appaserver_get_ignore_dictionary(
				dictionary_appaserver->
					working_post_dictionary );

		dictionary_appaserver->pair_one2m_dictionary =
			dictionary_appaserver_get_pair_one2m_dictionary(
				dictionary_appaserver->
					working_post_dictionary );

		dictionary_appaserver->non_prefixed_dictionary =
			dictionary_appaserver_get_non_prefixed_dictionary(
				dictionary_appaserver->
					working_post_dictionary,
				attribute_list,
				application_name );

		dictionary_appaserver->row_dictionary =
			dictionary_appaserver_get_row_dictionary_multi_row(
				dictionary_appaserver->
					non_prefixed_dictionary,
				attribute_get_attribute_name_list(
					attribute_list ),
				operation_name_list );
	}

	return dictionary_appaserver;

}

DICTIONARY *dictionary_appaserver_get_send_dictionary(
			DICTIONARY *sort_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *lookup_before_drop_down_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *pair_one2m_dictionary,
			DICTIONARY *non_prefixed_dictionary )
{
	DICTIONARY *send_dictionary;

	send_dictionary = dictionary_large_new();

	if ( dictionary_length( sort_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_add_prefix(
				sort_dictionary,
				SORT_BUTTON_PREFIX ) );
	}

	if ( dictionary_length( query_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_add_prefix(
				query_dictionary,
				QUERY_STARTING_LABEL ) );
	}

	if ( dictionary_length( preprompt_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_add_prefix(
				preprompt_dictionary,
				PREPROMPT_PREFIX ) );
	}

	if ( dictionary_length( lookup_before_drop_down_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_add_prefix(
				lookup_before_drop_down_dictionary,
				LOOKUP_BEFORE_DROP_DOWN_PREFIX ) );
	}

	if ( dictionary_length( ignore_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_add_prefix(
				ignore_dictionary,
				IGNORE_PUSH_BUTTON_PREFIX ) );
	}

	if ( dictionary_length( pair_one2m_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_add_prefix(
				pair_one2m_dictionary,
				PAIR_ONE2M_PREFIX ) );
	}

	if ( dictionary_length( non_prefixed_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary, non_prefixed_dictionary );
	}

	/* ------------------------- */
	/* Forbidden characters: `\" */
	/* ------------------------- */
	dictionary_search_replace_special_characters( send_dictionary );

	return send_dictionary;

}

DICTIONARY *dictionary_appaserver_get_working_post_dictionary(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			LIST *attribute_list )
{
	DICTIONARY *working_post_dictionary;

	working_post_dictionary = dictionary_copy( original_post_dictionary );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		QUERY_STARTING_LABEL );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		PREPROMPT_PREFIX );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		SORT_BUTTON_PREFIX );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		LOOKUP_BEFORE_DROP_DOWN_PREFIX );

	dictionary_appaserver_parse_multi_attribute_keys(
		working_post_dictionary,
		PAIR_ONE2M_PREFIX );

	/* Remove the index appended to the multi-select lookup widget. */
	/* ------------------------------------------------------------ */
	dictionary_trim_multi_drop_down_index(
		working_post_dictionary,
		ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	if ( attribute_list )
	{
		appaserver_library_post_dictionary_database_convert_dates(
				working_post_dictionary,
				application_name,
				attribute_list );
	}

	/* Trim the [common_non_primary_attribute data] 		 */
	/* Note: this must follow dictionary_trim_multi_drop_down_index()*/
	/* ------------------------------------------------------------- */
	dictionary_trim_double_bracked_data( working_post_dictionary );

	return working_post_dictionary;

}

DICTIONARY *dictionary_appaserver_get_sort_dictionary(
			DICTIONARY *working_post_dictionary )
{
	return dictionary_appaserver_get_prefixed_dictionary(
			working_post_dictionary,
			SORT_BUTTON_PREFIX );

}

DICTIONARY *dictionary_appaserver_get_prefixed_dictionary(
			DICTIONARY *post_dictionary,
			char *prefix )
{
	DICTIONARY *return_dictionary;

	if ( ! ( return_dictionary =
		dictionary_get_without_prefix(
			post_dictionary,
			prefix ) ) )
	{
		return_dictionary = dictionary_small_new();
	}

	return return_dictionary;

}

DICTIONARY *dictionary_appaserver_get_preprompt_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
			dictionary_get_without_prefix(
				working_post_dictionary,
				PREPROMPT_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	dictionary_add_suffix_zero( dictionary );

	dictionary_appaserver_remove_from_starting_label( dictionary );

	return dictionary;

}

DICTIONARY *dictionary_appaserver_get_ignore_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_get_without_prefix(
			working_post_dictionary,
			IGNORE_PUSH_BUTTON_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	dictionary_append_dictionary(
		dictionary,
		dictionary_get_without_prefix(
			working_post_dictionary,
			NO_DISPLAY_PUSH_BUTTON_PREFIX ) );

	dictionary_appaserver_parse_multi_attribute_keys(
				dictionary,
				(char *)0 /* prefix */ );

	return dictionary;

}

DICTIONARY *dictionary_appaserver_get_pair_one2m_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_get_without_prefix(
			working_post_dictionary,
			PAIR_ONE2M_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	return dictionary;
}

DICTIONARY *dictionary_appaserver_get_lookup_before_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_get_without_prefix(
			working_post_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_PREFIX ) ) )
	{
		dictionary = dictionary_small_new();
	}

	return dictionary;
}

DICTIONARY *dictionary_appaserver_get_query_dictionary(
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
			dictionary_get_without_prefix(
				working_post_dictionary,
				QUERY_STARTING_LABEL ) ) )
	{
		dictionary = dictionary_small_new();
	}

	dictionary_add_suffix_zero( dictionary );

	dictionary_appaserver_remove_from_starting_label( dictionary );

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

DICTIONARY *dictionary_appaserver_get_row_dictionary_row_zero(
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
			list_get_pointer( attribute_name_list );

		sprintf(	key,
				"%s_0",
				attribute_name );

		dictionary_row = dictionary_fetch(
					working_post_dictionary,
					key );

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

DICTIONARY *dictionary_appaserver_get_row_dictionary_multi_row(
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
		dictionary_get_highest_index(
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
					non_prefixed_dictionary,
					key );

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
				list_get_pointer(
					operation_name_list );

			sprintf(key,
				"%s_%d",
				operation_name,
				index );

			data =
				dictionary_fetch(
					non_prefixed_dictionary,
					key );

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

DICTIONARY *dictionary_appaserver_get_row_dictionary_row(
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
		attribute_name = list_get_pointer( attribute_name_list );

		sprintf( key, "%s_%d", attribute_name, row );

		if ( ( data = dictionary_fetch( row_dictionary, key ) ) )
		{
			dictionary_set_pointer(
				return_dictionary,
				attribute_name,
				data );
		}
	} while( list_next( attribute_name_list ) );

	return return_dictionary;

}

char *dictionary_appaserver_get_escaped_dictionary_string(
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
		dictionary_appaserver_get_send_dictionary(
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

DICTIONARY *dictionary_appaserver_get_non_prefixed_dictionary(
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

	key_list = dictionary_get_key_list( working_post_dictionary );
	if ( !list_rewind( key_list ) ) return (DICTIONARY *)0;

	dictionary = dictionary_large_new();

	strlen_sort = strlen( SORT_BUTTON_PREFIX );
	strlen_lookup = strlen( LOOKUP_BEFORE_DROP_DOWN_PREFIX );
	strlen_query = strlen( QUERY_STARTING_LABEL );
	strlen_preprompt = strlen( PREPROMPT_PREFIX );
	strlen_ignore = strlen( IGNORE_PUSH_BUTTON_PREFIX );
	strlen_pair_1tom = strlen( PAIR_ONE2M_PREFIX );
	strlen_no_display = strlen( NO_DISPLAY_PUSH_BUTTON_PREFIX );

	do {
		key = list_get_pointer( key_list );

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
				QUERY_STARTING_LABEL,
				strlen_query ) == 0
		||   strncmp(	key,
				IGNORE_PUSH_BUTTON_PREFIX,
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

		data = dictionary_get( 	working_post_dictionary, 
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
		appaserver_library_post_dictionary_database_convert_dates(
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
		appaserver_library_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->preprompt_dictionary,
				PREPROMPT_PREFIX ) );
	}

	if ( dictionary_length( dictionary_appaserver->
					lookup_before_drop_down_dictionary ) )
	{
		appaserver_library_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					lookup_before_drop_down_dictionary,
				LOOKUP_BEFORE_DROP_DOWN_PREFIX ) );
	}

	if ( dictionary_length( dictionary_appaserver->query_dictionary ) )
	{
		appaserver_library_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					query_dictionary,
				QUERY_STARTING_LABEL ) );
	}

	if ( dictionary_length( dictionary_appaserver->ignore_dictionary ) )
	{
		appaserver_library_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					ignore_dictionary,
				IGNORE_PUSH_BUTTON_PREFIX ) );
	}

	if ( dictionary_length( dictionary_appaserver->pair_one2m_dictionary ) )
	{
		appaserver_library_output_dictionary_as_hidden(
			dictionary_prepend_key(
				dictionary_appaserver->
					pair_one2m_dictionary,
				PAIR_ONE2M_PREFIX ) );
	}

/*
Can't output the sort dictionary.
	if ( dictionary_length( dictionary_appaserver->sort_dictionary ) )
	{
		appaserver_library_output_dictionary_as_hidden(
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
		appaserver_library_output_dictionary_as_hidden(
				dictionary_appaserver->
					non_prefixed_dictionary );
	}

}

LIST *dictionary_appaserver_get_posted_attribute_name_list(
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
			list_get_pointer( attribute_name_list );

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
				 QUERY_FROM_STARTING_LABEL,
				 attribute_name );
	
			if ( dictionary_populated_key_exists_index_zero(
					query_dictionary,
					key )
			||   dictionary_populated_key_exists_index_one(
					query_dictionary,
					key ) )
			{
				list_append_pointer( 	return_list, 
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
		    	QUERY_FROM_STARTING_LABEL );

}

void dictionary_appaserver_set_primary_data_list_string(
		DICTIONARY *row_dictionary,
		char *primary_data_list_string,
		LIST *primary_attribute_name_list,
		char delimiter )
{
	int highest_index;
	int index;
	LIST *primary_data_list;
	char *primary_key;
	char *primary_data;
	char key[ 128 ];

	if ( !list_length( primary_attribute_name_list) ) return;

	primary_data_list =
		list_string2list(
			primary_data_list_string,
			delimiter );

	if ( !list_length( primary_data_list) ) return;

	highest_index =
		dictionary_get_highest_index(
			row_dictionary );

	for( index = 1; index <= highest_index; index++ )
	{
		list_rewind( primary_attribute_name_list );
		list_rewind( primary_data_list );

		do {
			primary_data = list_get( primary_data_list );
			primary_key = list_get( primary_attribute_name_list );

			sprintf( key, "%s_%d", primary_key, index );

			dictionary_set_pointer(
					row_dictionary,
					strdup( key ),
					primary_data );

			if ( !list_next( primary_data_list ) ) break;

		} while( list_next( primary_attribute_name_list ) );

	}
}

