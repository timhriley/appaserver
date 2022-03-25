/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_choose_role.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "operation.h"
#include "list.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "application_constants.h"
#include "dictionary_separate.h"
#include "pair_one2m.h"
#include "element.h"
#include "javascript.h"
#include "frameset.h"
#include "button.h"
#include "appaserver.h"
#include "form.h"
#include "form_choose_role.h"

FORM_CHOOSE_ROLE *form_choose_role_calloc( void )
{
	FORM_CHOOSE_ROLE *form_choose_role;

	if ( ! ( form_choose_role = calloc( 1, sizeof( FORM_CHOOSE_ROLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_choose_role;
}

FORM_CHOOSE_ROLE *form_choose_role_new(
			LIST *role_name_list,
			char *post_action_string,
			char *frameset_menu_frame,
			char *form_name,
			char *drop_down_element_name )
{
	FORM_CHOOSE_ROLE *form_choose_role;
	APPASERVER_ELEMENT *element;

	/* List may be all the roles or missing the current role */
	/* ----------------------------------------------------- */
	if ( !list_length( role_name_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty role_name_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_choose_role = form_choose_role_calloc();

	form_choose_role->tag_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag_html(
			form_name,
			post_action_string,
			frameset_menu_frame /* target_frame */ );

	form_choose_role->onchange_javascript =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		form_choose_role_onchange_javascript(
			form_name );

	form_choose_role->element_list = list_new();

	list_set(
		form_choose_role->element_list,
		appaserver_element_new(
			table_open, (char *)0 ) );

	list_set(
		form_choose_role->element_list,
		appaserver_element_new(
			table_row, (char *)0 ) );

	list_set(
		form_choose_role->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		form_choose_role->element_list,
		( element =
			appaserver_element_new(
				drop_down,
				(char *)0 /* element_name */ ) ) );

	free( element->drop_down );

	element->drop_down =
		element_drop_down_new(
			drop_down_element_name,
			(LIST *)0 /* attribute_name_list */,
			role_name_list /* delimited_list */,
			0 /* not no_initial_capital */,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			1 /* output_select_option */,
			element_drop_down_display_size(
				list_length(
					role_name_list ) ),
			-1 /* tab order */,
			0 /* not multi_select */,
			form_choose_role->onchange_javascript
				/* post_change_javascript */,
			0 /* not readonly */,
			0 /* not recall */ );

	list_set(
		form_choose_role->element_list,
		appaserver_element_new(
			table_close, (char *)0 ) );

	form_choose_role->html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_choose_role_html(
			form_choose_role->tag_html,
			form_choose_role->element_list,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_html() );

	if ( !form_choose_role->html )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: form_choose_role_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_choose_role;
}

char *form_choose_role_onchange_javascript(
			char *form_name )
{
	static char onchange_javascript[ 128 ];

	if ( !form_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: form_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(onchange_javascript,
		"%s.submit()",
		form_name );

	return onchange_javascript;
}

char *form_choose_role_html(
			char *tag_html,
			LIST *element_list,
			char *form_close_html )
{
	char html[ 65536 ];
	char *list_html;

	if ( !tag_html
	||   !list_length( element_list )
	||   !form_close_html )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	if ( ! ( list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			element_list_html( element_list ) ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: appaserver_element_list_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	sprintf(html,
		"%s\n%s\n%s",
		tag_html,
		list_html,
		form_close_html );

	free( list_html );

	return strdup( html );
}
