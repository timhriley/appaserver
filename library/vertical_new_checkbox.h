/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_checkbox.h			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef VERTICAL_NEW_CHECKBOX_H
#define VERTICAL_NEW_CHECKBOX_H

#include "boolean.h"
#include "list.h"
#include "widget.h"

#define VERTICAL_NEW_CHECKBOX_PREFIX	"nnew_checkbox_"
#define VERTICAL_NEW_CHECKBOX_PROMPT	"New"

typedef struct
{
	boolean role_folder_insert_boolean;
	char *widget_checkbox_submit_javascript;
	char *widget_name;
	WIDGET_CONTAINER *widget_container;
} VERTICAL_NEW_CHECKBOX;

/* Usage */
/* ----- */
VERTICAL_NEW_CHECKBOX *vertical_new_checkbox_new(
		const char *vertical_new_checkbox_prefix,
		const char *vertical_new_checkbox_prompt,
		char *one_folder_name );

/* Process */
/* ------- */
VERTICAL_NEW_CHECKBOX *vertical_new_checkbox_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *vertical_new_checkbox_widget_name(
		const char *vertical_new_checkbox_prefix,
		char *one_folder_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
WIDGET_CONTAINER *vertical_new_checkbox_widget_container(
		const char *vertical_new_checkbox_prompt,
		char *vertical_new_checkbox_widget_name );

#endif

