/* library/regression.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "regression.h"
#include "timlib.h"
#include "column.h"

REGRESSION *regression_new(	char *lowest_outlier,
				char *highest_outlier )
{
	REGRESSION *r;

	r = (REGRESSION *)calloc( 1, sizeof( REGRESSION ) );
	if ( !r )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): memory allocation error.\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}

	r->temp_filename = regression_temp_filename();
	r->output_file = fopen( r->temp_filename, "w" );
	fprintf( r->output_file, "x,y\n" );

	r->lowest_x = DOUBLE_MAXIMUM;
	r->highest_x = DOUBLE_MINIMUM;

	if ( lowest_outlier && isdigit( *lowest_outlier ) )
	{
		r->lowest_outlier = atoi( lowest_outlier );
		r->highest_outlier = atoi( highest_outlier );
		r->exists_outliers = 1;
	}

	return r;

}

REGRESSION_PREDICTIVE_NODE *regression_predictive_node_new( void )
{
	REGRESSION_PREDICTIVE_NODE *r;

	r = (REGRESSION_PREDICTIVE_NODE *)
		calloc( 1, sizeof( REGRESSION_PREDICTIVE_NODE ) );

	if ( !r )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): memory allocation error.\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}

	return r;

}

void regression_set_output_file(	FILE *output_file,
					double *lowest_x,
					double *highest_x,
					int lowest_outlier,
					int highest_outlier,
					boolean exists_outliers,
					double x,
					double y )
{

	if ( exists_outliers )
	{
		int x_int = x;

		if ( x_int < lowest_outlier
		||   x_int > highest_outlier )
		{
			return;
		}
	}

	if ( x < *lowest_x ) *lowest_x = x;
	if ( x > *highest_x ) *highest_x = x;

	fprintf( output_file, "%.5lf,%.5lf\n", x, y );
}

void regression_free( REGRESSION *r )
{
	if ( r->predictive_list ) list_free( r->predictive_list );
	if ( r->curvilinear_regression ) free( r->curvilinear_regression );
	if ( r->linear_regression ) free( r->linear_regression );
	free( r );
}

char *regression_temp_filename( void )
{
	char temp_filename[ 128 ];

	sprintf( temp_filename, "/tmp/regression_temp_%d.csv", getpid() );
	return strdup( temp_filename );
}

LINEAR_REGRESSION *regression_linear_regression_new(
					void )
{
	LINEAR_REGRESSION *r;

	r = (LINEAR_REGRESSION *)
		calloc( 1, sizeof( LINEAR_REGRESSION ) );
	if ( !r )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): memory allocation error.\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}

	return r;

}

CURVILINEAR_REGRESSION *regression_curvilinear_regression_new(
					void )
{
	CURVILINEAR_REGRESSION *r;

	r = (CURVILINEAR_REGRESSION *)
		calloc( 1, sizeof( CURVILINEAR_REGRESSION ) );
	if ( !r )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): memory allocation error.\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}

	return r;

}

LINEAR_REGRESSION *regression_get_linear_regression(
						char *temp_filename )
{
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	FILE *input_pipe;
	char label[ 128 ];
	char value[ 1024 ];
	LINEAR_REGRESSION *linear_regression;

	linear_regression =
		regression_linear_regression_new();

	sprintf( sys_string,
		 "linear_regression.sh %s x y",
		 temp_filename );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		column( label, 0, input_buffer );

		if ( !column( value, 1, input_buffer ) )
		{
			pclose( input_pipe );
			return (LINEAR_REGRESSION *)0;
		}

		if ( timlib_strcmp( label, "a" ) == 0 )
		{
			linear_regression->a = atof( value );
		}
		else
		if ( timlib_strcmp( label, "b" ) == 0 )
		{
			linear_regression->b = atof( value );
		}
		else
		if ( timlib_strcmp( label, "error" ) == 0 )
		{
			linear_regression->error = atof( value );
		}
		else
		if ( timlib_strcmp( label, "beta" ) == 0 )
		{
			linear_regression->beta = atof( value );
		}
		else
		if ( timlib_strcmp( label, "y_intercept" ) == 0 )
		{
			linear_regression->y_intercept = atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			LINEAR_REGRESSION_COEFFICIENT_OF_DETERMINATION )
				== 0 )
		{
			linear_regression->
				coefficient_of_determination =
					atof( value );
		}
		else
		if ( timlib_strcmp(
				label,
				LINEAR_REGRESSION_BETA ) == 0 )
		{
			linear_regression->beta = atof( value );
		}
		else
		if ( timlib_strcmp(
				label,
				LINEAR_REGRESSION_Y_INTERCEPT ) == 0 )
		{
			linear_regression->y_intercept = atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			LINEAR_REGRESSION_BETA_T_VALUE ) == 0 )
		{
			linear_regression->beta_t_value =
				atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			LINEAR_REGRESSION_BETA_TEST_STATISTIC )
				== 0 )
		{
			linear_regression->beta_test_statistic =
				atof( value );
		}
		else
		if ( timlib_strcmp(
		      label,
		      LINEAR_REGRESSION_BETA_STANDARD_DEVIATION )
				== 0 )
		{
			linear_regression->
				beta_standard_deviation =
					atof( value );
		}
		else
		if ( timlib_strcmp(	label,
					"r_squared" ) == 0 )
		{
			linear_regression->r_squared = atof( value );
		}
		else
		if ( timlib_strcmp(	label,
					"residual_standard_deviation" ) == 0 )
		{
			linear_regression->
				residual_standard_deviation =
					atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			LINEAR_REGRESSION_BETA_IS_SIGNIFICANT )
				== 0 )
		{
			linear_regression->
				beta_is_significant =
					atoi( value );
		}
		else
		if ( timlib_strcmp(
			label,
			LINEAR_REGRESSION_MINIMUM_BETA )
				== 0 )
		{
			linear_regression->
				minimum_beta =
					atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			LINEAR_REGRESSION_MAXIMUM_BETA )
				== 0 )
		{
			linear_regression->
				maximum_beta =
					atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			LINEAR_REGRESSION_Y_INTERCEPT_MINIMUM )
				== 0 )
		{
			linear_regression->
				y_intercept_minimum =
					atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			LINEAR_REGRESSION_Y_INTERCEPT_MAXIMUM )
				== 0 )
		{
			linear_regression->
				y_intercept_maximum =
					atof( value );
		}
	}
	pclose( input_pipe );
	return linear_regression;

}

CURVILINEAR_REGRESSION *regression_get_curvilinear_regression(
						char *temp_filename )
{
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	FILE *input_pipe;
	char label[ 128 ];
	char value[ 1024 ];
	CURVILINEAR_REGRESSION *curvilinear_regression;

	curvilinear_regression =
		regression_curvilinear_regression_new();

	sprintf( sys_string,
		 "curvilinear_regression.sh %s x y",
		 temp_filename );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		column( label, 0, input_buffer );

		if ( !column( value, 1, input_buffer ) )
		{
			pclose( input_pipe );
			return (CURVILINEAR_REGRESSION *)0;
		}

		if ( timlib_strcmp( label, "a" ) == 0 )
		{
			curvilinear_regression->a = atof( value );
		}
		else
		if ( timlib_strcmp( label, "b" ) == 0 )
		{
			curvilinear_regression->b = atof( value );
		}
		else
		if ( timlib_strcmp( label, "c" ) == 0 )
		{
			curvilinear_regression->c = atof( value );
		}
		else
		if ( timlib_strcmp( label, "error" ) == 0 )
		{
			curvilinear_regression->error = atof( value );
		}
		else
		if ( timlib_strcmp( label, "beta" ) == 0 )
		{
			curvilinear_regression->beta = atof( value );
		}
		else
		if ( timlib_strcmp( label, "y_intercept" ) == 0 )
		{
			curvilinear_regression->y_intercept = atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			CURVILINEAR_REGRESSION_COEFFICIENT_OF_DETERMINATION )
				== 0 )
		{
			curvilinear_regression->
				coefficient_of_determination =
					atof( value );
		}
		else
		if ( timlib_strcmp(
				label,
				CURVILINEAR_REGRESSION_QUADRATIC_BETA ) == 0 )
		{
			curvilinear_regression->quadratic_beta = atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			CURVILINEAR_REGRESSION_QUADRATIC_BETA_T_VALUE ) == 0 )
		{
			curvilinear_regression->quadratic_beta_t_value =
				atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			CURVILINEAR_REGRESSION_QUADRATIC_BETA_TEST_STATISTIC )
				== 0 )
		{
			curvilinear_regression->quadratic_beta_test_statistic =
				atof( value );
		}
		else
		if ( timlib_strcmp(
		      label,
		      CURVILINEAR_REGRESSION_QUADRATIC_BETA_STANDARD_DEVIATION )
				== 0 )
		{
			curvilinear_regression->
				quadratic_beta_standard_deviation =
					atof( value );
		}
		else
		if ( timlib_strcmp(	label,
					"r_squared" ) == 0 )
		{
			curvilinear_regression->r_squared = atof( value );
		}
		else
		if ( timlib_strcmp(	label,
					"residual_standard_deviation" ) == 0 )
		{
			curvilinear_regression->
				residual_standard_deviation =
					atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			CURVILINEAR_REGRESSION_QUADRATIC_BETA_IS_SIGNIFICANT )
				== 0 )
		{
			curvilinear_regression->
				quadratic_beta_is_significant =
					atoi( value );
		}
		else
		if ( timlib_strcmp(
			label,
			CURVILINEAR_REGRESSION_MINIMUM_QUADRATIC_BETA )
				== 0 )
		{
			curvilinear_regression->
				minimum_quadratic_beta =
					atof( value );
		}
		else
		if ( timlib_strcmp(
			label,
			CURVILINEAR_REGRESSION_MAXIMUM_QUADRATIC_BETA )
				== 0 )
		{
			curvilinear_regression->
				maximum_quadratic_beta =
					atof( value );
		}
	}
	pclose( input_pipe );
	return curvilinear_regression;

}

LIST *regression_get_predictive_list(
			double a,
			double b,
			double c,
			double lowest_x,
			double highest_x )
{
	double x;
	REGRESSION_PREDICTIVE_NODE *predictive_node;
	LIST *predictive_list = list_new();

	for( x = lowest_x; x <= highest_x; x += 0.5 )
	{
		predictive_node = regression_predictive_node_new();

		predictive_node->x = x;

		predictive_node->y =
			( (x * x) * a ) + ( x * b ) + c;

		list_append_pointer( predictive_list, predictive_node );
	}

	return predictive_list;
}

void regression_remove_temp_file( char *temp_filename )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, "rm -f %s", temp_filename );
	if ( system( sys_string ) ){};

}

void regression_output_statistics_table(
				char *statistics_filename )
{
	FILE *input_file;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *output_pipe;
	char regression_label[ 128 ];
	char regression_value[ 1024 ];

	/* Output Curvilinear statistics */
	/* ----------------------------- */
	if ( ! ( input_file = fopen( statistics_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 statistics_filename );
		return;
	}

	printf( "\n<hr>\n<table><tr><td valign=top>\n" );
	fflush( stdout );

	sprintf( sys_string,
		 "html_table.e '^^Curvilinear Statistics' '' '|' left,right" );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_file ) )
	{
		column( regression_label, 0, input_buffer );
		column( regression_value, 1, input_buffer );

		if ( timlib_strcmp(
			regression_label,
			CURVILINEAR_REGRESSION_COEFFICIENT_OF_DETERMINATION )
				== 0
		||   timlib_strcmp(
			regression_label,
			CURVILINEAR_REGRESSION_QUADRATIC_BETA )
				== 0
		||   timlib_strcmp(
			regression_label,
			CURVILINEAR_REGRESSION_QUADRATIC_BETA_T_VALUE )
				== 0
		||   timlib_strcmp(
			regression_label,
			CURVILINEAR_REGRESSION_QUADRATIC_BETA_TEST_STATISTIC )
				== 0
		||   timlib_strcmp(
			regression_label,
			CURVILINEAR_REGRESSION_MINIMUM_QUADRATIC_BETA )
				== 0
		||   timlib_strcmp(
			regression_label,
			CURVILINEAR_REGRESSION_MAXIMUM_QUADRATIC_BETA )
				== 0
		||   timlib_strcmp(
			regression_label,
			CURVILINEAR_REGRESSION_QUADRATIC_BETA_IS_SIGNIFICANT )
				== 0
		||   timlib_strcmp(
		      regression_label,
		      CURVILINEAR_REGRESSION_QUADRATIC_BETA_STANDARD_DEVIATION )
				== 0 )
		{
			fprintf( output_pipe, "%s|%s\n",
				format_initial_capital(
					 regression_label,
					 regression_label ),
				regression_value );
		}
	}

	fclose( input_file );
	pclose( output_pipe );

	/* Output Linear statistics */
	/* ------------------------ */
	if ( ! ( input_file = fopen( statistics_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 statistics_filename );
		return;
	}

	printf( "\n<td valign=top>\n" );
	fflush( stdout );

	sprintf( sys_string,
		 "html_table.e '^^Linear Statistics' '' '|' left,right" );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_file ) )
	{
		column( regression_label, 0, input_buffer );
		column( regression_value, 1, input_buffer );

		if ( timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_COEFFICIENT_OF_DETERMINATION )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_BETA )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_Y_INTERCEPT )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_BETA_T_VALUE )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_BETA_TEST_STATISTIC )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_MINIMUM_BETA )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_MAXIMUM_BETA )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_BETA_IS_SIGNIFICANT )
				== 0
		||   timlib_strcmp(
		      regression_label,
		      LINEAR_REGRESSION_BETA_STANDARD_DEVIATION )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_Y_INTERCEPT_MINIMUM )
				== 0
		||   timlib_strcmp(
			regression_label,
			LINEAR_REGRESSION_Y_INTERCEPT_MAXIMUM )
				== 0 )
		{
			fprintf( output_pipe, "%s|%s\n",
				format_initial_capital(
					 regression_label,
					 regression_label ),
				regression_value );
		}
	}

	printf( "</table>\n" );

	fclose( input_file );
	pclose( output_pipe );

}

