/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/table.h			   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef TABLE_H
#define TABLE_H

#include "boolean.h"
#include "list.h"

enum table_column_type {
		column_string,
		column_double };

typedef struct
{
	char *table_title;
	char *latex_caption;
	char *latex_filename_tex;
	char *latex_filename_dvi;
	char *latex_working_directory;
	boolean latex_landscape_flag;
	char *latex_logo_filename;
	boolean latex_omit_page_numbers;
	char *latex_footline;
	LIST *table_column_list;
	LIST *table_row_list;
} TABLE;

/* Usage */
/* ----- */
TABLE *table_new(
		char *table_title );

typedef struct
{
	char *column_name;
	boolean right_justified;
	char *latex_paragraph_size;
	boolean latex_large_bold;
	int double_output_width;
	int double_output_decimal_places;
	enum table_column_type table_column_type;
} TABLE_COLUMN;

/* Usage */
/* ----- */
TABLE_COLUMN *table_column_new(
		char *column_name,
		enum table_column_type );

/* Process */
/* ------- */
TABLE_COLUMN *table_column_set(
		LIST *table_column_list,
		char *column_name,
		enum table_column_type );

typedef struct
{
	char *data_string;
	double data_double;
	TABLE_COLUMN *table_column;
} TABLE_CELL;

/* Usage */
/* ----- */
TABLE_CELL *table_cell_string_new(
		char *data_string,
		TABLE_COLUMN *table_column );

/* Process */
/* ------- */
TABLE_CELL *table_cell_calloc(
		void );

TABLE_CELL *table_cell_double_new(
		double data_double,
		TABLE_COLUMN *table_column );

TABLE_CELL *table_cell_string_set(
		LIST *table_row_cell_list,
		char *data_string,
		TABLE_COLUMN *table_column );

TABLE_CELL *table_cell_double_set(
		LIST *table_row_cell_list,
		double data_double,
		TABLE_COLUMN *table_column );

typedef struct
{
	int row_number;
	boolean latex_preceed_double_line;
	boolean latex_large_bold;
	TABLE_CELL *table_cell;
} TABLE_ROW;

/* Usage */
/* ----- */
TABLE_ROW *table_row_new(
		int row_number );

/* Process */
/* ------- */
TABLE_ROW *table_row_set(
		LIST *table_row_list,
		int row_number );


#endif
