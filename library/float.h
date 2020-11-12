/* $APPASERVER_HOME/library/float.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef FLOAT_H
#define FLOAT_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
double float_abs(	double f );

double abs_float(	double f );

double abs_dollar(	double f );

boolean double_virtually_same(
			double d1,
			double d2 );

boolean dollar_virtually_same(
			double d1,
			double d2 );

double round_double(	double d );

double round_money(	double d );

int round_int(		double d );

double ceiling_double(	double d );

double floor_double(	double d );

double ceiling(		double d );

double floor(		double d );

double round_dollar(	double d );

double round_float(	double d );

double round_pennies(	double d );

#endif
