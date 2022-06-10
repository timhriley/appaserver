/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_process.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_PROCESS_H
#define FORM_PROMPT_PROCESS_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "element.h"
#include "radio.h"
#include "query.h"
#include "dictionary_separate.h"
#include "form.h"
#include "recall.h"
#include "process_parameter.h"
#include "form_prompt_attribute_relational.h"

#define FORM_PROMPT_PROCESS_NAME	"prompt_process"

typedef struct
{
	LIST *element_list;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	FORM_PROMPT_ATTRIBUTE_RELATIONAL *form_prompt_attribute_relational;
} FORM_PROMPT_PROCESS_ATTRIBUTE;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_PROCESS_ATTRIBUTE *
	form_prompt_process_attribute_new(
			char *post_change_process,
			char *attribute_name,
			char *datatype_name,
			int attribute_width,
			char *hint_message );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute_calloc(
			void );

typedef struct
{
	LIST *element_list;
	APPASERVER_ELEMENT *prompt_appaserver_element;
	APPASERVER_ELEMENT *drop_down_appaserver_element;
} FORM_PROMPT_PROCESS_DROP_DOWN;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_PROCESS_DROP_DOWN *
	form_prompt_process_drop_down_new(
			char *post_change_javascript,
			PROCESS_PARAMETER_DROP_DOWN *
				process_parameter_drop_down );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down_calloc(
			void );

typedef struct
{
	LIST *element_list;
	APPASERVER_ELEMENT *text_appaserver_element;
} FORM_PROMPT_PROCESS_PROMPT;

/* Usage */
/* ----- */
FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt_new(
			char *post_change_javascript,
			char *prompt_name,
			int input_width,
			char *hint_message,
			boolean upload_filename,
			boolean date_boolean );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt_calloc(
			void );

APPASERVER_ELEMENT *form_prompt_process_prompt_upload_filename_element(
			char *post_change_process,
			char *prompt_name,
			char *hint_message );

APPASERVER_ELEMENT *form_prompt_process_prompt_date_element(
			char *post_change_process,
			char *prompt_name,
			char *hint_message );

APPASERVER_ELEMENT *form_prompt_process_prompt_text_element(
			char *post_change_process,
			char *prompt_name,
			int input_width,
			char *hint_message );

typedef struct
{
	LIST *element_list;
	FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute;
	FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down;
	FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt;
	char *appaserver_element_list_html;
} FORM_PROMPT_PROCESS_ELEMENT_LIST;

/* Usage */
/* ----- */
FORM_PROMPT_PROCESS_ELEMENT_LIST *
	form_prompt_process_element_list_new(
			LIST *process_parameter_list,
			char *post_change_javascript );

/* Process */
/* ------- */
FORM_PROMPT_PROCESS_ELEMENT_LIST *
	form_prompt_process_element_list_calloc(
			void );

typedef struct
{
	char *action_string;
	char *form_tag_html;
	FORM_PROMPT_PROCESS_ELEMENT_LIST *form_prompt_process_element_list;
	char *form_multi_select_all_javascript;
	RECALL *recall;
	LIST *form_prompt_lookup_button_list;
	char *html;
} FORM_PROMPT_PROCESS;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_PROCESS *form_prompt_process_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			LIST *process_parameter_list,
			char *post_change_javascript );

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
			char *process_or_set_name );

#endif
