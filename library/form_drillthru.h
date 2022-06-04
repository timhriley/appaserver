/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_drillthru.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_DRILLTHRU_H
#define FORM_DRILLTHRU_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "element.h"
#include "radio.h"
#include "query.h"
#include "dictionary_separate.h"
#include "recall.h"
#include "post_drillthru.h"
#include "form_prompt_lookup.h"

typedef struct
{
	char *action_string;
	char *form_tag_html;
	FORM_PROMPT_LOOKUP_ELEMENT_LIST *form_prompt_lookup_element_list;
	char *form_multi_select_all_javascript;
	RECALL *recall;
	LIST *form_prompt_lookup_button_list;
	char *html;
} FORM_DRILLTHRU;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_DRILLTHRU *form_drillthru_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *state,
			char *folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
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

#endif
