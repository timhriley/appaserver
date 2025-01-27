/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/dictionary.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "list.h"
#include "piece.h"
#include "sql.h"
#include "date_convert.h"
#include "hash_table.h"
#include "date.h"
#include "column.h"
#include "attribute.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "query.h"
#include "dictionary.h"

/* ------------------------------------------------------------ */
/* Sample:							*/
/* From:  "station^datatype_1=BA^stage"				*/
/* To:    "station_1=BA and datatype_1=stage"			*/
/* ------------------------------------------------------------ */
void dictionary_parse_multi_attribute_keys(
		DICTIONARY *dictionary, 
		char delimiter )
{
	LIST *key_list;
	char *key;
	char *data;
	char key_without_index[ 256 ];
	char key_with_index[ 128 ];
	LIST *attribute_key_list;
	LIST *attribute_data_list;
	int index;

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( string_exists_character( key, delimiter ) )
		{
			data =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
					key,
					dictionary );

			/* -------------------------------------------------- */
			/* Sample: attribute_name = "station" <-- returns -1  */
			/* Sample: attribute_name = "station_0" <-- returns 0 */
			/* Sample: attribute_name = "station_1" <-- returns 1 */
			/* -------------------------------------------------- */
			index = string_index( key );

			string_trim_index(
				key_without_index /* destination */,
				key );

			attribute_key_list = 
				list_string_to_list(
					key_without_index,
					delimiter );

			attribute_data_list = 
				list_string_to_list(
					data,
					delimiter );

			list_rewind( attribute_key_list );
			list_rewind( attribute_data_list );

			do {
				sprintf(key_with_index, 
				 	"%s_%d",
				 	(char *)list_get( attribute_key_list ),
					index );

				dictionary_set( 
					dictionary, 
					strdup( key_with_index ),
					list_get( attribute_data_list ) );

				list_next( attribute_data_list );

			} while( list_next( attribute_key_list ) );
		}
	} while( list_next( key_list ) );
}

void dictionary_parse_multi_attribute_relation_operator_keys(
		DICTIONARY *dictionary, 
		char multi_attribute_drop_down_delimiter )
{
	LIST *multi_attribute_key_list;
	LIST *attribute_key_list;
	char *full_key;
	char *key, *data;
	char full_parse_key[ 1024 ];
	char delimiter_string[ 2 ];

	/* Set the delimiter string */
	/* ------------------------ */
	*delimiter_string = multi_attribute_drop_down_delimiter;
	*(delimiter_string + 1) = '\0';

	multi_attribute_key_list = dictionary_key_list( dictionary );

	if ( list_rewind( multi_attribute_key_list ) )
	do {
		full_key = list_get( multi_attribute_key_list );

		/* Only parse relational operators */
		/* ------------------------------- */
		if ( string_strncmp(
			full_key,
			QUERY_RELATION_OPERATOR_PREFIX ) != 0 )
		{
			continue;
		}

		if ( string_instr( delimiter_string, full_key, 1 ) == -1 )
			continue;

		attribute_key_list = 
			list_string2list(
				full_key,
				multi_attribute_drop_down_delimiter );

		data = dictionary_data( dictionary, full_key );

		if ( list_rewind( attribute_key_list ) )
		do {
			key = list_get( attribute_key_list );

			if ( string_strncmp(
				key,
				QUERY_RELATION_OPERATOR_PREFIX ) == 0 )
			{
				strcpy( full_parse_key, key );
			}
			else
			{
				sprintf(full_parse_key,
					"%s%s",
					QUERY_RELATION_OPERATOR_PREFIX,
					key );
			}

			if ( dictionary_exists_key(
					dictionary,
					full_parse_key ) )
			{
				continue;
			}

			dictionary_set_pointer( 
					dictionary, 
					strdup( full_parse_key ),
					data );

		} while( list_next( attribute_key_list ) );

	} while( list_next( multi_attribute_key_list ) );
}

void dictionary_delete_keys_with_prefix(
		DICTIONARY *dictionary, 
		char *prefix )
{
	LIST *attribute_key_list;
	char *key;
	int str_len;

	attribute_key_list = dictionary_key_list( dictionary );

	if ( !( attribute_key_list
	&&      list_reset( attribute_key_list ) ) )
	{
		return;
	}
	
	str_len = strlen( prefix );

	do {
		key = list_get( attribute_key_list );

		if ( strncmp( key, prefix, str_len ) == 0 )
		{
			dictionary_delete_key( dictionary, key );
			continue;
		}

	} while( list_next( attribute_key_list ) );
}

boolean dictionary_key_boolean(
		DICTIONARY *dictionary,
		char *search_key )
{
	return
	dictionary_exists_key( dictionary, search_key );
}

boolean dictionary_key_exists(
		DICTIONARY *dictionary,
		char *search_key )
{
	return
	dictionary_exists_key( dictionary, search_key );
}

boolean dictionary_exists_key(
		DICTIONARY *dictionary,
		char *search_key )
{
	char *data;

	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	data = dictionary_get( search_key, dictionary );

	if ( data )
		return 1;
	else
		return 0;
}

boolean dictionary_populated_key_exists_index_one(
		DICTIONARY *dictionary,
		char *key )
{
	char *data;
	char dictionary_key[ 1024 ];

	sprintf(dictionary_key,
	 	"%s_1",
	 	key );
	data = dictionary_get_pointer( dictionary, dictionary_key );

	if ( !data || !*data )
		return 0;
	else
		return 1;

}

boolean dictionary_populated_key_exists_index_zero(
		DICTIONARY *dictionary,
		char *key )
{
	char *data;
	char dictionary_key[ 1024 ];

	sprintf(dictionary_key,
	 	"%s_0",
	 	key );

	data =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			dictionary_key,
			dictionary );

	if ( !data || !*data )
		return 0;
	else
		return 1;

}

boolean dictionary_non_populated_key_exists_index_zero(
		DICTIONARY *dictionary,
		char *key )
{
	char *data;
	char dictionary_key[ 1024 ];

	sprintf(dictionary_key,
	 	"%s_0",
	 	key );

	data = dictionary_get_pointer( dictionary, dictionary_key );

	if ( !data || !*data )
		return 1;
	else
		return 0;

}

boolean dictionary_exists_key_index(
		DICTIONARY *dictionary,
		char *search_key,
		int row )
{
	char *data;
	char dictionary_key[ 1024 ];

	sprintf(dictionary_key, 
	 	"%s_%d",
	 	search_key, row );

	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	data = dictionary_get( dictionary_key, dictionary );

	if ( !data && row == 0 )
	{
		sprintf(dictionary_key, 
		 	"%s",
		 	search_key );

		data = dictionary_get( dictionary_key, dictionary );
	}

	if ( data )
		return 1;
	else
		return 0;
}

