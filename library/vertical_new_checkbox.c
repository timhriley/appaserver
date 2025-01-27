/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_checkbox.c			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "vertical_new_checkbox.h"

char *vertical_new_checkbox_widget_name(
		const char *vertical_new_checkbox_prefix,
		char *one_folder_name )
{
	char widget_name[ 128 ];

	if ( !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf( widget_name,
		 "%s%s",
		 vertical_new_checkbox_prefix,
		 one_folder_name );

	return strdup( widget_name );
}

VERTICAL_NEW_CHECKBOX *vertical_new_checkbox_calloc( void )
{
	VERTICAL_NEW_CHECKBOX *vertical_new_checkbox;

	if ( ! ( vertical_new_checkbox =
			calloc( 1, sizeof ( VERTICAL_NEW_CHECKBOX ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return vertical_new_checkbox;
}

VERTICAL_NEW_CHECKBOX *vertical_new_checkbox_new(
		const char *vertical_new_checkbox_prefix,
		const char *vertical_new_checkbox_prompt,
		char *one_folder_name )
{
	VERTICAL_NEW_CHECKBOX *vertical_new_checkbox;

	if ( !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	vertical_new_checkbox = vertical_new_checkbox_calloc();

	vertical_new_checkbox->widget_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		vertical_new_checkbox_widget_name(
			vertical_new_checkbox_prefix,
			one_folder_name );

	vertical_new_checkbox->widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		vertical_new_checkbox_widget_container(
			vertical_new_checkbox_prompt,
			vertical_new_checkbox->widget_name );

	return vertical_new_checkbox;
}

WIDGET_CONTAINER *vertical_new_checkbox_widget_container(
		const char *vertical_new_checkbox_prompt,
		char *widget_name )
{
	WIDGET_CONTAINER *widget_container;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			checkbox,
			widget_name );

	widget_container->checkbox->prompt =
		(char *)vertical_new_checkbox_prompt;

	return widget_container;
}

