/* $APPASERVER_HOME/library/post_dictionary.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "basename.h"
#include "decode_html_post.h"
#include "post_dictionary.h"

POST_DICTIONARY *post_dictionary_calloc( void )
{
	POST_DICTIONARY *post_dictionary;

	if ( ! ( post_dictionary = calloc( 1, sizeof( POST_DICTIONARY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return post_dictionary;
}

POST_DICTIONARY *post_dictionary_fetch(
			FILE *input_stream,
			char *dictionary_string,
			char *appaserver_data_directory,
			char *session_key )
{
	POST_DICTIONARY *post_dictionary = post_dictionary_calloc();

	if ( input_stream )
	{
		post_dictionary->dictionary =
			post_dictionary_input_stream(
				input_stream,
				appaserver_data_directory,
				session_key );
	}
	else
	{
		post_dictionary->dictionary =
			dictionary_string_resolve(
				dictionary_string );
	}

	return post_dictionary;
}

DICTIONARY *post_dictionary_input_stream(
			FILE *input_stream,
			char *appaserver_data_directory,
			char *session_key )
{
	char input[ STRING_INPUT_LINE ];
	DICTIONARY *dictionary;
	char *key;

	/* Get first line */
	/* -------------- */
	string_input( input, input_stream, STRING_INPUT_LINE );

	/* If from Apache */
	/* -------------- */
	if ( string_strncmp( input, "---------" ) == 0
	||   string_strncmp( input, "------WebKitFormBoundary" ) == 0 )
	{
/*
sample input_buffer = "--------------20983969252595940091482100312"
sample input_buffer = "------WebKitFormBoundary"
*/
		key = post_dictionary_key( input );
		dictionary = dictionary_huge_new();

		while( string_input(
				input,
				input_stream,
				STRING_INPUT_LINE ) )
		{
			post_dictionary_line_input(
				dictionary,
				input,
				key,
				appaserver_data_directory,
				session_key,
				input_stream );
		}
	}
	else
	/* ------------------ */
	/* If from Appaserver */
	/* ------------------ */
	{
		dictionary =
			dictionary_string_resolve(
				input );
	}
	return dictionary;
}

char *post_dictionary_key( char *input )
{
	char key[ 1024 ];

	sprintf( key,
		"\r\n%s",
		 string_trim( input ) );

	return strdup( key );
}