int dictionary_key_list_highest_index(
		const char sql_delimiter,
		LIST *key_list,
		DICTIONARY *dictionary,
		int dictionary_highest_index )
{
	int highest_index = 0;
	int index;
	char *key_list_string;
	char search_key[ 1024 ];

	if ( !list_length( key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( dictionary_highest_index < 0 ) return -1;

	key_list_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			key_list,
			sql_delimiter );

	if ( dictionary_key_exists(
		dictionary,
		key_list_string ) )
	{
		return 0;
	}

	for (	index = 1;
		index <= dictionary_highest_index;
		index++ )
	{
		snprintf(
			search_key,
			sizeof ( search_key ),
			"%s_%d",
			key_list_string,
			index );

		if ( dictionary_key_exists(
			dictionary,
			search_key ) )
		{
			highest_index = index;
		}
	}

	return highest_index;
}

int dictionary_highest_index( DICTIONARY *dictionary )
{
	char *key;
	int highest_index = 0;
	int index;
	LIST *key_list;

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		/* -------------------------------------------------- */
		/* Sample: attribute_name = "station"   <-- returns 0  */
		/* Sample: attribute_name = "station_0" <-- returns 0 */
		/* Sample: attribute_name = "station_1" <-- returns 1 */
		/* -------------------------------------------------- */
		index = string_index( key );

		if ( index > highest_index )
		{
			highest_index = index;
		}

	} while( list_next( key_list ) );

	list_free_container( key_list );
	return highest_index;
}

int dictionary_prefix_highest_row(
		char *prefix,
		DICTIONARY *dictionary )
{
	char *key;
	int highest_row = -1;
	int row;
	LIST *key_list;

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( string_strncmp( key, prefix ) != 0 ) continue;

		row = string_row_number( key );

		if ( row > -1 )
		{
			if ( row > highest_row )
				highest_row = row;
		}
	} while( list_next( key_list ) );

	return highest_row;
}

char *dictionary_display_delimited(
		DICTIONARY *dictionary,
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter )
{
	char buffer[ STRING_SIZE_HASH_TABLE ];

	if ( !dictionary_length( dictionary ) ) return strdup( "" );

	return
	strdup(
		hash_table_display_delimiter(
			buffer, 
			dictionary->hash_table,
			dictionary_attribute_datum_delimiter,
			dictionary_element_delimiter,
			STRING_SIZE_HASH_TABLE /* buffer_size */ ) );
}

char *dictionary_display( DICTIONARY *dictionary )
{
	/* Returns heap memory */
	/* ------------------- */
	return
	dictionary_display_delimited(
		dictionary,
		DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
		DICTIONARY_ELEMENT_DELIMITER );
}

DICTIONARY *dictionary_string_resolve( char *dictionary_string )
{
	return
	dictionary_string_new(
		DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
		DICTIONARY_ELEMENT_DELIMITER,
		dictionary_string );
}

/* Sample: "datatype=salinity&station=BA&grade_1=a" */
/* ------------------------------------------------ */
DICTIONARY *dictionary_string_new(
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter,
		char *dictionary_string )
{
	DICTIONARY *dictionary;
        char attribute[ 1024 ];
	char element[ STRING_65K ];
        char datum[ STRING_64K ];
	int i;

	if ( !dictionary_string || !*dictionary_string ) return NULL;

	dictionary = dictionary_large();

 	for(	i = 0;
		piece(	element,
			dictionary_element_delimiter,
			dictionary_string,
			i );
		i++ )
        {
                piece(	attribute,
			dictionary_attribute_datum_delimiter,
			element,
			0 );

                piece(	datum,
			dictionary_attribute_datum_delimiter,
			element,
			1 );

		if ( *datum )
                {
			dictionary_set(
				dictionary, 
				strdup( attribute ),
				strdup( datum ) );
		}
	}

	return dictionary;
}

void dictionary_add( DICTIONARY *d, char *key, char *data, int size )
{
	dictionary_set_other_data( d, key, data, size );
}

int dictionary_all_null_data( DICTIONARY *d )
{
	return hash_table_all_null_data( d->hash_table );
}

DICTIONARY *dictionary_small()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_small );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
}

DICTIONARY *dictionary_new_huge_dictionary()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_huge );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
}

DICTIONARY *dictionary_super()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_super );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
}

DICTIONARY *dictionary_duper()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_duper );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
}

DICTIONARY *dictionary_calloc( void )
{
	DICTIONARY *dictionary;

	if ( ! ( dictionary = calloc( 1, sizeof ( DICTIONARY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return dictionary;
}

DICTIONARY *dictionary_large()
{
	DICTIONARY *dictionary =  dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_large );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
}

DICTIONARY *dictionary_huge()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_huge );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
}

DICTIONARY *dictionary_medium()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_medium );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
}

DICTIONARY *dictionary_new()
{
	return dictionary_medium();
}

int dictionary_len( DICTIONARY *d )
{
	return hash_table_len( d->hash_table );
}

int dictionary_length( DICTIONARY *d )
{
	if ( !d )
		return 0;
	else
		return hash_table_len( d->hash_table );
}

void dictionary_set_pointer(
		DICTIONARY *d,
		char *key,
		char *datum )
{
	dictionary_set( d, key, datum );
}

void dictionary_set_string(
		DICTIONARY *d,
		char *key,
		char *datum )
{
	dictionary_set(	d,
			strdup( key ),
			strdup( datum ) );
}

void dictionary_set(
		DICTIONARY *d,
		char *key,
		char *datum )
{
	if ( !d ) return;
	if ( !d->hash_table ) return;
	if ( !key ) return;

	if ( !datum ) datum = strdup( "" );

	hash_table_set( d->hash_table, key, datum );
}

void dictionary_set_other_data(
		DICTIONARY *d, 
		char *key, 
		void *other_data, 
		int sizeof_other_data )
{
	char *memory = (char *)calloc( 1, sizeof_other_data );

	if ( !memory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc(%d) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			sizeof_other_data );
		exit( 1 );
	}

	memcpy( memory, other_data, sizeof_other_data );
	hash_table_set( d->hash_table, key, memory );
}

void dictionary_add_pointer(
		DICTIONARY *d, 
		char *key, 
		void *data )
{
	if ( d && key && data ) hash_table_set( d->hash_table, key, data );

}

char *dictionary_pointer( char *key, DICTIONARY *d )
{
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	return dictionary_get( key, d );
}

char *dictionary_get_pointer( DICTIONARY *d, char *key )
{
	return dictionary_get( key, d );
}

char *dictionary_data( DICTIONARY *d, char *key )
{
	return dictionary_get( key, d );
}

