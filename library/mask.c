/* library/mask.c */
/* -------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mask.h"


char *mask_trim( char *destination, char *source )
{
	char *holder = destination;

	while( *source )
	{
		if ( isdigit( *source ) )
			*destination++ = *source++;
		else
			source++;
	}

	*destination = '\0';
	return holder;

} /* mask_trim() */


char *mask_set( char *destination, char *source, char *mask )
{
	char local_source[ 128 ];
	char *local_source_ptr;
	int number_of_pound_signs;
	int number_of_digits;
	char *holder;
	int count_characters( char *s, char c );
	int count_digits( char *s );
	int pad_leading_characters(	char *d, 
					char *s, 
					int number_digits_to_pad, 
					char c_to_pad );

	if ( !*source )
	{
		*destination = '\0';
		return destination;
	}

	if ( strlen( source ) > strlen( mask ) )
	{
		*destination = '\0';
		return destination;
	}

	number_of_pound_signs = count_characters( mask, '#' );
	number_of_digits = count_digits( source );

	if ( number_of_pound_signs < number_of_digits )
	{
		fprintf(stderr,
			"mask_set() has invalid input: source=(%s) mask=(%s)\n",
			source,
			mask );

		strcpy( destination, source );
		return destination;
	}

	pad_leading_characters(	local_source, 
				source, 
				number_of_pound_signs - number_of_digits, 
				'0' );
	
	holder = destination;
	local_source_ptr = local_source;

	while( *local_source_ptr && *mask )
	{
		if ( *mask == '#' )
		{
			if ( !isdigit( *local_source_ptr ) 
			&&   *local_source_ptr != *mask )
			{
				return (char *)0;
			}

			*destination++ = *local_source_ptr++;
		}
		else
		{
			*destination++ = *mask;
			if ( ispunct( *local_source_ptr ) ) local_source_ptr++;
		}
		mask++;
	}

	*destination = '\0';
	return holder;

} /* mask_set() */


int count_characters( char *s, char c )
{
	int count = 0;
	while( *s )
	{
		if ( *s == c ) count++;
		s++;
	}
	return count;

} /* count_characters() */


int pad_leading_characters(	char *d, 
				char *s, 
				int number_characters_to_pad, 
				char c_to_pad )
{
	if ( !d )
	{
		fprintf( stderr, 
		"pad_leading_characters() got null for a destination\n" );
		exit( 1 );
	}

	while( number_characters_to_pad-- ) *d++ = c_to_pad;
	strcpy( d, s );

	return 1;

} /* pad_leading_characters() */


int count_digits( char *s )
{
	int count = 0;
	while( *s )
	{
		if ( isdigit( *s ) ) count++;
		s++;
	}
	return count;

} /* count_digits() */
