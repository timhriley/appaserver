/* $APPASERVER_HOME/library/String.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "boolean.h"
#include "list.h"
#include "String.h"

/* Class variables */
/* --------------- */
static boolean string_get_line_check_utf_16 = 1;
static boolean string_is_utf_16 = 0;
static boolean string_utf_16_toggle = 1;

/* Temporal cohesion */
/* ----------------- */
void string_reset_get_line_check_utf_16( void )
{
	string_get_line_check_utf_16 = 1;
	string_is_utf_16 = 0;
	string_utf_16_toggle = 1;
}

/* Returns input_buffer or (char *)0 if all done. */
/* ---------------------------------------------- */
char *string_input(	char *input_buffer,
			FILE *infile,
			int buffer_size )
{
	int in_char;
	int size = 0;
	char *anchor = input_buffer;

	*anchor = '\0';

	/* Exit in middle. */
	/* --------------- */
	while ( 1 )
	{
		in_char = fgetc( infile );

		if ( string_get_line_check_utf_16 )
		{
			string_get_line_check_utf_16 = 0;

			if ( in_char == 255 )
			{
				in_char = fgetc( infile );

				if ( in_char == 254 )
				{
					string_is_utf_16 = 1;
					continue;
				}
			}
		}

		if ( string_is_utf_16 )
		{
			string_utf_16_toggle = 1 - string_utf_16_toggle;

			if ( string_utf_16_toggle )
			{
				continue;
			}
		}

		/* Why are there zeros? */
		/* -------------------- */
		if ( !in_char ) continue;

		if ( in_char == STRING_CR ) continue;

		if ( in_char == EOF )
		{
			/* --------------------------------------- */
			/* If last line in file doesn't have a CR, */
			/* then call this function one more time.  */
			/* --------------------------------------- */
			/* If you need to tweek this, then test    */
			/* process=execute_select_statement on a   */
			/* file without a trailing CR.		   */
			/* --------------------------------------- */
			if ( input_buffer != anchor )
			{
				*input_buffer = '\0';
				return anchor;
			}
			else
			{
				string_reset_get_line_check_utf_16();
				return (char *)0;
			}
		}

		if ( in_char == STRING_LF )
		{
			*input_buffer = '\0';
			return anchor;
		}

		/* If '\' then get the next character */
		/* ---------------------------------- */
		if ( in_char == '\\' )
		{
			in_char = fgetc( infile );

			if ( in_char == STRING_CR ) continue;

			/* Can't escape the LF */
			/* ------------------- */
			if ( in_char == STRING_LF )
			{
				*input_buffer = '\0';
				return anchor;
			}

			*input_buffer++ = '\\';
			size++;
		}

		if ( buffer_size && ( size++ >= buffer_size ) )
		{
			fprintf( stderr,
		"Warning in %s()/%d: exceeded max line length of %d:\n"
		"%.75s...\n\n",
				 __FUNCTION__,
				 __LINE__,
				 buffer_size - 1,
				 anchor );
			*input_buffer = '\0';
			return anchor;
		}

		*input_buffer++ = in_char;

	} /* while( 1 ) */
}

