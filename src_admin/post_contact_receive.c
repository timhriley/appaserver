/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_contact_receive.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "application.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "post_contact_submit.h"
#include "post_contact_receive.h"

POST_CONTACT_RECEIVE *post_contact_receive_new(
		int argc,
		char **argv )
{
	POST_CONTACT_RECEIVE *post_contact_receive;

	post_contact_receive = post_contact_receive_calloc();

	post_contact_receive->post_receive =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_receive_new(
			APPLICATION_ADMIN_NAME,
			APPASERVER_MAILNAME_FILESPECIFICATION,
			argc,
			argv );

	post_contact_receive->post =
		post_fetch(
			post_contact_receive->
				post_receive->
				email_address,
			post_contact_receive->
				post_receive->
				timestamp_space );

	if ( !post_contact_receive->post ) return post_contact_receive;

	post_contact_receive->post_contact =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_contact_fetch(
			post_contact_receive->
				post_receive->
				email_address,
			post_contact_receive->
				post_receive->
				timestamp_space );

	if ( !post_contact_receive->post_contact ) return post_contact_receive;

	post_contact_receive->upload_filespecification =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		post_contact_receive_upload_filespecification(
			APPLICATION_ADMIN_NAME,
			post_contact_receive->
				post_receive->
				appaserver_parameter->
				upload_directory,
			post_contact_receive->
				post_contact->
				upload_file );

	post_contact_receive->mailx_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_contact_receive_mailx_system_string(
			POST_CONTACT_RECEIVE_DESTINATION_EMAIL,
			POST_CONTACT_RECEIVE_ENTITY,
			POST_RETURN_USERNAME,
			post_contact_receive->
				post_receive->
				email_address,
			post_contact_receive->
				post->
				ip_address,
			post_contact_receive->
				post_contact->
				reason,
			post_contact_receive->upload_filespecification,
			post_contact_receive->
				post_receive->
				appaserver_mailname );

	post_contact_receive->post_confirmation_update_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_confirmation_update_statement(
			POST_TABLE,
			POST_CONFIRMATION_COLUMN,
			post_contact_receive->post_receive->email_address,
			post_contact_receive->post_receive->timestamp_space );

	post_contact_receive->display_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_contact_receive_display_system_string(
			POST_CONTACT_RECEIVE_SENT_FILENAME,
			post_contact_receive->
				post_receive->
				appaserver_parameter->
				document_root );

	return post_contact_receive;
}

POST_CONTACT_RECEIVE *post_contact_receive_calloc( void )
{
	POST_CONTACT_RECEIVE *post_contact_receive;

	if ( ! ( post_contact_receive =
			calloc( 1,
				sizeof ( POST_CONTACT_RECEIVE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_contact_receive;
}

char *post_contact_receive_mailx_system_string(
		const char *destination_email,
		const char *post_contact_receive_entity,
		const char *post_return_username,
		char *email_address,
		char *ip_address,
		char *reason,
		char *upload_filespecification,
		char *appaserver_mailname )
{
	char system_string[ 1024 ];
	char *ptr = system_string;

	if ( !email_address
	||   !ip_address
	||   !reason
	||   !appaserver_mailname )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"mailx -s \"%s\" -r %s@%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_contact_receive_subject(
			post_contact_receive_entity,
			email_address,
			ip_address,
			reason ),
		post_return_username,
		appaserver_mailname );

	if ( upload_filespecification )
	{
		ptr += sprintf( ptr, " -A '%s'", upload_filespecification );
	}

	ptr += sprintf( ptr, " %s", destination_email );

	return strdup( system_string );
}

char *post_contact_receive_subject(
		const char *post_contact_receive_entity,
		char *email_address,
		char *ip_address,
		char *reason )
{
	static char subject[ 256 ];

	if ( !email_address
	||   !ip_address
	||   !reason )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		subject,
		sizeof ( subject ),
		"%s [%s] from %s %s",
		post_contact_receive_entity,
		reason,
		email_address,
		ip_address );

	return subject;
}

char *post_contact_receive_display_system_string(
		const char *message_filename,
		char *document_root )
{
	if ( !document_root )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: document_root is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	post_contact_submit_display_system_string(
		message_filename,
		document_root );
}

char *post_contact_receive_upload_filespecification(
		const char *application_admin_name,
		char *upload_directory,
		char *upload_file )
{
	char upload_filespecification[ 1024 ];

	if ( !upload_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: upload_directory is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !upload_file ) return NULL;

	snprintf(
		upload_filespecification,
		sizeof ( upload_filespecification ),
		"%s/%s/%s",
		upload_directory,
		application_admin_name,
		upload_file );

	return strdup( upload_filespecification );
}
