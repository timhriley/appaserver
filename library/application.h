/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "boolean.h"
#include "list.h"

#define APPLICATION_TEMPLATE_NAME	"template"
#define APPLICATION_ADMIN_NAME		"admin"

#define APPLICATION_TABLE		"appaserver_application"

#define APPLICATION_SELECT			\
		"application,"			\
		"application_title,"		\
		"user_date_format,"		\
		"relative_source_directory,"	\
		"next_session_number,"		\
		"next_reference_number,"	\
		"background_color,"		\
		"grace_home_directory,"		\
		"grace_execution_directory,"	\
		"menu_horizontal_yn,"		\
		"ssl_support_yn,"		\
		"max_query_rows_for_drop_downs,"\
		"max_drop_down_size"

#define APPLICATION_UMASK			00007
#define APPLICATION_TRANSMIT_PROMPT		"Link to file."

typedef struct
{
	char *application_name;
	char *application_title;
	char *user_date_format;
	char *relative_source_directory;
	int next_session_number;
	int next_reference_number;
	char *background_color;
	char *grace_home_directory;
	char *grace_execution_directory;
	boolean menu_horizontal_boolean;
	boolean ssl_support_boolean;
	int max_query_rows_for_drop_downs;
	int max_drop_down_size;
} APPLICATION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION *application_fetch(
		char *application_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *application_primary_where(
		char *application_name );

/* Returns heap memory */
/* ------------------- */
char *application_system_string(
		char *application_select,
		char *application_table_name,
		char *where );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION *application_parse(
		char *input );

/* Usage */
/* ----- */
APPLICATION *application_new(
		char *application_name );

APPLICATION *application_calloc(
		void );

/* Usage */
/* ----- */
boolean application_name_invalid(
		char *security_escape_character_string,
		char *application_name );

/* Usage */
/* ----- */
boolean application_exists_boolean(
		char *application_log_filename );

/* Process */
/* ------- */
boolean application_file_exists_boolean(
		char *filename );

/* Usage */
/* ------ */
boolean application_menu_horizontal_boolean(
		char *application_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_table_name(
		char *folder_name );

/* Usage */
/* ----- */
LIST *application_name_list(
		char *application_log_extension,
		char *appaserver_parameter_log_directory );

/* Usage */
/* ----- */
LIST *application_table_name_list(
		void );

/* Usage */
/* ----- */
LIST *application_non_system_folder_name_list(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_email_file_system_string(
		char *subject,
		char *filename,
		char *email_address );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_server_address(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_http_prompt(
		char *cgi_directory,
	 	char *server_address,
		boolean ssl_support_boolean );

/* Usage */
/* ----- */
LIST *application_relative_source_directory_list(
		char *application_name );

/* Usage */
/* ----- */
LIST *application_not_null_data_list(
		char *table_name,
		char *column_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *application_not_null_system(
		char *table_name,
		char *column_name );

/* Public */
/* ------ */

/* Returns heap memory or "" */
/* ------------------------- */
char *application_title_string(
		char *application_name );

char *application_title(
		char *application_name );

/* Returns heap memory or "" */
/* ------------------------- */
char *application_relative_source_directory(
		char *application_name );

/* Returns heap memory or "" */
/* ------------------------- */
char *application_background_color(
		char *application_name );

/* Returns heap memory or "" */
/* ------------------------- */
char *application_grace_home_directory(
		char *application_name );

/* Returns heap memory or "" */
/* ------------------------- */
char *application_grace_execution_directory(
		char *application_name );

boolean application_ssl_support_boolean(
		char *application_name );

int application_max_query_rows_for_drop_downs(
		char *application_name );

int application_max_drop_down_size(
		char *application_name );

/* Returns heap memory */
/* ------------------- */
char *application_first_relative_source_directory(
		char *application_name );

/* Returns program memory */
/* ---------------------- */
char *application_http_prefix(
		boolean application_ssl_support_boolean );

void application_output_ftp_prompt(
		char *prompt_filename,
		char *prompt,
		char *target,
		char *mime_type );

#endif
