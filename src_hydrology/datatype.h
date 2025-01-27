/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/datatype.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef DATATYPE_H
#define DATATYPE_H

#include "boolean.h"
#include "list.h"

#define DATATYPE_TABLE		"datatype"

#define DATATYPE_SELECT		"datatype,"				\
				"units,"				\
				"aggregation_sum_yn,"			\
				"bar_graph_yn,"				\
				"scale_graph_to_zero_yn,"		\
				"set_negative_values_to_zero_yn,"	\
				"calibrated_yn"

typedef struct
{
	char *datatype_name;
	char *units;
	boolean aggregation_sum_boolean;
	boolean bar_graph_boolean;
	boolean scale_graph_to_zero_boolean;
	boolean set_negative_values_to_zero_boolean;
	boolean calibrated_boolean;
} DATATYPE;

/* Usage */
/* ----- */
LIST *datatype_list(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DATATYPE *datatype_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DATATYPE *datatype_new(
		char *datatype_name );

/* Process */
/* ------- */
DATATYPE *datatype_calloc(
		void );

/* Usage */
/* ----- */
DATATYPE *datatype_fetch(
		char *datatype_name );

/* Process */
/* ------- */
DATATYPE *datatype_seek(
		char *datatype_name,
		LIST *datatype_list );

#endif
