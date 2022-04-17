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
			char *upload_directory,
			char *session_key )
{
	POST_DICTIONARY *post_dictionary = post_dictionary_calloc();
	char input[ STRING_INPUT_BUFFER ];

	post_dictionary->apache_key =
		/* --------------------------- */
		/* Returns heap memory or NULL */
		/* --------------------------- */
		post_dictionary_apache_key(
			string_input(
				input,
				stdin,
				STRING_INPUT_BUFFER ) );

	if ( !post_dictionary->apache_key )
	{
		return post_dictionary_string_new( input );
	}

	post_dictionary->original_post_dictionary =
		post_dictionary_fetch(
			stdin,
			upload_directory,
			session_key,
			post_dictionary->apache_key );

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
			char *upload_directory,
			char *session_key,
			char *apache_key )
{
	char input[ 1024 ];
	char *apache_label;
	char *attribute_name;
	char *input_filename;
	char *store_filename;
	char *spool_filename;
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
			attribute_name =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				post_dictionary_attribute_name(
					input );

			if ( !*attribute_name )
			{
				fprintf(stderr,
"Warning in %s/%s()/%d: post_dictionary_attribute_name(%s) returned empty.\n",
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
				if ( !upload_directory
				||   !*upload_directory
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

				store_filename =
					/* ------------------- */
					/* Returns heap memory */
					/* ------------------- */
					post_dictionary_store_filename(
						input_filename,
						session_key );

				spool_filename =
					/* ------------------- */
					/* Returns heap memory */
					/* ------------------- */
					post_dictionary_spool_filename(
						store_filename,
						upload_directory );

				post_dictionary_spool_file(
					spool_filename,
					stdin,
					apache_key );

				dictionary_set(
					original_post_dictionary,
					strdup( attribute_name ),
					store_filename );
			}
			else
			{
				/* ------------------- */
				/* Not spooling a file */
				/* ------------------- */
				post_dictionary_stream_set(
					original_post_dictionary,
					stdin,
					attribute_name,
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
			char *attribute_name,
			char *apache_key )
{
	char input[ STRING_64K ];
	int multi_drop_down_index;
	int row = 0;
	char key[ 256 ];

	*key = '0';

	multi_drop_down_index = string_index( attribute_name );

	/* May be a multi-select drop-down or the first edit table row */
	/* ----------------------------------------------------------- */
	if ( multi_drop_down_index == 1 )
	{
		string_trim_index( attribute_name );
		row = 1;
	}
	else
	{
		string_strcpy( key, attribute_name, 256 );
	}

	while( string_input(
			input,
			stdin,
			STRING_64K ) )
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
				attribute_name,
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

void post_dictionary_input(
			char *data,
			FILE *stdin,
			char *post_dictionary_key )
{
	char input[ STRING_64K ];

	*data = '\0';

	while( string_input( input, stdin, STRING_64K ) )
	{
		if ( !*input ) continue;
		if ( strcmp( input, "select" ) == 0 ) continue;
		if ( string_strncmp( input, post_dictionary_key ) == 0 ) break;

		data += sprintf( data, "%s", input );
	}
}

void post_dictionary_spool_file(
			char *spool_filename,
			FILE *stdin,
			char *apache_key )
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
			char *store_filename,
			char *upload_directory )
{
	char spool_filename[ 1024 ];

	if ( !*store_filename || !upload_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: a parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(spool_filename,
	 	"%s/%s",
	 	upload_directory,
	 	store_filename );

	return strdup( spool_filename );
}

char *post_dictionary_apache_label( char *input )
{
	static char apache_label[ 128 ];

	return piece( apache_label, '"', input, 0 );
}

char *post_dictionary_attribute_name( char *input )
{
	static char attribute_name[ 256 ];

	*attribute_name = '\0';

	return piece( attribute_name, '"', input, 1 );
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

char *post_dictionary_store_filename(
			char *input_filename,
			char *session_key )
{
	char store_filename[ 256 ];

	sprintf(store_filename,
		"%s_%s",
		search_replace_character( input_filename, ' ', '_' ),
		session_key );

	return strdup( store_filename );
}

