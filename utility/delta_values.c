/* $APPASERVER_HOME/utility/delta_values.c		*/
/* ---------------------------------------------------- */
/* This utility outputs either the change from one value*/
/* to the previous, or it outputs the change from one   */
/* value to the first.					*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"
#include "appaserver_error.h"

#define DEFAULT_DELIMITER	'^'

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	char value_string[ 128 ];
	char replace_string[ 128 ];
	int value_piece_offset;
	boolean doing_previous;
	boolean first_time = 1;
	double value;
	double previous_value = -1.0;
	char delimiter;

	appaserver_error_stderr( argc, argv );

	if ( argc < 3 )
	{
		fprintf(stderr,
		"Usage: %s value_piece_offset first|previous [delimiter]\n",
			argv[ 0 ] );
		fprintf(stderr,
		"Note: the default delimiter is %c\n", DEFAULT_DELIMITER );
		exit( 1 );
	}

	value_piece_offset = atoi( argv[ 1 ] );

	if ( strcmp( argv[ 2 ], "previous" ) == 0 )
		doing_previous = 1;
	else
	if ( strcmp( argv[ 2 ], "first" ) == 0 )
		doing_previous = 0;
	else
	{
		fprintf(stderr,
		"ERROR in %s: see usage. first|previous not recognized.\n",
			argv[ 0 ] );
		fprintf(stderr,
"If you want the delta from the first value, then pass along 'first';\notherwise, pass along 'previous'.\n" );
		exit( 1 );
	}

	if ( argc == 4 && ( *argv[ 3 ] != 'd' ) )
	{
		delimiter = *argv[ 3 ];
	}
	else
	{
		delimiter = DEFAULT_DELIMITER;
	}

	while( get_line( input_buffer, stdin ) )
	{
		if ( !*input_buffer || *input_buffer == '#' ) continue;

		if ( !piece(	value_string, 
				delimiter, 
				input_buffer, 
				value_piece_offset ) )
		{
			fprintf(stderr,
		"%s Warning: cannot piece(%c) = %d value in (%s)\n",
				__FILE__,
				delimiter,
				value_piece_offset,
				input_buffer );
			continue;
		}

		if ( !*value_string
		||   strcasecmp( value_string, "null" ) == 0 )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		value = timlib_atof( value_string );

		if ( first_time )
		{
			previous_value = timlib_atof( value_string );
			strcpy( replace_string, "null" );
			first_time = 0;
		}
		else
		{
			sprintf(	replace_string,
					"%.3lf",
					value - previous_value );
		}

		if ( value_piece_offset )
		{
			piece_replace(
				input_buffer,
				delimiter,
				replace_string,
				value_piece_offset );

			printf( "%s\n", input_buffer );
		}
		else
		{
			printf( "%s%c%.3lf\n",
				input_buffer,
				delimiter,
				value - previous_value );
		}

		if ( doing_previous ) previous_value = value;
	}

	return 0;

} /* main() */


