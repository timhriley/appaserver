/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_contact.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "security.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "post.h"
#include "post_contact.h"

POST_CONTACT *post_contact_new(
		char *email_address,
		char *timestamp,
		char *reason,
		char *message,
		char *upload_filespecification )
{
	POST_CONTACT *post_contact;

	if ( !email_address
	||   !timestamp
	||   !reason )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_contact = post_contact_calloc();

	post_contact->insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_contact_insert_statement(
			POST_CONTACT_TABLE,
			POST_CONTACT_INSERT,
			email_address,
			timestamp,
			reason,
			message,
			upload_filespecification );

	return post_contact;
}

POST_CONTACT *post_contact_calloc( void )
{
	POST_CONTACT *post_contact;

	if ( ! ( post_contact = calloc( 1, sizeof ( POST_CONTACT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_contact;
}

char *post_contact_insert_statement(
		const char *post_contact_table,
		const char *post_contact_insert,
		char *email_address,
		char *timestamp,
		char *reason,
		char *message,
		char *upload_filespecification )
{
	char insert_statement[ STRING_65K ];
	char *ptr = insert_statement;

	ptr += sprintf(
		ptr,
		"insert into %s (%s) values ('%s','%s','%s'",
		post_contact_table,
		post_contact_insert,
		email_address,
		timestamp,
		reason );

	if ( message )
	{
		char *message_escape;

		message_escape =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				message /* datum */ );

		ptr += sprintf( ptr, ",'%s'", message_escape );

		free( message_escape );
	}
	else
	{
		ptr += sprintf( ptr, ",null" );
	}

	if ( upload_filespecification )
	{
		ptr += sprintf( ptr, ",'%s'", upload_filespecification );
	}
	else
	{
		ptr += sprintf( ptr, ",null" );
	}

	ptr += sprintf( ptr, ");" );

	return strdup( insert_statement );
}

POST_CONTACT *post_contact_fetch(
		char *email_address,
		char *timestamp )
{
	char *fetch;

	if ( !email_address
	||   !timestamp )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
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
				POST_CONTACT_TABLE,
				POST_CONTACT_SELECT,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				post_primary_where(
					email_address,
					timestamp ) ) );

	if ( !fetch ) return NULL;

	return
	post_contact_parse(
		email_address,
		timestamp,
		fetch );
}

POST_CONTACT *post_contact_parse(
		char *email_address,
		char *timestamp,
		char *string_fetch )
{
	POST_CONTACT *post_contact;
	char buffer[ STRING_64K ];

	if ( !email_address
	||   !timestamp
	||   !string_fetch )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_contact = post_contact_calloc();

	post_contact->email_address = email_address;
	post_contact->timestamp = timestamp;

	/* See POST_CONTACT_SELECT */
	/* ----------------------- */
	piece( buffer, SQL_DELIMITER, string_fetch, 0 );
	if ( *buffer ) post_contact->reason = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 1 );
	if ( *buffer ) post_contact->message = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 2 );
	if ( *buffer )
		post_contact->upload_filespecification =
			strdup( buffer );

	return post_contact;
}
