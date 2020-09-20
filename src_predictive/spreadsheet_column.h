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
	char *heading;
	int piece_offset;
} SPREADSHEET_COLUMN;

/* Operations */
/* ---------- */
SPREADSHEET_COLUMN *spreadsheet_column_new(
			char *heading,
			int piece_offset );

SPREADSHEET_COLUMN *spreadsheet_column_heading_seek(
			LIST *spreadsheet_column_list,
			char *heading );

#endif
