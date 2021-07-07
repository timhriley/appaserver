/* $APPASERVER_HOME/library/appaserver_user.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver user ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_USER_H
#define APPASERVER_USER_H

#include "boolean.h"
#include "list.h"

#define APPASERVER_USER_TABLE		"appaserver_user"
#define APPASERVER_USER_PRIMARY_KEY	"login_name"

enum password_function	{	no_encryption,
				old_password_function,
				regular_password_function,
				sha2_function };

typedef struct
{
	/* Input */
	/* ----- */
	char *login_name;
	char *full_name;
	char *typed_in_password;
	char *database_password;
	char *user_date_format;

	/* Process */
	/* ------- */
	char *encrypted_password;
	LIST *role_list;
	LIST *session_list;
	enum password_function password_function;
	boolean appaserver_user_password_encrypted;
} APPASERVER_USER;

/* Prototypes */
/* ---------- */
enum password_function
	appaserver_user_password_function(
			char *database_password );

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_encryption_select_clause(
			enum password_function,
			char *typed_in_password );

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_encrypted_password(
			char *application_name,
			char *typed_in_password,
			enum password_function );

LIST *appaserver_user_session_list(
			char *application_name,
			char *login_name );

boolean appaserver_user_exists_session(
			char *application_name,
			char *login_name,
			char *session );

boolean appaserver_user_exists_role(
			char *application_name,
			char *login_name,
			char *role_name );

APPASERVER_USER *appaserver_user_fetch(
			char *login_name,
			boolean fetch_role_list,
			boolean fetch_attribute_exclude_list,
			boolean fetch_session_list );

APPASERVER_USER *appaserver_user_parse(
			char *input,
			boolean fetch_role_list,
			boolean fetch_attribute_exclude_list,
			boolean fetch_session_list );

char *appaserver_user_person_full_name(
			char *login_name );

char *appaserver_user_password_fetch(
			char *login_name );

boolean appaserver_user_frameset_menu_horizontal(
			char *application_name,
			char *login_name );

LIST *appaserver_user_role_list(
			char *application_name,
			char *login_name );

APPASERVER_USER *appaserver_user_calloc(void );

boolean appaserver_user_password_match(
			char *database_password,
			char *injection_escaped_encrypted_password );

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_mysql_version(
			void );

enum password_function
	appaserver_user_mysql_version_password_function(
			char *mysql_version );

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

/* Returns static memory */
/* --------------------- */
char *appaserver_user_primary_where(
			char *login_name );

/* Returns program memory */
/* ---------------------- */
char *appaserver_user_select(
			void );

/* Returns heap memory */
/* ------------------- */
char *appaserver_user_system_string(
			char *where );

boolean appaserver_user_password_encrypted(
			char *password );

FILE *appaserver_user_update_open(
			void );

void appaserver_user_update(
			FILE *update_pipe,
			char *injection_escaped_password,
			char *login_name );
enum password_function
	appaserver_user_database_password_function(
			char *database_password );

#endif
