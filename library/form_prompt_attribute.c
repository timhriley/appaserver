/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_attribute.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "query.h"
#include "appaserver_error.h"
#include "widget.h"
#include "prompt_lookup.h"
#include "form_prompt_attribute.h"

FORM_PROMPT_ATTRIBUTE *form_prompt_attribute_new(
		char *application_name,
		char *login_name,
		char *attribute_name,
		char *folder_attribute_prompt,
		char *datatype_name,
		int attribute_width,
		char *hint_message )
{
	FORM_PROMPT_ATTRIBUTE *form_prompt_attribute;

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

	form_prompt_attribute =
		form_prompt_attribute_calloc();

	form_prompt_attribute->widget_container_list = list_new();

	list_set(
		form_prompt_attribute->widget_container_list,
		widget_container_new( table_data, (char *)0 ) );

	list_set(
		form_prompt_attribute->widget_container_list,
		widget_container_new(
			non_edit_text,
			folder_attribute_prompt ) );

	list_set(
		form_prompt_attribute->widget_container_list,
		widget_container_new( table_data, (char *)0 ) );

	list_set(
		form_prompt_attribute->widget_container_list,
		  widget_container_new(
			table_open,
			(char *)0 ) );

	list_set(
		form_prompt_attribute->widget_container_list,
		widget_container_new( table_row, (char *)0 ) );

	list_set(
		form_prompt_attribute->widget_container_list,
		widget_container_new( table_data, (char *)0 ) );

	list_set(
		form_prompt_attribute->widget_container_list,
		( form_prompt_attribute->
			relation_operator_widget_container =
			   widget_container_new(
				drop_down,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				form_prefix_widget_name(
					QUERY_RELATION_OPERATOR_PREFIX,
					attribute_name ) ) ) );

	form_prompt_attribute->
		relation_operator_widget_container->
		drop_down->
		widget_drop_down_option_list =
			form_prompt_attribute_option_list(
				datatype_name );

	form_prompt_attribute->
		relation_operator_widget_container->
			drop_down->
			top_select_boolean = 1;

	form_prompt_attribute->
		relation_operator_widget_container->
			drop_down->
			display_size = 1;

	form_prompt_attribute->
		relation_operator_widget_container->
		recall_boolean = 1;

	list_set(
		form_prompt_attribute->widget_container_list,
		widget_container_new( table_data, (char *)0 ) );

	if ( attribute_is_number( datatype_name )
	||   attribute_is_date( datatype_name )
	||   attribute_is_time( datatype_name )
	||   attribute_is_date_time( datatype_name )
	||   attribute_is_current_date_time( datatype_name ) )
	{
		form_prompt_attribute->from_widget_container =
		    form_prompt_attribute_from_container(
			application_name,
			login_name,
			datatype_name,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_prefix_widget_name(
				PROMPT_LOOKUP_FROM_PREFIX,
				attribute_name ) );

		list_set(
			form_prompt_attribute->widget_container_list,
			form_prompt_attribute->
				from_widget_container );

		list_set(
			form_prompt_attribute->widget_container_list,
			widget_container_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_attribute->widget_container_list,
			( form_prompt_attribute->
				and_widget_container =
				   widget_container_new(
					 non_edit_text,
					"and" /* widget_name */ ) ) );

		list_set(
			form_prompt_attribute->widget_container_list,
			widget_container_new( table_data, (char *)0 ) );

		form_prompt_attribute->to_widget_container =
			form_prompt_attribute_to_container(
				application_name,
				login_name,
				datatype_name,
				attribute_width,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				form_prefix_widget_name(
					PROMPT_LOOKUP_TO_PREFIX,
					attribute_name ) );

		list_set(
			form_prompt_attribute->widget_container_list,
			form_prompt_attribute->to_widget_container );
	}
	else
	{
		list_set(
			form_prompt_attribute->widget_container_list,
			( form_prompt_attribute->
				from_widget_container =
				    widget_container_new(
					text,
					attribute_name ) ) );

		form_prompt_attribute->
			from_widget_container->
				text->
				datatype_name = datatype_name;

		form_prompt_attribute->
			from_widget_container->
				text->
				widget_text_display_size =
					FORM_PROMPT_LOOKUP_FROM_SIZE;

		form_prompt_attribute->
			from_widget_container->
				text->
				attribute_width_max_length =
					FORM_PROMPT_LOOKUP_FROM_MAX_LENGTH;

		form_prompt_attribute->
			from_widget_container->
			recall_boolean = 1;
	}

	list_set(
		form_prompt_attribute->widget_container_list,
		widget_container_new(
			table_close,
			(char *)0 ) );

	if ( hint_message )
	{
		list_set(
			form_prompt_attribute->widget_container_list,
			widget_container_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_attribute->widget_container_list,
			( form_prompt_attribute->
				hint_message_widget_container =
					widget_container_new(
						non_edit_text,
						hint_message
							/* widget_name */ ) ) );

		form_prompt_attribute->
			hint_message_widget_container->
			non_edit_text->
			no_initial_capital_boolean = 1;
	}

	return form_prompt_attribute;
}