char *dictionary_string( DICTIONARY *d, char *key )
{
	return dictionary_get( key, d );
}

char *dictionary_seek( char *key, DICTIONARY *d )
{
	return dictionary_get( key, d );
}

char *dictionary_index_get(
		char *key,
		DICTIONARY *dictionary,
		int index )
{
	char *append_row_number;

	append_row_number =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		dictionary_attribute_name_append_row_number(
			key /* attribute_name */,
			index /* row_number */ );

	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		append_row_number,
		dictionary );
}

char *dictionary_get(
		char *key,
		DICTIONARY *d )
{
	int duplicate_indicator = 0;

	if ( !key || !d ) return (char *)0;

	return
	(char *)hash_table_retrieve_other_data( 
		d->hash_table, 
		key,
		&duplicate_indicator );
}

char *dictionary_list_get(
		DICTIONARY *dictionary,
		LIST *key_list )
{
	char *key;
	char *get;

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( ( get =
			dictionary_get(
				key,
				dictionary ) ) )
		{
			return get;
		}

	} while ( list_next( key_list ) );

	return NULL;
}

void dictionary_free_data( DICTIONARY *d, LIST *key_list )
{
	char *key;
	char *data;

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( ( data = dictionary_get(
				key, d ) ) )
		{
			free( data );
		}
	} while( list_next( key_list ) );
}

void dictionary_free_container( DICTIONARY *dictionary )
{
	hash_table_free_container( dictionary->hash_table );
	free( dictionary );
}

void dictionary_free( DICTIONARY *d )
{
	char *key;
	LIST *key_list;

	key_list = dictionary_key_list( d );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		free( dictionary_get( key, d ) );

	} while( list_next( key_list ) );

	list_free( key_list );
	dictionary_free_container( d );
}

LIST *dictionary_ordered_key_list( DICTIONARY *d )
{
	return hash_table_ordered_key_list( d->hash_table );
}

LIST *dictionary_key_list( DICTIONARY *d )
{
	if ( !d ) return (LIST *)0;
	return hash_table_key_list( d->hash_table );
}

void dictionary_replace_command_line( 	
		char *command_line,
		DICTIONARY *dictionary )
{
	int highest_index;
	LIST *list;
	char *column;
	char *get;

	highest_index = dictionary_highest_index( dictionary );

	list = column_list( command_line );

	if ( list_length( list ) <= 1 ) return;

	list_rewind( list );

	/* Skip the executable */
	/* ------------------- */
	list_next( list );

	do {
		column = list_get( list );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		if ( ( get = dictionary_get( column, dictionary ) ) )
		{
			string_replace_command_line(
				command_line,
				get /* string */, 
				column /* placeholder */ );
		}
		else
		if ( highest_index )
		{
			LIST *data_list =
				dictionary_indexed_data_list(
					dictionary,
					highest_index,
					column /* key_prefix */ );

			if ( list_length( data_list ) )
			{
				char *tmp;

				string_replace_command_line(
					command_line,
					/* --------------------------- */
					/* Returns heap memory or null */
					/* --------------------------- */
					( tmp = list_delimited_string(
							data_list,
							SQL_DELIMITER ) ),
						column /* placeholder */ );

				free( tmp );
			}
		}

	} while( list_next( list ) );
}

DICTIONARY *dictionary_copy_dictionary( DICTIONARY *dictionary )
{
	return dictionary_large_copy( dictionary );
}

DICTIONARY *dictionary_copy( DICTIONARY *dictionary )
{
	return dictionary_large_copy( dictionary );
}

void dictionary_set_delimited_string(
		DICTIONARY *dictionary,
		LIST *key_list,
		char *delimited_string,
		char delimiter )
{
	char *key;
	char piece_buffer[ 65536 ];
	int piece_offset = 0;

	if ( list_reset( key_list ) )
	do {
		key = list_get( key_list );

		if ( piece(	piece_buffer,
				delimiter,
				delimited_string,
				piece_offset ) )
		{
			dictionary_set(
				dictionary,
				key,
				strdup( piece_buffer ) );

			piece_offset++;
		}
	} while( list_next( key_list ) );
}

DICTIONARY *copy_dictionary( DICTIONARY *dictionary )
{
	return dictionary_large_copy( dictionary );
}

DICTIONARY *dictionary_append(
		DICTIONARY *destination_dictionary,
		DICTIONARY *source_dictionary )
{
	LIST *key_list;
	char *key;

	key_list = dictionary_key_list( source_dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		dictionary_set(
			destination_dictionary,
			strdup( key ),
			strdup(
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
				dictionary_get(
					key,
					source_dictionary ) ) );

	} while( list_next( key_list ) );

	list_free_container( key_list );
	return destination_dictionary;
}

DICTIONARY *dictionary_small_copy( DICTIONARY *dictionary )
{
	DICTIONARY *destination;
	LIST *key_list;
	char *key;

	destination = dictionary_small();

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		dictionary_set(
			destination,
			strdup( key ),
			strdup(
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
				dictionary_get(
					key,
					dictionary ) ) );

	} while( list_next( key_list ) );

	list_free_container( key_list );
	return destination;
}

DICTIONARY *dictionary_medium_copy( DICTIONARY *dictionary )
{
	DICTIONARY *destination;
	LIST *key_list;
	char *key;

	destination = dictionary_medium();

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		dictionary_set(
			destination,
			strdup( key ),
			strdup(
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
				dictionary_get(
					key,
					dictionary ) ) );

	} while( list_next( key_list ) );

	list_free_container( key_list );
	return destination;
}

DICTIONARY *dictionary_large_copy( DICTIONARY *dictionary )
{
	DICTIONARY *destination;
	LIST *key_list;
	char *key;

	destination = dictionary_large();

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		dictionary_set(
			destination,
			strdup( key ),
			strdup(
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
				dictionary_get(
					key,
					dictionary ) ) );

	} while( list_next( key_list ) );

	list_free_container( key_list );
	return destination;
}

/* ----------------------------------------------------------------- 
Two notes:
1) No memory gets allocated.
2) If a they share a key, then it clobbers the source_destinatation.
-------------------------------------------------------------------- */
DICTIONARY *dictionary_append_dictionary(
		DICTIONARY *source_destination,
		DICTIONARY *append_dictionary )
{
	LIST *key_list;
	char *key;

	if ( !append_dictionary ) return source_destination;
	if ( !source_destination ) return source_destination;

	key_list = dictionary_key_list( append_dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		dictionary_set(
			source_destination,
			key,
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				append_dictionary ) );

	} while( list_next( key_list ) );

	return source_destination;
}

