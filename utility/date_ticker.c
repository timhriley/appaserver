/* $APPASERVER_HOME/utility/date_ticker.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include "boolean.h"

/* Prototypes */
/* ---------- */
void date_ticker_second(int how_many,
			char *start_date);

void date_ticker_minute(int how_many,
			char *start_date );

void date_ticker_hour(	int how_many,
			char *start_date );

void date_ticker_day(	int how_many,
			char *start_date );

void date_ticker_week(	int how_many,
			char *start_date );

void date_ticker_month(	int how_many,
			char *start_date );

void date_ticker_year(	int how_many,
			char *start_date );

int main( int argc, char **argv )
{
	char *period;
	int how_many;
	char *start_date = {0};

	if ( argc < 3 )
	{
		fprintf(stderr,
"Usage: %s second|minute|hour|day|week|month|year how_many [start_date]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	period = argv[ 1 ];
	how_many = atoi( argv[ 2 ] );

	if ( argc == 4 ) start_date = argv[ 3 ];

	if ( strcmp( period, "second" ) == 0 )
	{
		date_ticker_second( how_many, start_date );
	}
	else
	if ( strcmp( period, "minute" ) == 0 )
	{
		date_ticker_minute( how_many, start_date );
	}
	else
	if ( strcmp( period, "hour" ) == 0 )
	{
		date_ticker_hour( how_many, start_date );
	}
	else
	if ( strcmp( period, "day" ) == 0 )
	{
		date_ticker_day( how_many, start_date );
	}
	else
	if ( strcmp( period, "week" ) == 0 )
	{
		date_ticker_week( how_many, start_date );
	}
	else
	if ( strcmp( period, "month" ) == 0 )
	{
		date_ticker_month( how_many, start_date );
	}
	else
	if ( strcmp( period, "year" ) == 0 )
	{
		date_ticker_year( how_many, start_date );
	}
	else
	{
		fprintf(	stderr,
				"Error in %s: invalid period = (%s)\n",
				argv[ 0 ],
				period );
		exit( 1 );
	}

	return 0;

}

void date_ticker_second(int how_many,
			char *start_date )
{
	DATE *d;
	int y;

	if ( start_date )
		d = date_yyyy_mm_dd_new( start_date );
	else
		d = date_today_new( date_utc_offset() );

	for( y = 0; y <= how_many; y++ )
	{
		printf( "%s\n", date_display_yyyy_mm_dd_colon_hms( d ) );

		date_increment_seconds( d, 1 );
	}

}

void date_ticker_minute(int how_many,
			char *start_date )
{
	DATE *d;
	int y;

	if ( start_date )
		d = date_yyyy_mm_dd_new( start_date );
	else
		d = date_today_new( date_utc_offset() );

	for( y = 0; y <= how_many; y++ )
	{
		printf( "%s\n", date_display_yyyy_mm_dd_colon_hms( d ) );

		date_increment_minutes( d, 1 );
	}

}

void date_ticker_hour(	int how_many,
			char *start_date )
{
	DATE *d;
	int y;

	if ( start_date )
		d = date_yyyy_mm_dd_new( start_date );
	else
		d = date_today_new( date_utc_offset() );

	for( y = 0; y <= how_many; y++ )
	{
		printf( "%s\n", date_display_yyyy_mm_dd_colon_hms( d ) );

		date_increment_hours( d, 1.0 );
	}

}

void date_ticker_day(	int how_many,
			char *start_date )
{
	DATE *d;
	int y;

	if ( start_date )
		d = date_yyyy_mm_dd_new( start_date );
	else
		d = date_today_new( date_utc_offset() );

	for( y = 0; y <= how_many; y++ )
	{
		printf( "%s\n", date_display_yyyy_mm_dd_colon_hms( d ) );

		date_increment_day( d );
	}

}

void date_ticker_week(	int how_many,
			char *start_date )
{
	DATE *d;
	int y;

	if ( start_date )
		d = date_yyyy_mm_dd_new( start_date );
	else
		d = date_today_new( date_utc_offset() );

	for( y = 0; y <= how_many; y++ )
	{
		printf( "%s\n", date_display_yyyy_mm_dd_colon_hms( d ) );

		date_increment_week( d );
	}

}

void date_ticker_month(	int how_many,
			char *start_date )
{
	DATE *d;
	int y;

	if ( start_date )
		d = date_yyyy_mm_dd_new( start_date );
	else
		d = date_today_new( date_utc_offset() );

	for( y = 0; y <= how_many; y++ )
	{
		printf( "%s\n", date_display_yyyy_mm_dd_colon_hms( d ) );

		date_increment_months( d, 1 );
	}

}

void date_ticker_year(	int how_many,
			char *start_date )
{
	DATE *d;
	int y;

	if ( start_date )
		d = date_yyyy_mm_dd_new( start_date );
	else
		d = date_today_new( date_utc_offset() );

	for( y = 0; y <= how_many; y++ )
	{
		printf( "%s\n", date_display_yyyy_mm_dd_colon_hms( d ) );

		date_increment_years( d, 1 );
	}

}

