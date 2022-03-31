/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/button.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef BUTTON_H
#define BUTTON_H

#include <unistd.h>

/* Constants */
/* --------- */
#define BUTTON_IMAGE_RELATIVE_DIRECTORY	"appaserver/zimages"
#define BUTTON_SUBMIT_LABEL		"|    Submit    |"
#define BUTTON_SKIP_MESSAGE		\
"Skip this form if you don't know exactly what you're looking for."

typedef struct
{
	char *html;
} BUTTON;

/* BUTTON operations */
/* ----------------- */
BUTTON *button_calloc(	void );

/* Usage */
/* ----- */
BUTTON *button_new(	char *label,
			char *action_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_html(	char *label,
			char *action_string );

/* Usage */
/* ----- */
BUTTON *button_submit(
			char *form_multi_select_all_javascript,
			char *form_keystrokes_save_javascript,
			char *form_keystrokes_multi_save_javascript,
			char *form_verify_notepad_widths_javascript,
			int form_number );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_submit_html(
			char *form_multi_select_all_javascript,
			char *form_keystrokes_save_javascript,
			char *form_keystrokes_multi_save_javascript,
			char *form_verify_notepad_widths_javascript,
			int form_number );

/* Usage */
/* ----- */
BUTTON *button_reset(	char *javascript_replace,
			int form_number );

/* Process */

/* ------- */
/* Returns heap memory */
/* ------------------- */
char *button_reset_html(
			char *javascript_replace,
			int form_number );

/* Usage */
/* ----- */
BUTTON *button_back(
			void );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *button_back_html(
			void );

/* Usage */
/* ----- */
BUTTON *button_forward(
			void );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *button_forward_html(
			void );

/* Usage */
/* ----- */
BUTTON *button_back_to_drillthru(
			char *action_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_back_to_drillthru_html(
			char *action_string );

/* Usage */
/* ----- */
BUTTON *button_back_to_top(
			void );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_back_to_top_html(
			void );

/* Usage */
/* ----- */
BUTTON *button_recall(
			char *form_keystrokes_recall_javascript,
			char *form_keystrokes_multi_recall_javascript );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_recall_html(
			char *form_keystrokes_recall_javascript,
			char *form_keystrokes_multi_recall_javascript );

/* Usage */
/* ----- */
BUTTON *button_drillthru_skip(
			void );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_drillthru_skip_html(
			void );

/* Usage */
/* ----- */
BUTTON *button_help(
			char *application_name,
			char *html_help_file_anchor );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_help_html(
			char *application_name,
			char *html_help_file_anchor );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *button_list_html(	LIST *button_list );

void button_list_free(	LIST *button_list );

LIST *button_insert_pair_one2m_submit_list(
			LIST *pair_one2m_folder_list );

#endif
