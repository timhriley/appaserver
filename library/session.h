/* $APPASERVER_HOME/library/session.h			*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef SESSION_H
#define SESSION_H

/* Includes */
/* -------- */
#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */
#define SESSION_TABLE_NAME			"appaserver_sessions"
#define SESSION_REMOTE_IP_ADDRESS_VARIABLE	"REMOTE_ADDR"
#define SESSION_MAX_SESSION_SIZE		10

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *sql_injection_escape_application_name;
	char *sql_injection_escape_session_key;

	/* Attributes */
	/* ---------- */
	char *session_key;
	char *login_name;
	char *login_date;
	char *login_time;
	char *last_access_date;
	char *last_access_time;
	char *http_user_agent;
	char *remote_ip_address;

	/* Process */
	/* ------- */
	char *session_current_ip_address;

	/* Generic */
	/* ------- */
	char *session_state_integrity;
} SESSION;

SESSION *session_calloc(
			void );

SESSION *session_fetch(
			/* Sets ENVIRONMENT_DATABASE */
			/* ------------------------- */
			char *sql_injection_escape_application_name,
			char *sql_injection_escape_session_key,
			char *integrity_check_login_name );

void session_access_failed_message_exit(
			char *application_name,
			char *current_ip_address,
			char *login_name );

SESSION *session_parse(	char *input );

void session_update_access_date_time(
			char *session_key );

boolean session_remote_ip_address_changed(
			char *remote_ip_address,
			char *current_ip_address );

void session_message_ip_address_changed_exit(
			char *application_name,
			char *session,
			char *login_name );

LIST *session_list_fetch(
			char *login_name );

LIST *session_system_list(
			char *system_string );

/* Returns heap memory */
/* ------------------- */
char *session_system_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *session_primary_where(
			char *session_key );

void session_message_ip_address_changed_exit(
			char *application_name,
			char *session_key,
			char *remote_ip_address,
			char *current_ip_address,
			char *login_name );

/* ----------------------------------------------	*/
/* Sets appaserver environment and outputs usage.	*/
/* Each parameter is security inspected.		*/
/* ----------------------------------------------	*/
SESSION *session_folder_integrity_exit(
			int argc,
			char **argv,
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *state );

/* Returns state, program memory, or null */
/* -------------------------------------- */
char *session_state_integrity(
			char *state,
			LIST *role_folder_list );

/* Returns heap memory or exits */
/* ---------------------------- */
char *session_current_ip_address(
			void );

#endif
