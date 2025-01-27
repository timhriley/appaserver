/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_generic_load.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_GENERIC_LOAD_H
#define FORM_GENERIC_LOAD_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "widget.h"
#include "query.h"
#include "relation_mto1.h"

#define FORM_GENERIC_LOAD_FORM_NAME	"form_generic_load"

typedef struct
{
	RELATION_MTO1 *relation_mto1;
	QUERY_DROP_DOWN *query_drop_down;
	WIDGET_CONTAINER *widget_container;
} FORM_GENERIC_LOAD_RELATION;

/* Usage */
/* ----- */
FORM_GENERIC_LOAD_RELATION *form_generic_load_relation_new(
			char *application_name,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *relation_mto1_recursive_list,
			SECURITY_ENTITY *security_entity,
			char *attribute_name,
			char *form_generic_load_constant_label,
			char *form_generic_load_after_field_onchange );

/* Process */
/* ------- */
FORM_GENERIC_LOAD_RELATION *form_generic_load_relation_calloc(
			void );

typedef struct
{
	WIDGET_CONTAINER *widget_container;
} FORM_GENERIC_LOAD_ATTRIBUTE;

/* Usage */
/* ----- */
FORM_GENERIC_LOAD_ATTRIBUTE *form_generic_load_attribute_new(
			char *application_name,
			char *login_name,
			char *attribute_name,
			char *datatype_name,
			int attribute_width,
			char *form_generic_load_constant_label,
			char *form_generic_load_after_field_onchange );

/* Process */
/* ------- */
FORM_GENERIC_LOAD_ATTRIBUTE *form_generic_load_attribute_calloc(
			void );

typedef struct
{
	char *action_string;
	char *form_tag;
	LIST *widget_container_list;
	char *widget_container_list_html;
	char *form_html;
} FORM_GENERIC_LOAD;

/* Usage */
/* ----- */
FORM_GENERIC_LOAD *form_generic_load_new(
			char *generic_load_filename_label,
			char *generic_load_skip_header_rows_label,
			char *generic_load_execute_yn_label,
			char *generic_load_position_prefix,
			char *generic_load_constant_prefix,
			char *generic_load_ignore_prefix,
			char *application_name,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *generic_load_hypertext_reference,
			LIST *folder_attribute_list,
			LIST *relation_mto1_recursive_list,
			SECURITY_ENTITY *security_entity );

/* Process */
/* ------- */
FORM_GENERIC_LOAD *form_generic_load_calloc( void );

/* Returns heap memory */
/* ------------------- */
char *form_generic_load_action_string(
			char *generic_load_hypertext_reference );

/* Returns prompt or heap memory */
/* ----------------------------- */
char *form_generic_load_attribute_prompt(
			char *prompt,
			char *hint_message );

/* Returns heap memory */
/* ------------------- */
char *form_generic_load_position_label(
			char *generic_load_position_prefix,
			char *attribute_name );

int form_generic_load_default_position(
			int primary_key_index,
			int default_position );

/* Returns heap memory */
/* ------------------- */
char *form_generic_load_constant_label(
			char *generic_load_constant_prefix,
			char *attribute_name );

/* Usage */
/* ----- */
LIST *form_generic_load_button_container_list(
			void );

/* Usage */
/* ----- */
LIST *form_generic_load_heading_container_list(
			void );

/* Usage */
/* ----- */
LIST *form_generic_load_upload_container_list(
			char *generic_load_filename_label,
			char *generic_load_skip_header_rows_label,
			char *generic_load_execute_yn_label );

/* Usage */
/* ----- */
WIDGET_CONTAINER *form_generic_load_ignore_container(
			char *generic_load_ignore_prefix,
			char *attribute_name,
			char *form_generic_load_after_ignore_onchange );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *form_generic_load_ignore_label(
			char *form_generic_load_ignore_prefix,
			char *attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_generic_load_after_field_onchange(
			char *form_generic_load_position_label,
			char *form_generic_load_constant_label,
			int form_generic_load_default_position );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_generic_load_after_ignore_onchange(
			char *form_generic_load_position_label,
			char *form_generic_load_constant_label );

#endif
