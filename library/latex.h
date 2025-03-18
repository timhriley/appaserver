/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/latex.h					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef LATEX_H
#define LATEX_H

#include "boolean.h"
#include "list.h"

#define LATEX_PBJ	"PB\\&J"

enum latex_column_enum {
	latex_column_text,
	latex_column_integer,
	latex_column_float };

typedef struct
{
	char *heading_string;
	enum latex_column_enum latex_column_enum;
	int float_decimal_count;
	char *paragraph_size;
	boolean first_column_boolean;
	boolean right_justify_boolean;
	char *display;

	/* Set externally */
	/* -------------- */
	boolean dollar_sign_boolean;
} LATEX_COLUMN;

/* Usage */
/* ----- */
LATEX_COLUMN *latex_column_new(
		char *heading_string,
		enum latex_column_enum latex_column_enum,
		int float_decimal_count,
		char *paragraph_size,
		boolean first_column_boolean );

/* Process */
/* ------- */
LATEX_COLUMN *latex_column_calloc(
		void );

/* Usage */
/* ----- */
boolean latex_column_right_justify_boolean(
		enum latex_column_enum latex_column_enum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *latex_column_display(
		char *heading_string );

/* Usage */
/* ----- */
LIST *latex_column_float_list(
		LIST *heading_string_list,
		boolean first_column_boolean,
		boolean dollar_sign_boolean );

/* Usage */
/* ----- */
LIST *latex_column_text_list(
		LIST *heading_string_list,
		boolean first_column_boolean,
		boolean right_justify_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *latex_column_header_text_line(
		LIST *latex_column_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *latex_column_header_format_line(
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_COLUMN *latex_column_seek(
		LIST *latex_column_list,
		char *heading_string );

typedef struct
{
	char *display;
} LATEX_CELL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LATEX_CELL *latex_cell_float_new(
		LATEX_COLUMN *latex_column,
		double value );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LATEX_CELL *latex_cell_small_new(
		LATEX_COLUMN *latex_column,
		char *datum );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LATEX_CELL *latex_cell_new(
		LATEX_COLUMN *latex_column,
		boolean first_row_boolean,
		char *datum,
		boolean large_boolean,
		boolean bold_boolean );

/* Process */
/* ------- */
LATEX_CELL *latex_cell_calloc(
		void );

boolean latex_cell_dollar_sign_boolean(
		boolean dollar_sign_boolean,
		boolean first_row_boolean );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *latex_cell_column_delimiter(
		boolean list_first_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *latex_cell_markup(
		boolean large_boolean,
		boolean bold_boolean );

/* Usage */
/* ----- */

/* Returns null, static memory, or datum */
/* ------------------------------------- */
char *latex_cell_formatted_datum(
		char *datum,
		enum latex_column_enum latex_column_enum,
		int float_decimal_count,
		boolean dollar_sign_boolean );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *latex_cell_dollar_sign(
		boolean dollar_sign_boolean );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *latex_cell_display(
		char *latex_cell_column_delimiter,
		char *latex_cell_markup,
		char *latex_cell_formatted_datum );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *latex_cell_escape(
		char *datum );

/* Usage */
/* ----- */
LIST *latex_cell_list(
		const char *calling_function,
		LIST *latex_column_list,
		LIST *data_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *latex_cell_list_display(
		LIST *latex_cell_list );

/* Usage */
/* ----- */
void latex_cell_list_free(
		LIST *latex_cell_list );

typedef struct
{
	LIST *cell_list;

	/* Set externally */
	/* -------------- */
	boolean preceed_double_line_boolean;
} LATEX_ROW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LATEX_ROW *latex_row_new(
		LIST *cell_list );

/* Process */
/* ------- */
LATEX_ROW *latex_row_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *latex_row_list_display(
		LIST *latex_row_list /* freed */ );

/* Driver */
/* ------ */
void latex_row_list_output(
		LIST *latex_row_list /* freed */,
		FILE *latex_output_file );

typedef struct
{
	char *directory_filename;
	char *documentclass;
	char *usepackage;
	char *footer_declaration;
	char *begin_document;
	char *logo_display;
	char *end_document;
} LATEX;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LATEX *latex_new(
		char *tex_filename,
		char *working_directory,
		boolean landscape_boolean,
		char *logo_filename );

/* Process */
/* ------- */
LATEX *latex_calloc(
		void );

/* Returns static memory */
/* --------------------- */ 
char *latex_directory_filename(
		char *tex_filename,
		char *working_directory );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *latex_documentclass(
		boolean landscape_boolean );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *latex_usepackage(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *latex_footer_declaration(
		char *date_now16,
		char *latex_pbj );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *latex_begin_document(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *latex_logo_display(
		char *logo_filename );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *latex_end_document(
		void );

/* Usage */
/* ------ */
void latex_tex2pdf(
		char *tex_filename,
		char *working_directory );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *latex_omit_page_numbers(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *latex_output_file(
		char *directory_filename );

/* Usage */
/* ----- */
void latex_output_document_head(
		LATEX *latex,
		FILE *latex_output_file );

/* Usage */
/* ----- */
void latex_output_document_tail(
		char *latex_table_end_document,
		FILE *latex_output_file );

typedef struct
{
	LIST *latex_column_list;
	LIST *latex_row_list;
	char *logo_display;
	char *begin_longtable;
	char *latex_column_header_format_line;
	char *caption_display;
	char *latex_column_header_text_line;
	char *footer_display;
	char *end_longtable;
} LATEX_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LATEX_TABLE *latex_table_new(
		char *title,
		LIST *latex_column_list,
		LIST *latex_row_list );

/* Process */
/* ------- */
LATEX_TABLE *latex_table_calloc(
		void );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *latex_table_begin_longtable(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *latex_table_caption_display(
		char *title );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *latex_table_footer_display(
		void );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *latex_table_end_longtable(
		void );

/* Usage */
/* ----- */
void latex_table_output_head(
		LATEX_TABLE *latex_table,
		FILE *latex_output_file );

/* Usage */
/* ----- */
void latex_table_output_row_list(
		LIST *latex_table_row_list /* freed */,
		FILE *latex_output_file );

/* Usage */
/* ----- */
void latex_table_output_tail(
		char *latex_table_end_longtable,
		FILE *latex_output_file );

/* Driver */
/* ------ */
void latex_table_output(
		char *tex_filename,
		char *tex_anchor_html,
		char *pdf_anchor_html,
		char *working_directory,
		LATEX *latex,
		LATEX_TABLE *latex_table );

/* Driver */
/* ------ */
void latex_table_list_output(
		char *tex_filename,
		char *tex_anchor_html,
		char *pdf_anchor_html,
		char *working_directory,
		LATEX *latex,
		LIST *latex_table_list );

#endif
