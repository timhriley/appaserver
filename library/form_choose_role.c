/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_choose_role.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "sql.h"
#include "appaserver_error.h"
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
			/* ------------------------------------------------ */
			/* List may be all the roles or missing the current */
			/* ------------------------------------------------ */
			LIST *role_name_list,
			char *post_choose_role_action_string,
			char *target_frame /* FRAMESET_MENU_FRAME */,
			char *form_choose_role_name,
			char *choose_role_drop_down_name )
{
	FORM_CHOOSE_ROLE *form_choose_role;

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

	form_choose_role->widget_container_list = list_new();

	list_set(
		form_choose_role->widget_container_list,
		widget_container_new(
			table_open, (char *)0 ) );
	
	list_set(
		form_choose_role->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );
	
	list_set(
		form_choose_role->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_choose_role->widget_container_list,
		( form_choose_role->drop_down_widget_container =
			widget_container_new(
				drop_down,
				choose_role_drop_down_name ) ) );

	form_choose_role->
		drop_down_widget_container->
		drop_down->
		widget_drop_down_option_list =
			widget_drop_down_option_list(
				ATTRIBUTE_MULTI_KEY_DELIMITER,
				WIDGET_DROP_DOWN_LABEL_DELIMITER,
				WIDGET_DROP_DOWN_EXTRA_DELIMITER,
				WIDGET_DROP_DOWN_DASH_DELIMITER,
				role_name_list,
				0 /* not no_initial_capital */ );

	form_choose_role->
		drop_down_widget_container->
		drop_down->
		top_select_boolean = 1;

	form_choose_role->onchange_javascript =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		form_choose_role_onchange_javascript(
			form_choose_role_name );

	form_choose_role->
		drop_down_widget_container->
		drop_down->
		post_change_javascript =
			form_choose_role->onchange_javascript;

	list_set(
		form_choose_role->widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	form_choose_role->form_tag =
		form_tag(
			form_choose_role_name,
			post_choose_role_action_string,
			target_frame );

	form_choose_role->html =
		form_choose_role_html(
			form_choose_role->form_tag,
			form_choose_role->widget_container_list,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	return form_choose_role;
}

char *form_choose_role_onchange_javascript(
			char *form_choose_role_name )
{
	static char onchange_javascript[ 128 ];

	if ( !form_choose_role_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_choose_role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(onchange_javascript,
		"%s.submit()",
		form_choose_role_name );

	return onchange_javascript;
}

char *form_choose_role_html(
			char *form_tag,
			LIST *widget_container_list,
			char *form_close_tag )
{
	char html[ 65536 ];
	char *list_html;

	if ( !form_tag
	||   !list_length( widget_container_list )
	||   !form_close_tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_list_html(
				(char *)0 /* state */,
				-1 /* row_number */,
				(char *)0 /* background_color */,
				widget_container_list ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: widget_container_list_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s",
		form_tag,
		list_html,
		form_close_tag );

	free( list_html );

	return strdup( html );
}
