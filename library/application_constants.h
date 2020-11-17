/* application_constants.h				*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef APPLICATION_CONSTANTS_H
#define APPLICATION_CONSTANTS_H

#include "dictionary.h"

typedef struct
{
	DICTIONARY *dictionary;
} APPLICATION_CONSTANTS;

APPLICATION_CONSTANTS *application_constants_new( void );

DICTIONARY *application_constants_dictionary(
				char *application_name );

DICTIONARY *application_constants_get_dictionary(
				char *application_name );

char *application_constants_fetch(
				DICTIONARY *application_constants_dictionary,
				char *key );

char *application_constants_safe_fetch(
				DICTIONARY *application_constants_dictionary,
				char *key );

void application_constants_get_easycharts_width_height(
				int *easycharts_width,
				int *easycharts_height,
				char *application_name );

void application_constants_free(APPLICATION_CONSTANTS *application_constants );

char *application_constants_quick_fetch(
				char *application_name,
				char *key );

boolean application_constants_cat_javascript_source(
				char *application_name );

#endif