DICTIONARY *dictionary_set_dictionary(
		DICTIONARY *source_destination,
		DICTIONARY *append_dictionary )
{
	return
	dictionary_append_dictionary(
		source_destination,
		append_dictionary );
}

DICTIONARY *dictionary_pipe(
		char *system_string,
		char delimiter )
{
	char buffer[ 65536 ];
	char data[ 65536 ];
	char key[ 1024 ];
	FILE *p = popen( system_string, "r" );
	DICTIONARY *d = dictionary_large();

	*data = '\0';

	while( string_input( buffer, p, 65536 ) )
	{
		if ( delimiter )
		{
			piece( key, delimiter, buffer, 0 );

			if ( !piece( data, delimiter, buffer, 1 ) )
				*data = '\0';
		}
		else
		{
			strcpy( key, buffer );
		}

		dictionary_set( d, strdup( key ), strdup( data ) );
	}

	pclose( p );
	return d;
}

DICTIONARY *dictionary_name_list_fetch(
		char *system_string,
		LIST *name_list,
		char delimiter )
{
	char *buffer;
	DICTIONARY *dictionary = {0};

	if ( !system_string
	||   !list_rewind( name_list )
	||   !delimiter )
	{
		return NULL;
	}

	/* Returns heap memory or null */
	/* --------------------------- */
	buffer = string_pipe( system_string );

	if ( buffer )
	{
		int p;
		char data[ 65536 ];

		dictionary = dictionary_small();

		for (	p = 0;
			piece( data, delimiter, buffer, p );
			p++ )
		{
			if ( *data )
			{
				dictionary_set(
					dictionary,
					list_get( name_list ) /* key */,
					strdup( data ) );
			}

			if ( !list_next( name_list ) ) break;
		}

		free( buffer );
	}

	return dictionary;
}

DICTIONARY *dictionary_merge_dictionary(
		DICTIONARY *dictionary1,
		DICTIONARY *dictionary2 )
{
	LIST *key_list;
	char *key;

	key_list = dictionary_key_list( dictionary2 );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( !dictionary_exists_key(
			dictionary1,
			key ) )
		{
			dictionary_set(
				dictionary1,
				key,
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
				dictionary_get(
					key,
					dictionary2 ) );
		}
	} while( list_next( key_list ) );

	return dictionary1;
}

DICTIONARY *dictionary_merge_lists2dictionary(
		LIST *key_list,
		LIST *data_list )
{
	char *key;
	DICTIONARY *dictionary;

	dictionary = dictionary_new();

	if ( list_rewind( key_list ) && list_rewind( data_list ) )
	do {
		key = list_get( key_list );

		dictionary_set(
			dictionary,
			key,
			list_get( data_list ) );

		if ( !list_next( data_list ) ) break;

	} while( list_next( key_list ) );

	return dictionary;
}

int dictionary_remove_key(
		DICTIONARY *d,
		char *key )
{
	return hash_table_remove_data( d->hash_table, key );
}

int dictionary_remove_data(
		DICTIONARY *d,
		char *key )
{
	return hash_table_remove_data( d->hash_table, key );
}

DICTIONARY *dictionary_search_replace_special_characters(
		DICTIONARY *dictionary )
{
	LIST *key_list;
	char *key;
	char *data;
	char *compare_data;

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		compare_data = data = dictionary_get( key, dictionary );

		string_search_replace_special_characters( data );

		if ( strcmp( compare_data, data ) != 0 )
		{
			dictionary_set( 
				dictionary,
				key,
				data );				
		}

	} while( list_next( key_list ) );

	return dictionary;
}

DICTIONARY *dictionary_subtract_dictionary(
		DICTIONARY *d1,
		DICTIONARY *d2 )
{
	LIST *d2_key_list;

	d2_key_list = dictionary_key_list( d2 );

	if ( list_rewind( d2_key_list ) )
	do {
		dictionary_remove_data(
			d1,
			list_get( d2_key_list ) );

	} while( list_next( d2_key_list ) );

	return d1;
}

boolean dictionary_exists_suffix( char *key )
{
	return dictionary_exists_index_in_key( key );
}

boolean dictionary_exists_index_in_key( char *key )
{
	char *end = key + strlen( key ) - 1;

	while( end != key )
	{
		if ( *end == '_' ) return 1;
		if ( !isdigit( *end ) ) return 0;
		end--;
	}
	return 0;
}

void dictionary_replace_double_quote_with_single(
		DICTIONARY *dictionary )
{
	char *key;
	char *data;
	LIST *key_list;

	key_list = dictionary_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary );

		string_search_replace( data, "\"", "'" );

	} while( list_next( key_list ) );
}

void dictionary_escape_single_quotes( DICTIONARY *dictionary )
{
	char *key;
	char *data;
	LIST *key_list;
	char new_data[ 4096 ];

	key_list = dictionary_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary );

		if ( string_character_count( '\'', data ) > 0 )
		{
			strcpy(
				new_data,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_escape_single_quotes( data ) );

			free( data );

			dictionary_set_pointer(
				dictionary,
				key,
				strdup( new_data ) );

		}
	} while( list_next( key_list ) );
}

LIST *dictionary_indexed_data_list(
		DICTIONARY *dictionary,
		int highest_index,
		char *key_prefix )
{
	char full_key[ 512 ];
	char *data;
	LIST *data_list = {0};
	int index;

	for ( index = 1; index <= highest_index; index++ )
	{
		sprintf(full_key,
			"%s_%d",
			key_prefix,
			index );

		if ( ( data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				full_key,
				dictionary ) ) )
		{
			if ( !data_list ) data_list = list_new();

			list_set( data_list, data );
		}
	}

	return data_list;
}

LIST *dictionary_populated_index_list(
		DICTIONARY *dictionary,
		char *key_prefix )
{
	char full_key[ 512 ];
	char *data;
	LIST *populated_index_list = list_new();
	int highest_index;
	int index;
	char index_string[ 64 ];

	highest_index = dictionary_highest_index( dictionary );

	for ( index = 1; index <= highest_index; index++ )
	{
		sprintf(full_key,
			"%s_%d",
			key_prefix,
			index );

		if ( ( data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				full_key,
				dictionary ) ) )
		{
			sprintf( index_string, "%d", index );

			list_set(
				populated_index_list,
				strdup( index_string ) );
		}
	}
	return populated_index_list;
}

LIST *dictionary_with_populated_index_list_index_data_list(
		DICTIONARY *dictionary,
		LIST *populated_index_list,
		char *key_prefix )
{
	char full_key[ 512 ];
	char *data;
	LIST *index_data_list = list_new_list();
	char *index;

	if ( list_rewind( populated_index_list ) )
	{
		do {
			index = list_get( populated_index_list );

			sprintf(full_key,
				"%s_%s",
				key_prefix,
				index );

			if ( ( data =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
					full_key,
					dictionary ) ) )
			{
				list_set(
					index_data_list,
					data );
			}
			else
			{
				list_set(
					index_data_list,
					"" );
			}
		} while( list_next( populated_index_list ) );
	}
	return index_data_list;
}

