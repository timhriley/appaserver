/* utility/filename_date_older.c			*/
/* ---------------------------------------------------- */
/* This process lets through only those filenames that  */
/* contain a date in them older than $days. It is a	*/
/* variation of "find -mtime $days"			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"
#include "date.h"
#include "boolean.h"

char *get_date_string(		char *buffer );

boolean date_days_older_than(	char *date_string,
				int days_older_than );

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	int days_older_than;
	char *date_string;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s days_older_than\n", argv[ 0 ] );
		exit( 1 );
	}

	days_older_than = atoi( argv[ 1 ] );

	while( get_line( buffer, stdin ) )
	{
		if ( ( date_string = get_date_string( buffer ) ) )
		{
			if ( date_days_older_than(
					date_string,
					days_older_than ) )
			{
				printf( "%s\n", buffer );
			}
		}
	}
	return 0;

} /* main() */

char *get_date_string( char *buffer )
{
	boolean got_year = 0;
	boolean got_month = 0;
	static char date_string[ 11 ];
	int day = 0;
	int year = 0;
	int month = 0;

	while( *buffer )
	{
		if ( isdigit( *buffer ) )
		{
			if ( !got_year )
			{
				char year_string[ 5 ];
	
				timlib_strncpy( year_string, buffer, 4 );
				year = atoi( year_string );
	
				if ( year >= 1900 && year <= 2199 )
				{
					got_year = 1;
	
					buffer += 4;
					if ( *buffer == '-' )
					{
						buffer++;
					}
					continue;
				}
			}
			else
			if ( got_year && !got_month )
			{
				char month_string[ 3 ];
	
				timlib_strncpy( month_string, buffer, 2 );
	
				month = atoi( month_string );
				if ( month >= 1 && month <= 12 )
				{
					got_month = 1;
	
					buffer += 2;
					if ( *buffer == '-' )
					{
						buffer++;
					}
					continue;
				}
			}
			else
			if ( got_year && got_month )
			{
				char day_string[ 3 ];
	
				timlib_strncpy( day_string, buffer, 2 );
	
				day = atoi( day_string );
				if ( day >= 1 && day <= 31 )
				{
					sprintf(	date_string,
							"%d-%.2d-%.2d",
							year,
							month,
							day );
	
					return date_string;
				}
				else
				{
					got_year = 0;
					got_month = 0;
				}
			}
		}
		else
		{
			got_year = 0;
			got_month = 0;
		}
		buffer++;
	}

	return (char *)0;

} /* get_date_string() */

boolean date_days_older_than(	char *date_string,
				int days_older_than )
{
	DATE *now = date_now_new( date_utc_offset() );
	DATE *earlier;
	int difference;

	earlier = date_yyyy_mm_dd_new( date_string );

	difference = date_subtract_days( now, earlier );

	date_free( now );
	date_free( earlier );

	return ( difference >= days_older_than );

} /* date_days_older_than() */

