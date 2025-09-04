/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_merge_purge.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "frameset.h"
#include "form.h"
#include "form_merge_purge.h"

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
		char *merge_purge_hypertext_reference )
{
	FORM_MERGE_PURGE *form_merge_purge;
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !login_name
	||   !list_length( folder_attribute_primary_list )
	||   !merge_purge_folder_keep_prompt
	||   !merge_purge_folder_delete_prompt
	||   !merge_purge_hypertext_reference )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_merge_purge = form_merge_purge_calloc();

	form_merge_purge->query_drop_down =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_drop_down_new(
			application_name,
			(char *)0 /* session_key */,
			login_name,
			(char *)0 /* role_name */,
			(char *)0 /* state */,
			(char *)0 /* many_folder_name */,
			folder_name /* one_folder_name */,
			(char *)0 /* related_column */,
			folder_attribute_primary_list
				/* one_folder_attribute_list */,
			(char *)0 /* populate_drop_down_process_name */,
			(LIST *)0 /* relation_mto1_list */,
			(DICTIONARY *)0 /* command_line_dictionary */,
			(SECURITY_ENTITY *)0 /* security_entity */,
			(LIST *)0 /* common_name_list */ );

	form_merge_purge->widget_drop_down_option_list =
		widget_drop_down_option_list(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			WIDGET_DROP_DOWN_LABEL_DELIMITER,
			WIDGET_DROP_DOWN_EXTRA_DELIMITER,
			WIDGET_DROP_DOWN_DASH_DELIMITER,
			form_merge_purge->
				query_drop_down->
				delimited_list,
			no_initial_capital );

	form_merge_purge->widget_container_list = list_new();

	list_set(
		form_merge_purge->widget_container_list,
		( widget_container =
			widget_container_new(
				table_open, (char *)0 ) ) );

	widget_container->table_open->border_boolean = 1;

	/* Keep row */
	/* -------- */
	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			non_edit_text,
			merge_purge_folder_keep_prompt ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		( form_merge_purge->keep_widget_container =
			widget_container_new(
				drop_down,
				(char *)merge_purge_keep_label ) ) );

	form_merge_purge->
		keep_widget_container->
		drop_down->
		widget_drop_down_option_list =
			form_merge_purge->
			widget_drop_down_option_list;

	/* Purge row */
	/* --------- */
	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			non_edit_text,
			merge_purge_folder_delete_prompt ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		( form_merge_purge->delete_widget_container =
			widget_container_new(
				drop_down,
				(char *)merge_purge_delete_label ) ) );

	form_merge_purge->
		delete_widget_container->
		drop_down->
		widget_drop_down_option_list =
			form_merge_purge->widget_drop_down_option_list;

	/* Execute row */
	/* ----------- */
	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			non_edit_text,
			APPASERVER_EXECUTE_YN ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			yes_no,
			APPASERVER_EXECUTE_YN ) );

	/* Table close */
	/* ----------- */
	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		( widget_container =
			widget_container_new(
				table_open, (char *)0 ) ) );

	widget_container->table_open->border_boolean = 1;

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set_list(
		form_merge_purge->widget_container_list,
		widget_button_submit_reset_container_list() );

	list_set(
		form_merge_purge->widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	form_merge_purge->widget_container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			form_merge_purge->widget_container_list );

	form_merge_purge->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_MERGE_PURGE_NAME,
			merge_purge_hypertext_reference,
			FRAMESET_TABLE_FRAME );

	form_merge_purge->form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_html(
			form_merge_purge->form_tag,
			form_merge_purge->widget_container_list_html,
			(char *)0 /* hidden_html */,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	return form_merge_purge;
}

FORM_MERGE_PURGE *form_merge_purge_calloc( void )
{
	FORM_MERGE_PURGE *form_merge_purge;

	if ( ! ( form_merge_purge =
			calloc( 1,
				sizeof ( FORM_MERGE_PURGE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_merge_purge;
}

void form_merge_purge_free( FORM_MERGE_PURGE *form_merge_purge )
{
	if ( !form_merge_purge )
	{
		char message[ 128 ];

		sprintf(message, "form_merge_purge is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( form_merge_purge->widget_container_list_html );
	free( form_merge_purge->form_tag );
	free( form_merge_purge->form_html );
	free( form_merge_purge );
}

