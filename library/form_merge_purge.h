/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_merge_purge.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_MERGE_PURGE_H
#define FORM_MERGE_PURGE_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "query.h"
#include "widget.h"

#define FORM_MERGE_PURGE_NAME		"form_merge_purge"

typedef struct
{
	QUERY_DROP_DOWN *query_drop_down;
	LIST *widget_drop_down_option_list;
	LIST *widget_container_list;
	WIDGET_CONTAINER *keep_widget_container;
	WIDGET_CONTAINER *delete_widget_container;
	char *widget_container_list_html;
	char *form_tag;
	char *form_html;
} FORM_MERGE_PURGE;

/* Usage */
/* ----- */
FORM_MERGE_PURGE *form_merge_purge_new(
		const char *merge_purge_keep_label,
		const char *merge_purge_delete_label,
		char *application_name,
		char *login_name,
		char *folder_name,
		boolean no_initial_capital,
		LIST *folder_attribute_primary_list,
		char *merge_purge_folder_keep_prompt,
		char *merge_purge_folder_delete_prompt,
		char *merge_purge_hypertext_reference );

/* Process */
/* ------- */
FORM_MERGE_PURGE *form_merge_purge_calloc(
		void );

/* Usage */
/* ----- */
void form_merge_purge_free(
		FORM_MERGE_PURGE *form_merge_purge );

#endif
