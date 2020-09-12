/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/spreadsheet_column.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef SPREADSHEET_COLUMN_H
#define SPREADSHEET_COLUMN_H

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
	char *heading;
	char *folder_name;
	char *attribute_name;
	char *description;

	/* Process */
	/* ------- */
	int spreadsheet_column_piece;
	char *spreadsheet_column_cell_extract;
} SPREADSHEET_COLUMN;

/* Operations */
/* ---------- */

/* Returns -1 if not found. */
/* ------------------------ */
int spreadsheet_column_piece(
			char *heading_line,
			char *heading );

/* Returns heap memory or null */
/* --------------------------- */
char *spreadhsheet_column_cell_extract(
			char *input_row,
			int spreadsheet_column_piece );

SPREADSHEET_COLUMN *spreadsheet_column_new(
			char *heading );

SPREADSHEET_COLUMN *spreadsheet_column_heading_seek(
			LIST *spreadsheet_column_list,
			char *heading );

SPREADSHEET_COLUMN *spreadsheet_column_attribute_seek(
			LIST *spreadsheet_column_list,
			char *folder_name,
			char *attribute_name );

SPREADSHEET_COLUMN *spreadsheet_column_parse(
			char *input );

LIST *spreadsheet_column_system_list(
			char *sys_string );

/* Safely returns heap memory */
/* -------------------------- */
char *spreadsheet_column_sys_string(
			char *where );

#endif
