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
#define POST_LOGIN_SECONDS_TO_SLEEP		2

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
	char *sql_injection_escape_application_name;
	char *sql_injection_escape_login_name;
	char *sql_injection_escape_password;
	boolean missing_name;
	boolean public_name;
	boolean name_email_address;
	char *database_password;
	boolean missing_database_password;
	enum post_login_password_match_return password_match_return;
	char *session_key;
} POST_LOGIN;

/* Operations */
/* ---------- */
boolean post_login_missing_name(
			char *post_login_name );

/* Inserts into APPASERVER_SESSIONS */
/* -------------------------------- */
POST_LOGIN *post_login_new(
			int argc,
			char **argv );

DICTIONARY *post_login_dictionary(
			void );

char *post_login_application_name(
			int argc,
			char **argv,
			DICTIONARY *post_dictionary );

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
			boolean post_login_name_email_address,
			boolean post_login_public_name,
			char *sql_injection_escape_password,
			char *database_password );

boolean post_login_missing_database_password(
			char *database_password );

boolean post_login_public_name(
			char *login_name );

char *post_login_session_key(
			char *application_name,
			char *login_name );

boolean post_login_name_email_address(
			char *login_name );

void post_login_frameset_output(
			char *application_name,
			char *login_name,
			char *session_key );

void post_login_horizontal_frameset(
			char *title,
			FRAMESET *frameset );

void post_login_vertical_frameset(
			char *title,
			FRAMESET *frameset );

#endif
