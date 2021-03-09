/* $APPASERVER_HOME/library/String.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "boolean.h"
#include "list.h"
#include "float.h"
#include "timlib.h"
#include "piece.h"
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
	char local_source[ 1024 ];

	strcpy( local_source, source );

	while ( *character_array )
	{
		destination =
			string_escape_character(
				destination,
				local_source,
				*character_array
					/* character_to_escape */ );
		character_array++;
		strcpy( local_source, destination );
	}
	return destination;
}

char *string_escape_quote(
			char *destination,
			char *source )
{
	return string_escape_quote_dollar(
			destination,
			source );
}

char *string_escape_full(
			char *destination,
			char *source )
{
	return string_escape_character_array(
			destination,
			source,
			"$'#" /* character_array */ );
}

char *string_escape_quote_dollar(
			char *destination,
			char *source )
{
	return string_escape_character_array(
			destination,
			source,
			"$'" /* character_array */ );
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

char *string_remove_control( char *input )
{
	static char remove_control[ 256 ];
	char *ptr = remove_control;
	unsigned char ch;

	while ( *input )
	{
		ch = *input;
		if ( isprint( ch ) ) *ptr++ = ch;
		input++;
	}
	*ptr = '\0';
	return remove_control;
}

char *string_escape_character(
			char *destination,
			char *data,
			int character_to_escape )
{
	char *anchor = destination;

	if ( !data )
	{
		*destination = '\0';
		return destination;
	}

	while ( *data )
	{
		if ( *data == character_to_escape )
			*destination++ = '\\';
		*destination++ = *data++;
	}
	*destination = '\0';
	return anchor;
}

char *string_commas_rounded_dollar( double d )
{
	char s[ 20 ];

	sprintf( s, "%.0lf", round_double( d ) );
	return place_commas_in_number_string( s );

}

char *string_commas_money( double d )
{
	return string_commas_dollar( d );
}

char *string_commas_dollar( double d )
{
	char *results;

	/* Returns static memory of 3 decimal places */
	/* ----------------------------------------- */
	results = string_commas_double( d );
	*( results + strlen( results ) - 1 ) = '\0';
	return results;
}

/* Returns heap memory of 3 decimal places */
/* --------------------------------------- */
char *string_commas_double( double d )
{
	char integer_part[ 64 ];
	char buffer[ 64 ];
	char decimal_part[ 8 ];
	char d_string[ 128 ];
	char reversed_integer[ 64 ];
	char *reversed_integer_pointer = reversed_integer;
	char *integer_part_pointer;
	int counter = 0;
	char destination[ 1024 ];

	/* Sometimes, d is -0.0 */
	/* -------------------- */
	if ( double_virtually_same( d, 0.0 ) ) d = 0.0;

	sprintf( d_string, "%.3lf", d );

	piece( integer_part, '.', d_string, 0 );
	piece( decimal_part, '.', d_string, 1 );
 
	integer_part_pointer = integer_part + strlen( integer_part ) - 1;

	while( integer_part_pointer >= integer_part )
	{
		*reversed_integer_pointer++ = *integer_part_pointer;

		if ( integer_part_pointer > integer_part
		&&   *(integer_part_pointer - 1) != '-' )
		{
			if ( !(++counter % 3 ) )
			{
				if ( integer_part_pointer != integer_part )
				{
					*reversed_integer_pointer++ = ',';
				}
			}
		}
		integer_part_pointer--;
	}
	*reversed_integer_pointer = '\0';
	reverse_string( buffer, reversed_integer );
	sprintf( destination, "%s.%s", buffer, decimal_part );

	return strdup( destination );
}

char *string_format_mnemonic(
			char *mnemonic,
			char *string )
{
	char *mnemonic_anchor = mnemonic;

	int beginning = 1;

	if ( !string ) return "";

	while( *string )
	{
		if ( *string == '\\' )
		{
			string++;
		}

		if ( beginning )
		{
			if ( isspace( *string )
			||   ispunct( *string ) )
			{
				beginning = 1;
			}
			else
			{
				beginning = 0;
				*mnemonic++ = tolower( *string++ );
			}
		}
		else
		if ( isspace( *string ) )
		{
			beginning = 1;
			*mnemonic++ = '_';
			string++;
		}
		else
		if ( ispunct( *string ) )
		{
			beginning = 1;
			string++;
		}
		else
		{
			*mnemonic++ = tolower( *string++ );
		}
	}
	*mnemonic = '\0';

	return mnemonic_anchor;
}

int string_character_exists(
			char *buffer,
			char c )
{
	return string_character_position( buffer, c );
}

int string_character_position(
			char *buffer,
			char c )
{
	int position = 1;

	if ( !buffer ) return 0;

	while( *buffer )
	{
		if ( *buffer++ == c ) return position;
		position++;
	}
	return 0;
}

char *string_trim_right(
			char *buffer,
			int length )
{
	int str_len;

	if ( !buffer || !*buffer ) return (char *)0;

	str_len = strlen( buffer );

	if ( str_len < length ) return buffer;

	buffer[ str_len - length ] = '\0';
	return buffer;
}

char *string_trim_character(
			char *buffer,
			char delimiter )
{
	int position;
	int offset;

	if ( ! ( position =
			/* ------------------- */
			/* Position is 1 based */
			/* ------------------- */
			string_character_position(
				buffer,
				delimiter ) ) )
	{
		return buffer;
	}

	offset = position - 1;

	string_strcpy( buffer, buffer + offset, 0 );
	return buffer;
}

int string_character_count(
			char ch,
			char *source )
{
	register int count = 0;

	if ( source )
	{
		while( *source )
		{
			if ( *source == ch ) count++;
			source++;
		}
	}
	return count;
}

char *string_rtrim( char *buffer )
{
        char *buf_ptr;

        /* If buffer is empty then return */
        /* ------------------------------ */
        if ( !buffer || !*buffer ) return buffer;

        buf_ptr = buffer + strlen( buffer ) - 1;

        while ( *buf_ptr && isspace( *buf_ptr ) )
        	buf_ptr--;

	if ( isspace( *buf_ptr ) )
		*buf_ptr = '\0';
	else
        	*(buf_ptr + 1) = '\0';

        return buffer;
}

boolean string_exists_substr(
			char *string,
			char *substring )
{
	return ( string_instr( substring, string, 1 ) > -1 );

}

int string_instr(	char *substr,
			char *string,
			int occurrence )
{
        int x,found;
        int str_len_str;
        int str_len_sub;

	if ( !substr || !*substr ) return -1;
	if ( !string || !*string ) return -1;

        str_len_str = strlen( string );
        str_len_sub = strlen( substr );

        for(	x = 0, found = 0;
		x < str_len_str;
		x++ )
	{
                if ( strncasecmp( &string[x], substr, str_len_sub ) == 0 )
                        if ( ++found == occurrence )
                                return x;
	}
        return -1;
}

char *string_input_tmp(	char *input_buffer,
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

char *string_pipe_fetch( char *sys_string )
{
	char buffer[ 65536 ];
	FILE *p;
	int null_input = 0;

	if ( !sys_string ) return (char *)0;

	*buffer = '\0';

	p = popen( sys_string, "r" );

	if ( !string_input( buffer, p, 65536 ) ) null_input = 1;

	pclose( p );

	if ( null_input )
		return (char *)0;
	else
		return strdup( buffer );

}

LIST *string_pipe_list(	char *system_string )
{
	char buffer[ 65536 ];
	FILE *p;
	LIST *list = {0};

	p = popen( system_string, "r" );

	while( string_input( buffer, p, 65536 ) )
	{
		if ( !list ) list = list_new();
		list_set( list, strdup( buffer ) );
	}

	pclose( p );
	return list;
}
