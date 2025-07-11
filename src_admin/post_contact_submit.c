/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_contact_submit.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "application.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "session.h"
#include "security.h"
#include "environ.h"
#include "post.h"
#include "file.h"
#include "post_contact_receive.h"
#include "post_contact_submit.h"

POST_CONTACT_SUBMIT *post_contact_submit_new(
		int argc,
		char **argv )
{
	POST_CONTACT_SUBMIT *post_contact_submit;

	post_contact_submit = post_contact_submit_calloc();

	session_environment_set( APPLICATION_ADMIN_NAME );

	appaserver_error_argv_file(
		argc,
		argv,
		APPLICATION_ADMIN_NAME,
		(char *)0 /* login_name */ );

	post_contact_submit->post_contact_submit_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_contact_submit_input_new();

	if (	post_contact_submit->
			post_contact_submit_input->
			email_invalid_boolean
	|| 	post_contact_submit->
			post_contact_submit_input->
			reason_invalid_boolean
	|| (	post_contact_submit->
			post_contact_submit_input->
			message_empty_boolean
	&&	!post_contact_submit->
			post_contact_submit_input->
			file_boolean ) )
	{
		post_contact_submit->display_system_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_contact_submit_display_system_string(
				POST_CONTACT_SUBMIT_NOT_SENT_FILENAME
					/* message_filename */,
				post_contact_submit->
					post_contact_submit_input->
					appaserver_parameter->
					document_root );

		return post_contact_submit;
	}

	post_contact_submit->post =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_new(
			post_contact_submit->
				post_contact_submit_input->
				email_address );

	if ( post_contact_submit->post->email_address->blocked_boolean )
	{
		return post_contact_submit;
	}

	post_contact_submit->post_contact =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_contact_new(
			post_contact_submit->
				post_contact_submit_input->
				email_address,
			post_contact_submit->
				post->
				timestamp,
			post_contact_submit->
				post_contact_submit_input->
				reason,
			post_contact_submit->
				post_contact_submit_input->
				message,
			post_contact_submit->
				post_contact_submit_input->
				file );

	post_contact_submit->post_return_email =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_return_email(
			POST_RETURN_USERNAME,
			post_contact_submit->
				post_contact_submit_input->
				appaserver_mailname );

	post_contact_submit->post_mailx_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_mailx_system_string(
			POST_CONTACT_SUBMIT_SUBJECT,
			post_contact_submit->post->email_address->email_address,
			post_contact_submit->post_return_email );

	post_contact_submit->post_receive_url =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_receive_url(
			POST_CONTACT_RECEIVE_EXECUTABLE,
			"appahost.com" /* website_domain_name */,
			post_contact_submit->
				post_contact_submit_input->
				appaserver_parameter->
				apache_cgi_directory,
			post_contact_submit->
				post_contact_submit_input->
				email_address,
			post_contact_submit->
				post->
				timestamp
			       /* timestamp_space */,
			post_contact_submit->
				post->
				random_number );

	appaserver_error_message_file(
		APPLICATION_ADMIN_NAME,
		(char *)0 /* login_name */,
		post_contact_submit->post_receive_url
			/* message */ );

	post_contact_submit->message =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_contact_submit_message(
			POST_CONTACT_SUBMIT_MESSAGE_PROMPT,
			post_contact_submit->post_receive_url );

	post_contact_submit->display_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_contact_submit_display_system_string(
			POST_CONTACT_SUBMIT_SENT_FILENAME
				/* message_filename */,
			post_contact_submit->
				post_contact_submit_input->
				appaserver_parameter->
				document_root );

	return post_contact_submit;
}

