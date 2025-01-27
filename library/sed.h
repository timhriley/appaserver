/* sed.h		 						*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef SED_H
#define SED_H

#include <regex.h>

#define SED_BUFFER_SIZE 65536

/* Objects */
/* ------- */
typedef struct
{
	char *replace;
	regex_t regex;
	regoff_t begin;
	regoff_t end;
} SED;

/* Operations */
/* ---------- */
void sed_free(				SED *sed );

SED *new_sed(				char *regular_expression,
					char *replace );

SED *sed_new(				char *regular_expression,
					char *replace );

int sed_search_replace(			char *buffer,
					SED *sed );

int sed_will_replace(			char *buffer,
					SED *sed );

void sed_get_begin_end(			regoff_t *begin,
					regoff_t *end,
					regex_t *regex,
					char *buffer );

/* Returns static memory */
/* --------------------- */
char *sed_trim_double_spaces(		char *string );

#endif
