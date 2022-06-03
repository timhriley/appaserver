/* $APPASERVER_HOME/library/recall.h				*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef TABLE_RECALL_H
#define TABLE_RECALL_H

#include "list.h"

#define RECALL_BUTTON_LABEL	"Recall"
#define RECALL_DELIMITER	'~'

typedef struct
{
	char *cookie_key;
	char *cookie_multi_key;
	char *keystrokes_save_javascript;
	char *keystrokes_multi_save_javascript;
	char *keystrokes_load_javascript;
	char *keystrokes_multi_load_javascript;
	char *save_javascript;
	char *load_javascript;
} RECALL;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
RECALL *recall_new(	char *folder_name,
			char *state,
			char *form_name,
			LIST *appaserver_element_list );

/* Process */
/* ------- */
RECALL *recall_calloc(	void );

/* Returns static memory */
/* --------------------- */
char *recall_cookie_key(
			char *folder_name,
			char *state );

/* Returns static memory */
/* --------------------- */
char *recall_cookie_multi_key(
			char *folder_name,
			char *state );

/* Returns heap memory or null */
/* --------------------------- */
char *recall_keystrokes_save_javascript(
			char *form_name,
			char *recall_cookie_key,
			LIST *appaserver_element_list,
			char recall_delimiter,
			char element_multi_move_left_right_delimiter );

/* Returns heap memory or null */
/* --------------------------- */
char *recall_keystrokes_multi_save_javascript(
			char *form_name,
			char *recall_cookie_key,
			LIST *appaserver_element_list,
			char recall_delimiter,
			char element_multi_move_left_right_delimiter );

/* Returns heap memory or null */
/* --------------------------- */
char *recall_keystrokes_load_javascript(
			char *form_name,
			char *recall_cookie_key,
			LIST *appaserver_element_list,
			char recall_delimiter,
			char element_multi_move_left_right_delimiter );

/* Returns heap memory or null */
/* --------------------------- */
char *recall_keystrokes_multi_load_javascript(
			char *form_name,
			char *recall_cookie_key,
			LIST *appaserver_element_list,
			char recall_delimiter,
			char element_multi_move_left_right_delimiter );

/* Returns heap memory or null */
/* --------------------------- */
char *recall_save_javascript(
			char *recall_keystrokes_save_javascript,
			char *recall_keystrokes_multi_save_javascript );

/* Returns heap memory or null */
/* --------------------------- */
char *recall_load_javascript(
			char *recall_keystrokes_load_javascript,
			char *recall_keystrokes_multi_load_javascript );

#endif
