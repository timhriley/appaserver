/* library/column.c */
/* ---------------- */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "column.h"
#include "timlib.h"
#include "piece.h"

/* Package variable */
/* ---------------- */
static char column_array[ COLUMN_MAX_FIELDS ]
			[ COLUMN_MAX_FIELD_LENGTH ];
static int column_array_count;

int column_count( char *string )
{
	return count_columns( string );
}

char *column_last(	char *destination,
			char *source )
{
	int count;

	count = column_count( source );

	if ( !count ) return (char *)0;

	return column(	destination, 
			count - 1 /* field_offset */,
			source );

} /* column_last() */

int count_columns( char *string )
{
	char put_here[ 1024 ];
	int this_column;

	this_column = 0;

	string = column_skip_spaces( string );

	while( *string )
	{
		this_column++;
		string = column_fill_characters( put_here, string );
		string = column_skip_spaces( string );
	}

	return this_column;

} /* count_columns() */


char *column(	char *destination, 
		int field_offset,
		char *string )
{
	int this_column = 0;

	if ( !string ) return (char *)0;

	/* Skip leading spaces */
	/* ------------------- */
	string = column_skip_spaces( string );

	while( *string )
	{
		if ( this_column == field_offset )
		{
			column_fill_characters( destination, string );
			return destination;
		}
		this_column++;
		string = column_fill_characters( destination, string );
		string = column_skip_spaces( string );
	}

	*destination = '\0';
	return (char *)0;
}


int column_get_str( char *buffer, char *string )
{
	int quote_flag = 0;

	while( *string )
	{
		if ( *string == '"' || *string == '\'' )
		{
			quote_flag = 1 - quote_flag;
			string++;
			continue;
		}

		if ( quote_flag )
		{
			*buffer++ = *string++;
			continue;
		}
		else
		{
			if ( isspace( *string ) ) break;

			*buffer++ = *string++;
		}
	}

	*buffer = '\0';
	return 1;
} /* column_get_str() */


char *column_skip_spaces( char *string )
{
	while( *string && isspace( *string ) ) string++;
	return string;

} /* column_skip_spaces() */

#ifdef NOT_DEFINED
Remove single_quote_flag.

char *column_fill_characters( char *destination, char *string )
{
	int double_quote_flag = 0;
	int single_quote_flag = 0;

	while( *string )
	{
		if ( *string == '\'' && !double_quote_flag )
		{
			single_quote_flag = 1 - single_quote_flag;
			string++;
			continue;
		}

		if ( *string == '"' && !single_quote_flag )
		{
			double_quote_flag = 1 - double_quote_flag;
			string++;
			continue;
		}

		/* Note: the order of these if's is important */
		/* ------------------------------------------ */
		if ( double_quote_flag || single_quote_flag )
		{
			*destination++ = *string++;
			continue;
		}
		else
		if ( isspace( *string ) )
		{
			break;
		}
		else
		{
			*destination++ = *string++;
		}
	}

	*destination = '\0';
	return string;

} /* column_fill_characters() */
#endif

char *column_fill_characters( char *destination, char *string )
{
	int double_quote_flag = 0;

	while( *string )
	{
		if ( *string == '"' )
		{
			double_quote_flag = 1 - double_quote_flag;
			string++;
			continue;
		}

		if ( double_quote_flag )
		{
			*destination++ = *string++;
			continue;
		}
		else
		if ( isspace( *string ) )
		{
			break;
		}
		else
		{
			*destination++ = *string++;
		}
	}

	*destination = '\0';
	return string;

} /* column_fill_characters() */

char *column2delimiter(	char *destination, char *source, char delimiter )
{
	char this_column[ 1024 ];
	int field_offset;
	int first_time = 1;
	char *hold = destination;

	*destination = '\0';
	for(	field_offset = 0;
		column(	this_column,
			field_offset, 
			source );
		field_offset++ )
	{
		if ( first_time )
			first_time = 0;
		else
			destination += sprintf( destination, "%c", delimiter );

		destination += sprintf( destination, "%s", this_column );
	}

	return hold;
} /* column2delimiter() */

LIST *column2list( char *s )
{
	char destination[ 1024 ];

	column2delimiter( destination, s, '^' );
	return list_string2list( destination, '^' );
} /* column2list() */

/* --------------------------------------------------------------------
This procedure takes the complete input string
breaks it in to fields and stores in the column_array[][];
----------------------------------------------------------------------- */
int column_string_to_array( char *str )
{
	int i = 0;
	char buffer[ COLUMN_MAX_FIELD_LENGTH ];

 	for ( i = 0; column( buffer, i, str ); i++ )
	{
		if ( i == COLUMN_MAX_FIELDS )
		{
			fprintf(	stderr, 
					"column max fields: %d\n",
				 	COLUMN_MAX_FIELDS );
			exit( 1 );
		}
		timlib_strcpy(	column_array[ i ],
				buffer,
				COLUMN_MAX_FIELD_LENGTH );
	}
	column_array_count = i;
	return 1;
} /* column_string_to_array() */

int column_array_to_string( char *str )
{
	int i;

	*str = '\0';

	for( i = 0; i < column_array_count; i++ )
	{
		if ( i == 0 )
		{
			sprintf( str + strlen( str ), "%s", 
				 column_array[ i ] );
		}
		else
		{
			sprintf( str + strlen( str ), " %s", 
				 column_array[ i ] );
		}
	}
	return 1;
} /* column_array_to_string() */

char *column_replace(	char *source_destination, 
			char *new_data, 
			int column_offset )
{
	column_string_to_array( source_destination );

	timlib_strcpy(	column_array[ column_offset ],
			new_data,
			COLUMN_MAX_FIELD_LENGTH );

	column_array_to_string( source_destination );

	return source_destination;

} /* column_replace() */

