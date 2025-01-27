/* parse_post.c */
/* ---------------------------------------------------- */
/*							*/
/* Input (argv[1]): 	1) attribute or data		*/
/*       (stdin): 	2) result of CGI post		*/
/*							*/
/* Output (stdout):	pipe delimited results		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 65536 ];
	char pair[ 65536 ];
	char attribute_data[ 65536 ];
	int pair_piece;
	int attribute_data_piece;
	int first_time = 1;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage; %s attribute|data\n", argv[ 0 ] );
		exit( 1 );
	}

	if ( strcmp( argv[ 1 ], "attribute" ) == 0 )
		attribute_data_piece = 0;
	else
		attribute_data_piece = 1;

	/* Expected input: "attribute=data[&attribute=data...] */
	/* --------------------------------------------------- */
	if ( get_line( buffer, stdin ) )
	{
		for (	pair_piece = 0;
			piece( pair, '&', buffer, pair_piece );
			pair_piece++ )
		{
			if ( first_time )
			{
				first_time = 0;
			}
			else
			{
				printf( "|" );
			}
			if ( piece(	attribute_data, 
					'=', 
					pair, 
					attribute_data_piece ) )
			{
				printf( "%s", attribute_data );
			}
		}
	}
	printf( "\n" );
	return 0;
} /* main() */
