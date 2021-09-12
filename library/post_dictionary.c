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
#include "piece.h"
#include "appaserver_error.h"
#include "dictionary.h"
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

POST_DICTIONARY *post_dictionary_stdin_new(
			char *appaserver_data_directory,
			char *session_key )
{
	POST_DICTIONARY *post_dictionary = post_dictionary_calloc();
	char input[ STRING_INPUT_LINE ];

	post_dictionary->post_dictionary_apache_key =
		/* --------------------------- */
		/* Returns heap memory or NULL */
		/* --------------------------- */
		post_dictionary_apache_key(
			string_input(
				input,
				stdin,
				STRING_INPUT_LINE ) );

	if ( !post_dictionary->post_dictionary_apache_key )
	{
		return post_dictionary_string_new( input );
	}

	post_dictionary->original_post_dictionary =
		post_dictionary_fetch(
			stdin,
			appaserver_data_directory,
			session_key,
			post_dictionary->
				post_dictionary_apache_key );

	return post_dictionary;
}

POST_DICTIONARY *post_dictionary_string_new(
			char *dictionary_string )
{
	POST_DICTIONARY *post_dictionary = post_dictionary_calloc();

	post_dictionary->original_post_dictionary =
		dictionary_string_resolve(
			dictionary_string );

	return post_dictionary;
}

