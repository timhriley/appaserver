/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_spreadsheet_upload_event.c	*/
/* -----------------------------------------------------------------	*/
/* No warranty and freely available software. See Appaserver.org	*/
/* -----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "date.h"
#include "appaserver_error.h"
#include "paypal_spreadsheet_upload_event.h"

char *paypal_spreadsheet_upload_date_time( void )
{
	/* Returns heap memory */
	/* ------------------- */
	return date_time_now19( date_utc_offset() );
}

char *paypal_spreadsheet_upload_event_sha256sum(
		char *spreadsheet_filename )
{
	char system_string[ 1024 ];

	if ( !timlib_file_exists( spreadsheet_filename ) ) return NULL;

	sprintf(system_string,
		"cat \"%s\"			|"
		"sha256sum			|"
		"column.e 0			 ",
		spreadsheet_filename );

	return string_pipe_fetch( system_string );
}

PAYPAL_SPREADSHEET_UPLOAD_EVENT *
	paypal_spreadsheet_upload_event_calloc(
		void )
{
	PAYPAL_SPREADSHEET_UPLOAD_EVENT *paypal_spreadsheet_upload_event;

	if ( ! ( paypal_spreadsheet_upload_event =
			calloc( 1,
				sizeof ( PAYPAL_SPREADSHEET_UPLOAD_EVENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return paypal_spreadsheet_upload_event;
}

PAYPAL_SPREADSHEET_UPLOAD_EVENT *
	paypal_spreadsheet_upload_event_new(
		char *spreadsheet_name,
		char *spreadsheet_filename )
{
	PAYPAL_SPREADSHEET_UPLOAD_EVENT *paypal_spreadsheet_upload_event;

	if ( !spreadsheet_name
	||   !spreadsheet_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	paypal_spreadsheet_upload_event =
		paypal_spreadsheet_upload_event_calloc();

	paypal_spreadsheet_upload_event->spreadsheet_name = spreadsheet_name;

	paypal_spreadsheet_upload_event->spreadsheet_filename =
		spreadsheet_filename;

	paypal_spreadsheet_upload_event->date_time =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		paypal_spreadsheet_upload_event_date_time();

	paypal_spreadsheet_upload_event->sha256sum =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		paypal_spreadsheet_upload_event_sha256sum(
			spreadsheet_filename );

	return paypal_spreadsheet_upload_event;
}

