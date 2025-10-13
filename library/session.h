/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/session.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SESSION_H
#define SESSION_H

#include "list.h"
#include "boolean.h"
#include "appaserver_user.h"

#define SESSION_TABLE				"session"
#define SESSION_REMOTE_IP_ADDRESS_VARIABLE	"REMOTE_ADDR"
#define SESSION_SIZE				10
#define SESSION_USER_AGENT_WIDTH		80
#define SESSION_SLEEP_SECONDS			2


#define SESSION_SELECT				"session,"		\
						"full_name,"		\
						"street_address,"	\
						"login_date,"		\
						"login_time,"		\
						"last_access_date,"	\
						"last_access_time,"	\
						"http_user_agent,"	\
						"remote_ip_address"

#define SESSION_INSERT				"session,"		\
						"full_name,"		\
						"street_address,"	\
						"login_date,"		\
						"login_time,"		\
						"last_access_date,"	\
						"last_access_time,"	\
						"http_user_agent,"	\
						"remote_ip_address"
typedef struct
{
	char *session_key;
	char *login_date;
	char *login_time;
	char *remote_ip_address;
	char *http_user_agent;
	APPASERVER_USER *appaserver_user;
	char *insert_string;
	char *insert_system_string;

	/* Fetch attributes */
	/* ---------------- */
	char *current_ip_address;
	boolean remote_ip_address_changed_boolean;
	char *last_access_date;
	char *last_access_time;
	char *update_sql;
	char *purge_temporary_files_system_string;
} SESSION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SESSION *session_new(
		char *application_name,
		char *login_name,
		boolean appaserver_user_boolean );

/* Process */
/* ------- */
SESSION *session_calloc(
		void );

/* Returns heap memory or exits */
/* ---------------------------- */
char *session_current_ip_address(
		char *environment_remote_ip_address );

/* Usage */
/* ----- */

/* If error then exit( 1 ) */
/* ----------------------- */
SESSION *session_fetch(
		char *application_name,
		char *session_key,
		char *login_name,
		boolean appaserver_user_boolean );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *session_primary_where(
		char *session_key );

/* Usage */
/* ----- */
SESSION *session_parse(
		boolean appaserver_user_boolean,
		char *string_fetch );

/* Usage */
/* ----- */

/* Returns heap memory */
/* --------------------*/
char *session_http_user_agent(
		const int session_user_agent_width /* probably 80 */,
		char *environment_http_user_agent );

/* Usage */
/* ----- */

/* Returns heap memory */
/* --------------------*/
char *session_login_date(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* --------------------*/
char *session_login_time(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* --------------------*/
char *session_last_access_date(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* --------------------*/
char *session_last_access_time(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *session_insert_string(
		char *session_key,
		char *login_date,
		char *login_time,
		char *http_user_agent,
		char *remote_ip_address,
		APPASERVER_USER *appaserver_user );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *session_insert_system_string(
		const char *session_table,
		const char *session_insert );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *session_purge_temporary_files_system_string(
		char *application_name );

/* Usage */
/* ----- */
void session_insert(
		char *session_insert_string,
		char *session_insert_system_string );

/* Returns heap memory */
/* ------------------- */
char *session_key(
		char *application_name );

/* Usage */
/* ----- */
boolean session_sql_injection_strcmp_okay(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name );

/* Usage */
/* ----- */
void session_sql_injection_message_exit(
		char *argv_0,
		char *environment_remote_ip_address );

/* Usage */
/* ----- */
void session_access_failed_message_exit(
		char *application_name,
		char *login_name,
		char *session_current_ip_address );

/* Usage */
/* ----- */
void session_environment_set(
		char *application_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *session_update_sql(
		const char *session_table,
		char *session_key,
		char *session_last_access_date,
		char *session_last_access_time );

/* Usage */
/* ----- */
boolean session_remote_ip_address_changed_boolean(
		char *session_parse_remote_ip_address,
		char *session_current_ip_address );

/* Usage */
/* ----- */
void session_message_ip_address_changed_exit(
		char *application_name,
		char *session_key,
		char *login_name,
		char *session_parse_remote_ip_address,
		char *session_current_ip_address );

/* Usage */
/* ----- */
void session_delete(
		const char *session_table,
		char *application_name,
		char *session_key );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *session_delete_sql(
		const char *session_table,
		char *session_key );

/* Usage */
/* ----- */

/* Returns directory_filename */
/* -------------------------- */
char *session_right_trim(
		char *directory_filename_session_suffix );

/* Usage */
/* ----- */
void session_folder_permission_message_exit(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *state,
		char *session_current_ip_address );

typedef struct
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *state;
	LIST *role_folder_list;
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
		char *state1,
		char *state2 );

/* Process */
/* ------- */
SESSION_FOLDER *session_folder_calloc(
		void );

boolean session_folder_valid(
		char *state1,
		char *state2,
		char *folder_name,
		LIST *role_folder_list );

typedef struct
{
	char *role_name;
	char *process_name;
	char *process_set_name;
	SESSION *session;
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
SESSION_PROCESS *session_process_calloc(
		void );

boolean session_process_valid(
		char *process_name,
		LIST *role_process_list,
		LIST *role_process_set_list );

#endif
