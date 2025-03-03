/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/dictionary_separate.c	   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "sql.h"
#include "widget.h"
#include "appaserver_error.h"
#include "relation.h"
#include "prompt_lookup.h"
#include "dictionary_separate.h"

DICTIONARY_SEPARATE *dictionary_separate_calloc( void )
{
	DICTIONARY_SEPARATE *dictionary_separate;

	if ( ! ( dictionary_separate =
			calloc( 1, sizeof ( DICTIONARY_SEPARATE ) ) ) )
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

DICTIONARY_SEPARATE_POST_TABLE_EDIT *
	dictionary_separate_post_table_edit_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *role_operation_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_POST_TABLE_EDIT *
		dictionary_separate_post_table_edit;
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	dictionary_separate_post_table_edit =
		dictionary_separate_post_table_edit_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_post_table_edit;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_edit_new(
			parse_multi->dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_post_table_edit->sort_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->query_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->drillthru_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->no_display_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_no_display(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->pair_one2m_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->multi_row_dictionary =
		dictionary_separate_multi_row(
			folder_attribute_name_list,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->prompt_dictionary =
		dictionary_separate_prompt(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			relation_mto1_list,
			folder_attribute_name_list,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->operation_dictionary =
		dictionary_separate_operation(
			role_operation_name_list,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_edit->no_display_name_list =
		dictionary_separate_no_display_name_list(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			dictionary_separate_post_table_edit->
				no_display_dictionary );

	return dictionary_separate_post_table_edit;
}

DICTIONARY_SEPARATE *dictionary_separate_string_new(
		DICTIONARY *original_post_dictionary )
{
	DICTIONARY_SEPARATE *dictionary_separate =
		dictionary_separate_calloc();

	dictionary_separate->original_post_dictionary =
		original_post_dictionary;

	dictionary_separate->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			original_post_dictionary );

	return dictionary_separate;
}

DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_calloc(
		void )
{
	DICTIONARY_SEPARATE_DATE_CONVERT *dictionary_separate_date_convert;

	if ( ! ( dictionary_separate_date_convert =
		   calloc( 1, sizeof ( DICTIONARY_SEPARATE_DATE_CONVERT ) ) ) )
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
	dictionary_separate_date_convert_prompt_new(
		const char *prompt_lookup_from_prefix,
		const char *prompt_lookup_to_prefix,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		DICTIONARY *original_post_dictionary )
{
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;

	if ( !application_name
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	date_convert = dictionary_separate_date_convert_calloc();
	date_convert->dictionary = original_post_dictionary;

	if ( list_length( folder_attribute_date_name_list ) )
	{
		dictionary_separate_date_convert_prompt_frame(
			prompt_lookup_from_prefix,
			prompt_lookup_to_prefix,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			date_convert->dictionary /* in/out */ );
	}

	return date_convert;
}

DICTIONARY_SEPARATE_DATE_CONVERT *
	dictionary_separate_date_convert_edit_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list )
{
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	int highest_index;

	date_convert = dictionary_separate_date_convert_calloc();
	date_convert->dictionary = original_post_dictionary;

	if ( !application_name
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( dictionary_length( date_convert->dictionary )
	&&   list_length( folder_attribute_date_name_list ) )
	{
		highest_index =
			dictionary_highest_index(
				date_convert->dictionary );

		if ( highest_index )
		{
			dictionary_separate_date_convert_edit_frame(
				date_convert->dictionary /* in/out */,
				application_name,
				login_name,
				folder_attribute_date_name_list,
				highest_index );
		}
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

		string_trim_index( key_buffer, key );

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
		const char *prefix,
		DICTIONARY *dictionary )
{
	return
	/* ---------------------------------- */
	/* Returns dictionary_small() or null */
	/* ---------------------------------- */
	dictionary_remove_prefix(
		prefix,
		dictionary );
}

char *dictionary_separate_key(
		char *attribute_name,
		int row_number )
{
	static char key[ 128 ];

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(key,
		"%s_%d",
		attribute_name,
		row_number );

	return key;
}

DICTIONARY *dictionary_separate_non_prefixed(
		const char *dictionary_separate_sort_prefix,
		const char *dictionary_separate_query_prefix,
		const char *dictionary_separate_drillthru_prefix,
		const char *dictionary_separate_ignore_prefix,
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_pair_prefix,
		DICTIONARY *dictionary )
{
	DICTIONARY *non_prefixed_dictionary = {0};
	LIST *key_list;
	char *key;

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( string_strncmp(
			key,
			(char *)dictionary_separate_sort_prefix ) == 0
		||   string_strncmp(
			key,
			(char *)dictionary_separate_query_prefix ) == 0
		||   string_strncmp(
			key,
			(char *)dictionary_separate_drillthru_prefix ) == 0
		||   string_strncmp(
			key,
			(char *)dictionary_separate_ignore_prefix ) == 0
		||   string_strncmp(
			key,
			(char *)dictionary_separate_no_display_prefix ) == 0
		||   string_strncmp(
			key,
			(char *)dictionary_separate_pair_prefix ) == 0 )
		{
			continue;
		}

		if ( !non_prefixed_dictionary )
			non_prefixed_dictionary =
				dictionary_large();

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

void dictionary_separate_date_convert_prompt_frame(
		const char *prompt_lookup_from_prefix,
		const char *prompt_lookup_to_prefix,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		DICTIONARY *dictionary /* in/out */ )
{
	char *attribute_name;
	char key[ 128 ];
	char *get;
	char *dup;

	if ( !application_name
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( folder_attribute_date_name_list ) )
	do {
		attribute_name =
			list_get(
				folder_attribute_date_name_list );

		if ( ( get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				attribute_name,
				dictionary ) ) )
		{
			(void)dictionary_separate_date_convert( 
				dictionary,
				application_name,
				login_name,
				get /* date_string */,
				attribute_name /* key */ );

			continue;
		}

		sprintf(key,
			"%s%s",
			prompt_lookup_from_prefix,
			attribute_name );

		if ( ( get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get( key, dictionary ) ) )
		{
			dup = strdup( key );

			if ( !dictionary_separate_date_convert( 
				dictionary,
				application_name,
				login_name,
				get /* date_string */,
				dup /* key */ ) )
			{
				free( dup );
			}
		}

		sprintf(key,
			"%s%s",
			prompt_lookup_to_prefix,
			attribute_name );
	
		if ( ( get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get( key, dictionary ) ) )
		{
			dup = strdup( key );

			if ( !dictionary_separate_date_convert( 
				dictionary,
				application_name,
				login_name,
				get /* date_string */,
				dup /* key */ ) )
			{
				free( dup );
			}
		}

	} while ( list_next( folder_attribute_date_name_list ) );
}

void dictionary_separate_date_convert_edit_frame(
		DICTIONARY *dictionary /* in/out */,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		int dictionary_highest_index )
{
	int index;
	char *attribute_name;
	char key[ 128 ];
	char *get;
	char *dup;

	if ( !dictionary
	||   !application_name
	||   !login_name
	||   !list_length( folder_attribute_date_name_list )
	||   !dictionary_highest_index )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	for( index = 1; index <= dictionary_highest_index; index++ )
	{
		list_rewind( folder_attribute_date_name_list );

		do {
			attribute_name =
				list_get(
					folder_attribute_date_name_list );

			sprintf(key,
				"%s_%d",
				attribute_name,
				index );

			if ( ( get =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
					key,
					dictionary ) ) )
			{
				dup = strdup( key );

				if ( !dictionary_separate_date_convert( 
					dictionary /* in/out */,
					application_name,
					login_name,
					get /* date_string */,
					dup ) )
				{
					free( dup );
				}
			}

		} while ( list_next( folder_attribute_date_name_list ) );
	}
}

boolean dictionary_separate_date_convert(
		DICTIONARY *dictionary /* in/out */,
		char *application_name,
		char *login_name,
		char *date_string,
		char *key )
{
	char *string;

	if ( !dictionary
	||   !application_name
	||   !login_name
	||   !date_string
	||   !key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		dictionary_separate_date_convert_string(
			application_name,
			login_name,
			date_string );

	if ( !string ) return 0;

	if ( strcmp( date_string, string ) != 0 )
	{
		dictionary_set( dictionary, key, string );
		return 1;
	}
	else
	{
		free( string );
		return 0;
	}
}

char *dictionary_separate_date_convert_string(
		char *application_name,
		char *login_name,
		char *date_string )
{
	char string[ 4096 ];
	char *ptr = string;
	char date_piece_string[ 1024 ];
	int i;
	DATE_CONVERT *date_convert;

	if ( !application_name
	||   !login_name
	||   !date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	*ptr = '\0';

	/* Loop if Or , */
	/* ------------ */
	for( i = 0; piece( date_piece_string, ',', date_string, i ); i++ )
	{
		if ( strlen( date_piece_string ) > 20 ) return NULL;

		if ( string_character_count(
			' ',
			date_piece_string ) > 1 )
		{
			return NULL;
		}

		date_convert =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			date_convert_destination_international(
				application_name,
				login_name,
				date_piece_string );

		if ( date_convert->source_enum == date_convert_unknown )
		{
			date_convert_free( date_convert );
			return NULL;
		}

		if ( i ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			date_convert->return_date_string );
	}

	if ( *string )
		return strdup( string );
	else
		return NULL;
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

char *dictionary_separate_send_string( DICTIONARY *send_dictionary )
{
	char escaped_dictionary_string[ STRING_SIZE_HASH_TABLE ];
	char *display_delimited;

	display_delimited =
		/* ------------------- */ 
		/* Returns heap memory */
		/* ------------------- */ 
		dictionary_display_delimited(
			send_dictionary,
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER
				/* probably '=' */,
			DICTIONARY_ELEMENT_DELIMITER
				/* probably '&' */ );

	if ( !*display_delimited ) return "";

	string_escape_character(
		escaped_dictionary_string,
		display_delimited,
		'$' );

	free( display_delimited );

	return
	strdup( escaped_dictionary_string );
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
	char *tmp;

	if ( !prefix )
	{
		char message[ 128 ];

		sprintf(message, "prefix is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	key_list = dictionary_key_list( dictionary );

	if ( !list_rewind( key_list ) ) return (char *)0;

	do {
		key = list_get( key_list );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		data = dictionary_get( key, dictionary );

		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		sprintf(input_name,
			"%s%s",
			prefix,
			key );

		ptr += sprintf(
			ptr,
			"%s",
			( tmp =
				/* Returns heap memory */
				/* ------------------- */
				widget_hidden_html_string(
					input_name
					   /* widget_hidden_container_key */,
					data
					   /* widget_hidden_value */ ) ) );

		free( tmp );

	} while ( list_next( key_list ) );

	return strdup( html );
}

DICTIONARY *dictionary_separate_row_number(
		DICTIONARY *destination_dictionary,
		LIST *attribute_name_list,
		DICTIONARY *source_dictionary,
		int row_number )
{
	char key[ 128 ];
	char *name;
	char *get;

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

		if ( ( get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get( key, source_dictionary ) ) )
		{
			dictionary_set(
				destination_dictionary,
				source_dictionary->
					hash_table->
					last_cell_retrieved->
					key,
				get );
		}

	} while ( list_next( attribute_name_list ) );

	return destination_dictionary;
}

DICTIONARY *dictionary_separate_prompt(
		const char *prompt_lookup_from_prefix,
		const char *prompt_lookup_to_prefix,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		DICTIONARY *dictionary )
{
	DICTIONARY *small;
	RELATION_MTO1 *relation_mto1;
	char *attribute_name;
	char *key;
	char *get;
	char *dup;

	if ( !list_length( folder_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "folder_attribute_name_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	small = dictionary_small();

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_list );

		if ( !list_length( relation_mto1->relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
			"relation_mto1->relation_foreign_key_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		key =
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			list_display_delimited(
				relation_mto1->
					relation_foreign_key_list,
				SQL_DELIMITER );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		if ( ( get = dictionary_get( key, dictionary ) ) )
		{
			dictionary_set(
				small,
				key,
				get );
		}
		else
		{
			free( key );
		}

	} while ( list_next( relation_mto1_list ) );

	if ( list_rewind( folder_attribute_name_list ) )
	do {
		attribute_name = list_get( folder_attribute_name_list );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		if ( ( get = dictionary_get( attribute_name, dictionary ) ) )
		{
			dictionary_set(
				small,
				attribute_name,
				get );

			continue;
		}

		key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			dictionary_separate_from_to_key(
				prompt_lookup_from_prefix,
				attribute_name );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		if ( ( get = dictionary_get( key, dictionary ) ) )
		{
			dup = strdup( key );

			dictionary_set(
				small,
				dup,
				get );

			dictionary_set(
				small,
				attribute_name,
				get );
		}

		key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			dictionary_separate_from_to_key(
				prompt_lookup_to_prefix,
				attribute_name );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		if ( ( get = dictionary_get( key, dictionary ) ) )
		{
			dup = strdup( key );

			dictionary_set(
				small,
				dup,
				get );
		}

	} while ( list_next( folder_attribute_name_list ) );

	return small;
}

DICTIONARY *dictionary_separate_multi_row(
		LIST *folder_attribute_name_list,
		DICTIONARY *dictionary )
{
	int highest_index;
	int row_number;
	DICTIONARY *destination_dictionary;

	highest_index =
		dictionary_highest_index(
			dictionary );

	if ( highest_index < 1 ) return (DICTIONARY *)0;

	destination_dictionary = dictionary_huge();

	for(	row_number = 1;
		row_number <= highest_index;
		row_number++ )
	{
		dictionary_separate_row_number(
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
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_drillthru;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_prompt_new(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			parse_multi->dictionary /* in/out */ );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_list );

	dictionary_separate_drillthru->drillthru_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_drillthru->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	return dictionary_separate_drillthru;
}

DICTIONARY_SEPARATE_DRILLTHRU *
	dictionary_separate_drillthru_calloc(
		void )
{
	DICTIONARY_SEPARATE_DRILLTHRU *dictionary_separate_drillthru;

	if ( ! ( dictionary_separate_drillthru =
		     calloc( 1, sizeof ( DICTIONARY_SEPARATE_DRILLTHRU ) ) ) )
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

DICTIONARY_SEPARATE_TABLE_EDIT *
	dictionary_separate_table_edit_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_TABLE_EDIT *dictionary_separate_table_edit;
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	dictionary_separate_table_edit =
		dictionary_separate_table_edit_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_table_edit;
	}

	dictionary_separate_table_edit->original_post_dictionary =
		original_post_dictionary;

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_edit_new(
			parse_multi->dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_table_edit->sort_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_table_edit->drillthru_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_table_edit->no_display_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_no_display(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_table_edit->no_display_name_list =
		dictionary_separate_no_display_name_list(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			dictionary_separate_table_edit->
				no_display_dictionary );

	dictionary_separate_table_edit->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_table_edit->query_dictionary =
		dictionary_separate_query(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			sql_injection_escape->dictionary,
			dictionary_separate_table_edit->
				non_prefixed_dictionary );

	return dictionary_separate_table_edit;
}

DICTIONARY_SEPARATE_TABLE_EDIT *dictionary_separate_table_edit_calloc( void )
{
	DICTIONARY_SEPARATE_TABLE_EDIT *dictionary_separate_table_edit;

	if ( ! ( dictionary_separate_table_edit =
		     calloc( 1, sizeof ( DICTIONARY_SEPARATE_TABLE_EDIT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return dictionary_separate_table_edit;
}

DICTIONARY_SEPARATE_POST_TABLE_EDIT *
	dictionary_separate_post_table_edit_calloc(
		void )
{
	DICTIONARY_SEPARATE_POST_TABLE_EDIT *
		dictionary_separate_post_table_edit;

	if ( ! ( dictionary_separate_post_table_edit =
		   calloc(
			1,
			sizeof ( DICTIONARY_SEPARATE_POST_TABLE_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary_separate_post_table_edit;
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
			sizeof ( DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET ) ) ) )
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
		const char *dictionary_separate_sort_prefix,
		const char *dictionary_separate_query_prefix,
		const char *dictionary_separate_drillthru_prefix,
		const char *dictionary_separate_ignore_prefix,
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_pair_prefix,
		const char sql_delimiter,
		DICTIONARY *dictionary )
{
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	LIST *key_list;

	parse_multi = dictionary_separate_parse_multi_calloc();

	parse_multi->dictionary = dictionary;

	key_list = dictionary_key_list( parse_multi->dictionary );

	if ( !list_length( key_list ) ) return parse_multi;

	dictionary_separate_parse_multi(
		(char *)dictionary_separate_sort_prefix,
		sql_delimiter,
		dictionary /* in/out */,
		key_list );

	dictionary_separate_parse_multi(
		(char *)dictionary_separate_query_prefix,
		sql_delimiter,
		dictionary /* in/out */,
		key_list );

	dictionary_separate_parse_multi(
		(char *)dictionary_separate_drillthru_prefix,
		sql_delimiter,
		dictionary /* in/out */,
		key_list );

	dictionary_separate_parse_multi(
		(char *)dictionary_separate_ignore_prefix,
		sql_delimiter,
		dictionary /* in/out */,
		key_list );

	dictionary_separate_parse_multi(
		(char *)dictionary_separate_no_display_prefix,
		sql_delimiter,
		dictionary /* in/out */,
		key_list );

	dictionary_separate_parse_multi(
		(char *)dictionary_separate_pair_prefix,
		sql_delimiter,
		dictionary /* in/out */,
		key_list );

	dictionary_separate_parse_multi(
		(char *)dictionary_separate_pair_prefix,
		sql_delimiter,
		dictionary /* in/out */,
		key_list );

	dictionary_separate_parse_multi(
		(char *)0 /* prefix */,
		sql_delimiter,
		dictionary /* in/out */,
		key_list );

	list_free_container( key_list );

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
			sizeof ( DICTIONARY_SEPARATE_PARSE_MULTI ) ) ) )
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

DICTIONARY_SEPARATE_TABLE_INSERT *
	dictionary_separate_table_insert_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list,
		LIST *folder_attribute_name_list )
{
	DICTIONARY_SEPARATE_TABLE_INSERT *
		dictionary_separate_table_insert =
			dictionary_separate_table_insert_calloc();
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_table_insert;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_prompt_new(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			parse_multi->dictionary /* in/out */ );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_table_insert->drillthru_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_table_insert->ignore_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_table_insert->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_table_insert->prompt_dictionary =
		dictionary_separate_prompt(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			relation_mto1_list,
			folder_attribute_name_list,
			sql_injection_escape->dictionary );

	dictionary_separate_table_insert->pair_one2m_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_table_insert->ignore_name_list =
		dictionary_key_list(
			dictionary_separate_table_insert->
				ignore_dictionary );

	dictionary_separate_table_insert->prompt_name_list =
		dictionary_key_list(
			dictionary_separate_table_insert->
				prompt_dictionary );

	return dictionary_separate_table_insert;
}

DICTIONARY_SEPARATE_TABLE_INSERT *
	dictionary_separate_table_insert_calloc(
		void )
{
	DICTIONARY_SEPARATE_TABLE_INSERT *dictionary_separate_table_insert;

	if ( ! ( dictionary_separate_table_insert =
		     calloc(
			1,
			sizeof ( DICTIONARY_SEPARATE_TABLE_INSERT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return dictionary_separate_table_insert;
}

DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *
	dictionary_separate_sql_injection_escape_new(
		DICTIONARY *original_post_dictionary,
		LIST *folder_attribute_list )
{
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *dictionary_separate;
	LIST *key_list;
	char *key;
	char *datum;
	FOLDER_ATTRIBUTE *folder_attribute;
	char *sql_injection_escape;
	char key_trimmed[ 128 ];

	dictionary_separate =
		dictionary_separate_sql_injection_escape_calloc();

	dictionary_separate->dictionary = original_post_dictionary;

	key_list =
		dictionary_key_list(
			dictionary_separate->dictionary );

	if ( !list_rewind( key_list ) )
	{
		free( dictionary_separate );
		return NULL;
	}

	do {
		key = list_get( key_list );

		datum =
			dictionary_get(
				key,
				dictionary_separate->dictionary );

		if ( datum )
		{
			string_search_replace_character(
				datum, '"', '\'' );

			string_trim_index(
				key_trimmed /* destination */,
				key );

			if ( ( folder_attribute =
					folder_attribute_seek(
						key_trimmed,
						folder_attribute_list ) ) )
			{
				if ( !folder_attribute->
					attribute )
				{
					char message[ 128 ];

					sprintf(message,
				"folder_attribute->attribute is empty." );

					appaserver_error_stderr_exit(
						__FILE__,
						__FUNCTION__,
						__LINE__,
						message );
				}

				if ( attribute_is_number(
					folder_attribute->
						attribute->
						datatype_name ) )
				{
					/* --------------- */
					/* Returns number. */
					/* Trims , and $   */
					/* --------------- */
					attribute_trim_number_characters(
						datum /* number */ );
				}
			}

			sql_injection_escape =
				/* ---------------------------- */
				/* Returns heap memory or datum */
				/* ---------------------------- */
				security_sql_injection_escape(
					SECURITY_ESCAPE_CHARACTER_STRING,
					datum );

			if ( sql_injection_escape != datum )
			{
				dictionary_set(
					dictionary_separate->dictionary,
					key,
					sql_injection_escape );
			}
		}

	} while ( list_next( key_list ) );

	return dictionary_separate;
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
		      sizeof ( DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE ) ) ) )
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
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
		dictionary_separate_post_prompt_insert;
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	if ( !application_name
	||   !login_name
	||   !folder_attribute_name_list
	||   !folder_attribute_append_isa_list )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	dictionary_separate_post_prompt_insert =
		dictionary_separate_post_prompt_insert_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_post_prompt_insert;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_prompt_new(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			parse_multi->dictionary /* in/out */ );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_post_prompt_insert->drillthru_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_prompt_insert->prompt_dictionary =
		dictionary_separate_prompt(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			relation_mto1_list,
			folder_attribute_name_list,
			sql_injection_escape->dictionary );

	dictionary_separate_post_prompt_insert->pair_one2m_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_prompt_insert->ignore_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_prompt_insert->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

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
			sizeof ( DICTIONARY_SEPARATE_POST_PROMPT_INSERT ) ) ) )
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

DICTIONARY_SEPARATE_PROMPT_PROCESS *
	dictionary_separate_prompt_process_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *prompt_process_date_name_list,
		LIST *folder_attribute_name_list_attribute_list )
{
	DICTIONARY_SEPARATE_PROMPT_PROCESS *
		dictionary_separate_prompt_process =
			dictionary_separate_prompt_process_calloc();
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_prompt_process;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_prompt_new(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			application_name,
			login_name,
			prompt_process_date_name_list,
			parse_multi->dictionary /* in/out */ );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_name_list_attribute_list );

	dictionary_separate_prompt_process->drillthru_dictionary =
		sql_injection_escape->dictionary;

	dictionary_separate_prompt_process->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	return dictionary_separate_prompt_process;
}

DICTIONARY_SEPARATE_PROMPT_PROCESS *
	dictionary_separate_prompt_process_calloc(
		void )
{
	DICTIONARY_SEPARATE_PROMPT_PROCESS *
		dictionary_separate_prompt_process;

	if ( ! ( dictionary_separate_prompt_process =
		     calloc(
			1,
			sizeof ( DICTIONARY_SEPARATE_PROMPT_PROCESS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return dictionary_separate_prompt_process;
}

DICTIONARY_SEPARATE_POST_TABLE_INSERT *
	dictionary_separate_post_table_insert_calloc(
		void )
{
	DICTIONARY_SEPARATE_POST_TABLE_INSERT *
		dictionary_separate_post_table_insert;

	if ( ! ( dictionary_separate_post_table_insert =
		     calloc(
			1,
			sizeof ( DICTIONARY_SEPARATE_POST_TABLE_INSERT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return dictionary_separate_post_table_insert;
}

DICTIONARY_SEPARATE_POST_TABLE_INSERT *
	dictionary_separate_post_table_insert_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_POST_TABLE_INSERT *
		dictionary_separate_post_table_insert =
			dictionary_separate_post_table_insert_calloc();
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_post_table_insert;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_edit_new(
			parse_multi->dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_post_table_insert->ignore_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_insert->pair_one2m_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_insert->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_insert->prompt_dictionary =
		dictionary_separate_prompt(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			relation_mto1_list,
			folder_attribute_name_list,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_insert->multi_row_dictionary =
		dictionary_separate_multi_row(
			folder_attribute_name_list,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_insert->drillthru_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_table_insert->ignore_name_list =
		dictionary_separate_no_display_name_list(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			dictionary_separate_post_table_insert->
				ignore_dictionary );

	return dictionary_separate_post_table_insert;
}

DICTIONARY_SEPARATE_PROMPT_LOOKUP *
	dictionary_separate_prompt_lookup_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_list )
{
	DICTIONARY_SEPARATE_PROMPT_LOOKUP *prompt_lookup;
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	if ( !application_name
	||   !login_name
	||   !list_length( folder_attribute_name_list )
	||   !list_length( folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_lookup = dictionary_separate_prompt_lookup_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return prompt_lookup;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_prompt_new(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			parse_multi->dictionary /* in/out */ );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_list );

	prompt_lookup->prompt_dictionary =
		dictionary_separate_prompt(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			relation_mto1_list,
			folder_attribute_name_list,
			sql_injection_escape->dictionary );

	prompt_lookup->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	return prompt_lookup;
}

DICTIONARY_SEPARATE_PROMPT_LOOKUP *
	dictionary_separate_prompt_lookup_calloc(
		void )
{
	DICTIONARY_SEPARATE_PROMPT_LOOKUP *dictionary_separate_prompt_lookup;

	if ( ! ( dictionary_separate_prompt_lookup =
		   calloc(
			1,
			sizeof ( DICTIONARY_SEPARATE_PROMPT_LOOKUP ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return dictionary_separate_prompt_lookup;
}

DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *
	dictionary_separate_post_prompt_lookup_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *folder_attribute_name_list,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *
		dictionary_separate_post_prompt_lookup;
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	if ( !application_name
	||   !login_name
	||   !list_length( folder_attribute_name_list )
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	dictionary_separate_post_prompt_lookup =
		dictionary_separate_post_prompt_lookup_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_post_prompt_lookup;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_prompt_new(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			parse_multi->dictionary /* in/out */ );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_post_prompt_lookup->drillthru_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_prompt_lookup->prompt_dictionary =
		dictionary_separate_prompt(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			relation_mto1_list,
			folder_attribute_name_list,
			sql_injection_escape->dictionary );

	dictionary_separate_post_prompt_lookup->no_display_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_no_display(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_prompt_lookup->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_post_prompt_lookup->no_display_name_list =
		dictionary_separate_no_display_name_list(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			dictionary_separate_post_prompt_lookup->
				no_display_dictionary );

	return dictionary_separate_post_prompt_lookup;
}

DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *
	dictionary_separate_post_prompt_lookup_calloc(
		void )
{
	DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *
		dictionary_separate_post_prompt_lookup;

	if ( ! ( dictionary_separate_post_prompt_lookup =
		   calloc(
			1,
			sizeof ( DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return dictionary_separate_post_prompt_lookup;
}

DICTIONARY_SEPARATE_LOOKUP_DELETE *
	dictionary_separate_lookup_delete_new(
		DICTIONARY *original_post_dictionary,
		char *application_name,
		char *login_name,
		LIST *folder_attribute_date_name_list,
		LIST *folder_attribute_append_isa_list )
{
	DICTIONARY_SEPARATE_TRIM_DOUBLE_BRACKET *trim_double_bracket;
	DICTIONARY_SEPARATE_PARSE_MULTI *parse_multi;
	DICTIONARY_SEPARATE_DATE_CONVERT *date_convert;
	DICTIONARY_SEPARATE_SQL_INJECTION_ESCAPE *sql_injection_escape;

	DICTIONARY_SEPARATE_LOOKUP_DELETE *
		dictionary_separate_lookup_delete =
			dictionary_separate_lookup_delete_calloc();

	if ( !dictionary_length( original_post_dictionary ) )
	{
		return dictionary_separate_lookup_delete;
	}

	trim_double_bracket =
		dictionary_separate_trim_double_bracket_new(
			original_post_dictionary );

	parse_multi =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_parse_multi_new(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			SQL_DELIMITER,
			trim_double_bracket->dictionary /* in/out */ );

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_date_convert_prompt_new(
			PROMPT_LOOKUP_FROM_PREFIX,
			PROMPT_LOOKUP_TO_PREFIX,
			application_name,
			login_name,
			folder_attribute_date_name_list,
			parse_multi->dictionary /* in/out */ );

	sql_injection_escape =
		dictionary_separate_sql_injection_escape_new(
			date_convert->dictionary,
			folder_attribute_append_isa_list );

	dictionary_separate_lookup_delete->query_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			sql_injection_escape->dictionary );

	dictionary_separate_lookup_delete->non_prefixed_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			sql_injection_escape->dictionary );

	return dictionary_separate_lookup_delete;
}

DICTIONARY_SEPARATE_LOOKUP_DELETE *
	dictionary_separate_lookup_delete_calloc(
		void )
{
	DICTIONARY_SEPARATE_LOOKUP_DELETE *
		dictionary_separate_lookup_delete;

	if ( ! ( dictionary_separate_lookup_delete =
		     calloc(
			1,
			sizeof ( DICTIONARY_SEPARATE_LOOKUP_DELETE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return dictionary_separate_lookup_delete;
}

DICTIONARY *dictionary_separate_no_display(
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_ignore_prefix,
		DICTIONARY *sql_injection_escape_dictionary )
{
	DICTIONARY *no_display_dictionary;

	no_display_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			dictionary_separate_no_display_prefix,
			sql_injection_escape_dictionary );

	if ( !dictionary_length( no_display_dictionary ) )
	{
		no_display_dictionary =
			/* ---------------------------------- */
			/* Returns dictionary_small() or null */
			/* ---------------------------------- */
			dictionary_separate_remove_prefix(
				dictionary_separate_ignore_prefix,
				sql_injection_escape_dictionary );
	}

	return no_display_dictionary;
}

DICTIONARY *dictionary_separate_query(
		const char *dictionary_separate_query_prefix,
		DICTIONARY *sql_injection_escape_dictionary,
		DICTIONARY *non_prefixed_dictionary )
{
	DICTIONARY *query_dictionary;

	query_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_separate_remove_prefix(
			dictionary_separate_query_prefix,
			sql_injection_escape_dictionary );

	if ( !dictionary_length( query_dictionary ) )
	{
		query_dictionary = non_prefixed_dictionary;
	}

	return query_dictionary;
}

char *dictionary_separate_relation_key(
		char *query_relation_operator_prefix,
		char *single_key )
{
	char key[ 128 ];

	if ( !single_key )
	{
		char message[ 128 ];

		sprintf(message, "single_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		key,
		sizeof ( key ),
		"%s%s",
		query_relation_operator_prefix,
		single_key );

	return strdup( key );
}

char *dictionary_separate_from_to_key(
		const char *prefix,
		char *attribute_name )
{
	static char key[ 128 ];

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	snprintf(
		key,
		sizeof ( key ),
		"%s%s",
		prefix,
		attribute_name );

	return key;
}

char *dictionary_separate_multi_prefix_skipped(
		char *prefix,
		char *multi_key )
{
	if ( !prefix )
		return multi_key;
	else
		return multi_key + strlen( prefix );
}

char *dictionary_separate_parse_multi_single_key(
		char *prefix,
		char *single_key )
{
	static char key[ 128 ];

	if ( !single_key )
	{
		char message[ 128 ];

		sprintf(message, "single_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strlen( single_key ) + string_strlen( prefix ) >= 128 )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter=[%s,%s] is too big.",
			(prefix) ? prefix : "null",
			single_key );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( prefix )
	{
		snprintf(
			key,
			sizeof ( key ),
			"%s%s",
			prefix,
			single_key );
	}
	else
	{
		strcpy( key, single_key );
	}

	return key;
}

char *dictionary_separate_parse_multi_index_key(
		char *prefix,
		int string_index,
		char *single_key )
{
	char index_key[ 128 ];
	char prefixed_index_key[ 256 ];

	if ( !single_key )
	{
		char message[ 128 ];

		sprintf(message, "single_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	if ( string_index > 0 )
	{
		snprintf(
			index_key,
			sizeof ( index_key ),
			"%s_%d",
			single_key,
			string_index );
	}
	else
	{
		string_strcpy( index_key, single_key, sizeof ( index_key ) );
	}

	if ( prefix )
	{
		snprintf(
			prefixed_index_key,
			sizeof ( prefixed_index_key ),
			"%s%s",
			prefix,
			index_key );
	}
	else
	{
		string_strcpy(
			prefixed_index_key,
			index_key,
			sizeof ( prefixed_index_key ) );
	}

	return strdup( prefixed_index_key );
}

void dictionary_separate_parse_multi_key(
		char *prefix,
		const char sql_delimiter,
		DICTIONARY *dictionary /* in/out */,
		int string_index,
		char *multi_key_without_prefix_index,
		char *multi_data )
{
	LIST *attribute_key_list;
	LIST *attribute_data_list;
	char *single_key;
	char *single_datum;
	char *index_key;

	if ( !dictionary_length( dictionary )
	||   !multi_key_without_prefix_index
	||   !multi_data )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	attribute_key_list =
		list_string_to_list(
			multi_key_without_prefix_index,
			sql_delimiter );

	attribute_data_list =
		list_string_to_list(
			multi_data,
			sql_delimiter );

	if ( !list_rewind( attribute_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "attribute_data_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( attribute_key_list ) )
	do {
		single_key = list_get( attribute_key_list );

		single_datum =
			list_get(
				attribute_data_list );

		single_key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			dictionary_separate_parse_multi_single_key(
				prefix,
				single_key );

		index_key =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			dictionary_separate_parse_multi_index_key(
				prefix,
				string_index,
				single_key );

		dictionary_set(
			dictionary,
			index_key,
			single_datum );

		/* Allow for multi-key but single datum */
		/* ------------------------------------ */
		if ( !list_at_end( attribute_data_list ) )
		{
			list_next( attribute_data_list );
		}

	} while ( list_next( attribute_key_list ) );

	list_free_string_list( attribute_key_list );
	list_free_container( attribute_data_list );
}

void dictionary_separate_parse_multi(
		char *prefix,
		const char sql_delimiter,
		DICTIONARY *dictionary /* in/out */,
		LIST *dictionary_key_list )
{
	char *multi_key;
	int index;
	char *prefix_skipped;
	char multi_key_without_prefix_index[ 256 ];
	char *multi_data;

	if ( list_rewind( dictionary_key_list ) )
	do {
		multi_key = list_get( dictionary_key_list );

		if ( !string_character_boolean(
			multi_key,
			sql_delimiter ) )
		{
			continue;
		}

		if ( prefix
		&&   string_strncmp( multi_key, prefix ) != 0 )
		{
			continue;
		}

		index = string_index( multi_key );

		prefix_skipped =
			/* --------------------------------------------- */
			/* Returns multi_key or a component of multi_key */
			/* --------------------------------------------- */
			dictionary_separate_multi_prefix_skipped(
				prefix,
				multi_key );

		/* ------------------- */
		/* Returns destination */
		/* ------------------- */
		(void)string_trim_index(
			multi_key_without_prefix_index
				/* destination */,
			prefix_skipped );

		if ( ! ( multi_data =
				dictionary_get(
					multi_key,
					dictionary ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"dictionary_get(%s) returned empty.",
				multi_key );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		dictionary_separate_parse_multi_key(
			prefix,
			sql_delimiter,
			dictionary /* in/out */,
			index,
			multi_key_without_prefix_index,
			multi_data );

	} while ( list_next( dictionary_key_list ) );
}

LIST *dictionary_separate_no_display_name_list(
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_ignore_prefix,
		DICTIONARY *no_display_dictionary )
{
	LIST *no_display_name_list = list_new();
	LIST *key_list;
	char *key;
	char *without_no_display;
	char *without_ignore;

	key_list = dictionary_key_list( no_display_dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		without_no_display =
			/* ------------------------------------- */
			/* Returns string or component of string */
			/* ------------------------------------- */
			string_skip_prefix(
				dictionary_separate_no_display_prefix,
				key );

		without_ignore =
			string_skip_prefix(
				dictionary_separate_ignore_prefix,
				without_no_display );

		list_unique_set(
			no_display_name_list,
			without_ignore );

	} while ( list_next( key_list ) );

	if ( !list_length( no_display_name_list ) )
	{
		list_free( no_display_name_list );
		no_display_name_list = NULL;
	}

	return no_display_name_list;
}
