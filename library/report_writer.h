/* library/report_writer.h				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */


#ifndef REPORT_WRITER_H
#define REPORT_WRITER_H

#include <stdio.h>
#include "list.h"

/* Constants */
/* --------- */
/* #define DEBUG_MODE			1 */

/* Structures */
/* ---------- */

typedef struct
{
	FOLDER *folder;
} REPORT_WRITER;


/* Prototypes */
/* ---------- */

REPORT_WRITER *report_writer_new(	char *application_name,
					char *session,
					char *folder_name,
					ROLE *role );

#endif
