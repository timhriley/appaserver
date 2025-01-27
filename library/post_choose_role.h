/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_choose_role.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef POST_CHOOSE_ROLE_H
#define POST_CHOOSE_ROLE_H

#include "boolean.h"
#include "list.h"
#include "post_dictionary.h"

#define POST_CHOOSE_ROLE_EXECUTABLE	"post_choose_role"

typedef struct
{
	char *sql_injection_escape_application_name;
	char *sql_injection_escape_session_key;
	char *sql_injection_escape_login_name;
	char *name;
	char *execute_system_string_menu;
} POST_CHOOSE_ROLE;

/* Usage */
/* ----- */
POST_CHOOSE_ROLE *post_choose_role_new(
		int argc,
		char **argv,
		char *choose_role_drop_down_widget_name );

/* Process */
/* ------- */
POST_CHOOSE_ROLE *post_choose_role_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_choose_role_name(
		int argc,
		char *argv_4,
		char *choose_role_drop_down_widget_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_choose_role_action_string(
		char *post_choose_post_executable,
		char *application_name,
		char *session_key,
		char *login_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_choose_role_href_string(
		char *role_name,
		char *post_choose_role_action_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_choose_role_anchor_tag(
		char *target_frame,
		char *post_choose_role_href_string );

#endif
