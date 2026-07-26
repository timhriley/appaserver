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
		char *recall_prefix,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list )
{
	char input[ STRING_128K ];
	POST_DICTIONARY *post_dictionary;
	POST_DICTIONARY_ATTRIBUTE *post_dictionary_attribute;

	post_dictionary = post_dictionary_calloc();

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

	post_dictionary->original_post_dictionary = dictionary_huge();

	while ( 1 )
	{
		post_dictionary_attribute =
			post_dictionary_attribute_new(
				WIDGET_SELECT_OPERATOR,
				recall_prefix,
				application_name,
				upload_directory,
				upload_filename_list,
				post_dictionary->apache_key );

		if ( !post_dictionary_attribute ) break;

		if ( post_dictionary_attribute->name
		&&   post_dictionary_attribute->datum )
		{
			post_dictionary_datum_set(
				WIDGET_MULTI_LEFT_RIGHT_DELIMITER,
				post_dictionary->
					original_post_dictionary
					/* out */,
				post_dictionary_attribute->name,
				post_dictionary_attribute->datum
					/* in/out */ );
		}
	}

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

char *post_dictionary_apache_key( char *input )
{

	if ( input && *input == '-' )
		return strdup( input );
	else
		return NULL;
}

void post_dictionary_datum_set(
		const char widget_multi_left_right_delimiter,
		DICTIONARY *dictionary /* out */,
		char *attribute_name,
		char *attribute_datum /* in/out */ )
{
	int row_number;

	if ( !dictionary )
	{
		char message[ 128 ];

		sprintf(message, "dictionary is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

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

	if ( !attribute_datum )
	{
		char message[ 128 ];

		sprintf(message, "attribute_datum is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_number =
		post_dictionary_row_number(
			attribute_datum /* in/out */,
			widget_multi_left_right_delimiter );

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
		attribute_datum );
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

	/* Skip "Content-Type: ..." */
	/* ------------------------ */
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

char *post_dictionary_attribute_apache_marker( char *input )
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
		const int file_name_maxsize,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *apache_key,
		char *input,
		char *attribute_name )
{
	POST_DICTIONARY_FILE *post_dictionary_file;

	if ( !application_name
	||   !upload_directory
	||   !apache_key
	||   !input
	||   !attribute_name )
	{
		return NULL;
	}

	if ( !post_dictionary_file_boolean(
		upload_filename_list,
		attribute_name ) )
	{
		return NULL;
	}

	post_dictionary_file = post_dictionary_file_calloc();

	post_dictionary_file->name_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		post_dictionary_file_name_string( input );

	if ( !post_dictionary_file->name_string )
	{
		return post_dictionary_file;
	}

	post_dictionary_file->filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		filename_new(
			post_dictionary_file->name_string,
			file_name_maxsize /* probably 80 */,
			1 /* append_date_boolean */ );

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

char *post_dictionary_attribute_datum(
		const char *widget_select_operator,
		FILE *input_stream,
		char *apache_key )
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
			apache_key ) == 0 )
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

char *post_dictionary_file_name_string(
		char *input )
{
	char filename[ 1024 ];

	if ( !input )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !piece( filename, '"', input, 3 ) ) return NULL;
	if ( !*filename ) return NULL;

	return
	strdup(
		/* --------------------------------- */
		/* Trims leading and trailing spaces */
		/* Returns buffer.		     */
		/* --------------------------------- */
		string_trim( filename /* buffer */ ) );
}

boolean post_dictionary_recall_boolean(
		char *recall_prefix,
		char *attribute_name )
{
	if ( !attribute_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"attribute_name is empty." );

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

char *post_dictionary_recall_attribute_name(
		char *recall_prefix,
		char *attribute_name )
{
	return
	/* -------------------- */
	/* Returns string or	*/
	/* component of string  */
	/* -------------------- */
	string_skip_prefix(
		recall_prefix,
		attribute_name /* string */ );
}

POST_DICTIONARY_RECALL *post_dictionary_recall_new(
		char *recall_prefix,
		const int file_name_maxsize,
		char *application_name,
		char *upload_directory,
		char *apache_key,
		char *attribute_name )
{
	POST_DICTIONARY_RECALL *post_dictionary_recall;

	if ( !recall_prefix
	||   !application_name
	||   !upload_directory
	||   !apache_key
	||   !attribute_name )
	{
		return NULL;
	}

	if ( !post_dictionary_recall_boolean(
		recall_prefix,
		attribute_name ) )
	{
		return NULL;
	}

	post_dictionary_recall = post_dictionary_recall_calloc();

	post_dictionary_recall->post_dictionary_attribute_datum =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		post_dictionary_attribute_datum(
			WIDGET_SELECT_OPERATOR,
			stdin /* input_stream */,
			apache_key );

	if ( !post_dictionary_recall->post_dictionary_attribute_datum )
	{
		return post_dictionary_recall;
	}

	post_dictionary_recall->filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		filename_new(
			post_dictionary_recall->
				post_dictionary_attribute_datum,
			file_name_maxsize /* probably 80 */,
			1 /* append_date_boolean */ );

	post_dictionary_recall->post_dictionary_file_specification =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_dictionary_file_specification(
			application_name,
			upload_directory,
			post_dictionary_recall->
				filename->
				return_string );

	/* [Recall] may retrieve another application’s filename */
	/* ---------------------------------------------------- */
	if ( !file_exists_boolean(
		post_dictionary_recall->
			post_dictionary_file_specification ) )
	{
		return post_dictionary_recall;
	}

	post_dictionary_recall->attribute_name =
		/* ------------------------------ */
		/* Returns component of parameter */
		/* ------------------------------ */
		post_dictionary_recall_attribute_name(
			recall_prefix,
			attribute_name );

	return post_dictionary_recall;
}

POST_DICTIONARY_RECALL *post_dictionary_recall_calloc( void )
{
	POST_DICTIONARY_RECALL *post_dictionary_recall;

	if ( ! ( post_dictionary_recall =
			calloc( 1,
				sizeof ( POST_DICTIONARY_RECALL ) ) ) )
	{
		char message[ 1024 ];

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

	return post_dictionary_recall;
}

POST_DICTIONARY_ATTRIBUTE *post_dictionary_attribute_new(
		const char *select_operator,
		char *recall_prefix,
		char *application_name,
		char *upload_directory,
		LIST *upload_filename_list,
		char *apache_key )
{
	char input[ STRING_65K ];
	char *apache_marker;
	POST_DICTIONARY_ATTRIBUTE *post_dictionary_attribute;

	if ( !apache_key )
	{
		char message[ 1024 ];

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
			post_dictionary_attribute_apache_marker(
				input );

		if ( !apache_marker ) continue;

		/* New attribute */
		/* ------------- */
		if ( strcmp(
			apache_marker,
			"Content-Disposition: form-data; name=" ) == 0 )
		{
			post_dictionary_attribute =
				post_dictionary_attribute_calloc();

			post_dictionary_attribute->name =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				post_dictionary_attribute_name(
					input );

			post_dictionary_attribute->post_dictionary_file =
			     post_dictionary_file_new(
				POST_DICTIONARY_FILE_NAME_MAXSIZE,
				application_name,
				upload_directory,
				upload_filename_list,
				apache_key,
				input,
				post_dictionary_attribute->name );

			if ( post_dictionary_attribute->post_dictionary_file )
			{
				if ( post_dictionary_attribute->
					post_dictionary_file->
					filename )
				{
					post_dictionary_attribute->datum =
						post_dictionary_attribute->
							post_dictionary_file->
							filename->
							return_string;
				}

				return post_dictionary_attribute;
			}

			post_dictionary_attribute->post_dictionary_recall =
			     post_dictionary_recall_new(
				recall_prefix,
				POST_DICTIONARY_FILE_NAME_MAXSIZE,
				application_name,
				upload_directory,
				apache_key,
				post_dictionary_attribute->name );

			if ( post_dictionary_attribute->post_dictionary_recall )
			{
				post_dictionary_attribute->name =
					post_dictionary_attribute->
						post_dictionary_recall->
						attribute_name;

				post_dictionary_attribute->datum =
					post_dictionary_attribute->
						post_dictionary_recall->
						filename->
						return_string;

				return post_dictionary_attribute;
			}

			post_dictionary_attribute->datum =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				post_dictionary_attribute_datum(
					select_operator,
					stdin /* input_stream */,
					apache_key );

			if ( post_dictionary_attribute->datum )
			{
				return post_dictionary_attribute;
			}
		} /* If new attribute */
	} /* while ( string_input() ) */

	return NULL;
}

POST_DICTIONARY_ATTRIBUTE *post_dictionary_attribute_calloc( void )
{
	POST_DICTIONARY_ATTRIBUTE *post_dictionary_attribute;

	if ( ! ( post_dictionary_attribute =
			calloc( 1,
				sizeof ( POST_DICTIONARY_ATTRIBUTE ) ) ) )
	{
		char message[ 1024 ];

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

	return post_dictionary_attribute;
}

boolean post_dictionary_file_boolean(
		LIST *upload_filename_list,
		char *attribute_name )
{
	boolean file_boolean = 0;

	if ( !attribute_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_exists_string(
		attribute_name,
		upload_filename_list ) )
	{
		file_boolean = 1;
	}

	return file_boolean;
}
