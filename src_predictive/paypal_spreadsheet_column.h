/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_spreadsheet_column.h		*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software. See Appaserver.org	*/
/* -------------------------------------------------------------	*/

#ifndef PAYPAL_SPREADSHEET_COLUMN_H
#define PAYPAL_SPREADSHEET_COLUMN_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *heading_string;
	int paypal_spreadsheet_column_piece;
} PAYPAL_SPREADSHEET_COLUMN;

/* Usage */
/* ----- */
LIST *paypal_spreadsheet_column_list(
		char *spreadsheet_filename,
		char *date_label );

/* Usage */
/* ----- */
PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column_new(
		char *heading_string,
		int piece_offset );

/* Process */
/* ------- */
PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column_calloc(
		void );

/* Usage */
/* ----- */
int paypal_spreadsheet_column_piece(
		char *heading_line,
		char *heading_string );

/* Usage */
/* ----- */
PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column_heading_seek(
		LIST *paypal_spreadsheet_column_list,
		char *heading_string );

/* Usage */
/* ----- */
char *paypal_spreadsheet_column_header_row_string(
		char *spreadsheet_filename,
		char *date_label );

/* Usage */
/* ----- */
boolean paypal_spreadsheet_column_header_boolean(
		char *date_label,
		char *header_buffer );

/* Usage */
/* ----- */
char *paypal_spreadsheet_column_heading_data(
		LIST *paypal_spreadsheet_column_list,
		char *input_row,
		char *heading_string );

/* Usage */
/* ----- */
char *paypal_spreadsheet_column_cell_extract(
		char *input_row,
		int paypal_spreadsheet_column_piece );

/* Usage */
/* ----- */
int paypal_spreadsheet_column_date_piece(
		char *spreadsheet_filename,
		char *date_label );

#endif
