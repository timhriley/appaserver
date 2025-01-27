/* library/regresion_dvr.c */
/* ----------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "piece.h"
#include "timlib.h"
#include "regression.h"

#define INPUT_FILE	"/tmp/home_electricity.csv"

int linear_regression( void );
int curvilinear_regression( void );

int main( void )
{
	/* curvilinear_regression(); */
	linear_regression();
	return 0;
}

int linear_regression( void )
{
	REGRESSION *regression;
	FILE *input_file;
	char input_line[ 128 ];
	char piece_buffer[ 16 ];
	double x;
	double y;

/*
	regression = regression_new( "25", "35" );
*/
	regression = regression_new( (char *)0, (char *)0 );

	if ( ! ( input_file = fopen( INPUT_FILE, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR: cannot open %s.\n", INPUT_FILE );
		exit( 1 );
	}

	get_line( input_line, input_file );

	while( get_line( input_line, input_file ) )
	{
/*
		piece( piece_buffer, ',', input_line, 2 );
		x = atof( piece_buffer );

		piece( piece_buffer, ',', input_line, 3 );
		y = atof( piece_buffer );
*/

		piece( piece_buffer, ',', input_line, 0 );
		x = atof( piece_buffer );

		piece( piece_buffer, ',', input_line, 1 );
		y = atof( piece_buffer );

		regression_set_output_file(	regression->output_file,
						&regression->lowest_x,
						&regression->highest_x,
						regression->lowest_outlier,
						regression->highest_outlier,
						regression->exists_outliers,
						x,
						y );
	}

	fclose( regression->output_file );
	fclose( input_file );

	if ( ! ( regression->linear_regression =
		regression_get_linear_regression(
					regression->temp_filename ) ) )
	{
		fprintf( stderr,
			 "ERROR: cannot get linear_regression.\n" );
		exit( 1 );
	}

	regression_remove_temp_file( regression->temp_filename );
/*
	printf( "a: %.3lf\nb: %.3lf\nc: %.3lf\nError: %.3lf\n",
		regression->curvilinear_regression->a,
		regression->curvilinear_regression->b,
		regression->curvilinear_regression->c,
		regression->curvilinear_regression->error );
*/

	regression->predictive_list =
		regression_get_predictive_list(
			0.0,
			regression->linear_regression->a,
			regression->linear_regression->b,
			regression->lowest_x,
			regression->highest_x );

	if( !list_rewind( regression->predictive_list ) )
	{
		fprintf( stderr,
			 "ERROR: empty predictive_list.\n" );
		exit( 1 );
	}

	do {
		regression->predictive_node =
			list_get_pointer( regression->predictive_list );

		printf( "%.3lf, %.3lf\n",
			regression->predictive_node->x,
			regression->predictive_node->y );

	} while( list_next( regression->predictive_list ) );
	regression_free( regression );
	return 1;
} /* linear_regression() */

int curvilinear_regression()
{
	REGRESSION *regression;
	FILE *input_file;
	char input_line[ 128 ];
	char piece_buffer[ 16 ];
	double x;
	double y;

/*
	regression = regression_new( "25", "35" );
*/
	regression = regression_new( (char *)0, (char *)0 );

	if ( ! ( input_file = fopen( INPUT_FILE, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR: cannot open %s.\n", INPUT_FILE );
		exit( 1 );
	}

	get_line( input_line, input_file );

	while( get_line( input_line, input_file ) )
	{
/*
		piece( piece_buffer, ',', input_line, 2 );
		x = atof( piece_buffer );

		piece( piece_buffer, ',', input_line, 3 );
		y = atof( piece_buffer );
*/

		piece( piece_buffer, ',', input_line, 0 );
		x = atof( piece_buffer );

		piece( piece_buffer, ',', input_line, 1 );
		y = atof( piece_buffer );

		regression_set_output_file(	regression->output_file,
						&regression->lowest_x,
						&regression->highest_x,
						regression->lowest_outlier,
						regression->highest_outlier,
						regression->exists_outliers,
						x,
						y );
	}

	fclose( regression->output_file );
	fclose( input_file );

	if ( ! ( regression->curvilinear_regression =
		regression_get_curvilinear_regression(
					regression->temp_filename ) ) )
	{
		fprintf( stderr,
			 "ERROR: cannot get curvilinear_regression.\n" );
		exit( 1 );
	}

	regression_remove_temp_file( regression->temp_filename );
/*
	printf( "a: %.3lf\nb: %.3lf\nc: %.3lf\nError: %.3lf\n",
		regression->curvilinear_regression->a,
		regression->curvilinear_regression->b,
		regression->curvilinear_regression->c,
		regression->curvilinear_regression->error );
*/

	regression->predictive_list =
		regression_get_predictive_list(
			regression->curvilinear_regression->a,
			regression->curvilinear_regression->b,
			regression->curvilinear_regression->c,
			regression->lowest_x,
			regression->highest_x );

	if( !list_rewind( regression->predictive_list ) )
	{
		fprintf( stderr,
			 "ERROR: empty predictive_list.\n" );
		exit( 1 );
	}

	do {
		regression->predictive_node =
			list_get_pointer( regression->predictive_list );

		printf( "%.3lf, %.3lf\n",
			regression->predictive_node->x,
			regression->predictive_node->y );

	} while( list_next( regression->predictive_list ) );
	regression_free( regression );
	return 1;
} /* curvilinear_regression() */

