/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/basename.c 					*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "basename.h"
#include "piece.h"
#include "String.h"

char *basename_get_directory( char *argv_0 )
{
	return basename_directory( argv_0 );
}

char *basename_directory( char *argv_0 )
{
	static char base_name[ 128 ];
	char local_buffer[ 512 ];
	char *base_name_ptr;

	string_strcpy( local_buffer, argv_0, 512 );
	base_name_ptr = local_buffer + strlen( local_buffer );

	while( base_name_ptr != local_buffer )
	{
		if ( *base_name_ptr == '\\' 
		||   *base_name_ptr == '/' )
		{
			*base_name_ptr = '\0';
			strcpy( base_name, local_buffer );
			return base_name;
			break;
		}
		else
		{
			base_name_ptr--;
		}
	}
	return "";
}

char *basename_filename( char *argv_0 )
{
	return
	/* Returns static memory */
	/* --------------------- */
	basename_base_name(
		argv_0,
		0 /* no strip_extension */ );
}

char *basename_get_filename( char *argv_0 )
{
	return
	basename_base_name(
		argv_0,
		0 /* no strip_extension */ );
}

char *basename_get_extension( char *argv_0 )
{
	return basename_extension( argv_0 );
}

char *basename_extension( char *argv_0 )
{
	static char extension[ 128 ];
	int count;

	*extension = '\0';

	count = string_character_count( '.', argv_0 );

	if ( count )
	{
		piece( extension, '.', argv_0, count );
	}

	return extension;
}

char *basename_get_base_name( char *argv_0, boolean strip_extension )
{
	return basename_base_name( argv_0, strip_extension );
}

char *basename_base_name( char *argv_0, boolean strip_extension )
{
	static char base_name[ 128 ];
	char local_buffer[ 512 ];
	char *end_ptr;

	string_strcpy( local_buffer, argv_0, sizeof ( local_buffer ) );
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

	/* Second, Isolate base name */
	/* ------------------------- */
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
	strcpy( base_name, end_ptr );

	return base_name;
}
