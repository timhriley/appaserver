/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/String.c			   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "float.h"
#include "piece.h"
#include "security.h"
#include "appaserver_error.h"
#include "environ.h"
#include "attribute.h"
#include "sed.h"
#include "String.h"

static boolean string_get_line_check_utf_16 = 1;
static boolean string_is_utf_16 = 0;
static boolean string_utf_16_toggle = 1;

void string_reset_get_line_check_utf_16( void )
{
	string_get_line_check_utf_16 = 1;
	string_is_utf_16 = 0;
	string_utf_16_toggle = 1;
}

char *string_double_quotes_around( char *s )
{
	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	string_quotes_around( s, '\"' );
}

char *string_quotes_around( char *s, char c )
{
	static char destination[ STRING_66K ];

	if ( !s )
	{
		sprintf( destination, "%c%c", c, c );
	}
	else
	{
		sprintf( destination, "%c%s%c", c, s, c );
	}

	return destination;
}

/* Returns input_buffer or (char *)0 if all done. */
/* ---------------------------------------------- */
char *string_input(
		char *input_buffer,
		FILE *infile,
		unsigned int buffer_size )
{
	int in_char;
	unsigned int size = 0;
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
			fprintf(stderr,
				"Exceeded max line length of %d\n"
				"%.75s[more follows]\n",
				 buffer_size - 1,
				 anchor );

			exit( 1 );
		}

		*input_buffer++ = in_char;
	}
}

