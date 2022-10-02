/* $APPASERVER_HOME/library/html_table.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef HTML_TABLE_H
#define HTML_TABLE_H

#include "list.h"
#include "boolean.h"

#define HTML_TABLE_ROWS_BETWEEN_HEADING	10

typedef struct
{
	char *heading;
	boolean right_justify_boolean;
} HTML_HEADING;

/* Usage */
/* ----- */
HTML_HEADING *html_heading_new(
			char *heading,
			boolean right_justify_boolean );

/* Process */
/* ------- */
HTML_HEADING *html_heading_calloc(
			void );

/* Usage */
/* ----- */
void html_heading_list_right_justify_set(
			LIST *heading_list,
			LIST *label_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void html_heading_list_output(
			LIST *heading_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *html_heading_tag(	char *heading );

typedef struct
{
	char *data;
	boolean large_boolean;
	boolean bold_boolean;
} HTML_CELL;

/* Usage */
/* ----- */
HTML_CELL *html_cell_new(
			char *data,
			boolean large_boolean,
			boolean bold_boolean );

/* Process */
/* ------- */
HTML_CELL *html_cell_calloc(
			void );

/* Usage */
/* ----- */
void html_cell_data_set(
			LIST *cell_list,
			char *data,
			boolean large_boolean,
			boolean bold_boolean );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void html_cell_data_list_set(
			LIST *cell_list,
			LIST *data_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void html_cell_output(
			HTML_HEADING *html_heading,
			HTML_CELL *html_cell );
			
/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *html_cell_tag(	char *html_cell_data,
			boolean large_boolean,
			boolean bold_boolean,
			boolean right_justify_boolean );

typedef struct
{
	LIST *cell_list;
} HTML_ROW;

/* Usage */
/* ----- */
HTML_ROW *html_row_new(	void );

/* Process */
/* ------- */
HTML_ROW *html_row_calloc(
			void );

/* Usage */
/* ----- */
void html_row_output(
			LIST *heading_list,
			HTML_ROW *html_row );

/* Process */
/* ------- */

typedef struct
{
	char *title;
	char *sub_title;
	char *sub_sub_title;
	LIST *heading_list;
	LIST *row_list;
	LIST *data_list;
	LIST *justify_list;
	int number_left_justified_columns;
	int number_right_justified_columns;
	int background_shaded;
} HTML_TABLE;

/* Usage */
/* ----- */
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
void html_table_output(	HTML_TABLE *html_table,
			int html_table_rows_between_heading );

/* Process */
/* ------- */
void html_table_heading(
			char *title,
			char *sub_title,
			char *sub_sub_title );

void html_table_close(	void );

/* Driver */
/* ------ */
void html_table_list_output(
			LIST *table_list,
			int html_table_rows_between_heading );

/* Legacy */
/* ------ */
HTML_TABLE *new_html_table(
			char *title,
			char *sub_title );

void html_table_output_data(
			LIST *data_list,
			int number_left_justified_columns,
			int number_right_justified_columns,
			int background_shaded,
			LIST *justify_list );

void html_table_output_data_heading(
			LIST *heading_list,
			int number_left_justified_columns,
			int number_right_justified_columns,
			LIST *justify_list );

void html_table_set_data_heading( 	HTML_TABLE *d, 
					char *heading );
void html_table_set_data( 		LIST *data_list, char *data );
void html_table_set_record_data(	HTML_TABLE *d,
					char *record_data,
					char delimiter );
void html_table_set_number_left_justified_columns(
					HTML_TABLE *d, int i );
void html_table_set_number_right_justified_columns(
					HTML_TABLE *d, int i );
void html_table_reset_data_heading( 	HTML_TABLE *d );

void html_table_set_heading( 		HTML_TABLE *d,
					char *heading );

void html_table_set_heading_list( 	HTML_TABLE *d,
					LIST *heading_list );

void html_table_set_background_shaded(	HTML_TABLE *d );
void html_table_set_background_unshaded(HTML_TABLE *d );
char *html_table_get_alignment(		int column_number,
					int number_left_justified_columns,
					int number_right_justified_columns );

void html_table_output_table_heading(
			char *title,
			char *sub_title );

void html_table_output_table_heading2(
			char *title,
			char *sub_title,
			char *sub_sub_title );

#endif
