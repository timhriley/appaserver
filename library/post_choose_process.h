/* $APPASERVER_HOME/library/post_choose_process.h	*/
/* ----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------	*/

#ifndef POST_CHOOSE_PROCESS_H
#define POST_CHOOSE_PROCESS_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "process.h"

/* Constants */
/* --------- */
#define POST_CHOOSE_PROCESS_EXECUTABLE	"post_choose_process"

typedef struct
{
	char *system_string;
} POST_CHOOSE_PROCESS;

/* POST_CHOOSE_PROCESS operations */
/* ------------------------------ */

/* Usage */
/* ----- */
POST_CHOOSE_PROCESS *post_choose_process_new(
			/* ------------------------------------ */
			/* See session_process_integrity_exit() */
			/* ------------------------------------ */
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *post_choose_process_system_string(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name );

/* Private */
/* ------- */
POST_CHOOSE_PROCESS *post_choose_process_calloc(
			void );

/* Public */
/* ------ */
char *post_choose_process_action_string(
			char *post_choose_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			char *frameset_prompt_frame );

#endif
