/* ---------------------------------------------------- */
/* $APPASERVER_HOME/library/form_lookup_delete.h	*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef FORM_LOOKUP_DELETE_H
#define FORM_LOOKUP_DELETE_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "widget.h"

#define FORM_LOOKUP_DELETE_NAME		"lookup_delete"
#define FORM_LOOKUP_DELETE_OMIT_MESSAGE \
		"Omit execute for display"

typedef struct
{
	LIST *widget_container_list;
	WIDGET_CONTAINER *widget_confirm_checkbox_container;
	WIDGET_CONTAINER *widget_submit_button_container;
	char *widget_container_list_html;
	char *form_tag;
	char *query_dictionary_hidden_html;
	char *form_html;
} FORM_LOOKUP_DELETE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_LOOKUP_DELETE *form_lookup_delete_new(
			char *lookup_delete_confirm_label,
			DICTIONARY *query_dictionary,
			char *lookup_delete_action_string );

/* Process */
/* ------- */
FORM_LOOKUP_DELETE *form_lookup_delete_calloc(
			void );

#endif

