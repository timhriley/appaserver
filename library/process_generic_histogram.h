/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_histogram.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_HISTOGRAM_H
#define PROCESS_GENERIC_HISTOGRAM_H

#include "list.h"
#include "boolean.h"
#include "dictionary.h"
#include "process_generic.h"

typedef struct
{
	char *process_generic_process_set_name;
	char *process_generic_process_name;
	PROCESS_GENERIC *process_generic;
	char *system_string;
} PROCESS_GENERIC_HISTOGRAM;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_HISTOGRAM *process_generic_histogram_new(
			char *application_name,
			char *login_name,
			char *process_name,
			char *process_set_name,
			DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROCESS_GENERIC_HISTOGRAM *process_generic_histogram_calloc(
			void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *process_generic_histogram_system_string(
			char delimiter,
			char *application_name,
			char *value_attribute_name,
			char *system_string,
			char *title,
			char *sub_title,
			int value_piece );

/* Process */
/* ------- */

#endif