STRING_OCCURRANCE *string_occurrance_new( char *ptr )
{
	STRING_OCCURRANCE *a;

	if ( ! ( a = (STRING_OCCURRANCE *)
			calloc( 1, sizeof ( STRING_OCCURRANCE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
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

char *string_enforce_utf16(
		char *destination,
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

LIST *string_negative_sequence_occurrance_list(
		char *source )
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

char *string_escape_quote(
		char *destination,
		char *source )
{
	return
	string_escape_character_array(
		destination,
		source,
		"'" /* character_array */ );
}

char *string_escape_full(
		char *destination,
		char *source )
{
	return
	string_escape_character_array(
		destination,
		source,
		"$'#;" /* character_array */ );
}

char *string_escape_quote_dollar(
		char *destination,
		char *source )
{
	return
	string_escape_character_array(
		destination,
		source,
		"'$" /* character_array */ );
}

char *string_escape_dollar(
		char *source )
{
	char destination[ STRING_65K ];

	return
	strdup(
		string_escape_character_array(
			destination,
			source,
			"$" /* character_array */ ) );
}

int string_strlen( char *string )
{
	register int length = 0;

	if ( !string ) return 0;
	while ( *string++ ) length++;

	return length;
}

int string_strcmp( char *s1, char *s2 )
{
	if ( !s1 && !s2 ) return 0;
	if ( !s2 ) return -1;
	if ( !s1 ) return 1;

	return string_strcasecmp( s1, s2 );
}

char *string_strcpy( char *d, char *s, unsigned int buffer_size )
{
	register unsigned int count = 0;
	char *anchor = d;

	if ( !s ) return anchor;
	if ( !d ) return anchor;
	if ( d == s ) return anchor;

	while( *s )
	{
		if ( buffer_size )
		{
			if ( ++count == buffer_size )
			{
				*d = '\0';
				return anchor;
			}
		}

		*d++ = *s++;
	}

	*d = '\0';
	return anchor;
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

int string_strncmp( char *large_string, char *small_string )
{
	int small_string_length;

	if ( !large_string || !small_string ) return -1;
	if ( !*large_string && !*small_string ) return 0;
	if ( !*large_string || !*small_string ) return -1;

	small_string_length = strlen( small_string );
	return strncasecmp( large_string, small_string, small_string_length );
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
		char character )
{
	char *anchor = destination;

	if ( !destination )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: destination is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	if ( !data )
	{
		*destination = '\0';
		return destination;
	}

	while ( *data )
	{
		if ( *data == '\\' )
		{
			if ( *(data + 1) == character )
			{
				*destination++ = *data++;
				*destination++ = *data++;
				continue;
			}
		}

		if ( *data == character )
		{
			*destination++ = '\\';
		}

		*destination++ = *data++;
	}

	*destination = '\0';

	return anchor;
}

char *string_commas_unsigned_long_long( unsigned long long n )
{
        char s[ 64 ];

        sprintf( s, "%llu", n );

	/* Returns static memory or "" */
	/* --------------------------- */
	return string_commas_number_string( s );
}
 
char *string_commas_unsigned_long( unsigned long n )
{
	char s[ 64 ];

	sprintf( s, "%lu", n );

	/* Returns static memory or "" */
	/* --------------------------- */
	return string_commas_number_string( s );
}
 
char *string_commas_integer( int i )
{
	char s[ 64 ];

	sprintf( s, "%d", i );

	/* Returns static memory or "" */
	/* --------------------------- */
	return string_commas_number_string( s );
}
 
char *string_integer( int i )
{
	static char s[ 64 ];

	snprintf( s, sizeof ( s ), "%d", i );

	return s;
}
 
char *string_commas_long( long n )
{
	char s[ 64 ];

	snprintf( s, sizeof ( s ), "%ld", n );

	/* Returns static memory or "" */
	/* --------------------------- */
	return string_commas_number_string( s );
}
 
char *string_commas_rounded_dollar( double d )
{
	char s[ 20 ];

	snprintf( s, sizeof ( s ), "%.0lf", round_double( d ) );

	return
	/* --------------------------- */
	/* Returns static memory or "" */
	/* --------------------------- */
	string_commas_number_string( s );
}

char *string_commas_money( double d )
{
	/* Returns static memory */
	/* --------------------- */
	return string_commas_dollar( d );
}

char *string_commas_dollar( double d )
{
	/* Returns static memory */
	/* --------------------- */
	return string_commas_double( d, 2 );
}

char *string_commas_float(
		double d,
		int decimal_count )
{
	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	string_commas_double( d, decimal_count );
}

char *string_commas_double(
		double d,
		int decimal_count )
{
	char integer_part[ 64 ];
	char buffer[ 64 ];
	char decimal_part[ 8 ];
	char d_string[ 128 ];
	char reversed_integer[ 64 ];
	char *reversed_integer_pointer = reversed_integer;
	char *integer_part_pointer;
	int counter = 0;
	static char destination[ 256 ];

	/* Sometimes, d is -0.0 */
	/* -------------------- */
	if ( float_virtually_same( d, 0.0 ) ) d = 0.0;

	sprintf( d_string, "%.*lf", decimal_count, d );

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
	string_reverse( buffer, reversed_integer );

	if ( decimal_count )
		sprintf( destination, "%s.%s", buffer, decimal_part );
	else
		strcpy( destination, buffer );

	return destination;
}

char *string_mnemonic( char *string )
{
	static char mnemonic[ 128 ];

	if ( string_strlen( string ) > 127 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: string is too big: [%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			string );
		exit( 1 );
	}

	return
	/* ---------------- */
	/* Returns mnemonic */
	/* ---------------- */
	string_format_mnemonic(
		mnemonic,
		string );
}

char *string_format_mnemonic(
		char *mnemonic,
		char *string )
{
	char *mnemonic_anchor = mnemonic;
	int beginning = 1;

	if ( !mnemonic )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: mnemonic is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !string ) return "";

	string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sed_trim_double_spaces(
			string );

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
		if ( isspace( *string )
		||   *string == '_' )
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

boolean string_character_boolean(
		char *datum,
		char c )
{
	return
	string_character_exists(
		datum,
		c );
}

boolean string_character_exists(
		char *datum,
		char c )
{
	if ( !c || !datum )
		return 0;
	else
		return
		(boolean)string_character_position( datum, c );
}

int string_character_position(
		char *data,
		char c )
{
	int position = 1;

	if ( !data ) return 0;

	while( *data )
	{
		if ( *data++ == c ) return position;
		position++;
	}
	return 0;
}

char *string_right(
		char *input,
		int length )
{
	int str_len;

	if ( !input || !*input ) return input;

	str_len = strlen( input );

	if ( str_len <= length ) return input;

	return input + (str_len - length);
}

char *string_trim_left_spaces( char *buffer )
{
	char *ptr;

	if ( !buffer || !*buffer ) return buffer;

	ptr = buffer;

	while ( *ptr )
	{
		if ( !isspace( *ptr ) ) break;
		string_strcpy( ptr, ptr + 1, 0 );
		ptr++;
	}

	return buffer;
}

char *string_trim_right_spaces( char *buffer )
{
	char *ptr;

	if ( !buffer || !*buffer ) return buffer;

	ptr = buffer + strlen( buffer ) - 1;

	while ( ptr > buffer )
	{
		if ( !isspace( *ptr ) ) break;
		ptr --;
	}

	*(ptr + 1) = '\0';

	return buffer;
}

char *string_trim_character(
		char *datum,
		char character )
{
	int position;
	int offset;

	while ( 1 )
	{
		if ( ! ( position =
				/* ------------------- */
				/* Position is 1 based */
				/* ------------------- */
				string_character_position(
					datum,
					character ) ) )
		{
			return datum;
		}

		offset = position - 1;
		string_strcpy( datum + offset, datum + position, 0 );
	}
}

char *string_trim_leading_character(
		char *data,
		char character )
{
	char *end_pointer;

	if ( !data || !*data ) return data;

	/* Start at NULL */
	/* ------------- */
	end_pointer = data + strlen( data );

	while ( end_pointer > data )
	{
		end_pointer--;

		if ( *end_pointer == character )
		{
			string_strcpy( data, end_pointer + 1, 0 );
			break;
		}
	}

	return data;
}

int string_character_count(
		char character,
		char *source )
{
	register int count = 0;

	if ( source )
	{
		while( *source )
		{
			if ( *source == character ) count++;
			source++;
		}
	}
	return count;
}

char *string_rtrim( char *buffer )
{
	return
	/* ----------------------------------------- */
	/* Returns buffer with end shortened (maybe) */
	/* ----------------------------------------- */
	string_trim_right_spaces( buffer );
}

boolean string_exists(
		char *string,
		char *substring )
{
	return string_exists_substring( string, substring );
}

boolean string_exists_substring(
		char *string,
		char *substring )
{
	return ( string_instr( substring, string, 1 ) > -1 );
}

int string_instr(
		char *substr,
		char *string,
		int occurrence )
{
        int x;
	boolean found = 0;
        int str_len_str;
        int str_len_sub;
	boolean inside_double_quote = 0;

	if ( !substr || !*substr ) return -1;
	if ( !string || !*string ) return -1;

        str_len_str = strlen( string );
        str_len_sub = strlen( substr );

        for(	x = 0;
		x < str_len_str;
		x++ )
	{
		if ( string[ x ] == '"' )
		{
			inside_double_quote = 1 - inside_double_quote;
			continue;
		}

                if ( !inside_double_quote
		&&   strncasecmp( &string[ x ], substr, str_len_sub ) == 0 )
		{
                        if ( ++found == occurrence )
                                return x;
		}
	}
        return -1;
}

char *string_pipe_input( char *system_string )
{
	return string_pipe( system_string );
}

char *string_fetch_pipe( char *system_string )
{
	return string_pipe( system_string );
}

char *string_fetch( char *system_string )
{
	return string_pipe( system_string );
}

char *string_pipe_fetch( char *system_string )
{
	return string_pipe( system_string );
}

char *string_pipe( char *system_string )
{
	char buffer[ STRING_64K ];
	FILE *pipe;
	int null_input = 0;

	if ( !system_string ) return (char *)0;

	*buffer = '\0';

	pipe = security_read_pipe( system_string );

	if ( !string_input( buffer, pipe, STRING_64K ) ) null_input = 1;

	pclose( pipe );

	if ( null_input )
		return (char *)0;
	else
		return strdup( buffer );
}

LIST *string_pipe_list(	char *system_string )
{
	char buffer[ STRING_64K ];
	FILE *pipe;
	LIST *list = {0};

	pipe = security_read_pipe( system_string );

	while( string_input( buffer, pipe, 65536 ) )
	{
		if ( !list ) list = list_new();
		list_set( list, strdup( buffer ) );
	}

	pclose( pipe );

	return list;
}

char *string_delimiter_repeat(
		char *string,
		char delimiter,
		int number_times )
{
	char return_buffer[ 2048 ];
	char *ptr = return_buffer;

	*ptr = '\0';

	for(	;
		number_times;
		number_times-- )
	{
		if ( !*ptr ) ptr += sprintf( ptr, "%c", delimiter );

		ptr += sprintf( ptr, "%s", string );
	}

	return strdup( return_buffer );
}

char *string_repeat(	char *string,
			int number_times )
{
	char return_buffer[ 2048 ];
	char *ptr = return_buffer;

	*ptr = '\0';

	for(	;
		number_times;
		number_times-- )
	{
		ptr += sprintf( ptr, "%s", string );
	}

	return strdup( return_buffer );
}

char *string_ftoa(
		double f,
		int decimal_count )
{
	static char s[ 16 ];

	snprintf( s, sizeof ( s ), "%.*lf", decimal_count, f );

	return s;
}

char *string_itoa( int i )
{
	static char s[ 16 ];

	sprintf( s, "%d", i );

	return s;
}

char *string_escape(
		char *destination,
		char *source,
		char *character_array )
{
	return
	string_escape_character_array(
		destination,
		source,
		character_array );
}

char *string_escape_array(
		char *character_array,
		char *source )
{
	char destination[ STRING_65K ];
	char local_source[ STRING_65K ];

	if ( !character_array )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: character_array is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !source ) return strdup( "" );

	if ( strlen( source ) > STRING_64K )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: source exceeded max length=%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			STRING_64K );
		exit( 1 );
	}

	strcpy( local_source, source );

	while ( *character_array )
	{
		string_escape_character(
			destination,
			local_source,
			*character_array
				/* character_to_escape */ );

		strcpy( local_source, destination );
		character_array++;
	}

	return strdup( destination );
}

char *string_escape_character_array(
		char *destination,
		char *source,
		char *character_array )
{
	char local_source[ STRING_65K ];

	if ( !destination
	||   !source
	||   !character_array )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strlen( source ) > STRING_64K )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: source exceeded max length=%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			STRING_64K );
		exit( 1 );
	}

	strcpy( local_source, source );

	while ( *character_array )
	{
		string_escape_character(
			destination,
			local_source,
			*character_array
				/* character_to_escape */ );

		strcpy( local_source, destination );
		character_array++;
	}

	return destination;
}

char *string_unescape_character_array(
		char *destination,
		char *source,
		char *character_array )
{
	char local_source[ STRING_64K ];

	string_strcpy( local_source, source, STRING_64K );

	while ( *character_array )
	{
		string_unescape_character(
			destination,
			local_source,
			*character_array
				/* character_to_unescape */ );

		character_array++;

		string_strcpy(
			local_source,
			destination,
			STRING_64K );
	}
	return destination;
}

char *string_trim_character_array(
		char *datum,
		char *character_array )
{
	if ( !datum || !*datum || !character_array || !character_array )
	{
		return datum;
	}

	while ( *character_array )
	{
		string_trim_character(
			datum,
			*character_array++ );
	}

	return datum;
}

char *string_strncpy(
		char *destination,
		char *source,
		int count )
{
	int str_len;

	if ( !destination )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: destination is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*destination = '\0';

	if ( !source ) return destination;

	str_len = string_strlen( source );

	if ( count > str_len ) count = str_len;

	strncpy( destination, source, count );
	*(destination + count) = '\0';

	return destination;
}

char *string_in_clause(	LIST *data_list )
{
	char in_clause[ STRING_64K ];
	char *ptr = in_clause;
	char *data;

	*ptr = '\0';

	if ( list_rewind( data_list ) )
	do {
		data = list_get( data_list );

		if ( ptr != in_clause ) ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "'%s'", data );

	} while( list_next( data_list ) );

	return strdup( in_clause );
}

