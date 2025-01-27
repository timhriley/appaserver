/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/html.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef HTML_H
#define HTML_H

#include "list.h"
#include "boolean.h"

#define HTML_TABLE_ROWS_BETWEEN_HEADING		10
#define HTML_TABLE_QUEUE_TOP_BOTTOM		50

typedef struct
{
	char *heading;
	boolean right_justify_boolean;
} HTML_COLUMN;

/* Usage */
/* ----- */
HTML_COLUMN *html_column_new(
		char *heading,
		boolean right_justify_boolean );

/* Process */
/* ------- */
HTML_COLUMN *html_column_calloc(
		void );

/* Usage */
/* ----- */
LIST *html_column_list(
		LIST *heading_list,
		LIST *justify_list );

/* Process */
/* ------- */
boolean html_column_right_justify_boolean(
		char *justify );

/* Usage */
/* ----- */
LIST *html_column_right_list(
		LIST *heading_list );

/* Usage */
/* ----- */
void html_column_list_output(
		LIST *column_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *html_column_tag(
		char *heading );

typedef struct
{
	char *datum;
	boolean large_boolean;
	boolean bold_boolean;
} HTML_CELL;

/* Usage */
/* ----- */
HTML_CELL *html_cell_new(
		char *datum,
		boolean large_boolean,
		boolean bold_boolean );

/* Process */
/* ------- */
HTML_CELL *html_cell_calloc(
		void );

/* Usage */
/* ----- */
LIST *html_cell_list(
		LIST *data_list );

/* Usage */
/* ----- */
void html_cell_output(
		HTML_COLUMN *html_column,
		HTML_CELL *html_cell,
		boolean background_shaded_boolean );
			
/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *html_cell_datum_tag(
		char *html_cell_datum,
		boolean large_boolean,
		boolean bold_boolean,
		boolean right_justify_boolean,
		boolean background_shaded_boolean );

typedef struct
{
	LIST *cell_list;

	/* Set externally */
	/* -------------- */
	boolean background_shaded_boolean;
} HTML_ROW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HTML_ROW *html_row_new(
		LIST *cell_list );

/* Process */
/* ------- */
HTML_ROW *html_row_calloc(
		void );

/* Usage */
/* ----- */
void html_row_output(
		LIST *column_list,
		HTML_ROW *html_row );

typedef struct
{
	char *title;
	char *sub_title;
	char *sub_sub_title;

	/* Set externally */
	/* -------------- */
	LIST *justify_list;
	LIST *column_list;
	LIST *row_list;
} HTML_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HTML_TABLE *html_table_new(
		char *title,
		char *sub_title,
		char *sub_sub_title );

/* Process */
/* ------- */
HTML_TABLE *html_table_calloc(
		void );

/* Driver */
/* ------ */
void html_table_output(
		HTML_TABLE *html_table,
		int html_table_rows_between_heading );

/* Process */
/* ------- */
void html_table_title_output(
		char *title,
		char *sub_title,
		char *sub_sub_title );

/* Returns program memory */
/* ---------------------- */
char *html_table_close_tag(
		void );

/* Driver */
/* ------ */
void html_table_list_output(
		LIST *table_list,
		int html_table_rows_between_heading );

/* Driver */
/* ------ */
void html_string_list_stdout(
		int html_table_queue_top_bottom,
		char *title,
		char *heading_string,
		LIST *string_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *html_table_system_string(
		int html_table_queue_top_bottom,
		char delimiter,
		char *title,
		char *heading_string );

#endif
