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

	if ( !attribute_name,
	||   !datatype_name,
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
		form_prompt_attribute_relational->element_list );

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

