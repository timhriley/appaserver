/* $APPASERVER_HOME/library/post_login.h		*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#ifndef POST_LOGIN_H
#define POST_LOGIN_H

/* Includes */
/* -------- */
#include "security.h"

/* Constants */
/* --------- */
#define SECONDS_TO_SLEEP		2
#define CLOUDACUS_LOCATION		"https://cloudacus.com:/index.php"
#define RETURN_ADDRESS			"login@cloudacus.com"
#define SUBJECT				"Appaserver login"

#define EMAIL_OUTPUT_FILE_TEMPLATE "%s/appaserver/%s/data/post_login_%s.html"
#define EMAIL_HTTP_FILE_TEMPLATE   "%s://%s/appaserver/%s/data/post_login_%s.html"

/* Enumerated types */
/* ---------------- */
enum post_login_password_match_return {
			password_match,
			password_fail,
			public_login,
			email_login };

/* Structures */
/* ---------- */
typedef struct
{
	DICTIONARY *post_login_dictionary;
	char *post_login_application_name;
	char *post_login_sql_injection_escape_name;
	boolean post_login_missing_name;
	boolean post_login_public_name;
	char *post_login_sql_injection_escape_password;
	char *post_login_database_password;
	boolean post_login_missing_database_password;
	enum post_login_password_match_return post_login_password_match_return;
} POST_LOGIN;

/* Operations */
/* ---------- */
boolean post_login_missing_name(
			char *post_login_name );

POST_LOGIN *post_login_new(
			int argc,
			char **argv );

DICTIONARY *post_login_dictionary(
			void );

char *post_login_application_name(
			int argc,
			char **argv,
			DICTIONARY *post_dictionary );

char *post_login_sql_injection_escape_name(
			DICTIONARY *post_login_dictionary );

char *post_login_database_password(
			char *application_name,
			char *login_name );

boolean post_login_email_login(
			char *login_name,
			char *email_http_filename );

void post_login_redraw_index_screen(
			char *application_name,
			char *location,
			char *message );

enum password_match_return post_login_password_match(
			char *application_name,
			char *post_login_sql_injection_escape_name,
			boolean post_login_public_name,
			char *post_login_sql_injection_escape_password,
			char *database_password );

void post_login_output_frameset(
			char *login_name,
			char *session );

boolean post_login_missing_database_password(
			char *database_password );

boolean post_login_public_name(
			char *login_name );

char *post_login_session(
			char *application_name,
			char *login_name );

char *post_login_session(
			char *application_name,
			char *login_name )
#endif
