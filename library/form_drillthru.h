/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_drillthru.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_DRILLTHRU_H
#define FORM_DRILLTHRU_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "widget.h"
#include "radio.h"
#include "recall.h"
#include "form_prompt_lookup.h"

typedef struct
{
	char *action_string;
	char *form_tag;
	FORM_PROMPT_LOOKUP_WIDGET_LIST *form_prompt_lookup_widget_list;
	LIST *widget_hidden_container_list;
	char *post_choose_folder_action_string;
	char *form_multi_select_all_javascript;
	RECALL_SAVE *recall_save;
	RECALL_LOAD *recall_load;
	LIST *form_prompt_lookup_button_list;
	char *form_prompt_lookup_html;
} FORM_DRILLTHRU;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_DRILLTHRU *form_drillthru_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *base_folder_name,
		char *folder_name,
		boolean first_time_boolean,
		LIST *folder_attribute_list,
		LIST *relation_mto1_list,
		SECURITY_ENTITY *security_entity,
		DICTIONARY *drillthru_dictionary );

/* Process */
/* ------- */
FORM_DRILLTHRU *form_drillthru_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *form_drillthru_action_string(
		char *post_drillthru_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *folder_name );

boolean form_drillthru_skip_button_boolean(
		boolean first_time_boolean,
		char *state );

#endif
