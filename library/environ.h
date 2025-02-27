/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/environ.h			   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef ENVIRON_H
#define ENVIRON_H

#include <sys/types.h>
#include <sys/stat.h>
#include "boolean.h"
#include "list.h"

#define ENVIRONMENT_REMOTE_KEY		"REMOTE_ADDR"
#define ENVIRONMENT_USER_AGENT_KEY	"HTTP_USER_AGENT"

/* Public */
/* ------ */
char *resolve_environment_variables(
		char *return_string,
		char *s );

/* Returns heap memory or null */
/* --------------------------- */
char *environment_get(
		char *variable_name );

void set_environment(
		char *environment,
		char *datum );

void environment_set(
		char *environment,
		char *datum );

void environment_database_set(
		char *database );

void environ_set_environment(
		char *environment,
		char *datum );

void set_path( 	char *path_to_add );

void add_path( 	char *path_to_add );

boolean environ_variable_exists(
		char *variable_name,
		char *looking_string );

boolean environ_name_to_value(
		char *variable_value,
		char *variable_name );

int environ_next_delimiter(
		char *looking_string );

boolean environ_browser_internet_explorer(
		void );

char *environ_http_referer(
		void );

char *environ_http_referer_filename(
		void );

void add_pwd_to_path(
		void );

void add_etc_to_path(
		void );

void environ_standard_unix_to_path(
		void );

void add_standard_unix_to_path(
		void );

void add_dot_to_path(
		void );

void environ_append_dot_to_path(
		void );

void environ_utility_to_path(
		void );

void environment_utility_path(
		void );

void add_utility_to_path(
		void );

void environ_src_appaserver_to_path(
		void );

void environ_src_system_to_path(
		void );

void add_src_appaserver_to_path(
		void );

void environ_appaserver_home(
		void );

void add_appaserver_home_to_environment(
		void );

void add_library_to_python_path(
		void );

void add_appaserver_home_to_python_path(
		void );

void add_python_library_path(
		void );

void environ_local_bin_to_path(
		void );

void environ_usr2_bin_to_path(
		void );

void add_local_bin_to_path(
		void );

void environ_relative_source_directory_to_path(
		char *application_name );

void add_relative_source_directory_to_path(
		char *application_name );

void environ_set_utc_offset(
		void );

char *environment_exit_application_name(
		char *argv_0 );

char *environ_exit_application_name(
		char *argv_0 );

char *environ_application_name(
		char *argv_0 );

char *environment_application(
		void );

char *environment_database(
		void );

void environ_output_application_shell(
		FILE *output_file );

char *environ_shell_snippet(
		void );

void environ_display(
		FILE *output_pipe );

/* Returns heap memory or null */
/* --------------------------- */
char *environment_application_name(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *environment_http_user_agent(
		const char *environment_user_agent_key );

/* Returns heap memory or null */
/* --------------------------- */
char *environment_remote_ip_address(
		const char *environment_remote_key );

void environ_umask(
		mode_t application_umask );

void environment_append_path(
		char *directory );

void environment_prepend_path(
		char *directory );

void environment_session_set(
		char *application_name );

#endif
