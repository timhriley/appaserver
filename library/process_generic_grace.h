/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_grace.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_GRACE_H
#define PROCESS_GENERIC_GRACE_H

#include "list.h"
#include "boolean.h"
#include "grace.h"
#include "process_generic.h"

typedef struct
{
	char *process_generic_process_set_name;
	char *process_generic_process_name;
	PROCESS_GENERIC *process_generic;
	LIST *point_list;
	int horizontal_range;
	GRACE_GENERIC *grace_generic;
	GRACE_WINDOW *grace_window;
	GRACE_PROMPT *grace_prompt;
} PROCESS_GENERIC_GRACE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_GRACE *process_generic_grace_new(
		char *application_name,
		char *login_name,
		char *process_name,
		char *process_set_name,
		char *document_root,
		DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROCESS_GENERIC_GRACE *process_generic_grace_calloc(
		void );

/* Usage */
/* ----- */
LIST *process_generic_grace_point_list(
		char sql_delimiter,
		char *process_generic_system_string,
		int process_generic_input_date_piece,
		int process_generic_input_time_piece,
		int process_generic_input_value_piece );

/* Usage */
/* ----- */
int process_generic_grace_horizontal_range(
		LIST *process_generic_grace_point_list );

#endif
