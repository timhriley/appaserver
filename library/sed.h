/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/sed.h		 			*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------	*/

#ifndef SED_H
#define SED_H

#include <regex.h>
#include "boolean.h"

#define SED_BUFFER_SIZE 65536

typedef struct
{
	char *replace;
	regex_t regex;
	regoff_t begin;
	regoff_t end;
} SED;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SED *sed_new(	char *regular_expression,
		char *replace );

/* Process */
/* ------- */
SED *sed_calloc(void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *sed_trim_double_spaces(
		char *string );

/* Public */
/* ------ */
void sed_free(	SED *sed );

SED *new_sed(	char *regular_expression,
		char *replace );

boolean sed_search_replace(
		char *buffer,
		SED *sed );

boolean sed_will_replace(
		char *string,
		SED *sed );

void sed_get_begin_end(
		regoff_t *begin,
		regoff_t *end,
		regex_t *regex,
		char *buffer );

boolean sed_match_boolean(
		char *string,
		char *regular_expression );

#endif
