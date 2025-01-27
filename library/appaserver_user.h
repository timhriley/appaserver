/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver_user.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
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
	boolean deactivated_boolean;

	/* Set externally */
	/* -------------- */

	/* Forbidden characters are \ and " */
	/* -------------------------------- */
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
		boolean person_full_name_exists );

/* Usage */
/* ----- */

/* Returns null if login_name not found */
/* ------------------------------------ */
APPASERVER_USER *appaserver_user_parse(
		boolean fetch_role_name_list,
		boolean full_name_exists,
		char *input );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_user_primary_where(
		char *login_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_USER *appaserver_user_new(
		char *login_name );

/* Process */
/* ------- */
APPASERVER_USER *appaserver_user_calloc(
		void );

/* Usage */
/* ----- */
void appaserver_user_update(
		char *password,
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

/* Usage */
/* ----- */

/* Returns heap memory or program memory */
/* ------------------------------------- */
char *appaserver_user_date_format_string(
		char *application_name,
		char *login_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_user_date_convert_string(
		char *appaserver_user_date_format_string,
		char *international_date_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_user_person_full_name(
		char *login_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_user_default_role_name(
		char *login_name );

/* Usage */
/* ----- */
LIST *appaserver_user_role_name_list(
		char *login_name );

#endif
