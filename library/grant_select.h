/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/grant_select.h 				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef GRANT_SELECT_H
#define GRANT_SELECT_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *login_host_name;
	char *flush_privileges_sql_statement;
	char *revoke_sql_statement;
	char *drop_user_sql_statement;
	char *security_generate_password;
	char *create_user_sql_statement;
	LIST *on_table_sql_statement_list;
	char *update_process_sql_statement;
	char *post_execute_message;
} GRANT_SELECT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRANT_SELECT *grant_select_new(
		char *application_name,
		char *login_name,
		char *connect_from_host,
		boolean revoke_only_boolean );

/* Process */
/* ------- */
GRANT_SELECT *grant_select_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *grant_select_flush_privileges_sql_statement(
		void );

/* Returns heap memory */
/* ------------------- */
char *grant_select_post_execute_message(
		char *application_name,
		char *connect_from_host,
		char *security_generate_password );

/* Driver */
/* ------ */
void grant_select_display(
		char *create_user_sql_statement,
		LIST *on_table_sql_statement_list,
		char *update_process_sql_statement,
		char *flush_privileges_sql_statement );

/* Driver */
/* ------ */
void grant_select_execute(
		char *create_user_sql_statement,
		LIST *on_table_sql_statement_list,
		char *update_process_sql_statement,
		char *flush_privileges_sql_statement );

/* Driver */
/* ------ */
void grant_select_revoke_display(
		char *revoke_sql_statement,
		char *drop_user_sql_statement,
		char *flush_privileges_sql_statement );

/* Driver */
/* ------ */
void grant_select_revoke_execute(
		char *revoke_sql_statement,
		char *drop_user_sql_statement,
		char *flush_privileges_sql_statement );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *grant_select_login_host_name(
			char *login_name,
		char *connect_from_host );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *grant_select_drop_user_sql_statement(
		char *grant_select_login_host_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *grant_select_create_user_sql_statement(
		char *grant_select_login_host_name,
		char *security_generate_password );

/* Usage */
/* ----- */
LIST *grant_select_on_table_sql_statement_list(
		char *application_name,
		char *grant_select_login_host_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *grant_select_on_table_sql_statement(
		char *application_name,
		char *grant_select_login_host_name,
		char *folder_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *grant_select_update_process_sql_statement(
		char *application_name,
		char *grant_select_login_host_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *grant_select_revoke_sql_statement(
		char *grant_select_login_host_name );
#endif
