/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/button.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef BUTTON_H
#define BUTTON_H

#include <unistd.h>

#define BUTTON_IMAGE_RELATIVE_DIRECTORY	"zimages"
#define BUTTON_SUBMIT_DISPLAY		"|    Submit    |"

#define BUTTON_SUBMIT_MESSAGE		\
"Submit this form to Appaserver."

#define BUTTON_DRILLTHRU_MESSAGE	\
"Start the drillthru sequence over again."

#define BUTTON_RECALL_MESSAGE				\
"Recall the last configuration of this form. "		\
"If this button stops working, then clear the "		\
"cookie cache."

#define BUTTON_BACK_MESSAGE		\
"Go back to the prior form."

#define BUTTON_FORWARD_MESSAGE		\
"Return forward if you pressed the Back button."

#define BUTTON_RESET_MESSAGE		\
"Reset the form's contents to their original values."

#define BUTTON_SKIP_MESSAGE		\
"Skip this form and enter a less specific lookup form."

#define BUTTON_TO_TOP_MESSAGE		\
"Scroll to the top of this form."

typedef struct
{
	char *label;

	/* Set externally */
	/* -------------- */
	char *action_string;
	char *hover_message;
	char *html;
} BUTTON;

/* Usage */
/* ----- */
BUTTON *button_new(
		char *label );

/* Process */
/* ------- */
BUTTON *button_calloc(	void );

/* Usage */
/* ----- */

/* Safely returns everything in heap */
/* --------------------------------- */
BUTTON *button_submit(
		char *form_name,
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *form_verify_notepad_widths_javascript );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_submit_html(
		char *form_name,
		char *button_submit_display,
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *form_verify_notepad_widths_javascript,
		char *button_submit_message );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
BUTTON *button_reset(
		char *form_name,
		char *javascript_replace );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_reset_html(
		char *form_name,
		char widget_multi_left_right_delimiter,
		char *javascript_replace,
		char *button_reset_message );

/* Usage */
/* ----- */
BUTTON *button_back(
		void );

/* Usage */
/* ----- */
BUTTON *button_forward(
		void );

/* Usage */
/* ----- */
BUTTON *button_restart_drillthru(
		char *post_choose_folder_action_string );

/* Usage */
/* ----- */
BUTTON *button_to_top(
		void );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_to_top_html(
		char *button_image_relative_directory,
		char *button_to_top_message );

/* Usage */
/* ----- */
BUTTON *button_recall(
		char *recall_load_javascript );

/* Usage */
/* ----- */
BUTTON *button_drillthru_skip(
		void );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *button_drillthru_skip_html(
		char *button_skip_message,
		char widget_multi_left_right_delimiter );

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

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *button_html(
		char *label,
		char *action_string,
		char *hover_message,
		int row_number );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *button_input_html(
		char *display,
		char *action_string,
		char *hover_message );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *button_display(
		char *label );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *button_anchor_html(
		char *display,
		char *href_action_string,
		char *onclick_action_string,
		char *hover_message );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *button_list_html(
		LIST *button_list );

void button_list_free(
		LIST *button_list );

#endif
