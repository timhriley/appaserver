/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/moving_statistic.h				   */
/* ----------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	   */
/* ----------------------------------------------------------------------- */

#ifndef MOVING_STATISTIC_H
#define MOVING_STATISTIC_H

#include "list.h"
#include "aggregate_statistic.h"
#include "statistic.h"

typedef struct
{
	char *input;
	char *date_string;
} MOVING_STATISTIC_VALUE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
MOVING_STATISTIC_VALUE *moving_statistic_value_new(
		char *input,
		char *date_string );

/* Process */
/* ------- */
MOVING_STATISTIC_VALUE *moving_statistic_value_calloc(
		void );

#define MOVING_STATISTIC_DEFAULT_DAYS	30

typedef struct
{
	int days_count;
	int value_piece;
	char delimiter;
	enum aggregate_statistic aggregate_statistic;
	char *begin_date_string;
	LIST *moving_statistic_value_list;
} MOVING_STATISTIC;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
MOVING_STATISTIC *moving_statistic_new(
		int days_count,
		int value_piece,
		char delimiter,
		enum aggregate_statistic aggregate_statistic,
		char *begin_date_string );

/* Process */
/* ------- */
MOVING_STATISTIC *moving_statistic_calloc(
		void );

/* Usage */
/* ----- */
void moving_statistic_input(
		LIST *moving_statistic_value_list,
		char delimiter,
		char *input );

/* Usage */
/* ----- */
void moving_statistic_display(
		int days_count,
		int value_piece,
		char delimiter,
		enum aggregate_statistic aggregate_statistic,
		char *begin_date_string,
		LIST *moving_statistic_value_list );

/* Usage */
/* ----- */
void moving_statistic_accumulate(
		STATISTIC *statistic /* in/out */,
		int days_count,
		LIST *moving_statistic_value_list );

/* Usage */
/* ----- */
void moving_statistic_output(
		int value_piece,
		char delimiter,
		enum aggregate_statistic aggregate_statistic,
		FILE *sort_pipe,
		char *input,
		double sum,
		STATISTIC_CALCULATE *statistic_calculate,
		STATISTIC_QUARTILE *statistic_quartile );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *moving_statistic_value_string(
		enum aggregate_statistic aggregate_statistic,
		double sum,
		STATISTIC_CALCULATE *statistic_calculate,
		STATISTIC_QUARTILE *statistic_quartile );

/* Returns heap memory */
/* ------------------- */
char *moving_statistic_replace(
		int value_piece,
		char delimiter,
		char *input,
		char *moving_statistic_value_string );

#endif
