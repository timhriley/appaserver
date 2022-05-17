/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/library/form_prompt_attribute_relational.h	*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef FORM_PROMPT_ATTRIBUTE_RELATIONAL_H
#define FORM_PROMPT_ATTRIBUTE_RELATIONAL_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "element.h"

typedef struct
{
	char *name;
	char *from_name;
	char *to_name;
	LIST *element_list;
	LIST *operation_list;
	APPASERVER_ELEMENT *yes_no_appaserver_element;
	APPASERVER_ELEMENT *relation_operator_appaserver_element;
	APPASERVER_ELEMENT *text_from_appaserver_element;
	APPASERVER_ELEMENT *and_appaserver_element;
	APPASERVER_ELEMENT *text_to_appaserver_element;
	APPASERVER_ELEMENT *hint_message_appaserver_element;
} FORM_PROMPT_ATTRIBUTE_RELATIONAL;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_ATTRIBUTE_RELATIONAL *
	form_prompt_attribute_relational_new(
			char *attribute_name,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			char *post_change_javascript );

/* Process */
/* ------- */
FORM_PROMPT_ATTRIBUTE_RELATIONAL *
	form_prompt_attribute_relational_calloc(
			void );

char *form_prompt_attribute_relational_name(
			char *relational_prefix,
			char *attribute_name );

char *form_prompt_attribute_relational_from_name(
			char *from_prefix,
			char *attribute_name );

char *form_prompt_attribute_relational_to_name(
			char *to_prefix,
			char *attribute_name );

LIST *form_prompt_attribute_relational_operation_list(
			char *datatype_name );

#endif
