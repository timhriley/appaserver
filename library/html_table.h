/* $APPASERVER_HOME/library/html_table.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef HTML_TABLE_H
#define HTML_TABLE_H

#include "list.h"

typedef struct
{
	char *column_data;
	boolean large_bold;
} HTML_COLUMN_DATA;

/* Usage */
/* ----- */
HTML_COLUMN_DATA *html_column_data_new(
			char *column_data,
			boolean large_bold );

/* Process */
/* ------- */
HTML_COLUMN_DATA *html_column_data_calloc(
			void );

/* Public */
/* ------ */
void html_column_data_set(
			LIST *column_data_list,
			char *data,
			boolean large_bold );

typedef struct
{
	LIST *column_data_list;
} HTML_ROW;

/* Usage */
/* ----- */
HTML_ROW *html_row_new(	void );

/* Process */
/* ------- */
HTML_ROW *html_row_calloc(
			void );

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

HTML_TABLE *new_html_table(
			char *title,
			char *sub_title );

/* Process */
/* ------- */
HTML_TABLE *html_table_calloc(
			void );

void html_table_output( 		HTML_TABLE *d );
void html_table_set_data_heading( 	HTML_TABLE *d, 
					char *heading );
void html_table_set_data( 		LIST *data_list, char *data );
void html_table_set_record_data(	HTML_TABLE *d,
					char *record_data,
					char delimiter );
void html_table_close( 			void );
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

void html_table_heading(
			char *title,
			char *sub_title,
			char *sub_sub_title );

void html_table_output_data_heading( 	LIST *heading_list,
					int number_left_justified_columns,
					int number_right_justified_columns,
					LIST *justify_list );

void html_table_output_data( 		LIST *data_list,
					int number_left_justified_columns,
					int number_right_justified_columns,
					int background_shaded,
					LIST *justify_list );

#endif
