/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/pkzip.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PKZIP_H
#define PKZIP_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"
#include "appaserver_link.h"

#define PKZIP_EXECUTABLE	"zip --quiet"

typedef struct
{
	char *date_now_yyyy_mm_dd;
	APPASERVER_LINK *appaserver_link;
	char *system_string;
	char *appaserver_link_anchor_html;
} PKZIP;

/* Usage */
/* ----- */
PKZIP *pkzip_new(
		char *application_name,
		LIST *filename_list,
		char *filename_stem,
		char *data_directory );

/* Process */
/* ------- */
PKZIP *pkzip_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *pkzip_system_string(
		const char *pkzip_executable,
		LIST *filename_list,
		char *output_filename );

#endif
