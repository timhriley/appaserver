/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/utility/grace_scatterplot.c       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "application.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "grace.h"
#include "session.h"
#include "list.h"
#include "column.h"
#include "regression.h"

void output_grace_scatterplot(
				char *application_name,
				char *session,
				char *title,
				char *subtitle,
				char *xaxis_label,
				char *yaxis_label );

void output_regression_statistics(
				char *statistics_output_filename,
				CURVILINEAR_REGRESSION *curvilinear_regression,
				LINEAR_REGRESSION *linear_regression );

char *statistics_output_filename_get(
				char *appaserver_data_directory,
				char *session );

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *title;
	char *subtitle;
	char *xaxis_label;
	char *yaxis_label;

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s application session title subtitle xaxis_label yaxis_label\n",
			argv[ 0 ] );

		fprintf( stderr,
"Note: the y axis is the dependent variable. x and z are independent.\n" );

		exit( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	title = argv[ 3 ];
	subtitle = argv[ 4 ];
	xaxis_label = argv[ 5 ];
	yaxis_label = argv[ 6 ];

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	output_grace_scatterplot(
			application_name,
			session_key,
			title,
			subtitle,
			xaxis_label,
			yaxis_label );

	return 0;
}

char *statistics_output_filename_get(
				char *appaserver_data_directory,
				char *session )
{
	char filename[ 128 ];

	sprintf( filename,
		 "%s/grace_scatterplot_statistics_%s.dat",
		 appaserver_data_directory,
		 session );

	return strdup( filename );
}

void output_regression_statistics(
		char *statistics_output_filename,
		CURVILINEAR_REGRESSION *curvilinear_regression,
		LINEAR_REGRESSION *linear_regression )
{
	FILE *statistics_file;

	if ( ! ( statistics_file =
			fopen( statistics_output_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 statistics_output_filename );
		exit( 1 );
	}

	fprintf( statistics_file, "%s %.5lf\n",
		CURVILINEAR_REGRESSION_COEFFICIENT_OF_DETERMINATION,
		curvilinear_regression->
			coefficient_of_determination );

	fprintf( statistics_file, "%s %.5lf\n",
		CURVILINEAR_REGRESSION_QUADRATIC_BETA,
		curvilinear_regression->
			quadratic_beta );

	fprintf( statistics_file, "%s %.5lf\n",
		CURVILINEAR_REGRESSION_QUADRATIC_BETA_STANDARD_DEVIATION,
		curvilinear_regression->
			quadratic_beta_standard_deviation );

	fprintf( statistics_file, "%s %.5lf\n",
		CURVILINEAR_REGRESSION_QUADRATIC_BETA_T_VALUE,
		curvilinear_regression->
			quadratic_beta_t_value );

	fprintf( statistics_file, "%s %.5lf\n",
		CURVILINEAR_REGRESSION_QUADRATIC_BETA_TEST_STATISTIC,
		curvilinear_regression->
			quadratic_beta_test_statistic );

	fprintf( statistics_file, "%s %s\n",
		CURVILINEAR_REGRESSION_QUADRATIC_BETA_IS_SIGNIFICANT,
		( curvilinear_regression->
			quadratic_beta_is_significant )
			? "true" : "false" );

	fprintf( statistics_file, "%s %.5lf\n",
		CURVILINEAR_REGRESSION_MINIMUM_QUADRATIC_BETA,
		curvilinear_regression->
			minimum_quadratic_beta );

	fprintf( statistics_file, "%s %.5lf\n",
		CURVILINEAR_REGRESSION_MAXIMUM_QUADRATIC_BETA,
		curvilinear_regression->
			maximum_quadratic_beta );

	/* Output linear statistics */
	/* ------------------------ */
	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_COEFFICIENT_OF_DETERMINATION,
		linear_regression->
			coefficient_of_determination );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_BETA,
		linear_regression->
			beta );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_Y_INTERCEPT,
		linear_regression->
			y_intercept );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_BETA_STANDARD_DEVIATION,
		linear_regression->
			beta_standard_deviation );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_BETA_T_VALUE,
		linear_regression->
			beta_t_value );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_BETA_TEST_STATISTIC,
		linear_regression->
			beta_test_statistic );

	fprintf( statistics_file, "%s %s\n",
		LINEAR_REGRESSION_BETA_IS_SIGNIFICANT,
		( linear_regression->
			beta_is_significant )
			? "true" : "false" );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_MINIMUM_BETA,
		linear_regression->
			minimum_beta );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_MAXIMUM_BETA,
		linear_regression->
			maximum_beta );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_Y_INTERCEPT_MINIMUM,
		linear_regression->
			y_intercept_minimum );

	fprintf( statistics_file, "%s %.5lf\n",
		LINEAR_REGRESSION_Y_INTERCEPT_MAXIMUM,
		linear_regression->
			y_intercept_maximum );

	fclose( statistics_file );
}

