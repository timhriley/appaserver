/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_dictionary.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "file.h"
#include "date.h"
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
		char *widget_upload_recall_prefix,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list )
{
	char input[ STRING_128K ];
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
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
			input );
	}

	post_dictionary->original_post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_fetch(
			widget_upload_recall_prefix,
			application_name,
			upload_directory,
			upload_filename_list,
			post_dictionary->apache_key );

	return post_dictionary;
}

POST_DICTIONARY *post_dictionary_string_new(
		const char *dictionary_attribute_datum_delimiter,
		const char *dictionary_element_delimiter,
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
		char *widget_upload_recall_prefix,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *apache_key )
{
	char input[ STRING_65K ];
	char *apache_marker;
	char *attribute_name;
	char *datum;
	POST_DICTIONARY_FILE *post_dictionary_file;
	DICTIONARY *original_post_dictionary = dictionary_huge();

	if ( !apache_key )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"apache_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	while( string_input( input, stdin, sizeof ( input ) ) )
	{
		if ( !*input ) continue;

		apache_marker =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			post_dictionary_apache_marker(
				input );

		if ( !apache_marker ) continue;

		/* New attribute */
		/* ------------- */
		if ( strcmp(
			apache_marker,
			"Content-Disposition: form-data; name=" ) == 0 )
		{
			attribute_name =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				post_dictionary_attribute_name(
					input );

			post_dictionary_file =
			     post_dictionary_file_new(
				WIDGET_SELECT_OPERATOR,
				widget_upload_recall_prefix,
				application_name,
				upload_directory,
				upload_filename_list,
				apache_key,
				input,
				attribute_name );

			if ( post_dictionary_file )
			{
				dictionary_set(
					original_post_dictionary,
					post_dictionary_file->attribute_name,
					post_dictionary_file->
						filename->
						return_string );

				if ( post_dictionary_file->specification_key )
				{
					dictionary_set(
						original_post_dictionary,
						post_dictionary_file->
							specification_key,
						post_dictionary_file->
							specification );
				}

				continue;

			} /* if post_dictionary_file_new() */

			datum =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				post_dictionary_datum(
					WIDGET_SELECT_OPERATOR,
					stdin /* input_stream */,
					apache_key );

			if ( datum )
			{
				post_dictionary_datum_set(
					original_post_dictionary,
					attribute_name,
					datum );
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
		char *post_dictionary_datum )
{
	int row_number;

	if ( !dictionary
	||   !attribute_name
	||   !post_dictionary_datum )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_number =
		post_dictionary_row_number(
			post_dictionary_datum /* in/out */,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER );

	if ( row_number )
	{
		char *row_attribute_name =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_dictionary_row_attribute_name(
				attribute_name,
				row_number );

		free( attribute_name );
		attribute_name = row_attribute_name;
	}

	dictionary_set(
		dictionary,
		attribute_name,
		post_dictionary_datum );
}

void post_dictionary_file_write(
		char *apache_key,
		char *specification )
{
	FILE *output_file;
	int false_alarm_buffer[ 1024 ];
	int false_alarm_buffer_index = 0;
	char *apache_key_anchor = apache_key;
	int c;

	if ( !apache_key
	||   !specification )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Skip "Content-Type: application/octet-stream" */
	/* --------------------------------------------- */
	file_skip_line( stdin );

	/* Skip "\n" */
	/* --------- */
	file_skip_line( stdin );

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			specification );

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

		/* Write to the file */
		/* ----------------- */
		putc( c, output_file );
	}

	fclose( output_file );
}

char *post_dictionary_file_specification(
		char *application_name,
		char *upload_directory,
		char *filename_return_string )
{
	char specification[ 1024 ];

	if ( !application_name
	||   !upload_directory
	||   !filename_return_string )
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
		specification,
		sizeof ( specification ),
	 	"%s/%s/%s",
	 	upload_directory,
		application_name,
	 	filename_return_string );

	return strdup( specification );
}

