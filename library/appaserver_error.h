/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver_error.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_ERROR_H
#define APPASERVER_ERROR_H

#include <stdio.h>

typedef struct
{
	char *filename;
} APPASERVER_ERROR;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_ERROR *appaserver_error_new(
		char *application_name );

/* Process */
/* ------- */
APPASERVER_ERROR *appaserver_error_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_error_filename(
		char *application_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_error_prompt(
		char *argv_0,
		char *login_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *appaserver_error_open_file(
		char *application_name );

/* Usage */
/* ----- */
void appaserver_error_stderr(
		int argc,
		char **argv );

/* Usage */
/* ----- */
void appaserver_error_argv_file(
		int argc,
		char **argv,
		char *application_name,
		char *login_name );

/* Usage */
/* ----- */
void appaserver_error_message_file(
		char *application_name,
		char *login_name,
		char *message );

/* Usage */
/* ----- */
void appaserver_error_stderr_exit(
		const char *file,
		const char *function,
		const int line,
		char *message );

/* Usage */
/* ----- */
void appaserver_error_warning(
		const char *file,
		const char *function,
		const int line,
		char *message );

/* Usage */
/* ------ */
void msg(	char *application_name,
		char *message );

/* Usage */
/* ----- */
void appaserver_error_argv_append_file(
		int argc,
		char **argv,
		char *application_name );

/* Usage */
/* ----- */
void appaserver_error_login_name_append_file(
		int argc,
		char **argv,
		char *application_name,
		char *login_name );

#endif
