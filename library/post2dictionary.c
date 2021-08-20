/* $APPASERVER_HOME/library/post2dictionary.c				*/
/* -------------------------------------------------------------------- */
/* This is the post2dictionary function.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "timlib.h"
#include "post2dictionary.h"
#include "basename.h"
#include "decode_html_post.h"

DICTIONARY *post2dictionary(
			FILE *input_stream,
			char *appaserver_data_directory,
			char *session )
{
	char input_buffer[ STRING_INPUT_LINE ];
	DICTIONARY *dictionary;
	char post_key[ 256 ];

	string_input( input_buffer, input_stream, STRING_INPUT_LINE );

/*
sample input_buffer = "--------------20983969252595940091482100312"
*/
	if ( timlib_strncmp( input_buffer, "---------" ) == 0
	||   timlib_strncmp( input_buffer, "------WebKitFormBoundary" ) == 0 )
	{
		timlib_strcpy( post_key, input_buffer, 256 );
		trim( post_key );
		dictionary = dictionary_huge_new();

		while( get_line( input_buffer, input_stream ) )
		{
			post2dictionary_process_input_buffer(
					dictionary,
					input_buffer,
					post_key,
					appaserver_data_directory,
					session,
					input_stream );
		}
	}
	else
	{
		char decoded_data[ MAX_INPUT_LINE ];

		timlib_strcpy( decoded_data, input_buffer, MAX_INPUT_LINE );

		decode_html_post( input_buffer, decoded_data );

		dictionary =
			dictionary_index_string2dictionary(
				input_buffer );
	}
	return dictionary;
}

void post2dictionary_process_input_buffer(
				DICTIONARY *dictionary,
				char *input_buffer,
				char *post_key,
				char *appaserver_data_directory,
				char *session,
				FILE *input_stream )
{
	char key[ 1024 ];
	char spool_filename[ 1024 ];
	char data[ MAX_INPUT_LINE ];
	int target;

	target = instr( "Content-Disposition: form-data; name=",
			input_buffer,
			1 );

	if ( target != -1 )
	{
		extract_delimited(
			key,
			input_buffer + 
			target   +
			strlen(
			 "Content-Disposition: form-data; name=" ) );

		target = instr( "; filename=",
				input_buffer,
				1 );

		if ( target != -1 )
		{
			char *base_filename;

			extract_delimited(
				data,
				input_buffer + 
				target   +
				strlen(
				"; filename=" ) );

			base_filename =
				basename_get_base_name(
					data, 0 );

			if ( base_filename && *base_filename )
			{
				char local_post_key[ 256 ];

				post2dictionary_get_spool_filename(
					spool_filename,
					appaserver_data_directory,
					base_filename,
					session );

				sprintf( local_post_key,
					 "\r\n%s",
					 post_key );

				if ( post2dictionary_spool_file(
					spool_filename,
					input_stream,
					local_post_key ) )
				{
					timlib_strcpy(
						data,
						spool_filename,
						MAX_INPUT_LINE );
				}
				else
				{
					search_replace_string(
						data,
						"\\",
						"/" );
				}
			}
		}
		else
		{
			post2dictionary_fetch(
				data,
				input_stream,
				post_key );
		}

		if ( *data )
		{
			post2dictionary_set_dictionary(
					dictionary,
					key,
					data );
		}
	}
}

void convert_folder_data_delimiter(
				char *data,
				char *key )
{
	if ( !character_exists( key, FOLDER_DATA_DELIMITER )
	&&    character_exists( data, FOLDER_DATA_DELIMITER ) )
	{
		char search[ 2] = {0};

		*search = FOLDER_DATA_DELIMITER;

		search_replace_string(
			data,
			search,
			POST2DICTIONARY_DELIMITER_REPLACEMENT );
	}

}

void post2dictionary_set_dictionary(	DICTIONARY *dictionary,
					char *key,
					char *data )
{
	char *current_data;
	int index;
	int post_index;
	int results;
	char trimmed_key[ 1024 ];

	post_index = get_index_from_string( key );
	trim_index( trimmed_key, key );
	search_replace_string( data, "\"", "'" );

	if ( strcmp( trimmed_key, key ) == 0 )
	{
		dictionary_set_pointer(	dictionary,
					strdup( key ), 
					strdup( data ) );
	}
	else
	{
		if ( post_index )
		{
			results = dictionary_get_index_data(	
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
					results = dictionary_get_index_data(	
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

void post2dictionary_fetch(
			char *destination,
			FILE *input_stream,
			char *post_key )
{
	char input_buffer[ MAX_INPUT_LINE ];

	*destination = '\0';

	while( get_line( input_buffer, input_stream ) )
	{
		if ( timlib_strncmp( input_buffer, post_key ) == 0 ) break;

		if ( !*input_buffer ) continue;
		if ( strcmp( input_buffer, "select" ) == 0 ) continue;

		destination +=
			sprintf( destination,
				 "%s",
				 input_buffer );
	}
}

void post2dictionary_get_spool_filename(
						char *spool_filename,
						char *appaserver_data_directory,
						char *base_filename,
						char *session )
{
	if ( appaserver_data_directory && session )
	{
		sprintf(spool_filename,
		 	"%s/%s_%s",
		 	appaserver_data_directory,
		 	base_filename,
			session );
	}
	else
	{
		sprintf( spool_filename,
			 "%s/%s_%d",
			 POST2DICTIONARY_TEMP_DIRECTORY,
			 base_filename,
			 getpid() );
	}
}

boolean post2dictionary_spool_file(	char *spool_filename,
					FILE *input_stream,
					char *post_key )
{
	FILE *output_stream;
	int false_alarm_buffer[ 256 ];
	int false_alarm_buffer_length;
	char *post_key_anchor = post_key;
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
		if ( c == *post_key )
		{
			post_key++;

			if ( !*post_key ) break;

			false_alarm_buffer[ false_alarm_buffer_length++ ] = c;
		}
		else
		{
			if ( post_key != post_key_anchor )
			{
				int i;

				for( i = 0; i < false_alarm_buffer_length; i++ )
				{
					putc(	false_alarm_buffer[ i ],
						output_stream );
				}

				post_key = post_key_anchor;

				false_alarm_buffer_length = 0;

				if ( c == *post_key )
				{
					post_key++;
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

	return 1;

}

