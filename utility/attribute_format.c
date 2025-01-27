/* utility/attribute_format.c */
/* -------------------------- */
/* This formats the output to sql if selecting from the ATTRIBUTE table */
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

void output_row( char *s );
void output_all( char *s );
void output_last_2( char *s );

int main( void )
{
	char buffer[ 128 ];

	while( get_line( buffer, stdin ) )
	{
		output_row( buffer );
	}

	return 0;

} /* main() */



void output_row( char *s )
{
	static char old_account_number[ 128 ];
	char account_number[ 128 ];

	if ( !piece( account_number, '|', s, 1 ) )
	{
		fprintf(stderr,
			"format_attribute.e Invalid input format: (%s)\n", s );
		exit( 1 );
	}

	if ( strcmp( account_number, old_account_number ) != 0 )
	{
		output_all( s );
		strcpy( old_account_number, account_number );
	}
	else
		output_last_2( s );

} /* output_row() */

void output_all( char *s )
{
	char field_name[ 128 ], data[ 128 ];
	int number_pieces, i;

	number_pieces = number_occurrences_char( '|', s ) + 1;

	for( i = 0; i < number_pieces; i += 2 )
	{
		piece( field_name, '|', s, i );
		piece( data, '|', s, i + 1 );
		printf( "%s|%s\n", field_name, data );
	}

} /* output_all() */


void output_last_2( char *s )
{
	char field_name[ 128 ], data[ 128 ];
	int number_pieces, i;

	number_pieces = number_occurrences_char( '|', s ) + 1;

	for( i = number_pieces - 2; i < number_pieces; i += 2 )
	{
		piece( field_name, '|', s, i );
		piece( data, '|', s, i + 1 );
		printf( "%s|%s\n", field_name, data );
	}

} /* output_last_2() */


