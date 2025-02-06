/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "post.h"
#include "post_signup.h"

POST_SIGNUP *post_signup_new(
		char *email_address,
		char *timestamp,
		char *application_key,
		char *application_title )
{
	POST_SIGNUP *post_signup;

	if ( !email_address
	||   !timestamp
	||   !application_key
	||   !application_title )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_signup = post_signup_calloc();

	post_signup->insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_signup_insert_statement(
			POST_SIGNUP_TABLE,
			POST_SIGNUP_INSERT,
			email_address,
			timestamp,
			application_key,
			application_title );

	return post_signup;
}

POST_SIGNUP *post_signup_calloc( void )
{
	POST_SIGNUP *post_signup;

	if ( ! ( post_signup = calloc( 1, sizeof ( POST_SIGNUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_signup;
}

char *post_signup_insert_statement(
		const char *post_signup_table,
		const char *post_signup_insert,
		char *email_address,
		char *timestamp,
		char *application_key,
		char *application_title )
{
	char insert_statement[ 1024 ];

	snprintf(
		insert_statement,
		sizeof ( insert_statement ),
		"insert into %s (%s) values ('%s','%s','%s','%s');",
		post_signup_table,
		post_signup_insert,
		email_address,
		timestamp,
		application_key,
		application_title );

	return strdup( insert_statement );
}

POST_SIGNUP *post_signup_fetch(
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
				POST_SIGNUP_TABLE,
				POST_SIGNUP_SELECT,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				post_primary_where(
					email_address,
					timestamp ) ) );

	if ( !fetch ) return NULL;

	return
	post_signup_parse(
		email_address,
		timestamp,
		fetch );
}

POST_SIGNUP *post_signup_parse(
		char *email_address,
		char *timestamp,
		char *string_fetch )
{
	POST_SIGNUP *post_signup;
	char buffer[ 128 ];

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

	post_signup = post_signup_calloc();

	post_signup->email_address = email_address;
	post_signup->timestamp = timestamp;

	/* See POST_SIGNUP_SELECT */
	/* ---------------------- */
	piece( buffer, SQL_DELIMITER, string_fetch, 0 );
	if ( *buffer ) post_signup->application_key = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 1 );
	if ( *buffer ) post_signup->application_title = strdup( buffer );

	return post_signup;
}
