/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_choose_role.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_CHOOSE_ROLE_H
#define FORM_CHOOSE_ROLE_H

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
	char *tag_html;
	char *drop_down_onchange_javascript;
	LIST *element_list;
	char *html;
} FORM_CHOOSE_ROLE;

/* FORM_CHOOSE_ROLE operations */
/* --------------------------- */
FORM_CHOOSE_ROLE *form_choose_role_calloc(
			void );

FORM_CHOOSE_ROLE *form_choose_role_new(
			LIST *role_name_list,
			char *post_choose_role_action_string,
			char *target_frame,
			char *form_name,
			char *drop_down_element_name );

/* Returns static memory */
/* --------------------- */
char *form_choose_role_drop_down_onchange_javascript(
			char *form_name );

LIST *form_choose_role_element_list(
			LIST *role_name_list,
			char *drop_down_onchange_javascript,
			char *drop_down_element_name );

/* Returns heap memory or null */
/* --------------------------- */
char *form_choose_role_html(
			char *tag_html,
			LIST *element_list,
			char *form_close_html );

#endif
