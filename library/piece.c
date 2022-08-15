/* library/piece.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "column.h"
#include "timlib.h"
 
/* Package variable */
/* ---------------- */
static char piece_buffer[ MAX_NUM_FIELDS ][ MAX_FIELD_LEN ];
static int num_piece_buffer;

/* Default to trim each field */
/* -------------------------- */
static int piece_do_trim = 1;

/* --------------------------------
  assume s = "abc|def|ghi"
         piece( buffer, '|', s, 2 )
	 buffer = "ghi"
  --------------------------------- */
char *piece( char *destination, char delimiter, char *source, int offset )
{
        int mark = 0;
        char *buf_ptr = destination;
 
	*destination = '\0';

	if ( !delimiter )
	{
		strcpy( destination, source );
		return destination;
	}

	if ( delimiter == PIECE_QUOTE_COMMA_DELIMITER_CODE )
	{
		return piece_quote_comma(
				destination,
				source,
				offset );
	}
	else
	if ( delimiter == PIECE_SPACES_DELIMITER_CODE
	||   delimiter == 9
	||   delimiter == 't' )
	{
		return column(
				destination,
				offset,
				source );
	}

        *buf_ptr = '\0';
 
        /* if offset is not zero, find occurrence */
        /* --------------------------------------- */
        if ( offset )
	{
                while( *source )
		{
			if ( *source == '\\' )
			{
				source++;
				if ( *source ) source++;
				continue;
			}

                       	if ( *source++ == delimiter )
                               	if ( ++mark == offset )
					break;
		}
	}
 
	/* If ran off the end */
	/* ------------------ */
        if ( mark != offset ) return (char *)0;
 
        while( *source )
	{
		if ( *source == '\\' )
		{
			if ( *( source + 1 ) == delimiter )
			{
				source++;
				*buf_ptr++ = *source++;
				continue;
			}
		}

		if ( *source == delimiter ) break;
		*buf_ptr++ = *source++;
	}
 
        *buf_ptr = '\0';
 
	if ( piece_do_trim ) trim( destination );
	return destination;
 
} /* piece */
 
LIST *piece_list(	LIST *source_list,
			char delimiter,
			int offset )
{
	char buffer[ 1024 ];
	LIST *return_list = list_new();

	if ( !list_rewind( source_list ) ) return return_list;

	do {
		if ( piece(	buffer,
				delimiter,
				list_get_pointer( source_list ),
				offset ) )
		{
			list_append_pointer(	return_list,
						strdup( buffer ) );
		}
		else
		{
			list_append_pointer(	return_list,
						strdup( "" ) );
		}
	} while( list_next( source_list ) );

	return return_list;

} /* piece_list() */

char *piece_double_quoted(
			char *destination,
			char delimiter,
			char *source,
			int offset )
{
	return piece_quoted(
			destination,
			delimiter,
			source,
			offset,
			'"' );
}

char *piece_quoted(	char *destination,
			char delimiter,
			char *source,
			int offset,
			char quote_character )
{
        int mark = 0;
        char *buf_ptr = destination;
	boolean inside_quote = 0;
 
	*destination = '\0';

	if ( !delimiter )
	{
		return (char *)0;
	}

	if ( delimiter == 't' ) delimiter = 9;

        /* if offset is not zero, find occurrence */
        /* --------------------------------------- */
        if ( offset )
	{
                while( *source )
		{
			if ( *source == '\\' )
			{
				source++;
				if ( *source ) source++;
				continue;
			}

			/* If opening quote */
			/* ---------------- */
			if ( *source == quote_character
			&& !inside_quote )
			{
				source++;
				inside_quote = 1;
				continue;
			}

			/* If closing quote */
			/* ---------------- */
			if ( *source == quote_character && inside_quote )
			{
				source++;
				inside_quote = 0;
				continue;
			}

			if ( inside_quote )
			{
				source++;
				continue;
			}

                       	if ( *source++ == delimiter )
			{
				if ( !inside_quote )
				{
                               		if ( ++mark == offset ) break;
				}
			}
		}
	}
 
	/* If ran off the end */
	/* ------------------ */
        if ( mark != offset ) return (char *)0;
 
	inside_quote = 0;

        while( *source )
	{
		if ( *source == '\\' )
		{
			if ( *( source + 1 ) == delimiter )
			{
				source++;
				*buf_ptr++ = *source++;
				continue;
			}
		}

		if ( *source == quote_character
		&&   !inside_quote )
		{
			inside_quote = 1;
			source++;
			continue;
		}

		if ( *source == quote_character && inside_quote )
		{
			inside_quote = 0;
			source++;
			continue;
		}

		if ( *source == delimiter && !inside_quote )
		{
			break;
		}

		*buf_ptr++ = *source++;
	}
 
        *buf_ptr = '\0';

	if ( piece_do_trim ) trim( destination );

	return destination;
 
} /* piece_quoted */
 
