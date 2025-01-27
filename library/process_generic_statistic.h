/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_statistic.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_STATISTIC_H
#define PROCESS_GENERIC_STATISTIC_H

#include "list.h"
#include "boolean.h"
#include "statistic.h"
#include "html.h"
#include "process_generic.h"

typedef struct
{
	char *process_generic_process_set_name;
	char *process_generic_process_name;
	PROCESS_GENERIC *process_generic;
	STATISTIC *statistic;
	HTML_TABLE *html_table;
} PROCESS_GENERIC_STATISTIC;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_STATISTIC *process_generic_statistic_new(
		char *application_name,
		char *login_name,
		char *process_name,
		char *process_set_name,
		DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROCESS_GENERIC_STATISTIC *process_generic_statistic_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HTML_TABLE *process_generic_statistic_html_table(
		char *value_attribute_name,
		char *process_generic_sub_title,
		STATISTIC *statistic );

#endif
