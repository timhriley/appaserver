/* pad_leading_number.c */
/* -------------------- */
/*						*/
/* Sample input:   1|30|test record		*/
/* Usage:	 pad_leading_number.e 2 3	*/
/* Output:       001|030|test record		*/
/*						*/
/* This is useful for sorting.			*/
/* -------------------------------------------- */
/* -- Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"

void pad_leading_numbers( int fields_to_pad, int total_field_size );

void replace_field_with_new_value(	char *input_buffer, 
					char *new_value, 
					int field_offset );

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s fields_to_pad total_field_size\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	pad_leading_numbers( atoi( argv[ 1 ] ), atoi( argv[ 2 ] ) );

	return 0;

} /* main() */


void pad_leading_numbers( int fields_to_pad, int total_field_size )
{
	char input_buffer[ 2048 ];
	char old_value[ 2048 ];
	char new_value[ 2048 ];
	int i;

	while( get_line( input_buffer, stdin ) )
	{
		for( i = 0; i < fields_to_pad; i++ )
		{
			if ( !piece( old_value, '|', input_buffer, i ) )
			{
				break;
			}

			sprintf( new_value, 
				 "%.*d", 
				 total_field_size,
				 atoi( old_value ) );

			replace_field_with_new_value(
					input_buffer, 
					new_value, 
					i );
		}
		printf( "%s\n", input_buffer );
	}

} /* pad_leading_numbers() */


void replace_field_with_new_value(	char *input_buffer, 
					char *new_value, 
					int field_offset )
{
	replace_piece(	input_buffer, 
			'|', 
			new_value, 
			field_offset );

} /* replace_field_with_new_value() */