char *post_dictionary_apache_marker( char *input )
{
	static char apache_marker[ 512 ];

	if ( strlen( input ) > sizeof ( apache_marker ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"apache_marker is too big for buffer of size %lu.",
			sizeof ( apache_marker ) - 1 );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* ---------------------------------------------------- */
	/* Returns destination or null if not enough delimiters */
	/* ---------------------------------------------------- */
	return piece( apache_marker, '"', input, 0 );
}

char *post_dictionary_attribute_name( char *input )
{
	char attribute_name[ 256 ];

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

	/* ---------------------------------------------------- */
	/* Returns destination or null if not enough delimiters */
	/* ---------------------------------------------------- */
	return strdup( piece( attribute_name, '"', input, 1 ) );
}

char *post_dictionary_file_raw_name(
		LIST *upload_filename_list,
		char *attribute_name,
		char *datum )
{
	if ( list_exists_string(
		attribute_name,
		upload_filename_list ) )
	{
		return datum;
	}
	else
	{
		return NULL;
	}
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

POST_DICTIONARY_FILE *post_dictionary_file_new(
		const char *widget_select_operator,
		char *widget_upload_recall_prefix,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *apache_key,
		char *input,
		char *attribute_name )
{
	POST_DICTIONARY_FILE *post_dictionary_file;

	if ( !application_name ) return NULL;

	if ( !upload_directory
	||   !apache_key
	||   !input
	||   !attribute_name )
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

	post_dictionary_file = post_dictionary_file_calloc();

	post_dictionary_file->recall_boolean =
		post_dictionary_file_recall_boolean(
			widget_upload_recall_prefix,
			attribute_name );

	if ( post_dictionary_file->recall_boolean )
	{
		post_dictionary_file->filename_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			post_dictionary_datum(
				widget_select_operator,
				stdin /* input_stream */,
				apache_key );
	}
	else
	{
		post_dictionary_file->filename_string =
			post_dictionary_filename_string(
				upload_filename_list,
				input,
				attribute_name );
	}

	if ( !post_dictionary_file->filename_string ) return NULL;

	post_dictionary_file->filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		filename_new(
			post_dictionary_file->filename_string,
			POST_DICTIONARY_FILE_FILENAME_MAX_SIZE
				/* probably 80 */,
			1 /* append_date_boolean */ );

	post_dictionary_file->attribute_name =
		/* ------------------------------------------- */
		/* Returns parameter or component of parameter */
		/* ------------------------------------------- */
		post_dictionary_file_attribute_name(
			widget_upload_recall_prefix,
			attribute_name,
			post_dictionary_file->recall_boolean );

	if ( post_dictionary_file->recall_boolean )
		return post_dictionary_file;

	post_dictionary_file->specification_key =
		/* ------------------ */
		/* Returns heap memory */
		/* ------------------ */
		post_dictionary_file_specification_key(
			POST_DICTIONARY_FILE_NAME_PREFIX,
			attribute_name );

	post_dictionary_file->specification =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_dictionary_file_specification(
			application_name,
			upload_directory,
			post_dictionary_file->
				filename->
				return_string /* heap memory */  );

	post_dictionary_file_write(
		apache_key,
		post_dictionary_file->specification );

	return post_dictionary_file;
}

POST_DICTIONARY_FILE *post_dictionary_file_calloc( void )
{
	POST_DICTIONARY_FILE *post_dictionary_file;

	if ( ! ( post_dictionary_file =
			calloc( 1,
				sizeof ( POST_DICTIONARY_FILE ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_dictionary_file;
}

char *post_dictionary_row_attribute_name(
		char *attribute_name,
		int row_number )
{
	char row_attribute_name[ 1024 ];

	if ( !attribute_name
	||   !row_number )
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
		row_attribute_name,
		sizeof ( row_attribute_name ),
		"%s_%d",
		attribute_name,
		row_number );

	return strdup( row_attribute_name );
}

char *post_dictionary_file_specification_key(
		const char *post_dictionary_file_name_prefix,
		char *attribute_name )
{
	char specification_key[ 256 ];

	if ( !attribute_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"attribute_name is empty." );

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
		specification_key,
		sizeof ( specification_key ),
		"%s_%s",
		post_dictionary_file_name_prefix,
		attribute_name );

	return strdup( specification_key );
}

char *post_dictionary_datum(
		const char *widget_select_operator,
		FILE *input_stream,
		char *post_dictionary_apache_key )
{
	char datum[ STRING_128K ];
	char *ptr = datum;
	char input[ STRING_64K ];

	*ptr = '\0';

	while ( 1 )
	{
		if ( !string_input(
			input,
			input_stream,
			sizeof ( input )  ) )
		{
			break;
		}

		if ( !*input ) continue;

		if ( string_strncmp(
			input,
			"Content-Type: " ) == 0 )
		{
			continue;
		}

		/* The last line will have "--" appended to the key */
		/* ------------------------------------------------ */
		if ( string_strncmp(
			input,
			post_dictionary_apache_key ) == 0 )
		{
			break;
		}

		if ( strlen( datum ) +
		     strlen( input ) +
		     1 >= STRING_128K )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( *datum ) ptr += sprintf( ptr, " " );

		ptr += sprintf(
			ptr,
			"%s",
			input );
	}

	if ( strcmp(
		datum,
		widget_select_operator ) == 0 )
	{
		return NULL;
	}

	return
	strdup(
		/* ------------------------------------ */
		/* Trims leading and trailing spaces.	*/
		/* Returns buffer.			*/
		/* ------------------------------------ */
		string_trim( datum /* buffer */ ) );
}

char *post_dictionary_filename_string(
		LIST *upload_filename_list,
		char *input,
		char *attribute_name )
{
	char filename[ 1024 ];

	if ( !input
	||   !attribute_name )
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

	if ( !list_exists_string(
		attribute_name,
		upload_filename_list ) )
	{
		return NULL;
	}

	if ( !piece( filename, '"', input, 3 ) ) return NULL;
	if ( !*filename ) return NULL;

	return
	strdup(
		/* --------------------------------- */
		/* Trims leading and trailing spaces */
		/* Returns buffer.		     */
		/* --------------------------------- */
		string_trim( filename ) );
}

boolean post_dictionary_file_recall_boolean(
		char *recall_prefix,
		char *attribute_name )
{
	if ( !recall_prefix
	||   !attribute_name )
	{
		char message[ 1024 ];

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

	return
	( string_strncmp(
		attribute_name,
		recall_prefix ) == 0 );
}

char *post_dictionary_file_attribute_name(
		char *recall_prefix,
		char *attribute_name,
		boolean recall_boolean )
{
	char *name;

	if ( !recall_boolean )
		name = attribute_name;
	else
		name =
			/* -------------------- */
			/* Returns string or	*/
			/* component of string  */
			/* -------------------- */
			string_skip_prefix(
				recall_prefix,
				attribute_name /* string */ );

	return name;
}

