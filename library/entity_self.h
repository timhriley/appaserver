/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/entity_self.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ENTITY_SELF_H
#define ENTITY_SELF_H

#include "boolean.h"
#include "list.h"
#include "entity.h"

#define ENTITY_SELF_TABLE		"self"

#define ENTITY_SELF_SELECT		"full_name,"		\
					"street_address"

typedef struct
{
	char *full_name;
	char *street_address;
	ENTITY *entity;
} ENTITY_SELF;

/* Usage */
/* ----- */
ENTITY_SELF *entity_self_fetch(
		boolean fetch_entity_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *entity_self_system_string(
		char *entity_self_select,
		char *entity_self_table );

/* Usage */
/* ----- */
ENTITY_SELF *entity_self_parse(
		char *string_input,
		boolean fetch_entity_boolean );


/* Usage */
/* ----- */
ENTITY_SELF *entity_self_new(
		char *full_name,
		char *street_address );

/* Process */
/* ------- */
ENTITY_SELF *entity_self_calloc(
		void );

#endif

