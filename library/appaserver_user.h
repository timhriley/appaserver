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

#define APPASERVER_USER_SELECT		"full_name,"		\
					"street_address,"	\
					"login_name,"		\
					"password,"		\
					"user_date_format,"	\
					"deactivated_yn"

#define APPASERVER_USER_LOGIN_NAME	"login_name"

typedef struct
{
	char *full_name;
	char *street_address;
	char *login_name;
	char *password;
	char *user_date_format;
	boolean deactivated_boolean;
	LIST *role_appaserver_user_name_list;

	/* Set externally */
	/* -------------- */

	/* Forbidden characters are \ and " */
	/* -------------------------------- */
	char *typed_in_password;

	char *encrypted_password;
	enum password_function password_function;
	boolean appaserver_user_password_encrypted;
} APPASERVER_USER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_USER *appaserver_user_login_fetch(
		char *login_name,
		boolean fetch_role_name_list );

/* Usage */
/* ----- */
APPASERVER_USER *appaserver_user_parse(
		boolean fetch_role_name_list,
		char *input );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_user_login_where(
		const char *appaserver_user_login_name,
		char *login_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_USER *appaserver_user_new(
		char *full_name,
		char *street_address );

/* Process */
/* ------- */
APPASERVER_USER *appaserver_user_calloc(
		void );

/* Usage */
/* ----- */
APPASERVER_USER *appaserver_user_fetch(
		char *full_name,
		char *street_address,
		boolean fetch_role_name_list );

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
LIST *appaserver_user_role_name_list(
		char *login_name );

/* Usage */
/* ----- */
/* --------- */
/* To retire */
/* --------- */
void appaserver_user_update(
		const char *appaserver_user_table,
		char *password,
		char *login_name );

/* Process */
/* ------- */
FILE *appaserver_user_update_open(
		const char *appaserver_user_table,
		const char *appaserver_user_login_name );

void appaserver_user_update_pipe(
		FILE *update_open,
		char *injection_escaped_password,
		char *login_name );

#endif
