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
#include "sql.h"
#include "appaserver_library.h"
#include "relation.h"
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
	dictionary_string_resolve( 
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

LIST *dictionary_separate_original_post_key_list(
			DICTIONARY *original_post_dictionary )
{
	return dictionary_key_list( original_post_dictionary );
}

void dictionary_separate_parse_multi(
			DICTIONARY *dictionary,
			LIST *relation_mto1_non_isa_list,
			char sql_delimiter )
{
	RELATION *relation;
	char *relation_key;
	char relation_key_prefix[ 128 ];
	char relation_key_row[ 128 ];
	char *relation_data;
	char *key;
	char *data;
	int highest_row;
	int row;

	if ( !list_rewind( relation_mto1_non_isa_list ) ) return;

	do {
		relation = list_get( relation_mto1_non_isa_list );

		if ( !list_length( relation->foreign_key_list ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ! ( relation_key =
				/* ------------------------ */
				/* Returns heap memory null */
				/* ------------------------ */
				list_delimited_string(
					relation->foreign_key_list,
					sql_delimiter ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: list_delimited_string() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		sprintf(relation_key_prefix,
			"%s_",
			relation_key );

		highest_row =
			dictionary_prefix_highest_row(
				relation_key_prefix,
				dictionary );

		if ( highest_row < 0 ) continue;

		for(	row = 0;
			row <= highest_row;
			row++ )
		{
			sprintf(relation_key_row,
				"%s_%d",
				relation_key );

			if ( ( relation_data =
					dictionary_get(
						relation_key_row,
						dictionary ) ) )
			{
here1
		list_rewind( key_list );

		do {
			key = list_get( key_list );

			if ( string_strncmp(
				key,
				relation_key ) == 0 )
			{
				dictionary_set(
					dictionary,
					key,
					dictionary_get(
						key,
						dictionary ) );
			}

		} while ( list_next( key_list ) );

	} while ( list_next( relation_mto1_non_isa_list ) );

	dictionary_parse_multi_attribute_keys(
		dictionary, 
		sql_delimiter );

	return dictionary;
}

DICTIONARY_SEPARATE *dictionary_separate_post_edit_table_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *relation_mto1_non_isa_list,
			LIST *folder_attribute_name_list,
			LIST *operation_name_list,
			LIST *folder_attribute_date_name_list )
{
	DICTIONARY_SEPARATE *dictionary_separate =
		dictionary_separate_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate;
	}

	dictionary_separate->parse_multi_attribute_dictionary =
		dictionary_separate_parse_multi_attribute_dictionary(
			original_post_dictionary,
			relation_mto1_non_isa_list,
			SQL_DELIMITER );

/* --------------------------------------------------------------------------- 
Two notes:
1) No memory gets allocated. Both the key and data still exist
   in the append_dictionary dictionary.
2) If a they share a key, then it doesn't clobber the source_destinatation.
--------------------------------------------------------------------------- */
	dictionary_append_dictionary(
		original_post_dictionary,
		dictionary_separate->parse_multi_attribute_dictionary );

	dictionary_separate->working_post_dictionary =
		dictionary_separate_working_post_new(
			original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	dictionary_separate->sort_dictionary =
		dictionary_separate_sort(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			dictionary_separate->
				working_post_dictionary->
				dictionary );

	dictionary_separate->query_dictionary =
		dictionary_separate_query(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->drillthru_dictionary =
		dictionary_separate_drillthru(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->ignore_dictionary =
		dictionary_separate_ignore(
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->ignore_select_attribute_name_list =
		dictionary_separate_ignore_select_attribute_name_list(
			dictionary_separate->ignore_dictionary );

	dictionary_separate->pair_one2m_dictionary =
		dictionary_separate_pair_one2m(
			DICTIONARY_SEPARATE_PAIR_ONE2M_PREFIX,
			dictionary_separate->
				working_post_dictionary );

	dictionary_separate->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			dictionary_separate->
				working_post_dictionary );

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

DICTIONARY_SEPARATE_TRIM_MULTI *
	dictionary_separate_trim_multi_calloc(
		void )
{
	DICTIONARY_SEPARATE_TRIM_MULTI *dictionary_separate_trim_multi;

	if ( ! ( dictionary_separate_trim_multi =
		   calloc( 1, sizeof( DICTIONARY_SEPARATE_TRIM_MULTI ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary_separate_trim_multi;
}

DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_calloc(
		void )
{
	DICTIONARY_SEPARATE_DATE_CONVERT *dictionary_separate_date_convert;

	if ( ! ( dictionary_separate_date_convert =
		   calloc( 1, sizeof( DICTIONARY_SEPARATE_DATE_CONVERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary_separate_date_convert;
}

DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list )
{
	DICTIONARY_SEPARATE_DATE_CONVERT *dictionary_separate_date_convert;

	if ( !dictionary_length( original_post_dictionary ) )
		return (DICTIONARY_SEPARATE_DATE_CONVERT *)0;

	dictionary_separate_date_convert =
		dictionary_separate_date_convert_calloc();

	dictionary_separate_date_convert->dictionary = original_post_dictionary;

	if ( list_length( folder_attribute_date_name_list ) )
	{
		dictionary_separate_date_convert(
			dictionary_separate_date_convert->dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			DICTIONARY_SEPARATE_FROM_PREFIX,
			DICTIONARY_SEPARATE_TO_PREFIX );
	}

	return dictionary_separate_date_convert;
}

DICTIONARY_SEPARATE_WORKING_POST *
	dictionary_separate_working_post_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list )
{
	DICTIONARY_SEPARATE_WORKING_POST *dictionary_separate_working_post;

	if ( !dictionary_length( original_post_dictionary ) )
		return (DICTIONARY_SEPARATE_WORKING_POST *)0;

	dictionary_separate_working_post =
		dictionary_separate_working_post_calloc();

	dictionary_separate_working_post->dictionary = original_post_dictionary;

	if ( list_length( folder_attribute_date_name_list ) )
	{
		dictionary_separate_working_post_date_convert(
			dictionary_separate_working_post->dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			DICTIONARY_SEPARATE_FROM_PREFIX,
			DICTIONARY_SEPARATE_TO_PREFIX );
	}

	/* Remove the index appended to the multi-select lookup widget. */
	/* ------------------------------------------------------------ */
	dictionary_separate_working_post_trim_multi_drop_down_index(
		dictionary_separate_working_post->dictionary,
		working_post_dictionary,
		ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	/* Trim the [additional attributes data]	 		      */
	/* Note: this must follow					      */
	/* 	 dictionary_separate_working_post_trim_multi_drop_down_index()*/
	/* ------------------------------------------------------------------ */
	dictionary_separate_working_post_trim_double_bracked_data(
		dictionary_separate_working_post->dictionary );

	return dictionary_separate_working_post;
}

DICTIONARY *dictionary_separate_sort(
			char *dictionary_separate_sort_prefix,
			DICTIONARY *working_post_dictionary )
{
	return dictionary_separate_prefixed(
		working_post_dictionary,
		dictionary_separate_sort_prefix );

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
			char *dictionary_separate_drillthru_prefix,
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
			dictionary_without_prefix(
				working_post_dictionary,
				dictionary_separate_drillthru_prefix ) ) )
	{
		dictionary = dictionary_small();
	}

	dictionary_add_suffix_zero( dictionary );

	dictionary_separate_remove_from_starting_label( dictionary );

	return dictionary;

}

DICTIONARY *dictionary_separate_ignore(
			char *dictionary_separate_ignore_prefix,
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_without_prefix(
			working_post_dictionary,
			dictionary_separate_ignore_prefix ) ) )
	{
		dictionary = dictionary_small();
	}

	dictionary_append_dictionary(
		dictionary,
		dictionary_without_prefix(
			working_post_dictionary,
			NO_DISPLAY_PUSH_BUTTON_PREFIX ) );

	return dictionary;
}

DICTIONARY *dictionary_separate_pair_one2m(
			char *dictionary_separate_pair_one2m_prefix,
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
		dictionary_without_prefix(
			working_post_dictionary,
			dictionary_separate_pair_one2m_prefix ) ) )
	{
		dictionary = dictionary_small();
	}

	return dictionary;
}

DICTIONARY *dictionary_separate_query(
			char *dictionary_separate_query_prefix,
			DICTIONARY *working_post_dictionary )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary =
			dictionary_without_prefix(
				working_post_dictionary,
				dictionary_separate_query_prefix ) ) )
	{
		dictionary = dictionary_small();
	}

	dictionary_add_suffix_zero( dictionary );

	dictionary_separate_remove_from_starting_label( dictionary );

	return dictionary;

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
				DICTIONARY_SEPARATE_IGNORE_PREFIX ) == 0
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
				DICTIONARY_SEPARATE_IGNORE_PREFIX ) );
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
			DICTIONARY_SEPARATE_IGNORE_PREFIX );

	list_set_list(
		ignore_select_attribute_name_list,
		dictionary_extract_and_remove_prefixed_key_list(
			remove_index_zero_dictionary,
			NO_DISPLAY_PUSH_BUTTON_PREFIX ) );

	return ignore_select_attribute_name_list;
}

void dictionary_separate_date_convert(
			DICTIONARY *dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			char *dictionary_separate_from_prefix,
			char *dictionary_separate_to_prefix )
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
				     dictionary_separate_date_convert_string( 
					dictionary,
					application_name,
					login_name,
					date_string,
					key );
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
				dictionary_separate_date_convert_string( 
					dictionary,
					application_name,
					login_name,
					date_string,
					key );
			}

			/* Do from date */
			/* ------------ */
			sprintf(key,
				"%s%s_%d",
				dictionary_separate_from_prefix,
				attribute_name,
				index );

			if ( ( date_string =
				dictionary_get(
					key,
					dictionary ) ) )
			{
				dictionary_separate_date_convert_string( 
					dictionary,
					application_name,
					login_name,
					date_string,
					key );
			}

			/* Do to date */
			/* ---------- */
			sprintf(key,
				"%s%s_%d",
				dictionary_separate_to_prefix,
				attribute_name,
				index );
	
			if ( ( date_string =
				dictionary_get(
					key,
					dictionary ) ) )
			{
				dictionary_separate_date_convert_string( 
					dictionary,
					application_name,
					login_name,
					date_string,
					key );
			}

		} while( list_next( folder_attribute_date_name_list ) );

	} /* for each index */
}

void dictionary_separate_working_post_trim_multi_drop_down_index(
			DICTIONARY *dictionary,
			char element_multi_move_left_right_delimiter )
{
	char *ptr;
	char *data;
	char *key;
	LIST *key_list;
	char place_holder = element_multi_move_left_right_delimiter;

	key_list = dictionary_key_list( dictionary );

	if ( !list_rewind( key_list ) ) return;

	do {
		key = list_get( key_list );

		data = dictionary_data( dictionary, key );

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

void dictionary_separate_date_convert_string(
			DICTIONARY *dictionary,
			char *application_name,
			char *login_name,
			char *date_string,
			char *key )
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
			fprintf(stderr,
		"ERROR in %s/%s()/%d: date_convert_international() empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
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

void dictionary_separate_working_post_trim_double_bracked_data(
			DICTIONARY *dictionary )
{
	dictionary_trim_double_bracked_data( dictionary );
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
				DICTIONARY_SEPARATE_IGNORE_PREFIX ) );
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
