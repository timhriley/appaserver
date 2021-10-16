/* $APPASERVER_HOME/library/appaserver_link.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef APPASERVER_LINK_H
#define APPASERVER_LINK_H

#include "boolean.h"

/* Constants */
/* --------- */
#define APPASERVER_KEY			"appaserver"
#define APPASERVER_DATA_KEY		"data"

/* Type definitions */
/* ---------------- */
typedef struct
{
	/* Process */
	/* ------- */
	char *filename;
} APPASERVER_LINK_OUTPUT;

typedef struct
{
	/* Process */
	/* ------- */
	char *filename;
} APPASERVER_LINK_PROMPT;

typedef struct
{
	/* Input */
	/* ----- */
	char *document_root_directory;
	char *filename_stem;
	char *application_name;
	pid_t process_id;
	char *session_key;
	char *begin_date_string;
	char *end_date_string;
	char *extension;

	/* Process */
	/* ------- */
	char *tail_half;
	APPASERVER_LINK_OUTPUT *output;
	APPASERVER_LINK_PROMPT *prompt;
} APPASERVER_LINK;

/* APPASERVER_LINK operations */
/* -------------------------- */
APPASERVER_LINK *appaserver_link_new(
			char *http_prefix,
			char *server_address,
			boolean prepend_http,
			char *document_root_directory,
			char *filename_stem,
			char *application_name,
			pid_t process_id,
			char *session_key,
			char *begin_date_string,
			char *end_date_string,
			char *extension );

APPASERVER_LINK *appaserver_link_calloc(
			void );

void appaserver_link_pid_filename(
			char **prompt_filename,
			char **output_filename,
			char *application_name,
			char *document_root_directory,
			pid_t process_id,
			char *process_name,
			char *extension );

/* ------------------- */
/* Returns heap memory */
/* ------------------- */
char *appaserver_link_working_directory(
			char *document_root_directory,
			char *application_name );

/* APPASERVER_LINK_PROMPT operations */
/* --------------------------------- */
APPASERVER_LINK_PROMPT *appaserver_link_prompt_new(
			char *http_prefix,
			char *server_address,
			boolean prepend_http );

APPASERVER_LINK_PROMPT *appaserver_link_prompt_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_prompt_link_half(
			boolean prepend_http,
			char *http_prefix,
			char *server_address );

/* APPASERVER_LINK_OUTPUT operations */
/* --------------------------------- */
APPASERVER_LINK_OUTPUT *appaserver_link_output_new(
			char *document_root_directory,
			char *tail_half );

APPASERVER_LINK_OUTPUT *appaserver_link_output_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_output_filename(
			char *document_root_directory,
			char *tail_half );

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_output_tail_half(
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session_key,
			char *extension );

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_output_abbreviated_filename(
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session_key,
			char *extension );

/* APPASERVER_LINK_PROMPT operations */
/* --------------------------------- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_link_prompt_filename(
			boolean prepend_http,
			char *http_prefix,
			char *server_address,
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session_key,
			char *extension );

#endif
