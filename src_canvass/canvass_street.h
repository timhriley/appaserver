/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_canvass/canvass_street.h	   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef CANVASS_STREET_H
#define CANVASS_STREET_H

#include "boolean.h"
#include "list.h"
#include "street.h"

#define CANVASS_STREET_SELECT		"street_name,"		\
					"city,"			\
					"state_code"

#define CANVASS_STREET_TABLE		"canvass_street"

typedef struct
{
	STREET *street;
} CANVASS_STREET;

/* Usage */
/* ----- */
LIST *canvass_street_fetch_list(
		char *canvass_name,
		boolean include_boolean,
		LIST *street_list );

/* Usage */
/* ----- */
CANVASS_STREET *canvass_street_parse(
		LIST *street_list,
		char *input );

/* Process */
/* ------- */
CANVASS_STREET *canvass_street_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *canvass_street_where(
		char *canvass_name,
		boolean include_boolean );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *canvass_street_include_where(
		boolean include_boolean );

/* Usage */
/* ----- */
void canvass_street_output(
		CANVASS_STREET *canvass_street );

#endif
