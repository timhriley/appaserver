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
	char *spreadsheet_name;
	char *date_column_string;

	/* Process */
	/* ------- */
	LIST *spreadsheet_column_list;
	SPREADSHEET_UPLOAD_EVENT *spreadsheet_upload_event;

	char *spreadsheet_minimum_date;
	char *maximum_date;
	int load_count;
} SPREADSHEET;

/* Operations */
/* ---------- */
SPREADSHEET *spreadsheet_fetch(
			char *spreadsheet_name );

SPREADSHEET *spreadsheet_new(
			char *spreadsheet_name );

LIST *spreadsheet_column_list(
			char *spreadsheet_name );

/* Safely returns heap memory. */
/* --------------------------- */
char *spreadsheet_primary_where(
			char *spreadsheet_name );

/* Returns static memory */
/* --------------------- */
char *spreadsheet_escape_name(
			char *spreadsheet_name );

char *spreadsheet_minimum_date(
			char **maximum_date,
			char *spreadsheet_filename );

/* Returns heap memory or null */
/* --------------------------- */
char *spreadsheet_data(
			LIST *spreadsheet_column_list,
			char *input_row,
			char *heading );

#endif
