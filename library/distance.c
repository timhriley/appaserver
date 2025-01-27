/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/distance.c					*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "distance.h"

unsigned int distance_pythagorean_integer(
		unsigned int point_a_x,
		unsigned int point_a_y,
		unsigned int point_b_x,
		unsigned int point_b_y )
{
	unsigned int x_difference;
	unsigned int y_difference;
	unsigned int distance_integer;

	x_difference = point_a_x - point_b_x;
	y_difference = point_a_y - point_b_y;

	distance_integer =
		(unsigned int)
			sqrt(
			( x_difference * x_difference ) + 
			( y_difference * y_difference ) );

	return distance_integer;
}

