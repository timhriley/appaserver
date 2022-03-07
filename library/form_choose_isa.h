/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_choose_isa.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_CHOOSE_ISA_H
#define FORM_CHOOSE_ISA_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "element.h"
#include "radio.h"
#include "query.h"
#include "dictionary_separate.h"

typedef struct
{
	/* Process */
	/* ------- */
	char *message_html;
	char *tag_html;
	LIST *element_list;
	LIST *button_element_list;
	char *html;
} FORM_CHOOSE_ISA;

/* FORM_CHOOSE_ISA operations */
/* -------------------------- */
FORM_CHOOSE_ISA *form_choose_isa_calloc(
			void );

FORM_CHOOSE_ISA *form_choose_isa_new(
			char *prompt_message,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *choose_isa_post_action_string );

LIST *form_choose_isa_element_list(
			char *choose_isa_prompt_message,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital );

LIST *form_choose_isa_button_element_list(
			void );

/* Returns heap memory */
/* ------------------- */
char *form_choose_isa_html(
			char *tag_html,
			LIST *element_list,
			LIST *button_element_list,
			char *form_close_tag_html );

#endif
