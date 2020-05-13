/* appaserver_user.h 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver user ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_USER_H
#define APPASERVER_USER_H

#include "boolean.h"
#include "list.h"

#define APPASERVER_USER_RECORD_DELIMITER	'^'

enum password_function	{	no_encryption,
				old_password_function,
				password_function,
				sha2_function };

typedef struct
{
	char *login_name;
	char *person_full_name;
	char *typed_in_password;
	char *database_password;
	char *user_date_format;
	enum password_function password_function;
	LIST *role_list;
	boolean frameset_menu_horizontal;
	LIST *session_list;
} APPASERVER_USER;

/* Prototypes */
/* ---------- */
enum password_function
	appaserver_user_password_function(
					char *database_password );

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_encryption_select(
					enum password_function,
					char *typed_in_password );

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_security_encrypted_password(
				char *application_name,
				char *typed_in_password,
				enum password_function );

LIST *appaserver_user_session_list(	char *application_name,
					char *login_name );

boolean appaserver_user_exists_session(	char *application_name,
					char *login_name,
					char *session );

boolean appaserver_user_exists_role(	char *application_name,
					char *login_name,
					char *role_name );

APPASERVER_USER *appaserver_user_fetch(
					char *application_name,
					char *login_name );

char *appaserver_user_person_full_name(
					char *application_name,
					char *login_name );

char *appaserver_user_password(		char *application_name,
					char *login_name );

boolean appaserver_user_frameset_menu_horizontal(
					char *application_name,
					char *login_name );

LIST *appaserver_user_role_list(
					char *application_name,
					char *login_name );

APPASERVER_USER *appaserver_user_calloc(void );

boolean appaserver_user_password_match(
					char *application_name,
					char *typed_in_password,
					char *database_password );

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_mysql_version(	void );

/* Returns heap memory. */
/* -------------------- */
char *appaserver_user_version_encrypted_password(
					char *application_name,
					char *typed_in_password,
					char *mysql_version );

enum password_function
	appaserver_user_version_password_function(
					char *mysql_version );

boolean appaserver_user_insert(		char *application_name,
					char *login_name,
					char *person_full_name,
					char *database_password,
					char *user_date_format );

FILE *appaserver_user_insert_open(	char *application_name,
					char *error_filename );

void appaserver_user_insert_stream(	FILE *output_pipe,
					char *login_name,
					char *person_full_name,
					char *database_password,
					char *user_date_format );

APPASERVER_USER *appaserver_user_parse(
					char *input_buffer );

#endif
