/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/complete_arrangement.c			*/
/* -------------------------------------------------------------------- */
/* This process displays integer positions for 				*/
/* objects that need to be systematically arranged.			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"
#include "date.h"
#include "boolean.h"

#define MAX_OBJECT_SIZE		8

void complete_arrangement(	int object_count );

int get_max_count(		int object_count );
int get_min_count(		int object_count );

boolean all_digits_within_range(
				char *number_string,
				int object_count );

void output_number_string(	char *number_string );

boolean no_numbers_repeat(	char *number_string,
				int object_count );

int main( int argc, char **argv )
{
	int object_count;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s object_count\n", argv[ 0 ] );
		exit( 1 );
	}

	object_count = atoi( argv[ 1 ] );

	if ( object_count < 3 )
	{
		fprintf( stderr,
			 "Error: object count of %d is less than 3.\n",
			 object_count );
		exit( 1 );
	}

	if ( object_count > MAX_OBJECT_SIZE )
	{
		fprintf( stderr,
			 "Error: object count of %d exceeds %d.\n",
			 object_count,
			 MAX_OBJECT_SIZE );
		exit( 1 );
	}

	complete_arrangement( object_count );

	return 0;

}

void complete_arrangement( int object_count )
{
	char number_string[ 10 ];
	int max_count;
	int min_count;
	int i;

	min_count = get_min_count( object_count );
	max_count = get_max_count( object_count );

	for( i = min_count; i < max_count; i++ )
	{
		snprintf(
			number_string,
			sizeof ( number_string ),
			"%d",
			i );

		if ( all_digits_within_range(
				number_string,
				object_count )
		&&   no_numbers_repeat(
				number_string,
				object_count ) )
		{
			output_number_string( number_string );
		}
	}

}

int get_max_count( int object_count )
{
	if ( object_count == 3 )
		return 999;
	else
	if ( object_count == 4 )
		return 9999;
	else
	if ( object_count == 5 )
		return 99999;
	else
	if ( object_count == 6 )
		return 999999;
	else
	if ( object_count == 7 )
		return 9999999;
	else
	if ( object_count == MAX_OBJECT_SIZE )
		return 99999999;
	else
		return 0;
}

int get_min_count( int object_count )
{
	if ( object_count == 3 )
		return 100;
	else
	if ( object_count == 4 )
		return 1000;
	else
	if ( object_count == 5 )
		return 10000;
	else
	if ( object_count == 6 )
		return 100000;
	else
	if ( object_count == 7 )
		return 1000000;
	else
	if ( object_count == MAX_OBJECT_SIZE )
		return 10000000;
	else
		return 0;
}

boolean all_digits_within_range(
				char *number_string,
				int object_count )
{
	char digit_string[ 2 ];
	int digit_integer;

	digit_string[ 1 ] = '\0';

	while( *number_string )
	{
		*digit_string = *number_string;
		digit_integer = atoi( digit_string );

		if ( digit_integer == 0
		||   digit_integer > object_count )
		{
			return 0;
		}
		number_string++;
	}

	return 1;
}

void output_number_string( char *number_string )
{
	boolean first_time = 1;

	while( *number_string )
	{
		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			printf( " " );
		}

		printf( "%c", *number_string );
		number_string++;
	}
	printf( "\n" );
}

boolean no_numbers_repeat(	char *number_string,
				int object_count )
{
	int i;
	char number[ 16 ];
	char c;

	for ( i = 1; i <= object_count; i++ )
	{
		snprintf(
			number,
			sizeof ( number ),
			"%d",
			i );

		c = *number;

		if ( character_count( c, number_string ) != 1 )
			return 0;
	}
	return 1;
}