int string_length( char *string )
{
	if ( !string )
		return 0;
	else
		return strlen( string );
}

char *string_trim( char *buffer )
{
        /* If buffer is empty then return */
        /* ------------------------------ */
        if ( !buffer || !*buffer ) return buffer;

	string_trim_right_spaces( buffer );

	return
	string_trim_left_spaces( buffer );
}

char *string_extract_lt_gt_delimited(
		char *destination,
		char *source )
{
	char *ptr = destination;
	char destination_delimiter;

	*ptr = '\0';
	destination_delimiter = *source++;

	if ( destination_delimiter == '<' ) destination_delimiter = '>';

	while(	*source && *source != destination_delimiter )
	{
		*ptr++ = *source++;
	}
	*ptr = '\0';

	return destination;
}

char *string_trim_index(
		char *destination,
		char *source )
{
	char *end_ptr;

	if ( !source || !destination ) return (char *)0;

	strcpy( destination, source );

	if ( !*destination ) return destination;

 	end_ptr = destination + strlen( destination ) - 1;

	while( end_ptr != destination )
	{
		if ( isdigit( *end_ptr ) )
		{
			end_ptr--;
			continue;
		}
		if ( *end_ptr == '_' )
		{
			*end_ptr = '\0';
			break;
		}
		else
		{
			break;
		}
	}
	return destination;
}

