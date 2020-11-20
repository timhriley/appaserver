/* $APPASERVER_HOME/library/email.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef EMAIL_H
#define EMAIL_H

typedef struct
{
	char *from_address;
	char *to_address;
	char *subject;
	char *message;
	char *reply_to;
	char *full_name;
	char *attachment_filename;
} EMAIL;

/* Function prototypes */
/* ------------------- */
EMAIL *email_new( void );

void email_sendmail(	char *from_address,
			char *to_address,
			char *subject,
			char *message,
			char *reply_to,
			char *self_full_name,
			char *attachment_filename );

/* Returns heap memory. */
/* -------------------- */
char *email_sendmail_command(	char *to_address,
				char *subject,
				char *attachment_filename );

/* Returns heap memory. */
/* -------------------- */
char *email_sendmail_string(
			char *from_address,
			char *to_address,
			char *subject,
			char *message,
			char *reply_to,
			char *self_full_name );

#endif
