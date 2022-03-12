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
#define POST_LOGIN_SECONDS_TO_SLEEP	2

#define POST_LOGIN_CLOUDACUS_LOCATION	\
					"https://cloudacus.com:/index.php"

#define POST_LOGIN_RETURN_ADDRESS	\
					"login@cloudacus.com"

#define POST_LOGIN_SUBJECT		\
					"Appaserver login"

#define POST_LOGIN_EMAIL_OUTPUT_TEMPLATE \
				"%s/appaserver/%s/data/post_login_%s.html"

#define POST_LOGIN_EMAIL_HTTP_TEMPLATE	\
				"%s://%s/appaserver/%s/data/post_login_%s.html"

/* Enumerated types */
/* ---------------- */
enum password_match_return {
			password_match,
			database_password_blank,
			password_fail,
			public_login,
			email_login,
			missing_name };

typedef struct
{
	DICTIONARY *dictionary;
	char *sql_injection_escape_application_name;
	char *sql_injection_escape_login_name;
	char *sql_injection_escape_password;
	boolean missing_name;
	boolean public_name;
	boolean name_email_address;
	char *database_password;
	boolean missing_database_password;
	enum password_match_return password_match_return;
	char *session_key;
	char *ip_address;
	char *location;
	char *message;
	char *output_pipe_string;
	char *frameset_output_system_string;
	char *redraw_index_screen_string;
	char *email_link_system_string;
} POST_LOGIN;

/* POST_LOGIN operations */
/* --------------------- */

POST_LOGIN *post_login_new(
			int argc,
			char **argv );

/* Process */
/* ------- */
DICTIONARY *post_login_dictionary(
			void );

char *post_login_application_name(
			int argc,
			char **argv,
			DICTIONARY *post_dictionary );

boolean post_login_missing_name(
			char *post_login_name );

boolean post_login_public_name(
			char *login_name );

char *post_login_database_password(
			char *login_name );

boolean post_login_missing_database_password(
			char *database_password );

boolean post_login_name_email_address(
			char *login_name );

enum password_match_return post_login_password_match(
			boolean post_login_missing_name,
			boolean missing_database_password,
			boolean name_email_address,
			boolean public_name,
			char *sql_injection_escape_password,
			char *database_password );

/* Returns heap memory */
/* ------------------- */
char *post_login_output_pipe_string(
			char *post_login_email_output_template,
			boolean post_login_name_email_address,
			char *sql_injection_escape_application_name,
			char *appaserver_parameter_document_root,
			char *session_key );

/* Returns heap memory */
/* ------------------- */
char *post_login_ip_address( void );

/* Returns post_login_cloudacus_location or null */
/* --------------------------------------------- */
char *post_login_location(
			char *post_login_cloudacus_location,
			char *post_login_ip_address );

/* Returns program memory or null */
/* ------------------------------ */
char *post_login_message(
			enum password_match_return );

/* Returns heap memory */
/* ------------------- */
char *post_login_redraw_index_screen_string(
			char *sql_injection_escape_application_name,
			char *post_login_location,
			char *post_login_message,
			boolean appaserver_library_from_php );

char *post_login_email_link_system_string(
			char *post_login_email_http_template,
			char *post_login_return_address,
			char *post_login_subject,
			char *sql_injection_escape_application_name,
			char *session_key,
			char *login_name );

/* Private */
/* ------- */
POST_LOGIN *post_login_calloc(
			void );

#endif
