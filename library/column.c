/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/column.c					   */
/* ----------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	   */
/* ----------------------------------------------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "list.h"
#include "String.h"
#include "column.h"

static char column_array[ COLUMN_MAX_FIELDS ]
			[ COLUMN_MAX_FIELD_LENGTH ];

static int column_array_count;

int column_count( char *string )
{
	return count_columns( string );
}

char *column_last(
		char *destination,
		char *source )
{
	int count;

	count = column_count( source );

	if ( !count ) return (char *)0;

	return column(	destination, 
			count - 1 /* field_offset */,
			source );
}

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
}

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
}


char *column_skip_spaces( char *string )
{
	while( *string && isspace( *string ) ) string++;
	return string;
}

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
}

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
}

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
		string_strcpy(	column_array[ i ],
				buffer,
				COLUMN_MAX_FIELD_LENGTH );
	}
	column_array_count = i;
	return 1;
}

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
}

char *column_replace(
		char *source_destination, 
		char *new_data, 
		int column_offset )
{
	column_string_to_array( source_destination );

	string_strcpy(	column_array[ column_offset ],
			new_data,
			COLUMN_MAX_FIELD_LENGTH );

	column_array_to_string( source_destination );

	return source_destination;

}

LIST *column2list( char *s )
{
	return column_list( s );
}

LIST *column_list( char *string )
{
	int c;
	char buffer[ 1024 ];
	LIST *list = list_new();

	if ( !string ) return (LIST *)0;

	for( c = 0; column( buffer, c, string ); c++ )
		list_set( list, strdup( buffer ) );

	return list;
}

