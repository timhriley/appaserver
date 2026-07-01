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
#include "basename.h"
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
		filename_basename( filename_string );

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
	char *extension;
	static char extension_return[ 128 ];

	*extension_return = '\0';

	extension =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		basename_extension( filename_string );

	if ( *extension )
	{
		snprintf(
			extension_return,
			sizeof ( extension_return ),
			".%s",
			extension );
	}

	return extension_return;
}

int filename_extension_length( char *filename_extension )
{
	return
	string_strlen( filename_extension );
}

char *filename_basename( char *filename_string )
{
	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	basename_base_name(
		filename_string,
		1 /* strip_extension */ );
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
	int subtract;
	int left_size;

	subtract = extension_length;

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
	char return_string[ 256 ];

	snprintf(
		return_string,
		sizeof ( return_string ),
		"%s%s",
		filename_date,
		filename_extension );

	return strdup( return_string );
}

