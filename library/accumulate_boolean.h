/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/accumulate_boolean.h 			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#pragma once

#include "boolean.h"
#include "dictionary.h"

#define ACCUMULATE_BOOLEAN_LABEL	"accumulate_yn"

/* Usage */
/* ----- */
boolean accumulate_boolean_get(
		char *accumulate_boolean_string );

/* Usage */
/* ----- */

/* Returns component of dictionary or null */
/* --------------------------------------- */
boolean accumulate_boolean_dictionary_extract(
		DICTIONARY *dictionary );

