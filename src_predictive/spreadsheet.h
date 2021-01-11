/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/spreadsheet.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "list.h"
#include "boolean.h"
#include "spreadsheet_upload_event.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *spreadsheet_filename;
	char *date_label;

	/* Process */
	/* ------- */
	LIST *spreadsheet_column_list;
	SPREADSHEET_UPLOAD_EVENT *spreadsheet_upload_event;

	char *spreadsheet_minimum_date;
	char *maximum_date;
} SPREADSHEET;

/* Operations */
/* ---------- */
SPREADSHEET *spreadsheet_calloc(
			void );

SPREADSHEET *spreadsheet_fetch(
			char *spreadsheet_filename,
			char *date_label );

LIST *spreadsheet_column_list(
			char *spreadsheet_filename,
			char *date_label );

/* Returns heap memory or null */
/* --------------------------- */
char *spreadsheet_minimum_date(
			char **maximum_date,
			int *row_count,
			char *spreadsheet_filename );

/* Returns heap memory or null */
/* --------------------------- */
char *spreadsheet_heading_data(
			LIST *spreadsheet_column_list,
			char *input_row,
			char *heading );

char *spreadsheet_header_row(
			char *filename,
			char *date_label );

boolean spreadsheet_header_label_success(
			char *date_label,
			char *header_buffer );

#endif
