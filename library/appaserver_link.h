/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver_link.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_LINK_H
#define APPASERVER_LINK_H

#include "boolean.h"

#define APPASERVER_LINK_PROMPT_DIRECTORY	"appaserver_data"

typedef struct
{
	char *filename;
} APPASERVER_LINK_OUTPUT;

/* Usage */
/* ----- */
APPASERVER_LINK_OUTPUT *appaserver_link_output_new(
		char *appaserver_parameter_data_directory,
		char *tail_half );

/* Process */
/* ------- */
APPASERVER_LINK_OUTPUT *appaserver_link_output_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_output_filename(
		char *appaserver_parameter_data_directory,
		char *tail_half );

typedef struct
{
	char *link_half;
	char *appaserver_link_filename;
	char *appaserver_link_tail_half;
	char *filename;
} APPASERVER_LINK_PROMPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_LINK_PROMPT *appaserver_link_prompt_new(
		char *appaserver_link_prompt_directory,
		char *http_prefix,
		char *server_address,
		char *application_name,
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session_key,
		char *extension );

/* Process */
/* ------- */
APPASERVER_LINK_PROMPT *appaserver_link_prompt_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_prompt_link_half(
		char *appaserver_link_prompt_directory,
		char *http_prefix,
		char *server_address );

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_prompt_filename(
		char *appaserver_link_prompt_link_half,
		char *appaserver_link_tail_half );

typedef struct
{
	char *http_prefix;
	char *server_address;
	char *appaserver_parameter_data_directory;
	char *filename_stem;
	char *application_name;
	pid_t process_id;
	char *session_key;
	char *begin_date_string;
	char *end_date_string;
	char *extension;
	APPASERVER_LINK_PROMPT *appaserver_link_prompt;
	char *filename;
	char *tail_half;
	APPASERVER_LINK_OUTPUT *appaserver_link_output;
} APPASERVER_LINK;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_LINK *appaserver_link_new(
		char *http_prefix,
		char *application_server_address,
		char *appaserver_parameter_data_directory,
		char *filename_stem,
		char *application_name,
		pid_t process_id,
		char *session_key,
		char *begin_date_string,
		char *end_date_string,
		char *extension );

/* Process */
/* ------- */
APPASERVER_LINK *appaserver_link_calloc(
		void );

/* Returns date_string or heap memory */
/* ---------------------------------- */
char *appaserver_link_date_string(
		char *date_string );

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_tail_half(
		char *application_name,
		char *appaserver_link_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_filename(
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session_key,
		char *extension );

/* Usage */
/* ----- */
void appaserver_link_pid_filename(
		char **prompt_filename,
		char **output_filename,
		char *application_name,
		char *appaserver_parameter_data_directory,
		pid_t process_id,
		char *process_name,
		char *extension );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_working_directory(
		char *appaserver_parameter_data_directory,
		char *application_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_anchor_html(
		char *prompt_filename,
		char *target_window,
		char *prompt_message );

#endif