int string_index( char *attribute_name )
{
	char *end_ptr;

	if ( !attribute_name || !*attribute_name ) return -1;

 	end_ptr = attribute_name + strlen( attribute_name ) - 1;

	while( end_ptr > attribute_name )
	{
		if ( *end_ptr == '_' )
		{
			end_ptr++;
			return atoi( end_ptr );
		}

		if ( isdigit( *end_ptr ) )
			end_ptr--;
		else
			return -1;
	}
	return -1;
}

boolean string_exists_character(
		char *s,
		char ch )
{
	while( *s )
	{
		if ( *s == ch ) return 1;
		s++;
	}
	return 0;
}

boolean string_exists_character_string(
		const char *character_string,
		char *string )
{
	while ( *character_string++ )
	{
		if ( string_exists_character(
			string,
			*character_string ) )
		{
			return 1;
		}
	}

	return 0;
}

char string_delimiter( char *string )
{
	if ( string_exists_character( string, '|' ) )
		return '|';
	else
	if ( string_exists_character( string, '^' ) )
		return '^';
	else
	if ( string_exists_character( string, ',' ) )
		return ',';
	else
	if ( string_exists_character( string, ';' ) )
		return ';';
	else
	if ( string_exists_character( string, ':' ) )
		return ':';
	else
		return 0;
}

