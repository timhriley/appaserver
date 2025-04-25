/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_login.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef POST_LOGIN_H
#define POST_LOGIN_H

#include "dictionary.h"
#include "document.h"
#include "security.h"
#include "session.h"
#include "appaserver_user.h"

#define POST_LOGIN_RETURN_USERNAME	"timriley"
#define POST_LOGIN_MAIL_FILENAME	"/etc/mailname"
#define POST_LOGIN_SUBJECT		"Appaserver login"
#define POST_LOGIN_SLEEP_SECONDS	3

#define POST_LOGIN_EMAIL_OUTPUT_TEMPLATE \
				"%s/appaserver_data/%s/post_login_%s.html"

#define POST_LOGIN_EMAIL_HTTP_TEMPLATE	\
				"%s://%s/appaserver_data/%s/post_login_%s.html"

enum post_login_document_location_enum {
		location_html,
		location_php,
		location_website };

typedef struct
{
	DICTIONARY *dictionary;
	char *application_name;
	char *remote_ip_address;
	boolean missing_application_boolean;
	boolean invalid_application_boolean;
	boolean application_exists_boolean;
	char *login_name;
	boolean missing_login_name_boolean;
	boolean signup_boolean;
	boolean email_address_boolean;
	boolean invalid_login_name_boolean;
	APPASERVER_USER *appaserver_user;
	boolean user_not_found_boolean;
	boolean public_boolean;
	boolean deactivated_boolean;
	char *form_password;
	boolean form_password_blank_boolean;
} POST_LOGIN_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_LOGIN_INPUT *post_login_input_new(
		int argc,
		char **argv );

/* Process */
/* ------- */
POST_LOGIN_INPUT *post_login_input_calloc(
		void );

/* --------------------------- */
/* Returns heap memory or null */
/* --------------------------- */
char *post_login_input_remote_ip_address(
		void );

DICTIONARY *post_login_input_dictionary(
		void );

/* ------------------------------------------------------------ */
/* Returns component of argv, component of dictionary, or null. */
/* ------------------------------------------------------------ */
char *post_login_input_application_name(
		int argc,
		char **argv,
		DICTIONARY *post_login_input_dictionary );

boolean post_login_input_missing_application_boolean(
		char *post_login_input_application_name );

boolean post_login_input_invalid_application_boolean(
		char *post_login_input_application_name );

boolean post_login_input_missing_login_name_boolean(
		char *post_login_input_login_name );

boolean post_login_input_invalid_login_name_boolean(
		char *post_login_input_login_name );

boolean post_login_input_email_address_boolean(
		char *post_login_input_login_name );

boolean post_login_input_user_not_found_boolean(
		APPASERVER_USER *appaserver_user );

boolean post_login_input_public_boolean(
		char *post_login_name );

boolean post_login_input_deactivated_boolean(
		char *database_password,
		boolean appaserver_user_deactivated_boolean,
		boolean post_login_input_public_boolean );

/* Returns component of post_login_dictionary or null */
/* -------------------------------------------------- */
char *post_login_input_form_password(
		DICTIONARY *post_login_input_dictionary );

boolean post_login_input_form_password_blank_boolean(
		char *post_login_input_form_password );

/* Usage */
/* ----- */

/* ------------------------------------------------------------ */
/* Returns component of argv, component of dictionary, or null. */
/* ------------------------------------------------------------ */
char *post_login_input_application_name(
		int argc,
		char **argv,
		DICTIONARY *post_login_dictionary );

/* Usage */
/* ----- */

/* ----------------------------------------- */
/* Returns component of dictionary, or null. */
/* ----------------------------------------- */
char *post_login_input_login_name(
		const char *appaserver_user_login_name,
		DICTIONARY *post_login_input_dictionary );

/* Usage */
/* ----- */
boolean post_login_input_application_exists_boolean(
		char *post_login_input_application_name,
		char *appaserver_parameter_log_directory );

typedef struct
{
	char *filename;
	enum post_login_document_location_enum location_enum;
	char *application_name;
	char *filename_location;
	char *application_title_string;
	DOCUMENT *document;
	char *document_body_open_tag;
	char *document_both_close_tag;
	char *html;
} POST_LOGIN_DOCUMENT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_LOGIN_DOCUMENT *post_login_document_new(
		DICTIONARY *post_login_input_dictionary,
		char *post_login_input_application_name,
		char *index_html_parameter );