DICTIONARY *dictionary_record_to_dictionary(
		LIST *key_list,
		char *record,
		char delimiter )
{
	return dictionary_merge_lists2dictionary(
				key_list,
				list_string_to_list( record, delimiter ) );
}

void dictionary_trim_double_bracked_data( DICTIONARY *dictionary )
{
	char *key;
	char *data;
	LIST *key_list;

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get( key_list );

			data =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
					key,
					dictionary );

			dictionary_trim_double_bracked_string( data );

		} while( list_next( key_list ) );
	}
}

char *dictionary_trim_double_bracked_string( char *string )
{
	int start, end;
	int str_len;

	str_len = strlen( string );

	/* Don't trim middle brackets. */
	/* --------------------------- */
	if ( str_len
	&&   *( string + str_len - 1 ) == ']' )
	{
		if ( ( start = string_instr( "[", string, 1 ) ) != -1
		&&   ( end = string_instr( "]", string, 1 ) ) != -1 )
		{
			strcpy( string + start, string + end + 1 );

			/* Returns buffer with end shortened (maybe) */
			/* ----------------------------------------- */
			string_trim_right_spaces(
				string /* buffer */ );
		}
	}

	return string;
}

int dictionary_delete_key( DICTIONARY *dictionary, char *key )
{
	return hash_table_remove_data( dictionary->hash_table, key );
}

int dictionary_delete( DICTIONARY *dictionary, char *key )
{
	return hash_table_remove_data( dictionary->hash_table, key );
}

void dictionary_delete_prefixed_keys(
		DICTIONARY *dictionary,
		char *prefix_string )
{
	LIST *key_list;
	char *key;
	int str_len;

	str_len = strlen( prefix_string );

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get( key_list );

			if ( strncmp( key, prefix_string, str_len ) == 0 )
			{
				dictionary_delete( dictionary, key );
			}
		} while( list_next( key_list ) );
	}
}

#ifdef NOT_DEFINED
DICTIONARY *dictionary_load_record_dictionary(
		FILE *input_pipe,
		int delimiter )
{
	char buffer[ 4096 ];
	char key[ 1024 ];
	char data[ 3072 ];
	char first_key[ 1024 ];
	DICTIONARY *d = (DICTIONARY *)0;
	int first_time = 1;

	while( get_line_queue( buffer, input_pipe ) )
	{
		piece( key, delimiter, buffer, 0 );

		if ( first_time )
		{
			d = dictionary_new();
			strcpy( first_key, key );
			first_time = 0;
		}
		else
		{
			if ( strcmp( key, first_key ) == 0 )
			{
				unget_line_queue( buffer );
				break;
			}
		}
		
		if ( piece( data, delimiter, buffer, 1 ) )
		{
			dictionary_set_pointer(
				d,
				strdup( key ),
				strdup( data ) );
		}
	}
	return d;
}
#endif

char *dictionary_fetch( char *key, DICTIONARY *d )
{
	if ( !d ) return (char *)0;

	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	return dictionary_get( key, d );
}

char *dictionary_safe_fetch( char *key, DICTIONARY *d )
{
	char *results;

	if ( !d ) return "";

	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	if ( ( results = dictionary_get( key, d ) ) )
		return results;
	else
		return "";
}

void dictionary_increment_count(
		DICTIONARY *dictionary,
		char *key )
{
	char *data_pointer;
	char count_string[ 32 ];
	int current_count;

	if ( ( data_pointer = dictionary_fetch( key, dictionary ) ) )
	{
		current_count = atoi( data_pointer );
		sprintf( count_string, "%d", current_count + 1 );
		free( data_pointer );
		data_pointer = strdup( count_string );
	}
	else
	{
		dictionary_set_pointer(
			dictionary,
			strdup( key ),
			strdup( "1" ) );
	}
}

void dictionary_output_with_prefix(
		DICTIONARY *dictionary,
		char *prefix )
{
	char *key;
	LIST *key_list;
	char *get;

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary );

		if ( !get )
		{
			char message[ 128 ];

			sprintf(message,
				"dictionary_get(%s) returned empty.",
				key );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		printf(	"%s%s=%s\n",
			prefix,
			key,
			get );

	} while( list_next( key_list ) );
}

void dictionary_output_html_table(
		DICTIONARY *dictionary,
		char *heading1,
		char *heading2,
		boolean align_right )
{
	char *key;
	LIST *key_list;
	char *data;

	key_list = dictionary_key_list( dictionary );

	printf( "<table border=1>\n" );

	printf( "<th>%s", heading1 );

	if ( align_right )
		printf( "<th align=right>%s\n", heading2 );
	else
		printf( "<th>%s\n", heading2 );

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get( key_list );

			data =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				(char *)dictionary_get(
					key,
					dictionary );

			/* Should always be true, but gcc warns otherwise. */
			/* ----------------------------------------------- */
			if ( data )
			{
				printf( "<tr><td>%s</td>\n",
					key );

				if ( align_right )
				{
					printf( "<td align=right>%s</td>\n",
					data );
				}
				else
				{
					printf( "<td>%s</td>\n",
						data );
				}
			}

		} while( list_next( key_list ) );
	}
	printf( "</table>\n" );
}

void dictionary_convert_index_to_index_zero(
		DICTIONARY *dictionary,
		int index )
{
	char *key;
	LIST *key_list;
	int key_index;
	char *data;
	char new_key[ 1026 ];
	char key_without_index[ 1024 ];

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		/* -------------------------------------------------- */
		/* Sample: attribute_name = "station" <-- returns -1  */
		/* Sample: attribute_name = "station_0" <-- returns 0 */
		/* Sample: attribute_name = "station_1" <-- returns 1 */
		/* -------------------------------------------------- */
		key_index = string_index( key );

		if ( key_index == index )
		{
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			data = dictionary_get( key, dictionary );

			string_trim_index( key_without_index, key );

			sprintf(new_key, 
 				"%s_0",
 				key_without_index );

			dictionary_set( 
				dictionary, 
				strdup( new_key ),
				data );

		}
	} while( list_next( key_list ) );
}

