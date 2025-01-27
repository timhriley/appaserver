/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/sort_delimited_numbers.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "list.h"

int number_match_function(
		char *list_number_string,
		char *input_number_string );

void sort_delimited_numbers(
		char reverse_yn );

void sort_delimited_numbers_output(
		LIST *list,
		char reverse_yn );

char delimiter;

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s delimiter reverse_yn\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	sort_delimited_numbers(
		*argv[ 2 ] /* reverse_yn */ );

	return 0;
}

void sort_delimited_numbers( char reverse_yn )
{
	FILE *input_pipe = popen( "cat", "r" );
	char delimited_number[ 128 ];
	LIST *list = list_new();

	while ( string_input( delimited_number, input_pipe, 128 ) )
	{
		list_set_order(
			list,
			strdup( delimited_number ),
			number_match_function );
	}

	pclose( input_pipe );

	sort_delimited_numbers_output(
		list,
		reverse_yn );
}

int number_match_function(
		char *list_number_string,
		char *input_number_string )
{
	char list_left_half_string[ 16 ];
	char list_right_half_string[ 16 ];
	char input_left_half_string[ 16 ];
	char input_right_half_string[ 16 ];
	int list_left_half_integer;
	int list_right_half_integer;
	int input_left_half_integer;
	int input_right_half_integer;

	/* Parse number in list */
	/* -------------------- */
	piece( list_left_half_string, delimiter, list_number_string, 0 );

	*list_right_half_string = '\0';
	piece( list_right_half_string, delimiter, list_number_string, 1 );

	list_left_half_integer = atoi( list_left_half_string );
	list_right_half_integer = atoi( list_right_half_string );

	/* Parse input number */
	/* ------------------ */
	piece( input_left_half_string, delimiter, input_number_string, 0 );

	*input_right_half_string = '\0';
	piece( input_right_half_string, delimiter, input_number_string, 1 );

	input_left_half_integer = atoi( input_left_half_string );
	input_right_half_integer = atoi( input_right_half_string );

	if ( list_left_half_integer < input_left_half_integer )
	{
		return -1;
	}

	if ( list_left_half_integer > input_left_half_integer )
	{
		return 1;
	}

	if ( list_right_half_integer < input_right_half_integer )
	{
		return -1;
	}

	if ( list_right_half_integer > input_right_half_integer )
	{
		return 1;
	}

	return 0;
}

void sort_delimited_numbers_output(
		LIST *list,
		char reverse_yn )
{
	if ( reverse_yn == 'y' )
	{
		if ( !list_tail( list ) ) return;

		do {
			printf( "%s\n", (char *)list_get( list ) );

		} while ( list_previous( list ) );
	}
	else
	{
		if ( !list_rewind( list ) ) return;

		do {
			printf( "%s\n", (char *)list_get( list ) );

		} while ( list_next( list ) );
	}
}

