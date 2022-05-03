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
			char sql_delimiter )
{
	/* ------------------------------------------------------------ */
	/* Sample:							*/
	/* From:  "station^datatype_1=BA^stage"				*/
	/* To:    "station_1=BA and datatype_1=stage"			*/
	/* ------------------------------------------------------------ */
	dictionary_parse_multi_attribute_keys(
		dictionary, 
		sql_delimiter );
}

DICTIONARY_SEPARATE_POST_EDIT_TABLE *
	dictionary_separate_post_edit_table_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_name_list,
			LIST *role_operation_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_POST_EDIT_TABLE *
		dictionary_separate_post_edit_table =
			dictionary_separate_post_edit_table_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_post_edit_table;
	}

	dictionary_separate_post_edit_table->trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	dictionary_separate_post_edit_table->parse_multi =
		dictionary_separate_parse_multi_new(
			dictionary_separate_post_edit_table->
				trim_double_bracket->
				dictionary,
			SQL_DELIMITER );

	dictionary_separate_post_edit_table->date_convert =
		dictionary_separate_date_convert_new(
			dictionary_separate_post_edit_table->
				parse_multi->
				dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	dictionary_separate_post_edit_table->sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			dictionary_separate_post_edit_table->
				date_convert->
				dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_post_edit_table->sort_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->query_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->drillthru_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->no_display_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->pair_one2m_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->multi_row_dictionary =
		dictionary_separate_multi_row(
			folder_attribute_name_list,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->row_zero_dictionary =
		dictionary_separate_row_zero(
			folder_attribute_name_list,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->operation_dictionary =
		dictionary_separate_operation(
			role_operation_name_list,
			dictionary_separate_post_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_edit_table->no_display_name_list =
		dictionary_key_list(
			dictionary_separate_post_edit_table->
				no_display_dictionary );

	return dictionary_separate_post_edit_table;
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
	dictionary_separate_trim_multi_new(
			DICTIONARY *original_post_dictionary )
{
	DICTIONARY_SEPARATE_TRIM_MULTI *
		trim_multi =
			dictionary_separate_trim_multi_calloc();

	trim_multi->dictionary = original_post_dictionary;

	dictionary_separate_trim_multi(
		trim_multi->
			dictionary,
			ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	return trim_multi;
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
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;

	if ( !dictionary_length( original_post_dictionary ) )
		return (DICTIONARY_SEPARATE_DATE_CONVERT *)0;

	date_convert = dictionary_separate_date_convert_calloc();

	date_convert->dictionary = original_post_dictionary;

	if ( list_length( folder_attribute_date_name_list ) )
	{
		dictionary_separate_date_convert(
			date_convert->dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			DICTIONARY_SEPARATE_FROM_PREFIX,
			DICTIONARY_SEPARATE_TO_PREFIX );
	}

	return date_convert;
}

DICTIONARY *dictionary_separate_operation(
			LIST *role_operation_name_list,
			DICTIONARY *dictionary )
{
	DICTIONARY *destination_dictionary = {0};
	LIST *key_list;
	char *operation_name;
	char *key;
	char key_buffer[ 128 ];

	if ( !list_length( role_operation_name_list ) )
		return (DICTIONARY *)0;

	key_list = dictionary_key_list( dictionary );

	if ( !list_rewind( key_list ) ) return (DICTIONARY *)0;

	do {
		key = list_get( key_list );

		strcpy( key_buffer, key );
		string_trim_index( key_buffer );

		list_rewind( role_operation_name_list );

		do {
			operation_name =
				list_get(
					role_operation_name_list );

			if ( strcmp(
				operation_name,
				key_buffer ) == 0 )
			{
				if ( !destination_dictionary )
				{
					destination_dictionary =
						dictionary_small();
				}

				dictionary_set( 
					destination_dictionary,
					key,
					"yes" );
			}
		} while ( list_next( role_operation_name_list ) );

	} while( list_next( key_list ) );

	list_free_container( key_list );

	return destination_dictionary;
}

DICTIONARY *dictionary_separate_remove_prefix(
			char *prefix,
			DICTIONARY *dictionary )
{
	return
	dictionary_remove_prefix(
		prefix,
		dictionary );
}

DICTIONARY *dictionary_separate_row(
			LIST *folder_attribute_name_list,
			DICTIONARY *multi_row_dictionary,
			int row_number )
{
	DICTIONARY *return_dictionary;
	char *attribute_name;
	char key[ 128 ];
	char *data;

	if ( !list_rewind( folder_attribute_name_list ) )
		return (DICTIONARY *)0;

	if ( !multi_row_dictionary ) return (DICTIONARY *)0;

	if ( row_number < 0 ) return (DICTIONARY *)0;

	return_dictionary = dictionary_small();

	do {
		attribute_name = list_get( folder_attribute_name_list );

		sprintf( key, "%s_%d", attribute_name, row_number );

		if ( ( data = dictionary_get( key, multi_row_dictionary ) ) )
		{
			dictionary_set(
				return_dictionary,
				attribute_name,
				data );
		}
	} while( list_next( folder_attribute_name_list ) );

	return return_dictionary;
}

DICTIONARY *dictionary_separate_non_prefixed(
			char *dictionary_separate_sort_prefix,
			char *dictionary_separate_query_prefix,
			char *dictionary_separate_drillthru_prefix,
			char *dictionary_separate_ignore_prefix,
			char *dictionary_separate_no_display_prefix,
			char *dictionary_separate_pair_prefix,
			DICTIONARY *dictionary )
{
	DICTIONARY *non_prefixed_dictionary;
	LIST *key_list;
	char *key;

	key_list = dictionary_key_list( dictionary );

	if ( !list_rewind( key_list ) ) return (DICTIONARY *)0;

	non_prefixed_dictionary = dictionary_large();

	do {
		key = list_get( key_list );

		if ( string_strncmp(
				key,
				dictionary_separate_sort_prefix ) == 0
		||   string_strncmp(
				key,
				dictionary_separate_query_prefix ) == 0
		||   string_strncmp(
				key,
				dictionary_separate_drillthru_prefix ) == 0
		||   string_strncmp(
				key,
				dictionary_separate_ignore_prefix ) == 0
		||   string_strncmp(
				key,
				dictionary_separate_no_display_prefix ) == 0
		||   string_strncmp(
				key,
				dictionary_separate_pair_prefix ) == 0
		||   string_index(
				key ) >= 0 )
		{
			continue;
		}

		dictionary_set( 
			non_prefixed_dictionary,
			key,
			dictionary_get(
				key,
				dictionary ) );

	} while( list_next( key_list ) );

	list_free_container( key_list );

	return non_prefixed_dictionary;
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

void dictionary_separate_trim_multi(
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

DICTIONARY *dictionary_separate_send_dictionary(
			DICTIONARY *sort_dictionary,
			char *dictionary_separate_sort_prefix,
			DICTIONARY *query_dictionary,
			char *dictionary_separate_query_prefix,
			DICTIONARY *drillthru_dictionary,
			char *dictionary_separate_drillthru_prefix,
			DICTIONARY *ignore_dictionary,
			char *dictionary_separate_ignore_prefix,
			DICTIONARY *no_display_dictionary,
			char *dictionary_separate_no_display_prefix,
			DICTIONARY *pair_one2m_dictionary,
			char *dictionary_separate_pair_prefix,
			DICTIONARY *non_prefixed_dictionary )
{
	DICTIONARY *send_dictionary = dictionary_large();

	if ( dictionary_length( sort_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				sort_dictionary,
				dictionary_separate_sort_prefix ) );
	}

	if ( dictionary_length( query_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				query_dictionary,
				dictionary_separate_query_prefix ) );
	}

	if ( dictionary_length( drillthru_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				drillthru_dictionary,
				dictionary_separate_drillthru_prefix ) );
	}

	if ( dictionary_length( ignore_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				ignore_dictionary,
				dictionary_separate_ignore_prefix ) );
	}

	if ( dictionary_length( no_display_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				no_display_dictionary,
				dictionary_separate_no_display_prefix ) );
	}

	if ( dictionary_length( pair_one2m_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			dictionary_prefix(
				pair_one2m_dictionary,
				dictionary_separate_pair_prefix ) );
	}

	if ( dictionary_length( non_prefixed_dictionary ) )
	{
		dictionary_append_dictionary(
			send_dictionary,
			non_prefixed_dictionary );
	}

	return send_dictionary;
}

char *dictionary_separate_send_string(
			DICTIONARY *replaced_send_dictionary )
{
	char escaped_dictionary_string[ STRING_64K ];

	escape_character(
		escaped_dictionary_string,
		dictionary_display_delimiter(
			replaced_send_dictionary,
			'&' ),
		'$' );

	return strdup( escaped_dictionary_string );
}

char *dictionary_separate_hidden_html(
			char *prefix,
			DICTIONARY *dictionary )
{
	char html[ STRING_64K ];
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

DICTIONARY *dictionary_separate_multi_row_attribute(
			DICTIONARY *destination_dictionary,
			LIST *attribute_name_list,
			DICTIONARY *source_dictionary,
			int row_number )
{
	char key[ 128 ];
	char *name;
	char *data;

	if ( !destination_dictionary
	||   !list_rewind( attribute_name_list )
	||   !source_dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		name = list_get( attribute_name_list );

		sprintf( key, "%s_%d", name, row_number );

		if ( ( data =
			dictionary_get(
				key,
				source_dictionary ) ) )
		{
			dictionary_set(
				destination_dictionary,
				source_dictionary->
					hash_table->
					last_cell_retrieved->
					key,
				data );
		}
	} while ( list_next( attribute_name_list ) );

	return destination_dictionary;
}

DICTIONARY *dictionary_separate_row_zero(
			LIST *folder_attribute_name_list,
			DICTIONARY *dictionary )
{
	DICTIONARY *destination_dictionary = dictionary_small();

	return
	/* ------------------------------ */
	/* Returns destination_dictionary */
	/* ------------------------------ */
	dictionary_separate_multi_row_attribute(
		destination_dictionary,
		folder_attribute_name_list,
		dictionary /* source_dictionary */,
		0 /* row_number */ );
}

DICTIONARY *dictionary_separate_multi_row(
			LIST *folder_attribute_name_list,
			DICTIONARY *dictionary )
{
	int highest_row;
	int row_number;
	DICTIONARY *destination_dictionary;

	highest_row =
		dictionary_highest_row(
			dictionary );

	if ( highest_row < 1 ) return (DICTIONARY *)0;

	destination_dictionary = dictionary_huge();

	for(	row_number = 1;
		row_number <= highest_row;
		row_number++ )
	{
		dictionary_separate_multi_row_attribute(
			destination_dictionary,
			folder_attribute_name_list,
			dictionary /* source_dictionary */,
			row_number );
	}

	return destination_dictionary;
}

DICTIONARY_SEPARATE_DRILLTHRU *
	dictionary_separate_drillthru_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_list )
{
	DICTIONARY_SEPARATE_DRILLTHRU *
		dictionary_separate_drillthru =
			dictionary_separate_drillthru_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_drillthru;
	}

	dictionary_separate_drillthru->trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	dictionary_separate_drillthru->parse_multi =
		dictionary_separate_parse_multi_new(
			dictionary_separate_drillthru->
				trim_double_bracket->
				dictionary,
			SQL_DELIMITER );

	dictionary_separate_drillthru->date_convert =
		dictionary_separate_date_convert_new(
			dictionary_separate_drillthru->
				parse_multi->
				dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	dictionary_separate_drillthru->sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			dictionary_separate_drillthru->
				date_convert->
				dictionary,
			folder_attribute_list );

	dictionary_separate_drillthru->drillthru_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			dictionary_separate_drillthru->
				sql_injection_escape->
				dictionary );

	return dictionary_separate_drillthru;
}

