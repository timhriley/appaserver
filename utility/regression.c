/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/regression.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timlib.h"
#include "column.h"
#include "list.h"

typedef struct
{
	double x;
	double y;
} REGRESSION_NODE;

void calculate_sum(
				double *sum_x,
				double *sum_y,
				LIST *regression_node_list );
void calculate_sum_product_squared(
				double *sum_x_squares,
				double *sum_y_squares,
				LIST *regression_node_list );
double calculate_determination(
				LIST *regression_node_list );
REGRESSION_NODE *new_regression_node( double x, double y );
void calculate_averages(	double *average1,
				double *average2,
				LIST *regression_node_list );
double calculate_slope(		double average1,
				double average2,
				LIST *regression_node_list );
double calculate_sum_squared_errors(
				double slope,
				double y_intercept,
				LIST *regression_node_list );

int main( void )
{
	char buffer[ 128 ];
	char x_buffer[ 64 ];
	char y_buffer[ 64 ];
	double average1;
	double average2;
	double slope;
	double sum_squared_errors;
	double root_mean_squared_error;
	double y_intercept;
	double determination;
	LIST *regression_node_list;
	REGRESSION_NODE *node;

	regression_node_list = list_new_list();

	while( get_line( buffer, stdin ) )
	{
		column( x_buffer, 0, buffer );
		if ( column( y_buffer, 1, buffer ) )
		{
			node = new_regression_node(
					atof( x_buffer ),
					atof( y_buffer ) );
			list_append_pointer( regression_node_list, node );
		}
	}

	if ( !list_length( regression_node_list ) ) exit( 0 );

	calculate_averages( &average1, &average2, regression_node_list );
	slope = calculate_slope( average1,
				 average2,
				 regression_node_list );

	y_intercept = average2 - (slope * average1);

	sum_squared_errors = calculate_sum_squared_errors(
				slope,
				y_intercept,
				regression_node_list );

	if ( list_length( regression_node_list ) <= 2 )
	{
		root_mean_squared_error = 0.0;
	}
	else
	{
		double degrees_of_freedom = 
			(double)list_length( regression_node_list ) - 2.0;

		root_mean_squared_error =
			sqrt( sum_squared_errors / degrees_of_freedom );
	}

	determination = calculate_determination(
				regression_node_list );

	printf( "%lf %lf %lf %lf\n",
		slope,
		y_intercept,
		root_mean_squared_error,
		determination );

	return 0;
} /* main() */

REGRESSION_NODE *new_regression_node( double x, double y )
{
	REGRESSION_NODE *node;

	node = (REGRESSION_NODE *)calloc( 1, sizeof( REGRESSION_NODE ) );
	if ( !node )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot allocate memory\n",
			__FILE__,
			__FUNCTION__ );
		exit( 1 );
	}
	node->x = x;
	node->y = y;
	return node;
} /* new_regression_node() */

void calculate_averages(	double *average1,
				double *average2,
				LIST *regression_node_list )
{
	double sum1 = 0.0;
	double sum2 = 0.0;
	REGRESSION_NODE *node;

	if ( !list_rewind( regression_node_list ) ) return;

	do {
		node = list_get_pointer( regression_node_list );
		sum1 += node->x;
		sum2 += node->y;

	} while( list_next( regression_node_list ) );

	*average1 = sum1 / (double)list_length( regression_node_list );
	*average2 = sum2 / (double)list_length( regression_node_list );
}

double calculate_slope(
		double average1,
		double average2,
		LIST *regression_node_list )
{
	double slope_numerator = 0.0;
	double slope_denominator = 0.0;
	REGRESSION_NODE *node;

	if ( !list_rewind( regression_node_list ) ) return 0.0;

	do {
		node = list_get_pointer( regression_node_list );
		slope_numerator +=
			( (node->x - average1) *
			  (node->y - average2) );

		slope_denominator +=
			( (node->x - average1) *
			  (node->x - average1) );

	} while( list_next( regression_node_list ) );

	if ( !slope_denominator )
		return 0.0;
	else
		return slope_numerator / slope_denominator;
}

double calculate_sum_squared_errors(
			double slope,
			double y_intercept,
			LIST *regression_node_list )
{
	double predicted_y;
	double difference_squared;
	double sum_squared_errors = 0.0;
	REGRESSION_NODE *node;

	if ( !list_rewind( regression_node_list ) ) return 0.0;

	do {
		node = list_get_pointer( regression_node_list );
		predicted_y = node->x * slope + y_intercept;
		difference_squared = pow( predicted_y - node->y, 2.0 );
		sum_squared_errors += difference_squared;
	} while( list_next( regression_node_list ) );
	return sum_squared_errors;
}

double calculate_sum_products(
		LIST *regression_node_list )
{
	double sum_products = 0.0;
	REGRESSION_NODE *node;

	if ( !list_rewind( regression_node_list ) ) return 0.0;

	do {
		node = list_get_pointer( regression_node_list );
		sum_products += ( node->x * node->y );
	} while( list_next( regression_node_list ) );
	return sum_products;
}

double calculate_product_sums(
			LIST *regression_node_list )
{
	double sum_x;
	double sum_y;

	calculate_sum(
			&sum_x,
			&sum_y,
			regression_node_list );
	return sum_x * sum_y;
}

void calculate_sum(
		double *sum_x,
		double *sum_y,
		LIST *regression_node_list )
{
	REGRESSION_NODE *node;

	*sum_x = 0.0;
	*sum_y = 0.0;

	if ( list_rewind( regression_node_list ) )
	do {
		node = list_get_pointer( regression_node_list );
		*sum_x += node->x;
		*sum_y += node->y;

	} while( list_next( regression_node_list ) );
}

void calculate_sum_product_squared(
		double *sum_x_squares,
		double *sum_y_squares,
		LIST *regression_node_list )
{
	REGRESSION_NODE *node;

	*sum_x_squares = 0.0;
	*sum_y_squares = 0.0;

	if ( list_rewind( regression_node_list ) )
	do {
		node = list_get_pointer( regression_node_list );
		*sum_x_squares += pow( node->x, 2.0 );
		*sum_y_squares += pow( node->y, 2.0 );

	} while( list_next( regression_node_list ) );
}

double calculate_determination(
		LIST *regression_node_list )
{
	double sum_x;
	double sum_y;
	double sum_x_squares;
	double sum_y_squares;
	double product_sums;
	double sum_products;
	double numerator;
	double denominator_squared;
	int length;

	if ( ! ( length = list_length( regression_node_list ) ) )
		return 0.0;

	calculate_sum(
		&sum_x,
		&sum_y,
		regression_node_list );

	calculate_sum_product_squared(
			&sum_x_squares,
			&sum_y_squares,
			regression_node_list );

	product_sums =
		calculate_product_sums(
			regression_node_list );

	sum_products =
		calculate_sum_products(
			regression_node_list );

	numerator = ((double)length * sum_products) - product_sums;

	denominator_squared =
		(((double)length * sum_x_squares) - pow( sum_x, 2.0 ) ) *
		(((double)length * sum_y_squares) - pow( sum_y, 2.0 ) );

	return pow( (numerator / sqrt( denominator_squared ) ), 2.0 );
} /* calculate_determination() */

