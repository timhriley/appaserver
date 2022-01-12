/* ------------------------------------------------------	*/
/* $APPASERVER_HOME/src_alligator/sort_alligator_census.c	*/
/* ------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "list.h"

int date_match_function(
			char *list_date_string,
			char *input_date_string );

void sort_alligator_census_output(
			LIST *list );

int main( void )
{
	FILE *input_pipe = popen( "cat", "r" );
	char delimited_date[ 128 ];
	LIST *list = list_new();

	while ( string_input( delimited_date, input_pipe, 128 ) )
	{
		list_add_pointer_in_order(
			list,
			strdup( delimited_date ),
			date_match_function );
	}

	pclose( input_pipe );

	sort_alligator_census_output( list );

	return 0;
}

int date_match_function(
			char *list_date_string,
			char *input_date_string )
{
	char list_left_half_string[ 32 ];
	char list_right_half_string[ 32 ];
	char input_left_half_string[ 32 ];
	char input_right_half_string[ 32 ];
	int list_left_half_integer;
	int input_left_half_integer;

	/* Parse date in list */
	/* -------------------- */
	piece( list_left_half_string, '-', list_date_string, 0 );
	list_left_half_integer = atoi( list_left_half_string );

	strcpy( list_right_half_string, list_date_string );
	piece_inverse( list_right_half_string, '-', 0 );

	/* Parse input date */
	/* ------------------ */
	piece( input_left_half_string, '-', input_date_string, 0 );
	input_left_half_integer = atoi( input_left_half_string );

	strcpy( input_right_half_string, input_date_string );
	piece_inverse( input_right_half_string, '-', 0 );

	if ( list_left_half_integer < input_left_half_integer )
	{
		return 1;
	}

	if ( list_left_half_integer > input_left_half_integer )
	{
		return -1;
	}

	if ( strcmp( list_right_half_string, input_right_half_string ) < 0 )
	{
		return -1;
	}

	if ( strcmp( list_right_half_string, input_right_half_string ) > 0 )
	{
		return 1;
	}

	return 0;
}

void sort_alligator_census_output( LIST *list )
{
	if ( !list_rewind( list ) ) return;

	do {
		printf( "%s\n", (char *)list_get( list ) );
	} while ( list_next( list ) );
}

