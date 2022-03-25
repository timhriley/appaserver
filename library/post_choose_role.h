/* $APPASERVER_HOME/library/post_choose_role.h		*/
/* -----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* -----------------------------------------------	*/

#ifndef POST_CHOOSE_ROLE_H
#define POST_CHOOSE_ROLE_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "post_dictionary.h"

/* Constants */
/* --------- */
#define POST_CHOOSE_ROLE_EXECUTABLE	"post_choose_role"

typedef struct
{
	char *sql_injection_escape_application_name;
	char *sql_injection_escape_session_key;
	char *sql_injection_escape_login_name;
	char *sql_injection_escape_role_name;
	char *menu_output_system_string;
} POST_CHOOSE_ROLE;

/* Usage */
/* ----- */
POST_CHOOSE_ROLE *post_choose_role_new(
			int argc,
			char **argv,
			char *choose_role_drop_down_element_name );

/* Process */
/* ------- */
POST_CHOOSE_ROLE *post_choose_role_calloc(
			void );

char *post_choose_role_name(
			int argc,
			char *argv_4,
			char *choose_role_drop_down_element_name );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *post_choose_role_action_string(
			char *post_choose_post_executable,
			char *application_name,
			char *session_key,
			char *login_name );

/* Returns heap memory */
/* ------------------- */
char *post_choose_role_href_string(
			char *post_choose_post_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *frameset_prompt_frame );


#endif
