/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_google.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_GOOGLE_H
#define PROCESS_GENERIC_GOOGLE_H

#include "list.h"
#include "boolean.h"
#include "google_chart.h"
#include "process_generic.h"

typedef struct
{
	char *process_generic_process_set_name;
	char *process_generic_process_name;
	PROCESS_GENERIC *process_generic;
	LIST *point_list;
	GOOGLE_GENERIC *google_generic;
	GOOGLE_WINDOW *google_window;
} PROCESS_GENERIC_GOOGLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_GOOGLE *process_generic_google_new(
			char *application_name,
			char *login_name,
			char *process_name,
			char *process_set_name,
			char *data_directory,
			DICTIONARY *post_dictionary /* in/out */ );

/* Process */
/* ------- */
PROCESS_GENERIC_GOOGLE *process_generic_google_calloc(
			void );


/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *process_generic_google_point_list(
			char sql_delimiter,
			char *process_generic_system_string,
			int process_generic_input_date_piece,
			int process_generic_input_time_piece,
			int process_generic_input_value_piece );

/* Process */
/* ------- */

#endif
