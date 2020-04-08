/* $APPASERVER_HOME/src_hydrology/measurement_spike.h	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef MEASUREMENT_SPIKE_H
#define MEASUREMENT_SPIKE_H

#include <string.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "boolean.h"
#include "list.h"
#include "timlib.h"
#include "piece.h"
#include "station_datatype.h"
#include "measurement.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *begin_date;
	char *end_date;
	double threshold;
	boolean trim_negative_drop;
	STATION_DATATYPE *station_datatype;
} MEASUREMENT_SPIKE_INPUT;

typedef struct
{
	double last_good_measurement_value;
	double next_first_good_measurement_value;
	LIST *spike_measurement_list;
} MEASUREMENT_SPIKE_BLOCK;

typedef struct
{
	MEASUREMENT_SPIKE_INPUT input;
	LIST *spike_block_list;
} MEASUREMENT_SPIKE;

/* Prototypes */
/* ---------- */
MEASUREMENT_SPIKE_BLOCK *measurement_spike_block_new(
			void );

MEASUREMENT_SPIKE *measurement_spike_new(
			char *application_name,
			char *station_name,
			char *datatype_name,
			char *begin_date,
			char *end_date,
			double threshold,
			boolean trim_negative_drop );

/* -------------------------------------- */
/* Sets last_good_measurement_value	  */
/* Sets next_first_good_measurement_value */
/* Doesn't set measurement_update	  */
/* -------------------------------------- */
LIST *measurement_spike_get_block_list(
			LIST *measurement_list,
			double threshold,
			boolean trim_negative_drop );

void measurement_spike_block_update_output(
			char *application_name,
			LIST *spike_block_list );

void measurement_spike_update_output(
			FILE *output_pipe,
			LIST *measurement_list );

LIST *measurement_spike_block_measurement_list(
			/* ------------------------- */
			/* Starts with the first bad */
			/* ------------------------- */
			LIST *measurement_list /* in/out */,
			double *next_first_good_measurement_value,
			double last_good_measurement_value,
			double threshold,
			boolean trim_negative_drop );

boolean measurement_spike_exceed_threshold(
			double measurement_value,
			double prior_measurement_value,
			double threshold,
			boolean trim_negative_drop );

void measurement_spike_set_block_measurement_update(
			LIST *spike_block_list );

void measurement_spike_set_measurement_update(
			LIST *spike_measurement_list,
			double last_good_measurement_value,
			double next_first_good_measurement_value );

char *measurement_spike_block_display(
			LIST *spike_block_list );

void measurement_spike_text_output(
			LIST *spike_measurement_list,
			char delimiter );

#endif
