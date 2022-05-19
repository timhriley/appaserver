/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_process.c			*/
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
#include "post_prompt_process.h"
#include "form_prompt_attribute_relational.h"
#include "form_prompt_process.h"

FORM_PROMPT_PROCESS_ATTRIBUTE *
	form_prompt_process_attribute_new(
			char *post_change_javascript,
			char *attribute_name,
			char *datatype_name,
			int attribute_width,
			char *hint_message )
{
	FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute;

	if ( !attribute_name
	||   !datatype_name
	||   !attribute_width )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_process_attribute = form_prompt_process_attribute_calloc();

	form_prompt_process_attribute->element_list = list_new();

	list_set(
		form_prompt_process_attribute->element_list,
		( form_prompt_process_attribute->
			prompt_appaserver_element =
				appaserver_element_new(
					non_edit_text, (char *)0 ) ) );

	free( form_prompt_process_attribute->
			prompt_appaserver_element->
			non_edit_text );

	form_prompt_process_attribute->
		prompt_appaserver_element->
		non_edit_text =
			element_non_edit_text_new(
				(char *)0,
				attribute_name );

	list_set(
		form_prompt_process_attribute->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	form_prompt_process_attribute->
		form_prompt_attribute_relational =
			form_prompt_attribute_relational_new(
				attribute_name,
				datatype_name,
				attribute_width,
				hint_message,
				post_change_javascript );

	if ( !form_prompt_process_attribute->form_prompt_attribute_relational )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_attribute_relational_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set_list(
		form_prompt_process_attribute->element_list,
		form_prompt_process_attribute->
			form_prompt_attribute_relational->
			element_list );

	return form_prompt_process_attribute;
}

FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute_calloc( void )
{
	FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute;

	if ( ! ( form_prompt_process_attribute =
			calloc(	1,
				sizeof( FORM_PROMPT_PROCESS_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_attribute;
}

FORM_PROMPT_PROCESS_DROP_DOWN *
	form_prompt_process_drop_down_new(
			char *post_change_javascript,
			PROCESS_PARAMETER_DROP_DOWN *
				process_parameter_drop_down )
{
	FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down;

	if ( !process_parameter_drop_down )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: process_parameter_drop_down is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_process_drop_down = form_prompt_process_drop_down_calloc();

	form_prompt_process_drop_down->element_list = list_new();

	list_set(
		form_prompt_process_drop_down->element_list,
		( form_prompt_process_drop_down->
			prompt_appaserver_element =
				appaserver_element_new(
					non_edit_text, (char *)0 ) ) );

	free(	form_prompt_process_drop_down->
			prompt_appaserver_element->
			non_edit_text );

	form_prompt_process_drop_down->
		prompt_appaserver_element->
		non_edit_text =
			element_non_edit_text_new(
				(char *)0,
				process_parameter_drop_down->
					drop_down_name );

	list_set(
		form_prompt_process_drop_down->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	form_prompt_process_drop_down->
		drop_down_appaserver_element =
			appaserver_element_new(
				prompt_drop_down,
				process_parameter_drop_down->drop_down_name );

	form_prompt_process_drop_down->
		drop_down_appaserver_element->
		prompt_drop_down =
			element_prompt_drop_down_new(
				form_prompt_process_drop_down->
				drop_down_appaserver_element->
					element_name,
				process_parameter_drop_down->
					delimited_list,
				0 /* not no_initial_capital */,
				0 /* not output_null_option */,
				0 /* not output_not_null_option */,
				1 /* output_select_option */,
				1 /* display_size */,
				-1 /* appaserver_element_tab_order() */,
				process_parameter_drop_down->
					drop_down_multi_select,
				post_change_javascript,
				1 /* recall */ );

	list_set(
		form_prompt_process_drop_down->element_list,
		form_prompt_process_drop_down->drop_down_appaserver_element );

	return form_prompt_process_drop_down;
}

FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down_calloc( void )
{
	FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down;

	if ( ! ( form_prompt_process_drop_down =
			calloc(	1,
				sizeof( FORM_PROMPT_PROCESS_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_drop_down;
}

FORM_PROMPT_PROCESS_ELEMENT_LIST *
	form_prompt_process_element_list_new(
			LIST *process_parameter_list,
			char *post_change_javascript )
{
	FORM_PROMPT_PROCESS_ELEMENT_LIST *form_prompt_process_element_list;
	PROCESS_PARAMETER *process_parameter;

	if ( !list_rewind( process_parameter_list ) )
	{
		return (FORM_PROMPT_PROCESS_ELEMENT_LIST *)0;
	}

	form_prompt_process_element_list =
		form_prompt_process_element_list_calloc();

	form_prompt_process_element_list->element_list = list_new();

	do {
		process_parameter =
			list_get(
				process_parameter_list );

		list_set(
			form_prompt_process_element_list->element_list,
			appaserver_element_new(
				table_row, (char *)0 ) );

		list_set(
			form_prompt_process_element_list->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		if ( process_parameter->attribute )
		{
			form_prompt_process_element_list->
				form_prompt_process_attribute =
					form_prompt_process_attribute_new(
						post_change_javascript,
						process_parameter->
							attribute->
							attribute_name,
						process_parameter->
							attribute->
							datatype_name,
						process_parameter->
							attribute->
							width,
						process_parameter->
							attribute->
							hint_message );

			if ( !form_prompt_process_element_list->
				form_prompt_process_attribute )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_process_attribute_new() returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			list_set_list(
				form_prompt_process_element_list->element_list,
				form_prompt_process_element_list->
					form_prompt_process_attribute->
					element_list );
		}
		else
		if ( process_parameter->process_parameter_drop_down )
		{
			form_prompt_process_element_list->
				form_prompt_process_drop_down =
				    form_prompt_process_drop_down_new(
					post_change_javascript,
					process_parameter->
						process_parameter_drop_down );

			if ( !form_prompt_process_element_list->
				form_prompt_process_drop_down )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_process_drop_down_new() returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			list_set_list(
				form_prompt_process_element_list->
					element_list,
				form_prompt_process_element_list->
					form_prompt_process_drop_down->
						element_list );
		}

	} while ( list_next( process_parameter_list ) );

	form_prompt_process_element_list->appaserver_element_list_html =
		appaserver_element_list_html(
			form_prompt_process_element_list->element_list );

	return form_prompt_process_element_list;
}

FORM_PROMPT_PROCESS_ELEMENT_LIST *
	form_prompt_process_element_list_calloc(
			void )
{
	FORM_PROMPT_PROCESS_ELEMENT_LIST *form_prompt_process_element_list;

	if ( ! ( form_prompt_process_element_list =
			calloc(	1,
				sizeof( FORM_PROMPT_PROCESS_ELEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_element_list;
}

