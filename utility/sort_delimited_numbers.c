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

int sort_delimited_numbers_integer_compare(
		char *list_string,
		char *input_string );

void sort_delimited_numbers_output(
		LIST *list,
		char reverse_yn );

/* Needs to be global b/c number_match_function() can only have 2 parameters. */
/* -------------------------------------------------------------------------- */
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
	int p;
	char list_string[ 128 ];
	char input_string[ 128 ];
	int integer_compare;

	for (	p = 0;
		piece( list_string, delimiter, list_number_string, p );
		p++ )
	{
		if ( !piece( input_string, delimiter, input_number_string, p ) )
		{
			return -1;
		}

		integer_compare =
			sort_delimited_numbers_integer_compare(
				list_string,
				input_string );

		if ( integer_compare != 0 ) return integer_compare;
	}

	return 0;
}

int sort_delimited_numbers_integer_compare(
		char *list_string,
		char *input_string )
{
	int list;
	int input;

	list = string_atoi( list_string );
	input = string_atoi( input_string );

	if ( list < input )
		return -1;
	else
	if ( list == input )
		return 0;
	else
		return 1;
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

