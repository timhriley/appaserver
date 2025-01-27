/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_spreadsheet_upload_event.h	*/
/* -----------------------------------------------------------------	*/
/* No warranty and freely available software. See Appaserver.org	*/
/* -----------------------------------------------------------------	*/

#ifndef PAYPAL_SPREADSHEET_UPLOAD_EVENT_H
#define PAYPAL_SPREADSHEET_UPLOAD_EVENT_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *spreadsheet_name;
	char *spreadsheet_filename;
	char *date_time;
	char *sha256sum;
} PAYPAL_SPREADSHEET_UPLOAD_EVENT;

/* Usage */
/* ----- */
PAYPAL_SPREADSHEET_UPLOAD_EVENT *
	paypal_spreadsheet_upload_event_new(
		char *spreadsheet_name,
		char *spreadsheet_filename );

/* Process */
/* ------- */
PAYPAL_SPREADSHEET_UPLOAD_EVENT *
	paypal_spreadsheet_upload_event_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *paypal_spreadsheet_upload_event_date_time(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *paypal_spreadsheet_upload_event_sha256sum(
		char *spreadsheet_filename );

#endif
