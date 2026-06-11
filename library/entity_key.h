/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/entity_key.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ENTITY_KEY_H
#define ENTITY_KEY_H

#include "boolean.h"
#include "list.h"

typedef struct
{
	boolean contact_key_boolean;
} ENTITY_KEY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ENTITY_KEY *entity_key_new(
		const char *entity_table,
		const char *entity_contact_key_column );

/* Process */
/* ------- */
ENTITY_KEY *entity_key_calloc(
		void );

/* Usage */
/* ----- */
boolean entity_key_contact_key_boolean(
		const char *table_name,
		const char *column_name );

#endif

