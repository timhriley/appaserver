/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/entity_self.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "entity.h"
#include "entity_self.h"

ENTITY_SELF *entity_self_calloc( void )
{
	ENTITY_SELF *entity_self;

	if ( ! ( entity_self = calloc( 1, sizeof ( ENTITY_SELF ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return entity_self;
}

ENTITY_SELF *entity_self_new(
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key )
{
	ENTITY_SELF *entity_self;

	if ( !full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: full_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( contact_key_boolean
	&& ( !contact_key || !*contact_key ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: contact_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	entity_self = entity_self_calloc();

	entity_self->entity_contact_key_boolean = contact_key_boolean;
	entity_self->full_name = full_name;

	entity_self->contact_key = contact_key;

	return entity_self;
}

ENTITY_SELF *entity_self_fetch(
		boolean contact_key_boolean,
		boolean fetch_entity_boolean )
{
	char *select;
	char *system_string;
	char *input;
	static ENTITY_SELF *entity_self;

	if ( entity_self ) return entity_self;

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_self_select(
			ENTITY_SELF_SELECT,
			ENTITY_CONTACT_KEY_COLUMN,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			ENTITY_SELF_TABLE,
			(char *)0 /* where */ );

	free( select );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input(
			system_string );

	if ( ! ( entity_self =
			entity_self_parse(
				contact_key_boolean,
				fetch_entity_boolean,
				input ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity_self_parse(%s) returned empty.",
			input );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( input );

	return entity_self;
}

ENTITY_SELF *entity_self_parse(
		boolean contact_key_boolean,
		boolean fetch_entity_boolean,
		char *input )
{
	char full_name[ 128 ];
	char buffer[ 128 ];
	char *contact_key = {0};
	ENTITY_SELF *entity_self;

	if ( !input || !*input ) return NULL;

	/* See entity_self_select() */
	/* ------------------------ */
	piece( full_name, SQL_DELIMITER, input, 0 );

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 7 );
		contact_key = strdup( buffer );
	}

	entity_self =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		entity_self_new(
			contact_key_boolean,
			strdup( full_name ),
			contact_key );

	piece( buffer, SQL_DELIMITER, input, 1 );
	entity_self->credit_card_number = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	entity_self->credit_card_expiration_month_year = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	entity_self->credit_card_security_code = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	entity_self->credit_provider = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	entity_self->invoice_amount_due = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	entity_self->invoice_statement_current = strdup( buffer );

	if ( fetch_entity_boolean )
	{
		entity_self->entity =
			entity_fetch(
				entity_self->full_name,
				entity_self->contact_key,
				contact_key_boolean );
	}

	return entity_self;
}

char *entity_self_select(
		const char *entity_self_select,
		const char *entity_contact_key_column,
		boolean contact_key_boolean )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	entity_select_string(
		entity_self_select,
		entity_contact_key_column,
		contact_key_boolean );
}
