/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/accumulate_boolean.c 			*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "accumulate_boolean.h"

boolean accumulate_boolean_get( char *accumulate_boolean_string )
{
	return
	( accumulate_boolean_string &&
	  *accumulate_boolean_string == 'y' );
}

boolean accumulate_boolean_dictionary_extract( DICTIONARY *dictionary )
{
	return
	accumulate_boolean_get(
		dictionary_get(
			ACCUMULATE_BOOLEAN_LABEL,
			dictionary ) );
}

