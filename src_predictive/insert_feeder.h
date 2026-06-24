/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/insert_feeder.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INSERT_FEEDER_H
#define INSERT_FEEDER_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	boolean account_empty_boolean;
	boolean name_empty_boolean;
	boolean phrase_empty_boolean;
	boolean entity_contact_key_boolean;
	char *entity_contact_key;
	char *error_message;
} INSERT_FEEDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INSERT_FEEDER *insert_feeder_new(
		char *feeder_account,
		char *new_full_name,
		char *feeder_phrase );

INSERT_FEEDER *insert_feeder_calloc(
		void );

boolean insert_feeder_account_empty_boolean(
		char *feeder_account );

boolean insert_feeder_name_empty_boolean(
		char *new_full_name );

boolean insert_feeder_phrase_empty_boolean(
		char *feeder_phrase );

/* Returns program memory or null */
/* ------------------------------ */
char *insert_feeder_error_message(
		boolean account_empty_boolean,
		boolean name_empty_boolean,
		boolean phrase_empty_boolean );

#endif
