/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/spreadsheet.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <stdio.h>
#include <unistd.h>
#include "list.h"
#include "boolean.h"
#include "appaserver_link.h"

typedef struct
{
	char *data;
	boolean string_boolean;
} SPREADSHEET_CELL;

/* Usage */
/* ----- */
SPREADSHEET_CELL *spreadsheet_cell_new(
			char *data,
			boolean string_boolean );

/* Process */
/* ------- */
SPREADSHEET_CELL *spreadsheet_cell_calloc(
			void );

/* Usage */
/* ----- */
void spreadsheet_cell_output(
			SPREADSHEET_CELL *spreadsheet_cell,
			FILE *output_file );

/* Process */
/* ------- */

typedef struct
{
	LIST *cell_list;
} SPREADSHEET_ROW;

/* Usage */
/* ----- */
SPREADSHEET_ROW *spreadsheet_row_new(
			void );

/* Process */
/* ------- */
SPREADSHEET_ROW *spreadsheet_row_calloc(
			void );

/* Usage */
/* ----- */
void spreadsheet_row_output(
			SPREADSHEET_ROW *spreadsheet_row,
			FILE *output_file );

/* Process */
/* ------- */

typedef struct
{
	char *heading_string;
} SPREADSHEET_HEADING;

/* Usage */
/* ----- */
SPREADSHEET_HEADING *spreadsheet_heading_new(
			char *heading_string );

/* Process */
/* ------- */
SPREADSHEET_HEADING *spreadsheet_heading_calloc(
			void );

/* Usage */
/* ----- */
void spreadsheet_heading_list_output(
			LIST *heading_list,
			FILE *output_file );

typedef struct
{
	LIST *heading_list;
	LIST *row_list;
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *appaserver_link_anchor_html;
} SPREADSHEET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SPREADSHEET *spreadsheet_new(
			char *application_name,
			char *data_directory,
			pid_t process_id );

/* Process */
/* ------- */
SPREADSHEET *spreadsheet_calloc(
			void );

/* Driver */
/* ------ */
void spreadsheet_output(
			SPREADSHEET *spreadsheet,
			FILE *output_file );

/* Process */
/* ------- */

#endif

