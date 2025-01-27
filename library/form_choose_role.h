/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_choose_role.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_CHOOSE_ROLE_H
#define FORM_CHOOSE_ROLE_H

#define FORM_CHOOSE_ROLE_NAME	"choose_role"

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "widget.h"

typedef struct
{
	LIST *widget_container_list;
	WIDGET_CONTAINER *drop_down_widget_container;
	char *onchange_javascript;
	char *form_tag;
	char *html;
} FORM_CHOOSE_ROLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_CHOOSE_ROLE *form_choose_role_new(
			/* ------------------------------------------------ */
			/* List may be all the roles or missing the current */
			/* ------------------------------------------------ */
			LIST *role_name_list,
			char *post_choose_role_action_string,
			char *target_frame /* FRAMESET_MENU_FRAME */,
			char *form_choose_role_name,
			char *choose_role_drop_down_name );

/* Process */
/* ------- */
FORM_CHOOSE_ROLE *form_choose_role_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *form_choose_role_html(
			char *form_tag,
			LIST *widget_container_list,
			char *form_close_tag );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *form_choose_role_onchange_javascript(
			char *choose_role_form_name );

/* Process */
/* ------- */

#endif