char *piece_multiple(	char *destination, 
			char delimiter, 
			char *source, 
			int columns_to_piece )
{
        char *buf_ptr = destination;
 
	*destination = '\0';

	if ( delimiter == 't' ) delimiter = 9;

        *buf_ptr = '\0';
 
        while( *source )
	{
		if ( *source == '\\' )
		{
			if ( *( source + 1 ) == delimiter )
			{
				columns_to_piece--;
				source++;
				*buf_ptr++ = *source++;
				continue;
			}
		}

		if ( *source == delimiter )
		{
			columns_to_piece--;
			if ( !columns_to_piece )
				break;
			else
				*buf_ptr++ = *source++;
		}
		else
			*buf_ptr++ = *source++;
	}
 
        *buf_ptr = '\0';
 
	if ( piece_do_trim ) trim( destination );
	return destination;
 
} /* piece_mulitple */
 

/* --------------------------------------------------------------------
This procedure takes the complete input string and the delimeter of the 
field and breaks it in to fields and stores in the piece_buffer[][];
----------------------------------------------------------------------- */
int piece_string_to_record( char *str, char delimiter )
{
	int i = 0;
	char buffer[ MAX_FIELD_LEN + 1 ];

 	for ( i = 0; piece( buffer, delimiter, str, i ); i++ )
	{
		if ( i == MAX_NUM_FIELDS )
		{
			fprintf(	stderr, 
					"piece_record max fields: %d\n",
				 	MAX_NUM_FIELDS );
			exit( 1 );
		}
		strcpy( piece_buffer[ i ], buffer );
	}
	num_piece_buffer = i;
	return 1;
}


int piece_record_to_string( char *str, char delimiter )
{
	int i;

	*str = '\0';

	for( i = 0; i < num_piece_buffer; i++ )
	{
		if ( i == 0 )
		{
			sprintf( str + strlen( str ), "%s", 
				 piece_buffer[ i ] );
		}
		else
		{
			sprintf( str + strlen( str ), "%c%s", 
				 delimiter, 
				 piece_buffer[ i ] );
		}
	}
	return 1;
} /* piece_record_to_string() */




/* -------------------------------------------------------------------------
This procedure takes the integer num_field as the argument and returns field 
number num_field in the global array piece_buffer[][]
---------------------------------------------------------------------------- */
char *piece_field( int field_number )
{
	if ( field_number > num_piece_buffer )
	{
		fprintf( stderr, "piece_field( %d ) is larger than: %d\n",
			 field_number,
			 num_piece_buffer );

		return (char *)0;
	}
	else
	{
		return( piece_buffer[ field_number ] );
	}
}

char *replace_piece(	char *source_destination, 
			char delimiter, 
			char *new_data, 
			int piece_offset )
{
	return piece_replace(
			source_destination, 
			delimiter, 
			new_data, 
			piece_offset );

} /* replace_piece() */


void piece_set_notrim()
{
	piece_do_trim = 0;
}

void piece_set_trim()
{
	piece_do_trim = 1;
}

char *piece_insert( 	char *source_destination, 
			char delimiter, 
			char *insert_this,
			int piece_offset )
{
	int here;
	char delimiter_string[ 2 ];
	char insert_buffer[ 1024 ];

	/* If insert in the beginning */
	/* -------------------------- */
	if ( !piece_offset )
	{
		if ( delimiter )
		{
			sprintf(	insert_buffer,
					"%s%c",
					insert_this,
					delimiter );
		}
		else
		{
			sprintf(	insert_buffer,
					"%s",
					insert_this );
		}
		return insert_string( source_destination, insert_buffer, 0 );
	}
	else
	{
		*delimiter_string = delimiter;
		*(delimiter_string + 1) = '\0';
	
		here = instr(	delimiter_string, 
				source_destination, 
				piece_offset );

		/* If insert at the end */
		/* -------------------- */
		if ( here == -1 )
			here = strlen( source_destination );

		sprintf( insert_buffer, "%c%s", delimiter, insert_this );
		return insert_string(	source_destination, 
					insert_buffer,
					here );
	}

} /* piece_insert() */

