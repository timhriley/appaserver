/* library/report_writer.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "piece.h"
#include "column.h"
#include "document.h"
#include "date.h"
#include "timlib.h"
#include "folder.h"
#include "related_folder.h"
#include "appaserver_error.h"
#include "role.h"
#include "appaserver_parameter.h"
#include "report_writer.h"

REPORT_WRITER *report_writer_new(	char *application_name,
					char *session,
					char *folder_name,
					ROLE *role )
{
	REPORT_WRITER *report_writer;

	if ( ! ( report_writer =
			(REPORT_WRITER *)
			calloc( 1, sizeof( REPORT_WRITER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	report_writer->folder =
		folder_with_load_new(	application_name,
					session,
					folder_name,
					role );

	return report_writer;

} /* report_writer_new() */

