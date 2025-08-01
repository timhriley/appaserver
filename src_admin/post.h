/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_H
#define POST_H

#include "boolean.h"
#include "list.h"
#include "insert.h"
#include "appaserver_parameter.h"
#include "email_address.h"

/*
#define POST_RETURN_USERNAME		"DoNotReply"
*/
#define POST_RETURN_USERNAME		"timriley"

#define POST_HTTP_USER_AGENT_SIZE	80
#define POST_TABLE			"post"
#define POST_CONFIRMATION_COLUMN	"confirmation_received_date"
#define POST_RANDOM_NUMBER_SYSTEM	"random.e 100000 1"

#define POST_SELECT			"IP_address,"			\
					"http_user_agent,"		\
					"random_number,"		\
					"confirmation_received_date"

#define POST_INSERT			"email_address,"		\
					"IP_address,"			\
					"http_user_agent,"		\
					"timestamp,"			\
					"random_number"

#define POST_DENY_EXECUTABLE		"/usr2/ufw/ufw_submit"
#define POST_SLEEP_SECONDS		60

typedef struct
{
	char *ip_address;
	char *http_user_agent;
	char *timestamp;
	int random_number;
	EMAIL_ADDRESS *email_address;
	char *ip_deny_system_string;
	char *insert_statement;
	char *confirmation_received_date;
} POST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST *post_new(	char *email_address );

/* Process */
/* ------- */
POST *post_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *post_ip_deny_system_string(
		const char *post_deny_executable,
		char *ip_address );

/* Returns heap memory */
/* ------------------- */
char *post_insert_statement(
		const char *post_table,
		const char *post_insert,
		const int post_http_user_agent_size,
		char *email_address,
		char *environment_remote_ip_address,
		char *environment_http_user_agent,
		char *post_timestamp,
		int post_random_number );

/* Usage */
/* ----- */
POST *post_fetch(
		char *email_address,
		char *timestamp );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *post_primary_where(
		char *email_address,
		char *timestamp );

/* Usage */
/* ----- */
POST *post_parse(
		char *email_address,
		char *timestamp,
		char *string_fetch );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_timestamp(
		void );

/* Usage */
/* ----- */
int post_random_number(
		const char *post_random_number_system );

/* Usage */
/* ----- */
void post_mailx(
		char *message,
		char *mailx_system_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_return_email(
		const char *post_return_username,
		char *appaserver_mailname );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_mailx_system_string(
		const char *subject,
		char *email_address,
		char *post_return_email );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_confirmation_update_sql(
		const char *post_table,
		const char *post_confirmation_column,
		char *email_address,
		char *timestamp_space );

/* Usage */
/* ----- */
boolean post_bot_boolean(
		int receive_random_number,
		int post_random_number );

typedef struct
{
	char *email_address;
	char *timestamp_spaceless;
	char *timestamp_space;
	int random_number;
	APPASERVER_PARAMETER *appaserver_parameter;
	char *appaserver_mailname;
	char *appaserver_error_filename;
} POST_RECEIVE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_RECEIVE *post_receive_new(
		const char *application_admin_name,
		const char *appaserver_mailname_filespecification,
		int argc,
		char **argv );

/* Process */
/* ------- */
POST_RECEIVE *post_receive_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *post_receive_timestamp_space(
		char *timestamp_spaceless );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_receive_url(
		const char *receive_executable,
		const char *website_domain_name,
		char *apache_cgi_directory,
		char *email_address,
		char *timestamp_space,
		int post_random_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *post_receive_timestamp_spaceless(
		char *timestamp_space );

#endif
