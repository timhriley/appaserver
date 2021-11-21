/* $APPASERVER_HOME/utility/date_min_max.c */
/* --------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "julian.h"
#include "piece.h"
#include "timlib.h"
#include "boolean.h"

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	char date_buffer[ 128 ];
	JULIAN *input_date;
	JULIAN *minimum_date = {0};
	JULIAN *maximum_date = {0};
	int date_piece_offset = 0;
	char delimiter = {0};
	int total_count = 0;
	int populated_count = 0;
	int date_count = 0;
	boolean exists_time = 0;

	/* Usage: %s [piece_offset] [delimiter] */
	/* ------------------------------------ */
	
	if ( argc == 3 )
	{
		date_piece_offset = atoi( argv[ 1 ] );
		delimiter = *argv[ 2 ];
	}

	while( get_line( buffer, stdin ) )
	{
		if ( !delimiter )
		{
			strcpy( date_buffer, buffer );
		}
		else
		if ( !piece( 	date_buffer, 
				delimiter, 
				buffer, 
				date_piece_offset ) )
		{
			continue;
		}

		if ( delimiter != ':' && !exists_time )
		{
			exists_time = character_exists( date_buffer, ':' );
		}

		total_count++;

		if ( !*date_buffer ) continue;

		populated_count++;

		input_date = julian_yyyy_mm_dd_new( date_buffer );

		date_count++;

		if ( !minimum_date )
		{
			minimum_date = maximum_date = input_date;

		}
		else
		{
			if ( input_date->current < minimum_date->current )
			{
				minimum_date = input_date;
			}
			else
			if ( input_date->current > maximum_date->current )
			{
				maximum_date = input_date;
			}
		}
	}

	if ( !populated_count )
	{
		printf( "%s:\n", DATE_MINIMUM );
		printf( "%s:\n", DATE_MAXIMUM );
		printf( "%s: 0\n", DATE_COUNT );

		if ( total_count )
			printf( "%s:100.0\n", DATE_PERCENT_MISSING );
		else
			printf( "%s:0.0\n", DATE_PERCENT_MISSING );
	}
	else
	{
		if ( date_count )
		{
			printf( "%s:", DATE_MINIMUM );

			printf( "%s",
				julian_display_yyyy_mm_dd(
					minimum_date->current ) );

			if ( exists_time )
			{
				printf( ":%s",
					julian_display_hhmm(
					minimum_date->current ) );
			}

			printf( "\n" );

			printf( "%s:", DATE_MAXIMUM );

			printf( "%s",
				julian_display_yyyy_mm_dd(
					maximum_date->current ) );

			if ( exists_time )
			{
				printf( ":%s",
					julian_display_hhmm(
					maximum_date->current ) );
			}
			printf( "\n" );

			printf( "%s:%d\n", DATE_COUNT, populated_count );
			printf( "%s:%lf\n",
				DATE_PERCENT_MISSING,
				(double)(total_count - populated_count) /
				(double)total_count * 100.0 );
		}
		else
		{
			printf( "%s:\n", DATE_MINIMUM );
			printf( "%s:\n", DATE_MAXIMUM );
			printf( "%s:%d\n", DATE_COUNT, populated_count );
			printf( "%s:%lf\n",
				DATE_PERCENT_MISSING,
				(double)(total_count - populated_count) /
				(double)total_count * 100.0 );
		}
	}
	return 0;
} /* main() */

