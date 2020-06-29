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
	int column_piece;
	/* --------------------------- */
	/* Looks like: Waterflow (GPM) */
	/* --------------------------- */
	char *spreadsheet_header_label;
	/* --------------------- */
	/* Looks like: Waterflow */
	/* --------------------- */
	char *spreadsheet_datatype_label;
	/* --------------- */
	/* Looks like: GPM */
	/* --------------- */
	char *spreadsheet_units_label;
	/* ---------------------------------------- */
	/* Looks like: DATATYPE->datatype_name=flow */
	/* ---------------------------------------- */
	DATATYPE *spreadsheet_translate_datatype;
	/* --------------- */
	/* Looks like: CFS */
	/* --------------- */
	char *spreadsheet_translate_units_name;
	/* -------------------- */
	/* Looks like: 0.002228 */
	/* -------------------- */
	double spreadsheet_units_converted_multiply_by;
} SPREADSHEET_HEADER_CELL;

typedef struct
{
	char *application_name;
	char *station_name;
	/* ----------------------------- */
	/* Looks like: Date Sample Taken */
	/* ----------------------------- */
	char *date_header_label;
	char *filename;
	boolean two_lines;
	char *second_line;
	char *spreadsheet_header_row;
	LIST *spreadsheet_datatype_list;
	LIST *spreadsheet_shef_upload_datatype_list;
	LIST *spreadsheet_station_datatype_alias_list;
	LIST *spreadsheet_header_cell_list;
	LIST *spreadsheet_output_datatype_list;
} SPREADSHEET;

/* Operations */
/* ---------- */

LIST *spreadsheet_datatype_list(
				char *application_name,
				char *station_name );

SPREADSHEET *spreadsheet_new(	char *application_name,
				char *station_name,
				/* ---------------- */
				/* Looks like: Date */
				/* ---------------- */
				char *date_header_label,
				char *filename,
				boolean two_lines );

SPREADSHEET_HEADER_CELL *spreadsheet_header_cell_new(
				int column_piece,
				char *spreadsheet_header_label );

/* Looks like: DATATYPE->datatype_name=flow */
/* ---------------------------------------- */
DATATYPE *spreadsheet_translate_datatype(
				char *spreadsheet_datatype_label,
				LIST *spreadsheet_shef_upload_datatype_list,
				LIST *spreadsheet_station_datatype_alias_list,
				LIST *spreadsheet_datatype_list );

/* Looks like: UNITS->units_name = CFS */
/* ----------------------------------- */
UNITS *spreadsheet_translate_units(
				char *spreadsheet_units_label,
				LIST *units_alias_list );

LIST *spreadsheet_header_cell_list(
				char *spreadsheet_header_row,
				char *second_line,
				LIST *spreadsheet_shef_upload_datatype_list,
				LIST *spreadsheet_station_datatype_alias_list,
				LIST *spreadsheet_datatype_list );

LIST *spreadsheet_output_datatype_list(
				LIST *spreadsheet_header_cell_list );

/* Sets:
	spreadsheet->spreadsheet_datatype_list
	spreadsheet->spreadsheet_header_row
	spreadsheet->spreadsheet_header_cell_list
------------------------------------------------- */
SPREADSHEET *spreadsheet_fetch(
				char *application_name,
				char *station_name,
				char *filename,
				/* ---------------- */
				/* Looks like: Date */
				/* ---------------- */
				char *date_header_label,
				boolean two_lines );

/* -------------------------------------------------------------------- */
/* Returns heap memory and populates heap memory. 			*/
/* Looks like:								*/
/* Sensor,Date Sample Taken,IsDataValid,Pressure (psi),Pressure (psi)	*/
/* -------------------------------------------------------------------- */
char *spreadsheet_header_row(
				char **second_line,
				char *filename,
				char *date_header_label,
				boolean two_lines );

boolean spreadsheet_header_label_success(
				char *date_header_label,
				char *header_buffer );

/* ---------------------------- */
/* Returns heap memory.		*/
/* Looks like: Salinity (PSU)	*/
/* ---------------------------- */
char *spreadsheet_header_label(
				boolean *all_done,
				char *spreadsheet_header_row,
				char *second_line,
				int column_piece );

/* -------------------- */
/* Returns heap memory. */
/* Looks like: Salinity */
/* -------------------- */
char *spreadsheet_datatype_label(
				char *spreadsheet_header_label );

/* -------------------- */
/* Returns heap memory. */
/* Looks like: PSU	*/
/* -------------------- */
char *spreadsheet_units_label(
				char *spreadsheet_header_label );

SPREADSHEET_HEADER_CELL *spreadsheet_header_cell_parse(
				boolean *all_done,
				char *spreadsheet_header_row,
				char *second_line,
				int column_piece,
				LIST *spreadsheet_shef_upload_datatype_list,
				LIST *spreadsheet_station_datatype_alias_list,
				LIST *spreadsheet_datatype_list );

/* ---------------------------- */
/* Returns units_name or null.  */
/* Looks like: CFS		*/
/* ---------------------------- */
char *spreadsheet_translate_units_name(
				char *spreadsheet_units_label,
				char *units_name,
				LIST *units_alias_list,
				LIST *units_converted_list );

double spreadsheet_units_converted_multiply_by(
				char *spreadsheet_units_label,
				LIST *units_converted_list );

LIST *spreadsheet_station_datatype_alias_list(
				char *application_name,
				char *station_name );

DATATYPE *spreadsheet_seek_datatype(
				LIST *spreadsheet_header_cell_list,
				char *datatype_name );

#endif