/* Process */
/* ------- */
POST_LOGIN_DOCUMENT *post_login_document_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
char *post_login_document_application_name(
		const char *application_template_name,
		char *post_login_input_application_name );

/* Usage */
/* ----- */

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *post_login_document_filename(
		DICTIONARY *post_login_input_dictionary );

/* Usage */
/* ----- */
enum post_login_document_location_enum
	post_login_document_location_enum_resolve(
		char *post_login_document_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_login_document_filename_location(
		char *post_login_input_application_name,
		char *index_html_parameter,
		enum post_login_document_location_enum location_enum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_login_document_html(
		enum post_login_document_location_enum location_enum,
		char *post_login_document_filename_location,
		char *document_html,
		char *document_body_open_tag,
		char *document_close_tag );

typedef struct
{
	SESSION *session;
	char *output_pipe_string;
	char *appaserver_error_filename;
	char *execute_system_string_frameset;
	char *return_address;
	char *email_link_url;
	char *sendmail_message;
	char *sendmail_filename;
	char *email_system_string;
	char *index_html_parameter;
} POST_LOGIN_SUCCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_LOGIN_SUCCESS *post_login_success_new(
		char *post_login_input_application_name,
		char *post_login_input_login_name,
		boolean post_login_input_form_password_blank_boolean,
		boolean post_login_input_email_address_boolean );

/* Process */
/* ------- */
POST_LOGIN_SUCCESS *post_login_success_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *post_login_success_output_pipe_string(
		const char *post_login_email_output_template,
		char *post_login_input_application_name,
		boolean post_login_input_form_password_blank_boolean,
		boolean post_login_input_email_address_boolean,
		char *appaserver_parameter_document_root,
		char *session_key );

/* Returns static memory */
/* --------------------- */
char *post_login_success_return_address(
		const char *post_login_return_username,
		char *appaserver_mailname );

/* Returns static memory */
/* --------------------- */
char *post_login_success_email_link_url(
		const char *post_login_email_http_template,
		char *post_login_input_application_name,
		char *session_key );

/* Returns static memory */
/* --------------------- */
char *post_login_success_sendmail_message(
		const char *post_login_subject,
		char *post_login_input_login_name,
		char *post_login_success_return_address,
		char *post_login_success_email_link_url );

/* Returns static memory */
/* --------------------- */
char *post_login_success_sendmail_filename(
		char *appaserver_parameter_data_directory,
		pid_t process_id );

/* Returns heap memory */
/* ------------------- */
char *post_login_success_email_system_string(
		char *post_login_success_sendmail_message,
		char *post_login_success_sendmail_filename );

/* Returns program memory */
/* ---------------------- */
char *post_login_success_index_html_parameter(
		void );

typedef struct
{
	POST_LOGIN_INPUT *post_login_input;
	char *security_encrypt_password;
	boolean password_fail_boolean;
	char *reject_index_html_parameter;
	POST_LOGIN_DOCUMENT *post_login_document;
	POST_LOGIN_SUCCESS *post_login_success;
} POST_LOGIN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_LOGIN *post_login_new(
		int argc,
		char **argv );

/* Process */
/* ------- */
POST_LOGIN *post_login_calloc(
		void );

boolean post_login_password_fail_boolean(
		boolean email_address_boolean,
		char *database_password,
		boolean form_password_blank_boolean,
		char *post_login_security_encrypt_password );

/* Returns program memory or null */
/* ------------------------------ */
char *post_login_reject_index_html_parameter(
		boolean post_login_input_missing_application_boolean,
		boolean post_login_input_invalid_application_boolean,
		boolean post_login_input_missing_login_name_boolean,
		boolean post_login_input_invalid_login_name_boolean,
		boolean application_not_exists_boolean,
		boolean post_login_input_user_not_found_boolean,
		boolean post_login_input_deactivated_boolean,
		boolean post_login_password_fail_boolean );

/* Usage */
/* ----- */

/* Returns database_password or heap memory */
/* ---------------------------------------- */
char *post_login_security_encrypt_password(
		char *form_password,
		char *database_password );

#endif