DICTIONARY *dictionary_prefix(
		DICTIONARY *dictionary,
		char *prefix )
{
	char *key;
	LIST *key_list;
	char new_key[ 1024 ];
	DICTIONARY *prefix_dictionary;
	char *get;

	if ( !dictionary ) return (DICTIONARY *)0;

	prefix_dictionary = dictionary_medium();

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		sprintf( new_key, "%s%s", prefix, key );

		get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary );

		if ( !get )
		{
			char message[ 128 ];

			sprintf(message,
				"dictionary_get(%s) returned empty.",
				key );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		dictionary_set(
			prefix_dictionary,
			strdup( new_key ),
			get );

	} while( list_next( key_list ) );

	list_free_container( key_list );
	return prefix_dictionary;
}

LIST *dictionary_key_data_list(
		DICTIONARY *dictionary,
		LIST *key_list,
		char delimiter )
{
	char *data, *key;
	char key_data[ 1024 ];
	LIST *list = list_new_list();

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( ( data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary ) ) )
		{
			sprintf( key_data,
				 "%s%c%s",
				 key,
				 delimiter,
				 data );

			list_set( list, strdup( key_data ) );
		}

	} while( list_next( key_list ) );

	return list;
}

void dictionary_clear_key_list(
		DICTIONARY *dictionary,
		LIST *key_list )
{
	char *data, *key;

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( ( data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary ) ) )
		{
			hash_table_remove(
				dictionary->hash_table,
				key );
		}

	} while( list_next( key_list ) );
}

void dictionary_clear_key_list_index_zero(
		DICTIONARY *dictionary,
		LIST *key_list )
{
	char *data, *key;
	char indexed_key[ 512 ];

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		sprintf( indexed_key, "%s_0", key );

		if ( ( data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				indexed_key,
				dictionary ) ) )
		{
			hash_table_remove(
				dictionary->hash_table,
				indexed_key );
		}

	} while( list_next( key_list ) );
}

void dictionary_trim_prefix(
		DICTIONARY *dictionary,
		char *prefix )
{
	LIST *key_list;
	char *key;
	char *data;
	char key_without_starting_prefix[ 1024 ];
	int str_len;

	str_len = strlen( prefix );

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		data = dictionary_get( key, dictionary );

		if ( strncmp( prefix, key, str_len ) == 0 )
		{
			strcpy( key_without_starting_prefix,
				key + str_len );

			dictionary_set( 
				dictionary,
				strdup( key_without_starting_prefix ),
				data );

			dictionary_delete( dictionary, key );
		}
	} while( list_next( key_list ) );
}

DICTIONARY *dictionary_zap_index_zero_attribute_name_list(
		DICTIONARY *dictionary,
		LIST *attribute_name_list )
{
	char *attribute_name;
	char dictionary_key[ 1024 ];

	if ( list_rewind( attribute_name_list ) )
	do {
		attribute_name =
			list_get( attribute_name_list );

		if ( dictionary_populated_key_exists_index_zero(
				dictionary,
				attribute_name ) )
		{
			sprintf(dictionary_key,
	 			"%s_0",
	 			attribute_name );

			hash_table_remove(
				dictionary->hash_table,
				dictionary_key );

			hash_table_remove(
				dictionary->hash_table,
				attribute_name );
		}
	} while( list_next( attribute_name_list ) );

	return dictionary;
}

void dictionary_zap_where_key_contains_character(
		DICTIONARY *dictionary,
		char character )
{
	LIST *key_list;
	char *key;

	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( string_exists_character( key, character ) )
		{
			dictionary_delete( dictionary, key );
		}
	} while( list_next( key_list ) );
}

void dictionary_list_output_to_file(
		char *output_filename, 
		LIST *row_dictionary_list,
		LIST *attribute_name_list,
		char *heading_display )
{
	DICTIONARY *row_dictionary;
	FILE *output_file;

	output_file = fopen( output_filename, "w" );

	if ( !output_file )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot open %s\n",
			 __FILE__,
			 __FUNCTION__,
			 output_filename );
		exit( 1 );
	}

	if ( heading_display && *heading_display )
		fprintf( output_file, "%s\n", heading_display );

	dictionary_output_heading_to_file(
		output_file,
		attribute_name_list );

	if ( list_rewind( row_dictionary_list ) )
	do {
		row_dictionary = list_get( row_dictionary_list );

		dictionary_row_output_to_file(
			output_file,
			row_dictionary,
			attribute_name_list );

	} while( list_next( row_dictionary_list ) );

	fclose( output_file );
}

void dictionary_row_output_to_file(
		FILE *output_file, 
		DICTIONARY *row_dictionary,
		LIST *attribute_name_list )
{
	char *value;
	char *attribute_name;
	int first_time = 1;

	if ( !row_dictionary ) return;

	if ( list_rewind( attribute_name_list ) )
	do {
		attribute_name = list_get( attribute_name_list );

		value =
			dictionary_safe_fetch(
				attribute_name,
				row_dictionary );

		if ( first_time )
		{
			first_time = 0;
			fprintf( output_file, "\"%s\"", value );
		}
		else
		{
			fprintf( output_file,
				 ",\"%s\"",
				 value );
		}
	} while( next_item( attribute_name_list ) );

	fprintf( output_file, "\n" );
}

void dictionary_output_heading_to_file(
		FILE *output_file, 
		LIST *attribute_name_list )
{
	char *attribute_name;
	int first_time = 1;

	if ( list_rewind( attribute_name_list ) )
	do {
		attribute_name = list_get( attribute_name_list );

		if ( first_time )
		{
			first_time = 0;
			fprintf( output_file, "\"%s\"", attribute_name );
		}
		else
		{
			fprintf(output_file,
				",\"%s\"",
				attribute_name );
		}
	} while( next_item( attribute_name_list ) );

	fprintf( output_file, "\n" );
}

LIST *dictionary_seek_delimited_list(
		DICTIONARY *dictionary,
		char *key )
{
	char *delimited_data;

	if ( ! ( delimited_data = dictionary_seek( key, dictionary ) ) )
		return (LIST *)0;

	if ( !*delimited_data ) return (LIST *)0;

	return list_delimited_string_to_list( delimited_data );

}

LIST *dictionary_non_indexed_key_list(
		DICTIONARY *dictionary )
{
	LIST *non_indexed_key_list;
	LIST *key_list;
	char *key;
	char key_without_index[ 1024 ];

	key_list = dictionary_key_list( dictionary );

	non_indexed_key_list = list_new();

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		string_trim_index( key_without_index, key );

		if ( !list_exists_string(
			key_without_index,
			non_indexed_key_list ) )
		{
			list_set(
				non_indexed_key_list,
				strdup( key_without_index ) );
		}

	} while( list_next( key_list ) );

	if ( !list_length( non_indexed_key_list ) )
	{
		list_free( non_indexed_key_list );
		non_indexed_key_list = NULL;
	}

	return non_indexed_key_list;
}

