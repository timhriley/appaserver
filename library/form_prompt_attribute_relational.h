/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_attribute_relational.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORM_PROMPT_ATTRIBUTE_H
#define FORM_PROMPT_ATTRIBUTE_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "element.h"

#define FORM_PROMPT_ATTRIBUTE_RELATIONAL_PREFIX \
					"rrelational_"

#define FORM_PROMPT_ATTRIBUTE_FROM_PREFIX \
					"ffrom_"

#define FORM_PROMPT_ATTRIBUTE_TO_PREFIX	"tto_"

typedef struct
{
	LIST *element_list;
	APPASERVER_ELEMENT *relation_operator_appaserver_element;
	APPASERVER_ELEMENT *text_from_appaserver_element;
	APPASERVER_ELEMENT *and_appaserver_element;
	APPASERVER_ELEMENT *text_to_appaserver_element;
} FORM_PROMPT_ATTRIBUTE_RELATIONAL;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_LOOKUP_RELATIONAL *
	form_prompt_lookup_relational_new(
			char *form_prompt_lookup_attribute_relational_name,
			char *form_prompt_lookup_attribute_from_name,
			char *form_prompt_lookup_attribute_to_name,
			char *datatype_name,
			int attribute_width );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_RELATIONAL *
	form_prompt_lookup_relational_calloc(
			void );

LIST *form_prompt_lookup_relational_operation_list(
			char *datatype_name );

typdef struct
{
} FORM_PROMPT_ATTRIBUTE_RELATIONAL;

/* Usage */
/* ----- */
FORM_PROMPT_ATTRIBUTE_RELATIONAL *
	form_prompt_attribute_relational_new(
			char *attribute_name,
			char *datatype_name,
			int attribute_width );

/* Process */
/* ------- */

/* Public */
/* ------ */

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

char *form_prompt_attribute_from_name(
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

char *form_prompt_attribute_to_name(
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

#endif
