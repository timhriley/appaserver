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
#include "form.h"
#include "security.h"
#include "environ.h"
#include "post.h"
#include "upload_source.h"
#include "post_contact_receive.h"
#include "post_contact_submit.h"

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
		post_contact_submit->display_system_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
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

	post_contact_submit->form_field_insert_list =
		post_contact_submit_form_field_insert_list(
			post_contact_submit->
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

	post_contact_submit->form_field_datum_insert_statement_list =
		form_field_datum_insert_statement_list(
			FORM_FIELD_DATUM_TABLE,
			post_contact_submit->
				form_field_insert_list->
				list
				/* form_field_insert_list */ );

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
			post_contact_submit->
				post_contact_submit_input->
				appaserver_mailname );

	post_contact_submit->post_mailx_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_mailx_system_string(
			POST_CONTACT_SUBMIT_SUBJECT,
			post_contact_submit->post_return_email );

	post_contact_submit->post_receive_url =
		post_receive_url(
			POST_CONTACT_RECEIVE_EXECUTABLE,
			post_contact_submit->
				post_contact_submit_input->
				appaserver_parameter->
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

LIST *post_contact_submit_form_field_datum_list(
		char *timestamp,
		char *email_address,
		char *form_name,
		char *reason,
		char *message,
		char *filespecification )
{
	LIST *form_field_datum_list = list_new();
	FORM_FIELD_DATUM *form_field_datum;

	if ( !timestamp
	||   !email_address
	||   !form_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"timestamp" /* field_name */,
			timestamp /* field_datum */,
			(char *)0 /* message_datum */,
			1 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"email_address" /* field_name */,
			email_address /* field_datum */,
			(char *)0 /* message_datum */,
			2 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"form_name" /* field_name */,
			form_name /* field_datum */,
			(char *)0 /* message_datum */,
			3 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"reason" /* field_name */,
			reason /* field_datum */,
			(char *)0 /* message_datum */,
			4 /* primary_key_index */ );

	list_set( form_field_datum_list, form_field_datum );

	if ( message )
	{
		form_field_datum =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			form_field_datum_new(
				"message" /* field_name */,
				(char *)0 /* field_datum */,
				message /* message_datum */,
				0 /* primary_key_index */ );

		list_set( form_field_datum_list, form_field_datum );
	}

	if ( filespecification )
	{
		form_field_datum =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			form_field_datum_new(
				"upload_file" /* field_name */,
				filespecification /* field_datum */,
				(char *)0 /* message_datum */,
				0 /* primary_key_index */ );

		list_set( form_field_datum_list, form_field_datum );
	}

	return form_field_datum_list;
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
		post_contact_submit_input_upload_filename_list();

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

	post_contact_submit_input->environment_remote_ip_address =
		environment_remote_ip_address();

	post_contact_submit_input->environment_http_user_agent =
		environment_http_user_agent();

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

FORM_FIELD_INSERT_LIST *post_contact_submit_form_field_insert_list(
		char *email_address,
		char *reason,
		char *message,
		char *filespecification,
		char *form_name,
		char *timestamp )
{
	FORM_FIELD_INSERT_LIST *form_field_insert_list;
	LIST *form_field_datum_list;
	FORM_FIELD_DATUM *form_field_datum;
	FORM_FIELD_INSERT *form_field_insert;

	if ( !email_address
	||   !reason
	||   !form_name
	||   !timestamp )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Safely returns */
	/* -------------- */
	form_field_insert_list = form_field_insert_list_new();

	/* Set reason */
	/* ---------- */
	form_field_datum_list = list_new();

	form_field_datum =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_datum_new(
			"reason" /* field_name */,
			reason /* field_datum */,
			(char *)0 /* message_datum */,
			0 /* primary_key_index */ );

	list_set(
		form_field_datum_list,
		form_field_datum );

	form_field_insert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_field_insert_new(
			email_address,
			form_name,
			timestamp,
			form_field_datum_list /* in/out */ );

	list_set(
		form_field_insert_list->list,
		form_field_insert );

	/* Set message */
	/* ----------- */
	if ( message )
	{
		form_field_datum_list = list_new();

		form_field_datum =
			form_field_datum_new(
				"message" /* field_name */,
				(char *)0 /* field_datum */,
				message /* message_datum */,
				0 /* primary_key_index */ );

			list_set(
				form_field_datum_list,
				form_field_datum );

		form_field_insert =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			form_field_insert_new(
				email_address,
				form_name,
				timestamp,
				form_field_datum_list /* in/out */ );

		list_set(
			form_field_insert_list->list,
			form_field_insert );
	}

	/* Set filespecification */
	/* --------------------- */
	if ( filespecification )
	{
		form_field_datum_list = list_new();

		form_field_datum =
			form_field_datum_new(
				"upload_file" /* field_name */,
				filespecification /* field_datum */,
				(char *)0 /* message_datum */,
				0 /* primary_key_index */ );

		list_set(
			form_field_datum_list,
			form_field_datum );

		form_field_insert =
			form_field_insert_new(
				email_address,
				form_name,
				timestamp,
				form_field_datum_list /* in/out */ );

		list_set(
			form_field_insert_list->list,
			form_field_insert );
	}

	return form_field_insert_list;
}

