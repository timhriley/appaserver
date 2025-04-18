/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_table_insert.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "query.h"
#include "widget.h"
#include "sql.h"
#include "appaserver.h"
#include "application.h"
#include "button.h"
#include "frameset.h"
#include "post_table_insert.h"
#include "form_table_insert.h"

FORM_TABLE_INSERT_ATTRIBUTE *form_table_insert_attribute_new(
		char *application_name,
		char *login_name,
		char *post_change_javascript,
		char *attribute_name,
		char *folder_attribute_prompt,
		char *datatype_name,
		int attribute_width,
		LIST *form_table_insert_relation_list )
{
	FORM_TABLE_INSERT_ATTRIBUTE *form_table_insert_attribute;

	if ( !application_name
	||   !login_name
	||   !attribute_name
	||   !folder_attribute_prompt
	||   !datatype_name
	||   !attribute_width )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( form_table_insert_relation_attribute_exists(
		attribute_name,
		form_table_insert_relation_list ) )
	{
		return NULL;
	}

	form_table_insert_attribute =
		form_table_insert_attribute_calloc();

	form_table_insert_attribute->widget_container_list = list_new();

	list_set(
		form_table_insert_attribute->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	if ( attribute_is_yes_no( attribute_name ) )
	{
		list_set(
			form_table_insert_attribute->widget_container_list,
			( form_table_insert_attribute->widget_container =
				widget_container_new(
					yes_no,
					attribute_name
						/* widget_name */ ) ) );

		form_table_insert_attribute->
			widget_container->
			yes_no->
			post_change_javascript =
				post_change_javascript;
	}
	else
	if ( attribute_is_notepad(
		ATTRIBUTE_DATATYPE_NOTEPAD,
		datatype_name ) )
	{
		list_set(
			form_table_insert_attribute->widget_container_list,
			( form_table_insert_attribute->widget_container =
				widget_container_new(
					notepad,
					attribute_name
						/* widget_name */ ) ) );

		form_table_insert_attribute->
			widget_container->
			notepad->
			attribute_size = attribute_width;

		form_table_insert_attribute->
			widget_container->
			notepad->
			post_change_javascript =
				post_change_javascript;
	}
	else
	if ( attribute_is_date( datatype_name )
	||   attribute_is_date_time( datatype_name )
	||   attribute_is_current_date_time( datatype_name ) )
	{
		list_set(
			form_table_insert_attribute->widget_container_list,
			( form_table_insert_attribute->
				widget_container =
					widget_container_new(
						widget_date,
						attribute_name
							/* widget_name */ ) ) );

		form_table_insert_attribute->
			widget_container->
			date->
			datatype_name = datatype_name;

		form_table_insert_attribute->
			widget_container->
			date->
			attribute_width_max_length = attribute_width;

		form_table_insert_attribute->
			widget_container->
			date->
			display_size = attribute_width;

		form_table_insert_attribute->
			widget_container->
			date->
			application_name = application_name;

		form_table_insert_attribute->
			widget_container->
			date->
			login_name = login_name;

		form_table_insert_attribute->
			widget_container->
			date->
			post_change_javascript =
				post_change_javascript;
	}
	else
	{
		list_set(
			form_table_insert_attribute->widget_container_list,
			( form_table_insert_attribute->widget_container =
				widget_container_new(
					character,
					attribute_name
						/* widget_name */ ) ) );

		form_table_insert_attribute->
			widget_container->
			text->
			datatype_name = datatype_name;

		form_table_insert_attribute->
			widget_container->
			text->
			widget_text_display_size =
				widget_text_display_size(
					WIDGET_TEXT_DEFAULT_MAX_LENGTH,
					attribute_width );

		form_table_insert_attribute->
			widget_container->
			text->
			attribute_width_max_length = attribute_width;

		form_table_insert_attribute->
			widget_container->
			text->
			post_change_javascript =
				post_change_javascript;
	}

	form_table_insert_attribute->
		widget_container->
		heading_string =
			folder_attribute_prompt;

	return form_table_insert_attribute;
}

FORM_TABLE_INSERT_ATTRIBUTE *form_table_insert_attribute_calloc( void )
{
	FORM_TABLE_INSERT_ATTRIBUTE *form_table_insert_attribute;

	if ( ! ( form_table_insert_attribute =
			calloc( 1,
				sizeof ( FORM_TABLE_INSERT_ATTRIBUTE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_table_insert_attribute;
}

FORM_TABLE_INSERT_RELATION *form_table_insert_relation_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *post_change_javascript,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *process_command_line_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *form_table_insert_relation_list,
		char *attribute_name,
		int primary_key_index )
{
	FORM_TABLE_INSERT_RELATION *form_table_insert_relation;
	LIST *one_folder_relation_mto1_list;

	if ( !application_name
	||   !login_name
	||   !insert_folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( form_table_insert_relation_attribute_exists(
		attribute_name,
		form_table_insert_relation_list ) )
	{
		return NULL;
	}

	form_table_insert_relation = form_table_insert_relation_calloc();

	form_table_insert_relation->relation_mto1 =
		relation_mto1_consumes(
			attribute_name,
			many_folder_relation_mto1_list );

	if ( !form_table_insert_relation->relation_mto1 )
	{
		form_table_insert_relation->relation_mto1 =
			relation_mto1_isa_consumes(
				attribute_name,
				relation_mto1_isa_list );

		if ( !form_table_insert_relation->relation_mto1 )
		{
			free( form_table_insert_relation );
			return NULL;
		}
	}

	if ( primary_key_index
	&&   form_table_insert_relation->
		relation_mto1->
		relation->
		relation_type_isa )
	{
		free( form_table_insert_relation );
		return NULL;
	}

	form_table_insert_relation->widget_container_list = list_new();

	list_set(
		form_table_insert_relation->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	if ( !form_table_insert_relation->relation_mto1->one_folder
	||   !form_table_insert_relation->relation_mto1->relation
	||   !list_length(
		form_table_insert_relation->
			relation_mto1->
			one_folder->
			folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"relation_mto1->one_folder is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( form_table_insert_relation->
		relation_mto1->
		relation->
		ajax_fill_drop_down )
	{
		form_table_insert_relation->ajax_client =
			form_table_insert_relation_ajax_client(
				role_name,
				form_table_insert_relation->relation_mto1 );
	}

	if ( form_table_insert_relation->ajax_client )
	{
		list_set_list(
			form_table_insert_relation->widget_container_list,
			form_table_insert_relation->
				ajax_client->
				widget_container_list );

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: ajax_client->widget_container_list=[%s]\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	widget_container_list_display(
		form_table_insert_relation->
			ajax_client->
			widget_container_list ) );
msg( (char *)0, message );
}
		return form_table_insert_relation;
	}

	list_set(
		form_table_insert_relation->widget_container_list,
		( form_table_insert_relation->drop_down_widget_container =
			widget_container_new(
				drop_down,
				form_table_insert_relation->
					relation_mto1->
					relation_name
					/* widget_name */ ) ) );

	form_table_insert_relation->
		drop_down_widget_container->
		drop_down->
		post_change_javascript =
			post_change_javascript;

	form_table_insert_relation->
		drop_down_widget_container->
		heading_string =
			form_table_insert_relation->
				relation_mto1->
				relation_prompt;

	one_folder_relation_mto1_list =
		relation_mto1_list(
			role_name,
			form_table_insert_relation->
				relation_mto1->
				one_folder_name
					/* many_folder_name */,
			form_table_insert_relation->
				relation_mto1->
				one_folder->
				folder_attribute_primary_key_list
					/* many_folder_primary_key_list */ );

	form_table_insert_relation->query_drop_down =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_drop_down_new(
			application_name,
			login_name,
			insert_folder_name
				/* many_folder_name */,
			form_table_insert_relation->
				relation_mto1->
				one_folder_name,
			APPASERVER_INSERT_STATE,
			form_table_insert_relation->
				relation_mto1->
				one_folder->
				folder_attribute_list,
			form_table_insert_relation->
				relation_mto1->
				one_folder->
				populate_drop_down_process_name,
			one_folder_relation_mto1_list,
			process_command_line_dictionary,
			security_entity );

	form_table_insert_relation->
		drop_down_widget_container->
		drop_down->
		widget_drop_down_option_list =
			widget_drop_down_option_list(
				ATTRIBUTE_MULTI_KEY_DELIMITER,
				WIDGET_DROP_DOWN_LABEL_DELIMITER,
				WIDGET_DROP_DOWN_EXTRA_DELIMITER,
				WIDGET_DROP_DOWN_DASH_DELIMITER,
				form_table_insert_relation->
					query_drop_down->
					delimited_list,
				form_table_insert_relation->
					relation_mto1->
					one_folder->
					no_initial_capital );

	form_table_insert_relation->
		drop_down_widget_container->
		drop_down->
		top_select_boolean = 1;

	return form_table_insert_relation;
}

FORM_TABLE_INSERT_RELATION *form_table_insert_relation_calloc( void )
{
	FORM_TABLE_INSERT_RELATION *form_table_insert_relation;

	if ( ! ( form_table_insert_relation =
			calloc( 1,
				sizeof ( FORM_TABLE_INSERT_RELATION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_table_insert_relation;
}

FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
	form_table_insert_automatic_widget_list_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *post_change_javascript,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *process_command_line_dictionary,
		LIST *prompt_name_list,
		SECURITY_ENTITY *security_entity,
		char *relation_mto1_automatic_preselection_name )
{
	FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
		form_table_insert_automatic_widget_list;
	FORM_TABLE_INSERT_RELATION *form_table_insert_relation;
	FOLDER_ATTRIBUTE *folder_attribute;
	WIDGET_CONTAINER *table_data_widget_container;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !insert_folder_name
	||   !list_length( folder_attribute_append_isa_list )
	||   !relation_mto1_automatic_preselection_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_table_insert_automatic_widget_list =
		form_table_insert_automatic_widget_list_calloc();

	form_table_insert_automatic_widget_list->
		widget_container_list =
			list_new();

	list_set(
		form_table_insert_automatic_widget_list->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	table_data_widget_container =
		widget_container_new(
			table_data, (char *)0 );

	table_data_widget_container->table_data->width_em = 35;

	list_set(
		form_table_insert_automatic_widget_list->widget_container_list,
		table_data_widget_container );

	form_table_insert_automatic_widget_list->
		non_edit_text_widget_container =
			widget_container_new(
				non_edit_text,
				(char *)0 /* widget_name */ );

	list_set(
		form_table_insert_automatic_widget_list->
			widget_container_list,
		form_table_insert_automatic_widget_list->
			non_edit_text_widget_container );

	form_table_insert_automatic_widget_list->hidden_widget_container =
		widget_container_new(
			hidden,
			relation_mto1_automatic_preselection_name
				/* widget_name */ );

	list_set(
		form_table_insert_automatic_widget_list->
			widget_container_list,
		form_table_insert_automatic_widget_list->
			hidden_widget_container );

	list_rewind( folder_attribute_append_isa_list );

	do {
		folder_attribute =
			list_get( 
				folder_attribute_append_isa_list );

		if ( list_string_exists(
				folder_attribute->attribute_name,
				prompt_name_list ) )
		{
			continue;
		}

		if ( folder_attribute->primary_key_index ) continue;

		if ( folder_attribute->omit_insert ) continue;

		if ( ( form_table_insert_relation =
			form_table_insert_relation_new(
				application_name,
				login_name,
				role_name,
				insert_folder_name,
				post_change_javascript,
				many_folder_relation_mto1_list,
				relation_mto1_isa_list,
				process_command_line_dictionary,
				security_entity,
				form_table_insert_automatic_widget_list->
					form_table_insert_relation_list,
				folder_attribute->attribute_name,
				folder_attribute->primary_key_index ) ) )
		{
			if ( !form_table_insert_automatic_widget_list->
				form_table_insert_relation_list )
			{
				form_table_insert_automatic_widget_list->
					form_table_insert_relation_list =
						list_new();
			}

			list_set(	
				form_table_insert_automatic_widget_list->
					form_table_insert_relation_list,
				form_table_insert_relation );

			list_set_list(
			     form_table_insert_automatic_widget_list->
					widget_container_list,
			     form_table_insert_relation->
					widget_container_list );

			continue;
		}

		if ( ( form_table_insert_automatic_widget_list->
			form_table_insert_attribute =
			form_table_insert_attribute_new(
				application_name,
				login_name,
				post_change_javascript,
				folder_attribute->attribute_name,
				folder_attribute->prompt,
				folder_attribute->attribute->datatype_name,
				folder_attribute->attribute->width,
				form_table_insert_automatic_widget_list->
					form_table_insert_relation_list ) ) )
		{
			list_set_list(
				form_table_insert_automatic_widget_list->
					widget_container_list,
				form_table_insert_automatic_widget_list->
					form_table_insert_attribute->
					widget_container_list );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	return form_table_insert_automatic_widget_list;
}

FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
	form_table_insert_automatic_widget_list_calloc(
		void )
{
	FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
		form_table_insert_automatic_widget_list;

	if ( ! ( form_table_insert_automatic_widget_list =
		   calloc(
			1,
			sizeof ( FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_table_insert_automatic_widget_list;
}

FORM_TABLE_INSERT *form_table_insert_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		int table_insert_rows_number,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *pair_one2m_dictionary,
		LIST *ignore_name_list,
		LIST *prompt_name_list,
		SECURITY_ENTITY *security_entity )
{
	FORM_TABLE_INSERT *form_table_insert;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_table_insert = form_table_insert_calloc();

	form_table_insert->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_table_insert_action_string(
			POST_TABLE_INSERT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame );

	form_table_insert->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_TABLE_INSERT_FORM_NAME,
			form_table_insert->action_string,
			target_frame );

	form_table_insert->form_table_insert_widget_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_table_insert_widget_list_new(
			application_name,
			login_name,
			role_name,
			folder_name /* insert_folder_name */,
			post_change_javascript,
			relation_mto1_list,
			relation_mto1_isa_list,
			folder_attribute_append_isa_list,
			drillthru_dictionary,
			prompt_dictionary,
			ignore_name_list,
			prompt_name_list,
			security_entity );

	form_table_insert->widget_container_heading_label_list =
		widget_container_heading_label_list(
			form_table_insert->
				form_table_insert_widget_list->
				widget_container_list );

	form_table_insert->widget_container_heading_list =
		widget_container_heading_list(
			form_table_insert->
				widget_container_heading_label_list );

	form_table_insert->prompt_hidden_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_dictionary_hidden_html(
			prompt_dictionary );

	form_table_insert->pair_one2m_hidden_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_dictionary_hidden_html(
			pair_one2m_dictionary );

	form_table_insert->button_list =
		form_table_insert_button_list();

	form_table_insert->button_list_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_list_html(
			form_table_insert->button_list );

	form_table_insert->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_table_insert_html(
			FORM_TABLE_INSERT_DEFAULT_ROWS_NUMBER,
			session_key,
			login_name,
			role_name,
			table_insert_rows_number,
			form_table_insert->form_tag,
			form_table_insert->
				form_table_insert_widget_list->
				form_table_insert_relation_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_table_open_tag(
				1 /* border_boolean */,
				0 /* cell_spacing */,
				0 /* cell_padding */ ),
			form_table_insert->widget_container_heading_list,
			form_table_insert->
				form_table_insert_widget_list->
				widget_container_list,
			form_table_insert->prompt_hidden_html,
			form_table_insert->pair_one2m_hidden_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			widget_table_close_tag(),
			form_table_insert->button_list_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	return form_table_insert;
}

FORM_TABLE_INSERT *form_table_insert_calloc( void )
{
	FORM_TABLE_INSERT *form_table_insert;

	if ( ! ( form_table_insert =
			calloc( 1, sizeof ( FORM_TABLE_INSERT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_table_insert;
}

char *form_table_insert_action_string(
		char *post_table_insert_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame )
{
	char action_string[ 1024 ];

	if ( !post_table_insert_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s+%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean(
				application_name ) ),
		post_table_insert_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		target_frame );

	return strdup( action_string );
}

boolean form_table_insert_relation_attribute_exists(
		char *attribute_name,
		LIST *form_table_insert_relation_list )
{
	FORM_TABLE_INSERT_RELATION *form_table_insert_relation;

	if ( !list_rewind( form_table_insert_relation_list ) ) return 0;

	do {
		form_table_insert_relation =
			list_get(
				form_table_insert_relation_list );

		if ( !form_table_insert_relation->relation_mto1 )
		{
			char message[ 128 ];

			sprintf(message,
			"form_table_insert_relation->relation_mto1 is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !list_length(
			form_table_insert_relation->
				relation_mto1->
				relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
			"relation_mto1->relation_foreign_key_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( list_string_exists(
			attribute_name,
			form_table_insert_relation->
				relation_mto1->
				relation_foreign_key_list ) )
		{
			return 1;
		}

	} while ( list_next( form_table_insert_relation_list ) );

	return 0;
}

LIST *form_table_insert_button_list( void )
{
	LIST *button_list = list_new();

	list_set(
		button_list,
		button_submit(
			(char *)0 /* form_multi_select_all_javascript */,
			(char *)0 /* recall_save_javascript */,
			(char *)0 /* form_verify_notepad_widths_javascript */,
			0 /* form_number */ ) );

	list_set(
		button_list,
		button_reset(
			(char *)0 /* javascript_replace */,
			0 /* form_number */ ) );

	list_set(
		button_list,
		button_back() );

	list_set(
		button_list,
		button_forward() );

	return button_list;
}

char *form_table_insert_html(
		int form_table_insert_default_rows_number,
		char *session_key,
		char *login_name,
		char *role_name,
		int table_insert_rows_number,
		char *form_tag,
		LIST *form_table_insert_relation_list,
		char *widget_table_open_tag,
		LIST *widget_container_heading_list,
		LIST *widget_container_list,
		char *widget_table_close_tag,
		char *prompt_hidden_html,
		char *pair_one2m_hidden_html,
		char *button_list_html,
		char *form_close_tag )
{
	int row_number;
	char *container_list_html;
	LIST *ajax_client_list;
	char html[ STRING_704K ];
	char *ptr = html;

	if ( !form_table_insert_default_rows_number
	||   !session_key
	||   !login_name
	||   !role_name
	||   !form_tag
	||   !widget_table_open_tag
	||   !list_length( widget_container_heading_list )
	||   !list_length( widget_container_list )
	||   !widget_table_close_tag
	||   !prompt_hidden_html
	||   !pair_one2m_hidden_html
	||   !button_list_html
	||   !form_close_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ajax_client_list =
		form_table_insert_relation_ajax_client_list(
			form_table_insert_relation_list );

	if ( list_length( ajax_client_list ) )
	{
		char *javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			ajax_client_list_javascript(
				session_key,
				login_name,
				role_name,
				ajax_client_list );

		ptr += sprintf(
			ptr,
			"%s\n",
			javascript );

		/* free( javascript ); */
	}

	ptr += sprintf(
		ptr,
		"%s\n%s\n",
		form_tag,
		widget_table_open_tag );

	container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			widget_container_heading_list );

	ptr += sprintf(
		ptr,
		"%s\n",
		container_list_html );

	/* free( container_list_html ); */

	if ( table_insert_rows_number <= 0 )
		table_insert_rows_number =
			form_table_insert_default_rows_number;

	for (	row_number = 1;
		row_number <= table_insert_rows_number;
		row_number++ )
	{
		container_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_list_html(
				APPASERVER_INSERT_STATE,
				row_number,
				form_background_color(),
				widget_container_list );

		if (	strlen( html ) +
			strlen( container_list_html ) + 1 >= STRING_704K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_704K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			container_list_html );

		/* free( container_list_html ); */
	}

	if (	strlen( html ) +
		strlen( widget_table_close_tag ) + 1 >= STRING_704K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_704K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		widget_table_close_tag );

	if (	strlen( html ) +
		strlen( prompt_hidden_html ) +
		strlen( pair_one2m_hidden_html ) +
		strlen( button_list_html ) +
		strlen( form_close_tag ) + 3 >= STRING_704K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_704K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"%s\n%s\n%s\n%s",
		prompt_hidden_html,
		pair_one2m_hidden_html,
		button_list_html,
		form_close_tag );

	return strdup( html );
}

FORM_TABLE_INSERT_WIDGET_LIST *
	form_table_insert_widget_list_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *prompt_dictionary,
		LIST *ignore_name_list,
		LIST *prompt_name_list,
		SECURITY_ENTITY *security_entity )
{
	FORM_TABLE_INSERT_WIDGET_LIST *form_table_insert_widget_list;
	FOLDER_ATTRIBUTE *folder_attribute;
	FORM_TABLE_INSERT_RELATION *form_table_insert_relation;
	FORM_TABLE_INSERT_ATTRIBUTE *form_table_insert_attribute;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !list_rewind( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_table_insert_widget_list = form_table_insert_widget_list_calloc();

	form_table_insert_widget_list->
		widget_container_list =
			list_new();

	form_table_insert_widget_list->
		process_command_line_dictionary =
			/* ------------------------ */
			/* Returns either parameter */
			/* ------------------------ */
			process_command_line_dictionary(
				drillthru_dictionary,
				prompt_dictionary );

	list_set(
		form_table_insert_widget_list->
			widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( list_string_exists(
			folder_attribute->attribute_name,
			ignore_name_list ) )
		{
			continue;
		}

		if ( list_string_exists(
			folder_attribute->attribute_name,
			prompt_name_list ) )
		{
			continue;
		}

		if ( folder_attribute->omit_insert ) continue;

		if ( ( form_table_insert_relation =
			form_table_insert_relation_new(
				application_name,
				login_name,
				role_name,
				insert_folder_name,
				post_change_javascript,
				relation_mto1_list
					/* many_folder_relation_mto1_list */,
				relation_mto1_isa_list,
				form_table_insert_widget_list->
					process_command_line_dictionary,
				security_entity,
				form_table_insert_widget_list->
					form_table_insert_relation_list,
				folder_attribute->attribute_name,
				folder_attribute->primary_key_index ) ) )
		{
			if ( !form_table_insert_widget_list->
				form_table_insert_relation_list )
			{
				form_table_insert_widget_list->
					form_table_insert_relation_list =
						list_new();
			}

			list_set(
				form_table_insert_widget_list->
					form_table_insert_relation_list,
				form_table_insert_relation );

			list_set_list(
				form_table_insert_widget_list->
					widget_container_list,
				form_table_insert_relation->
					widget_container_list );

			continue;
		}

		if ( ( form_table_insert_attribute =
			form_table_insert_attribute_new(
				application_name,
				login_name,
				post_change_javascript,
				folder_attribute->attribute_name,
				folder_attribute->prompt,
				folder_attribute->attribute->datatype_name,
				folder_attribute->attribute->width,
				form_table_insert_widget_list->
					form_table_insert_relation_list ) ) )
		{
			if ( !form_table_insert_widget_list->
				form_table_insert_attribute_list )
			{
				form_table_insert_widget_list->
					form_table_insert_attribute_list =
						list_new();
			}

			list_set(
				form_table_insert_widget_list->
					form_table_insert_attribute_list,
				form_table_insert_attribute );

			list_set_list(
				form_table_insert_widget_list->
					widget_container_list,
				form_table_insert_attribute->
					widget_container_list );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	list_set_list(
		form_table_insert_widget_list->
			widget_container_list,
		widget_container_dictionary_hidden_list(
			prompt_dictionary ) );

	return form_table_insert_widget_list;
}

FORM_TABLE_INSERT_WIDGET_LIST *form_table_insert_widget_list_calloc( void )
{
	FORM_TABLE_INSERT_WIDGET_LIST *form_table_insert_widget_list;

	if ( ! ( form_table_insert_widget_list =
			calloc( 1,
				sizeof ( FORM_TABLE_INSERT_WIDGET_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_table_insert_widget_list;
}

FORM_TABLE_INSERT_AUTOMATIC *form_table_insert_automatic_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *post_change_javascript,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *pair_one2m_dictionary,
		LIST *prompt_name_list,
		SECURITY_ENTITY *security_entity,
		RELATION_MTO1 *relation_mto1_automatic_preselection,
		LIST *query_dictionary_delimited_list )
{
	FORM_TABLE_INSERT_AUTOMATIC *form_table_insert_automatic;
	LIST *one_folder_relation_mto1_list;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !list_rewind( folder_attribute_append_isa_list )
	||   !relation_mto1_automatic_preselection
	||   !relation_mto1_automatic_preselection->one_folder
	||   !list_length(
		relation_mto1_automatic_preselection->
			one_folder->
			folder_attribute_primary_key_list )
	||   !list_length(
		relation_mto1_automatic_preselection->
			one_folder->
			folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_table_insert_automatic = form_table_insert_automatic_calloc();

	form_table_insert_automatic->
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_table_insert_action_string =
			form_table_insert_action_string(
				POST_TABLE_INSERT_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				target_frame );

	form_table_insert_automatic->form_tag =
		form_tag(
			FORM_TABLE_INSERT_FORM_NAME,
			form_table_insert_automatic->
				form_table_insert_action_string,
			FRAMESET_TABLE_FRAME );

	one_folder_relation_mto1_list =
		relation_mto1_list(
			role_name,
			relation_mto1_automatic_preselection->
				one_folder_name
				/* many_folder_name */,
			relation_mto1_automatic_preselection->
				one_folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	form_table_insert_automatic->
		process_command_line_dictionary =
			/* ------------------------ */
			/* Returns either parameter */
			/* ------------------------ */
			process_command_line_dictionary(
				drillthru_dictionary,
				prompt_dictionary );

	form_table_insert_automatic->query_drop_down =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_drop_down_new(
			application_name,
			login_name,
			folder_name /* many_folder_name */,
			relation_mto1_automatic_preselection->
				one_folder_name,
			APPASERVER_INSERT_STATE,
			relation_mto1_automatic_preselection->
				one_folder->
				folder_attribute_list,
			relation_mto1_automatic_preselection->
				one_folder->
				populate_drop_down_process_name,
			one_folder_relation_mto1_list,
			form_table_insert_automatic->
				process_command_line_dictionary,
			security_entity );

	form_table_insert_automatic->form_table_insert_automatic_widget_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_table_insert_automatic_widget_list_new(
			application_name,
			login_name,
			role_name,
			folder_name /* insert_folder_name */,
			post_change_javascript,
			many_folder_relation_mto1_list,
			relation_mto1_isa_list,
			folder_attribute_append_isa_list,
			form_table_insert_automatic->
				process_command_line_dictionary,
			prompt_name_list,
			security_entity,
			relation_mto1_automatic_preselection->relation_name );

	form_table_insert_automatic->widget_container_heading_label_list =
		widget_container_heading_label_list(
			form_table_insert_automatic->
				form_table_insert_automatic_widget_list->
				widget_container_list );

	list_set_first(
		form_table_insert_automatic->
			widget_container_heading_label_list,
		"key" );

	form_table_insert_automatic->widget_container_heading_list =
		widget_container_heading_list(
			form_table_insert_automatic->
				widget_container_heading_label_list );

	form_table_insert_automatic->prompt_hidden_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_dictionary_hidden_html(
			prompt_dictionary );

	form_table_insert_automatic->pair_one2m_hidden_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_dictionary_hidden_html(
			dictionary_prefix(
				pair_one2m_dictionary,
				DICTIONARY_SEPARATE_PAIR_PREFIX ) );

	form_table_insert_automatic->
		form_table_insert_button_list =
			form_table_insert_button_list();

	form_table_insert_automatic->button_list_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_list_html(
			form_table_insert_automatic->
				form_table_insert_button_list );

	form_table_insert_automatic->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_table_insert_automatic_html(
			relation_mto1_automatic_preselection->
				one_folder->
				no_initial_capital,
			query_dictionary_delimited_list,
			form_table_insert_automatic->form_tag,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_table_open_tag(
				1 /* border_boolean */,
				0 /* cell_spacing */,
				0 /* cell_padding */ ),
			form_table_insert_automatic->
				widget_container_heading_list,
			form_table_insert_automatic->
				query_drop_down->delimited_list,
			form_table_insert_automatic->
				form_table_insert_automatic_widget_list
				/* in/out */,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			widget_table_close_tag(),
			form_table_insert_automatic->prompt_hidden_html,
			form_table_insert_automatic->pair_one2m_hidden_html,
			form_table_insert_automatic->button_list_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	return form_table_insert_automatic;
}

FORM_TABLE_INSERT_AUTOMATIC *form_table_insert_automatic_calloc( void )
{
	FORM_TABLE_INSERT_AUTOMATIC *form_table_insert_automatic;

	if ( ! ( form_table_insert_automatic =
			calloc( 1,
				sizeof ( FORM_TABLE_INSERT_AUTOMATIC ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_table_insert_automatic;
}

char *form_table_insert_automatic_html(
		boolean no_initial_capital,
		LIST *query_dictionary_delimited_list,
		char *form_tag,
		char *widget_table_open_tag,
		LIST *widget_container_heading_list,
		LIST *query_drop_down_delimited_list,
		FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
			form_table_insert_automatic_widget_list
			/* in/out */,
		char *widget_table_close_tag,
		char *prompt_hidden_html,
		char *pair_one2m_hidden_html,
		char *button_list_html,
		char *form_close_tag )
{
	char *widget_html;
	char *hidden_html;
	char *container_list_html;
	char html[ STRING_640K ];
	char *ptr = html;

	if ( !form_tag
	||   !widget_table_open_tag
	||   !list_length( widget_container_heading_list )
	||   !form_table_insert_automatic_widget_list
	||   !widget_table_close_tag
	||   !prompt_hidden_html
	||   !pair_one2m_hidden_html
	||   !button_list_html
	||   !form_close_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr,
		"%s\n%s\n",
		form_tag,
		widget_table_open_tag );

	container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			widget_container_heading_list );

	ptr += sprintf( ptr,
		"%s\n",
		container_list_html );

	free( container_list_html );

	widget_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_table_insert_automatic_widget_html(
			no_initial_capital,
			query_dictionary_delimited_list,
			query_drop_down_delimited_list,
			form_table_insert_automatic_widget_list
				/* in/out */ );

	hidden_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_table_insert_automatic_hidden_html(
			query_dictionary_delimited_list,
			query_drop_down_delimited_list,
			form_table_insert_automatic_widget_list
				/* in/out */ );

	if (	strlen( html ) +
		strlen( widget_html ) +
		strlen( widget_table_close_tag ) +
		strlen( hidden_html ) +
		strlen( prompt_hidden_html ) +
		strlen( pair_one2m_hidden_html ) +
		strlen( button_list_html ) +
		strlen( form_close_tag ) + 7 >= STRING_640K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_640K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(ptr,
		"%s\n%s\n%s\n%s\n%s\n%s\n%s",
		widget_html,
		widget_table_close_tag,
		hidden_html,
		prompt_hidden_html,
		pair_one2m_hidden_html,
		button_list_html,
		form_close_tag );

	free( widget_html );
	free( hidden_html );

	return strdup( html );
}

char *form_table_insert_automatic_message( char *delimited_string )
{
	char buffer[ 1024 ];

	if ( !delimited_string )
	{
		char message[ 128 ];

		sprintf(message, "delimited_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	string_initial_capital(
		buffer,
		delimited_string );

	return strdup( buffer );
}

LIST *form_table_insert_relation_ajax_client_list(
		LIST *form_table_insert_relation_list )
{
	FORM_TABLE_INSERT_RELATION *form_table_insert_relation;
	LIST *ajax_client_list = {0};

	if ( list_rewind( form_table_insert_relation_list ) )
	do {
		form_table_insert_relation =
			list_get(
				form_table_insert_relation_list );

		if ( form_table_insert_relation->ajax_client )
		{
			if ( !ajax_client_list ) ajax_client_list = list_new();

			list_set(
				ajax_client_list,
				form_table_insert_relation->ajax_client );
		}

	} while ( list_next( form_table_insert_relation_list ) );

	return ajax_client_list;
}

char *form_table_insert_automatic_widget_html(
		boolean no_initial_capital,
		LIST *query_dictionary_delimited_list,
		LIST *query_drop_down_delimited_list,
		FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
			form_table_insert_automatic_widget_list )
{
	int row_number = 0;
	char *container_list_html;
	char *delimited_string;
	char html[ STRING_512K ];
	char *ptr = html;
	char *display_string;
	char *value_string;

	if ( !form_table_insert_automatic_widget_list
	||   !list_length(
		form_table_insert_automatic_widget_list->
			widget_container_list )
	||   !form_table_insert_automatic_widget_list->
		non_edit_text_widget_container )
	{
		char message[ 128 ];

		sprintf(message,
	"form_table_insert_automatic_widget_list is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( query_drop_down_delimited_list ) )
		return strdup( "" );

	*html = '\0';

	do {
		delimited_string =
			list_get(
				query_drop_down_delimited_list );

		value_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_option_value_string(
				WIDGET_DROP_DOWN_LABEL_DELIMITER
					/* probably '|' */,
				WIDGET_DROP_DOWN_EXTRA_DELIMITER
					/* probably '[' */,
				delimited_string );

		if ( list_exists_string(
			value_string,
			query_dictionary_delimited_list ) )
		{
			continue;
		}

		display_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_drop_down_option_display_string(
				WIDGET_DROP_DOWN_LABEL_DELIMITER /* '|' */,
				WIDGET_DROP_DOWN_DASH_DELIMITER,
				no_initial_capital,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				sql_delimiter_string( SQL_DELIMITER ),
				delimited_string );

		form_table_insert_automatic_widget_list->
			non_edit_text_widget_container->
			non_edit_text->
			widget_name = display_string;

		container_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_list_html(
				APPASERVER_INSERT_STATE,
				++row_number,
				form_background_color(),
				form_table_insert_automatic_widget_list->
					widget_container_list );

		if (	strlen( html ) +
			strlen( container_list_html ) + 1 >= STRING_512K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_512K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			container_list_html );

		free( display_string );
		free( container_list_html );

	} while ( list_next( query_drop_down_delimited_list ) );

	return strdup( html );
}

char *form_table_insert_automatic_hidden_html(
		LIST *query_dictionary_delimited_list,
		LIST *query_drop_down_delimited_list,
		FORM_TABLE_INSERT_AUTOMATIC_WIDGET_LIST *
			form_table_insert_automatic_widget_list )
{
	int row_number = 0;
	char *container_list_html;
	char *delimited_string;
	char html[ STRING_64K ];
	char *ptr = html;
	char *value_string;

	if ( !form_table_insert_automatic_widget_list
	||   !list_length(
		form_table_insert_automatic_widget_list->
			widget_container_list )
	||   !form_table_insert_automatic_widget_list->
		hidden_widget_container )
	{
		char message[ 128 ];

		sprintf(message,
	"form_table_insert_automatic_widget_list is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( query_drop_down_delimited_list ) )
		return strdup( "" );

	*html = '\0';

	do {
		delimited_string =
			list_get(
				query_drop_down_delimited_list );

		value_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_option_value_string(
				WIDGET_DROP_DOWN_LABEL_DELIMITER
					/* probably '|' */,
				WIDGET_DROP_DOWN_EXTRA_DELIMITER
					/* probably '[' */,
				delimited_string );

		if ( list_exists_string(
			value_string,
			query_dictionary_delimited_list ) )
		{
			continue;
		}

		form_table_insert_automatic_widget_list->
			hidden_widget_container->
			hidden->
			value_string = value_string;

		container_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_list_hidden_html(
				++row_number,
				form_table_insert_automatic_widget_list->
					/* ------------------ */
					/* Only one is hidden */
					/* ------------------ */
					widget_container_list );

		if ( !container_list_html )
		{
			char message[ 128 ];

			sprintf(message,
			"widget_container_list_hidden_html() returned empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if (	strlen( html ) +
			strlen( container_list_html ) >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s",
			/* --------------- */
			/* Has trailing CR */
			/* --------------- */
			container_list_html );

		free( container_list_html );

	} while ( list_next( query_drop_down_delimited_list ) );

	return strdup( html );
}

char *form_table_insert_automatic_key(
		char query_key_extra_delimiter,
		char *delimited_string )
{
	static char key[ 128 ];

	if ( !string_character_exists(
			delimited_string,
			query_key_extra_delimiter ) )
	{
		return delimited_string;
	}

	piece(	key,
		query_key_extra_delimiter,
		delimited_string,
		0 );

	return
	/* ----------------------------------------- */
	/* Returns buffer with end shortened (maybe) */
	/* ----------------------------------------- */
	string_trim_right_spaces( key /* buffer */ );
}

AJAX_CLIENT *form_table_insert_relation_ajax_client(
		char *role_name,
		RELATION_MTO1 *relation_mto1 )
{
	LIST *relation_mto1_list;

	if ( !role_name
	||   !relation_mto1 )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_mto1_list =
		relation_mto1_to_one_fetch_list(
			role_name,
			relation_mto1->one_folder_name,
			relation_mto1->
				one_folder->
				folder_attribute_primary_key_list );

	return
	/* --------------------------------- */
	/* Returns null if not participating */
	/* --------------------------------- */
	ajax_client_relation_mto1_new(
		relation_mto1,
		relation_mto1_list,
		1 /* top_select_boolean */ );
}