char *string_separate_delimiter(
			char *destination,
			char *source )
{
	boolean beginning = 1;
	char *destination_ptr = destination;

	*destination_ptr = '\0';

	if ( !source ) return destination;

	while( *source )
	{
		if ( *source == '(' )
		{
			*destination_ptr++ = *source++;
			beginning = 1;

			while( *source )
			{
				*destination_ptr++ = *source++;
				if ( *source == ')' ) break;
			}
		}

		if ( *source == '\\' )
		{
			*destination_ptr++ = *source++;
			beginning = 0;
		}
		else
		if ( beginning )
		{

			if ( !isspace( *source )
			&& ( !ispunct( *source )
			&&   *source != '\'' ) )
			{
				*destination_ptr++ = *source++;
				beginning = 0;
			}
			else
			{
				*destination_ptr++ = *source++;
			}
		}
		else
		if ( *source == '_' || *source == '|' || *source == '^' )
		{
			beginning = 1;
			*destination_ptr++ = ' ';
			source++;
		}
		else
		if ( isspace( *source )
		|| ( ispunct( *source ) && *source != '\'' ) )
		{
			beginning = 1;
			*destination_ptr++ = *source++;
		}
		else
		{
			*destination_ptr++ = *source++;
		}
	}
	*destination_ptr = '\0';

	return destination;
}

char *string_initial_capital(
		char *destination,
		char *source )
{
	boolean beginning = 1;
	char *destination_ptr = destination;

	if ( !destination ) return "";

	if ( !source )
	{
		*destination = '\0';
		return destination;
	}

	while( *source )
	{
		if ( *source == '(' )
		{
			*destination_ptr++ = *source++;
			beginning = 1;

			while( *source )
			{
				*destination_ptr++ = *source++;
				if ( *source == ')' ) break;
			}
		}

		if ( *source == '\\' )
		{
			*destination_ptr++ = *source++;
			beginning = 0;
		}
		else
		if ( beginning )
		{
			if ( !isspace( *source )
			&& ( !ispunct( *source )
			&&   *source != '\'' ) )
			{
				*destination_ptr++ = toupper( *source++ );
				beginning = 0;
			}
			else
			{
				*destination_ptr++ = *source++;
			}
		}
		else
		/*
		if ( *source == '_' || *source == '|' || *source == '^' )
		*/
		if ( *source == '_' || *source == '^' )
		{
			beginning = 1;
			*destination_ptr++ = ' ';
			source++;
		}
		else
		if ( isspace( *source )
		|| ( ispunct( *source ) && *source != '\'' ) )
		{
			beginning = 1;
			*destination_ptr++ = *source++;
		}
		else
		{
			*destination_ptr++ = *source++;
		}
	}

	*destination_ptr = '\0';

	/* Keep booleans at lower case */
	/* --------------------------- */
	string_search_replace( destination, " Yn", " yn" );

	if ( strcmp( destination, "Y" ) == 0 ) *destination = 'y';
	if ( strcmp( destination, "N" ) == 0 ) *destination = 'n';

	return destination;
}

char *string_search_replace_character(
			char *source_destination,
			char search_character,
			char replace_character )
{
	char *ptr = source_destination;

	while ( *ptr )
	{
		if ( *ptr == search_character )
		{
			*ptr = replace_character;
		}
		ptr++;
	}

	return source_destination;
}

char *string_search_replace(
		char *source_destination,
		char *search_string,
		char *replace_string )
{
        int here;
	int len_search;
	char *anchor;
	int len_replace;

	if ( !source_destination ) return (char *)0;

	if ( !search_string || !replace_string )
		return source_destination;

	if ( strcmp( search_string, replace_string ) == 0 )
		return source_destination;

	len_search = strlen( search_string );
	len_replace = strlen( replace_string );
	anchor = source_destination;

        while( 1 )
	{
                if ( ( here =
			string_instr(
				search_string,
				source_destination,
				1 ) ) == -1 )
		{
                        return anchor;
		}

                string_delete( source_destination, here, len_search );
                string_insert( source_destination, replace_string, here );

		source_destination += ( here + len_replace );
        }
}

