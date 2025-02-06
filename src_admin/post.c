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
#include "environ.h"
#include "session.h"
#include "application.h"
#include "appaserver_error.h"
#include "post.h"

POST *post_new(
		const char *application_admin_name,
		char *email_address )
{
	POST *post;

	if ( !email_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: email_address is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post = post_calloc();

	session_environment_set( (char *)application_admin_name );

	/* Returns heap memory or null */
	/* --------------------------- */
	post->ip_address = environment_remote_ip_address();

	/* Returns heap memory or null */
	/* --------------------------- */
	post->http_user_agent = environment_http_user_agent();

	/* Returns heap memory */
	/* ------------------- */
	post->timestamp = post_timestamp();

	post->email_address =
		email_address_fetch(
			email_address );

	if ( post->email_address )
	{
		if ( post->email_address->blocked_boolean )
		{
			post->ip_deny_system_string =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				post_ip_deny_system_string(
					POST_DENY_EXECUTABLE,
					POST_DENY_FILESPECIFICATION,
					post->ip_address );

			return post;
		}
	}
	else
	{
		post->email_address =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			email_address_new(
				email_address );
	}

	post->insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_insert_statement(
			POST_TABLE,
			POST_INSERT,
			email_address,
			post->ip_address,
			post->http_user_agent,
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
		char *email_address,
		char *ip_address,
		char *http_user_agent,
		char *timestamp )
{
	char insert_statement[ 1024 ];

	if ( !email_address
	||   !ip_address
	||   !http_user_agent
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
		insert_statement,
		sizeof ( insert_statement ),
		"insert into %s (%s) values ('%s','%s','%s','%s');",
		post_table,
		post_insert,
		email_address,
		ip_address,
		http_user_agent,
		timestamp );

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
				POST_SELECT,
				POST_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				post_primary_where(
					email_address,
					timestamp ) ) );

	if ( !fetch ) return NULL;

	return
	post_parse(
		email_address,
		timestamp,
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
		char *string_fetch )
{
	POST *post;
	char buffer[ 1024 ];

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

	if ( !string_fetch ) return NULL;

	post = post_calloc();

	/* See POST_SELECT */
	/* ---------------- */
	piece( buffer, SQL_DELIMITER, string_fetch, 0 );
	if ( *buffer ) post->ip_address = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 1 );
	if ( *buffer ) post->http_user_agent = strdup( buffer );

	piece( buffer, SQL_DELIMITER, string_fetch, 2 );
	if ( *buffer ) post->confirmation_received_date = strdup( buffer );

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

POST_RECEIVE *post_receive_new(
		const char *application_admin_name,
		const char *appaserver_mailname_filespecification,
		int argc,
		char **argv )
{
	POST_RECEIVE *post_receive;

	if ( argc != 3
	||   !argv )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: parameter is empty or invalid.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_receive = post_receive_calloc();

	post_receive->email_address = argv[ 1 ];
	post_receive->timestamp = argv[ 2 ];

	session_environment_set( (char *)application_admin_name );

	post_receive->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_receive->appaserver_mailname =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_mailname(
			appaserver_mailname_filespecification );

	post_receive->appaserver_error_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_error_filename(
			(char *)application_admin_name );

	return post_receive;
}

POST_RECEIVE *post_receive_calloc( void )
{
	POST_RECEIVE *post_receive;

	if ( ! ( post_receive =
			calloc( 1,
				sizeof ( POST_RECEIVE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_receive;
}

char *post_receive_url(
		const char *receive_executable,
		char *apache_cgi_directory,
		char *email_address,
		char *timestamp )
{
	char receive_url[ 1024 ];

	if ( !apache_cgi_directory
	||   !email_address
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
		receive_url,
		sizeof ( receive_url ),
		"%s/%s?%s+%s",
		apache_cgi_directory,
		receive_executable,
		email_address,
		timestamp );

	return strdup( receive_url );
}

char *post_ip_deny_system_string(
		const char *post_deny_executable,
		const char *post_deny_filespecification,
		char *ip_address )
{
	static char system_string[ 128 ];

	if ( !ip_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: ip_address is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s",
		post_deny_executable,
		post_deny_filespecification,
		ip_address );

	return system_string;
}
