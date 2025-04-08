/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_constant.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_CONSTANT_H
#define APPLICATION_CONSTANT_H

#include "dictionary.h"

#define APPLICATION_CONSTANT_TABLE	"application_constant"

#define APPLICATION_CONSTANT_SELECT	"application_constant,"		\
					"application_constant_value"

typedef struct
{
	DICTIONARY *dictionary;
} APPLICATION_CONSTANT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_CONSTANT *application_constant_new(
		void );

/* Process */
/* ------- */
APPLICATION_CONSTANT *application_constant_calloc(
		void );

DICTIONARY *application_constant_dictionary(
		void );

/* Usage */
/* ----- */

/* Returns component of application_constant->dictionary->get() or "" */
/* ------------------------------------------------------------------ */
char *application_constant_fetch(
		char *key );

/* Public */
/* ------ */

/* Returns component of application_constant_dictionary or null */
/* ------------------------------------------------------------ */
char *application_constant_get(
		char *key,
		DICTIONARY *application_constant_dictionary );

/* Returns component of application_constant_dictionary or "" */
/* ---------------------------------------------------------- */
char *application_constant_safe_get(
		char *key,
		DICTIONARY *application_constant_dictionary );

boolean application_constant_cat_javascript_source(
		void );

void application_constant_free(
		APPLICATION_CONSTANT *application_constants );

#endif