char *string_with_space_search_replace(
		char *source_destination,
		char *search_string,
		char *replace_string )
{
        int here;
	int len_search;
	int len_replace;
	char *anchor;
	char with_space_search_string[ 65536 ];
	char with_space_replace_string[ 65536 ];

	if ( !search_string || !replace_string ) return source_destination;

	if ( strcmp( search_string, replace_string ) == 0 )
		return source_destination;

	sprintf(with_space_search_string,
		" %s",
		search_string );

	sprintf(with_space_replace_string,
		" %s",
		replace_string );

	len_search = strlen( with_space_search_string );
	len_replace = strlen( with_space_replace_string );
	anchor = source_destination;

        while( 1 )
	{
                if ( ( here =
			string_instr(
				with_space_search_string,
				source_destination,
				1 ) ) == -1 )
		{
                        return anchor;
		}

                string_delete( source_destination, here, len_search );

                string_insert(
			source_destination,
			with_space_replace_string,
			here );

		source_destination += ( here + len_replace );
        }
}

char *string_insert(
		char *string,
		char *substring,
		int pos )
{
        char *temp = malloc( strlen( string ) + strlen( substring ) + 1 );

        /* Copy first part */
	/* --------------- */
        strncpy( temp, string, pos );
        temp[ pos ] = '\0';

        /* place substring in middle */
	/* ------------------------- */
        strcat( temp, substring );

        /* Copy rest of string */
	/* ------------------- */
        strcat( temp, &string[ pos ] );

        /* copy entire string back to original */
	/* ----------------------------------- */
        strcpy( string, temp );

        free( temp );

        return string;
}

char *string_delete(
		char *string,
		int start,
		int num_chars )
{
        int far_str;

        for(	far_str = start + num_chars;
                far_str <= (int)strlen( string );
		far_str++, start++ )
	{
		string[ start ] = string[ far_str ];
	}

        return string;
}

/* Sample: attribute_name = "station_1" */
/* ------------------------------------ */
int string_row_number( char *attribute_name )
{
	char *end_ptr;

	if ( !attribute_name || !*attribute_name ) return -1;

 	end_ptr = attribute_name + strlen( attribute_name ) - 1;

	while( end_ptr != attribute_name )
	{
		if ( *end_ptr == '_' )
		{
			end_ptr++;
			return atoi( end_ptr );
		}

		if ( isdigit( *end_ptr ) )
			end_ptr--;
		else
			return -1;
	}
	return -1;
}

char *string_file_fetch(
		char *filename,
		char *delimiter )
{
	char string[ STRING_64K ];
	char input[ 4096 ];
	char *ptr = string;
	FILE *file;

	if ( ! ( file = fopen( filename, "r" ) ) ) return (char *)0;

	while( string_input( input, file, 1024 ) )
	{
		if ( delimiter )
		{
			if ( ptr != string )
			{
				ptr += sprintf( ptr, "%s", delimiter );
			}
		}

		ptr += sprintf(
			ptr,
			"%s",
			input );
	}

	fclose( file );

	if ( ptr == string )
		return (char *)0;
	else
		return strdup( string );
}

char *string_append(
		char *message_list_string,
		char *message_string,
		char *delimiter )
{
	char append_string[ STRING_64K ];

	if ( !message_string || !delimiter )
		return strdup( "" );

	if ( !message_list_string )
	{
		strcpy( append_string, message_string );
	}
	else
	{
		sprintf(append_string,
			"%s%s%s",
			message_list_string,
			delimiter,
			message_string );

		free( message_list_string );
	}

	free( message_string );

	return strdup( append_string );
}

char string_last_character( char *string )
{
	int str_length;

	if ( !string ) return 0;

	if ( ! ( str_length = strlen( string ) ) ) return 0;

	return *(string + str_length - 1);
}

char *string_up( char *s )
{
        char *hold = s;

        while ( *s )
        {
                *s = toupper( *s );
                s++;
        }
        return hold;
}

char *string_low( char *s )
{
        char *hold = s;

	if ( !s ) return s;

        while ( *s )
        {
                *s = tolower( *s );
                s++;
        }
        return hold;
}

char *string_commas_number_string( char *s )
{
	static char return_string[ 64 ];
	char *r_ptr;
	char *s_ptr;
	int c = 0;
 
	if ( !string_strlen( s ) ) return "";

	r_ptr = &return_string[ 63 ];
	*r_ptr-- = '\0';
 
	s_ptr = s + strlen( s ) - 1;

	if ( *s_ptr != '.' && !isdigit( *s_ptr ) ) return s;

	if ( string_instr( ".", s, 1 ) != -1 )
	{
		while( s_ptr >= s )
		{
			if ( *s_ptr == '.' )
			{
				*r_ptr-- = *s_ptr--;
				break;
			}
			else
			{
				*r_ptr-- = *s_ptr--;
			}
		}
	}

	while( s_ptr >= s )
	{
		*r_ptr-- = *s_ptr--;

		if ( *s_ptr == '-' )
		{
			*r_ptr-- = *s_ptr--;
			continue;
		}

		if ( ++c == 3 )
		{
			if ( s_ptr != s - 1 ) *r_ptr-- = ',';
			c = 0;
		}
	}

	return r_ptr + 1;
}

