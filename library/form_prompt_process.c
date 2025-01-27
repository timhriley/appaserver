/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_process.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
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
#include "appaserver_parameter.h"
#include "dictionary_separate.h"
#include "pair_one2m.h"
#include "widget.h"
#include "javascript.h"
#include "frameset.h"
#include "button.h"
#include "appaserver.h"
#include "post_prompt_process.h"
#include "form_prompt_lookup.h"
#include "form_prompt_attribute.h"
#include "form_prompt_process.h"

FORM_PROMPT_PROCESS_WIDGET_LIST *
	form_prompt_process_widget_list_new(
		char *application_name,
		char *login_name,
		LIST *process_parameter_list,
		char *post_change_javascript )
{
	FORM_PROMPT_PROCESS_WIDGET_LIST *form_prompt_process_widget_list;
	PROCESS_PARAMETER *process_parameter;
	WIDGET_CONTAINER *widget_container;

	if ( !list_rewind( process_parameter_list ) ) return NULL;

	form_prompt_process_widget_list =
		form_prompt_process_widget_list_calloc();

	form_prompt_process_widget_list->widget_container_list = list_new();

	list_set(
		form_prompt_process_widget_list->widget_container_list,
		( widget_container =
		  	widget_container_new(
				table_open,
				(char *)0 ) ) );

	widget_container->table_open->border_boolean = 1;

	do {
		process_parameter =
			list_get(
				process_parameter_list );

		list_set(
			form_prompt_process_widget_list->widget_container_list,
			widget_container_new(
				table_row, (char *)0 ) );

		if ( process_parameter->process_parameter_yes_no )
		{
			form_prompt_process_widget_list->
			    form_prompt_process_yes_no =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				form_prompt_process_yes_no_new(
					process_parameter->
						process_parameter_yes_no->
						prompt_name,
					process_parameter->
						process_parameter_yes_no->
						process_parameter_prompt->
						hint_message,
					post_change_javascript );

			list_set_list(
				form_prompt_process_widget_list->
					widget_container_list,
				form_prompt_process_widget_list->
					form_prompt_process_yes_no->
					form_prompt_lookup_yes_no->
					widget_container_list );
		}
		else
		if ( process_parameter->process_parameter_attribute )
		{
			if ( !process_parameter->
				process_parameter_attribute->
				attribute )
			{
				char message[ 128 ];

				sprintf(message,
			"process_parameter_attribute->attribute is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			form_prompt_process_widget_list->
			    form_prompt_attribute =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				form_prompt_attribute_new(
					application_name,
					login_name,
					process_parameter->
						process_parameter_attribute->
						attribute->
						attribute_name,
					process_parameter->
						process_parameter_attribute->
						attribute->
						attribute_name
						/* folder_attribute_prompt */,
					process_parameter->
						process_parameter_attribute->
						attribute->
						datatype_name,
					process_parameter->
						process_parameter_attribute->
						attribute->
						width,
					process_parameter->
						process_parameter_attribute->
						attribute->
						hint_message );

			list_set_list(
				form_prompt_process_widget_list->
					widget_container_list,
				form_prompt_process_widget_list->
					form_prompt_attribute->
					widget_container_list );
		}
		else
		if ( process_parameter->process_parameter_drop_down )
		{
			form_prompt_process_widget_list->
			    form_prompt_process_drop_down =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				form_prompt_process_drop_down_new(
					process_parameter->
						process_parameter_drop_down,
					post_change_javascript );

			list_set_list(
				form_prompt_process_widget_list->
					widget_container_list,
				form_prompt_process_widget_list->
					form_prompt_process_drop_down->
					widget_container_list );
		}
		else
		if ( process_parameter->process_parameter_drop_down_prompt )
		{
			form_prompt_process_widget_list->
			  form_prompt_process_drop_down_prompt =
			    /* -------------- */
			    /* Safely returns */
			    /* -------------- */
			    form_prompt_process_drop_down_prompt_new(
				process_parameter->
					process_parameter_drop_down_prompt,
				post_change_javascript );

			list_set_list(
				form_prompt_process_widget_list->
					widget_container_list,
				form_prompt_process_widget_list->
					form_prompt_process_drop_down_prompt->
					widget_container_list );
		}
		else
		if ( process_parameter->process_parameter_prompt )
		{
			form_prompt_process_widget_list->
			    form_prompt_process_prompt =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				form_prompt_process_prompt_new(
					application_name,
					login_name,
					process_parameter->
						process_parameter_prompt->
						prompt_name,
					process_parameter->
						process_parameter_prompt->
						input_width,
					process_parameter->
						process_parameter_prompt->
						hint_message,
					process_parameter->
						process_parameter_prompt->
						upload_filename_boolean,
					process_parameter->
						process_parameter_prompt->
						date_boolean,
					post_change_javascript );

			list_set_list(
				form_prompt_process_widget_list->
					widget_container_list,
				form_prompt_process_widget_list->
					form_prompt_process_prompt->
					widget_container_list );
		}
		else
		{
			char message[ 128 ];

			sprintf(message, "invalid configuration." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

	} while ( list_next( process_parameter_list ) );

	list_set(
		form_prompt_process_widget_list->widget_container_list,
		widget_container_new(
			table_close,
			(char *)0 ) );

	return form_prompt_process_widget_list;
}

FORM_PROMPT_PROCESS_WIDGET_LIST *
	form_prompt_process_widget_list_calloc(
		void )
{
	FORM_PROMPT_PROCESS_WIDGET_LIST *form_prompt_process_widget_list;

	if ( ! ( form_prompt_process_widget_list =
			calloc(	1,
				sizeof ( FORM_PROMPT_PROCESS_WIDGET_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_widget_list;
}

FORM_PROMPT_PROCESS *form_prompt_process_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name,
		LIST *process_parameter_list,
		char *post_change_javascript,
		boolean is_drillthru )
{
	FORM_PROMPT_PROCESS *form_prompt_process;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_process = form_prompt_process_calloc();

	form_prompt_process->form_prompt_process_widget_list =
		form_prompt_process_widget_list_new(
			application_name,
			login_name,
			process_parameter_list,
			post_change_javascript );

	if ( !form_prompt_process->form_prompt_process_widget_list )
	{
		char message[ 128 ];

		sprintf(message,
		"form_prompt_process_widget_list_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_process->recall_save =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recall_save_new(
			(char *)0 /* state */,
			FORM_PROMPT_PROCESS_NAME,
			(char *)0 /* folder_name */,
			form_prompt_process->
				form_prompt_process_widget_list->
				widget_container_list,
			application_ssl_support_boolean(
				application_name ) );

	form_prompt_process->recall_load =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recall_load_new(
			(char *)0 /* state */,
			FORM_PROMPT_PROCESS_NAME,
			(char *)0 /* folder_name */,
			form_prompt_process->
				form_prompt_process_widget_list->
				widget_container_list );

	form_prompt_process->form_multi_select_all_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_multi_select_all_javascript(
			form_prompt_process->
				form_prompt_process_widget_list->
				widget_container_list );

	form_prompt_process->form_prompt_lookup_button_list =
		form_prompt_lookup_button_list(
			FORM_PROMPT_PROCESS_NAME,
			0 /* not button_drillthru_skip_boolean */,
			(char *)0 /* post_choose_folder_action_string */,
			form_prompt_process->form_multi_select_all_javascript,
			form_prompt_process->recall_save->javascript,
			form_prompt_process->recall_load->javascript );

	form_prompt_process->button_list_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		button_list_html(
			form_prompt_process->
				form_prompt_lookup_button_list );

	form_prompt_process->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_process_action_string(
			POST_PROMPT_PROCESS_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			is_drillthru );

	form_prompt_process->target_frame =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		form_prompt_process_target_frame(
			FRAMESET_PROMPT_FRAME,
			FRAMESET_TABLE_FRAME,
			is_drillthru );

	form_prompt_process->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_PROMPT_PROCESS_NAME,
			form_prompt_process->action_string,
			(char *)form_prompt_process->target_frame );

	form_prompt_process->form_prompt_lookup_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_process_html(
			form_prompt_process->form_tag,
			form_prompt_process->
				form_prompt_process_widget_list->
				widget_container_list,
			form_prompt_process->button_list_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	free( form_prompt_process->button_list_html );

	button_list_free(
		form_prompt_process->
			form_prompt_lookup_button_list );

	return form_prompt_process;
}

char *form_prompt_process_action_string(
		char *post_prompt_process_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name,
		boolean is_drillthru )
{
	char action_string[ 1024 ];

	if ( !post_prompt_process_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s+%c",
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
		post_prompt_process_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_or_set_name,
		(is_drillthru) ? 'y' : 'n' );

	return strdup( action_string );
}

FORM_PROMPT_PROCESS *form_prompt_process_calloc( void )
{
	FORM_PROMPT_PROCESS *form_prompt_process;

	if ( ! ( form_prompt_process =
			calloc( 1, sizeof ( FORM_PROMPT_PROCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process;
}

FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt_new(
		char *application_name,
		char *login_name,
		char *prompt_name,
		int input_width,
		char *hint_message,
		boolean upload_filename_boolean,
		boolean date_boolean,
		char *post_change_javascript )
{
	FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt;

	if ( !prompt_name )
	{
		char message[ 128 ];

		sprintf(message, "prompt_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_process_prompt = form_prompt_process_prompt_calloc();

	form_prompt_process_prompt->widget_container_list = list_new();

	list_set(
		form_prompt_process_prompt->
			widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_process_prompt->widget_container_list,
		( form_prompt_process_prompt->prompt_widget_container =
			widget_container_new(
				non_edit_text,
				prompt_name
					/* widget_name */ ) ) );

	list_set(
		form_prompt_process_prompt->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	if ( upload_filename_boolean )
	{
		form_prompt_process_prompt->widget_container =
			/* --------------- */
			/* Safely returns */
			/* -------------- */
			form_prompt_process_prompt_upload_widget(
				prompt_name,
				post_change_javascript );
	}
	else
	if ( date_boolean )
	{
		form_prompt_process_prompt->widget_container =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			form_prompt_process_prompt_date_widget(
				application_name,
				login_name,
				prompt_name,
				post_change_javascript );
	}
	else
	if ( input_width > 128 )
	{
		form_prompt_process_prompt->widget_container =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			form_prompt_process_prompt_notepad_widget(
				prompt_name,
				input_width,
				post_change_javascript );
	}
	else
	{
		form_prompt_process_prompt->widget_container =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			form_prompt_process_prompt_text_widget(
				prompt_name,
				input_width,
				form_prompt_process_prompt_size(
					input_width,
					FORM_PROMPT_PROCESS_PROMPT_SIZE ),
				post_change_javascript );
	}

	list_set(
		form_prompt_process_prompt->widget_container_list,
		form_prompt_process_prompt->widget_container );

	if ( hint_message )
	{
		list_set(
			form_prompt_process_prompt->widget_container_list,
			widget_container_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_process_prompt->widget_container_list,
			( form_prompt_process_prompt->
				hint_message_widget_container =
					widget_container_new(
						non_edit_text,
						hint_message
							/* widget_name */ ) ) );

		form_prompt_process_prompt->
			hint_message_widget_container->
			non_edit_text->
			no_initial_capital_boolean = 1;
	}

	return form_prompt_process_prompt;
}

FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt_calloc( void )
{
	FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt;

	if ( ! ( form_prompt_process_prompt =
			calloc( 1, sizeof ( FORM_PROMPT_PROCESS_PROMPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_prompt;
}

WIDGET_CONTAINER *form_prompt_process_prompt_upload_widget(
		char *prompt_name,
		char *post_change_javascript )
{
	WIDGET_CONTAINER *widget_container;

	if ( !prompt_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	widget_container =
		widget_container_new(
			upload,
			prompt_name );

	widget_container->recall_boolean = 1;

	widget_container->upload->post_change_javascript =
		post_change_javascript;

	return widget_container;
}

WIDGET_CONTAINER *form_prompt_process_prompt_date_widget(
		char *application_name,
		char *login_name,
		char *prompt_name,
		char *post_change_javascript )
{
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !login_name
	||   !prompt_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		widget_container_new(
			widget_date,
			prompt_name
				/* widget_name */ );

	widget_container->recall_boolean = 1;

	widget_container->date->application_name = application_name;
	widget_container->date->login_name = login_name;
	widget_container->date->post_change_javascript = post_change_javascript;
	widget_container->date->datatype_name = WIDGET_DATE_DATATYPE_NAME;

	widget_container->date->attribute_width_max_length =
	widget_container->date->display_size =
		WIDGET_DATE_DISPLAY_SIZE;

	return widget_container;
}

int form_prompt_process_prompt_size(
		int input_width,
		int prompt_size )
{
	if ( input_width < prompt_size )
		return input_width;
	else
		return prompt_size;
}

WIDGET_CONTAINER *form_prompt_process_prompt_notepad_widget(
		char *prompt_name,
		int input_width,
		char *post_change_javascript )
{
	WIDGET_CONTAINER *widget_container;

	if ( !prompt_name
	||   !input_width )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		widget_container_new(
			notepad,
			prompt_name
				/* widget_name */ );

	widget_container->notepad->attribute_size = input_width;

	widget_container->text->post_change_javascript =
		post_change_javascript;

	return widget_container;
}

WIDGET_CONTAINER *form_prompt_process_prompt_text_widget(
		char *prompt_name,
		int input_width,
		int form_prompt_process_prompt_size,
		char *post_change_javascript )
{
	WIDGET_CONTAINER *widget_container;

	if ( !prompt_name
	||   !input_width )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		widget_container_new(
			text,
			prompt_name
				/* widget_name */ );

	widget_container->text->attribute_width_max_length = input_width;

	widget_container->text->widget_text_display_size =
		form_prompt_process_prompt_size;

	widget_container->recall_boolean = 1;

	widget_container->text->post_change_javascript =
		post_change_javascript;

	return widget_container;
}

FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down_new(
		PROCESS_PARAMETER_DROP_DOWN *
			process_parameter_drop_down,
		char *post_change_javascript )
{
	FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down;
	char *widget_name;

	if ( !process_parameter_drop_down )
	{
		char message[ 128 ];

		sprintf(message, "process_parameter_drop_down is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_process_drop_down = form_prompt_process_drop_down_calloc();

	form_prompt_process_drop_down->widget_container_list = list_new();

	list_set(
		form_prompt_process_drop_down->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	widget_name =
		/* ----------------------------------- */
		/* Returns component of folder or name */
		/* ----------------------------------- */
		form_prompt_process_drop_down_name(
			process_parameter_drop_down->folder,
			process_parameter_drop_down->name );

	if ( !widget_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
		"form_prompt_process_drop_down_name() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_set(
		form_prompt_process_drop_down->widget_container_list,
		widget_container_new(
			non_edit_text, 
			widget_name ) );

	list_set(
		form_prompt_process_drop_down->
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

	if ( process_parameter_drop_down->multi_select_boolean )
	{
		list_set(
			form_prompt_process_drop_down->widget_container_list,
			( form_prompt_process_drop_down->
			    drop_down_widget_container =
				widget_container_new(
					multi_drop_down,
					process_parameter_drop_down->name
						/* widget_name */ ) ) );

		form_prompt_process_drop_down->
		    drop_down_widget_container->
			multi_drop_down->
			original_drop_down->
			widget_drop_down_option_list =
				process_parameter_drop_down->
					widget_drop_down_option_list;

		form_prompt_process_drop_down->
			drop_down_widget_container->
			multi_drop_down->
			original_drop_down->
			top_select_boolean  = 1;

		form_prompt_process_drop_down->
			drop_down_widget_container->
			multi_drop_down->
			original_drop_down->
			display_size =
				WIDGET_MULTI_DISPLAY_SIZE;

		form_prompt_process_drop_down->
			drop_down_widget_container->
			multi_drop_down->
			original_drop_down->
			post_change_javascript = post_change_javascript;
	}
	else
	{
		list_set(
			form_prompt_process_drop_down->widget_container_list,
			( form_prompt_process_drop_down->
			      drop_down_widget_container =
				widget_container_new(
					drop_down,
					process_parameter_drop_down->name
						/* widget_name */ ) ) );

		form_prompt_process_drop_down->
			drop_down_widget_container->
			drop_down->
			widget_drop_down_option_list =
			process_parameter_drop_down->
				widget_drop_down_option_list;

		form_prompt_process_drop_down->
			drop_down_widget_container->
			drop_down->
			top_select_boolean = 1;

		form_prompt_process_drop_down->
			drop_down_widget_container->
			drop_down->
			display_size = 1;

		form_prompt_process_drop_down->
			drop_down_widget_container->
			drop_down->
			post_change_javascript = post_change_javascript;
	}

	form_prompt_process_drop_down->
		drop_down_widget_container->
		recall_boolean = 1;

	return form_prompt_process_drop_down;
}

FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down_calloc( void )
{
	FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down;

	if ( ! ( form_prompt_process_drop_down =
			calloc( 1,
				sizeof ( FORM_PROMPT_PROCESS_DROP_DOWN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_prompt_process_drop_down;
}

FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT *
	form_prompt_process_drop_down_prompt_new(
		PROCESS_PARAMETER_DROP_DOWN_PROMPT *
			process_parameter_drop_down_prompt,
		char *post_change_javascript )
{
	FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT *
		form_prompt_process_drop_down_prompt;
	char *prompt_name;

	if ( !process_parameter_drop_down_prompt
	||   !process_parameter_drop_down_prompt->drop_down_prompt_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_process_drop_down_prompt =
		form_prompt_process_drop_down_prompt_calloc();

	form_prompt_process_drop_down_prompt->widget_container_list =
		list_new();

	list_set(
		form_prompt_process_drop_down_prompt->
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

	prompt_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		form_prompt_process_drop_down_prompt_name(
			process_parameter_drop_down_prompt->
				drop_down_prompt_name,
			process_parameter_drop_down_prompt->
				optional_display );

	list_set(
		form_prompt_process_drop_down_prompt->
			widget_container_list,
		( form_prompt_process_drop_down_prompt->
			prompt_widget_container =
			   widget_container_new(
				non_edit_text,
				prompt_name /* widget_name */ ) ) );

	list_set(
		form_prompt_process_drop_down_prompt->
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

	list_set(
		form_prompt_process_drop_down_prompt->
			widget_container_list,
		( form_prompt_process_drop_down_prompt->
			drop_down_widget_container =
			   widget_container_new(
				drop_down, 
				prompt_name /* widget_name */ ) ) );

	form_prompt_process_drop_down_prompt->
		drop_down_widget_container->
		drop_down->
		widget_drop_down_option_list =
			process_parameter_drop_down_prompt->
				data_option_list;

	form_prompt_process_drop_down_prompt->
		drop_down_widget_container->
			drop_down->
			top_select_boolean = 1;

	form_prompt_process_drop_down_prompt->
		drop_down_widget_container->
			drop_down->
			multi_select_boolean =
				process_parameter_drop_down_prompt->
					multi_select_boolean;

	form_prompt_process_drop_down_prompt->
		drop_down_widget_container->
		drop_down->
		post_change_javascript =
			post_change_javascript;

	form_prompt_process_drop_down_prompt->
		drop_down_widget_container->
		recall_boolean = 1;

	return form_prompt_process_drop_down_prompt;
}

FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT *
	form_prompt_process_drop_down_prompt_calloc(
		void )
{
	FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT *
		form_prompt_process_drop_down_prompt;

	if ( ! ( form_prompt_process_drop_down_prompt =
			calloc(
			   1,
			   sizeof ( FORM_PROMPT_PROCESS_DROP_DOWN_PROMPT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_prompt_process_drop_down_prompt;
}

char *form_prompt_process_html(
			char *form_tag,
			LIST *widget_container_list,
			char *button_list_html,
			char *form_close_tag )
{
	char *container_list_html;
	char html[ STRING_128K ];

	if ( !form_tag
	||   !list_length( widget_container_list )
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

	container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			widget_container_list );

	if (	string_strlen( form_tag ) +
		string_strlen( button_list_html ) +
		string_strlen( container_list_html ) +
		string_strlen( form_close_tag ) + 3 >= STRING_128K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_128K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s",
		form_tag,
		container_list_html,
		button_list_html,
		form_close_tag );

	free( container_list_html );

	return strdup( html );
}

FORM_PROMPT_PROCESS_YES_NO *form_prompt_process_yes_no_new(
		char *prompt_name,
		char *hint_message,
		char *post_change_javascript )
{
	FORM_PROMPT_PROCESS_YES_NO *form_prompt_process_yes_no;

	if ( !prompt_name )
	{
		char message[ 128 ];

		sprintf(message, "prompt_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_process_yes_no = form_prompt_process_yes_no_calloc();

	form_prompt_process_yes_no->
	    form_prompt_lookup_yes_no =
		form_prompt_lookup_yes_no_new(
			prompt_name /* attribute_name */,
			prompt_name /* folder_attribute_prompt */,
			hint_message,
			post_change_javascript,
			0 /* not no_display_boolean */,
			(LIST *)0 /* form_prompt_lookup_relation_list */ );

	if ( !form_prompt_process_yes_no->form_prompt_lookup_yes_no )
	{
		char message[ 128 ];

		sprintf(message,
			"form_prompt_lookup_yes_no_new() returned empty" );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_prompt_process_yes_no;
}

FORM_PROMPT_PROCESS_YES_NO *form_prompt_process_yes_no_calloc( void )
{
	FORM_PROMPT_PROCESS_YES_NO *form_prompt_process_yes_no;

	if ( ! ( form_prompt_process_yes_no =
			calloc( 1,
				sizeof ( FORM_PROMPT_PROCESS_YES_NO ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_prompt_process_yes_no;
}

const char *form_prompt_process_target_frame(
		const char *frameset_prompt_frame,
		const char *frameset_table_frame,
		boolean is_drillthru )
{
	if ( is_drillthru )
		return frameset_prompt_frame;
	else
		return frameset_table_frame;
}

char *form_prompt_process_drop_down_prompt_name(
		char *drop_down_prompt_name,
		char *optional_display )
{
	if ( optional_display )
		return optional_display;
	else
		return drop_down_prompt_name;
}

char *form_prompt_process_drop_down_name(
		FOLDER *folder,
		char *process_parameter_drop_down_name )
{
	if ( folder )
	{
		return folder->folder_name;
	}
	else
	{
		return process_parameter_drop_down_name;
	}
}
