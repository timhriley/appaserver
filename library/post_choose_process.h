/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_choose_process.h			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

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
	boolean process_parameter_drillthru_boolean;
	char *post_prompt_process_system_string;
	char *prompt_process_output_system_string;
} POST_CHOOSE_PROCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
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

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_choose_process_action_string(
			char *post_choose_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_choose_process_href_string(
			char *process_or_set_name,
			char *post_choose_process_action_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_choose_process_anchor_tag(
			char *target_frame,
			char *post_choose_process_href_string );

/* Process */
/* ------- */

#endif