char *piece_delete( char *source_destination, char delimiter, int piece_offset )
{
	return piece_inverse( source_destination, delimiter, piece_offset );
}

char *piece_delete_multiple(
			char *source_destination,
			char delimiter,
			int columns_to_piece )
{
	if ( columns_to_piece <= 0 ) return source_destination;

	do {
		if ( !piece_inverse(
			source_destination,
			delimiter,
			0 ) )
		{
			return source_destination;
		}
	} while( --columns_to_piece );

	return source_destination;

} /* piece_delete_multiple() */

char *piece_inverse( 	char *source_destination, 
			char delimiter, 
			int piece_offset )
{
	int this_piece, piece_count;
	char *s;
	char *s_here;

	s = s_here = source_destination;

	for( piece_count = 0, this_piece = 0; *s; this_piece++ )
	{
		if ( *s == delimiter )
		{
			if ( piece_count == piece_offset )
			{
				piece_trim(	s_here, 
						s, 
						piece_offset );
				return source_destination;
			}
			else
			{
				piece_count++;
				s_here = s;
			}
		}
		s++;
	}

	piece_trim( s_here, s, piece_offset );
	return source_destination;

} /* piece_inverse() */

char *piece_swap( 	char *source_destination, 
			char delimiter, 
			int piece1_offset,
			int piece2_offset )
{
	char data1[ 2048 ], data2[ 2048 ];

	if ( !piece( data1, delimiter, source_destination, piece1_offset ) )
		fprintf( stderr, 
			 "piece_swap (%d,%d) failed with (%s)\n",
			 piece1_offset, piece2_offset, source_destination );

	if ( !piece( data2, delimiter, source_destination, piece2_offset ) )
		fprintf( stderr, 
			 "piece_swap (%d,%d) failed with (%s)\n",
			 piece1_offset, piece2_offset, source_destination );

	replace_piece(	source_destination, 
			delimiter, 
			data2, 
			piece1_offset );

	return replace_piece(	source_destination, 
				delimiter, 
				data1, 
				piece2_offset );

} /* piece_swap() */


char *piece_split_in_two( char *destination, 
			  char delimiter, 
			  char *source, 
			  int offset )
{
	/* If offset is zero, then just regular piece() */
	/* -------------------------------------------- */
	if ( offset == 0 )
	{
		piece( destination, delimiter, source, offset );
		return destination;
	}

	if ( offset > 1 )
	{
		fprintf( stderr,
			 "piece_split_in_two() has invalid offset: (%d)\n",
			 offset );
		exit( 1 );
	}
	
        *destination = '\0';
 
        while( *source )
	{
		if ( *source == delimiter )
		{
			break;
		}
		else
		{
			source++;
		}
	}
 
        if ( *source != delimiter )
	{
		fprintf( stderr,
			 "piece_split_in_two() has no delimiter: %c\n",
			 delimiter );
		exit( 1 );
	}
 
	strcpy( destination, source + 1 );

	if ( piece_do_trim )
        	return trim( destination );
	else
		return destination;
 

} /* piece_split_in_two() */


char *piece_trim( char *s_here, char *s, int piece_offset )
{
	/* Case 1: trim the beginning */
	/* -------------------------- */
	if ( piece_offset == 0 )
	{
		timlib_strcpy( s_here, s + 1, 9999 );
		return s_here;
	}
	else
	/* Case 2: trim in the middle or trim the end */
	/* ------------------------------------------ */
	{
		timlib_strcpy( s_here, s, 9999 );
		return s_here;
	}

} /* piece_trim() */

