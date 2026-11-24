/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/recall.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "widget.h"

#define RECALL_BUTTON_LABEL		"Recall"
#define RECALL_NAME_DELIMITER		'~'
#define RECALL_EQUAL_DELIMITER		"=="
#define RECALL_BLOCK_DELIMITER		"~~"
#define RECALL_MULTI_DELIMITER		"&&"
#define RECALL_MULTI_INDEX_DELIMITER	'|'
#define RECALL_MULTI_KEY_PREFIX		"multi_"
#define RECALL_PROCESS_COOKIE_KEY	"process_cookie"

typedef struct
{
	char *cookie_key;
	char *javascript;
	char *cookie_multi_key;
	char *multi_javascript;
	char *apply_javascript;
} RECALL_SAVE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RECALL_SAVE *recall_save_new(
		char *state,
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
		const char *recall_process_cookie_key,
		char *folder_name,
		char *state );

/* Returns static memory */
/* --------------------- */
char *recall_save_cookie_multi_key(
		const char *recall_multi_key_prefix,
		const char *recall_process_cookie_key,
		char *folder_name,
		char *state );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *recall_save_javascript(
		const char recall_name_delimiter /* Probably , */,
		const char *recall_equal_delimiter /* Probably == */,
		const char *recall_block_delimiter /* Probably ~~ */,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_key );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *recall_save_multi_javascript(
		const char recall_name_delimiter,
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		const char *recall_multi_delimiter,
		const char recall_multi_index_delimiter,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_multi_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_save_open(
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_save_close(
		const char recall_name_delimiter,
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_save_multi_open(
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_multi_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_save_multi_close(
		const char recall_name_delimiter,
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		const char *recall_multi_delimiter,
		const char recall_multi_index_delimiter );

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

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *recall_save_display(
		RECALL_SAVE *recall_save );

typedef struct
{
	char *recall_save_cookie_key;
	char *button_reset_javascript;
	char *javascript;
	char *recall_save_cookie_multi_key;
	char *multi_javascript;
	char *apply_javascript;
} RECALL_LOAD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RECALL_LOAD *recall_load_new(
		char *state,
		char *form_name,
		char *folder_name );

/* Process */
/* ------- */
RECALL_LOAD *recall_load_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *recall_load_apply_javascript(
		char *button_reset_javascript,
		char *recall_load_javascript,
		char *recall_load_multi_javascript );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *recall_load_javascript(
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		char *form_name,
		char *recall_save_cookie_key );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *recall_load_multi_javascript(
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		const char *recall_multi_delimiter,
		char recall_multi_index_delimiter,
		char *form_name,
		char *recall_save_cookie_multi_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_load_open(
		char *form_name,
		char *recall_save_cookie_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_load_close(
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_load_multi_open(
		char *form_name,
		char *recall_save_cookie_multi_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_load_multi_close(
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		const char *recall_multi_delimiter,
		const char recall_multi_index_delimiter );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *recall_load_display(
		RECALL_LOAD *recall_load );

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

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *recall_widget_name_string(
		const char recall_name_delimiter,
		WIDGET_CONTAINER *widget_container,
		boolean got_one );

/* Usage */
/* ----- */

/* Returns component of parameter */
/* ------------------------------ */
char *recall_widget_name(
		WIDGET_CONTAINER *widget_container );
