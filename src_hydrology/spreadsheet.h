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
	char *datatype_label;
	char *units_label;
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
				char *filename,
				char *date_heading_label,
				boolean two_lines,
				LIST *spreadsheet_datatype_list,
				LIST *spreadsheet_units_list );

/* Format: datatype^column_piece^multiply_by */
/* ----------------------------------------- */
LIST *spreadsheet_text_output_list(
				LIST *spreadsheet_header_cell_list );

#endif