/* Sample fmt_str = "where last_activity_date = '&2'" */
/* -------------------------------------------------- */
char *piece_replace_with_fmt_str(	char *destination, 
					char *fmt_str, 
					char *pipe_delimited_record )
{
	int piece_index, here;
	char piece_buffer[ 128 ];
	char search_string[ 128 ];

	strcpy( destination, fmt_str );

	while( ( here = instr( "&", destination, 1 ) ) != -1 )
	{
		piece_index = atoi( destination + here + 1 );

		/* Trap something like this: "where field = '&non_digit'" */
		/* ------------------------------------------------------ */
		if ( piece_index == 0
		&&   ( *(destination + here + 1) ) != '0' )
		{
			fprintf( stderr,
	"piece_replace_with_fmt_str ERROR: cannot parse (%s)\n",
				 fmt_str );
			exit( 1 );
		}
	
		sprintf( search_string, "&%d", piece_index );

		if ( !piece(	piece_buffer, 
				'|', 
				pipe_delimited_record, 
				piece_index ) )
		{
			fprintf( stderr,
	"piece_replace_with_fmt_str ERROR: cannot piece(%d) record (%s)\n",
				 piece_index,
				 pipe_delimited_record );
			return (char *)0;
		}

		search_replace(	search_string, piece_buffer, destination );
	}

	return destination;

} /* piece_replace_with_fmt_str() */

void piece_bar_or_error( char *d, char *s, int piece_offset )
{
	if ( !piece( d, '|', s, piece_offset ) )
	{
		fprintf( stderr, "Cannot piece(%d) on (%s)\n",
			 piece_offset,
			 s );
		exit( 1 );
	}
} /* piece_bar_or_error() */


char *piece_last( char *destination, char delimiter, char *source )
{
	return piece(	destination,
			delimiter,
			source,
			count_characters( delimiter, source ) );
}

char *piece_replace(	char *source_destination, 
			char delimiter, 
			char *new_data, 
			int piece_offset )
{
	if ( delimiter == PIECE_QUOTE_COMMA_DELIMITER_CODE )
	{
		char source[ 1024 ];

		timlib_strcpy( source, source_destination, 1024 );
		*source_destination = '\0';

		return piece_replace_quote_comma(
				source_destination,
				source,
				new_data,
				piece_offset );
	}
	else
	if ( delimiter == PIECE_SPACES_DELIMITER_CODE
	||   delimiter == 9
	||   delimiter == 't' )
	{
		return column_replace(
				source_destination, 
				new_data, 
				piece_offset );
	}

	if ( instr_character( delimiter, source_destination ) == -1 )
	{
		*source_destination = '\0';
		delimiter = 0;
	}
	else
	{
		piece_delete( source_destination, delimiter, piece_offset );
	}

	piece_insert(	source_destination, 
			delimiter, 
			new_data, 
			piece_offset );

	return source_destination;

} /* piece_replace() */

char *piece_grep(	char *source,
			char delimiter,
			char *grep_list,
			int piece_offset,
			boolean grep_exclude )
{
	char buffer[ 1024 ];
	char *grep;
	LIST *string_list;

	string_list = string2list( grep_list, delimiter );
	if ( !list_rewind( string_list ) ) return (char *)0;
	do {
		grep = list_get_string( string_list );
		if ( !piece( buffer, delimiter, source, piece_offset ) )
			return (char *)0;

/*
		if ( !grep_exclude )
		{
			if ( strcmp( grep, buffer ) == 0 ) return source;
		}
		else
		{
			if ( strcmp( grep, buffer ) != 0 ) return source;
		}
*/
		if ( !grep_exclude )
		{
			if ( timlib_exists_string(
				buffer /* string */,
				grep /* substring */ ) )
			{
				return source;
			}
		}
		else
		{
			if ( !timlib_exists_string(
				buffer /* string */,
				grep /* substring */ ) )
			{
				return source;
			}
		}

	} while( list_next( string_list ) );

	return (char *)0;

} /* piece_grep() */

char *piece_shift_left(	char *source_destination, 
			char delimiter )
{
	char data[ 2048 ];

	piece( data, delimiter, source_destination, 0 );

	piece_insert(	source_destination,
			delimiter,
			data,
			count_characters( delimiter, source_destination ) + 1 );

	piece_delete( source_destination, delimiter, 0 );
	return source_destination;

} /* piece_shift_left() */

char *piece_shift_right(char *source_destination, 
			char delimiter )
{
	char data[ 2048 ];

	piece(	data,
		delimiter,
		source_destination,
		count_characters( delimiter, source_destination ) );

	piece_insert(	source_destination,
			delimiter,
			data,
			0 );

/*
	piece_delete(
		source_destination,
		delimiter,
		1 );
*/

	piece_delete(
		source_destination,
		delimiter,
		count_characters( delimiter, source_destination ) + 1 );
/*
	piece_delete(
		source_destination,
		delimiter,
		count_characters( delimiter, source_destination ) );
*/

/*
	{
		int str_len = strlen( source_destination );
		*(source_destination + str_len) = delimiter;
		*(source_destination + str_len + 1) = '\0';
	}
*/

	return source_destination;

} /* piece_shift_right() */

