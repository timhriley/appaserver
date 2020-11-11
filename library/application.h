/* $APPASERVER_HOME/library/application.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver APPLICATION ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "boolean.h"

#define APPLICATION_RECORD_DELIMITER	'^'

typedef struct
{
	char *application_name;
	char *application_title;
	/* char only_one_primary_yn; */
	char *relative_source_directory;
	char *background_color;
	char *distill_directory;
	char *ghost_script_directory;
	char *grace_home_directory;
	char *grace_execution_directory;
	char grace_free_option_yn;
	char *grace_output;
	char frameset_menu_horizontal_yn;
	char *chart_email_command_line;
	char ssl_support_yn;
	char prepend_http_protocol_yn;
	int max_query_rows_for_drop_downs;
	int max_drop_down_size;
	char *appaserver_version;
} APPLICATION;

/* Prototypes */
/* ---------- */
APPLICATION *application_new_application(	char *application_string );
char *application_get_title_string(		char *application_string );
char *application_get_application_title(	char *application_string );
char *application_get_relative_source_directory(char *application_string );
char *application_get_background_color(		char *application_string );
char *application_get_distill_directory(	char *application_string );
char *application_get_ghost_script_directory(	char *application_string );
char *application_get_grace_home_directory(	char *application_string );
char *application_get_grace_execution_directory(char *application_string );
char application_get_grace_free_option_yn(	char *application_string );
char *application_get_grace_output(		char *application_string );
char application_get_ssl_support_yn(		char *application_string );

char application_get_prepend_http_protocol_yn(
			char *application_string );

char application_prepend_http_protocol_yn(
			char *application_string );

int application_get_max_drop_down_size(		char *application_string );
int application_get_max_query_rows_for_drop_downs(
						char *application_string );

char application_get_frameset_menu_horizontal_yn(
						char *application_string );

boolean application_get_is_primary_application(	char *application_string );

boolean application_is_primary_application(	char *application_string );
char *application_get_chart_email_command_line(	char *application_string );

/*
boolean application_get_alternative_is_primary_application(
						char *application_string );
*/

char *application_get_version(			char *application_string );

void application_reset(				void );

char *application_get_http_prefix(
			char *application_string );

char *application_http_prefix(
			char *application_string );

char *application_get_first_relative_source_directory(
						char *application_string );

char application_ssl_support_yn(
			char *application_string );

#endif