void post_dictionary_line_input(
			DICTIONARY *dictionary,
			char *input,
			char *post_dictionary_key,
			char *appaserver_data_directory,
			char *session_key,
			FILE *input_stream )
{
	char key[ 1024 ];
	char *spool_filename;
	char *base_name;
	char data[ STRING_INPUT_LINE ];
	int target;

	target =
		instr( "Content-Disposition: form-data; name=",
			input,
			1 );

	if ( target != -1 )
	{
		string_extract_lt_gt_delimited(
			key,
			input + 
			target +
			strlen(
			 "Content-Disposition: form-data; name=" ) );

		target =
			instr( "; filename=",
				input,
				1 );

		if ( target != -1 )
		{
			string_extract_lt_gt_delimited(
				data,
				input + 
				target   +
				strlen(
				"; filename=" ) );

			base_name =
				basename_base_name(
					data,
					0 /* not strip_extension */ );

			if ( !base_name || !*base_name )
			{
				fprintf(stderr,
				"ERROR in %s/%s()/%d: base_name is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			spool_filename =
				post_dictionary_spool_filename(
					appaserver_data_directory,
					base_name,
					session_key );

			post_dictionary_spool_file(
				spool_filename,
				input_stream,
				post_dictionary_key );

			string_strcpy(
				data,
				spool_filename,
				STRING_INPUT_LINE );
		}
		else
		{
			post_dictionary_input(
				data,
				input_stream,
				post_dictionary_key );
		}

		if ( *data )
		{
			post_dictionary_set(
				dictionary,
				key,
				data );
		}
	}
}

void post_dictionary_set(
			DICTIONARY *dictionary,
			char *key,
			char *data )
{
	char *current_data;
	int index;
	int post_index;
	int results;
	char trimmed_key[ 1024 ];

	post_index = string_index( key );
	trim_index( trimmed_key, key );
	search_replace_string( data, "\"", "'" );

	if ( strcmp( trimmed_key, key ) == 0 )
	{
		dictionary_set(
			dictionary,
			strdup( key ), 
			strdup( data ) );
	}
	else
	{
		if ( post_index )
		{
			results =
				dictionary_index_data(	
					&current_data,
					dictionary,
					trimmed_key,
					post_index );
	
			if ( results == -1 )
			{
				dictionary_set_string_index_key(
					dictionary,
					trimmed_key, 
					strdup( data ),
					post_index );
			}
			else
			if ( results == 0 )
			{
				dictionary_set_string_index_key(
					dictionary,
					trimmed_key, 
					strdup( data ),
					0 );
			}
			else
			{
				/* To satisfy multi-select drop downs */
				/* ---------------------------------- */
				for ( index = 1;; index++ )
				{
					results =
						dictionary_index_data(	
							&current_data,
							dictionary,
							trimmed_key,
							index );
		
					if ( results == -1 )
					{
						dictionary_set_string_index_key(
							dictionary,
							trimmed_key, 
							strdup( data ),
							index );
						break;
					}
					else
					if ( results == 0 )
					{
						dictionary_set_string_index_key(
							dictionary,
							trimmed_key, 
							strdup( data ),
							0 );
						break;
					}
				}
			}
		}
		else
		{
			dictionary_set_string_index_key(
				dictionary,
				trimmed_key, 
				strdup( data ),
				0 );
		}
		return;
	}
}

void post_dictionary_input(
			char *data,
			FILE *input_stream,
			char *post_dictionary_key )
{
	char input[ STRING_INPUT_LINE ];

	*data = '\0';

	while( string_input( input, input_stream, STRING_INPUT_LINE ) )
	{
		if ( !*input ) continue;
		if ( strcmp( input, "select" ) == 0 ) continue;
		if ( string_strncmp( input, post_dictionary_key ) == 0 ) break;

		data += sprintf( data, "%s", input );
	}
}

void post_dictionary_spool_file(
			char *spool_filename,
			FILE *input_stream,
			char *post_dictionary_key )
{
	FILE *output_stream;
	int false_alarm_buffer[ 256 ];
	int false_alarm_buffer_length;
	char *post_key_anchor = post_dictionary_key;
	int c;

	if ( ! ( output_stream = fopen( spool_filename, "w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s for write.\n",
			__FILE__,
			__FUNCTION__,
			spool_filename );
		exit( 1 );
	}

	/* Skip "Content-Type: application/octet-stream" */
	/* --------------------------------------------- */
	skip_line( input_stream );

	/* Skip "\n" */
	/* --------- */
	skip_line( input_stream );

	false_alarm_buffer_length = 0;

	while( ( c = getc( input_stream ) ) != EOF )
	{
		if ( c == *post_dictionary_key )
		{
			post_dictionary_key++;

			if ( !*post_dictionary_key ) break;

			false_alarm_buffer[ false_alarm_buffer_length++ ] = c;
		}
		else
		{
			if ( post_dictionary_key != post_key_anchor )
			{
				int i;

				for( i = 0; i < false_alarm_buffer_length; i++ )
				{
					putc(	false_alarm_buffer[ i ],
						output_stream );
				}

				post_dictionary_key = post_key_anchor;

				false_alarm_buffer_length = 0;

				if ( c == *post_dictionary_key )
				{
					post_dictionary_key++;
					false_alarm_buffer[
					false_alarm_buffer_length++ ] = c;
				}
				else
				{
					putc( c, output_stream );
				}
			}
			else
			{
				putc( c, output_stream );
			}
		}
	}

	fclose( output_stream );
}

char *post_dictionary_spool_filename(
			char *appaserver_data_directory,
			char *base_name,
			char *session_key )
{
	char spool_filename[ 1024 ];

	if ( !appaserver_data_directory
	||   !base_name
	||   !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: a parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(spool_filename,
	 	"%s/%s_%s",
	 	appaserver_data_directory,
	 	base_name,
		session_key );

	return strdup( spool_filename );
}

