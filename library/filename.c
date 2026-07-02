/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/filename.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include "appaserver_error.h"
#include "String.h"
#include "piece.h"
#include "security.h"
#include "date.h"
#include "filename.h"

FILENAME *filename_new(
		char *filename_string,
		int filename_max_size,
		boolean append_date_boolean )
{
	FILENAME *filename;

	if ( !filename_string
	||   !filename_max_size )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	filename = filename_calloc();

	filename->extension =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		filename_extension( filename_string );

	filename->extension_length =
		filename_extension_length(
			filename->extension );

	filename->basename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		filename_basename(
			filename_string,
			1 /* strip_extension */ );

	filename->clean =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		filename_clean(
			SECURITY_ESCAPE_CHARACTER_STRING,
			filename->basename );

	filename->clean_length =
		filename_clean_length(
			filename->clean );

	filename->string_left_size =
		filename_string_left_size(
			FILENAME_APPEND_DATE_LENGTH,
			filename_max_size,
			append_date_boolean,
			filename->extension_length );

	filename->trim =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		filename_trim(
			filename->clean,
			filename->string_left_size );

	filename->date =
		/* -------------------------------- */
		/* Returns parameter or heap memory */
		/* -------------------------------- */
		filename_date(
			append_date_boolean,
			filename->trim );

	filename->return_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		filename_return_string(
			filename->extension,
			filename->date );

	return filename;
}

FILENAME *filename_calloc( void )
{
	FILENAME *filename;

	if ( ! ( filename = calloc( 1, sizeof ( FILENAME ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return filename;
}

char *filename_extension( char *filename_string )
{
	static char extension[ 128 ];
	int count;

	*extension = '\0';

	count = string_character_count( '.', filename_string );

	if ( count )
	{
		piece( extension, '.', filename_string, count );
	}

	return extension;
}

int filename_extension_length( char *filename_extension )
{
	return
	string_strlen( filename_extension );
}

char *filename_clean(
		const char *security_escape_character_string,
		char *basename )
{
	static char clean[ 256 ];

	string_strcpy(
		clean,
		basename,
		sizeof ( clean ) /* buffer_size */ );

	/* Returns source_destination */
	/* -------------------------- */
	string_search_replace_character(
		clean /* source_destination */,
		' ',
		'_' );

	/* Returns source_destination */
	/* -------------------------- */
	string_remove_character_string(
		clean /* source_destination */,
		(char *)security_escape_character_string
			/* character_string */ );

	/* Returns source_destination */
	/* -------------------------- */
	string_remove_character(
		clean /* source_destination */,
		',' );

	return clean;
}

int filename_clean_length( char *filename_clean )
{
	return
	string_strlen( filename_clean );
}

int filename_string_left_size(
		const int filename_append_date_length,
		int filename_max_size,
		boolean append_date_boolean,
		int extension_length )
{
	int subtract = 0;
	int left_size;

	if ( extension_length )
	{
		/* Add the dot too */
		/* --------------- */
		subtract = extension_length + 1;
	}

	if ( append_date_boolean ) subtract += filename_append_date_length;

	left_size =
		filename_max_size -
		subtract;

	return left_size;
}

char *filename_trim(
		char *filename_clean,
		int string_left_size )
{
	char destination[ 256 ];

	return
	strdup(
		/* Returns destination */
		/* ------------------- */
		string_left(
			destination,
			filename_clean /* source */,
			string_left_size /* how_many */ ) );
}

char *filename_date(
		boolean append_date_boolean,
		char *filename_trim )
{
	if ( !append_date_boolean )
		return filename_trim;
	else
	{
		char date[ 256 ];

		snprintf(
			date,
			sizeof ( date ),
			"%s_%s",
			filename_trim,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			date_now_yyyymmdd() );

		return strdup( date );
	}
}

char *filename_return_string(
		char *filename_extension /* static memory */,
		char *filename_date )
{
	char dot_string[ 2 ];
	char return_string[ 256 ];


	if ( *filename_extension )
	{
		strcpy( dot_string, "." );
	}
	else
	{
		*dot_string = '\0';
	}

	snprintf(
		return_string,
		sizeof ( return_string ),
		"%s%s%s",
		filename_date,
		dot_string,
		filename_extension );

	return strdup( return_string );
}

char *filename_basename(
		char *filename,
		boolean strip_extension )
{
	static char basename[ 512 ];
	char local_buffer[ 512 ];
	char *end_ptr;

	string_strcpy( local_buffer, filename, sizeof ( local_buffer ) );
	end_ptr = local_buffer + strlen( local_buffer );

	/* First, strip off extension */
	/* -------------------------- */
	if ( strip_extension && string_character_exists( local_buffer, '.' ) )
	{
		while( end_ptr != local_buffer )
		{
			/* If found extension */
			/* ------------------ */
			if ( *end_ptr == '.' )
			{
				/* Make this the end of string */
				/* --------------------------- */
				*end_ptr-- = '\0';
				break;
			}
			else
			{
				end_ptr--;
			}
		}
	}

	/* Second, Isolate basename */
	/* ------------------------ */
	while( end_ptr != local_buffer )
	{
		/* If at the back slash in front of the base name */
		/* ---------------------------------------------- */
		if ( *end_ptr == '\\' 
		||   *end_ptr == '/' )
		{
			/* Base name is the next character */
			/* ------------------------------- */
			end_ptr++;
			break;
		}
		else
		{
			end_ptr--;
		}
	}

	/* Copy to static memory to return */
	/* ------------------------------- */
	strcpy( basename, end_ptr );

	return basename;
}

char *filename_directory( char *filename_string )
{
	static char directory[ 256 ];
	char local_buffer[ 512 ];
	char *directory_ptr;

	*directory = '\0';

	string_strcpy( local_buffer, filename_string, sizeof ( local_buffer ) );
	directory_ptr = local_buffer + strlen( local_buffer );

	while( directory_ptr != local_buffer )
	{
		if ( *directory_ptr == '\\' 
		||   *directory_ptr == '/' )
		{
			*directory_ptr = '\0';
			strcpy( directory, local_buffer );
			break;
		}
		else
		{
			directory_ptr--;
		}
	}

	return directory;
}