char *string_strdup( char *string )
{
	if ( !string )
		return strdup( "" );
	else
		return strdup( string );
}

double string_atof( char *string )
{
	if ( !string || !*string || isalpha( *string ) )
		return 0.0;
	else
	{
		return
		atof(
			string_trim_character(
				string,
				',' /* character */ ) );
	}
}

int string_atoi( char *string )
{
	if ( !string || !*string || isalpha( *string ) )
		return 0;
	else
		return atoi( string );
}

char *string_double_quote_comma(
		char *source,
		char delimiter )
{
	static char destination[ STRING_64K ];
	char *ptr;
	char item_piece[ STRING_65K ];
	int p;

	if ( !delimiter )
	{
		string_strcpy( destination, source, 0 );
	}
	else
	if ( !source )
	{
		strcpy( destination, "\"\"" );
	}
	else
	for(	ptr = destination, p = 0;
		piece( item_piece, delimiter, source, p );
		p++ )
	{
		if ( p ) ptr += sprintf( ptr, "," );

		ptr +=
			sprintf(ptr,
				"%s",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_double_quotes_around( 
					item_piece /* source */ ) );
	}

	return destination;
}

boolean string_file_write(
		char *filename,
		char *string )
{
	FILE *file;

	if ( !filename || !string ) return 0;

	if ( ! ( file = fopen( filename, "w" ) ) ) return 0;

	fprintf( file, "%s\n", string );
	fclose( file );

	return 1;
}


void string_replace_command_line(
		char *command_line /* in/out */,
		char *string,
		char *placeholder )
{
	char *double_quotes_around;

	if ( !command_line
	||   !placeholder )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !string ) string = "";

	if ( strlen( string ) + 2 >= STRING_64K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_64K );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	double_quotes_around =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_double_quotes_around(
			string );

	string_with_space_search_replace(
		command_line /* source_destination */,
		placeholder /* search_string */,
		double_quotes_around /* replace_string */ );
}

char *string_unescape_character(
		char *destination,
		char *datum,
		char character_to_unescape )
{
	char *anchor = destination;

	if ( !destination )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: destination is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !datum )
	{
		*destination = '\0';
		return destination;
	}

	while ( *datum )
	{
		if ( *datum == '\\'
		&&   *(datum + 1) == character_to_unescape )
		{
			datum++;
		}

		*destination++ = *datum++;
	}

	*destination = '\0';

	return anchor;
}

char *string_reverse(
		char *destination,
		char *source )
{
	char *marker = destination;
	char *ptr = source + strlen( source ) - 1;

	while( ptr >= source )
	{
		*destination++ = *ptr--;
	}
	*destination = '\0';
	return marker;
}

char *string_trim_trailing_character(
		char *source_destination,
		char character )
{
	char *end_ptr;

	if ( !source_destination ) return (char *)0;

 	end_ptr = source_destination + strlen( source_destination ) - 1;

	if ( *end_ptr == character ) *end_ptr = '\0';

	return source_destination;
}

char *string_trim_right(
		char *source_destination,
		int length )
{
	int str_len;

	str_len = string_strlen( source_destination );

	if ( str_len > length )
	{
		source_destination[ str_len - length ] = '\0';
	}
	else
	{
		*source_destination = '\0';
	}

	return source_destination;
}

char *string_search_replace_special_characters( char *buffer )
{
	string_search_replace( buffer, "`", "'" );
	string_search_replace( buffer, "\\", "" );
	string_search_replace( buffer, "\"", "'" );

	return buffer;
}

char *string_escape_single_quotes( char *source )
{
	static char destination[ 4096 ];

	string_escape_character( destination, source, '\'' );

	return destination;
}

char *string_remove_thousands_separator(
		char *destination,
		char *source )
{
	string_strcpy( destination, source, 0 );

	if ( !string_is_number( destination ) )
	{
		return destination;
	}
	else
	{
		return string_trim_character( destination, ',' );
	}
}

boolean string_is_number( char *string )
{
	while ( *string )
	{
		if ( string_exists_character( "+-,.", *string )  )
		{
			string++;
			continue;
		}

		if ( isdigit( *string ) )
		{
			string++;
			continue;
		}

		return 0;
	}
	return 1;
}

int string_instr_character(
		char character,
		char *string )
{
        int x;

	for( x = 0; *string; x++ )
		if ( *string++ == character )
			return x;

	return -1;
}

