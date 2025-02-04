/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/post_signup_receive.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "form_field.h"
#include "post_signup_receive.h"

POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record_new(
		LIST *form_field_datum_list )
{
	POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record;
	FORM_FIELD_DATUM *form_field_datum;

	post_signup_receive_record =
		post_signup_receive_record_calloc();

	if ( ( form_field_datum =
			form_field_datum_seek(
				form_field_datum_list,
				"application_key" ) ) )
	{
		post_signup_receive_record->application_key =
			form_field_datum->field_datum;
	}

	if ( ( form_field_datum =
			form_field_datum_seek(
				form_field_datum_list,
				"application_title" ) ) )
	{
		post_signup_receive_record->application_title =
			form_field_datum->field_datum;
	}

	return post_signup_receive_record;
}

POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record_calloc( void )
{
	POST_SIGNUP_RECEIVE_RECORD *post_signup_receive_record;

	if ( ! ( post_signup_receive_record =
			calloc( 1,
				sizeof ( POST_SIGNUP_RECEIVE_RECORD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_signup_receive_record;
}
