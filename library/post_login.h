/* $APPASERVER_HOME/library/post_login.h		*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#ifndef POST_LOGIN_H
#define POST_LOGIN_H

/* Includes */
/* -------- */
#include "dictionary.h"
#include "security.h"

/* Constants */
/* --------- */
#define POST_LOGIN_SECONDS_TO_SLEEP	2

#define POST_LOGIN_RETURN_ADDRESS	\
					"login@cloudacus.com"

#define POST_LOGIN_SUBJECT		\
					"Appaserver login"

#define POST_LOGIN_EMAIL_OUTPUT_TEMPLATE \
				"%s/appaserver/%s/data/post_login_%s.html"

#define POST_LOGIN_EMAIL_HTTP_TEMPLATE	\
				"%s://%s/appaserver/%s/data/post_login_%s.html"

#define POST_LOGIN_EMPTY_EXECUTABLE	"create_empty_application"

#define POST_LOGIN_TEMPLATE_APPLICATION	"template"

/* Enumerated types */
/* ---------------- */
enum password_match_return {
			password_match,
			database_password_blank,
			password_fail,
			public_login,
			email_login,
			missing_name,
			signup,
			signup_duplicate };

typedef struct
{
	DICTIONARY *dictionary;
	char *sql_injection_escape_application_name;
	char *sql_injection_escape_login_name;
	boolean missing_name;
	char *sql_injection_escape_password;
	char *sql_injection_escape_signup_yn;
	char *sql_injection_escape_application_key;
	char *sql_injection_escape_application_title;
	boolean signup;
	boolean signup_duplicate;
	boolean public_name;
	char *database_password;
	boolean missing_database_password;
	boolean name_email_address;
	enum password_match_return password_match_return;
	char *ip_address;
	char *message;
	char *redraw_application_name;
	char *redraw_index_screen_string;
	char *session_key;
	char *output_pipe_string;
	char *frameset_output_system_string;
	char *email_link_system_string;
	char *empty_application_system_string;
} POST_LOGIN;

/* POST_LOGIN operations */
/* --------------------- */

/* Usage */
/* ----- */
POST_LOGIN *post_login_new(
			int argc,
			char **argv );

/* Process */
/* ------- */
POST_LOGIN *post_login_calloc(
			void );

DICTIONARY *post_login_dictionary(
			void );

char *post_login_application_name(
			int argc,
			char **argv,
			DICTIONARY *post_dictionary );

boolean post_login_missing_name(
			char *post_login_name );

boolean post_login_signup(
			char *sql_injection_escape_signup_yn,
			char *sql_injection_escape_application_key,
			char *sql_injection_escape_application_title );

boolean post_login_signup_duplicate(
			char *sql_injection_escape_application_key,
			char *appaserver_parameter_error_directory );

boolean post_login_public_name(
			char *login_name );

char *post_login_database_password(
			char *login_name );

boolean post_login_missing_database_password(
			char *database_password );

boolean post_login_name_email_address(
			char *login_name );

enum password_match_return post_login_password_match(
			boolean post_login_signup_duplicate,
			boolean post_login_signup,
			boolean post_login_missing_name,
			boolean missing_database_password,
			boolean name_email_address,
			boolean public_name,
			char *sql_injection_escape_password,
			char *database_password );

/* Returns heap memory */
/* ------------------- */
char *post_login_ip_address( void );

/* Returns program memory or null */
/* ------------------------------ */
char *post_login_message(
			enum password_match_return,
			boolean post_login_name_email_address );

/* Returns one of the parameters */
/* ----------------------------- */
char *post_login_redraw_application_name(
			char *sql_injection_escape_application_name,
			char *sql_injection_escape_application_key );

/* Returns heap memory */
/* ------------------- */
char *post_login_redraw_index_screen_string(
			char *post_login_redraw_application_name,
			char *post_login_ip_address,
			char *post_login_message,
			boolean appaserver_library_from_php );

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
char *post_login_email_link_system_string(
			char *post_login_email_http_template,
			char *post_login_return_address,
			char *post_login_subject,
			char *sql_injection_escape_application_name,
			char *session_key,
			char *sql_injection_escape_login_name );


/* Returns heap memory */
/* ------------------- */
char *post_login_empty_application_system_string(
			char *post_login_empty_executable,
			char *post_login_template_application,
			char *session_key,
			char *sql_injection_escape_login_name,
			char *sql_injection_escape_application_key,
			char *sql_injection_escape_application_title );

#endif