int piece_delimiter_position(
			char *source,
			char delimiter,
			int piece_offset )
{
	int position = 0;
	int this_piece = 0;

	while( *source )
	{
		if ( *source == delimiter )
		{
			if ( this_piece++ == piece_offset )
				return position;
		}
		source++;
		position++;
	}
	return -1;
}

char *piece_quote(	char *destination,
			char *source,
			int offset )
{
        int mark = 0;
        char *buf_ptr = destination;
	boolean inside_quote = 0;
	char next_character;
	char buffer[ 65536 ];
 
	if ( !destination || !source ) return (char *)0;

	*destination = '\0';

        /* if offset is not zero, find the mark */
        /* ------------------------------------ */
        if ( offset )
	{
                while( *source )
		{
			/* If escaped. */
			/* ----------- */
			if ( *source == '\\' )
			{
				source++;
				if ( *source ) source++;
				continue;
			}

			/* ------------------------ */
			/* Case not inside a quote. */
			/* ------------------------ */
			if ( !inside_quote )
			{
				/* Case got a quote. */
				/* ----------------- */
				if ( *source == PIECE_QUOTE )
				{
					source++;
					inside_quote = 1;
					continue;
				}

				/* Case got a comma. */
				/* ----------------- */
				if ( *source == PIECE_COMMA )
				{
					source++;
                               		if ( ++mark == offset ) break;
					continue;
				}

				/* Case got neither. */
				/* ----------------- */
				source++;
				continue;
			}

			/* -------------------- */
			/* Case inside a quote. */
			/* -------------------- */
			if ( inside_quote )
			{
				/* Case got a quote. */
				/* ----------------- */
				if ( *source == PIECE_QUOTE )
				{
					next_character = *(source + 1);

					if ( next_character == PIECE_COMMA
					||   next_character == '\0' )
					{
						/* Next character is comma. */
						/* ------------------------ */
						source++;
						inside_quote = 0;
						continue;
					}
					source++;
					continue;
				}

				/* Case got a comma. */
				/* ----------------- */
				if ( *source == PIECE_COMMA )
				{
					/* Keep going. */
					/* ----------- */
					source++;
					continue;
				}

				/* Case got neither. */
				/* ----------------- */
				source++;
			}
		}
	}
 
	/* If ran off the end */
	/* ------------------ */
        if ( offset > mark ) return (char *)0;
 
	/* =================== */
	/* Copy to destination */
	/* =================== */
	*buf_ptr = '\0';
	inside_quote = 0;

        while( *source )
	{
		/* If escaped. */
		/* ----------- */
		if ( *source == '\\' )
		{
			source++;
			*buf_ptr++ = *source++;
			continue;
		}

		/* ------------------------ */
		/* Case not inside a quote. */
		/* ------------------------ */
		if ( !inside_quote )
		{
			/* Case got a quote. */
			/* ----------------- */
			if ( *source == PIECE_QUOTE )
			{
				source++;
				inside_quote = 1;
				continue;
			}

			/* Case got a comma. */
			/* ----------------- */
			if ( *source == PIECE_COMMA )
			{
				/* All done */
				/* -------- */
				*buf_ptr = '\0';
				break;
			}

			/* Case got neither. */
			/* ----------------- */
			*buf_ptr++ = *source++;
		}

		/* -------------------- */
		/* Case inside a quote. */
		/* -------------------- */
		if ( inside_quote )
		{
			/* Case got a quote. */
			/* ----------------- */
			if ( *source == PIECE_QUOTE )
			{
				next_character = *(source + 1);

				if ( next_character == PIECE_COMMA
				||   next_character == '\0' )
				{
					/* All done */
					/* -------- */
					*buf_ptr = '\0';
					break;
				}
				*buf_ptr++ = *source++;
				continue;
			}

			/* Case got a comma. */
			/* ----------------- */
			if ( *source == PIECE_COMMA )
			{
				*buf_ptr++ = *source++;
				continue;
			}

			/* Case got neither. */
			/* ----------------- */
			*buf_ptr++ = *source++;
			continue;
		}
	}
 
        *buf_ptr = '\0';

	if ( piece_do_trim ) trim( destination );

	strcpy( buffer, destination );

	return timlib_remove_thousands_separator(
			destination,
			buffer );
}

