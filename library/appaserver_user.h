/* $APPASERVER_HOME/library/appaserver_user.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_USER_H
#define APPASERVER_USER_H

#include "boolean.h"
#include "list.h"
#include "security.h"

#define APPASERVER_USER_TABLE		"appaserver_user"
#define APPASERVER_USER_PRIMARY_KEY	"login_name"

typedef struct
{
	char *login_name;
	char *full_name;
	boolean frameset_menu_horizontal_exists;
	boolean frameset_menu_horizontal;
	char *typed_in_password;
	char *database_password;
	char *encrypted_password;
	char *user_date_format;
	LIST *role_name_list;
	LIST *session_list;
	enum password_function password_function;
	boolean appaserver_user_password_encrypted;
} APPASERVER_USER;

/* Usage */
/* ----- */
APPASERVER_USER *appaserver_user_fetch(
			char *login_name,
			boolean fetch_role_name_list,
			boolean fetch_session_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *appaserver_user_select(
			boolean full_name_exists,
			boolean person_full_name_exists,
			boolean frameset_menu_horizontal_exists );

/* Usage */
/* ----- */
APPASERVER_USER *appaserver_user_parse(
			char *input,
			boolean fetch_role_name_list,
			boolean fetch_session_list,
			boolean full_name_exists,
			boolean frameset_menu_horizontal_exists );

/* Process */
/* ------- */
APPASERVER_USER *appaserver_user_calloc(
			void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_user_primary_where(
			char *login_name );

/* Process */
/* ------- */

enum password_function
	appaserver_user_password_function(
			char *database_password );

/* Public */
/* ------ */
boolean appaserver_user_exists_session(
			char *login_name,
			char *session_key );

boolean appaserver_user_exists_role(
			char *login_name,
			char *role_name );

char *appaserver_user_person_full_name(
			char *login_name );

char *appaserver_user_password_fetch(
			char *login_name );

boolean appaserver_user_frameset_menu_horizontal(
			char *login_name );

LIST *appaserver_user_role_name_list(
			char *login_name );

boolean appaserver_user_insert(
			char *application_name,
			char *login_name,
			char *database_password,
			char *person_full_name,
			char *frameset_menu_horizontal_yn,
			char *user_date_format );

FILE *appaserver_user_insert_open(
			char *application_name,
			char *error_filename,
			char *frameset_menu_horiontal_yn );

void appaserver_user_insert_stream(
			FILE *output_pipe,
			char *login_name,
			char *person_full_name,
			char *database_password,
			char *user_date_format,
			char *frameset_menu_horizontal_yn );

/* Returns heap memory */
/* ------------------- */
char *appaserver_user_system_string(
			char *appaserver_user_select,
			char *appaserver_user_table,
			char *where );

FILE *appaserver_user_update_open(
			void );

void appaserver_user_update(
			FILE *update_pipe,
			char *injection_escaped_password,
			char *login_name );

char *appaserver_user_default_role_name(
			char *login_name );

#endif
