/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/recall.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RECALL_H
#define RECALL_H

#include "list.h"
#include "boolean.h"

#define RECALL_BUTTON_LABEL		"Recall"
#define RECALL_DELIMITER		'~'
#define RECALL_PROCESS_COOKIE_KEY	"process_cookie"

typedef struct
{
	char *cookie_key;
	char *keystrokes_javascript;
	char *cookie_multi_key;
	char *keystrokes_multi_javascript;
	char *javascript;
} RECALL_SAVE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RECALL_SAVE *recall_save_new(
		char *state,
		char *form_name,
		char *folder_name,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean );

/* Process */
/* ------- */
RECALL_SAVE *recall_save_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *recall_save_cookie_key(
		char *recall_process_cookie_key,
		char *folder_name,
		char *state );

/* Returns static memory */
/* --------------------- */
char *recall_save_cookie_multi_key(
		char *folder_name,
		char *state );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *recall_save_keystrokes_javascript(
		const char recall_delimiter,
		const char widget_multi_left_right_delimiter,
		char *form_name,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_key );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *recall_save_keystrokes_multi_javascript(
		const char recall_delimiter,
		const char widget_multi_left_right_delimiter,
		char *form_name,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_multi_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_save_keystrokes_open(
		char *form_name,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_save_keystrokes_close(
		char recall_delimiter,
		char widget_multi_left_right_delimiter );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_save_keystrokes_multi_open(
		char *form_name,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_multi_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_save_keystrokes_multi_close(
		char recall_delimiter,
		char widget_multi_left_right_delimiter );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *recall_save_samesite_string(
		boolean application_ssl_support_boolean );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *recall_save_secure_string(
		boolean application_ssl_support_boolean );

typedef struct
{
	char *recall_save_cookie_key;
	char *keystrokes_javascript;
	char *recall_save_cookie_multi_key;
	char *keystrokes_multi_javascript;
	char *javascript;
} RECALL_LOAD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RECALL_LOAD *recall_load_new(
		char *state,
		char *form_name,
		char *folder_name,
		LIST *widget_container_list );

/* Process */
/* ------- */
RECALL_LOAD *recall_load_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *recall_load_javascript(
		char *recall_load_keystrokes_javascript,
		char *recall_load_keystrokes_multi_javascript );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *recall_load_keystrokes_javascript(
		const char recall_delimiter,
		const char widget_multi_left_right_delimiter,
		char *form_name,
		LIST *widget_container_list,
		char *recall_save_cookie_key );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *recall_load_keystrokes_multi_javascript(
		const char recall_delimiter,
		const char widget_multi_left_right_delimiter,
		char *form_name,
		LIST *widget_container_list,
		char *recall_save_cookie_multi_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_load_keystrokes_open(
		char *form_name,
		char *recall_save_cookie_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_load_keystrokes_close(
		char recall_delimiter,
		char widget_multi_left_right_delimiter );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_load_keystrokes_multi_open(
		char *form_name,
		char *recall_save_cookie_multi_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_load_keystrokes_multi_close(
		char recall_delimiter,
		char widget_multi_left_right_delimiter );

typedef struct
{
	/* Stub */
} RECALL;

/* Usage */
/* ----- */
boolean recall_ignore_boolean(
		const char *appaserver_execute_yn,
		char *widget_name,
		enum widget_type widget_type,
		boolean recall_boolean,
		boolean multi_drop_down_boolean );

#endif
