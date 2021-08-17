/* $APPASERVER_HOME/library/appaserver_link_file.h			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_LINK_FILE_H
#define APPASERVER_LINK_FILE_H

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
} LINK_OUTPUT_FILE;

typedef struct
{
	boolean prepend_http_boolean;
	char *http_prefix;
	char *server_address;
} LINK_PROMPT;

typedef struct
{
	char *application_name;
	char *filename_stem;
	char *begin_date_string;
	char *end_date_string;
	pid_t process_id;
	char *session;
	char *extension;
	LINK_OUTPUT_FILE *output_file;
	LINK_PROMPT *link_prompt;
} APPASERVER_LINK_FILE;

/* Prototypes */
/* ---------- */
APPASERVER_LINK_FILE *appaserver_link_file_new(
				char *http_prefix,
				char *server_address,
				boolean prepend_http_boolean,
				char *document_root_directory,
				char *filename_stem,
				char *application_name,
				pid_t process_id,
				char *session,
				char *extension );

LINK_PROMPT *appaserver_link_prompt_new(
				char *http_prefix,
				char *server_address,
				boolean prepend_http_boolean );

LINK_OUTPUT_FILE *appaserver_link_output_file_new(
				char *document_root_directory );

char *appaserver_link_abbreviated_output_filename(
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session,
		char *extension );

/* ---------------- */
/* Returns strdup() */
/* ---------------- */
char *appaserver_link_output_filename(
		char *document_root_directory,
		char *application_name,
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session,
		char *extension );

/* ---------------- */
/* Returns strdup() */
/* ---------------- */
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
