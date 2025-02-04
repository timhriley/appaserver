/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_contact_receive.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_CONTACT_RECEIVE_H
#define POST_CONTACT_RECEIVE_H

#include "boolean.h"
#include "list.h"
#include "post.h"

#define POST_CONTACT_RECEIVE_DESTINATION_EMAIL	"timriley@timriley.net"
#define POST_CONTACT_RECEIVE_APPLICATION	"appahost"
#define POST_CONTACT_RECEIVE_ENTITY		"Appahost"
#define POST_CONTACT_RECEIVE_SENT_FILENAME	"message_sent.php"
#define POST_CONTACT_RECEIVE_EXECUTABLE		"post_contact_receive_execute"

typedef struct
{
	char *reason;
	char *message;
	char *upload_file;
} POST_CONTACT_RECEIVE_RECORD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CONTACT_RECEIVE_RECORD *post_contact_receive_record_new(
		LIST *form_field_datum_list );

/* Process */
/* ------- */
POST_CONTACT_RECEIVE_RECORD *post_contact_receive_record_calloc(
		void );

typedef struct
{
	POST_RECEIVE_INPUT *post_receive_input;
	POST_CONTACT_RECEIVE_RECORD *post_contact_receive_record;
	char *mailx_system_string;
	char *display_system_string;
} POST_CONTACT_RECEIVE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CONTACT_RECEIVE *post_contact_receive_new(
		int argc,
		char **argv );

/* Process */
/* ------- */
POST_CONTACT_RECEIVE *post_contact_receive_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_contact_receive_mailx_system_string(
		const char *post_contact_receive_destination_email,
		const char *post_contact_receive_entity,
		char *email_address,
		char *remote_ip_address,
		char *reason,
		char *filespecification,
		char *appaserver_mailname );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *post_contact_receive_subject(
		const char *post_contact_receive_entity,
		char *email_address,
		char *remote_ip_address,
		char *reason );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_contact_receive_display_system_string(
		const char *message_filename,
		char *document_root );

#endif