char *piece_quote_comma_delimited(
			char *destination,
			char *source,
			int offset )
{
	return piece_quote_comma(
			destination,
			source,
			offset );
}

char piece_delimiter_search( char *source )
{
	PIECE_DELIMITER_COUNT *piece_delimiter_count;
	LIST *piece_delimiter_count_list;
	char search_highest;

	if ( timlib_exists_string( source, ",\"" ) )
		return PIECE_QUOTE_COMMA_DELIMITER_CODE;

	piece_delimiter_count_list = list_new();

	piece_delimiter_count = piece_delimiter_count_new( '|' );
	piece_delimiter_count->count = character_count( '|', source );
	list_append_pointer(	piece_delimiter_count_list,
				piece_delimiter_count );

	piece_delimiter_count = piece_delimiter_count_new( '^' );
	piece_delimiter_count->count = character_count( '^', source );
	list_append_pointer(	piece_delimiter_count_list,
				piece_delimiter_count );

	piece_delimiter_count = piece_delimiter_count_new( '&' );
	piece_delimiter_count->count = character_count( '&', source );
	list_append_pointer(	piece_delimiter_count_list,
				piece_delimiter_count );

	piece_delimiter_count = piece_delimiter_count_new( ',' );
	piece_delimiter_count->count = character_count( ',', source );
	list_append_pointer(	piece_delimiter_count_list,
				piece_delimiter_count );

/*
	piece_delimiter_count = piece_delimiter_count_new(
					PIECE_SPACES_DELIMITER_CODE );
	piece_delimiter_count->count = column_count( source );
	list_append_pointer(	piece_delimiter_count_list,
				piece_delimiter_count );
*/

	if ( ! ( search_highest =
		piece_delimiter_search_highest(
			piece_delimiter_count_list ) ) )
	{
		if ( column_count( source ) > 1 )
			return PIECE_SPACES_DELIMITER_CODE;
		else
			return ',';
	}

	return search_highest;
}

char piece_delimiter_search_highest( LIST *piece_delimiter_count_list )
{
	PIECE_DELIMITER_COUNT *piece_delimiter_count;
	PIECE_DELIMITER_COUNT *highest_piece_delimiter_count = {0};

	if ( !list_rewind( piece_delimiter_count_list ) ) return ',';

	do {
		piece_delimiter_count =
			list_get_pointer(
				piece_delimiter_count_list );

		if ( !highest_piece_delimiter_count )
		{
			highest_piece_delimiter_count = piece_delimiter_count;
		}
		else
		{
			if ( piece_delimiter_count->count >
			     highest_piece_delimiter_count->count )
			{
				highest_piece_delimiter_count =
					piece_delimiter_count;
			}
		}
	} while( list_next( piece_delimiter_count_list ) );

	if ( !highest_piece_delimiter_count->count )
		return (char)0;
	else
		return highest_piece_delimiter_count->delimiter;
}

char *piece_unknown(	char *destination,
			char *delimiter,
			char *source,
			int offset )
{
	if ( !*delimiter )
	{
		*delimiter = piece_delimiter_search( source );
	}

	if ( *delimiter == PIECE_QUOTE_COMMA_DELIMITER_CODE )
	{
		return piece_quoted(
				destination,
				',',
				source,
				offset,
				'"' );
	}
	else
	if ( *delimiter == PIECE_SPACES_DELIMITER_CODE
	||   *delimiter == 9
	||   *delimiter == 't' )
	{
		return column(
			destination, 
			offset, 
			source );
	}
	else
	{
		return piece(	destination,
				*delimiter,
				source,
				offset );
	}
}

char *piece_string(	char *destination,
			char *delimiter,
			char *source,
			int piece_offset )
{
	int start_offset;
	int end_offset;
	int source_strlen = strlen( source );
	int delimiter_strlen = strlen( delimiter );

	*destination = '\0';
	if ( !source ) return (char *)0;
	if ( !*source ) return "";

	if ( piece_offset == 0 )
		start_offset = 0;
	else
		start_offset = instr( delimiter, source, piece_offset );

	if ( start_offset == -1 )
	{
		*destination = '\0';
		return (char *)0;
	}

	end_offset = instr( delimiter, source, piece_offset + 1 );

	if ( end_offset == -1 ) end_offset = source_strlen;

	if ( start_offset == 0 )
	{
		timlib_strncpy(	destination,
				source,
				end_offset - start_offset );
	}
	else
	{
		timlib_strncpy(
			destination,
			source + start_offset + delimiter_strlen,
			( end_offset - start_offset ) - delimiter_strlen );
	}

	return destination;
}

