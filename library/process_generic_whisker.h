/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_whisker.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_WHISKER_H
#define PROCESS_GENERIC_WHISKER_H

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
	double vertical_range;
	GRACE_WHISKER *grace_whisker;
	GRACE_WINDOW *grace_window;
	GRACE_PROMPT *grace_prompt;
} PROCESS_GENERIC_WHISKER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_WHISKER *process_generic_whisker_new(
		char *application_name,
		char *login_name,
		char *process_name,
		char *process_set_name,
		char *document_root,
		DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROCESS_GENERIC_WHISKER *process_generic_whisker_calloc(
		void );

/* Usage */
/* ----- */
LIST *process_generic_whisker_point_list(
		char sql_delimiter,
		char *process_generic_system_string,
		int process_generic_input_date_piece,
		int process_generic_input_time_piece,
		int average_value_piece,
		int low_value_piece,
		int high_value_piece );

/* Usage */
/* ----- */
int process_generic_whisker_horizontal_range(
		LIST *process_generic_whisker_point_list );

/* Usage */
/* ----- */
double process_generic_whisker_vertical_range(
		LIST *process_generic_whisker_point_list );

#endif
