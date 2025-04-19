/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/regression.h	 			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef REGRESSION_H
#define REGRESSION_H

#include "boolean.h"
#include "list.h"

#define CURVILINEAR_REGRESSION_COEFFICIENT_OF_DETERMINATION		\
		"curvilinear_coefficient_of_determination"
#define CURVILINEAR_REGRESSION_QUADRATIC_BETA				\
		"quadratic_beta"
#define CURVILINEAR_REGRESSION_QUADRATIC_BETA_STANDARD_DEVIATION	\
		"quadratic_beta_standard_deviation"
#define CURVILINEAR_REGRESSION_QUADRATIC_BETA_T_VALUE			\
		"quadratic_beta_t_value"
#define CURVILINEAR_REGRESSION_QUADRATIC_BETA_TEST_STATISTIC		\
		"quadratic_beta_test_statistic"
#define CURVILINEAR_REGRESSION_QUADRATIC_BETA_IS_SIGNIFICANT		\
		"quadratic_beta_is_significant"
#define CURVILINEAR_REGRESSION_MINIMUM_QUADRATIC_BETA			\
		"minimum_quadratic_beta"
#define CURVILINEAR_REGRESSION_MAXIMUM_QUADRATIC_BETA			\
		"maximum_quadratic_beta"

#define LINEAR_REGRESSION_COEFFICIENT_OF_DETERMINATION			\
		"linear_coefficient_of_determination"
#define LINEAR_REGRESSION_BETA						\
		"slope"
#define LINEAR_REGRESSION_Y_INTERCEPT					\
		"y_intercept"
#define LINEAR_REGRESSION_BETA_STANDARD_DEVIATION			\
		"slope_standard_deviation"
#define LINEAR_REGRESSION_BETA_T_VALUE					\
		"slope_t_value"
#define LINEAR_REGRESSION_BETA_TEST_STATISTIC				\
		"slope_test_statistic"
#define LINEAR_REGRESSION_BETA_IS_SIGNIFICANT				\
		"slope_is_significant"
#define LINEAR_REGRESSION_MINIMUM_BETA					\
		"minimum_slope"
#define LINEAR_REGRESSION_MAXIMUM_BETA					\
		"maximum_slope"
#define LINEAR_REGRESSION_Y_INTERCEPT_MINIMUM				\
		"y_intercept_minimum"
#define LINEAR_REGRESSION_Y_INTERCEPT_MAXIMUM				\
		"y_intercept_maximum"

typedef struct
{
	double x;
	double y;
} REGRESSION_PREDICTIVE_NODE;

typedef struct
{
	double a;
	double b;
	double c;
	double error;
	double quadratic_beta;
	double beta;
	double y_intercept;
	double coefficient_of_determination;
	double r_squared;
	double residual_standard_deviation;
	double quadratic_beta_standard_deviation;
	double quadratic_beta_t_value;
	double quadratic_beta_test_statistic;
	boolean quadratic_beta_is_significant;
	double minimum_quadratic_beta;
	double maximum_quadratic_beta;
} CURVILINEAR_REGRESSION;

typedef struct
{
	double a;
	double b;
	double error;
	double beta;
	double y_intercept;
	double coefficient_of_determination;
	double r_squared;
	double residual_standard_deviation;
	double beta_standard_deviation;
	double beta_t_value;
	double beta_test_statistic;
	boolean beta_is_significant;
	double minimum_beta;
	double maximum_beta;
	double y_intercept_minimum;
	double y_intercept_maximum;
} LINEAR_REGRESSION;

typedef struct
{
	FILE *output_file;
	char *temp_filename;
	double lowest_x;
	double highest_x;
	LINEAR_REGRESSION *linear_regression;
	CURVILINEAR_REGRESSION *curvilinear_regression;
	LIST *predictive_list;
	REGRESSION_PREDICTIVE_NODE *predictive_node;
	int lowest_outlier;
	int highest_outlier;
	boolean exists_outliers;
} REGRESSION;

REGRESSION *regression_new(		char *lowest_outlier,
					char *highest_outlier );

char *regression_temp_filename(	void );

void regression_set_output_file(	FILE *output_file,
					double *lowest_x,
					double *highest_x,
					int lowest_outlier,
					int highest_outlier,
					boolean exists_outliers,
					double x,
					double y );

void regression_free(			REGRESSION *r );

CURVILINEAR_REGRESSION *regression_curvilinear_regression_new(
					void );

LINEAR_REGRESSION *regression_linear_regression_new(
					void );

LINEAR_REGRESSION *regression_get_linear_regression(
					char *temp_filename );

CURVILINEAR_REGRESSION *regression_get_curvilinear_regression(
					char *temp_filename );

REGRESSION_PREDICTIVE_NODE *regression_predictive_node_new(
					void );

LIST *regression_get_predictive_list(
					double a,
					double b,
					double c,
					double lowest_x,
					double highest_x );

void regression_remove_temp_file(	char *temp_filename );

void regression_output_statistics_table(
					char *statistics_filename );

#endif
