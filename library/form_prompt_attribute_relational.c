/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/library/form_prompt_attribute_relational.c	*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "attribute.h"
#include "form.h"
#include "form_prompt_attribute_relational.h"

FORM_PROMPT_ATTRIBUTE_RELATIONAL *
	form_prompt_attribute_relational_new(
			char *attribute_name,
			char *datatype_name,
			int attribute_width,
			char *hint_message )
{
	FORM_PROMPT_ATTRIBUTE_RELATIONAL *form_prompt_attribute_relational =
		form_prompt_attribute_relational_calloc();

	form_prompt_attribute_relational->element_list = list_new();

	form_prompt_attribute_relational->from_name =
		form_prompt_attribute_relational_from_name(
			FORM_PROMPT_ATTRIBUTE_RELATIONAL_FROM_PREFIX,
			attribute_name );

	if ( attribute_is_yes_no( attribute_name ) )
	{
		list_set(
		   form_prompt_attribute_relational->element_list,
		   ( form_prompt_attribute_relational->
			yes_no_appaserver_element =
			   appaserver_element_new(
				yes_no,
				form_prompt_attribute_relational->
					from_name ) ) );

		form_prompt_attribute_relational->
			yes_no_appaserver_element->
			yes_no =
				element_yes_no_new(
					(char *)0 /* attribute_name */,
					form_prompt_attribute_relational->
						yes_no_appaserver_element->
						element_name,
					1 /* output_null_option */,
					1 /* output_not_null_option */,
					(char *)0 /* post_change_javascript */,
					-1 /* tab_order */,
					1 /* recall */ );

		list_set(
			form_prompt_attribute_relational->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_attribute_relational->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_attribute_relational->element_list,
			appaserver_element_new( table_data, (char *)0 ) );
	}
	else
	{
		form_prompt_attribute_relational->name =
			form_prompt_attribute_relational_name(
				FORM_PROMPT_ATTRIBUTE_RELATIONAL_PREFIX,
				attribute_name );

		form_prompt_attribute_relational->to_name =
			form_prompt_attribute_relational_to_name(
				FORM_PROMPT_ATTRIBUTE_RELATIONAL_TO_PREFIX,
				attribute_name );

		form_prompt_attribute_relational->operation_list =
			form_prompt_attribute_relational_operation_list(
				datatype_name );

		list_set(
			form_prompt_attribute_relational->element_list,
			( form_prompt_attribute_relational->
				relation_operator_appaserver_element =
				   appaserver_element_new(
					prompt_drop_down,
					form_prompt_attribute_relational->
						name ) ) );

		form_prompt_attribute_relational->
			relation_operator_appaserver_element->
			prompt_drop_down =
				element_prompt_drop_down_new(
				   form_prompt_attribute_relational->
					relation_operator_appaserver_element->
					element_name,
				   form_prompt_attribute_relational->
					operation_list,
				   0 /* not no_initial_capital */,
				   1 /* output_null_option */,
				   1 /* output_not_null_option */,
				   1 /* output_select_option */,
				   1 /* display_size */,
				   -1 /* tab_order */,
				   0 /* not multi_select */,
				   (char *)0 /* post_change_javascript */,
				   1 /* recall */ );

		list_set(
			form_prompt_attribute_relational->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_attribute_relational->element_list,
			( form_prompt_attribute_relational->
				text_from_appaserver_element =
				    appaserver_element_new(
					text,
					form_prompt_attribute_relational->
						from_name ) ) );

		form_prompt_attribute_relational->
			text_from_appaserver_element->text =
				element_text_new(
					form_prompt_attribute_relational->
						text_from_appaserver_element->
						element_name,
					datatype_name,
					FORM_FROM_ATTRIBUTE_WIDTH,
					0 /* not null_to_slash */,
					1 /* prevent_carrot */,
					(char *)0 /* on_change */,
					(char *)0 /* on_focus */,
					(char *)0 /* on_keyup */,
					-1 /* tab_order */,
					1 /* recall */ );

		list_set(
			form_prompt_attribute_relational->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_attribute_relational->element_list,
			( form_prompt_attribute_relational->
				and_appaserver_element =
				   appaserver_element_new(
					 non_edit_text, (char *)0 ) ) );

		form_prompt_attribute_relational->
			and_appaserver_element->non_edit_text =
				element_non_edit_text_new(
					(char *)0,
					"and" /* message */ );

		list_set(
			form_prompt_attribute_relational->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_attribute_relational->element_list,
			( form_prompt_attribute_relational->
				text_to_appaserver_element =
					appaserver_element_new(
					   text,
					   form_prompt_attribute_relational->
						to_name ) ) );

		form_prompt_attribute_relational->
			text_from_appaserver_element->text =
				element_text_new(
					form_prompt_attribute_relational->
						text_to_appaserver_element->
						element_name,
					datatype_name,
					attribute_width,
					0 /* not null_to_slash */,
					1 /* prevent_carrot */,
					(char *)0 /* on_change */,
					(char *)0 /* on_focus */,
					(char *)0 /* on_keyup */,
					-1 /* tab_order */,
					1 /* recall */ );
	}

	if ( hint_message )
	{
		list_set(
			form_prompt_attribute_relational->element_list,
				appaserver_element_new(
					table_data, (char *)0 ) );

		list_set(
			form_prompt_attribute_relational->element_list,
			( form_prompt_attribute_relational->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		form_prompt_attribute_relational->
			hint_message_appaserver_element->
			non_edit_text =
				element_non_edit_text_new(
					(char *)0 /* prompt_name */,
					hint_message );
	}

	return form_prompt_attribute_relational;
}

FORM_PROMPT_ATTRIBUTE_RELATIONAL *
	form_prompt_attribute_relational_calloc(
			void )
{
	FORM_PROMPT_ATTRIBUTE_RELATIONAL *
		form_prompt_attribute_relational;

	if ( ! ( form_prompt_attribute_relational =
			calloc(	1,
				sizeof( FORM_PROMPT_ATTRIBUTE_RELATIONAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_attribute_relational;
}

char *form_prompt_attribute_relational_name(
			char *relational_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	if ( !relational_prefix
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(name,
		"%s%s",
		relational_prefix,
		attribute_name );

	return strdup( name );
}

char *form_prompt_attribute_relational_from_name(
			char *from_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	if ( !from_prefix
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(name,
		"%s%s",
		from_prefix,
		attribute_name );

	return strdup( name );
}

char *form_prompt_attribute_relational_to_name(
			char *to_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	if ( !to_prefix
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(name,
		"%s%s",
		to_prefix,
		attribute_name );

	return strdup( name );
}

LIST *form_prompt_attribute_relational_operation_list(
			char *datatype_name )
{
	LIST *list;

	list = list_new();

	if ( attribute_is_date( datatype_name )
	||   attribute_is_time( datatype_name )
	||   attribute_is_date_time( datatype_name ) )
	{
		list_set( list, APPASERVER_BEGINS );
		list_set( list, APPASERVER_EQUAL );
		list_set( list, APPASERVER_BETWEEN );
		list_set( list, APPASERVER_OR );
		list_set( list, APPASERVER_GREATER_THAN );
		list_set( list, APPASERVER_GREATER_THAN_EQUAL_TO );
		list_set( list, APPASERVER_LESS_THAN );
		list_set( list, APPASERVER_LESS_THAN_EQUAL_TO );
		list_set( list, APPASERVER_NOT_EQUAL );
		list_set( list, APPASERVER_NULL );
		list_set( list, APPASERVER_NOT_NULL );
	}
	else
	if ( attribute_is_notepad( datatype_name ) )
	{
		list_set( list, APPASERVER_CONTAINS );
		list_set( list, APPASERVER_NOT_CONTAINS );
		list_set( list, APPASERVER_NULL );
		list_set( list, APPASERVER_NOT_NULL );
	}
	else
	if ( attribute_is_text( datatype_name )
	||   attribute_is_upload( datatype_name ) )
	{
		list_set( list, APPASERVER_BEGINS );
		list_set( list, APPASERVER_EQUAL );
		list_set( list, APPASERVER_CONTAINS );
		list_set( list, APPASERVER_NOT_CONTAINS );
		list_set( list, APPASERVER_OR );
		list_set( list, APPASERVER_GREATER_THAN_EQUAL_TO );
		list_set( list, APPASERVER_NOT_EQUAL );
		list_set( list, APPASERVER_NULL );
		list_set( list, APPASERVER_NOT_NULL );
	}
	else
	{
		list_set( list, APPASERVER_EQUAL );
		list_set( list, APPASERVER_BETWEEN );
		list_set( list, APPASERVER_BEGINS );
		list_set( list, APPASERVER_CONTAINS );
		list_set( list, APPASERVER_NOT_CONTAINS );
		list_set( list, APPASERVER_OR );
		list_set( list, APPASERVER_NOT_EQUAL );
		list_set( list, APPASERVER_GREATER_THAN );
		list_set( list, APPASERVER_GREATER_THAN_EQUAL_TO );
		list_set( list, APPASERVER_LESS_THAN );
		list_set( list, APPASERVER_LESS_THAN_EQUAL_TO );
		list_set( list, APPASERVER_NULL );
		list_set( list, APPASERVER_NOT_NULL );
	}

	return list;
}

