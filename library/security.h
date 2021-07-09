/* $APPASERVER_HOME/library/security.h			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef SECURITY_H
#define SECURITY_H

/* Includes */
/* -------- */
#include <stdio.h>
#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */
enum password_function	{	no_encryption,
				old_password_function,
				regular_password_function,
				sha2_function };

/* Prototypes */
/* ---------- */

boolean security_password_match(
			char *database_password,
			char *injection_escaped_encrypted_password );

enum password_function
	security_database_password_function(
			char *database_password );

/* Returns heap memory. */
/* -------------------- */
char *security_encrypted_password(
			char *application_name,
			char *password_sql_injection_escape,
			enum password_function );

enum password_function
	security_mysql_version_password_function(
			char *mysql_version );

/* Returns static memory. */
/* ---------------------- */
char *security_mysql_version(
			void );

/* Returns heap memory. */
/* -------------------- */
char *security_encryption_select_clause(
			enum password_function,
			char *typed_in_password );

boolean security_password_encrypted(
			char *password );

/* Returns data */
/* ------------ */
char *security_replace_special_characters(
			char *data );

/* Returns heap memory */
/* ------------------- */
char *security_sql_injection_escape(
			char *data );

/* Returns destination */
/* ------------------- */
char *security_escape_character_array(
			char *destination,
			char *source,
			char *character_array );

#endif
