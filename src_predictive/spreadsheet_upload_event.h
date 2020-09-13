/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/spreadsheet_upload_event.h	*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef SPREADSHEET_UPLOAD_EVENT_H
#define SPREADSHEET_UPLOAD_EVENT_H

#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *spreadsheet_name;
	char *spreadsheet_filename;
	char *login_name;

	/* Process */
	/* ------- */
	char *spreadsheet_upload_date_time;
	char *spreadsheet_upload_file_sha256sum;

} SPREADSHEET_UPLOAD_EVENT;

/* Operations */
/* ---------- */

SPREADSHEET_UPLOAD_EVENT *
		spreadsheet_upload_event_calloc(
			void );

SPREADSHEET_UPLOAD_EVENT *
		spreadsheet_upload_event(
			char *spreadsheet_name,
			char *spreadsheet_filename );

/* Safely returns heap memory */
/* -------------------------- */
char *spreadsheet_upload_date_tine(
			void );

/* Returns heap memory or null */
/* --------------------------- */
char *spreadsheet_upload_file_sha256sum(
			char *spreadsheet_filename );

#endif
