/* library/validation_level.c */
/* ------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validation_level.h"

enum validation_level validation_level_string_resolve(
				char *validation_level_string )
{
	return 
		validation_level_get_validation_level(
			validation_level_string );
}

enum validation_level validation_level_get_validation_level(
				char *validation_level_string )
{
	enum validation_level validation_level = {0};

	if ( strcmp( validation_level_string, "provisional" ) == 0 )
		validation_level = provisional;
	else
	if ( strcmp( validation_level_string, "validated" ) == 0 )
		validation_level = validated;
	else
		validation_level = either;

	return validation_level;

} /* validation_level_get_validation_level() */

char *validation_level_display( enum validation_level validation_level )
{
	return validation_level_get_string( validation_level );
}

char *validation_level_get_string(
				enum validation_level validation_level )
{
	if ( validation_level == provisional )
		return "provisional";
	else
	if ( validation_level == validated )
		return "validated";
	else
	if ( validation_level == either )
		return "either";
	else
		return "validation level error";

} /* validation_level_get_string() */

char *validation_level_get_title_string(
				enum validation_level validation_level )
{
	char title_string[ 128 ];

	if ( validation_level == provisional
	||   validation_level == validated )
	{
		sprintf( title_string,
			 "/%s",
			 validation_level_get_string(
				validation_level ) );
	}
	else
	{
		*title_string = '\0';
	}

	return strdup( title_string );

} /* validation_level_get_title_string() */

