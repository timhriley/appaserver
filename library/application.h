/* $APPASERVER_HOME/library/application.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "boolean.h"

/* Constants */
/* --------- */
#define APPLICATION_SELECT			\
		"application_name,"		\
		"application_title,"		\
		"appaserver_version,"		\
		"database_date_format,"		\
		"user_date_format,"		\
		"relative_source_directory,"	\
		"next_session_number,"		\
		"next_reference_number,"	\
		"background_color,"		\
		"distill_directory,"		\
		"ghost_script_directory,"	\
		"grace_home_directory,"		\
		"grace_execution_directory,"	\
		"grace_free_option_yn,"		\
		"grace_output,"			\
		"frameset_menu_horizontal_yn,"	\
		"ssl_support_yn,"		\
		"prepend_http_protocol_yn,"	\
		"chart_email_command_line,"	\
		"max_query_rows_for_drop_downs,"\
		"max_drop_down_size"

typedef struct
{
	char *application_name;
	char *application_title;
	char *appaserver_version;
	char *database_date_format;
	char *user_date_format;
	char *relative_source_directory;
	char *next_session_number;
	char *next_reference_number;
	char *background_color;
	char *distill_directory;
	char *ghost_script_directory;
	char *grace_home_directory;
	char *grace_execution_directory;
	char grace_free_option_yn;
	char *grace_output;
	char frameset_menu_horizontal_yn;
	char ssl_support_yn;
	char prepend_http_protocol_yn;
	char *chart_email_command_line;
	int max_query_rows_for_drop_downs;
	int max_drop_down_size;
} APPLICATION;

/* Prototypes */
/* ---------- */
APPLICATION *application_calloc(
			void );

APPLICATION *application_new(
			char *application_name );

char *application_title_string(
			char *application_name );

char *application_title(char *application_name );

char *application_relative_source_directory(
			char *application_name );

char *application_background_color(
			char *application_name );

char *application_distill_directory(
			char *application_name );

char *application_ghost_script_directory(
			char *application_name );

char *application_grace_home_directory(
			char *application_name );

char *application_grace_execution_directory(
			char *application_name );

char application_grace_free_option_yn(
			char *application_name );

char *application_grace_output(
			char *application_name );

char application_ssl_support_yn(
			char *application_name );

char application_prepend_http_protocol_yn(
			char *application_name );

char application_prepend_http_protocol_yn(
			char *application_name );

int application_max_drop_down_size(
			char *application_name );

int application_max_query_rows_for_drop_downs(
			char *application_name );

char application_frameset_menu_horizontal_yn(
			char *application_name );

char *application_chart_email_command_line(
			char *application_name );

char *application_version(
			char *application_name );

void application_reset(	void );

char *application_http_prefix(
			char *application_name );

char *application_http_prefix(
			char *application_name );

char *application_first_relative_source_directory(
			char *application_name );

char application_ssl_support_yn(
			char *application_name );

/* Safely returns heap memory */
/* -------------------------- */
char *application_primary_where(
			char *application_name );

/* Safely returns heap memory */
/* -------------------------- */
char *application_system_string(
			char *where );

APPLICATION *application_parse(
			char *input );

APPLICATION *application_fetch(
			void );

char *application_table_name(
			char *folder_name );

char *application_database_date_format(
			void );

#endif
