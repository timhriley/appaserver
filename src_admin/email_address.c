/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/email_address.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "email_address.h"

EMAIL_ADDRESS *email_address_new( char *email_address )
{
	EMAIL_ADDRESS *address;

	if ( !email_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	address = email_address_calloc();

	address->email_address = email_address;

	address->insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		email_address_insert_statement(
			EMAIL_ADDRESS_TABLE,
			EMAIL_ADDRESS_INSERT,
			email_address );

	return address;
}

EMAIL_ADDRESS *email_address_calloc( void )
{
	EMAIL_ADDRESS *email_address;

	if ( ! ( email_address = calloc( 1, sizeof ( EMAIL_ADDRESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return email_address;
}

char *email_address_insert_statement(
		const char *email_address_table,
		const char *email_address_insert,
		char *email_address )
{
	char insert_statement[ 1024 ];

	snprintf(
		insert_statement,
		sizeof ( insert_statement ),
		"insert into %s (%s) values ('%s');",
		email_address_table,
		email_address_insert,
		email_address );

	return strdup( insert_statement );
}

EMAIL_ADDRESS *email_address_fetch( char *email_address )
{
	char *fetch;

	if ( !email_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: email_address is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_fetch(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				EMAIL_ADDRESS_SELECT,
				EMAIL_ADDRESS_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				email_address_primary_where(
					email_address ) ) );

	if ( !fetch ) return NULL;

	return
	email_address_parse(
		email_address,
		fetch );
}

char *email_address_primary_where( char *email_address )
{
	static char primary_where[ 128 ];

	snprintf(
		primary_where,
		sizeof ( primary_where ),
		"email_address = '%s'",
		email_address );

	return primary_where;
}

EMAIL_ADDRESS *email_address_parse(
		char *email_address,
		char *string_fetch )
{
	EMAIL_ADDRESS *address;

	if ( !email_address
	||   !string_fetch )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	address = email_address_calloc();

	address->email_address = email_address;
	address->blocked_boolean = (*string_fetch == 'y');

	return address;
}

char *email_address_blocked_update_statement(
		const char *email_address_table,
		const char *email_address_blocked_column,
		char *email_address )
{
	static char blocked_update_statement[ 256 ];

	snprintf(
		blocked_update_statement,
		sizeof ( blocked_update_statement ),
		"update %s set %s = 'y' where %s;",
		email_address_table,
		email_address_blocked_column,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		email_address_primary_where(
			email_address ) );

	return blocked_update_statement;
}