DICTIONARY *dictionary_key_piece(
		DICTIONARY *dictionary,
		char delimiter,
		int piece_offset )
{
	LIST *key_list;
	char *key;
	char *data;
	char piece_buffer[ 1024 ];

	if ( !dictionary ) return (DICTIONARY *)0;

	key_list = dictionary_key_list( dictionary );

	if ( list_reset( key_list ) )
	do {
		key = list_get( key_list );

		data = dictionary_data( dictionary, key );

		if ( piece( piece_buffer, delimiter, key, piece_offset ) )
		{
			dictionary_set_pointer(
				dictionary,
				strdup( piece_buffer ),
				data );
		}

	} while( list_next( key_list ) );

	return dictionary;
}

void dictionary_output_as_hidden( DICTIONARY *dictionary )
{
	LIST *key_list = dictionary_key_list( dictionary );
	char *key;
	char *data;

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( ( data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary ) ) )
		{
			printf( 
		"<input name=\"%s\" type=\"hidden\" value=\"%s\">\n",
				key,
				data );
		}
	} while( list_next( key_list ) );

	list_free_container( key_list );
}

char *dictionary_row(
		char *attribute_name,
		int row,
		DICTIONARY *dictionary )
{
	char dictionary_key[ 1024 ];
	char *data;

	if ( !attribute_name ) return (char *)0;
	if ( !dictionary ) return (char *)0;

	/* Try to get the data with the row number */
	/* --------------------------------------- */
	sprintf(dictionary_key, 
	 	"%s_%d",
	 	attribute_name,
		row );

	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	if ( ! ( data = dictionary_get( dictionary_key, dictionary ) ) )
	{
		/* Try again with the row zero */
		/* --------------------------- */
		sprintf(dictionary_key, 
		 	"%s_0",
		 	attribute_name );

		if ( ! ( data = dictionary_get( dictionary_key, dictionary ) ) )
		{
			/* Try with the no row number */
			/* -------------------------- */
			data = dictionary_get( attribute_name, dictionary );
		}
	}
	return data;
}

