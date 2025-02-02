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
#include "appaserver.h"
#include "post.h"
#include "appaserver_error.h"
#include "post.h"

POST *post_new(
		const char *form_name,
		char *email_address,
		char *ip_address )
{
}

POST *post_calloc( void )
{
}

char *post_insert_statement(
		const char *post_table,
		const char *post_insert_columns,
		const char *form_name,
		char *email_address,
		char *ip_address,
		char *post_timestamp )
{
}

void post_mailx(
		char *message,
		char *mailx_system_string )
{
}

char *post_return_email(
		const char *post_return_username,
		char *appaserver_mailname )
{
}

char *post_mailx_system_string(
		const char *subject,
		char *post_return_email )
{
}

POST *post_fetch(
		char *email_address,
		char *timestamp )
{
}

char *post_primary_where(
		char *email_address,
		char *timestamp )
{
}

POST *post_parse(
		char *post_primary_where,
		char *string_fetch )
{
}

