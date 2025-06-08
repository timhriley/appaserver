/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_lookup.c			*/
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
#include "widget.h"
#include "sql.h"
#include "javascript.h"
#include "frameset.h"
#include "button.h"
#include "folder.h"
#include "appaserver.h"
#include "post_prompt_lookup.h"
#include "form_prompt_lookup.h"

FORM_PROMPT_LOOKUP *form_prompt_lookup_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean omit_insert_button,
		boolean omit_delete_button,
		boolean include_chart_buttons,
		boolean include_sort_button,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *relation_one2m_join_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		boolean drillthru_status_active_boolean,
		char *post_choose_folder_action_string )
{
	FORM_PROMPT_LOOKUP *form_prompt_lookup;
	char *tmp;

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

	form_prompt_lookup = form_prompt_lookup_calloc();

	form_prompt_lookup->action_string =
		form_prompt_lookup_action_string(
			POST_PROMPT_LOOKUP_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	form_prompt_lookup->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_PROMPT_LOOKUP_NAME,
			form_prompt_lookup->action_string,
			FRAMESET_TABLE_FRAME /* target_frame */ );

	form_prompt_lookup->no_display_all_widget_list =
		/* --------------------------------------- */
		/* Note: ends with dangling table open tag */
		/* --------------------------------------- */
		form_prompt_lookup_no_display_all_widget_list();

	form_prompt_lookup->radio_pair_list =
		form_prompt_lookup_radio_pair_list(
			omit_insert_button,
			omit_delete_button,
			include_chart_buttons,
			include_sort_button,
			list_length( relation_mto1_list ) );

	form_prompt_lookup->radio_list =
		radio_list_new(
			FORM_RADIO_LIST_NAME,
			form_prompt_lookup->radio_pair_list,
			RADIO_LOOKUP_VALUE /* initial_value */ );

	form_prompt_lookup->form_prompt_lookup_widget_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_prompt_lookup_widget_list_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			post_change_javascript,
			relation_mto1_list,
			relation_mto1_isa_list,
			relation_one2m_join_list,
			folder_attribute_append_isa_list,
			drillthru_dictionary,
			drillthru_status_active_boolean,
			(SECURITY_ENTITY *)0 /* security_entity */,
			1 /* no_display_boolean */,
			1 /* drop_down_extra_options_boolean */ );

	if ( !form_prompt_lookup->
		form_prompt_lookup_widget_list )
	{
		char message[ 128 ];

		sprintf(message,
	"form_prompt_lookup->form_prompt_lookup_widget_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_set(
		form_prompt_lookup->
			form_prompt_lookup_widget_list->
			widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	form_prompt_lookup->
		widget_hidden_container_list =
			widget_hidden_container_list(
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
				drillthru_dictionary );

	list_set_list(
		form_prompt_lookup->
			form_prompt_lookup_widget_list->
			widget_container_list,
		form_prompt_lookup->
			widget_hidden_container_list );

	list_set_list(
		form_prompt_lookup->
			form_prompt_lookup_widget_list->
			widget_container_list,
		widget_container_back_to_top_list() );

	form_prompt_lookup->form_multi_select_all_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_multi_select_all_javascript(
			form_prompt_lookup->
				form_prompt_lookup_widget_list->
				widget_container_list );

	form_prompt_lookup->ajax_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_prompt_lookup_ajax_javascript(
			session_key,
			login_name,
			role_name,
			form_prompt_lookup->
				form_prompt_lookup_widget_list->
				form_prompt_lookup_relation_list );

	form_prompt_lookup->recall_save =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recall_save_new(
			APPASERVER_LOOKUP_STATE,
			FORM_PROMPT_LOOKUP_NAME,
			folder_name,
			form_prompt_lookup->
				form_prompt_lookup_widget_list->
				widget_container_list,
			application_ssl_support_boolean(
				application_name ) );

/*
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: recall_save=\n%s\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	recall_save_display( form_prompt_lookup->recall_save ) );
msg( (char *)0, message );
}
*/
	form_prompt_lookup->recall_load =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recall_load_new(
			APPASERVER_LOOKUP_STATE,
			FORM_PROMPT_LOOKUP_NAME,
			folder_name,
			form_prompt_lookup->
				form_prompt_lookup_widget_list->
				widget_container_list );

/*
{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: recall_load=\n%s\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	recall_load_display( form_prompt_lookup->recall_load ) );
msg( (char *)0, message );
}
*/
	form_prompt_lookup->button_list =
		form_prompt_lookup_button_list(
			FORM_PROMPT_LOOKUP_NAME,
			0 /* not button_drillthru_skip_boolean */,
			post_choose_folder_action_string,
			form_prompt_lookup->
				form_multi_select_all_javascript,
			form_prompt_lookup->
				recall_save->
				javascript,
			form_prompt_lookup->
				recall_load->
				javascript );

	form_prompt_lookup->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_lookup_html(
			form_prompt_lookup->form_tag,
			form_prompt_lookup->no_display_all_widget_list,
			form_prompt_lookup->radio_list->html,
			form_prompt_lookup->ajax_javascript,
			form_prompt_lookup->
				form_prompt_lookup_widget_list->
				widget_container_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = button_list_html(
				   form_prompt_lookup->button_list ) )
					     /* button_list_html */,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	/* Free button_list_html() */
	/* ----------------------- */
	free( tmp );

	button_list_free( form_prompt_lookup->button_list );

	return form_prompt_lookup;
}

LIST *form_prompt_lookup_radio_pair_list(
		boolean omit_insert_button,
		boolean omit_delete_button,
		boolean include_chart_buttons,
		boolean include_sort_button,
		int relation_mto1_list_length )
{
	LIST *radio_pair_list = list_new();

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_STATISTICS_VALUE,
			RADIO_STATISTICS_DISPLAY ) );

	if ( relation_mto1_list_length )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_GROUP_COUNT_VALUE,
				RADIO_GROUP_COUNT_DISPLAY ) );
	}

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_SPREADSHEET_VALUE,
			RADIO_SPREADSHEET_DISPLAY ) );

	if ( !omit_insert_button )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_INSERT_VALUE,
				RADIO_INSERT_DISPLAY ) );
	}

	if ( !omit_delete_button )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_DELETE_VALUE,
				RADIO_DELETE_DISPLAY ) );
	}

	if ( include_chart_buttons )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_RCHART_VALUE,
				RADIO_RCHART_DISPLAY ) );

		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_GRACE_VALUE,
				RADIO_GRACE_DISPLAY ) );

		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_GOOGLE_VALUE,
				RADIO_GOOGLE_DISPLAY ) );

		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_HISTOGRAM_VALUE,
				RADIO_HISTOGRAM_DISPLAY ) );
	}

	if ( include_sort_button )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_SORT_VALUE,
				RADIO_SORT_DISPLAY ) );
	}

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_LOOKUP_VALUE,
			RADIO_LOOKUP_DISPLAY ) );

	return radio_pair_list;
}