LIST *dictionary_attribute_data_list(
		LIST *attribute_name_list,
		DICTIONARY *dictionary )
{
	LIST *data_list = list_new();
	char *data;

	if ( list_rewind( attribute_name_list ) )
	do {
		if ( ! ( data =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get( 
					(char *)list_get(
						attribute_name_list ),
					dictionary ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: dictionary_get(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				(char *)list_get( attribute_name_list ) );
			exit( 1 );
		}

		list_set( data_list, data );

	} while ( list_next( attribute_name_list ) );

	if ( !list_length( data_list ) )
	{
		list_free( data_list );
		data_list = NULL;
	}

	return data_list;
}

char *dictionary_attribute_name_append_row_number(
		char *attribute_name,
		int row_number )
{
	static char append_row_number[ 128 ];

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(append_row_number,
		"%s_%d",
		attribute_name,
		row_number );

	return append_row_number;
}

char *dictionary_attribute_name_list_string(
		DICTIONARY *dictionary,
		LIST *attribute_name_list,
		char sql_delimiter,
		int row_number )
{
	char *attribute_name;
	char *data;
	char *append_row_number;
	char list_string[ STRING_128K ];
	char *ptr = list_string;

	*ptr = '\0';

	if ( list_rewind( attribute_name_list ) )
	do {
		attribute_name = list_get( attribute_name_list );

		if ( ! ( data =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get( 
					attribute_name,
					dictionary ) ) )
		{
			fprintf(stderr,
		"Warning in %s/%s()/%d: dictionary_get(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				attribute_name );
			continue;
		}

		append_row_number =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			dictionary_attribute_name_append_row_number(
				attribute_name,
				row_number );

		ptr += sprintf(
			ptr,
			"%s%c%s\n",
			append_row_number,
			sql_delimiter,
			data );

	} while ( list_next( attribute_name_list ) );

	if ( !*list_string )
		return NULL;
	else
		return strdup( list_string );
}

DICTIONARY *dictionary_row_fetch(
		LIST *attribute_name_list,
		char *folder_table_name,
		char *where )
{
	char *select_string;
	char *system_string;
	char *input;

	if ( !list_length( attribute_name_list )
	||   !folder_table_name
	||   !where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		dictionary_select_attribute_string(
			attribute_name_list );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		dictionary_system_string(
			select_string,
			folder_table_name,
			where );

	if ( ! ( input =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_input( system_string ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"string_pipe_input(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	dictionary_parse(
		SQL_DELIMITER,
		attribute_name_list,
		input );
}

char *dictionary_select_attribute_string( LIST *attribute_name_list )
{
	if ( !list_length( attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_display_delimited(
		attribute_name_list,
		',' );
}

char *dictionary_system_string(
		char *select_string,
		char *folder_table_name,
		char *where )
{
	char system_string[ 1024 ];

	if ( !select_string
	||   !folder_table_name
	||   !where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select_string,
		folder_table_name,
		where );

	return strdup( system_string );
}

DICTIONARY *dictionary_parse(
		char sql_delimiter,
		LIST *attribute_name_list,
		char *input )
{
	char piece_buffer[ STRING_64K ];
	char *attribute_name;
	int i = 0;
	DICTIONARY *dictionary;

	if ( !sql_delimiter
	||   !list_rewind( attribute_name_list )
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	dictionary = dictionary_small();

	do {
		attribute_name = list_get( attribute_name_list );

		if ( !piece( piece_buffer, sql_delimiter, input, i++ ) )
		{
			char message[ 128 ];

			sprintf(message,
				"piece(%d,%s) returned empty.",
				i - 1,
				input );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		dictionary_set(
			dictionary,
			attribute_name,
			strdup( piece_buffer ) );

	} while ( list_next( attribute_name_list ) );

	return dictionary;
}

DICTIONARY *dictionary_file_fetch(
		char *filename,
		char delimiter )
{
	FILE *f;
	DICTIONARY *dictionary;
	char buffer[ STRING_64K ];
	char key[ 128 ];
	char data[ STRING_64K ];

	f = fopen( filename, "r" );

	if ( !f ) return (DICTIONARY *)0;

	dictionary = dictionary_large();

	while( string_input( buffer, f, STRING_64K ) )
	{
		piece( key, delimiter, buffer, 0 );

		*data = '\0';
		piece( data, delimiter, buffer, 1 );

		if ( !dictionary_key_exists(
			dictionary,
			key ) )
		{
			dictionary_set(
				dictionary,
				strdup( key ),
				strdup( data ) );
		}
	}

	fclose( f );

	return dictionary;
}

DICTIONARY *dictionary_single_row(
		LIST *key_list,
		int row_number,
		DICTIONARY *multi_row_dictionary )
{
	DICTIONARY *single_row = dictionary_small();
	char indexed_key[ 128 ];
	char *key;
	char *data;

	if ( !list_rewind( key_list ) ) return (DICTIONARY *)0;

	do {
		key = list_get( key_list );

		data = (char *)0;

		sprintf(indexed_key,
			"%s_%d",
			key,
			row_number );

		if ( ( data =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				indexed_key,
				multi_row_dictionary ) ) )
		{
			dictionary_set(
				single_row,
				key,
				data );
		}
	} while ( list_next( key_list ) );

	return single_row;
}

DICTIONARY *dictionary_remove_index( DICTIONARY *source_dictionary )
{
	DICTIONARY *dictionary = {0};
	LIST *key_list;
	char *key;
	char key_without_index[ 1024 ];

	key_list = dictionary_key_list( source_dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		string_trim_index(
			key_without_index /* destination */,
			key /* source */ );

		if ( !dictionary )
			dictionary =
				dictionary_small();

		dictionary_set( 
			dictionary,
			strdup( key_without_index ),
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				source_dictionary ) );

	} while( list_next( key_list ) );

	return dictionary;
}

DICTIONARY *dictionary_remove_prefix(
		const char *prefix,
		DICTIONARY *source_dictionary )
{
	DICTIONARY *small = {0};
	LIST *key_list;
	char *key;
	char key_without_starting_prefix[ 1024 ];
	int str_len;

	str_len = strlen( prefix );

	key_list = dictionary_key_list( source_dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( strncmp( prefix, key, str_len ) == 0 )
		{
			strcpy( key_without_starting_prefix,
				key + str_len );

			if ( !small ) small = dictionary_small();

			dictionary_set( 
				small,
				strdup( key_without_starting_prefix ),
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
					key,
					source_dictionary ) );
		}

	} while( list_next( key_list ) );

	return small;
}

LIST *dictionary_prefix_key_list(
		const char *prefix,
		DICTIONARY *dictionary )
{
	LIST *key_list;
	char *key;
	int str_len;
	LIST *list = {0};

	str_len = strlen( prefix );
	key_list = dictionary_key_list( dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		if ( strncmp( prefix, key, str_len ) == 0 )
		{
			if ( !list ) list = list_new();

			list_set( list, key );
		}

	} while( list_next( key_list ) );

	return list;
}

DICTIONARY *dictionary_prefix_extract(
	    	const char *prefix,
		DICTIONARY *source_dictionary )
{
	LIST *key_list;
	char *key;
	char *data;
	char key_without_prefix[ 1024 ];
	int str_len;
	DICTIONARY *destination_dictionary;

	str_len = string_strlen( (char *)prefix );

	key_list = dictionary_key_list( source_dictionary );

	if ( !str_len
	||   !list_rewind( key_list ) )
	{
		return (DICTIONARY *)0;
	}

	destination_dictionary = dictionary_medium();

	do {
		key = list_get( key_list );

		if ( strncmp( prefix, key, str_len ) == 0 )
		{
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			data = dictionary_get( key, source_dictionary );

			strcpy( key_without_prefix,
				key + str_len );

			dictionary_set( 
				destination_dictionary,
				strdup( key_without_prefix ),
				data );				
		}

	} while( list_next( key_list ) );

	return destination_dictionary;
}

void dictionary_index_set(
		DICTIONARY *dictionary /* out */,
		DICTIONARY *multi_row_dictionary,
		LIST *dictionary_key_list,
		int index )
{
	char *key;
	int index_key;
	char *get;

	if ( !list_rewind( dictionary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "dictionary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		key = list_get( dictionary_key_list );

		/* -------------------------------------------------- */
		/* Sample: attribute_name = "station" <-- returns -1  */
		/* Sample: attribute_name = "station_0" <-- returns 0 */
		/* Sample: attribute_name = "station_1" <-- returns 1 */
		/* -------------------------------------------------- */
		index_key = string_index( key );

		if ( index == index_key )
		{
			get =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
				key,
				multi_row_dictionary );

			if ( !get )
			{
				char message[ 128 ];

				sprintf(message,
					"dictionary_get(%s) returned empty.",
					key );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			dictionary_set(
				dictionary,
				key,
				get /* data */ );
		}
	} while ( list_next( dictionary_key_list ) );
}

LIST *dictionary_index_populated_name_list(
		LIST *folder_attribute_non_primary_key_list,
		DICTIONARY *multi_row_dictionary,
		int index )
{
	LIST *name_list;
	char *non_primary_key;
	char key_index[ 128 ];

	if ( !list_rewind( folder_attribute_non_primary_key_list )
	||   !dictionary_length( multi_row_dictionary )
	||   !index )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	name_list = list_new();

	do {
		non_primary_key =
			list_get(
				folder_attribute_non_primary_key_list );

		snprintf(
			key_index,
			sizeof ( key_index ),
			"%s_%d",
			non_primary_key,
			index );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		if ( dictionary_get(
			key_index,
			multi_row_dictionary ) )
		{
			list_set( name_list, non_primary_key );
		}

	} while ( list_next( folder_attribute_non_primary_key_list ) );

	if ( !list_length( name_list ) )
	{
		list_free( name_list );
		name_list = (LIST *)0;
	}

	return name_list;
}

LIST *dictionary_data_list(
		LIST *key_list,
		DICTIONARY *dictionary )
{
	LIST *data_list;
	char *data;
	char *key;

	if ( !list_rewind( key_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		key = list_get( key_list );

		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		if ( ! ( data = dictionary_get( key, dictionary ) ) )
		{
			fprintf(stderr,
		"Warning in %s/%s()/%d: dictionary_get(%s) returned empty.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 key );

			return NULL;
		}

		list_set( data_list, data );

	} while( list_next( key_list ) );

	return data_list;
}

void dictionary_prefixed_dictionary_set(
		DICTIONARY *dictionary,
		char *prefix,
		DICTIONARY *source_dictionary )
{
	LIST *key_list;
	char *key;
	char *get;

	if ( !dictionary
	||   !prefix )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	key_list = dictionary_key_list( source_dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		get =
			dictionary_get(
				key,
				source_dictionary );

		dictionary_set(
			dictionary,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			dictionary_prefixed_key(
				prefix,
				key ),
			get );

	} while ( list_next( key_list ) );

	list_free_container( key_list );
}

char *dictionary_prefixed_key(
		char *prefix,
		char *key )
{
	char prefixed_key[ 256 ];

	if ( !prefix
	&&   !key )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		prefixed_key,
		sizeof ( prefixed_key ),
		"%s%s",
		prefix,
		key );

	return strdup( prefixed_key );
}

