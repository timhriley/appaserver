/* $APPASERVER_HOME/library/float.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef FLOAT_H
#define FLOAT_H

#include "boolean.h"
#include "list.h"

double float_abs(	double f );

double abs_float(	double f );

double abs_dollar(	double f );

boolean double_virtually_same(
			double d1,
			double d2 );

boolean money_virtually_same(
			double d1,
			double d2 );

boolean dollar_virtually_same(
			double d1,
			double d2 );

boolean double_virtually_same_places(
			double d1,
			double d2,
			int places );

double round_double(	double d );

double round_money(	double d );

int round_int(		double d );

int float_round_int(	double d );

double ceiling_double(	double d );

double floor_double(	double d );

double ceiling(		double d );

double floor(		double d );

double round_dollar(	double d );

double round_float(	double d );

double round_pennies(	double d );

int float_percent_of_total(
			double total,
			double denominator );

int float_delta_prior_percent(
			double prior_total,
			double total );

int float_ratio_to_percent(
			double ratio );

#endif
