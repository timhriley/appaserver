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
	char *document_root_directory;
} APPASERVER_LINK_OUTPUT;

typedef struct
{
	boolean prepend_http_boolean;
	char *http_prefix;
	char *server_address;
} APPASERVER_LINK_PROMPT;

typedef struct
{
	/* Input */
	/* ----- */
	char *filename_stem;
	char *application_name;
	pid_t process_id;
	char *session_key;
	char *extension;

	/* Process */
	/* ------- */
	char *begin_date_string;
	char *end_date_string;
	APPASERVER_LINK_OUTPUT *output;
	APPASERVER_LINK_PROMPT *prompt;
} APPASERVER_LINK;

/* APPASERVER_LINK operations */
/* -------------------------- */
APPASERVER_LINK *appaserver_link_new(
			char *http_prefix,
			char *server_address,
			boolean prepend_http_boolean,
			char *document_root_directory,
			char *filename_stem,
			char *application_name,
			pid_t process_id,
			char *session_key,
			char *extension );

/* APPASERVER_LINK_PROMPT operations */
/* --------------------------------- */
APPASERVER_LINK_PROMPT *appaserver_link_prompt_new(
			char *http_prefix,
			char *server_address,
			boolean prepend_http_boolean );

/* APPASERVER_LINK_OUTPUT operations */
/* --------------------------------- */
APPASERVER_LINK_OUTPUT *appaserver_link_output_new(
			char *document_root_directory );

char *appaserver_link_abbreviated_output_filename(
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session,
			char *extension );

/* ------------------- */
/* Returns heap memory */
/* ------------------- */
char *appaserver_link_output_filename(
			char *document_root_directory,
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session,
			char *extension );

/* ------------------- */
/* Returns heap memory */
/* ------------------- */
char *appaserver_link_output_filename(
			char *document_root_directory,
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session,
			char *extension );

/* ------------------- */
/* Returns heap memory */
/* ------------------- */
char *appaserver_link_prompt_filename(
			boolean prepend_http_boolean,
			char *http_prefix,
			char *server_address,
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session,
			char *extension );

/* ------------------- */
/* Returns heap memory */
/* ------------------- */
char *appaserver_link_source_directory(
			char *document_root_directory,
			char *application_name );

/* --------------------- */
/* Returns static memory */
/* --------------------- */
char *appaserver_link_tail_half(
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session,
			char *extension );

/* --------------------- */
/* Returns static memory */
/* --------------------- */
char *appaserver_link_tail_half(
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session,
			char *extension );

void appaserver_link_pid_filename(
			char **output_filename,
			char **prompt_filename,
			char *application_name,
			char *document_root_directory,
			pid_t pid,
			char *process_name,
			char *extension );

void appaserver_link_pid_filename(
			char **output_filename,
			char **prompt_filename,
			char *application_name,
			char *document_root_directory,
			pid_t pid,
			char *process_name,
			char *extension );

#endif
