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
#include "appaserver_error.h"
#include "form.h"
#include "post.h"
#include "form_field_datum.h"
#include "post_contact_submit.h"

POST_CONTACT_SUBMIT_INPUT *post_contact_submit_new( void )
{
	POST_CONTACT_SUBMIT_INPUT *post_contact_submit_input;

	post_contact_submit_input->upload_filename_list =
		/* ----------- */
		/* List of one */
		/* ----------- */
		post_contact_submit_input_upload_filename_list();

	post_contact_submit_input->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_contact_submit_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			POST_CONTACT_SUBMIT_APPLICATION,
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
			post_contact_input->reason );

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
			post_contact_input->message );

	post_contact_submit_input->filespecification =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_contact_submit_input_filespecification(
			POST_DICTIONARY_SPOOL_PREFIX,
			post_contact_submit_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_submit_input->filespecification_boolean =
		post_contact_submit_input_filespecification_boolean(
			post_contact_submit_input->filespecification );

	return post_contact_submit_input;
}

POST_CONTACT_SUBMIT_INPUT *post_contact_submit_calloc( void )
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

LIST *post_contact_submit_input_upload_filename_list( void )
{
	LIST *filename_list = list_new();
	list_set( filename_list, "upload_file" );

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
	/* ---------------------------- */
	/* Returns heap memory or datum */
	/* ---------------------------- */
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

char *post_contact_submit_input_filespecification(
		const char *post_dictionary_spool_prefix,
		DICTIONARY *post_dictionary )
{
	char key[ 128 ];

	snprintf(
		key,
		sizeof ( key ),
		"%s%s",
		post_dictionary_spool_prefix,
		"upload_file" );

	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		key,
		post_dictionary );
}

boolean post_contact_submit_input_filespecification_boolean(
		char *filespecification )
{
	return
	upload_source_file_exists_boolean(
		filespecification
			/* directory_filename_session */ );
}

POST_CONTACT_SUBMIT *post_contact_submit_new( void )
{
	POST_CONTACT_SUBMIT *post_contact_submit;

	post_contact_submit = post_contact_submit_calloc();

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
			filespecification_boolean ) )
	{
		post_contact_submit->message_not_sent_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_contact_submit_display_system_string(
				POST_CONTACT_SUBMIT_NOT_SENT_FILENAME,
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
			FORM_CONTACT /* FORM_NAME */,
			post_contact_submit->
				post_contact_submit_input->
				email_address,
			post_contact_submit->
				post_contact_submit_input->
				environment_remote_ip_address );

	post_contact_submit->form_field_datum_list =
		post_contact_submit_form_field_datum_list(
			post_contact_submit->
				post_contact_submit_input->
				post->
				timestamp,
			post_contact_submit->
				post_contact_submit_input->
				email_address,
			post_contact_submit->
				post->
				form_name,
			post_contact_submit->
				post_contact_submit_input->
				reason,
			post_contact_submit->
				post_contact_submit_input->
				message,
			post_contact_submit->
				post_contact_submit_input->
				filespecification );

	post_contact_submit->form_field_datum_insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_field_datum_insert_statement(
			FORM_FIELD_DATUM_TABLE,
			post_contact_submit->form_field_datum_list );

	post_contact_submit->session =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		session_new(
			APPLICATION_ADMIN_NAME,
			POST_LOGIN_NAME,
			post_contact_submit->
				post_contact_submit_input->
				environment_http_user_agent,
			post_contact_submit->
				post_contact_submit_input->
				environment_remote_ip_address );

	post_contact_submit->post_return_email =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_return_email(
			POST_RETURN_USERNAME,
			post_contact_submit_input->
				appaserver_mailname );

	post_contact_submit->post_mailx_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_mailx_system_string(
			POST_CONTACT_SUBMIT_SUBJECT,
			post_contact_submit->post_return_email );

	post_contact_submit->receive_url =
		post_contact_submit_receive_url(
			POST_CONTACT_RECEIVE_EXECUTABLE,
			post_contact_submit->
				post_contact_submit_input->
				apache_cgi_directory,
			post_contact_submit->
				post_contact_submit_input->
				email_address,
			post_contact_submit->
				post->
				timestamp,
			post_contact_submit->
				session->
				session_key );

char *post_contact_submit_message(
	POST_CONTACT_SUBMIT_MESSAGE_PROMPT,
	post_contact_submit_receive_url() );

char *post_contact_submit_display_system_string(
	POST_CONTACT_DISPLAY_SENT_FILENAME
		/* message_filename */,
	post_contact_input->
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

char *post_contact_submit_receive_url(
		const char *post_contact_receive_executable,
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
		post_contact_receive_executable,
		email_address,
		timestamp,
		session_key );

	return strdup( receive_url );
}

char *post_contact_submit_display_system_string(
		const char *message_filename,
		char *document_root )
{
	char display_system_string[ 1024 ];

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
		system_string,
		sizeof ( system_string ),
		"php %s/../appahost_com/%s",
		document_root,
		message_filename );

	return strdup( display_system_string );
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