FORM_PROMPT_LOOKUP *form_prompt_lookup_calloc( void )
{
	FORM_PROMPT_LOOKUP *form_prompt_lookup;

	if ( ! ( form_prompt_lookup =
			calloc( 1, sizeof ( FORM_PROMPT_LOOKUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup;
}

FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute_new(
			char *application_name,
			char *login_name,
			char *attribute_name,
			char *folder_attribute_prompt,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			boolean no_display_boolean,
			LIST *form_prompt_lookup_relation_list )
{
	FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute;

	if ( !attribute_name
	||   !folder_attribute_prompt
	||   !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( form_prompt_lookup_relation_attribute_exists(
		attribute_name,
		form_prompt_lookup_relation_list ) )
	{
		return NULL;
	}

	form_prompt_lookup_attribute = form_prompt_lookup_attribute_calloc();

	form_prompt_lookup_attribute->widget_container_list = list_new();

	if ( no_display_boolean )
	{
		list_set(
			form_prompt_lookup_attribute->widget_container_list,
			widget_container_new( table_data, (char *)0 ) );

		list_set(
		   form_prompt_lookup_attribute->widget_container_list,
		   ( form_prompt_lookup_attribute->
		    	no_display_widget_container =
		      	widget_container_new(
				checkbox,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				form_prefix_widget_name(
					DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
					attribute_name ) ) ) );

		form_prompt_lookup_attribute->
			no_display_widget_container->
			checkbox->
			prompt = FORM_NO_DISPLAY_HEADING;

		form_prompt_lookup_attribute->
			no_display_widget_container->
			recall_boolean = 1;
	}

	form_prompt_lookup_attribute->form_prompt_attribute =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_prompt_attribute_new(
			application_name,
			login_name,
			attribute_name,
			folder_attribute_prompt,
			datatype_name,
			attribute_width,
			hint_message );

	list_set_list(
		form_prompt_lookup_attribute->widget_container_list,
		form_prompt_lookup_attribute->
			form_prompt_attribute->
			widget_container_list );

	return form_prompt_lookup_attribute;
}

FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute_calloc( void )
{
	FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute;

	if ( ! ( form_prompt_lookup_attribute =
			calloc( 1, sizeof ( FORM_PROMPT_LOOKUP_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup_attribute;
}

FORM_PROMPT_LOOKUP_WIDGET_LIST *
	form_prompt_lookup_widget_list_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *relation_one2m_join_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		boolean drillthru_status_active_boolean,
		SECURITY_ENTITY *security_entity,
		boolean no_display_boolean,
		boolean drop_down_extra_options_boolean )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	FORM_PROMPT_LOOKUP_WIDGET_LIST *form_prompt_lookup_widget_list;
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;
	FORM_PROMPT_LOOKUP_YES_NO *form_prompt_lookup_yes_no;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name
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

	form_prompt_lookup_widget_list =
		form_prompt_lookup_widget_list_calloc();

	form_prompt_lookup_widget_list->widget_container_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		list_set(
			form_prompt_lookup_widget_list->widget_container_list,
			widget_container_new(
				table_row, (char *)0 ) );

		if ( ( form_prompt_lookup_relation =
			    form_prompt_lookup_relation_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name
					/* many_folder_name */,
				relation_mto1_list
					/* many_folder_relation_mto1_list */,
				relation_mto1_isa_list,
				folder_attribute->attribute_name,
				folder_attribute->primary_key_index,
				drillthru_dictionary,
				drillthru_status_active_boolean,
				security_entity,
				no_display_boolean,
				drop_down_extra_options_boolean,
				form_prompt_lookup_widget_list->
					form_prompt_lookup_relation_list ) ) )
		{
			if ( !form_prompt_lookup_widget_list->
				form_prompt_lookup_relation_list )
			{
				form_prompt_lookup_widget_list->
					form_prompt_lookup_relation_list =
						list_new();
			}

			list_set(
				form_prompt_lookup_widget_list->
					form_prompt_lookup_relation_list,
				form_prompt_lookup_relation );

			list_set_list(
				form_prompt_lookup_widget_list->
					widget_container_list,
				form_prompt_lookup_relation->
					widget_container_list );

			continue;
		}

		if ( ( form_prompt_lookup_yes_no =
			form_prompt_lookup_yes_no_new(
				folder_attribute->attribute_name,
				folder_attribute->prompt,
				folder_attribute->attribute->hint_message,
				post_change_javascript,
				no_display_boolean,
				form_prompt_lookup_widget_list->
					form_prompt_lookup_relation_list ) ) )
		{
			list_set_list(
				form_prompt_lookup_widget_list->
					widget_container_list,
				form_prompt_lookup_yes_no->
					widget_container_list );

			continue;
		}

		if ( ( form_prompt_lookup_widget_list->
			form_prompt_lookup_attribute =
			   form_prompt_lookup_attribute_new(
				application_name,
				login_name,
				folder_attribute->attribute_name,
				folder_attribute->prompt,
				folder_attribute->
					attribute->
					datatype_name,
				folder_attribute->
					attribute->
					width,
				folder_attribute->
					attribute->
					hint_message,
				no_display_boolean,
				form_prompt_lookup_widget_list->
				    form_prompt_lookup_relation_list ) ) )
		{
			list_set_list(
				form_prompt_lookup_widget_list->
					widget_container_list,
				form_prompt_lookup_widget_list->
					form_prompt_lookup_attribute->
					widget_container_list );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( list_length( relation_one2m_join_list ) )
	{
		form_prompt_lookup_widget_list->
			join_widget_container_list =
			   form_prompt_lookup_join_container_list(
				DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
				relation_one2m_join_list );

		list_set_list(
			form_prompt_lookup_widget_list->widget_container_list,
			form_prompt_lookup_widget_list->
				join_widget_container_list );
	}

	return form_prompt_lookup_widget_list;
}

LIST *form_prompt_lookup_join_container_list(
			char *dictionary_separate_no_display_prefix,
			LIST *relation_one2m_join_list )
{
	LIST *widget_container_list;
	WIDGET_CONTAINER *widget_container;
	RELATION_ONE2M *relation_one2m;
	char widget_name[ 128 ];

	if ( !list_rewind( relation_one2m_join_list ) ) return (LIST *)0;

	widget_container_list = list_new();

	do {
		relation_one2m =
			list_get(
				relation_one2m_join_list );

		if ( !relation_one2m->many_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: many_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			widget_container_list,
			widget_container_new(
				table_row, (char *)0 ) );

		list_set(
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		sprintf(widget_name,
			"%s%s",
			dictionary_separate_no_display_prefix,
			relation_one2m->many_folder->folder_name );

		list_set(
			widget_container_list,
			( widget_container =
				widget_container_new(
					checkbox,
					strdup( widget_name ) ) ) );

		widget_container->checkbox->prompt =
			FORM_NO_DISPLAY_HEADING;

		widget_container->recall_boolean = 1;

	} while ( list_next( relation_one2m_join_list ) );

	return widget_container_list;
}

FORM_PROMPT_LOOKUP_WIDGET_LIST *
	form_prompt_lookup_widget_list_calloc(
			void )
{
	FORM_PROMPT_LOOKUP_WIDGET_LIST *form_prompt_lookup_widget_list;

	if ( ! ( form_prompt_lookup_widget_list =
			calloc( 1,
				sizeof ( FORM_PROMPT_LOOKUP_WIDGET_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup_widget_list;
}

FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		char *attribute_name,
		int primary_key_index,
		DICTIONARY *drillthru_dictionary,
		boolean drillthru_status_active_boolean,
		SECURITY_ENTITY *security_entity,
		boolean no_display_boolean,
		boolean drop_down_extra_options_boolean,
		LIST *form_prompt_lookup_relation_list )
{
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;
	LIST *mto1_to_one_list;

	if ( !application_name
	||   !login_name
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

	if ( !list_length( many_folder_relation_mto1_list )
	&&   !list_length( relation_mto1_isa_list ) )
	{
		return NULL;
	}

	if ( form_prompt_lookup_relation_attribute_exists(
		attribute_name,
		form_prompt_lookup_relation_list ) )
	{
		return NULL;
	}

	form_prompt_lookup_relation = form_prompt_lookup_relation_calloc();

	form_prompt_lookup_relation->relation_mto1 =
		relation_mto1_consumes(
			attribute_name,
			many_folder_relation_mto1_list );

	if ( !form_prompt_lookup_relation->relation_mto1 )
	{
		form_prompt_lookup_relation->relation_mto1 =
			relation_mto1_isa_consumes(
				attribute_name,
				relation_mto1_isa_list );

		if ( !form_prompt_lookup_relation->relation_mto1 )
		{
			free( form_prompt_lookup_relation );
			return NULL;
		}
	}

	if ( primary_key_index
	&&   form_prompt_lookup_relation->
		relation_mto1->
		relation->
		relation_type_isa )
	{
		free( form_prompt_lookup_relation );
		return NULL;
	}

	if ( !form_prompt_lookup_relation->relation_mto1->one_folder
	||   !list_length(
			form_prompt_lookup_relation->
				relation_mto1->
				one_folder->
				folder_attribute_primary_key_list )
	||   !form_prompt_lookup_relation->relation_mto1->relation )
	{
		char message[ 128 ];

		sprintf(message, "relation_mto1 is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_lookup_relation->widget_container_list = list_new();

	if ( no_display_boolean )
	{
		list_set(
			form_prompt_lookup_relation->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_lookup_relation->widget_container_list,
			( form_prompt_lookup_relation->
				no_display_widget_container =
				   widget_container_new(
				   checkbox,
				   /* ------------------- */
				   /* Returns heap memory */
				   /* ------------------- */
				   form_prefix_widget_name(
					DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
					form_prompt_lookup_relation->
						relation_mto1->
						relation_name
						/* attribute_name */ ) ) ) );

		form_prompt_lookup_relation->
			no_display_widget_container->
			checkbox->
			prompt = FORM_NO_DISPLAY_HEADING;

		form_prompt_lookup_relation->
			no_display_widget_container->
			recall_boolean = 1;
	}

	list_set(
		form_prompt_lookup_relation->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_lookup_relation->widget_container_list,
		( form_prompt_lookup_relation->prompt_widget_container =
			widget_container_new(
				non_edit_text,
				form_prompt_lookup_relation->
					relation_mto1->
					relation_prompt ) ) );

	list_set(
		form_prompt_lookup_relation->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	if ( !form_prompt_lookup_relation->
		relation_mto1->
		relation->
		omit_drillthru )
	{
		mto1_to_one_list =
			relation_mto1_list(
				role_name,
				form_prompt_lookup_relation->
					relation_mto1->
					one_folder_name
						/* many_folder_name */,
				form_prompt_lookup_relation->
					relation_mto1->
					one_folder->
					folder_attribute_primary_key_list
					/* many_folder_primary_key_list */ );

		form_prompt_lookup_relation->query_drop_down =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_drop_down_new(
				application_name,
				session_key,
				login_name,
				role_name,
				APPASERVER_LOOKUP_STATE,
				many_folder_name,
				form_prompt_lookup_relation->
					relation_mto1->
					one_folder->
					folder_name,
				form_prompt_lookup_relation->
					relation_mto1->
					one_folder->
					folder_attribute_list,
				form_prompt_lookup_relation->
					relation_mto1->
					one_folder->
					populate_drop_down_process_name,
				mto1_to_one_list,
				drillthru_dictionary
					/* process_command_line_dictionary */,
				security_entity,
				(LIST *)0 /* common_name_list */ );

		form_prompt_lookup_relation->
			widget_drop_down_option_list =
				widget_drop_down_option_list(
					ATTRIBUTE_MULTI_KEY_DELIMITER,
					WIDGET_DROP_DOWN_LABEL_DELIMITER,
					WIDGET_DROP_DOWN_EXTRA_DELIMITER,
					WIDGET_DROP_DOWN_DASH_DELIMITER,
					form_prompt_lookup_relation->
						query_drop_down->delimited_list,
					form_prompt_lookup_relation->
						relation_mto1->
						one_folder->
						no_initial_capital );
	}

	if ( form_prompt_lookup_relation->
		relation_mto1->
		relation->
		drop_down_multi_select )
	{
		list_set(
			form_prompt_lookup_relation->widget_container_list,
			( form_prompt_lookup_relation->
				drop_down_widget_container =
					widget_container_new(
						multi_drop_down,
						form_prompt_lookup_relation->
							relation_mto1->
							relation_name ) ) );

		form_prompt_lookup_relation->
			drop_down_widget_container->
			multi_drop_down->
			original_drop_down->
			widget_drop_down_option_list =
				form_prompt_lookup_relation->
					widget_drop_down_option_list;

		form_prompt_lookup_relation->
			drop_down_widget_container->
			multi_drop_down->
			original_drop_down->
			display_size = WIDGET_MULTI_DISPLAY_SIZE;

		if ( drop_down_extra_options_boolean )
		{
			form_prompt_lookup_relation->
				drop_down_widget_container->
				multi_drop_down->
				original_drop_down->
				null_boolean = 1;

			form_prompt_lookup_relation->
				drop_down_widget_container->
				multi_drop_down->
				original_drop_down->
				not_null_boolean = 1;
		}

		goto form_prompt_lookup_relation_hint_message;
	}

	form_prompt_lookup_relation->ajax_client =
		form_prompt_lookup_relation_ajax_client(
			role_name,
			form_prompt_lookup_relation->
				relation_mto1 );

	if ( form_prompt_lookup_relation->ajax_client )
	{
		form_prompt_lookup_relation->
			drop_down_widget_container =
				list_first(
					form_prompt_lookup_relation->
						ajax_client->
						widget_container_list );

		form_prompt_lookup_relation->
			drop_down_widget_container->
				recall_boolean = 1;

		list_set_list(
			form_prompt_lookup_relation->
				widget_container_list,
			form_prompt_lookup_relation->
				ajax_client->
				widget_container_list );

		goto form_prompt_lookup_relation_hint_message;
	}

	list_set(
		form_prompt_lookup_relation->widget_container_list,
		( form_prompt_lookup_relation->
			drop_down_widget_container =
				widget_container_new(
					drop_down,
					form_prompt_lookup_relation->
						relation_mto1->
						relation_name ) ) );

	form_prompt_lookup_relation->
		drop_down_widget_container->
		drop_down->
		widget_drop_down_option_list =
			form_prompt_lookup_relation->
				widget_drop_down_option_list;

	form_prompt_lookup_relation->
		drop_down_widget_container->
		drop_down->
		display_size = 1;

	/* De Morgan's law:				*/
	/* If active_boolean && drillthru, then skip	*/
	/* -------------------------------------------- */
	if ( !drillthru_status_active_boolean
	||   !form_prompt_lookup_relation->
		relation_mto1->
		one_folder->
		drillthru )
	{
		form_prompt_lookup_relation->
			drop_down_widget_container->
			drop_down->
			top_select_boolean = 1;

		if ( drop_down_extra_options_boolean )
		{
			form_prompt_lookup_relation->
				drop_down_widget_container->
				drop_down->
				null_boolean = 1;

			form_prompt_lookup_relation->
				drop_down_widget_container->
				drop_down->
				not_null_boolean = 1;
		}
	}
	else
	{
		form_prompt_lookup_relation->
			drop_down_widget_container->
			drop_down->
			bottom_select_boolean = 1;
	}
	
	form_prompt_lookup_relation->
		drop_down_widget_container->
		recall_boolean = 1;

form_prompt_lookup_relation_hint_message:

	if ( form_prompt_lookup_relation->
		relation_mto1->
		relation->
		hint_message )
	{
		list_set(
			form_prompt_lookup_relation->
				widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_lookup_relation->widget_container_list,
			( form_prompt_lookup_relation->
				hint_message_widget_container =
					widget_container_new(
						non_edit_text,
						form_prompt_lookup_relation->
							relation_mto1->
							relation->
							hint_message ) ) );

		form_prompt_lookup_relation->
			hint_message_widget_container->
			non_edit_text->
			no_initial_capital_boolean = 1;
	}

	return form_prompt_lookup_relation;
}

boolean form_prompt_lookup_relation_attribute_exists(
		char *attribute_name,
		LIST *form_prompt_lookup_relation_list )
{
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;

	if ( list_rewind( form_prompt_lookup_relation_list ) )
	do {
		form_prompt_lookup_relation =
			list_get(
				form_prompt_lookup_relation_list );

		if ( list_string_exists(
			attribute_name,
			form_prompt_lookup_relation->
				relation_mto1->
				relation_foreign_key_list ) )
		{
			return 1;
		}

	} while ( list_next( form_prompt_lookup_relation_list ) );

	return 0;
}

FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation_calloc( void )
{
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;

	if ( ! ( form_prompt_lookup_relation =
			calloc( 1, sizeof ( FORM_PROMPT_LOOKUP_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup_relation;
}

char *form_prompt_lookup_html(
		char *form_tag,
		LIST *no_display_all_widget_list,
		char *radio_list_html,
		char *ajax_javascript,
		LIST *widget_container_list,
		char *button_list_html,
		char *form_close_tag )
{
	char *no_display_all_html = {0};
	char *container_list_html;
	char *hidden_html;
	char html[ STRING_704K ];

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

	if ( list_length( no_display_all_widget_list ) )
	{
		no_display_all_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_list_html(
				(char *)0 /* state */,
				JAVASCRIPT_PROMPT_ROW_NUMBER
					/* probably 0 */,
				(char *)0 /* background_color */,
				no_display_all_widget_list );
	}

	container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			JAVASCRIPT_PROMPT_ROW_NUMBER
				/* probably 0 */,
			(char *)0 /* background_color */,
			widget_container_list );

	hidden_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_hidden_html(
			JAVASCRIPT_PROMPT_ROW_NUMBER
				/* probably 0 */,
			widget_container_list );

	if (	string_strlen( form_tag ) +
		string_strlen( no_display_all_html ) +
		string_strlen( button_list_html ) +
		string_strlen( radio_list_html ) +
		string_strlen( ajax_javascript ) +
		string_strlen( container_list_html ) +
		string_strlen( hidden_html ) +
		string_strlen( form_close_tag ) + 6 >= STRING_704K )
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

	snprintf(
		html,
		sizeof ( html ),
		"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
		form_tag,
		(ajax_javascript) ? ajax_javascript : "",
		button_list_html,
		(no_display_all_html) ? no_display_all_html : "",
		(radio_list_html) ? radio_list_html : "",
		container_list_html,
		(hidden_html) ? hidden_html : "",
		form_close_tag );

	if ( no_display_all_html ) free( no_display_all_html );

	free( container_list_html );

	if ( hidden_html ) free( hidden_html );

	return strdup( html );
}

LIST *form_prompt_lookup_button_list(
		char *form_name,
		boolean button_drillthru_skip_boolean,
		char *post_choose_folder_action_string,
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *recall_load_javascript )
{
	LIST *button_list;

	if ( !form_name )
	{
		char message[ 128 ];

		sprintf(message, "form_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	button_list = list_new();

	list_set(
		button_list,
		button_submit(
			form_name,
			form_multi_select_all_javascript,
			recall_save_javascript,
			(char *)0
			/* form_verify_notepad_widths_javascript */ ) );

	list_set(
		button_list,
		button_reset(
			form_name,
			(char *)0 /* javascript_replace */ ) );

	list_set(
		button_list,
		button_back() );

	list_set(
		button_list,
		button_forward() );

	list_set(
		button_list,
		button_recall( recall_load_javascript ) );

	if ( post_choose_folder_action_string )
	{
		list_set(
			button_list,
			button_restart_drillthru(
				post_choose_folder_action_string ) );
	}

	if ( button_drillthru_skip_boolean )
	{
		list_set(
			button_list,
			button_drillthru_skip() );
	}

	return button_list;
}

char *form_prompt_lookup_action_string(
		char *post_prompt_lookup_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	char action_string[ 1024 ];

	if ( !post_prompt_lookup_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s",
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
		post_prompt_lookup_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name );

	return strdup( action_string );
}

LIST *form_prompt_lookup_no_display_all_widget_list( void )
{
	LIST *widget_container_list = list_new();
	WIDGET_CONTAINER *widget_container;

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
		( widget_container =
			widget_container_new(
			checkbox, (char *)0 ) ) );

	widget_container->
		checkbox->
		prompt = FORM_NO_DISPLAY_HEADING;

	widget_container->
		checkbox->
		onclick =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			form_prompt_lookup_no_display_all_onclick(
				DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
			table_data, (char *)0 ) ) );

	widget_container->
		table_data->
		column_span = 99;

	return widget_container_list;
}

char *form_prompt_lookup_no_display_all_onclick(
		char *form_no_display_prefix )
{
	static char onclick[ 128 ];

	sprintf(onclick,
		"timlib_set_all_push_buttons( this, '%s' )",
		form_no_display_prefix );

	return onclick;
}

FORM_PROMPT_LOOKUP_YES_NO *form_prompt_lookup_yes_no_new(
		char *attribute_name,
		char *folder_attribute_prompt,
		char *hint_message,
		char *post_change_javascript,
		boolean no_display_boolean,
		LIST *form_prompt_lookup_relation_list )
{
	FORM_PROMPT_LOOKUP_YES_NO *form_prompt_lookup_yes_no;
	WIDGET_CONTAINER *no_display_widget_container;
	WIDGET_CONTAINER *prompt_widget_container;
	WIDGET_CONTAINER *yes_no_widget_container;

	if ( !attribute_name
	||   !folder_attribute_prompt )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( form_prompt_lookup_relation_attribute_exists(
		attribute_name,
		form_prompt_lookup_relation_list ) )
	{
		return NULL;
	}

	if ( !attribute_is_yes_no( attribute_name ) )
	{
		return NULL;
	}

	form_prompt_lookup_yes_no = form_prompt_lookup_yes_no_calloc();

	form_prompt_lookup_yes_no->widget_container_list = list_new();

	if ( no_display_boolean )
	{
		list_set(
			form_prompt_lookup_yes_no->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_lookup_yes_no->widget_container_list,
			( no_display_widget_container =
			     widget_container_new(
				checkbox,
				form_prefix_widget_name(
					DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
					attribute_name ) ) ) );

		no_display_widget_container->checkbox->prompt =
			FORM_NO_DISPLAY_HEADING;

		no_display_widget_container->
			recall_boolean = 1;
	}

	list_set(
		form_prompt_lookup_yes_no->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_lookup_yes_no->widget_container_list,
		( prompt_widget_container =
			widget_container_new(
				non_edit_text,
				folder_attribute_prompt ) ) );

	list_set(
		form_prompt_lookup_yes_no->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_lookup_yes_no->widget_container_list,
		( yes_no_widget_container =
			widget_container_new(
				yes_no,
				attribute_name ) ) );

	yes_no_widget_container->
		yes_no->
		post_change_javascript =
			post_change_javascript;

	yes_no_widget_container->recall_boolean = 1;

	if ( hint_message )
	{
		WIDGET_CONTAINER *hint_message_widget_container;

		list_set(
			form_prompt_lookup_yes_no->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_lookup_yes_no->widget_container_list,
			( hint_message_widget_container =
				widget_container_new(
					non_edit_text,
					hint_message
						/* widget_name */ ) ) );

		hint_message_widget_container->
			non_edit_text->
			no_initial_capital_boolean = 1;
	}

	return form_prompt_lookup_yes_no;
}

FORM_PROMPT_LOOKUP_YES_NO *form_prompt_lookup_yes_no_calloc( void )
{
	FORM_PROMPT_LOOKUP_YES_NO *form_prompt_lookup_yes_no;

	if ( ! ( form_prompt_lookup_yes_no =
			calloc( 1,
				sizeof ( FORM_PROMPT_LOOKUP_YES_NO ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_prompt_lookup_yes_no;
}

AJAX_CLIENT *form_prompt_lookup_relation_ajax_client(
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

LIST *form_prompt_lookup_relation_ajax_client_list(
		LIST *form_prompt_lookup_relation_list )
{
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;
	LIST *ajax_client_list = {0};

	if ( list_rewind( form_prompt_lookup_relation_list ) )
	do {
		form_prompt_lookup_relation =
			list_get(
				form_prompt_lookup_relation_list );

		if ( form_prompt_lookup_relation->ajax_client )
		{
			if ( !ajax_client_list ) ajax_client_list = list_new();

			list_set(
				ajax_client_list,
				form_prompt_lookup_relation->ajax_client );
		}

	} while ( list_next( form_prompt_lookup_relation_list ) );

	return ajax_client_list;
}

char *form_prompt_lookup_ajax_javascript(
		char *session_key,
		char *login_name,
		char *role_name,
		LIST *form_prompt_lookup_relation_list )
{
	LIST *relation_ajax_client_list;
	char *ajax_javascript = {0};

	relation_ajax_client_list =
		form_prompt_lookup_relation_ajax_client_list(
			form_prompt_lookup_relation_list );

	if ( list_length( relation_ajax_client_list ) )
	{
		ajax_javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			ajax_client_list_javascript(
				session_key,
				login_name,
				role_name,
				relation_ajax_client_list );
	}

	return ajax_javascript;
}

