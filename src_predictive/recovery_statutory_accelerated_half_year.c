/* ------------------------------------------------------------ */
/* src_predictive/recovery_statutory_accelerated_half_year.c	*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double rate_function(	double *book,
			double cost,
			int life,
			int year,
			double declining_rate,
			double freeze_rate );

int main( int argc, char **argv )
{
	double cost;
	double book;
	int life;
	int year;
	double declining_rate;
	double freeze_rate = 0.0;
	double rate;

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s cost life\n", argv[ 0 ] );
		exit( 1 );
	}

	cost = atof( argv[ 1 ] );
	life = atoi( argv[ 2 ] );

	if ( life <= 10 )
		declining_rate = 2.0;
	else
		declining_rate = 1.5;

	for (	book = cost,
		year = 1;
		year <= life + 1;
		year++ )
	{
		if ( life == 3 && year == 3 )
		{
			freeze_rate = 0.1481;
		}
		else
		if ( life == 5 && year == 4 )
		{
			freeze_rate = 0.1152;
		}
		else
		if ( life == 7 && year == 5 )
		{
			freeze_rate = 0.08925;
		}
		else
		if ( life == 10 && year == 7 )
		{
			freeze_rate = 0.06555;
		}
		else
		if ( life == 15 && year == 7 )
		{
			freeze_rate = 0.05905;
		}
		else
		if ( life == 20 && year == 9 )
		{
			freeze_rate = 0.044615;
		}

		rate =
			rate_function(
				&book,
				cost,
				life,
				year,
				declining_rate,
				freeze_rate );

		printf( "%d|%.6lf\n", year, rate );
	}
	return 0;
}

double rate_function(	double *book,
			double cost,
			int life,
			int year,
			double declining_rate,
			double freeze_rate )
{
	double straight_line_rate;
	double rate;
	double depreciation;

	if ( year == life + 1 )
	{
		freeze_rate /= 2.0;
		depreciation = *book * freeze_rate;
		*book -= depreciation;
		return freeze_rate;
	}
	else
	if ( freeze_rate )
	{
		depreciation = *book * freeze_rate;
		*book -= depreciation;
		return freeze_rate;
	}

	straight_line_rate = 1.0 / (double)life;
	depreciation = declining_rate * straight_line_rate * *book;

	if ( year == 1 ) depreciation = depreciation * 0.5;

	rate = depreciation / cost;

	*book = *book - depreciation;

	return rate;
}

