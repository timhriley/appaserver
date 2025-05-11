/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef GOOGLE_H
#define GOOGLE_H

#include "boolean.h"
#include "list.h"
#include "appaserver_link.h"

#define GOOGLE_FILENAME_STEM	"google"

typedef struct
{
	pid_t process_id;
	APPASERVER_LINK *appaserver_link;
	char *http_output_filename;
	char *http_prompt_filename;
	char *appaserver_link_anchor_html;
} GOOGLE_FILENAME;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GOOGLE_FILENAME *google_filename_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *number_attribute_name,
		char *chart_filename_key );

/* Process */
/* ------- */
GOOGLE_FILENAME *google_filename_calloc(
		void );

pid_t google_filename_process_id(
		char *session_key );

/* Returns heap memory */
/* ------------------- */
char *google_filename_http_output_filename(
		APPASERVER_LINK *appaserver_link );

/* Returns heap memory */
/* ------------------- */
char *google_filename_http_prompt_filename(
		APPASERVER_LINK *appaserver_link );

typedef struct
{
	char *target;
	char *html;
} GOOGLE_PROMPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GOOGLE_PROMPT *google_prompt_new(
		GOOGLE_FILENAME *google_filename,
		char *chart_filename_key,
		pid_t process_id );

/* Process */
/* ------- */
GOOGLE_PROMPT *google_prompt_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_prompt_target(
		char *chart_filename_key,
		pid_t process_id );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_prompt_html(
		char *http_prompt_filename,
		char *chart_filename_key,
		char *google_prompt_target );

typedef struct
{
	char *google_prompt_target;
	char *onload_string;
	char *date_time_now;
	char *screen_title;
	char *html;
} GOOGLE_WINDOW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GOOGLE_WINDOW *google_window_new(
		GOOGLE_FILENAME *google_filename,
		char *sub_title,
		pid_t process_id );

/* Process */
/* ------- */
GOOGLE_WINDOW *google_window_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *google_window_screen_title(
		char *date_time_now );

#endif
