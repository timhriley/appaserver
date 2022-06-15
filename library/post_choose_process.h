/* $APPASERVER_HOME/library/post_choose_process.h	*/
/* ----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------	*/

#ifndef POST_CHOOSE_PROCESS_H
#define POST_CHOOSE_PROCESS_H

#include "boolean.h"
#include "list.h"
#include "prompt_process.h"
#include "session.h"

#define POST_CHOOSE_PROCESS_EXECUTABLE	"post_choose_process"

typedef struct
{
	SESSION_PROCESS *session_process;
	boolean no_parameters;
	boolean process_parameter_has_drillthru;
	char *post_prompt_process_system_string;
	char *prompt_process_output_system_string;
} POST_CHOOSE_PROCESS;

/* Usage */
/* ----- */
POST_CHOOSE_PROCESS *post_choose_process_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name );

/* Process */
/* ------- */
POST_CHOOSE_PROCESS *post_choose_process_calloc(
			void );

boolean post_choose_process_no_parameters(
			char *process_name );

/* Public */
/* ------ */
char *post_choose_process_href_string(
			char *post_choose_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			char *frameset_prompt_frame );

#endif
