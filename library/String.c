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
#include "float.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_error.h"
#include "environ.h"
#include "attribute.h"
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
	return string_escape_character_array(
			destination,
			source,
			"'" /* character_array */ );
}

char *string_escape_full(
			char *destination,
			char *source )
{
	return string_escape_character_array(
			destination,
			source,
			"$'#;" /* character_array */ );
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
			char character )
{
	char *anchor = destination;

	if ( !data )
	{
		*destination = '\0';
		return destination;
	}

	while ( *data )
	{
		if ( *data == character ) *destination++ = '\\';

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
			char *data,
			char c )
{
	return string_character_position( data, c );
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

char *string_right(	char *input,
			int length )
{
	int str_len;

	if ( !input || !*input ) return input;

	str_len = strlen( input );

	if ( str_len <= length ) return input;

	return input + (str_len - length);
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
			char *data,
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
					data,
					character ) ) )
		{
			return data;
		}

		offset = position - 1;
		string_strcpy( data + offset, data + position, 0 );
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

boolean string_exists(
			char *string,
			char *substring )
{
	return string_exists_substr( string, substring );
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

char *string_fetch_pipe( char *system_string )
{
	return string_pipe_fetch( system_string );
}

char *string_pipe_fetch( char *system_string )
{
	char buffer[ 65536 ];
	FILE *p;
	int null_input = 0;

	if ( !system_string ) return (char *)0;

	*buffer = '\0';

	p = popen( system_string, "r" );

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

char *string_itoa( int i )
{
	static char s[ 16 ];

	if ( !i )
		*s = '\0';
	else
		sprintf( s, "%d", i );
	return s;
}

char *security_escape_character_array(
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
	char local_source[ STRING_WHERE_BUFFER ];

	string_strcpy( local_source, source, STRING_WHERE_BUFFER );

	while ( *character_array )
	{
		string_escape_character(
			destination,
			local_source,
			*character_array
				/* character_to_escape */ );

		character_array++;

		if ( *character_array )
		{
			string_strcpy(
				local_source,
				destination,
				STRING_WHERE_BUFFER );
		}
	}
	return destination;
}

char *string_trim_character_array(
			char *data,
			char *character_array )
{
	if ( !data || !*data || !character_array || !character_array )
	{
		return data;
	}

	while ( *character_array )
	{
		string_trim_character(
			data,
			*character_array++ );
	}

	return data;
}

char *string_trim_number_characters(
			char *number,
			char *datatype_name )
{
	if ( !datatype_name
	||   !*datatype_name
	||   !number
	||   !*number )
	{
		return number;
	}

	if ( attribute_is_number( datatype_name ) )
	{
		return
		string_trim_character_array(
			number,
			",$" );
	}
	else
	{
		return number;
	}
}

char *string_strncpy(	char *destination,
			char *source,
			int count )
{
	char *results;

	*destination = '\0';

	if ( !source ) return destination;

	results = strncpy( destination, source, count );
	*(destination + count) = '\0';

	return results;
}

char *string_in_clause(	LIST *data_list )
{
	char in_clause[ STRING_WHERE_BUFFER ];
	char *ptr = in_clause;
	char *data;
	char *escaped_data;

	if ( !list_rewind( data_list ) ) strdup( "" );

	*ptr = '\0';

	do {
		data = list_get_pointer( data_list );

		if ( !*ptr ) ptr += sprintf( ptr, "," );

		escaped_data = timlib_sql_injection_escape( data );

		ptr += sprintf( ptr, "'%s'", escaped_data );

		free( escaped_data );

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

char *string_decode_html_post(
			char *destination,
			char *source )
{
	char *source_ptr, *destination_ptr;
	char hex[ 3 ];
	int c;

	source_ptr = source;
	destination_ptr = destination;
	*(hex + 2) = '\0';

	while( *source_ptr )
	{
		if ( *source_ptr == '%' )
		{
			/* Sometimes, there's a preceeding "25" */
			/* ------------------------------------ */
			if ( strncmp( source_ptr + 1, "25", 2 ) == 0 )
			{
				strcpy( ( source_ptr + 1 ),
					( source_ptr + 3 ) );
			}

			/* If 10 */
			/* ------ */
			if ( strncmp( source_ptr + 1, "0A", 2 ) == 0 )
			{
				*destination_ptr++ = *source_ptr++;
				*destination_ptr++ = *source_ptr++;
				*destination_ptr++ = *source_ptr++;
				continue;
			}

			/* ----- */
			/* If 13 */
			/* ----- */
			if ( strncmp( source_ptr + 1, "0D", 2 ) == 0 )
			{
				*destination_ptr++ = *source_ptr++;
				*destination_ptr++ = *source_ptr++;
				*destination_ptr++ = *source_ptr++;
				continue;
			}

			if ( *(source_ptr + 1)
			&&     isalnum( *(source_ptr + 1) ) )
			{
				*hex = *(source_ptr + 1);
				*(hex + 1) = *(source_ptr + 2);
				sscanf( hex, "%x", &c );

				/* Make some conversions */
				/* --------------------- */
				/* if ( c == '+' ) c = ' '; */
				if ( c == '"' ) c = '\'';

				if ( c == '=' )
					*destination_ptr++ = '\\';
				if ( c == '&' )
					*destination_ptr++ = '\\';

				*destination_ptr++ = c;
				source_ptr += 3;
				continue;
			}
		}

		if ( *source_ptr == '\\' )
		{
			if ( *( source_ptr + 1 ) == ',' )
			{
				source_ptr++;
				*destination_ptr++ = *source_ptr++;
				continue;
			}
		}

		*destination_ptr++ = *source_ptr++;
	}
	*destination_ptr = '\0';
	return destination;
}

char *string_trim( char *buffer )
{
        char *buf_ptr = buffer;

        /* If buffer is empty then return */
        /* ------------------------------ */
        if ( !buffer || !*buffer ) return buffer;

        /* First trim leading spaces */
        /* ------------------------- */
        while( *buf_ptr && isspace( *buf_ptr ) )
        	buf_ptr++;

        /* If *buf_ptr is NULL then buffer was just spaces */
        /* ----------------------------------------------- */
        if ( !*buf_ptr )
        {
        	*buffer = '\0';
        	return buffer;
        }

        /* Flush left the buffer */
        /* --------------------- */
        if ( buffer != buf_ptr ) string_strcpy( buffer, buf_ptr, 0 );

        /* Trim trailing spaces */
        /* -------------------- */
        buf_ptr = buffer + strlen( buffer ) - 1;

        while (	*buf_ptr && isspace( *buf_ptr ) )
	{
        	buf_ptr--;
	}

        *(buf_ptr + 1) = '\0';

        return buffer;
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

char *string_trim_index( char *string )
{
	char *end_ptr;

	if ( string || !*string ) return string;

 	end_ptr = string + strlen( string ) - 1;

	while( end_ptr != string )
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
	return string;
}

/* Sample: attribute_name = "station_1" */
/* ------------------------------------ */
int string_index( char *source )
{
	char *end_ptr;

	if ( !source || !*source ) return -1;

 	end_ptr = source + strlen( source ) - 1;

	while( end_ptr > source )
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

char *string_initial_capital(
			char *destination,
			char *source )
{
	boolean beginning = 1;
	char *destination_ptr = destination;

	if ( !source ) return "";

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
		if ( *source == '_' || *source == '|' )
		{
			beginning = 1;
			*destination_ptr++ = *source++;
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
	search_replace_string( destination, " Yn", " yn" );

	if ( strcmp( destination, "Y" ) == 0 ) *destination = 'y';
	if ( strcmp( destination, "N" ) == 0 ) *destination = 'n';

	return destination;
}

char *string_search_replace(
			char *source_destination,
			char *search_string ,
			char *replace_string )
{
        int here;
	int len_search = strlen( search_string );
	char *anchor;
	int str_len = strlen( replace_string );

	anchor = source_destination;

	if ( strcmp( search_string, replace_string ) == 0 )
		return anchor;

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

		source_destination += ( here + str_len );
        }
}

char *string_insert(	char *string,
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

char *string_delete(	char *string,
			int start,
			int num_chars )
{
        int far_str;

        for(	far_str = start + num_chars;
                far_str <= strlen( string );
		far_str++, start++ )
	{
		string[ start ] = string[ far_str ];
	}

        return string;
}

