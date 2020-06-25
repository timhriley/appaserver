/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/spreadsheet.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include "spreadsheet.h"

LIST *spreadsheet_datatype_list( char *application_name )
{
}

LIST *spreadsheet_units_list( char *application_name )
{
}

SPREADSHEET *spreadsheet_new( void )
{
}

UNITS *spreadsheet_translate_units(
				LIST *spreadsheet_units_list,
				char *units_label )
{
}

double spreadsheet_multiply_by(	LIST *spreadsheet_units_list,
				UNITS *units,
				char *units_label )
{
}

LIST *spreadsheet_header_cell_list(
				char *filename,
				char *date_heading_label,
				boolean two_lines,
				LIST *spreadsheet_datatype_list,
				LIST *spreadsheet_units_list )
{
}

/* Format: datatype^column_piece^multiply_by */
/* ----------------------------------------- */
LIST *spreadsheet_text_output_list(
				LIST *spreadsheet_header_cell_list )
{
}