STRING_OCCURRANCE *string_occurrance_new( char *ptr )
{
	STRING_OCCURRANCE *a;

	if ( ! ( a = (STRING_OCCURRANCE *)
			calloc( 1, sizeof ( STRING_OCCURRANCE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	a->ptr = ptr;
	return a;
}

char *string_occurrance_list_display(
				char *destination,
				LIST *occurrance_list )
{
	STRING_OCCURRANCE *string_occurrance;
	char *ptr = destination;

	*ptr = '\0';

	if ( list_rewind( occurrance_list ) )
	{
		do {
			string_occurrance = list_get( occurrance_list );

			if ( ptr == destination )
			{
				ptr += sprintf( ptr,
			"\nat: [%s] are %d occurrances starting with %d\n",
						string_occurrance->ptr,
						string_occurrance->occurrance,
						*string_occurrance->ptr );
			}
			else
			{
				ptr += sprintf( ptr,
			"at: [%s] are %d occurrances starting with %d\n",
						string_occurrance->ptr,
						string_occurrance->occurrance,
						*string_occurrance->ptr );
			}
		} while ( list_next( occurrance_list ) );
	}
	return destination;
}

char *string_enforce_utf16(	char *destination,
				char *source )
{
	LIST *negative_occurrance_list;
	STRING_OCCURRANCE *occurrance;
	char *skip_to;

	strcpy( destination, source );

	negative_occurrance_list =
		string_negative_sequence_occurrance_list(
			destination );

if ( list_length( negative_occurrance_list ) )
{
char buffer[ 65536 ];
fprintf( stderr, "%s/%s()/%d: negative_occurrance_list = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
string_occurrance_list_display( buffer, negative_occurrance_list ) );
fflush( stderr );
}

	if ( !list_go_tail( negative_occurrance_list ) )
		return destination;

	do {
		occurrance = list_get( negative_occurrance_list );

		if ( occurrance->occurrance > 2 )
		{
			skip_to = occurrance->ptr + occurrance->occurrance;

			strcpy( occurrance->ptr, skip_to );
		}

	} while ( list_previous( negative_occurrance_list ) );

	return destination;
}

LIST *string_negative_sequence_occurrance_list( char *source )
{
	char *ptr;
	boolean inside_negative_sequence;
	LIST *occurrance_list;
	STRING_OCCURRANCE *string_occurrance = {0};

	if ( !*source ) return (LIST *)0;

	occurrance_list = list_new();

	inside_negative_sequence = 0;

	ptr = source;

	while ( *ptr++ )
	{
		if ( *ptr < 0 )
		{
			if ( inside_negative_sequence )
			/* ------------ */
			/* Still inside */
			/* ------------ */
			{
				if ( !string_occurrance )
				{
					fprintf( stderr,
			"ERROR in %s/%s()/%d: empty string_occurrance.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__ );
					exit( 1 );
				}

				string_occurrance->occurrance++;
			}
			else
			/* ------------ */
			/* Newly inside */
			/* ------------ */
			{
				string_occurrance =
					string_occurrance_new(
						ptr );

				string_occurrance->occurrance = 1;

				list_set(	occurrance_list,
						string_occurrance );

			}
		}
		else
		/* Must be *ptr >= 0 */
		{
			if ( inside_negative_sequence )
			{
				/* ---------------- */
				/* No longer inside */
				/* ---------------- */
				inside_negative_sequence = 0;
			}
			else
			{
				/* Do nothing */
			}
		}
	}

	return occurrance_list;
}

char *string_escape(
			char *destination,
			char *source,
			char *character_array )
{
	return string_escape_character_array(
			destination,
			source,
			character_array );
}

char *string_escape_character_array(
			char *destination,
			char *source,
			char *character_array )
{
	char *character_array_anchor = character_array;
	char *destination_anchor = destination;

	while ( *source )
	{
		character_array = character_array_anchor;

		while( *character_array )
		{
			if ( *source == *character_array )
			{
				*destination++ = '\\';
				break;
			}
			character_array++;
		}
		*destination++ = *source++;

	}
	*destination = '\0';

	return destination_anchor;
}

char *string_escape_quote(	char *destination,
				char *source )
{
	return string_escape_character_array(
			destination,
			source,
			"'" /* character_array */ );
}

int string_strlen( char *s )
{
	if ( !s ) return 0;
	return strlen( s );
}

boolean string_strcmp( char *s1, char *s2 )
{
	if ( !s1 && !s2 ) return 0;
	if ( !s2 ) return -1;
	if ( !s1 ) return 1;

	return strcasecmp( s1, s2 );
}

boolean string_strcpy( char *d, char *s, int buffer_size )
{
	int str_len;

	if ( d && !s )
	{
		*d = '\0';
		return 0;
	}

	if ( !d || !s ) return 0;

	if ( d == s ) return 1;

	*d = '\0';

	str_len = strlen( s );

	if ( buffer_size && str_len >= buffer_size ) return 0;

	while( *s ) *d++ = *s++;

	*d = '\0';
	return 1;
}

char *string_strcat( char *d, char *s )
{
	char *end;

	if ( !d ) return "";
	if ( !s ) return d;

	end = d + strlen( d );
	while( *s ) *end++ = *s++;
	*end = '\0';
	return d;
}

int string_strncmp( char *s1, char *s2 )
{
	int str_len2;

	if ( !s1 || !s2 ) return -1;
	if ( !*s1 && !*s2 ) return 0;
	if ( !*s1 || !*s2 ) return -1;

	str_len2 = strlen( s2 );
	return strncasecmp( s1, s2, str_len2 );
}

boolean string_loose_strcmp( char *s1, char *s2 )
{
	if ( !s1 && !s2 ) return 0;
	if ( !s2 ) return -1;
	if ( !s1 ) return 1;

	return strcmp( s1, s2 );
}