FORM_PROMPT_ATTRIBUTE *
	form_prompt_attribute_calloc(
		void )
{
	FORM_PROMPT_ATTRIBUTE *form_prompt_attribute;

	if ( ! ( form_prompt_attribute =
			calloc(	1,
				sizeof( FORM_PROMPT_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_attribute;
}

LIST *form_prompt_attribute_option_list( char *datatype_name )
{
	LIST *list;

	if ( !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "datatype_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	if ( attribute_is_date( datatype_name )
	||   attribute_is_time( datatype_name )
	||   attribute_is_date_time( datatype_name ) )
	{
		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_BEGINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_EQUAL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_BETWEEN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_OR ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_GREATER_THAN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_GREATER_THAN_EQUAL_TO ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_LESS_THAN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_LESS_THAN_EQUAL_TO ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_EQUAL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_IS_NULL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_NULL ) );
	}
	else
	if ( attribute_is_notepad( datatype_name ) )
	{
		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_CONTAINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_CONTAINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_IS_NULL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_NULL ) );
	}
	else
	if ( attribute_is_number( datatype_name ) )
	{
		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_EQUAL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_BETWEEN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_OR ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_GREATER_THAN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_GREATER_THAN_EQUAL_TO ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_LESS_THAN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_LESS_THAN_EQUAL_TO ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_EQUAL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_IS_NULL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_NULL ) );
	}
	else
	if ( attribute_is_text( datatype_name )
	||   attribute_is_upload( datatype_name ) )
	{
		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_BEGINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_EQUAL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_CONTAINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_CONTAINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_OR ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_GREATER_THAN_EQUAL_TO ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_EQUAL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_IS_NULL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_NULL ) );
	}
	else
	{
		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_EQUAL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_BETWEEN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_BEGINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_CONTAINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_CONTAINS ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_OR ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_EQUAL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_GREATER_THAN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_GREATER_THAN_EQUAL_TO ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_LESS_THAN ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_LESS_THAN_EQUAL_TO ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_IS_NULL ) );

		list_set(
			list,
			widget_drop_down_option_new(
				QUERY_NOT_NULL ) );
	}

	return list;
}

WIDGET_CONTAINER *form_prompt_attribute_from_container(
		char *application_name,
		char *login_name,
		char *datatype_name,
		char *form_prefix_widget_name )
{
	WIDGET_CONTAINER *from_widget_container = {0};

	if ( !datatype_name
	||   !form_prefix_widget_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( attribute_is_date( datatype_name )
	||   attribute_is_date_time( datatype_name )
	||   attribute_is_current_date_time( datatype_name ) )
	{
		from_widget_container =
			widget_container_new(
				widget_date,
				form_prefix_widget_name );

		from_widget_container->
			date->
			application_name =
				application_name;

		from_widget_container->
			date->
			login_name =
				login_name;

		from_widget_container->
			date->
			datatype_name =
				datatype_name;

		from_widget_container->
			date->
			display_size =
				FORM_PROMPT_LOOKUP_FROM_SIZE;

		from_widget_container->
			date->
			attribute_width_max_length =
				FORM_PROMPT_LOOKUP_FROM_MAX_LENGTH;
	}
	else
	{
		from_widget_container =
			widget_container_new(
				text,
				form_prefix_widget_name );

		from_widget_container->
			text->
			datatype_name = datatype_name;

		from_widget_container->
			text->
			widget_text_display_size =
				FORM_PROMPT_LOOKUP_FROM_SIZE;

		from_widget_container->
			text->
			attribute_width_max_length =
				FORM_PROMPT_LOOKUP_FROM_MAX_LENGTH;
	}

	from_widget_container->recall_boolean = 1;

	return from_widget_container;
}

WIDGET_CONTAINER *form_prompt_attribute_to_container(
		char *application_name,
		char *login_name,
		char *datatype_name,
		int attribute_width,
		char *form_prefix_widget_name )
{
	WIDGET_CONTAINER *to_widget_container = {0};

	if ( !datatype_name
	||   !form_prefix_widget_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( attribute_is_date( datatype_name )
	||   attribute_is_date_time( datatype_name )
	||   attribute_is_current_date_time( datatype_name ) )
	{
		to_widget_container =
			widget_container_new(
				widget_date,
				form_prefix_widget_name );

		to_widget_container->
			date->
			application_name =
				application_name;

		to_widget_container->
			date->
			login_name =
				login_name;

		to_widget_container->
			date->
			datatype_name =
				datatype_name;

		to_widget_container->
			date->
			display_size =
				FORM_PROMPT_LOOKUP_TO_SIZE;

		if ( attribute_is_date( datatype_name ) )
		{
			to_widget_container->
				date->
				attribute_width_max_length =
					WIDGET_DATE_DISPLAY_SIZE;
		}
		else
		{
			to_widget_container->
				date->
				attribute_width_max_length =
					WIDGET_DATE_TIME_DISPLAY_SIZE;
		}
	}
	else
	{
		to_widget_container =
			widget_container_new(
				text,
				form_prefix_widget_name );

		to_widget_container->
			text->
			datatype_name = datatype_name;

		to_widget_container->
			text->
			widget_text_display_size =
				FORM_PROMPT_LOOKUP_TO_SIZE;

		to_widget_container->
			text->
			attribute_width_max_length =
				attribute_width;
	}

	to_widget_container->recall_boolean = 1;

	return to_widget_container;
}

