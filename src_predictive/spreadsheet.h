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

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *spreadsheet_name;
	LIST *spreadsheet_column_list;
} SPREADSHEET;

/* Operations */
/* ---------- */
SPREADSHEET *spreadsheet_new(
			char *spreadsheet_name );

SPREADSHEET *spreadsheet_fetch(
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

#endif
