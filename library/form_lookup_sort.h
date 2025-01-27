/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_lookup_sort.h				*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_LOOKUP_SORT_H
#define FORM_LOOKUP_SORT_H

#include "boolean.h"
#include "list.h"
#include "widget.h"

#define FORM_LOOKUP_SORT_TEXT_WIDTH			30
#define FORM_LOOKUP_SORT_ORDER_DISPLAY_SIZE		10
#define FORM_LOOKUP_SORT_TEXT_DISPLAY_SIZE		25

#define FORM_LOOKUP_SORT_NAME				"lookup_sort"

typedef struct
{
	LIST *widget_container_list;
	LIST *heading_container_list;
	char *onclick;
} FORM_LOOKUP_SORT_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_LOOKUP_SORT_TABLE *form_lookup_sort_table_new(
		LIST *folder_attribute_primary_key_list,
		char *lookup_sort_input_attribute_name,
		LIST *query_fetch_row_list );

/* Process */
/* ------- */
FORM_LOOKUP_SORT_TABLE *form_lookup_sort_table_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_lookup_sort_table_label(
		int row_number,
		char *prefix );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *form_lookup_sort_table_heading_container_list(
		LIST *folder_attribute_primary_key_list,
		char *lookup_sort_input_attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_lookup_sort_table_primary_heading(
		char *primary_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_lookup_sort_table_attribute_heading(
		char *lookup_sort_input_attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_lookup_sort_table_onclick(
		LIST *folder_attribute_primary_key_list );

typedef struct
{
	char *renumber_onclick;
	LIST *button_container_list;
	LIST *widget_container_list;
	FORM_LOOKUP_SORT_TABLE *form_lookup_sort_table;
	char *widget_container_list_html;
	char *form_tag;
	char *form_html;
} FORM_LOOKUP_SORT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_LOOKUP_SORT *form_lookup_sort_new(
		LIST *folder_attribute_primary_key_list,
		char *lookup_sort_input_attribute_name,
		LIST *query_fetch_row_list,
		char *lookup_sort_form_action_string );

/* Process */
/* ------- */
FORM_LOOKUP_SORT *form_lookup_sort_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *form_lookup_sort_button_container_list(
		char *form_lookup_sort_renumber_onclick );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_lookup_sort_renumber_onclick(
		char *lookup_sort_input_attribute_name );

#endif
