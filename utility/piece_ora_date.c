/* piece_ora_date.c */
/* ---------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

char *to_ora_date( char *s, char *format_mask );
void piece_ora_date( int piece_offset, char *format_mask );

char *month_array[] = { 	"JAN",
				"FEB",
				"MAR",
				"APR",
				"MAY",
				"JUN",
				"JUL",
				"AUG",
				"SEP",
				"OCT",
				"NOV",
				"DEC" };

int main( int argc, char **argv )
{
	int piece_offset;
	char *format_mask;

	if ( argc != 3 )
	{
		fprintf( stderr, 
		"Usage: %s piece_offset [YY-MM-DD]\n", argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );
	format_mask = argv[ 2 ];

	piece_ora_date( piece_offset, format_mask );
	return 0;

} /* main() */



void piece_ora_date( int piece_offset, char *format_mask )
{
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	char *ora_date;

	while( get_line( input_buffer, stdin ) )
	{
		if ( ! piece( piece_buffer, '|', input_buffer, piece_offset ) )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		ora_date = to_ora_date( piece_buffer, format_mask );

		printf( "%s\n", 
			replace_piece(	input_buffer,
					'|',
					ora_date, 
					piece_offset ) );
	}

} /* piece_ora_date() */


char *to_ora_date( char *s, char *format_mask )
{
	static char ret_buffer[ 128 ];
	int mm_int;
	char yy[ 128 ], mm[ 128 ], dd[ 128 ];

	/* Currently, only one format mask */
	/* ------------------------------- */
	if ( strcmp( format_mask, "YY-MM-DD" ) != 0 ) return s;

	if ( ! piece( yy, '-', s, 0 ) ) return s;
	if ( ! piece( mm, '-', s, 1 ) ) return s;
	if ( ! piece( dd, '-', s, 2 ) ) return s;

	mm_int = atoi( mm );
	if ( mm_int < 1 || mm_int > 12 ) return s;

	sprintf( ret_buffer, 
		 "%s-%s-%s",
		 dd,
		 month_array[ mm_int - 1 ],
		 yy );

	return ret_buffer;

} /* to_ora_date() */

