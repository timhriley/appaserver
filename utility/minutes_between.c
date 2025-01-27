/* utility/minutes_between.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* Date/time format: YYYY-MM-DD:hhmmss					*/
/* -----------------------------------					*/
/* Usage: minutes_between.e start_date_time end_date_time		*/
/*    or: echo start_date_time end_date_time | minutes_between.e	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "date.h"
#include "boolean.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
int minutes_between(		boolean *results,
				char *starting_date_time_string,
				char *ending_date_time_string );

int main( int argc, char **argv )
{
	char starting_date_time_string[ 128 ];
	char ending_date_time_string[ 128 ];
	char input_buffer[ 256 ];
	boolean results = 0;

	if ( argc == 3 )
	{
		int m;

		m = minutes_between( &results, argv[ 1 ], argv[ 2 ] );

		if ( results )
			printf( "%d\n", m );
		else
			printf( "\n" );
	}
	else
	{
		while( get_line( input_buffer, stdin ) )
		{
			if ( column_count( input_buffer ) != 2 )
			{
				fprintf( stderr,
					 "Warning invalid input: %s\n",
					 input_buffer );
				printf( "\n" );
			}
			else
			{
				int m;

				column( starting_date_time_string,
					0,
					input_buffer );

				column( ending_date_time_string,
					1,
					input_buffer );

				m = minutes_between(
						&results,
						starting_date_time_string,
						ending_date_time_string );

				if ( results )
					printf( "%d\n", m );
				else
					printf( "\n" );
			}
		}
	}

	return 0;

} /* main() */

int minutes_between(	boolean *results,
			char *starting_date_time_string,
			char *ending_date_time_string )
{
	int minutes_between;
	char starting_date_string[ 128 ];
	char starting_time_string[ 128 ];
	char ending_date_string[ 128 ];
	char ending_time_string[ 128 ];

	*results = 0;

	if ( character_count( ':', starting_date_time_string ) != 1 )
		return 0;

	if ( character_count( ':', ending_date_time_string ) != 1 )
		return 0;

	piece( starting_date_string, ':', starting_date_time_string, 0 );
	piece( starting_time_string, ':', starting_date_time_string, 1 );

	piece( ending_date_string, ':', ending_date_time_string, 0 );
	piece( ending_time_string, ':', ending_date_time_string, 1 );

	minutes_between =
		date_minutes_between(
			starting_date_string,
			starting_time_string,
			ending_date_string,
	       		ending_time_string,
			1 /* add one */ );

	*results = 1;
	return minutes_between;

} /* minutes_between() */

