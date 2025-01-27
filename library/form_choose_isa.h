/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_choose_isa.h			 	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_CHOOSE_ISA_H
#define FORM_CHOOSE_ISA_H

#include "boolean.h"
#include "list.h"
#include "widget.h"

#define FORM_CHOOSE_ISA_NAME		"choose_isa"

typedef struct
{
	LIST *widget_container_list;
	char *drop_down_name;
	WIDGET_CONTAINER *drop_down_widget_container;
	WIDGET_CONTAINER *widget_lookup_checkbox_container;
	LIST *button_widget_container_list;
	char *form_tag;
	char *html;
} FORM_CHOOSE_ISA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_CHOOSE_ISA *form_choose_isa_new(
		const char *widget_lookup_checkbox_name,
		char *choose_isa_prompt_message,
		LIST *primary_key_list,
		LIST *delimited_list,
		boolean no_initial_capital,
		char *choose_isa_action_string,
		char *target_frame );

/* Process */
/* ------- */
FORM_CHOOSE_ISA *form_choose_isa_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *form_choose_isa_drop_down_name(
		char sql_delimiter,
		LIST *primary_key_list );

LIST *form_choose_isa_button_widget_container_list(
		void );

/* Returns heap memory */
/* ------------------- */
char *form_choose_isa_html(
		char *form_tag,
		LIST *widget_container_list,
		char *form_close_tag );

#endif
