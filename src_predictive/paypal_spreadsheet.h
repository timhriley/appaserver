/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_spreadsheet.h			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit Appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef PAYPAL_SPREADSHEET_H
#define PAYPAL_SPREADSHEET_H

#include "list.h"
#include "boolean.h"
#include "paypal_spreadsheet_column.h"
#include "paypal_spreadsheet_upload_event.h"

typedef struct
{
	int paypal_spreadsheet_column_date_piece;
	int row_count;
	char minimum_date_international[ 11 ];
	char maximum_date_international[ 11 ];
} PAYPAL_SPREADSHEET_SUMMARY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PAYPAL_SPREADSHEET_SUMMARY *paypal_spreadsheet_summary_new(
		char *spreadsheet_filename,
		char *date_label );

/* Process */
/* ------- */
PAYPAL_SPREADSHEET_SUMMARY *paypal_spreadsheet_summary_calloc(
		void );

typedef struct
{
	char *spreadsheet_filename;
	char *date_label;
	LIST *paypal_spreadsheet_column_list;
	PAYPAL_SPREADSHEET_SUMMARY *paypal_spreadsheet_summary;
	PAYPAL_SPREADSHEET_UPLOAD_EVENT *paypal_spreadsheet_upload_event;
} PAYPAL_SPREADSHEET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PAYPAL_SPREADSHEET *paypal_spreadsheet_fetch(
		char *spreadsheet_filename,
		char *date_label );

/* Process */
/* ------- */
PAYPAL_SPREADSHEET *paypal_spreadsheet_calloc(
		void );


/* Returns heap memory or null */
/* --------------------------- */
char *spreadsheet_minimum_date(
		int *row_count,
		char *spreadsheet_filename,
		char *date_label );

/* Returns heap memory or null */
/* --------------------------- */
char *spreadsheet_maximum_date(
		int *row_count,
		char *spreadsheet_filename,
		char *date_label );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *paypal_spreadsheet_heading_data(
		LIST *paypal_spreadsheet_column_list,
		char *input,
		char *heading_string );

#endif
