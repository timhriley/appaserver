/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/populate_helper_process.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POPULATE_HELPER_PROCESS_H
#define POPULATE_HELPER_PROCESS_H

#include <unistd.h>
#include "boolean.h"
#include "application.h"

typedef struct
{
	char *name;
	char *data_directory;
	char *application_name;
	pid_t process_id;
	char *output_filename;
	char *prompt;
} POPULATE_HELPER_PROCESS;

POPULATE_HELPER_PROCESS *populate_helper_process_calloc(
		void );

POPULATE_HELPER_PROCESS *populate_helper_process_new(
		char *name,
		char *data_directory,
		char *application_name,
		pid_t process_id );

char *populate_helper_process_output_filename(
		char *name,
		char *data_directory,
		char *application_name,
		pid_t process_id );

char *populate_helper_process_prompt(
		char *name,
		char *data_directory,
		char *application_name,
		pid_t process_id );

#endif
