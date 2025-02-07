/* $APPASERVER_HOME/library/post_dictionary.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "appaserver.h"
#include "widget.h"
#include "security.h"
#include "post_dictionary.h"

POST_DICTIONARY *post_dictionary_calloc( void )
{
	POST_DICTIONARY *post_dictionary;

	if ( ! ( post_dictionary = calloc( 1, sizeof ( POST_DICTIONARY ) ) ) )
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
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list )
{
	char input[ STRING_64K ];
	POST_DICTIONARY *post_dictionary = post_dictionary_calloc();

	post_dictionary->apache_key =
		/* --------------------------- */
		/* Returns heap memory or NULL */
		/* --------------------------- */
		post_dictionary_apache_key(
			string_input(
				input,
				stdin,
				sizeof ( input ) ) );

	if ( !post_dictionary->apache_key )
	{
		free( post_dictionary );

		return
		post_dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
			input );
	}

	post_dictionary->original_post_dictionary =
		post_dictionary_fetch(
			application_name,
			upload_directory,
			upload_filename_list,
			post_dictionary->apache_key );

	return post_dictionary;
}

POST_DICTIONARY *post_dictionary_string_new(
		const char dictionary_attribute_datum_delimiter,
		const char dictionary_element_delimiter,
		char *dictionary_string )
{
	POST_DICTIONARY *post_dictionary = post_dictionary_calloc();

	post_dictionary->original_post_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_large() or null */
		/* ---------------------------------- */
		dictionary_string_new(
			dictionary_attribute_datum_delimiter,
			dictionary_element_delimiter,
			dictionary_string );

	return post_dictionary;
}

DICTIONARY *post_dictionary_fetch(
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *apache_key )
{
	char input[ STRING_128K ];
	char *apache_label;
	char *attribute_name;
	char *input_filename;
	char *spool_filename;
	DICTIONARY *original_post_dictionary = dictionary_huge();

	while( string_input( input, stdin, sizeof ( input ) ) )
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

			/* ------------------------ */
			/* Check if spooling a file */
			/* ------------------------ */
			input_filename =
				/* ----------------------------- */
				/* Returns static memory or null */
				/* ----------------------------- */
				post_dictionary_input_filename(
					upload_filename_list,
					input,
					attribute_name );

			if ( input_filename )
			{
				char spool_key[ 128 ];

				if ( !application_name
				||   !upload_directory )
				{
					char message[ 128 ];

					sprintf(message,
"Received a request to spool a file, but one or more parameters are empty." );

					appaserver_error_stderr_exit(
						__FILE__,
						__FUNCTION__,
						__LINE__,
						message );
				}

				/* If exists filename prompt but is empty */
				/* -------------------------------------- */
				if ( !*input_filename ) continue;

				string_search_replace_character(
					input_filename,
					' ',
					'_' );

				string_remove_character_string(
					input_filename
						/* source_destination */,
					SECURITY_ESCAPE_CHARACTER_STRING
						/* character_string */ );

				spool_filename =
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					post_dictionary_spool_filename(
						application_name,
						upload_directory,
						input_filename );

				post_dictionary_spool_file(
					spool_filename,
					apache_key );

/*
				dictionary_set(
					original_post_dictionary,
					strdup( attribute_name ),
					strdup( input_filename ) );
*/

				snprintf(
					spool_key,
					sizeof ( spool_key ),
					"%s%s",
					POST_DICTIONARY_SPOOL_PREFIX,
					attribute_name );

				dictionary_set(
					original_post_dictionary,
					strdup( spool_key ),
					strdup( input_filename ) );

				continue;
			}

			while ( string_input(
					input,
					stdin,
					sizeof ( input ) ) )
			{
				if ( string_strncmp(
					input,
					apache_key ) == 0 )
				{
					break;
				}

				if ( *input )
				{
					post_dictionary_datum_set(
					     original_post_dictionary,
					     attribute_name,
					     input /* datum */ );
				}
			}
		}
	}

	return original_post_dictionary;
}

char *post_dictionary_apache_key( char *input )
{

	if ( input && *input == '-' )
		return strdup( input );
	else
		return NULL;
}

void post_dictionary_datum_set(
		DICTIONARY *dictionary /* out */,
		char *attribute_name,
		char *datum )
{
	int row_number;

	if ( !dictionary
	||   !attribute_name
	||   !datum )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( datum, WIDGET_SELECT_OPERATOR ) == 0 ) return;

	row_number =
		post_dictionary_row_number(
			datum /* in/out */,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER );

	if ( row_number )
	{
		sprintf(attribute_name + strlen( attribute_name ),
			"_%d",
			row_number );
	}

	dictionary_set(
		dictionary,
		strdup( attribute_name ),
		strdup( datum ) );
}

void post_dictionary_spool_file(
		char *spool_filename,
		char *apache_key )
{
	FILE *output_file;
	int false_alarm_buffer[ 1024 ];
	int false_alarm_buffer_index = 0;
	char *apache_key_anchor = apache_key;
	int c;

	if ( !spool_filename
	||   !apache_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			spool_filename );

	/* Skip "Content-Type: application/octet-stream" */
	/* --------------------------------------------- */
	skip_line( stdin );

	/* Skip "\n" */
	/* --------- */
	skip_line( stdin );

	while ( ( c = getc( stdin ) ) != EOF )
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

		/* If started an all done, but it's not all done. */
		/* ---------------------------------------------- */
		if ( apache_key != apache_key_anchor )
		{
			int i;

			/* Write false_alarm_buffer */
			/* ------------------------ */
			for( i = 0; i < false_alarm_buffer_index; i++ )
			{
				putc(	false_alarm_buffer[ i ],
					output_file );
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
		putc( c, output_file );
	}

	fclose( output_file );
}

char *post_dictionary_spool_filename(
		char *application_name,
		char *upload_directory,
		char *input_filename )
{
	static char spool_filename[ 512 ];

	if ( !application_name
	||   !upload_directory
	||   !input_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		spool_filename,
		sizeof ( spool_filename ),
	 	"%s/%s/%s",
	 	upload_directory,
		application_name,
	 	input_filename );

	return spool_filename;
}

char *post_dictionary_apache_label( char *input )
{
	static char apache_label[ 128 ];
	return piece( apache_label, '"', input, 0 );
}

char *post_dictionary_attribute_name( char *input )
{
	static char attribute_name[ 256 ];

	if ( !string_character_count(
		'"',
		input ) )
	{
		char message[ 128 ];

		sprintf(message,
			"Expecting a double quote but got none." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return piece( attribute_name, '"', input, 1 );
}

char *post_dictionary_input_filename(
		LIST *upload_filename_list,
		char *input,
		char *post_dictionary_attribute_name )
{
	static char filename[ 256 ];

	if ( list_exists_string(
		post_dictionary_attribute_name,
		upload_filename_list ) )
	{
		piece( filename, '"', input, 3 );
		return filename;
	}

	return NULL;
}

int post_dictionary_row_number(
		char *datum /* in/out */,
		char widget_multi_left_right_delimiter )
{
	char *ptr;
	int str_len = string_strlen( datum );

	if ( !str_len ) return 0;

	ptr = datum + str_len - 1;

	while ( ptr > datum )
	{
		if ( isdigit( *ptr ) )
		{
			ptr--;
			continue;
		}

		if ( *ptr == widget_multi_left_right_delimiter )
		{
			int row_number = atoi( ptr + 1 ) + 1;
			*ptr = '\0';
			return row_number;
		}

		break;
	}
	return 0;
}
