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

POST *post_new(	char *email_address )
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

	post->ip_address =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		environment_remote_ip_address(
			ENVIRONMENT_REMOTE_KEY );

	post->http_user_agent =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		environment_http_user_agent(
			ENVIRONMENT_USER_AGENT_KEY );

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
			POST_HTTP_USER_AGENT_SIZE,
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
		const int post_http_user_agent_size,
		char *email_address,
		char *ip_address,
		char *http_user_agent,
		char *timestamp )
{
	char insert_statement[ 1024 ];
	char http_user_agent_trimmed[ 256 ];

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

	string_strcpy(
		http_user_agent_trimmed,
		http_user_agent,
		post_http_user_agent_size + 1 /* buffer_size */ );

	snprintf(
		insert_statement,
		sizeof ( insert_statement ),
		"insert into %s (%s) values ('%s','%s','%s','%s');",
		post_table,
		post_insert,
		email_address,
		ip_address,
		http_user_agent_trimmed,
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
		char *email_address,
		char *post_return_email )
{
	static char mailx_system_string[ 128 ];

	if ( !email_address
	||   !post_return_email )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		mailx_system_string,
		sizeof ( mailx_system_string ),
		"mailx -s \"%s\" -r %s %s",
		subject,
		post_return_email,
		email_address );

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
	post_receive->timestamp_spaceless = argv[ 2 ];

	post_receive->timestamp_space =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_receive_timestamp_space(
			post_receive->timestamp_spaceless );

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
		const char *website_domain_name,
		char *apache_cgi_directory,
		char *email_address,
		char *timestamp_space )
{
	char receive_url[ 1024 ];

	if ( !apache_cgi_directory
	||   !email_address
	||   !timestamp_space )
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
		"https://%s%s/%s?%s+%s",
		website_domain_name,
		apache_cgi_directory,
		receive_executable,
		email_address,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_receive_timestamp_spaceless(
			timestamp_space ) );

	return strdup( receive_url );
}

char *post_ip_deny_system_string(
		const char *post_deny_executable,
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
		"%s %s",
		post_deny_executable,
		ip_address );

	return system_string;
}

char *post_receive_timestamp_space( char *timestamp_spaceless )
{
	static char timestamp_space[ 20 ];

	if ( !timestamp_spaceless
	||   strlen( timestamp_spaceless ) != 19 )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: timestamp_spaceless is empty or invalid.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	strcpy( timestamp_space, timestamp_spaceless );

	return
	string_search_replace_character(
		timestamp_space /* source_destination */,
		'_' /* search_character */,
		' ' /* replace_character */ );
}

char *post_receive_timestamp_spaceless( char *timestamp_space )
{
	static char timestamp_spaceless[ 20 ];

	if ( !timestamp_space
	||   strlen( timestamp_space ) != 19 )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: timestamp_space is empty or invalid.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	strcpy( timestamp_spaceless, timestamp_space );

	return
	string_search_replace_character(
		timestamp_spaceless /* source_destination */,
		' ' /* search_character */,
		'_' /* replace_character */ );
}

char *post_confirmation_update_statement(
		const char *post_table,
		const char *post_confirmation_column,
		char *email_address,
		char *timestamp_space )
{
	static char confirmation_update_statement[ 256 ];

	if ( !email_address
	||   !timestamp_space )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		confirmation_update_statement,
		sizeof ( confirmation_update_statement ),
		"update %s set %s = '%s' where %s;",
		post_table,
		post_confirmation_column,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now_yyyy_mm_dd( date_utc_offset() ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_primary_where(
			email_address,
			timestamp_space ) );

	return confirmation_update_statement;
}

