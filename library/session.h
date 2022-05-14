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
#define SESSION_TABLE				"appaserver_sessions"
#define SESSION_REMOTE_IP_ADDRESS_VARIABLE	"REMOTE_ADDR"
#define SESSION_SIZE				10

#define SESSION_SELECT				"appaserver_session,"	\
						"login_name,"		\
						"login_date,"		\
						"login_time,"		\
						"last_access_date,"	\
						"last_access_time,"	\
						"http_user_agent,"	\
						"remote_ip_address"

/* Structures */
/* ---------- */
typedef struct
{
	/* Attributes */
	/* ---------- */
	char *application_name;
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
	char *current_ip_address;
	boolean remote_ip_address_changed;
} SESSION;

/* SESSION operations */
/* ------------------ */

/* Usage */
/* ----- */

/* If error then exit( 1 ) */
/* ----------------------- */
SESSION *session_fetch(
			char *application_name,
			char *session_key,
			char *login_name );

/* Process */
/* ------- */
SESSION *session_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *session_primary_where(
			char *session_key );

/* Returns heap memory */
/* ------------------- */
char *session_system_string(
			char *session_select,
			char *session_table,
			char *session_primary_where );

SESSION *session_parse(	char *input );

/* Returns heap memory or exits */
/* ---------------------------- */
char *session_current_ip_address(
			char *environment_remote_ip_address );

boolean session_remote_ip_address_changed(
			char *remote_ip_address,
			char *current_ip_address );

void session_message_ip_address_changed_exit(
			char *application_name,
			char *session_key,
			char *login_name,
			char *remote_ip_address,
			char *current_ip_address );

void session_update_access_date_time(
			char *session_key );

void session_environment_set(
			char *application_name );

void session_purge_temporary_files(
			char *application_name );

/* Public */
/* ------ */
boolean session_sql_injection_strcmp_okay(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name );

void session_sql_injection_message_exit(
			char *argv_0,
			char *environment_remote_ip_address );

void session_access_failed_message_exit(
			char *application_name,
			char *login_name,
			char *session_current_ip_address );

char *session_login_name(
			char *session_key );

LIST *session_system_list(
			char *session_system_string );

LIST *session_list_fetch(
			char *login_name );

/* Returns heap memory */
/* ------------------- */
char *session_key(	char *application_name );

void session_insert(
			char *session_key,
			char *login_name,
			char *date_now_yyyy_mm_dd_string,
			char *date_now_hhmm_string,
			char *http_user_agent,
			char *remote_ip_address );

void session_delete(	char *session_key );

typedef struct
{
	SESSION *session;
} SESSION_FOLDER;

/* Usage */
/* ----- */

/* --------------------------------------------- */
/* Sets appaserver environment and outputs argv. */
/* Each parameter is security inspected.	 */
/* Any error will exit(1).			 */
/* --------------------------------------------- */
SESSION_FOLDER *session_folder_integrity_exit(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state );

/* Process */
/* ------- */
SESSION_FOLDER *session_folder_calloc(
			void );

boolean session_folder_valid(
			char *state,
			LIST *role_folder_list );

typedef struct
{
	SESSION *session;
	char *security_sql_injection_escape_role_name;
	char *process_name;
	char *process_set_name;
} SESSION_PROCESS;

/* Usage */
/* ----- */

/* --------------------------------------------- */
/* Sets appaserver environment and outputs argv. */
/* Each parameter is security inspected.	 */
/* Any error will exit( 1 ).			 */
/* --------------------------------------------- */
SESSION_PROCESS *session_process_integrity_exit(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name );

/* Process */
/* ------- */
boolean session_process_valid(
			char *process_name,
			LIST *role_process_list,
			LIST *role_process_set_list );

/* Private */
/* ------- */
SESSION_PROCESS *session_process_calloc(
			void );

#endif
