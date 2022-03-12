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
	int next_session_number;
	int next_reference_number;
	char *background_color;
	char *distill_directory;
	char *ghost_script_directory;
	char *grace_home_directory;
	char *grace_execution_directory;
	boolean grace_free_option;
	char *grace_output;
	boolean frameset_menu_horizontal;
	boolean ssl_support;
	boolean prepend_http_protocol;
	char *chart_email_command_line;
	int max_query_rows_for_drop_downs;
	int max_drop_down_size;
} APPLICATION;

/* APPLICATION operations */
/* ---------------------- */

/* Usage */
/* ----- */
APPLICATION *application_fetch(
			char *application_name );

APPLICATION *application_new(
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
			char *where );

APPLICATION *application_parse(
			char *input );

/* Private */
/* ------- */
APPLICATION *application_calloc(
			void );

/* Public */
/* ------ */

char *application_table_name(
			char *folder_name );

/* Returns heap memory or "" */
/* ------------------------- */
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

boolean application_grace_free_option(
			char *application_name );

char *application_grace_output(
			char *application_name );

boolean application_frameset_menu_horizontal_yn(
			char *application_name );

boolean application_ssl_support(
			char *application_name );

char application_ssl_support_yn(
			char *application_name );

boolean application_prepend_http_protocol(
			char *application_name );

char application_prepend_http_protocol_yn(
			char *application_name );

char *application_chart_email_command_line(
			char *application_name );

int application_max_query_rows_for_drop_downs(
			char *application_name );

int application_max_drop_down_size(
			char *application_name );

char *application_first_relative_source_directory(
			char *application_name );

char *application_http_prefix(
			char *application_name );

void application_reset(	void );

#endif
