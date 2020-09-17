/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/spreadsheet_upload_event.c	*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "list.h"
#include "boolean.h"
#include "date.h"
#include "spreadsheet_upload_event.h"

char *spreadsheet_upload_date_tine( void )
{
	/* Safely returns heap memory */
	/* -------------------------- */
	return date_time_now19( date_utc_offset() );
}

char *spreadsheet_upload_file_sha256sum(
			char *spreadsheet_filename )
{
	char sys_string[ 1024 ];

	if ( !timlib_file_exists( spreadsheet_filename ) )
		return (char *)0;

	sprintf( sys_string,
		 "cat \"%s\"			|"
		 "sha256sum			|"
		 "column.e 0			 ",
		 spreadsheet_filename );

	return pipe2string( sys_string );
}

SPREADSHEET_UPLOAD_EVENT *
		spreadsheet_upload_event_calloc(
			void )
{
	SPREADSHEET_UPLOAD_EVENT *i;

	if ( ! ( i = calloc( 1, sizeof( SPREADSHEET_UPLOAD_EVENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return i;
}

SPREADSHEET_UPLOAD_EVENT *
		spreadsheet_upload_event(
			char *spreadsheet_name,
			char *spreadsheet_filename )
{
	SPREADSHEET_UPLOAD_EVENT *
		spreadsheet_upload_event;

	spreadsheet_upload_event =
		spreadsheet_upload_event_calloc();

	spreadsheet_upload_event->
			spreadsheet_name =
				spreadsheet_name;

	spreadsheet_upload_event->
			spreadsheet_filename =
				spreadsheet_filename;

	return spreadsheet_upload_event;
}