char *string_midstr(
		char *destination,
		char *source,
		int start,
		int how_many )
{
	if ( !destination
	||   !source )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	strncpy( destination, source + start, how_many );
	*( destination + how_many ) = '\0';

	return destination;
}

LIST *string_list( char *string )
{
	char delimiter;
	LIST *list = list_new();

	if ( string && *string )
	{
		delimiter = string_delimiter( string );

		if ( !delimiter )
		{
			list_set( list, string );
		}
		else
		{
			char buffer[ 1024 ];
			int p;

			for (	p = 0;
				piece( buffer, delimiter, string, p );
				p++ )
			{
				list_set(
					list,
					strdup( buffer ) );
			}
		}
	}

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

char *string_character_fill(
		char *string,
		char character )
{
	char *ptr = string;

	if ( !string )
	{
		char message[ 128 ];

		sprintf(message, "string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	while ( *ptr ) *ptr++ = character;
	return string;
}

void string_free( char *string )
{
	if ( string ) free( string );
}

char *string_skip_prefix(
		const char *prefix,
		char *string )
{
	if ( !string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( string_strncmp( string, (char *)prefix ) == 0 )
		return string + strlen( prefix );
	else
		return string;
}

boolean string_mnemonic_boolean( char *string )
{
	if ( isdigit( *string ) ) return 0;

	for ( ; *string; string++ )
	{
		if ( isalpha( *string ) ) continue;
		if ( isdigit( *string ) ) continue;
		if ( *string == '_' ) continue;

		return 0;
	}

	return 1;
}

char *string_left(
		char *destination,
		char *source,
		int how_many )
{
	if ( !destination
	||   !source )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns destination */
	/* ------------------- */
	string_middle(
		destination,
		source,
		0 /* start */,
		how_many );
}

char *string_substr(
		char *destination,
		char *source,
		int start,
		int how_many )
{
	return
	/* ------------------- */
	/* Returns destination */
	/* ------------------- */
	string_middle(
		destination,
		source,
		start,
		how_many );
}

char *string_middle(
		char *destination,
		char *source,
		int start,
		int how_many )
{
	if ( !destination
	||   !source )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_strncpy( destination, source + start, how_many );
	*( destination + how_many ) = '\0';

	return destination;
}

boolean string_email_address_boolean( char *string )
{
	if ( !string || !*string )
	{
		return 0;
	}

	if ( string_exists_character(
		string, ' ' ) )
	{
		return 0;
	}

	if ( string_exists_character_string(
		SECURITY_ESCAPE_CHARACTER_STRING,
		string ) )
	{
		return 0;
	}

	if ( string_character_exists( string, '@' ) )
	{
		return 1;
	}

	return 0;
}

char *string_remove_character_string(
		char *source_destination,
		const char *character_string )
{
	char character;

	while ( ( character = *character_string ) )
	{
		string_remove_character( source_destination, character );
		character_string++;
	}

	return source_destination;
}

char *string_remove_character( char *source_destination, char character )
{
	char *anchor = source_destination;

	if ( source_destination )
	{
		while( *source_destination )
		{
			if ( *source_destination == character )
				string_strcpy(	source_destination,
						source_destination + 1,
						0 );
			else
				source_destination++;
		}
	}

	return anchor;
}

int string_strict_case_instr(
		char *substr,
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

        for(	x = 0,found = 0;
		x < str_len_str;
		x++ )
	{
                if ( strncmp( &string[x], substr, str_len_sub ) == 0 )
                        if ( ++found == occurrence )
                                return x;
	}
        return -1;
}

int string_strict_case_strcmp( char *s1, char *s2 )
{
	if ( !s1 && !s2 ) return 0;
	if ( !s2 ) return -1;
	if ( !s1 ) return 1;

	return strcmp( s1, s2 );
}

int string_strcasecmp( char *s1, char *s2 )
{
	char tmp1[ STRING_16K ];
	char tmp2[ STRING_16K ];

	if ( !s1 && !s2 ) return 0;
	if ( !s2 ) return -1;
	if ( !s1 ) return 1;

	if ( strlen( s1 ) >= STRING_16K
	||   strlen( s2) >= STRING_16K )
	{
		return strcasecmp( s1, s2 );
	}

	strcpy( tmp1, s1 );
	strcpy( tmp2, s2 );

	string_remove_character( tmp1, '\\' );
	string_remove_character( tmp2, '\\' );

	return strcasecmp( tmp1, tmp2 );
}

boolean string_populated_boolean(
		const char *filler_string,
		char *string )
{
	if ( string
	&&   *string
	&&   strcmp( string, filler_string ) != 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
