/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/float.h					   */
/* ----------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	   */
/* ----------------------------------------------------------------------- */

#ifndef FLOAT_H
#define FLOAT_H

#include "boolean.h"
#include "list.h"
#include "float.h"

#define FLOAT_MAXIMUM_DOUBLE	DBL_MAX
#define FLOAT_MINIMUM_DOUBLE	-(DBL_MAX - 1.0)

double float_abs(
		double f );

double abs_float(
		double f );

/* Rounds to the dollar, not the penny */
/* ----------------------------------- */
double abs_dollar(
		double f );

boolean float_virtually_same(
		double d1,
		double d2 );

boolean double_virtually_same(
		double d1,
		double d2 );

boolean float_money_virtually_same(
		double d1,
		double d2 );

boolean float_dollar_virtually_same(
		double d1,
		double d2 );

boolean dollar_virtually_same(
		double d1,
		double d2 );

boolean float_virtually_same_places(
		double d1,
		double d2,
		int places );

double round_double(
		double d );

double float_round_money(
		double d );

double round_money(
		double d );

int round_int(	double d );

int float_round_int(
		double d );

double ceiling_double(
		double d );

double floor_double(
		double d );

double ceiling(	double d );

double floor(	double d );

double round_dollar(
		double d );

double round_float(
		double d );

double round_pennies(
		double d );

int float_percent_of_total(
		double total,
		double denominator );

int float_delta_prior_percent(
		double prior_total,
		double total );

int float_ratio_to_percent(
		double ratio );

boolean float_integer_boolean(
		double d );

boolean float_is_integer(
		double d );

/* Returns static memory */
/* --------------------- */
char *float_string(
		double d,
		int decimal_count );

#endif
