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
	/* Process */
	/* ------- */
	char *html;
} BUTTON;

/* BUTTON operations */
/* ----------------- */
BUTTON *button_calloc(	void );

BUTTON *button_submit(
			char *form_post_change_multi_select_all_javascript,
			char *form_keystrokes_save_javascript,
			char *form_keystrokes_multi_save_javascript,
			char *form_verify_attribute_widths_javascript,
			int form_number );

/* Safely returns heap memory */
/* -------------------------- */
char *button_submit_html(
			char *form_post_change_multi_select_all_javascript,
			char *form_keystrokes_save_javascript,
			char *form_keystrokes_multi_save_javascript,
			char *form_verify_attribute_widths_javascript,
			int form_number );

BUTTON *button_reset(	char *post_change_javascript,
			char *state,
			int form_number );

/* Safely returns heap memory */
/* -------------------------- */
char *button_reset_html(
			char *post_change_javascript,
			char *state,
			int form_number );

BUTTON *button_back(
			void );

/* Returns program memory */
/* ---------------------- */
char *button_back_html(
			void );

BUTTON *button_forward(
			void );

/* Returns program memory */
/* ---------------------- */
char *button_forward_html(
			void );

BUTTON *button_back_to_drillthru(
			char *action_string );

/* Safely returns heap memory */
/* -------------------------- */
char *button_back_to_drillthru_html(
			char *action_string );

BUTTON *button_back_to_top(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *button_back_to_top_html(
			void );

BUTTON *button_recall(
			char *action_string );

/* Safely returns heap memory */
/* -------------------------- */
char *button_recall_html(
			char *action_string );

BUTTON *button_drillthru_skip(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *button_drillthru_skip_html(
			void );

BUTTON *button_help(
			char *application_name,
			char *html_help_file_anchor );

/* Safely returns heap memory */
/* -------------------------- */
char *button_help_html(
			char *application_name,
			char *html_help_file_anchor );

/* Safely returns heap memory */
/* -------------------------- */
char *button_list_html(
			LIST *button_list );

LIST *button_insert_pair_one2m_submit_list(
			LIST *pair_one2m_folder_list );

#endif
