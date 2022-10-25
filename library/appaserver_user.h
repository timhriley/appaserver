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
	enum password_function password_function;
	boolean appaserver_user_password_encrypted;
} APPASERVER_USER;

/* Usage */
/* ----- */
APPASERVER_USER *appaserver_user_fetch(
			char *login_name,
			boolean fetch_role_name_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *appaserver_user_select(
			boolean full_name_exists,
			boolean person_full_name_exists,
			boolean frameset_menu_horizontal_exists );

/* Returns heap memory */
/* ------------------- */
char *appaserver_user_system_string(
			char *appaserver_user_select,
			char *appaserver_user_table,
			char *where );

/* Usage */
/* ----- */
APPASERVER_USER *appaserver_user_parse(
			char *input,
			boolean fetch_role_name_list,
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

/* Usage */
/* ----- */
void appaserver_user_update(
			char *database_password,
			char *login_name );

/* Process */
/* ------- */
FILE *appaserver_user_update_open(
			char *appaserver_user_table,
			char *appaserver_user_primary_key );

void appaserver_user_update_pipe(
			FILE *update_open,
			char *injection_escaped_password,
			char *login_name );

/* Public */
/* ------ */
char *appaserver_user_person_full_name(
			char *login_name );

char *appaserver_user_default_role_name(
			char *login_name );

boolean appaserver_user_frameset_menu_horizontal(
			char *login_name );

LIST *appaserver_user_role_name_list(
			char *login_name );

#endif