DICTIONARY *post_dictionary_fetch(
			FILE *stdin,
			char *appaserver_data_directory,
			char *session_key,
			char *apache_key )
{
	char input[ 1024 ];
	char *apache_label;
	char *appaserver_key;
	char *input_filename;
	DICTIONARY *original_post_dictionary = dictionary_huge();

	while( string_input(
			input,
			stdin,
			1024 ) )
	{
		if ( !*input ) continue;

		apache_label =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_dictionary_apache_label(
				input );

		if ( string_strcmp(
			apache_label,
			"Content-Disposition: form-data; name=" ) == 0 )
		{
			appaserver_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				post_dictionary_appaserver_key(
					input );

			if ( !*appaserver_key )
			{
				fprintf(stderr,
"Warning in %s/%s()/%d: post_dictionary_appaserver_key(%s) returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					input );
				continue;
			}

			/* ------------------------ */
			/* Check if spooling a file */
			/* ------------------------ */
			input_filename =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				post_dictionary_input_filename(
					input );

			if ( *input_filename )
			{
				char *spool_filename;

				if ( !appaserver_data_directory
				||   !*appaserver_data_directory
				||   !session_key
				||   !*session_key )
				{
					fprintf(stderr,
				"ERROR in %s/%s()/%d: spool file error.\n",
						__FILE__,
						__FUNCTION__,
						__LINE__ );
					exit( 1 );
				}

				spool_filename =
					/* ------------------- */
					/* Returns heap memory */
					/* ------------------- */
					post_dictionary_spool_filename(
						search_replace_character(
							input_filename,
							' ',
							'_' ),
						appaserver_data_directory,
						session_key );

				dictionary_set(
					original_post_dictionary,
					strdup( appaserver_key ),
					spool_filename );

				post_dictionary_spool_file(
					stdin,
					apache_key,
					spool_filename );
			}
			else
			{
				/* ------------------- */
				/* Not spooling a file */
				/* ------------------- */
				post_dictionary_stream_set(
					original_post_dictionary,
					stdin,
					appaserver_key,
					apache_key );
			}
		}
		else
		/* ---------------------------------------- */
		/* If Appaserver sent a dictionary to stdin */
		/* ---------------------------------------- */
		{
			original_post_dictionary =
				dictionary_string_resolve(
					input );

			/* Should only be one */
			/* ------------------ */
			break;
		}
	}

	return original_post_dictionary;
}

char *post_dictionary_apache_key( char *input )
{

	if ( *input == '-' )
		return strdup( input );
	else
		return (char *)0;
}

void post_dictionary_stream_set(
			DICTIONARY *dictionary,
			FILE *stdin,
			/* ------------- */
			/* static memory */
			/* ------------- */
			char *appaserver_key,
			/* ------------- */
			/* static memory */
			/* ------------- */
			char *apache_key )
{
	char input[ STRING_INPUT_BUFFER ];
	int multi_drop_down_index;
	int row = 0;
	char key[ 256 ];

	*key = '0';

	multi_drop_down_index = string_index( appaserver_key );

	/* May be a multi-select drop-down or the first edit table row */
	/* ----------------------------------------------------------- */
	if ( multi_drop_down_index == 1 )
	{
		string_trim_index( appaserver_key );
		row = 1;
	}
	else
	{
		string_strcpy( key, appaserver_key, 256 );
	}

	while( string_input(
			input,
			stdin,
			STRING_INPUT_BUFFER ) )
	{
		if ( !*input ) continue;

		if ( strcmp( input, "select" ) == 0 ) continue;

		/* All done */
		/* -------- */
		if ( string_strncmp( input, apache_key ) == 0 ) break;

		if ( multi_drop_down_index == 1 )
		{
			sprintf(key,
				"%s_%d",
				appaserver_key,
				row++ );

			dictionary_set(
				dictionary,
				strdup( key ),
				strdup( input ) );
		}
		else
		{
			dictionary_set(
				dictionary,
				strdup( key ),
				strdup( input ) );
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
			FILE *stdin,
			char *post_dictionary_key )
{
	char input[ STRING_INPUT_LINE ];

	*data = '\0';

	while( string_input( input, stdin, STRING_INPUT_LINE ) )
	{
		if ( !*input ) continue;
		if ( strcmp( input, "select" ) == 0 ) continue;
		if ( string_strncmp( input, post_dictionary_key ) == 0 ) break;

		data += sprintf( data, "%s", input );
	}
}

void post_dictionary_spool_file(
			FILE *stdin,
			char *apache_key,
			char *spool_filename )
{
	FILE *output_stream;
	int false_alarm_buffer[ 1024 ];
	int false_alarm_buffer_index = 0;
	char *apache_key_anchor = apache_key;
	int c;

	if ( ! ( output_stream = fopen( spool_filename, "w" ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s(): fopen(%s) for write returned empty.\n",
			__FILE__,
			__FUNCTION__,
			spool_filename );
		exit( 1 );
	}

	/* Skip "Content-Type: application/octet-stream" */
	/* --------------------------------------------- */
	skip_line( stdin );

	/* Skip "\n" */
	/* --------- */
	skip_line( stdin );

	while( ( c = getc( stdin ) ) != EOF )
	{
		if ( c == *apache_key )
		{
			apache_key++;

			/* All done */
			/* -------- */
			if ( !*apache_key ) break;

			false_alarm_buffer[ false_alarm_buffer_index++ ] = c;
			continue;
		}

		/* If started an all done, but it's not. */
		/* ------------------------------------- */
		if ( apache_key != apache_key_anchor )
		{
			int i;

			/* Write false_alarm_buffer */
			/* ------------------------ */
			for( i = 0; i < false_alarm_buffer_index; i++ )
			{
				putc(	false_alarm_buffer[ i ],
					output_stream );
			}

			/* Reset apache_key and index */
			/* -------------------------- */
			apache_key = apache_key_anchor;
			false_alarm_buffer_index = 0;

			/* If NOW starting an all done */
			/* --------------------------- */
			if ( c == *apache_key )
			{
				apache_key++;

				false_alarm_buffer[
					false_alarm_buffer_index++ ] =
						c;
				continue;
			}
		}

		/* Write to the spool file */
		/* ----------------------- */
		putc( c, output_stream );
	}

	fclose( output_stream );
}

char *post_dictionary_spool_filename(
			/* ------------- */
			/* Static memory */
			/* ------------- */
			char *input_filename,
			char *appaserver_data_directory,
			char *session_key )
{
	char spool_filename[ 1024 ];

	if ( !*input_filename
	||   !appaserver_data_directory
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
	 	input_filename,
		session_key );

	return strdup( spool_filename );
}

char *post_dictionary_apache_label( char *input )
{
	static char apache_label[ 128 ];

	return piece( apache_label, '"', input, 0 );
}

char *post_dictionary_appaserver_key( char *input )
{
	static char appaserver_key[ 256 ];

	*appaserver_key = '\0';

	return piece( appaserver_key, '"', input, 1 );
}

char *post_dictionary_input_filename( char *input )
{
	static char input_filename[ 256 ];

	*input_filename = '\0';

	if ( instr( "; filename=", input, 1 ) > -1 )
		return piece( input_filename, '"', input, 3 );
	else
		return input_filename;
}

