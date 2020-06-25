/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/spreadsheet.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

/* Includes */
/* -------- */
#include "list.h"
#include "units.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *spreadsheet_datatype_label;
	char *spreadsheet_units_label;
	int column_piece;
	UNITS *spreadsheet_translate_units;
	double spreadsheet_multiply_by;
} SPREADSHEET_HEADER_CELL;

typedef struct
{
	LIST *spreadsheet_datatype_list;
	LIST *spreadsheet_units_list;
	LIST *spreadsheet_header_cell_list;
	LIST *spreadsheet_text_output_list;
} SPREADSHEET;

/* Operations */
/* ---------- */
char *spreadsheet_datatype_label(
LIST *spreadsheet_datatype_list(
				char *application_name );

LIST *spreadsheet_units_list(	char *application_name );

SPREADSHEET *spreadsheet_new(	void );

UNITS *spreadsheet_translate_units(
				LIST *spreadsheet_units_list,
				char *units_label );

double spreadsheet_multiply_by(	LIST *spreadsheet_units_list,
				UNITS *units,
				char *units_label );

LIST *spreadsheet_header_cell_list(
				char *station_name,
				char *filename,
				char *date_heading_label,
				boolean two_lines,
				LIST *spreadsheet_datatype_list,
				LIST *spreadsheet_units_list );

LIST *spreadsheet_output_datatype_list(
				LIST *spreadsheet_header_cell_list );

SPREADHSEET *spreadsheet_fetch(
				char *application_name,
				char *station_name,
				char *filename,
				char *date_heading_label,
				boolean two_lines );

#endif