char *piece_delete_quote_comma(	char *destination,
				char *source,
				int piece_offset )
{
	int start_offset;
	int end_offset;
	char *delimiter = "\",\"";

	*destination = '\0';
	if ( !source ) return (char *)0;
	if ( !*source ) return "";

	if ( piece_offset == 0 )
	{
		start_offset = 0;
	}
	else
	{
		start_offset =
			instr( delimiter, source, piece_offset ) + 2;
	}

	end_offset = instr( delimiter, source, piece_offset + 1 );

	/* If not enough delimiters */
	/* ------------------------ */
	if ( start_offset == 1 && end_offset == -1 ) return (char *)0;

	/* If deleting the only piece */
	/* -------------------------- */
	if ( start_offset == 0 && end_offset == -1 ) return "";

	/* If deleting the first piece */
	/* --------------------------- */
	if ( start_offset == 0 )
	{
		timlib_strcpy(	destination,
				source + end_offset + 2,
				9999 );
	}
	else
	{
		/* -------------------------- */
		/* If deleting a middle piece */
		/* -------------------------- */
		if ( end_offset != -1 )
		{
			timlib_strncpy(
				destination,
				source,
				start_offset );

			strcat( destination,
				source + end_offset + 2 );
		}
		else
		/* -------------------------- */
		/* If deleting the last piece */
		/* -------------------------- */
		{
			timlib_strncpy(
				destination,
				source,
				start_offset - 1 );
		}
	}

	return destination;
}

char *piece_insert_quote_comma(	char *destination,
				char *source,
				char *string,
				int piece_offset )
{
	int start_offset;
	char *delimiter = "\",\"";

	if ( !source ) return (char *)0;

	/* If empty source */
	/* --------------- */
	if ( !*source )
	{
		sprintf( destination, "\"%s\"", string );
		return destination;
	}

	/* If insert at the beginning */
	/* -------------------------- */
	if ( piece_offset == 0 )
	{
		sprintf( destination, "\"%s\",", string );
		strcat( destination, source );
		return destination;
	}

	start_offset =
		instr( delimiter, source, piece_offset );

	if ( start_offset != -1 )
	{
		/* ----------------------- */
		/* If insert in the middle */
		/* ----------------------- */
		timlib_strncpy( destination, source, start_offset );
		strcat( destination, "\",\"" );
		strcat( destination, string );
		strcat( destination, source + start_offset );
	}
	else
	{
		/* ----------------------- */
		/* If insert at the end */
		/* ----------------------- */
		sprintf( destination,
			 "%s,\"%s\"",
			 source,
			 string );
	}

	return destination;
}

char *piece_replace_quote_comma(
			char *destination, 
			char *source, 
			char *new_data, 
			int piece_offset )
{
	char tmp[ 1024 ];
	char *results;

	piece_delete_quote_comma( tmp, source, piece_offset );

	*destination = '\0';
	results = piece_insert_quote_comma(
				destination,
				tmp,
				new_data,
				piece_offset );
	return results;
}

PIECE_DELIMITER_COUNT *piece_delimiter_count_new(
				char delimiter )
{
	PIECE_DELIMITER_COUNT *piece_delimiter_count;

	piece_delimiter_count =
		(PIECE_DELIMITER_COUNT *)
			calloc( 1, sizeof( PIECE_DELIMITER_COUNT ) );
	if ( !piece_delimiter_count )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	piece_delimiter_count->delimiter = delimiter;
	return piece_delimiter_count;

}

char *piece_quote_comma(	char *destination,
				char *source,
				int offset )
{
	return piece_quote(
			destination,
			source,
			offset );
}

int piece_quote_comma_seek(
			char *quote_comma_row,
			char *label )
{
	int p;
	char destination[ 1024 ];

	for(	p = 0;
		piece_quote_comma(
			destination,
			quote_comma_row,
			p );
		p++ )
	{
		if ( string_strcmp( destination, label ) == 0 )
			return p;
	}
	return -1;
}

