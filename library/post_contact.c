/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_contact.c		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "String.h"
#include "environ.h"
#include "security.h"
#include "upload_source.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "post_contact.h"

POST_CONTACT *post_contact_new( char *argv_0 )
{
	POST_CONTACT *post_contact;

	if ( !argv_0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: argv_0 is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_contact = post_contact_calloc();

	post_contact->post_contact_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_contact_input_new(
			argv_0 );

	if (	post_contact->
			post_contact_input->
			bot_generated->
			yes_boolean
	||  	post_contact->
			post_contact_input->
			email_invalid_boolean
	|| 	post_contact->
			post_contact_input->
			reason_invalid_boolean
	|| (	post_contact->
			post_contact_input->
			message_empty_boolean
	&&	!post_contact->
			post_contact_input->
			filespecification_boolean ) )
	{
		post_contact->message_not_sent_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_contact_display_system_string(
				POST_CONTACT_NOT_SENT_FILENAME,
				post_contact->
					post_contact_input->
					appaserver_parameter->
					document_root );

		return post_contact;
	}

	post_contact->mailx_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_contact_mailx_system_string(
			POST_CONTACT_DESTINATION_EMAIL,
			POST_CONTACT_ENTITY,
			post_contact->
				post_contact_input->
				bot_generated->
				remote_ip_address,
			post_contact->
				post_contact_input->
				email_address,
			post_contact->
				post_contact_input->
				reason,
			post_contact->
				post_contact_input->
				filespecification,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_mailname(
				APPASERVER_MAILNAME_FILESPECIFICATION ) );

	post_contact->message_sent_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_contact_display_system_string(
			POST_CONTACT_SENT_FILENAME,
			post_contact->
				post_contact_input->
				appaserver_parameter->
				document_root );

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

char *post_contact_display_system_string(
		const char *message_filename,
		char *document_root )
{
	char system_string[ 1024 ];

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

	return strdup( system_string );
}

char *post_contact_mailx_system_string(
		const char *post_contact_destination_email,
		const char *post_contact_entity,
		char *remote_ip_address,
		char *email_address,
		char *reason,
		char *filespecification,
		char *appaserver_mailname )
{
	char system_string[ 1024 ];
	char *ptr = system_string;

	if ( !remote_ip_address
	||   !email_address
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
		post_contact_subject(
			post_contact_entity,
			remote_ip_address,
			email_address,
			reason ),
		appaserver_mailname );

	if ( filespecification )
	{
		ptr += sprintf( ptr, " -A '%s'", filespecification );
	}

	ptr += sprintf( ptr, " %s", post_contact_destination_email );

	return strdup( system_string );
}

char *post_contact_subject(
		const char *post_contact_entity,
		char *remote_ip_address,
		char *email_address,
		char *reason )
{
	static char subject[ 512 ];

	if ( !remote_ip_address
	||   !email_address
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
		post_contact_entity,
		reason,
		email_address,
		remote_ip_address );

	return subject;
}

void post_contact_mailx(
		char *message,
		char *system_string )
{
	FILE *pipe;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	post_contact_write(
		message,
		pipe );

	pclose( pipe );
}

void post_contact_write(
		char *message,
		FILE *pipe )
{
	if ( !pipe )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: pipe is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	if ( message )
		fprintf( pipe, "%s\n", message );
	else
		fprintf( pipe, "No message\n" );

	/* environ_display( pipe ); */
}

POST_CONTACT_INPUT *post_contact_input_new( char *argv_0 )
{
	POST_CONTACT_INPUT *post_contact_input;

	post_contact_input = post_contact_input_calloc();

	post_contact_input->bot_generated =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		bot_generated_new(
			argv_0 );

	if ( post_contact_input->bot_generated->yes_boolean )
		return post_contact_input;

	post_contact_input->upload_filename_list =
		/* ----------- */
		/* List of one */
		/* ----------- */
		post_contact_input_upload_filename_list();

	post_contact_input->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_contact_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			POST_CONTACT_APPLICATION,
			post_contact_input->
				appaserver_parameter->
				upload_directory,
			post_contact_input->upload_filename_list );

	post_contact_input->email_address =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_contact_input_email_address(
			post_contact_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_input->email_invalid_boolean =
		post_contact_input_email_invalid_boolean(
			post_contact_input->email_address );

	if ( post_contact_input->email_invalid_boolean )
		return post_contact_input;

	post_contact_input->reason =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_contact_input_reason(
			post_contact_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_input->reason_invalid_boolean =
		post_contact_input_reason_invalid_boolean(
			post_contact_input->reason );

	if ( post_contact_input->reason_invalid_boolean )
		return post_contact_input;

	post_contact_input->message =
		/* ----------------------------------------------------- */
		/* Returns component of dictionary, heap memory, or null */
		/* ----------------------------------------------------- */
		post_contact_input_message(
			post_contact_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_input->message_empty_boolean =
		post_contact_input_message_empty_boolean(
			post_contact_input->message );

	post_contact_input->filespecification =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		post_contact_input_filespecification(
			POST_DICTIONARY_SPOOL_PREFIX,
			post_contact_input->
				post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	post_contact_input->filespecification_boolean =
		post_contact_input_filespecification_boolean(
			post_contact_input->filespecification );

	return post_contact_input;
}

POST_CONTACT_INPUT *post_contact_input_calloc( void )
{
	POST_CONTACT_INPUT *post_contact_input;

	if ( ! ( post_contact_input =
			calloc( 1,
				sizeof ( POST_CONTACT_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_contact_input;
}

LIST *post_contact_input_upload_filename_list( void )
{
	LIST *filename_list = list_new();
	list_set( filename_list, "upload_file" );

	return filename_list;
}

char *post_contact_input_email_address( DICTIONARY *post_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		"email_address",
		post_dictionary );
}

boolean post_contact_input_email_invalid_boolean(
		char *post_contact_input_email_address )
{
	return
	!string_email_address_boolean(
		post_contact_input_email_address );
}

char *post_contact_input_reason( DICTIONARY *post_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		"reason",
		post_dictionary );
}

boolean post_contact_input_reason_invalid_boolean( char *reason )
{
	return
	!string_mnemonic_boolean( reason );
}

char *post_contact_input_message( DICTIONARY *post_dictionary )
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

boolean post_contact_input_message_empty_boolean( char *message )
{
	if ( !message )
		return 1;
	else
		return 0;
}

char *post_contact_input_filespecification(
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

boolean post_contact_input_filespecification_boolean( char *filespecification )
{
	return
	upload_source_file_exists_boolean(
		filespecification
			/* directory_filename_session */ );
}