POST_CONTACT_SUBMIT *post_contact_submit_calloc( void )
{
	POST_CONTACT_SUBMIT *post_contact_submit;

	if ( ! ( post_contact_submit =
			calloc( 1,
				sizeof ( POST_CONTACT_SUBMIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_contact_submit;
}

char *post_contact_submit_display_system_string(
		const char *message_filename,
		char *document_root )
{
	static char display_system_string[ 128 ];

	if ( !document_root )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: document_root is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		display_system_string,
		sizeof ( display_system_string ),
		"php %s/../appahost_com/%s",
		document_root,
		message_filename );

	return display_system_string;
}

char *post_contact_submit_message(
		const char *message_prompt,
		char *receive_url )
{
	static char message[ 128 ];

	if ( !receive_url )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: receive_url is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		message,
		sizeof ( message ),
		"%s: %s",
		message_prompt,
		receive_url );

	return message;
}

POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input_new( void )
{
	POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input;

	post_contact_submit_input = post_contact_submit_input_calloc();

	post_contact_submit_input->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_contact_submit_input->upload_filename_list =
		/* ----------- */
		/* List of one */
		/* ----------- */
		post_contact_submit_input_upload_filename_list(
			"upload_file" /* attribute_name */ );

	post_contact_submit_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			APPLICATION_ADMIN_NAME,
			post_contact_submit_input->
				appaserver_parameter->
				upload_directory,
			post_contact_submit_input->upload_filename_list );

	post_contact_submit_input->email_address =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_contact_submit_input_email_address(
			post_contact_submit_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_submit_input->email_invalid_boolean =
		post_contact_submit_input_email_invalid_boolean(
			post_contact_submit_input->email_address );

	if ( post_contact_submit_input->email_invalid_boolean )
		return post_contact_submit_input;

	post_contact_submit_input->reason =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_contact_submit_input_reason(
			post_contact_submit_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_submit_input->reason_invalid_boolean =
		post_contact_submit_input_reason_invalid_boolean(
			post_contact_submit_input->reason );

	if ( post_contact_submit_input->reason_invalid_boolean )
		return post_contact_submit_input;

	post_contact_submit_input->message =
		/* ----------------------------------------------------- */
		/* Returns component of dictionary, heap memory, or null */
		/* ----------------------------------------------------- */
		post_contact_submit_input_message(
			post_contact_submit_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_submit_input->message_empty_boolean =
		post_contact_submit_input_message_empty_boolean(
			post_contact_submit_input->message );

	post_contact_submit_input->file =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_contact_submit_input_file(
			post_contact_submit_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_submit_input->file_boolean =
		post_contact_submit_input_file_boolean(
			post_contact_submit_input->file );

	post_contact_submit_input->appaserver_mailname =
		appaserver_mailname(
			APPASERVER_MAILNAME_FILESPECIFICATION );

	post_contact_submit_input->appaserver_error_filename =
		appaserver_error_filename(
			APPLICATION_ADMIN_NAME );

	return post_contact_submit_input;
}

POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input_calloc( void )
{
	POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input;

	if ( ! ( post_contact_submit_input =
			calloc( 1,
				sizeof ( POST_CONTACT_SUBMIT_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_contact_submit_input;
}

LIST *post_contact_submit_input_upload_filename_list(
		const char *attribute_name )
{
	LIST *filename_list = list_new();
	list_set( filename_list, (char *)attribute_name );

	return filename_list;
}

char *post_contact_submit_input_email_address( DICTIONARY *post_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		"email_address",
		post_dictionary );
}

boolean post_contact_submit_input_email_invalid_boolean(
		char *post_contact_submit_input_email_address )
{
	return
	!string_email_address_boolean(
		post_contact_submit_input_email_address );
}

char *post_contact_submit_input_reason( DICTIONARY *post_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		"reason",
		post_dictionary );
}

boolean post_contact_submit_input_reason_invalid_boolean( char *reason )
{
	return
	!string_mnemonic_boolean( reason );
}

char *post_contact_submit_input_message( DICTIONARY *post_dictionary )
{
	char *message;

	message =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			"message",
			post_dictionary );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	security_sql_injection_escape(
		SECURITY_ESCAPE_CHARACTER_STRING,
		message /* datum */ );
}

boolean post_contact_submit_input_message_empty_boolean( char *message )
{
	if ( !message )
		return 1;
	else
		return 0;
}

char *post_contact_submit_input_file(
		DICTIONARY *post_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		"upload_file",
		post_dictionary );
}

boolean post_contact_submit_input_file_boolean( char *file )
{
	if ( file )
		return 1;
	else
		return 0;
}

