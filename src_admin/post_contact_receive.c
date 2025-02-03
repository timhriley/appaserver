/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_contact_receive.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "form_field_datum.h"
#include "post_contact_submit.h"
#include "post_contact_receive.h"

POST_CONTACT_RECEIVE_RECORD *post_contact_receive_record_new(
		LIST *form_field_datum_list )
{
	POST_CONTACT_RECEIVE_RECORD *post_contact_receive_record;
	FORM_FIELD_DATUM *form_field_datum;

	post_contact_receive_record =
		post_contact_receive_record_calloc();

	if ( ( form_field_datum =
			form_field_datum_seek(
				form_field_datum_list,
				"reason" ) ) )
	{
		post_contact_receive_record->reason =
			form_field_datum->field_datum;
	}

	if ( ( form_field_datum =
			form_field_datum_seek(
				form_field_datum_list,
				"message" ) ) )
	{
		post_contact_receive_record->message =
			form_field_datum->field_datum;
	}

	if ( ( form_field_datum =
			form_field_datum_seek(
				form_field_datum_list,
				"upload_file" ) ) )
	{
		post_contact_receive_record->upload_file =
			form_field_datum->field_datum;
	}

	return post_contact_receive_record;
}

POST_CONTACT_RECEIVE_RECORD *post_contact_receive_record_calloc( void )
{
	POST_CONTACT_RECEIVE_RECORD *post_contact_receive_record;

	if ( ! ( post_contact_receive_record =
			calloc( 1,
				sizeof ( POST_CONTACT_RECEIVE_RECORD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_contact_receive_record;
}

POST_CONTACT_RECEIVE *post_contact_receive_new(
		int argc,
		char **argv )
{
	POST_CONTACT_RECEIVE *post_contact_receive;

	post_contact_receive = post_contact_receive_calloc();

	post_contact_receive->post_receive_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_receive_input_new(
			argc,
			argv );

	post_contact_receive->post_contact_receive_record =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_contact_receive_record_new(
			post_contact_receive->
				post_receive_input->
				post->
				form_field_datum_list );

	post_contact_receive->mailx_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_contact_receive_mailx_system_string(
			POST_CONTACT_RECEIVE_DESTINATION_EMAIL,
			POST_CONTACT_RECEIVE_ENTITY,
			post_contact_receive->
				post_receive_input->
				post->
				email_address,
			post_contact_receive->
				post_receive_input->
				post->
				ip_address,
			post_contact_receive->
				post_contact_receive_record->
				reason,
			post_contact_receive->
				post_contact_receive_record->
				upload_file
					/* filespecification */,
			post_contact_receive->
				post_receive_input->
				appaserver_mailname );

	post_contact_receive->message_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_contact_receive_message_system_string(
			POST_CONTACT_RECEIVE_SENT_FILENAME,
			post_contact_receive->
				post_receive_input->
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
		char *email_address,
		char *remote_ip_address,
		char *reason,
		char *filespecification,
		char *appaserver_mailname )
{
	char system_string[ 1024 ];
	char *ptr = system_string;

	if ( !email_address
	||   !remote_ip_address
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
		"mailx -s \"%s\" -r timriley@%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_contact_receive_subject(
			post_contact_receive_entity,
			email_address,
			remote_ip_address,
			reason ),
		appaserver_mailname );

	if ( filespecification )
	{
		ptr += sprintf( ptr, " -A '%s'", filespecification );
	}

	ptr += sprintf( ptr, " %s", destination_email );

	return strdup( system_string );
}

char *post_contact_receive_subject(
		const char *post_contact_receive_entity,
		char *email_address,
		char *remote_ip_address,
		char *reason )
{
	static char subject[ 256 ];

	if ( !email_address
	||   !remote_ip_address
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
		remote_ip_address );

	return subject;
}

char *post_contact_receive_message_system_string(
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
