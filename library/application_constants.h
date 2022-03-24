/* $APPASERVER_HOME/library/application_constants.h	*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef APPLICATION_CONSTANTS_H
#define APPLICATION_CONSTANTS_H

#include "dictionary.h"

#define APPLICATION_CONSTANTS_TABLE	"application_constants"

#define APPLICATION_CONSTANTS_SELECT	"application_constant,"		\
					"application_constant_value"

typedef struct
{
	DICTIONARY *dictionary;
} APPLICATION_CONSTANTS;

APPLICATION_CONSTANTS *application_constants_new( void );

DICTIONARY *application_constants_dictionary(
			void );

char *application_constants_fetch(
			DICTIONARY *application_constants_dictionary,
			char *key );

char *application_constants_safe_fetch(
			DICTIONARY *application_constants_dictionary,
			char *key );

void application_constants_free(
			APPLICATION_CONSTANTS *application_constants );

char *application_constants_quick_fetch(
			char *key );

boolean application_constants_cat_javascript_source(
			void );

#endif
