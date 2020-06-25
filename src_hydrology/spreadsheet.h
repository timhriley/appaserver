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
#include "datatype.h"

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
	DATATYPE *spreadsheet_translate_datatype;
	double spreadsheet_convert_multiply_by;
} SPREADSHEET_HEADER_CELL;

typedef struct
{
	LIST *spreadsheet_datatype_list;
	LIST *spreadsheet_units_list;
	LIST *spreadsheet_header_cell_list;
	LIST *spreadsheet_output_datatype_list;
} SPREADSHEET;

/* Operations */
/* ---------- */

LIST *spreadsheet_datatype_list(
				char *application_name );

LIST *spreadsheet_units_list(	char *application_name );

SPREADSHEET *spreadsheet_new(	void );

SPREADSHEET_HEADER_CELL *spreadsheet_header_cell_new(
				void );

DATATYPE *spreadsheet_translate_datatype(
				char *datatype_label,
				LIST *spreadsheet_datatype_list );

UNITS *spreadsheet_translate_units(
				char *units_label,
				LIST *spreadsheet_units_list );

double spreadsheet_convert_multiply_by(
				LIST *spreadsheet_units_converted_list,
				UNITS *units,
				char *units_label );

LIST *spreadsheet_header_cell_list(
				char *station_name,
				char *filename,
				char *date_header_label,
				boolean two_lines,
				LIST *spreadsheet_datatype_list,
				LIST *spreadsheet_units_list );

LIST *spreadsheet_output_datatype_list(
				LIST *spreadsheet_header_cell_list );

SPREADSHEET *spreadsheet_fetch(
				char *application_name,
				char *station_name,
				char *filename,
				char *date_header_label,
				boolean two_lines );

/* Returns heap memory and populates heap memory. */
/* ---------------------------------------------- */
char *spreadsheet_header_buffer(
				char **second_line,
				char *filename,
				char *date_header_label,
				boolean two_lines );

boolean spreadsheet_header_label_success(
				char *date_header_label,
				char *header_buffer );

/* -------------------------------------------- */
/* Returns heap memory.				*/
/* Heading label looks like: Salinity (PSU)	*/
/* -------------------------------------------- */
char *spreadsheet_header_label(
				char *header_buffer,
				char *second_line,
				int column_piece );

/* Returns heap memory. */
/* -------------------- */
char *spreadsheet_datatype_label(
				/* ---------------------------------------- */
				/* Heading label looks like: Salinity (PSU) */
				/* ---------------------------------------- */
				char *spreadsheet_header_label );

/* Returns heap memory. */
/* -------------------- */
char *spreadsheet_units_label(
				/* ---------------------------------------- */
				/* Heading label looks like: Salinity (PSU) */
				/* ---------------------------------------- */
				char *spreadsheet_header_label );

SPREADSHEET_HEADER_CELL *spreadsheet_header_cell_parse(
				char *datatype_heading_first_line,
				char *second_line,
				int column_piece,
				LIST *spreadsheet_datatype_list,
				LIST *spreadsheet_units_list );

#endif
