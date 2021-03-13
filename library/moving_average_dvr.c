/* library/moving_average_dvr.c */
/* ======================================================= */
/* Freely available software: see Appaserver.org	   */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "moving_average.h"

void test_average( void );
void test_maximum( void );
void test_minimum( void );
void test_is_null( void );

int main( void )
{
	printf( "test_average()\n" );
	test_average();
	printf( "test_is_null()\n" );
	test_is_null();
	/* printf( "test_maximum()\n" ); */
	/* test_maximum(); */
	/* printf( "test_minimum()\n" ); */
	/* test_minimum(); */
	return 0;
}

void test_average( void )
{
	MOVING_AVERAGE *moving_average;
	LIST *value_list;

	moving_average =
		moving_average_new(
			5, '^', average, "1999-06-05");

	value_list = moving_average->value_list;

	moving_average_set( value_list, 1.0, "1999-06-01", 0);
	moving_average_set( value_list, 2.0, "1999-06-02", 0);
	moving_average_set( value_list, 3.0, "1999-06-03", 0);
	moving_average_set( value_list, 4.0, "1999-06-04", 0);
	moving_average_set( value_list, 5.0, "1999-06-05", 0);
	moving_average_set( value_list, 6.0, "1999-06-06", 0);
	moving_average_set( value_list, 7.0, "1999-06-07", 0);
	moving_average_set( value_list, 8.0, "1999-06-08", 0);
	moving_average_set( value_list, 9.0, "1999-06-09", 0);
	moving_average_set( value_list, 10.0, "1999-06-10", 0);

	moving_average_display(
				value_list,
				moving_average->number_of_days,
				moving_average->delimiter,
				moving_average->aggregate_statistic,
				moving_average->begin_date_string );

} /* test_average() */

void test_is_null( void )
{
	MOVING_AVERAGE *moving_average;
	LIST *value_list;

	moving_average =
		moving_average_new(
			5, '^', average, "1999-06-05" );

	value_list = moving_average->value_list;

	moving_average_set( value_list, 1.0, "1999-06-01", 0);
	moving_average_set( value_list, 2.0, "1999-06-02", 0);
	moving_average_set( value_list, 3.0, "1999-06-03", 0);
	moving_average_set( value_list, 4.0, "1999-06-04", 0);
	moving_average_set( value_list, 0.0, "1999-06-05", 1);
	moving_average_set( value_list, 1.0, "1999-06-06", 0);
	moving_average_set( value_list, 2.0, "1999-06-07", 0);
	moving_average_set( value_list, 3.0, "1999-06-08", 0);
	moving_average_set( value_list, 4.0, "1999-06-09", 0);
	moving_average_set( value_list, 5.0, "1999-06-10", 0);

	moving_average_display(
				value_list,
				moving_average->number_of_days,
				moving_average->delimiter,
				moving_average->aggregate_statistic,
				moving_average->begin_date_string );

} /* test_is_null() */

void test_maximum( void )
{
	MOVING_AVERAGE *moving_average;
	LIST *value_list;

	moving_average =
		moving_average_new(
			5, '^', maximum, "1999-06-05" );

	value_list = moving_average->value_list;

	moving_average_set( value_list, -1.0, "1999-06-01", 0);
	moving_average_set( value_list, -2.0, "1999-06-02", 0);
	moving_average_set( value_list, -3.0, "1999-06-03", 0);
	moving_average_set( value_list, -4.0, "1999-06-04", 0);
	moving_average_set( value_list, -5.0, "1999-06-05", 0);
	moving_average_set( value_list, -6.0, "1999-06-06", 0);
	moving_average_set( value_list, -7.0, "1999-06-07", 0);
	moving_average_set( value_list, -8.0, "1999-06-08", 0);
	moving_average_set( value_list, -9.0, "1999-06-09", 0);
	moving_average_set( value_list, -10.0, "1999-06-10", 0);

	moving_average_display(
				value_list,
				moving_average->number_of_days,
				moving_average->delimiter,
				moving_average->aggregate_statistic,
				moving_average->begin_date_string );

} /* test_maximum() */

void test_minimum( void )
{
	MOVING_AVERAGE *moving_average;
	LIST *value_list;

	moving_average =
		moving_average_new(
			5, '^', minimum, "1999-06-05" );

	value_list = moving_average->value_list;

	moving_average_set( value_list, -1.0, "1999-06-01", 0);
	moving_average_set( value_list, -2.0, "1999-06-02", 0);
	moving_average_set( value_list, -3.0, "1999-06-03", 0);
	moving_average_set( value_list, -4.0, "1999-06-04", 0);
	moving_average_set( value_list, -5.0, "1999-06-05", 0);
	moving_average_set( value_list, -6.0, "1999-06-06", 0);
	moving_average_set( value_list, -7.0, "1999-06-07", 0);
	moving_average_set( value_list, -8.0, "1999-06-08", 0);
	moving_average_set( value_list, -9.0, "1999-06-09", 0);
	moving_average_set( value_list, -10.0, "1999-06-10", 0);

	moving_average_display(
				value_list,
				moving_average->number_of_days,
				moving_average->delimiter,
				moving_average->aggregate_statistic,
				moving_average->begin_date_string );

} /* test_minimum() */

