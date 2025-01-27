/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_generic_load.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "sql.h"
#include "appaserver_error.h"
#include "attribute.h"
#include "folder_attribute.h"
#include "query.h"
#include "frameset.h"
#include "form.h"
#include "form_generic_load.h"

FORM_GENERIC_LOAD_ATTRIBUTE *form_generic_load_attribute_calloc( void )
{
	FORM_GENERIC_LOAD_ATTRIBUTE *form_generic_load_attribute;

	if ( ! ( form_generic_load_attribute =
			calloc( 1,
				sizeof ( FORM_GENERIC_LOAD_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_generic_load_attribute;
}

FORM_GENERIC_LOAD_ATTRIBUTE *form_generic_load_attribute_new(
		char *application_name,
		char *login_name,
		char *attribute_name,
		char *datatype_name,
		int attribute_width,
		char *form_generic_load_constant_label,
		char *form_generic_load_after_field_onchange )
{
	FORM_GENERIC_LOAD_ATTRIBUTE *form_generic_load_attribute;

	if ( !application_name
	||   !login_name
	||   !attribute_name
	||   !datatype_name
	||   !attribute_width
	||   !form_generic_load_constant_label
	||   !form_generic_load_after_field_onchange )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_generic_load_attribute = form_generic_load_attribute_calloc();

	if ( attribute_is_yes_no( attribute_name ) )
	{
		form_generic_load_attribute->widget_container =
			widget_container_new(
				yes_no,
				form_generic_load_constant_label
					/* widget_name */ );

		form_generic_load_attribute->
			widget_container->
			yes_no->
			post_change_javascript =
				form_generic_load_after_field_onchange;
	}
	else
	if ( attribute_is_date( datatype_name ) )
	{
		form_generic_load_attribute->widget_container =
			widget_container_new(
				widget_date,
				form_generic_load_constant_label
					/* widget_name */ );

		form_generic_load_attribute->
			widget_container->
			date->
			application_name = application_name;

		form_generic_load_attribute->
			widget_container->
			date->
			login_name = login_name;

		form_generic_load_attribute->
			widget_container->
			date->
			post_change_javascript =
				form_generic_load_after_field_onchange;
	}
	else
	{
		form_generic_load_attribute->widget_container =
			widget_container_new(
				text,
				form_generic_load_constant_label
					/* widget_name */ );

		form_generic_load_attribute->
			widget_container->
			text->
			widget_text_display_size =
				widget_text_display_size(
					WIDGET_TEXT_DEFAULT_MAX_LENGTH,
					attribute_width );

		form_generic_load_attribute->
			widget_container->
			text->
			post_change_javascript =
				form_generic_load_after_field_onchange;
	}

	return form_generic_load_attribute;
}

FORM_GENERIC_LOAD_RELATION *form_generic_load_relation_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *relation_mto1_recursive_list,
		SECURITY_ENTITY *security_entity,
		char *attribute_name,
		char *form_generic_load_constant_label,
		char *form_generic_load_after_field_onchange )
{
	FORM_GENERIC_LOAD_RELATION *form_generic_load_relation;
	LIST *one_folder_relation_mto1_list;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !attribute_name
	||   !form_generic_load_constant_label
	||   !form_generic_load_after_field_onchange )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( relation_mto1_recursive_list ) ) return NULL;

	form_generic_load_relation = form_generic_load_relation_calloc();

	form_generic_load_relation->relation_mto1 =
		relation_mto1_consumes(
			attribute_name,
			relation_mto1_recursive_list,
			(LIST *)0 /* relation_mto1_isa_list */ );

	if ( !form_generic_load_relation->relation_mto1 )
	{
		free( form_generic_load_relation );
		return NULL;
	}

	if ( !form_generic_load_relation->relation_mto1->one_folder )
	{
		char message[ 128 ];

		sprintf(message, "relation_mto1->one_folder is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	one_folder_relation_mto1_list =
		relation_mto1_list(
			role_name,
			form_generic_load_relation->
				relation_mto1->
				one_folder_name
					/* many_folder_name */,
			form_generic_load_relation->
				relation_mto1->
				one_folder->
				folder_attribute_primary_key_list
					/* many_folder_primary_key_list */ );

	form_generic_load_relation->query_drop_down =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_drop_down_new(
			application_name,
			login_name,
			folder_name /* many_folder_name */,
			form_generic_load_relation->
				relation_mto1->
				one_folder_name,
			(char *)0 /* state */,
			form_generic_load_relation->
				relation_mto1->
				one_folder->
				folder_attribute_list,
			form_generic_load_relation->
				relation_mto1->
				one_folder->
				populate_drop_down_process_name,
			one_folder_relation_mto1_list,
			(DICTIONARY *)0 /* command_line_dictionary */,
			security_entity );

	form_generic_load_relation->widget_container =
		widget_container_new(
			drop_down,
			form_generic_load_constant_label
				/* widget_name */ );

	form_generic_load_relation->
		widget_container->
		drop_down->
		widget_drop_down_option_list =
			widget_drop_down_option_list(
				SQL_DELIMITER,
				WIDGET_DROP_DOWN_LABEL_DELIMITER,
				WIDGET_DROP_DOWN_EXTRA_DELIMITER,
				WIDGET_DROP_DOWN_DASH_DELIMITER,
				form_generic_load_relation->
					query_drop_down->
					delimited_list,
				form_generic_load_relation->
					relation_mto1->
					one_folder->
					no_initial_capital );

	form_generic_load_relation->
		widget_container->
		drop_down->
		top_select_boolean = 1;

	form_generic_load_relation->
		widget_container->
		drop_down->
		post_change_javascript =
			form_generic_load_after_field_onchange;

	return form_generic_load_relation;
}

FORM_GENERIC_LOAD_RELATION *form_generic_load_relation_calloc( void )
{
	FORM_GENERIC_LOAD_RELATION *form_generic_load_relation;

	if ( ! ( form_generic_load_relation =
			calloc( 1,
				sizeof ( FORM_GENERIC_LOAD_RELATION ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_generic_load_relation;
}

FORM_GENERIC_LOAD *form_generic_load_new(
		char *generic_load_filename_label,
		char *generic_load_skip_header_rows_label,
		char *generic_load_execute_yn_label,
		char *generic_load_position_prefix,
		char *generic_load_constant_prefix,
		char *generic_load_ignore_prefix,
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *generic_load_hypertext_reference,
		LIST *folder_attribute_list,
		LIST *relation_mto1_recursive_list,
		SECURITY_ENTITY *security_entity )
{
	FORM_GENERIC_LOAD *form_generic_load;
	WIDGET_CONTAINER *widget_container;
	int default_position = 0;
	FOLDER_ATTRIBUTE *folder_attribute;
	char *position_label;
	char *constant_label;
	char *after_field_onchange;
	char *after_ignore_onchange;
	FORM_GENERIC_LOAD_RELATION *form_generic_load_relation;
	FORM_GENERIC_LOAD_ATTRIBUTE *form_generic_load_attribute;

	if ( !generic_load_filename_label
	||   !generic_load_skip_header_rows_label
	||   !generic_load_execute_yn_label
	||   !generic_load_position_prefix
	||   !generic_load_constant_prefix
	||   !generic_load_ignore_prefix
	||   !application_name
	||   !login_name
	||   !generic_load_hypertext_reference
	||   !list_rewind( folder_attribute_list ) )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_generic_load = form_generic_load_calloc();

	form_generic_load->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_generic_load_action_string(
			generic_load_hypertext_reference );

	form_generic_load->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_GENERIC_LOAD_FORM_NAME,
			form_generic_load->action_string,
			FRAMESET_TABLE_FRAME );

	form_generic_load->widget_container_list = list_new();

	list_set_list(
		form_generic_load->widget_container_list,
		form_generic_load_button_container_list() );

	list_set_list(
		form_generic_load->widget_container_list,
		form_generic_load_upload_container_list(
			generic_load_filename_label,
			generic_load_skip_header_rows_label,
			generic_load_execute_yn_label ) );

	list_set(
		form_generic_load->widget_container_list,
		widget_container_new(
			line_break, (char *)0 ) );

	list_set(
		form_generic_load->widget_container_list,
		( widget_container =
			widget_container_new(
				table_open, (char *)0 ) ) );

	widget_container->
		table_open->
		border_boolean = 1;

	list_set_list(
		form_generic_load->widget_container_list,
		form_generic_load_heading_container_list() );

	do {
		folder_attribute = list_get( folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 256 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		++default_position;

		list_set(
			form_generic_load->widget_container_list,
			widget_container_new(
				table_row, (char *)0 ) );

		list_set(
			form_generic_load->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_generic_load->widget_container_list,
			widget_container_new(
				text,
				/* ----------------------------- */
				/* Returns prompt or heap memory */
				/* ----------------------------- */
				form_generic_load_attribute_prompt(
					folder_attribute->prompt,
					folder_attribute->
						attribute->
						hint_message ) ) );

		position_label =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_generic_load_position_label(
			      generic_load_position_prefix,
			      folder_attribute->attribute_name );

		list_set(
			form_generic_load->widget_container_list,
			( widget_container = widget_container_new(
				text,
				position_label ) ) );

		widget_container->
			text->
			attribute_width_max_length = 3;

		constant_label =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_generic_load_constant_label(
				generic_load_constant_prefix,
				folder_attribute->attribute_name );

		after_field_onchange =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_generic_load_after_field_onchange(
				position_label,
				constant_label,
				form_generic_load_default_position(
					folder_attribute->primary_key_index,
					default_position ) );

		widget_container->
			text->
			post_change_javascript = after_field_onchange;

		widget_container->
			text->
			value_string =
				strdup(	
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_itoa(
						default_position ) );

		list_set(
			form_generic_load->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		if ( list_length( relation_mto1_recursive_list )
		&& ( form_generic_load_relation =
			  form_generic_load_relation_new(
				application_name,
				login_name,
				role_name,
				folder_name,
				relation_mto1_recursive_list,
				security_entity,
				folder_attribute->attribute_name,
				constant_label,
				after_field_onchange ) ) )
		{
			list_set(
				form_generic_load->widget_container_list,
				form_generic_load_relation->
					widget_container );
		}
		else
		{
			form_generic_load_attribute =
				form_generic_load_attribute_new(
					application_name,
					login_name,
					folder_attribute->attribute_name,
					folder_attribute->
						attribute->
						datatype_name,
					folder_attribute->attribute->width,
					constant_label,
					after_field_onchange );

			list_set(
				form_generic_load->widget_container_list,
				form_generic_load_attribute->
					widget_container );
		}

		if ( !folder_attribute->primary_key_index )
		{
			list_set(
				form_generic_load->widget_container_list,
				widget_container_new(
					table_data, (char *)0 ) );

			after_ignore_onchange =
				form_generic_load_after_ignore_onchange(
					position_label,
					constant_label );

			list_set(
				form_generic_load->widget_container_list,
				form_generic_load_ignore_container(
					generic_load_ignore_prefix,
					folder_attribute->attribute_name,
					after_ignore_onchange ) );
		}

	} while ( list_next( folder_attribute_list ) );

	list_set(
		form_generic_load->widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	form_generic_load->widget_container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			0 /* row_number */,
			(char *)0 /* background_color */,
			form_generic_load->widget_container_list );

	form_generic_load->form_html =
		form_html(
			form_generic_load->form_tag,
			form_generic_load->widget_container_list_html,
			(char *)0 /* dictionary_hidden_html */,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	free( form_generic_load->widget_container_list_html );

	return form_generic_load;
}

FORM_GENERIC_LOAD *form_generic_load_calloc( void )
{
	FORM_GENERIC_LOAD *form_generic_load;

	if ( ! ( form_generic_load =
			calloc( 1, sizeof ( FORM_GENERIC_LOAD ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_generic_load;
}

char *form_generic_load_action_string( char *hypertext_reference )
{
	char action_string[ 1024 ];

	if ( !hypertext_reference )
	{
		char message[ 256 ];

		sprintf(message, "hypertext_reference is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(action_string,
		"action=\"%s\"",
		hypertext_reference );

	return strdup( action_string );
}

char *form_generic_load_position_label(
		char *generic_load_position_prefix,
		char *attribute_name )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	form_prefix_widget_name(
		generic_load_position_prefix,
		attribute_name );
}

char *form_generic_load_constant_label(
		char *generic_load_constant_prefix,
		char *attribute_name )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	form_prefix_widget_name(
		generic_load_constant_prefix,
		attribute_name );
}

LIST *form_generic_load_button_container_list( void )
{
	return widget_button_submit_reset_container_list();
}

LIST *form_generic_load_heading_container_list( void )
{
	LIST *widget_container_list = list_new();

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_heading, "Attribute" ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_heading, "Position" ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_heading, "Constant" ) );

	return widget_container_list;
}

WIDGET_CONTAINER *form_generic_load_ignore_container(
		char *generic_load_ignore_prefix,
		char *attribute_name,
		char *form_generic_load_after_ignore_onchange )
{
	WIDGET_CONTAINER *widget_container;

	if ( !generic_load_ignore_prefix
	||   !attribute_name
	||   !form_generic_load_after_ignore_onchange )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		widget_container_new(
			checkbox,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_generic_load_ignore_label(
				generic_load_ignore_prefix,
				attribute_name ) );

	widget_container->
		checkbox->
		onclick =
			form_generic_load_after_ignore_onchange;

	widget_container->
		checkbox->
		prompt = "Ignore";

	return widget_container;
}

char *form_generic_load_ignore_label(
		char *form_generic_load_ignore_prefix,
		char *attribute_name )
{
	if ( !form_generic_load_ignore_prefix
	||   !attribute_name )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	form_prefix_widget_name(
		form_generic_load_ignore_prefix,
		attribute_name );
}

LIST *form_generic_load_upload_container_list(
		char *generic_load_filename_label,
		char *generic_load_skip_header_rows_label,
		char *generic_load_execute_yn_label )
{
	LIST *widget_container_list;
	WIDGET_CONTAINER *widget_container;

	if ( !generic_load_filename_label
	||   !generic_load_skip_header_rows_label
	||   !generic_load_execute_yn_label )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container_list = list_new();

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				table_open, (char *)0 ) ) );

	widget_container->table_open->border_boolean = 1;

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			non_edit_text, "Filename" ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			upload,
			generic_load_filename_label ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			non_edit_text, "Skip Header Rows" ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				text,
				generic_load_skip_header_rows_label ) ) );

	widget_container->
		text->
		attribute_width_max_length = 3;

	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			non_edit_text,
			"How many header rows to skip?" ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			non_edit_text, "Execute yn" ) );

	list_set(
		widget_container_list,
		widget_container_new(
			yes_no,
			generic_load_execute_yn_label ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	return widget_container_list;
}

char *form_generic_load_attribute_prompt(
		char *prompt,
		char *hint_message )
{
	char attribute_prompt[ 1024 ];

	if ( !prompt )
	{
		char message[ 256 ];

		sprintf(message, "prompt is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !hint_message ) return prompt;

	sprintf(attribute_prompt,
		"%s (%s)",
		prompt,
		hint_message );

	return strdup( attribute_prompt );
}

int form_generic_load_default_position(
		int primary_key_index,
		int default_position )
{
	if ( primary_key_index )
		return default_position;
	else
		return 0;
}

char *form_generic_load_after_ignore_onchange(
		char *form_generic_load_position_label,
		char *form_generic_load_constant_label )
{
	char after_ignore_onchange[ 1024 ];

	if ( !form_generic_load_position_label
	||   !form_generic_load_constant_label )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(after_ignore_onchange,
		"after_ignore( this, '%s', '%s' )",
		form_generic_load_position_label,
		form_generic_load_constant_label );

	return strdup( after_ignore_onchange );
}

char *form_generic_load_after_field_onchange(
		char *form_generic_load_position_label,
		char *form_generic_load_constant_label,
		int form_generic_load_default_position )
{
	char after_field_onchange[ 1024 ];

	if ( !form_generic_load_position_label
	||   !form_generic_load_constant_label )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(after_field_onchange,
		"after_field( '%s', '%s', %d )",
		form_generic_load_position_label,
		form_generic_load_constant_label,
		form_generic_load_default_position );

	return strdup( after_field_onchange );
}

