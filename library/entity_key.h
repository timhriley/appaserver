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
} ENTITY_KEY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ENTITY_KEY *entity_key_new(
		void );

/* Process */
/* ------- */
ENTITY_KEY *entity_key_calloc(
		void );

#endif

