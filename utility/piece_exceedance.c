/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/piece_exceedance.c		       		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "piece.h"
#include "list.h"
#include "String.h"

typedef struct
{
	double value;
	char *input_string;
} EXCEEDANCE_VALUE;

/* Usage */
/* ----- */
LIST *exceedance_value_stdin_list(
			int value_piece,
			char delimiter );

/* Process */
/* ------- */

/* Usage */
/* ----- */
EXCEEDANCE_VALUE *exceedance_value_new(
			double value,
			char *input_string );

/* Process */
/* ------- */
EXCEEDANCE_VALUE *exceedance_value_calloc(
			void );

/* Usage */
/* ----- */
int exceedance_value_count_below(
			LIST *exceedance_value_list,
			double value );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void exceedance_value_list_output(
			LIST *value_list,
			char delimiter );

/* Process */
/* ------- */

int main( int argc, char **argv )
{
	LIST *exceedance_value_list;
	int value_piece;
	char delimiter;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s value_piece delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	value_piece = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	exceedance_value_list =
		exceedance_value_stdin_list(
			value_piece,
			delimiter );

	if ( list_length( exceedance_value_list ) )
	{
		exceedance_value_list_output(
			exceedance_value_list,
			delimiter );
	}

	return 0;
}

void exceedance_value_list_output(
			LIST *exceedance_value_list,
			char delimiter )
{
	EXCEEDANCE_VALUE *exceedance_value;
	int count_below;

	if ( !list_rewind( exceedance_value_list ) ) return;

	do {
		exceedance_value = list_get( exceedance_value_list );

		count_below =
			exceedance_value_count_below(
				exceedance_value_list,
				exceedance_value->value );

		printf( "%s%c%d%c%.3lf\n",
			exceedance_value->input_string,
			delimiter,
			count_below,
			delimiter,
			( (double)count_below /
				(double)list_length(
					exceedance_value_list ) ) * 100 );

	} while( list_next( exceedance_value_list ) );
}

LIST *exceedance_value_stdin_list(
			int value_piece,
			char delimiter )
{
	char system_string[ 128 ];
	char input[ 1024 ];
	char value_string[ 128 ];
	FILE *input_pipe;
	EXCEEDANCE_VALUE *exceedance_value;
	LIST *exceedance_value_list;

	sprintf(system_string,
		"sort -t'%c' -k%d -n -r",
		delimiter,
		value_piece + 1 );

	input_pipe = popen( system_string, "r" );

	exceedance_value_list = list_new();

	while( string_input( input, input_pipe, 1024 ) )
	{
		piece(	value_string,
			delimiter,
			input,
			value_piece );

		if ( *value_string
		&& ( strcasecmp( value_string, "null" ) != 0 ) )
		{
			exceedance_value =
				exceedance_value_new(
					atof( value_string ),
					strdup( input ) );

			list_set(
				exceedance_value_list,
				exceedance_value );
		}
	}

	pclose( input_pipe );
	return exceedance_value_list;
}

EXCEEDANCE_VALUE *exceedance_value_calloc( void )
{
	EXCEEDANCE_VALUE *exceedance_value;

	if ( ! ( exceedance_value = calloc( 1, sizeof ( EXCEEDANCE_VALUE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
	}

	return exceedance_value;
}

EXCEEDANCE_VALUE *exceedance_value_new(
			double value,
			char *input_string )
{
	EXCEEDANCE_VALUE *exceedance_value = exceedance_value_calloc();

	exceedance_value->value = value;
	exceedance_value->input_string = input_string;
	return exceedance_value;
}

int exceedance_value_count_below(
			LIST *exceedance_value_list,
			double value )
{
	int count_below = 0;
	EXCEEDANCE_VALUE *exceedance_value;

	if ( list_at_end( exceedance_value_list ) ) return 0;

	list_push( exceedance_value_list );
	list_next( exceedance_value_list );

	do {
		exceedance_value = list_get( exceedance_value_list );

		if ( value != exceedance_value->value )
			count_below++;

	} while( list_next( exceedance_value_list ) );

	list_pop( exceedance_value_list );
	return count_below;
}

