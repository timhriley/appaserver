/* $APPASERVER_HOME/library/session.h			*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef SESSION_H
#define SESSION_H

#include "list.h"
#include "boolean.h"

#define SESSION_REMOTE_IP_ADDRESS_VARIABLE	"REMOTE_ADDR"
#define SESSION_MAX_SESSION_SIZE		10

typedef struct
{
	char *session;
	char *login_name;
	char *login_date;
	char *login_time;
	char *last_access_date;
	char *last_access_time;
	char *http_user_agent;
	char *remote_ip_address;
} SESSION;

SESSION *session_calloc(
			void );

int session_access_folder(
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *permission );

void session_access_failed_message_and_exit(
			char *application_name,
			char *session_key,
			char *login_name );

int session_get_session_folder_access_ok(
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *permission );

SESSION *session_parse(	char *input );

int session_load(
			char **login_name,
			char **last_access_date,
			char **last_access_time,
			char **http_user_agent,
			char *application_name,
			char *session_key );

boolean session_access(	char *application_name,
			char *session,
			char *login_name );

int session_access_process(
			char *application_name,
			char *session,
			char *process_name,
			char *login_name,
			char *role_name );

int session_get_session_process_access_ok(
			char *application_name,
			char *login_name,
			char *session,
			char *process_name,
			char *role_name );

char *session_get_login_name(
			char *application_name,
			char *session );

char *session_get_http_user_agent(
			char *application_name,
			char *session );

boolean session_key_exists(
			char *application_name,
			char *session_key );

char *session_degrade_state(
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name );

void session_update_access_date_time(
			char *application_name,
			char *session );

boolean session_remote_ip_address_changed(
			char *application_name,
			char *session );

void session_message_ip_address_changed_exit(
			char *application_name,
			char *login_name );

LIST *session_list_fetch(
			char *login_name );

LIST *session_system_list(
			char *system_string );

/* Returns static memory */
/* --------------------- */
char *session_system_string(
			char *where );

#endif
