/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/pkzip.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "application.h"
#include "date.h"
#include "pkzip.h"

PKZIP *pkzip_new(
		char *application_name,
		LIST *filename_list,
		char *filename_stem,
		char *data_directory )
{
	PKZIP *pkzip;

	if ( !application_name
	||   !filename_stem
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( filename_list ) ) return (PKZIP *)0;

	pkzip = pkzip_calloc();

	pkzip->date_now_yyyy_mm_dd =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now_yyyy_mm_dd(
			date_utc_offset() );

	pkzip->appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			application_server_address(),
			data_directory,
			filename_stem,
			application_name,
			getpid() /* process_id */,
			(char *)0 /* session_key */,
			pkzip->date_now_yyyy_mm_dd /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"zip" /* extension */ );

	pkzip->system_string =
		/* ---------------------------- */
		/* Returns heap memory or null */
		/* ---------------------------- */
		pkzip_system_string(
			PKZIP_EXECUTABLE,
			filename_list,
			pkzip->
				appaserver_link->
				appaserver_link_output->
				filename );

	pkzip->appaserver_link_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			pkzip->
				appaserver_link->
				appaserver_link_prompt->
				filename,
			"_new" /* target_window */,
			"Link to zip file" /* prompt_message */ );

	return pkzip;
}

PKZIP *pkzip_calloc( void )
{
	PKZIP *pkzip;

	if ( ! ( pkzip = calloc( 1, sizeof ( PKZIP ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return pkzip;
}

char *pkzip_system_string(
		const char *pkzip_executable,
		LIST *filename_list,
		char *output_filename )
{
	char system_string[ 4096 ];
	char *ptr = system_string;

	if ( !output_filename )
	{
		char message[ 128 ];

		sprintf(message, "output_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( filename_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s %s",
		pkzip_executable,
		output_filename );

	do {
		ptr += sprintf(
			ptr,
			" %s",
			(char *)list_get( filename_list ) );

	} while ( list_next( filename_list ) );

	return strdup( system_string );
}
