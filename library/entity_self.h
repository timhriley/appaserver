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

#define ENTITY_SELF_SELECT		"full_name,"			    \
					"credit_card_number,"		    \
					"credit_card_expiration_month_year,"\
					"credit_card_security_code,"	    \
					"credit_provider,"		    \
					"invoice_amount_due,"		    \
					"invoice_statement_current"

typedef struct
{
	boolean entity_contact_key_boolean;
	char *full_name;
	char *contact_key /* optional */;
	char *credit_card_number;
	char *credit_card_expiration_month_year;
	char *credit_card_security_code;
	char *credit_provider;
	double invoice_amount_due;
	char *invoice_statement_current;
	ENTITY *entity;
} ENTITY_SELF;

/* Usage */
/* ----- */

/* Returns heap memory of static pointer */
/* ------------------------------------- */
ENTITY_SELF *entity_self_fetch(
		boolean entity_contact_key_boolean,
		boolean fetch_entity_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *entity_self_select(
		const char *entity_self_select,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
ENTITY_SELF *entity_self_parse(
		boolean entity_contact_key_boolean,
		boolean fetch_entity_boolean,
		char *string_system_input );


/* Usage */
/* ----- */

/* Safely returns heap memory of static pointer */
/* -------------------------------------------- */
ENTITY_SELF *entity_self_new(
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key );

/* Process */
/* ------- */
ENTITY_SELF *entity_self_calloc(
		void );

#endif

