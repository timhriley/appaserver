/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/float.c			   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "boolean.h"
#include "list.h"
#include "float.h"

double float_abs( double f )
{
	if ( f < 0.0 )
		return -f;
	else
		return f;
}

/* Rounds to the dollar, not the penny */
/* ----------------------------------- */
double abs_dollar( double f )
{
	return round_float( float_abs( f ) );
}

double abs_float( double f )
{
	return float_abs( f );
}

boolean float_money_virtually_same( double d1, double d2 )
{
	return float_dollar_virtually_same( d1, d2 );
}

boolean dollar_virtually_same( double d1, double d2 )
{
	return float_dollar_virtually_same( d1, d2 );
}

boolean float_dollar_virtually_same( double d1, double d2 )
{
	double difference = d1 - d2;
	boolean results;
	
	results = ( float_abs( difference ) < 0.005 );
	return results;
}

boolean double_virtually_same( double d1, double d2 )
{
	return float_virtually_same( d1, d2 );
}

boolean float_virtually_same( double d1, double d2 )
{
	double difference = d1 - d2;
	boolean results;
	
	results = ( float_abs( difference ) < 0.000005 );
	return results;
}

boolean float_virtually_same_places(
		double d1, double d2, int places )
{
	char d1_string[ 32 ];
	char d2_string[ 32 ];

	sprintf( d1_string, "%.*lf", places, d1 );
	sprintf( d2_string, "%.*lf", places, d2 );

	return ( strcmp( d1_string, d2_string )  == 0 );
}

double floor( double d )
{
	if ( d >=0 )
		return (double)(int)d;
	else
		return -ceiling( -d );
}

double ceiling( double d )
{
	if ( d >= 0 )
		return (double)(int)(d + 0.99);
	else
		return -floor( -d );
}

double round_pennies( double d )
{
	return float_round_money( d );
}

double round_money( double d )
{
	return float_round_money( d );
}

double float_round_money( double d )
{
	char string[ 32 ];

	if ( double_virtually_same( d, 0.0 ) ) return 0.0;

	sprintf( string, "%.2lf", d );
	return atof( string );
}

int round_int( double d )
{
	return float_round_integer( d );
}

int float_round_int( double d )
{
	return float_round_integer( d );
}

int float_round_integer( double d )
{
	if ( d >= 0.0 )
		return (int)(d + 0.5);
	else
		return (int)(d - 0.5);
}

double round_float( double d )
{
	return round_double( d );
}

double round_double( double d )
{
	if ( d >= 0.0 )
		return (double)(int)(d + 0.5);
	else
		return -floor( -d );
}

double floor_double( double d )
{
	if ( d >= 0.0 )
	{
		if ( d > 1.0 )
			return (double)(int)d;
		else
		if ( d > 0.5 )
			return 0.5;
		else
		if ( d > 0.2 )
			return 0.2;
		else
		if ( d > 0.1 )
			return 0.1;
		else
		if ( d > 0.05 )
			return 0.05;
		else
			return 0.0;
	}
	else
	{
		return -ceiling_double( -d );
	}
}

double ceiling_double( double d )
{
	if ( d >= 0.0 )
	{
		if ( d < 0.05 )
			return 0.05;
		else
		if ( d < 0.1 )
			return 0.1;
		else
		if ( d < 0.2 )
			return 0.2;
		else
		if ( d < 0.5 )
			return 0.5;
		else
			return (double)(int)(d + 0.99);
	}
	else
	{
		return -floor_double( -d );
	}
}

int float_percent_of_total(
			double total,
			double denominator )
{
	double percent;

	if ( !denominator ) return 0;

	percent =
		(total /
		 denominator) * 100.0;

	return float_round_int( percent );
}

int float_delta_prior_percent(
			double prior_total,
			double total )
{
	double difference;
	double delta_percent;

	if ( !prior_total ) return 0;

	difference =
		total -
		prior_total;

	delta_percent = (difference / prior_total) * 100.0;

	return float_round_int( delta_percent );
}

int float_ratio_to_percent( double ratio )
{
	return
	float_round_int( ratio * 100.0 );
}

boolean float_integer_boolean( double d )
{
	int i;

	i = (int)d;

	return
	float_virtually_same( (double)i, d );
}

boolean float_is_integer( double d )
{
	return float_integer_boolean( d );
}

char *float_string( double d, int decimal_count )
{
	static char string[ 128 ];

	sprintf(string,
		"%.*lf",
		decimal_count,
		d );

	return string;
}

