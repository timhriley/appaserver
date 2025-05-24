/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/security.h			   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SECURITY_H
#define SECURITY_H

#include <stdio.h>
#include "boolean.h"
#include "list.h"

/* Leave out the percent sign b/c mysql will save the backslash. */
/* ------------------------------------------------------------- */
#define SECURITY_ESCAPE_CHARACTER_STRING	"~!`'$;&=()\""

#define SECURITY_FORBID_CHARACTER_STRING	"~!`$;&=()\""
#define SECURITY_FORK_CHARACTER			'`'
#define SECURITY_FORK_STRING			"$("

enum password_function	{	no_encryption,
				old_password_function,
				regular_password_function,
				sha2_function };

typedef struct
{
	/* Stub */
} SECURITY;

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *security_sql_injection_escape(
		const char *security_escape_character_string,
		char *datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *security_escape(
		char *datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *security_sql_injection_unescape(
		const char *security_escape_character_string,
		char *datum );

/* Usage */
/* ----- */

/* Returns datum */
/* ------------- */
char *security_replace_special_characters(
		char *datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *security_sql_injection_escape_quote_delimit(
		char *datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *security_generate_password(
		void );

/* Usage */
/* ----- */
unsigned long security_random(
		unsigned long up_to );

/* Usage */
/* ----- */

/* Returns heap memory or datum */
/* ---------------------------- */
char *security_remove_semicolon(
		char *datum );

/* Usage */
/* ----- */
boolean security_forbid_boolean(
		const char *security_forbid_character_string,
		char *string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *security_encrypt_password(
		char *post_login_password,
		enum password_function );

/* Usage */
/* ----- */
boolean security_password_match(
		char *database_password,
		char *security_encrypt_password );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *security_encrypt_select_clause(
		enum password_function,
		char *post_login_password );

/* Usage */
/* ----- */
void security_system(
		const char security_fork_character,
		const char *security_fork_string,
		char *system_string );

/* Process */
/* ------- */
boolean security_system_string_valid_boolean(
		const char security_fork_character,
		const char *security_fork_string,
		char *system_string );

/* Usage */
/* ----- */
FILE *security_read_pipe(
		char *system_string );

/* Usage */
/* ----- */
enum password_function
	security_password_function(
		char *database_password );

/* Public */
/* ------ */
enum password_function
	security_mysql_version_password_function(
		char *mysql_version );

/* Returns static memory. */
/* ---------------------- */
char *security_mysql_version(
		void );

boolean security_password_encrypted(
		char *password );

LIST *security_sql_injection_escape_list(
		LIST *data_list );

#endif
