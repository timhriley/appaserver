/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_attribute.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_ATTRIBUTE_H
#define FORM_PROMPT_ATTRIBUTE_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "widget.h"

typedef struct
{
	WIDGET_CONTAINER *yes_no_widget_container;
	WIDGET_CONTAINER *relation_operator_widget_container;
	WIDGET_CONTAINER *from_widget_container;
	WIDGET_CONTAINER *and_widget_container;
	WIDGET_CONTAINER *to_widget_container;
	WIDGET_CONTAINER *hint_message_widget_container;
	LIST *widget_container_list;
} FORM_PROMPT_ATTRIBUTE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_PROMPT_ATTRIBUTE *form_prompt_attribute_new(
		char *application_name,
		char *login_name,
		char *attribute_name,
		char *folder_attribute_prompt,
		char *datatype_name,
		int attribute_width,
		char *hint_message );

/* Process */
/* ------- */
FORM_PROMPT_ATTRIBUTE *form_prompt_attribute_calloc(
		void );

LIST *form_prompt_attribute_option_list(
		char *datatype_name );

/* Usage */
/* ----- */
WIDGET_CONTAINER *form_prompt_attribute_from_container(
		char *application_name,
		char *login_name,
		char *datatype_name,
		char *form_prefix_widget_name );

/* Usage */
/* ----- */
WIDGET_CONTAINER *form_prompt_attribute_to_container(
		char *application_name,
		char *login_name,
		char *datatype_name,
		int attribute_width,
		char *form_prefix_widget_name );

#endif
