/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_process.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_PROCESS_H
#define FORM_PROMPT_PROCESS_H

#define FORM_PROMPT_PROCESS_PROMPT_SIZE	30

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "widget.h"
#include "radio.h"
#include "query.h"
#include "dictionary_separate.h"
#include "form.h"
#include "recall.h"
#include "process_parameter.h"
#include "form_prompt_lookup.h"
#include "form_prompt_attribute.h"

#define FORM_PROMPT_PROCESS_NAME	"prompt_process"

typedef struct
{
	FORM_PROMPT_LOOKUP_YES_NO *form_prompt_lookup_yes_no;
} FORM_PROMPT_PROCESS_YES_NO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_PROCESS_YES_NO *form_prompt_process_yes_no_new(
		char *prompt_name,
		char *hint_message,
		char *post_change_javascript );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_YES_NO *form_prompt_process_yes_no_calloc(
		void );

typedef struct
{
	WIDGET_CONTAINER *prompt_widget_container;
	WIDGET_CONTAINER *drop_down_widget_container;
	LIST *widget_container_list;
} FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT *form_prompt_process_drop_down_prompt_new(
		PROCESS_PARAMETER_DROP_DOWN_PROMPT *
			process_parameter_drop_down_prompt,
		char *post_change_javascript );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT *
	form_prompt_process_drop_down_prompt_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
char *form_prompt_process_drop_down_prompt_name(
		char *drop_down_prompt_name,
		char *optional_display );

typedef struct
{
	WIDGET_CONTAINER *drop_down_widget_container;
	LIST *widget_container_list;
} FORM_PROMPT_PROCESS_DROP_DOWN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down_new(
		PROCESS_PARAMETER_DROP_DOWN *
			process_parameter_drop_down,
		char *post_change_javascript );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down_calloc(
		void );

/* Returns component of folder or name */
/* ----------------------------------- */
char *form_prompt_process_drop_down_name(
		FOLDER *folder,
		char *process_parameter_drop_down_name );

typedef struct
{
	WIDGET_CONTAINER *prompt_widget_container;
	WIDGET_CONTAINER *widget_container;
	WIDGET_CONTAINER *hint_message_widget_container;
	LIST *widget_container_list;
} FORM_PROMPT_PROCESS_PROMPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt_new(
		char *application_name,
		char *login_name,
		char *prompt_name,
		int input_width,
		char *hint_message,
		boolean upload_filename_boolean,
		boolean date_boolean,
		char *post_change_javascript );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt_calloc(
		void );

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *form_prompt_process_prompt_upload_widget(
		char *prompt_name,
		char *post_change_javascript );

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *form_prompt_process_prompt_date_widget(
		char *application_name,
		char *login_name,
		char *prompt_name,
		char *post_change_javascript );

WIDGET_CONTAINER *form_prompt_process_prompt_notepad_widget(
		char *prompt_name,
		int input_width,
		char *post_change_javascript );

int form_prompt_process_prompt_size(
		int input_width,
		int prompt_size );

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *form_prompt_process_prompt_text_widget(
		char *prompt_name,
		int input_width,
		int form_prompt_process_prompt_size,
		char *post_change_javascript );

typedef struct
{
	FORM_PROMPT_PROCESS_YES_NO *form_prompt_process_yes_no;
	FORM_PROMPT_ATTRIBUTE *form_prompt_attribute;
	FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down;
	FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT *
		form_prompt_process_drop_down_prompt;
	FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt;
	LIST *widget_container_list;
} FORM_PROMPT_PROCESS_WIDGET_LIST;

/* Usage */
/* ----- */
FORM_PROMPT_PROCESS_WIDGET_LIST *
	form_prompt_process_widget_list_new(
		char *application_name,
		char *login_name,
		LIST *process_parameter_list,
		char *post_change_javascript );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_WIDGET_LIST *
	form_prompt_process_widget_list_calloc(
		void );

typedef struct
{
	FORM_PROMPT_PROCESS_WIDGET_LIST *form_prompt_process_widget_list;
	RECALL_SAVE *recall_save;
	RECALL_LOAD *recall_load;
	char *form_multi_select_all_javascript;
	LIST *form_prompt_lookup_button_list;
	char *button_list_html;
	char *action_string;
	const char *target_frame;
	char *form_tag;
	char *form_prompt_lookup_html;
} FORM_PROMPT_PROCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_PROCESS *form_prompt_process_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name,
		LIST *process_parameter_list,
		char *post_change_javascript,
		boolean is_drillthru );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS *form_prompt_process_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_process_action_string(
		char *post_prompt_process_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name,
		boolean is_drillthru );

/* Returns either parameter */
/* ------------------------ */
const char *form_prompt_process_target_frame(
		const char *frameset_prompt_frame,
		const char *frameset_table_frame,
		boolean is_drillthru );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_process_html(
		char *form_tag,
		LIST *widget_container_list,
		char *button_list_html,
		char *form_close_tag );

#endif