DICTIONARY_SEPARATE_DRILLTHRU *
	dictionary_separate_drillthru_calloc(
			void )
{
	DICTIONARY_SEPARATE_DRILLTHRU *dictionary_separate_drillthru;

	if ( ! ( dictionary_separate_drillthru =
		     calloc( 1, sizeof( DICTIONARY_SEPARATE_DRILLTHRU ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return dictionary_separate_drillthru;
}

DICTIONARY_SEPARATE_EDIT_TABLE *
	dictionary_separate_edit_table_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_EDIT_TABLE *
		dictionary_separate_edit_table =
			dictionary_separate_edit_table_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_edit_table;
	}

	dictionary_separate_edit_table->trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	dictionary_separate_edit_table->parse_multi =
		dictionary_separate_parse_multi_new(
			dictionary_separate_edit_table->
				trim_double_bracket->
				dictionary,
			SQL_DELIMITER );

	dictionary_separate_edit_table->date_convert =
		dictionary_separate_date_convert_new(
			dictionary_separate_edit_table->
				parse_multi->
				dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	dictionary_separate_edit_table->sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			dictionary_separate_edit_table->
				date_convert->
				dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_edit_table->sort_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			dictionary_separate_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_edit_table->query_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			dictionary_separate_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_edit_table->drillthru_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			dictionary_separate_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_edit_table->no_display_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			dictionary_separate_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_edit_table->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			dictionary_separate_edit_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_edit_table->no_display_name_list =
		dictionary_key_list(
			dictionary_separate_edit_table->
				no_display_dictionary );

	return dictionary_separate_edit_table;
}

DICTIONARY_SEPARATE_EDIT_TABLE *dictionary_separate_edit_table_calloc( void )
{
	DICTIONARY_SEPARATE_EDIT_TABLE *dictionary_separate_edit_table;

	if ( ! ( dictionary_separate_edit_table =
		     calloc( 1, sizeof( DICTIONARY_SEPARATE_EDIT_TABLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return dictionary_separate_edit_table;
}

DICTIONARY_SEPARATE_POST_EDIT_TABLE *
	dictionary_separate_post_edit_table_calloc(
			void )
{
	DICTIONARY_SEPARATE_POST_EDIT_TABLE *
		dictionary_separate_post_edit_table;

	if ( ! ( dictionary_separate_post_edit_table =
		   calloc(
			1,
			sizeof( DICTIONARY_SEPARATE_POST_EDIT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary_separate_post_edit_table;
}

DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *
	dictionary_separate_trim_double_bracket_new(
			DICTIONARY *original_post_dictionary )
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *
		trim_double_bracket;

	if ( !original_post_dictionary )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: original_post_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_calloc();

	trim_double_bracket->dictionary = original_post_dictionary;

	dictionary_separate_trim_double_bracket(
		trim_double_bracket->dictionary );

	return trim_double_bracket;
}

DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *
	dictionary_separate_trim_double_bracket_calloc(
			void )
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *
		dictionary_separate_trim_double_bracket;

	if ( ! ( dictionary_separate_trim_double_bracket =
		   calloc(
			1,
			sizeof( DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary_separate_trim_double_bracket;
}

void dictionary_separate_trim_double_bracket(
			DICTIONARY *dictionary )
{
	dictionary_trim_double_bracked_data( dictionary );
}

DICTIONARY_SEPARATE_PARSE_MULTI *
	dictionary_separate_parse_multi_new(
			DICTIONARY *original_post_dictionary,
			char sql_delimiter )
{
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;

	if ( !original_post_dictionary )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: original_post_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	parse_multi = dictionary_separate_parse_multi_calloc();

	parse_multi->dictionary = original_post_dictionary;

	dictionary_separate_parse_multi(
		parse_multi->dictionary,
		sql_delimiter );

	return parse_multi;
}

DICTIONARY_SEPARATE_PARSE_MULTI *
	dictionary_separate_parse_multi_calloc(
			void )
{
	DICTIONARY_SEPARATE_PARSE_MULTI *
		dictionary_separate_parse_multi;

	if ( ! ( dictionary_separate_parse_multi =
		   calloc(
			1,
			sizeof( DICTIONARY_SEPARATE_PARSE_MULTI ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary_separate_parse_multi;
}

DICTIONARY_SEPARATE_INSERT_TABLE *
	dictionary_separate_insert_table_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_append_isa_list,
			LIST *folder_attribute_name_list )
{
	DICTIONARY_SEPARATE_INSERT_TABLE *
		dictionary_separate_insert_table =
			dictionary_separate_insert_table_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_insert_table;
	}

	dictionary_separate_insert_table->trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	dictionary_separate_insert_table->parse_multi =
		dictionary_separate_parse_multi_new(
			dictionary_separate_insert_table->
				trim_double_bracket->
				dictionary,
			SQL_DELIMITER );

	dictionary_separate_insert_table->date_convert =
		dictionary_separate_date_convert_new(
			dictionary_separate_insert_table->
				parse_multi->
				dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	dictionary_separate_insert_table->sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			dictionary_separate_insert_table->
				date_convert->
				dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_insert_table->drillthru_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			dictionary_separate_insert_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_insert_table->ignore_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			dictionary_separate_insert_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_insert_table->ignore_name_list =
		dictionary_key_list(
			dictionary_separate_insert_table->
				ignore_dictionary );

	dictionary_separate_insert_table->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			dictionary_separate_insert_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_insert_table->multi_row_dictionary =
		dictionary_separate_multi_row(
			folder_attribute_name_list,
			dictionary_separate_insert_table->
				sql_injection_escape->
				dictionary );

	dictionary_separate_insert_table->row_zero_dictionary =
		dictionary_separate_row_zero(
			folder_attribute_name_list,
			dictionary_separate_insert_table->
				sql_injection_escape->
				dictionary );

	return dictionary_separate_insert_table;
}

DICTIONARY_SEPARATE_INSERT_TABLE *
	dictionary_separate_insert_table_calloc(
			void )
{
	DICTIONARY_SEPARATE_INSERT_TABLE *dictionary_separate_insert_table;

	if ( ! ( dictionary_separate_insert_table =
		     calloc( 1, sizeof( DICTIONARY_SEPARATE_INSERT_TABLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return dictionary_separate_insert_table;
}

DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *
	dictionary_separate_sql_injection_escape_new(
			DICTIONARY *original_post_dictionary,
			LIST *folder_attribute_list )
{
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *
		dictionary_separate_sql_injection_escape;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *d;
	char *datatype_name;

	dictionary_separate_sql_injection_escape =
		dictionary_separate_sql_injection_escape_calloc();

	d = dictionary_separate_sql_injection_escape;

	d->dictionary = original_post_dictionary;

	d->dictionary_key_list =
		dictionary_key_list(
			d->dictionary );

	if ( !list_rewind( d->dictionary_key_list ) )
	{
		free( d );
		return (DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *)0;
	}

	do {
		d->key = list_get( d->dictionary_key_list );

		d->data =
			dictionary_get(
				d->key,
				d->dictionary );

		if ( d->data )
		{
			if ( ! ( d->folder_attribute =
					folder_attribute_seek(
						d->key,
						folder_attribute_list ) ) )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_seek(%s) returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					d->key );
				exit( 1 );
			}

			if ( !d->folder_attribute->attribute )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			datatype_name =
				d->folder_attribute->
					attribute->
					datatype_name;

			d->security_sql_injection_escape =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				security_sql_injection_escape(
					/* ------------------------- */
					/* Forbidden characters: ‘\” */
					/* Returns data		     */
					/* ------------------------- */
					security_replace_special_characters(
						/* -------------- */
						/* Returns number */
						/* -------------- */
						string_trim_number_characters(
							d->data /* number */,
							datatype_name ) ) );

			if ( !d->security_sql_injection_escape )
			{
				d->security_sql_injection_escape = "";
			}

			dictionary_set(
				d->dictionary,
				d->key,
				d->security_sql_injection_escape );
		}

	} while ( list_next( d->dictionary_key_list ) );

	return dictionary_separate_sql_injection_escape;
}

DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *
	dictionary_separate_sql_injection_escape_calloc(
			void )
{
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *
		dictionary_separate_sql_injection_escape;

	if ( ! ( dictionary_separate_sql_injection_escape =
		     calloc(
			1,
			sizeof( DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return dictionary_separate_sql_injection_escape;
}

DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
	dictionary_separate_post_prompt_insert_new(
			DICTIONARY *original_post_dictionary,
			char *application_name,
			char *login_name,
			LIST *folder_attribute_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
		dictionary_separate_post_prompt_insert =
			dictionary_separate_post_prompt_insert_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_post_prompt_insert;
	}

	dictionary_separate_post_prompt_insert->trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	dictionary_separate_post_prompt_insert->parse_multi =
		dictionary_separate_parse_multi_new(
			dictionary_separate_post_prompt_insert->
				trim_double_bracket->
				dictionary,
			SQL_DELIMITER );

	dictionary_separate_post_prompt_insert->date_convert =
		dictionary_separate_date_convert_new(
			dictionary_separate_post_prompt_insert->
				parse_multi->
				dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	dictionary_separate_post_prompt_insert->sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			dictionary_separate_post_prompt_insert->
				date_convert->
				dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_post_prompt_insert->drillthru_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			dictionary_separate_post_prompt_insert->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_prompt_insert->row_zero_dictionary =
		dictionary_separate_row_zero(
			folder_attribute_name_list,
			dictionary_separate_post_prompt_insert->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_prompt_insert->ignore_dictionary =
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			dictionary_separate_post_prompt_insert->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_prompt_insert->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			dictionary_separate_post_prompt_insert->
				sql_injection_escape->
				dictionary );

	dictionary_separate_post_prompt_insert->ignore_name_list =
		dictionary_key_list(
			dictionary_separate_post_prompt_insert->
				ignore_dictionary );

	return dictionary_separate_post_prompt_insert;
}

DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
	dictionary_separate_post_prompt_insert_calloc(
			void )
{
	DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
		dictionary_separate_post_prompt_insert;

	if ( ! ( dictionary_separate_post_prompt_insert =
		     calloc(
			1,
			sizeof( DICTIONARY_SEPARATE_POST_PROMPT_INSERT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return dictionary_separate_post_prompt_insert;
}

