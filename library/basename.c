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
#include "timlib.h"

char *basename_get_directory( char *argv_0 )
{
	return basename_directory( argv_0 );
}

char *basename_directory( char *argv_0 )
{
	static char base_name[ 128 ];
	char local_buffer[ 512 ];
	char *base_name_ptr;

	timlib_strcpy( local_buffer, argv_0, 512 );
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

	if ( !piece( extension, '.', argv_0, 1 ) ) *extension = '\0';

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
	char *base_name_ptr;

	strcpy( local_buffer, argv_0 );
	base_name_ptr = local_buffer + strlen( local_buffer );

	/* First, strip off extension */
	/* -------------------------- */
	if ( strip_extension && character_exists( local_buffer, '.' ) )
	{
		while( base_name_ptr != local_buffer )
		{
			/* If found extension */
			/* ------------------ */
			if ( *base_name_ptr == '.' )
			{
				/* Make this the end of string */
				/* --------------------------- */
				*base_name_ptr-- = '\0';
				break;
			}
			else
			{
				base_name_ptr--;
			}
		}
	}

	/* Second, Isolate base name */
	/* ------------------------- */
	while( base_name_ptr != local_buffer )
	{
		/* If at the back slash in front of the base name */
		/* ---------------------------------------------- */
		if ( *base_name_ptr == '\\' 
		||   *base_name_ptr == '/' )
		{
			/* Base name is the next character */
			/* ------------------------------- */
			base_name_ptr++;
			break;
		}
		else
		{
			base_name_ptr--;
		}
	}

	/* Copy to static memory to return */
	/* ------------------------------- */
	strcpy( base_name, base_name_ptr );
	return base_name;
}