void output_grace_scatterplot(
			char *application_name,
			char *session_key,
			char *title,
			char *subtitle,
			char *xaxis_label,
			char *yaxis_label )
{
	GRACE *grace = {0};
	APPASERVER_PARAMETER *appaserver_parameter;
	char graph_identifier[ 128 ];
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	char input_buffer[ 128 ];
	char x_value[ 32 ];
	double x_value_double;
	char y_value[ 32 ];
	double y_value_double;
	GRACE_GRAPH *first_grace_graph;
	char sys_string[ 128 ];
	char regression_filename[ 128 ];
	char regression_buffer[ 128 ];
	FILE *regression_pipe;
	FILE *regression_file;
	double slope = 0.0;
	double y_intercept = 0.0;
	double root_mean_squared_error = 0.0;
	char new_title[ 256 ];
	char new_sub_title[ 256 ];
	double highest_x = DOUBLE_MINIMUM;
	double highest_y;
	double lowest_x = DOUBLE_MAXIMUM;
	double lowest_y;
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	GRACE_POINT *point;
	char y_string[ 16 ];
	REGRESSION *regression;
	char *statistics_output_filename;
	boolean grace_results;

	appaserver_parameter = appaserver_parameter_new();

	sprintf( new_title, "%s %s", title, subtitle );
	*subtitle = '\0';

	grace = grace_new_xy_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				new_title,
				subtitle,
				yaxis_label,
				"" /* legend */ );

	strcpy( graph_identifier, session );

	grace->grace_output =
		application_grace_output( application_name );

	grace_get_filenames(
			&grace->agr_filename,
			&grace->ftp_agr_filename,
			&grace->postscript_filename,
			&grace->output_filename,
			&grace->ftp_output_filename,
			application_name,
			appaserver_parameter->
				document_root,
			graph_identifier,
			grace->grace_output );

	/* Open pipe to regression.e */
	/* ------------------------- */
	sprintf( regression_filename, "/tmp/regression_%d.dat", getpid() );
	sprintf( sys_string, "regression.e > %s", regression_filename );
	regression_pipe = popen( sys_string, "w" );

	regression = regression_new( (char *)0, (char *)0 );

	while( get_line( input_buffer, stdin ) )
	{
		if ( !*input_buffer ) continue;

		column(	x_value, 0, input_buffer );

		if ( !column( y_value, 1, input_buffer ) )
		{
			continue;
		}

		x_value_double = atof( x_value );
		y_value_double = atof( y_value );

		grace_set_xy_to_point_list(
				grace->graph_list, 
				x_value_double,
				strdup( y_value ),
				(char *)0 /* optional_label */,
				grace->dataset_no_cycle_color );

		/* Send to regression.e */
		/* -------------------- */
		fprintf(regression_pipe,
			"%s %s\n",
			x_value,
			y_value );

		if ( x_value_double > highest_x ) highest_x = x_value_double;
		if ( x_value_double < lowest_x ) lowest_x = x_value_double;

		/* Set to regression ADT */
		/* --------------------- */
		regression_set_output_file(	regression->output_file,
						&regression->lowest_x,
						&regression->highest_x,
						regression->lowest_outlier,
						regression->highest_outlier,
						regression->exists_outliers,
						x_value_double,
						y_value_double );
	}

	/* Get from regression.e */
	/* --------------------- */
	pclose( regression_pipe );
	regression_file = fopen( regression_filename, "r" );

	if ( get_line( regression_buffer, regression_file ) )
	{
		sscanf(	regression_buffer,
			"%lf %lf %lf",
			&slope,
			&y_intercept,
			&root_mean_squared_error );
	}

	fclose( regression_file );

	sprintf( sys_string, "rm -f %s", regression_filename );
	if ( system( sys_string ) ){};

	/* Close regression ADT file */
	/* ------------------------- */
	fclose( regression->output_file );

	/* Process regression ADT */
	/* ---------------------- */
	if ( ! (regression->linear_regression =
		regression_get_linear_regression(
				regression->temp_filename ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot get linear_regression.\n",
			 __FILE__ );
	}

	if ( ! (regression->curvilinear_regression =
		regression_get_curvilinear_regression(
				regression->temp_filename ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot get curvilinear_regression.\n",
			 __FILE__ );
	}

	regression_remove_temp_file( regression->temp_filename );

	if ( y_intercept >= 0.0 )
	{
		sprintf(new_sub_title,
		 "%s\\ny = %.2lfx + %.2lf",
			subtitle,
		 	slope,
		 	y_intercept );
	}
	else
	{
		sprintf(new_sub_title,
		"%s\\ny = %.2lfx - %.2lf",
			subtitle,
		 	slope,
		 	fabs( y_intercept ) );
	}

	if ( regression->curvilinear_regression->b >= 0.0 )
	{
		if ( regression->curvilinear_regression->c >= 0.0 )
		{
			sprintf( new_sub_title + strlen( new_sub_title ),
		 		"\\ny = %.3lfx^2 + %.3lfx + %.3lf",
				regression->curvilinear_regression->a,
				regression->curvilinear_regression->b,
				regression->curvilinear_regression->c );
		}
		else
		{
			sprintf( new_sub_title + strlen( new_sub_title ),
		 		"\\ny = %.3lfx^2 + %.3lfx - %.3lf",
				regression->curvilinear_regression->a,
				regression->curvilinear_regression->b,
				fabs( regression->curvilinear_regression->c ) );
		}
	}
	else
	{
		if ( regression->curvilinear_regression->c >= 0.0 )
		{
			sprintf( new_sub_title + strlen( new_sub_title ),
		 		"\\ny = %.3lfx^2 - %.3lfx + %.3lf",
				regression->curvilinear_regression->a,
				fabs( regression->curvilinear_regression->b ),
				regression->curvilinear_regression->c );
		}
		else
		{
			sprintf( new_sub_title + strlen( new_sub_title ),
		 		"\\ny = %.3lfx^2 - %.3lfx - %.3lf",
				regression->curvilinear_regression->a,
				fabs( regression->curvilinear_regression->b ),
				fabs( regression->curvilinear_regression->c ) );
		}
	}

	grace->sub_title = new_sub_title;

	grace_set_scale_to_zero( grace->graph_list );
	grace_set_xaxis_ticklabel_angle( grace->graph_list, 45 );
	grace_set_yaxis_ticklabel_precision( grace->graph_list, 0 );

	grace_set_symbol_size(	grace->graph_list,
				SYMBOL_SIZE );

	grace_set_structures(
				&page_width_pixels,
				&page_length_pixels,
				&distill_landscape_flag,
				&grace->landscape_mode,
				grace,
				grace->graph_list,
				grace->anchor_graph_list,
				grace->begin_date_julian,
				grace->end_date_julian,
				grace->number_of_days,
				grace->grace_graph_type,
				0 /* not force_landscape_mode */ );

	grace_set_x_minor_tickmarks_to_zero( grace->graph_list );
	grace_set_y_minor_tickmarks_to_zero( grace->graph_list );
	grace_set_xaxis_tickmarks_off( grace->graph_list );
	grace_set_yaxis_tickmajor_integer_ceiling( grace->graph_list );

	grace_set_world_max_x_based_on_grace_point_high(
				&grace->world_max_x,
				grace->graph_list );

	grace_set_world_min_x_based_on_grace_point_low(
				&grace->world_min_x,
				grace->graph_list );

	first_grace_graph = list_first( grace->graph_list );
	grace_set_line_linestyle( first_grace_graph->datatype_list, 0 );
	first_grace_graph->xaxis_label = xaxis_label;
	grace->symbols = 1;

	first_grace_graph->world_max_y =
				ceiling(
					grace_get_maximum_y_double(
					first_grace_graph->datatype_list ) );

	first_grace_graph->world_min_y =
				floor(
					grace_get_minimum_y_double(
					first_grace_graph->datatype_list ) );

	/* Build regression line from old regression.e */
	/* ------------------------------------------- */
	lowest_y = lowest_x * slope + y_intercept;
	highest_y = highest_x * slope + y_intercept;

	dataset = grace_new_grace_dataset(
					1 /* dataset number */,
					1 /* dataset_no_cycle_color */ );
	dataset->line_color = 2; /* red */

	sprintf( y_string, "%.2lf", lowest_y );
	point = grace_point_new(lowest_x,
				strdup( y_string ),
				(char *)0 /* low_string */,
				(char *)0 /* high_string */,
				(char *)0 /* optional_label */ );
	list_append_pointer( dataset->point_list, point );

	sprintf( y_string, "%.2lf", highest_y );
	point = grace_point_new(highest_x,
				strdup( y_string ),
				(char *)0 /* low_string */,
				(char *)0 /* high_string */,
				(char *)0 /* optional_label */ );
	list_append_pointer( dataset->point_list, point );
	datatype = grace_new_grace_datatype( 
				(char *)0 /* datatype_entity */,
				(char *)0 /* datatype_name */ );
	datatype->legend = "Linear regression";

	list_append_pointer( first_grace_graph->datatype_list, datatype );
	list_append_pointer( datatype->dataset_list, dataset );

	/* Build regression curve from new regression ADT */
	/* ---------------------------------------------- */
	dataset = grace_new_grace_dataset(
					2 /* dataset number */,
					1 /* dataset_no_cycle_color */ );
	dataset->line_color = 3; /* blue */

	regression->predictive_list =
		regression_get_predictive_list(
			regression->curvilinear_regression->a,
			regression->curvilinear_regression->b,
			regression->curvilinear_regression->c,
			regression->lowest_x,
			regression->highest_x );

	if( list_rewind( regression->predictive_list ) )
	{
		do {
			regression->predictive_node =
				list_get(
					regression->predictive_list );

			sprintf(	y_value,
					"%.3lf",
					regression->predictive_node->y );

			point = grace_point_new(
				regression->predictive_node->x,
				strdup( y_value ),
				(char *)0 /* low_string */,
				(char *)0 /* high_string */,
				(char *)0 /* optional_label */ );

			list_append_pointer( dataset->point_list, point );

		} while( list_next( regression->predictive_list ) );

		datatype = grace_new_grace_datatype( 
				(char *)0 /* datatype_entity */,
				(char *)0 /* datatype_name */ );

		datatype->legend = "Curvilinear regression";

		list_append_pointer(	first_grace_graph->datatype_list,
					datatype );

		list_append_pointer(	datatype->dataset_list,
					dataset );

	}

	grace_move_legend_bottom_left(	first_grace_graph,
					grace->landscape_mode );


	grace_results =
		grace_output_charts(
				grace->output_filename, 
				grace->postscript_filename,
				grace->agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				page_width_pixels,
				page_length_pixels,
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_ghost_script_directory(
					application_name ),
				grace_get_optional_label_list(
					grace->graph_list ),
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list );

	/* Output new regression ADT to file */
	/* --------------------------------- */
	statistics_output_filename =
		statistics_output_filename_get(
			appaserver_parameter->data_directory,
			session );

	output_regression_statistics(
		statistics_output_filename,
		regression->curvilinear_regression,
		regression->linear_regression );

	regression_free( regression );

	if ( grace_results )
	{
		printf(	"%s %s %s %s\n",
			grace->ftp_agr_filename,
			grace->ftp_output_filename,
			grace->output_filename,
			statistics_output_filename );
	}
	else
	{
		printf(	"null null null %s\n",
			statistics_output_filename );
	}

}

