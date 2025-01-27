/* $APPASERVER_HOME/utility/balance_accumulate_check.c	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "boolean.h"

#define DEFAULT_DELIMITER	'^'
boolean balance_accumulate_check( char delimiter );

int main( int argc, char **argv )
{
	char delimiter = DEFAULT_DELIMITER;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s delimiter\n",
			argv[ 0 ] );

		fprintf(stderr,
			"Input: opening^null^balance0\n"
			"       key1^accumulate1^balance1\n"
			"       key2^accumulate2^balance2\n"
			"               ...\n" );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	if ( !balance_accumulate_check( delimiter ) )
		return 1;
	else
		return 0;

} /* main() */

boolean balance_accumulate_check( char delimiter )
{
	double stated_balance = 0.0;
	double accumulate_amount;
	double calculated_balance;
	char input_buffer[ 1024 ];
	char piece_buffer[ 512 ];
	boolean first_time = 1;
	int row_number = 0;

	while( get_line( input_buffer, stdin ) )
	{
		row_number++;

		if ( !*input_buffer ) continue;
		if ( *input_buffer == '#' ) continue;

		if ( character_count( delimiter, input_buffer ) != 2 )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: not two delimiters in: %s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			continue;
		}

		piece( piece_buffer, delimiter, input_buffer, 1 );
		accumulate_amount = atof( piece_buffer );

		piece( piece_buffer, delimiter, input_buffer, 2 );
		stated_balance = atof( piece_buffer );

		if ( first_time )
		{
			calculated_balance = stated_balance;
			first_time = 0;
			continue;
		}

		if ( !timlib_dollar_virtually_same(
			stated_balance,
			calculated_balance + accumulate_amount ) )
		{
			piece( piece_buffer, delimiter, input_buffer, 0 );

			printf(
		"out_of_balance^row^%d^key^%s^should_be^%.2lf^is^%.2lf\n",
				row_number,
				piece_buffer,
				calculated_balance + accumulate_amount,
				stated_balance );
			return 0;
		}

		calculated_balance += accumulate_amount;
	}

	return 1;

} /* balance_accumulate_check() */
