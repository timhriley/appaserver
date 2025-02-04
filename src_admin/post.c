/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "date.h"
#include "appaserver.h"
#include "application.h"
#include "appaserver_error.h"
#include "post.h"

POST *post_new(
		const char *form_name,
		char *email_address,
		char *ip_address )
{
	POST *post;

	if ( !email_address
	||   !ip_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post = post_calloc();

	post->form_name = (char *)form_name;
	post->ip_address = ip_address;
	post->email_address = email_address;

	/* Returns heap memory */
	/* ------------------- */
	post->timestamp = post_timestamp();

	post->insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_insert_statement(
			POST_TABLE,
			POST_INSERT,
			form_name,
			email_address,
			ip_address,
			post->timestamp );

	return post;
}

POST *post_calloc( void )
{
	POST *post;

	if ( ! ( post = calloc( 1, sizeof ( POST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post;
}

char *post_insert_statement(
		const char *post_table,
		const char *post_insert,
		const char *form_name,
		char *email_address,
		char *ip_address,
		char *post_timestamp )
{
	char insert_statement[ 1024 ];

	if ( !email_address
	||   !ip_address
	||   !post_timestamp )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		insert_statement,
		sizeof ( insert_statement ),
		"insert into %s (%s) values ('%s','%s','%s','%s');",
		post_table,
		post_insert,
		post_timestamp,
		email_address,
		ip_address,
		form_name );

	return strdup( insert_statement );
}

void post_mailx(
		char *message,
		char *mailx_system_string )
{
	FILE *output_pipe;

	if ( !message
	||   !mailx_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			mailx_system_string );

	fprintf(
		output_pipe,
		"%s\n",
		message );

	pclose( output_pipe );
}

char *post_return_email(
		const char *post_return_username,
		char *appaserver_mailname )
{
	static char return_email[ 128 ];

	if ( !appaserver_mailname )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: appaserver_mailname is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		return_email,
		sizeof ( return_email ),
		"%s@%s",
		post_return_username,
		appaserver_mailname );

	return return_email;
}

char *post_mailx_system_string(
		const char *subject,
		char *post_return_email )
{
	static char mailx_system_string[ 256 ];

	if ( !post_return_email )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: post_return_email is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		mailx_system_string,
		sizeof ( mailx_system_string ),
		"mailx -s \"%s\" -r %s",
		subject,
		post_return_email );

	return mailx_system_string;
}

POST *post_fetch(
		char *email_address,
		char *timestamp )
{
	char *primary_where;
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

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_primary_where(
			email_address,
			timestamp );

	fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_fetch(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				POST_SELECT,
				POST_TABLE,
				primary_where ) );

	if ( !fetch ) return NULL;

	return
	post_parse(
		email_address,
		timestamp,
		primary_where,
		fetch );
}

char *post_primary_where(
		char *email_address,
		char *timestamp )
{
	static char primary_where[ 128 ];

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

	snprintf(
		primary_where,
		sizeof ( primary_where ),
		"timestamp = '%s' and email_address = '%s'",
		timestamp,
		email_address );

	return primary_where;
}

POST *post_parse(
		char *email_address,
		char *timestamp,
		char *post_primary_where,
		char *string_fetch )
{
	POST *post;
	char buffer[ 1024 ];

	if ( !email_address
	||   !timestamp
	||   !post_primary_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !string_fetch ) return NULL;

	post = post_calloc();

	/* See POST_SELECT */
	/* ---------------- */
	piece( buffer, SQL_DELIMITER, string_fetch, 0 );
	if ( *buffer )
		post->ip_address =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 1 );
	if ( *buffer )
		post->form_name =
			strdup( buffer );

	return post;
}

char *post_timestamp( void )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now19( date_utc_offset() );
}

POST_RECEIVE_INPUT *post_receive_input_new(
		int argc,
		char **argv )
{
	POST_RECEIVE_INPUT *post_receive_input;

	if ( argc != 4
	||   !argv )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_receive_input = post_receive_input_calloc();

	post_receive_input->email_address = argv[ 1 ];
	post_receive_input->timestamp = argv[ 2 ];
	post_receive_input->session_key = argv[ 3 ];

	session_environment_set( APPLICATION_ADMIN_NAME );

	post_receive_input->session =
		session_fetch(
			APPLICATION_ADMIN_NAME,
			post_receive_input->session_key,
			(char *)0 /* login_name */ );

	post_receive_input->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	if ( ! ( post_receive_input->post =
			post_fetch(
				post_receive_input->email_address,
				post_receive_input->timestamp ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: post_fetch(%s,%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			post_receive_input->email_address,
			post_receive_input->timestamp );
		exit( 1 );
	}

	post_receive_input->appaserver_mailname =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_mailname(
			APPASERVER_MAILNAME_FILESPECIFICATION );

	return post_receive_input;
}

POST_RECEIVE_INPUT *post_receive_input_calloc( void )
{
	POST_RECEIVE_INPUT *post_receive_input;

	if ( ! ( post_receive_input =
			calloc( 1,
				sizeof ( POST_RECEIVE_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_receive_input;
}

char *post_receive_url(
		const char *receive_executable,
		char *apache_cgi_directory,
		char *email_address,
		char *timestamp,
		char *session_key )
{
	char receive_url[ 1024 ];

	if ( !apache_cgi_directory
	||   !email_address
	||   !timestamp
	||   !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		receive_url,
		sizeof ( receive_url ),
		"%s/%s?%s+%s+%s",
		apache_cgi_directory,
		receive_executable,
		email_address,
		timestamp,
		session_key );

	return strdup( receive_url );
}

