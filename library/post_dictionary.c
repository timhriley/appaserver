/* $APPASERVER_HOME/library/post_dictionary.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "basename.h"
#include "piece.h"
#include "file.h"
#include "basename.h"
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

		apache_label =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_dictionary_apache_label(
				input );

		/* New attribute */
		/* ------------- */
		if ( string_strcmp(
			apache_label,
			"Content-Disposition: form-data; name=" ) == 0 )
		{
			POST_DICTIONARY_FILE *post_dictionary_file;

			attribute_name =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				post_dictionary_attribute_name(
					input );

			post_dictionary_file =
				post_dictionary_file_new(
					SECURITY_ESCAPE_CHARACTER_STRING,
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
					attribute_name,
					post_dictionary_file->date_name );

				dictionary_set(
					original_post_dictionary,
					post_dictionary_file->
						specification_key,
					post_dictionary_file->
						specification );

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
					     /* ------------------ */
					     /* Expect heap memory */
					     /* ------------------ */
					     attribute_name,
					     /* ------------------- */
					     /* Expect stack memory */
					     /* ------------------- */
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
		/* ------------------ */
		/* Expect heap memory */
		/* ------------------ */
		char *attribute_name,
		/* ------------------- */
		/* Expect stack memory */
		/* ------------------- */
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
		strdup( datum ) );
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
		char *date_name )
{
	char specification[ 512 ];

	if ( !application_name
	||   !upload_directory
	||   !date_name )
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
	 	date_name );

	return strdup( specification );
}

char *post_dictionary_apache_label( char *input )
{
	static char apache_label[ 128 ];

	/* ---------------------------------------------------- */
	/* Returns destination or null if not enough delimiters */
	/* ---------------------------------------------------- */
	return piece( apache_label, '"', input, 0 );
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
		char *input,
		char *attribute_name )
{
	static char raw_name[ 256 ];

	if ( list_exists_string(
		attribute_name,
		upload_filename_list ) )
	{
		return
		/* ---------------------------------------------------- */
		/* Returns destination or null if not enough delimiters */
		/* ---------------------------------------------------- */
		piece( raw_name, '"', input, 3 );
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

char *post_dictionary_file_clean_name(
		const char *security_escape_character_string,
		char *raw_name )
{
	static char clean_name[ 256 ];

	string_strcpy(
		clean_name,
		raw_name,
		sizeof ( clean_name ) /* buffer_size */ );

	/* Returns source_destination */
	/* -------------------------- */
	string_search_replace_character(
		clean_name /* source_destination */,
		' ',
		'_' );

	/* Returns source_destination */
	/* -------------------------- */
	string_remove_character_string(
		clean_name /* source_destination */,
		(char *)security_escape_character_string
			/* character_string */ );

	return clean_name;
}

char *post_dictionary_file_date_name( char *clean_name )
{
	char date_name[ 1024 ];
	static char *yyyymmdd;
	char *base_name;
	char *extension;
	char *dot;

	if ( !clean_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"clean_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Returns static memory */
	/* --------------------- */
	if ( !yyyymmdd ) yyyymmdd = date_now_yyyymmdd();

	base_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		basename_base_name(
			clean_name,
			1 /* strip_extension */ );

	extension =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		basename_extension(
			clean_name );

	if ( *extension )
		dot = ".";
	else
		dot = "";

	snprintf(
		date_name,
		sizeof ( date_name ),
		"%s_%s%s%s",
		base_name,
		yyyymmdd,
		dot,
		extension );

	return strdup( date_name );
}

POST_DICTIONARY_FILE *post_dictionary_file_new(
		const char *security_escape_character_string,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *apache_key,
		char *input,
		char *attribute_name )
{
	POST_DICTIONARY_FILE *post_dictionary_file;
	char *raw_name;
	char *clean_name;
	char *specification;

	if ( !list_length( upload_filename_list ) ) return NULL;

	if ( !application_name
	||   !upload_directory )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
"An upload file is attempted, but application_name or upload_directory is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	if ( !apache_key
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

	if ( !list_length( upload_filename_list ) ) return NULL;

	post_dictionary_file = post_dictionary_file_calloc();

	raw_name =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		post_dictionary_file_raw_name(
			upload_filename_list,
			input,
			attribute_name );

	if ( !raw_name )
	{
		free( post_dictionary_file );
		return NULL;
	}

	/* If exists filename attribute but is empty */
	/* ----------------------------------------- */
	if ( !*raw_name )
	{
		free( post_dictionary_file );
		return NULL;
	}

	clean_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_dictionary_file_clean_name(
			security_escape_character_string,
			raw_name );

	post_dictionary_file->date_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_dictionary_file_date_name(
			clean_name );

	specification =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_dictionary_file_specification(
			application_name,
			upload_directory,
			post_dictionary_file->date_name );

	post_dictionary_file->specification_key =
		/* ------------------ */
		/* Returns heap memory */
		/* ------------------ */
		post_dictionary_file_specification_key(
			POST_DICTIONARY_FILE_NAME_PREFIX,
			attribute_name );

	post_dictionary_file_write(
		apache_key,
		specification );

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
