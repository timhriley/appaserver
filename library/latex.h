/* library/latex.h					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef LATEX_H
#define LATEX_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */
#define WITH_CAPTION	1
#define LATEX_PBJ	"PB\\&J"

/* Structures */
/* ---------- */
typedef struct
{
	char *heading;
	boolean right_justified_flag;
	char *paragraph_size;
} LATEX_TABLE_HEADING;

typedef struct
{
	char *column_data;
	boolean large_bold;
} LATEX_COLUMN_DATA;

typedef struct
{
	LIST *column_data_list;
	boolean preceed_double_line;
	boolean large_bold;
} LATEX_ROW;

typedef struct
{
	char *caption;
	LIST *heading_list;
	LIST *row_list;
} LATEX_TABLE;

typedef struct
{
	FILE *output_stream;
	char *tex_filename;
	char *dvi_filename;
	char *working_directory;
	boolean landscape_flag;
	LIST *table_list;
	char *logo_filename;
} LATEX;

/* Prototypes */
/* ---------- */
LATEX_COLUMN_DATA *latex_column_data_new(
					char *column_data,
					boolean large_bold );

void latex_tex2pdf(			char *tex_filename,
					char *working_directory );

LATEX *latex_new(	char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean landscape_flag,
			char *logo_filename );

LATEX *latex_new_latex(	char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean landscape_flag,
			char *logo_filename );

LATEX_TABLE *latex_new_latex_table(
			char *caption );

LATEX_TABLE *latex_table_new(
			char *caption );

LATEX_ROW *latex_new_latex_row(
			void );

LATEX_ROW *latex_row_new(
			void );

LATEX_TABLE_HEADING *latex_table_heading_new(
			void );

LATEX_TABLE_HEADING *latex_new_latex_table_heading(
			void );

void latex_output_longtable_document_heading(
			FILE *output_stream,
			boolean landscape_flag,
			boolean table_package_flag,
			char *logo_filename,
			boolean omit_page_numbers,
			char *footline );

void latex_output_longtable_heading(
			FILE *output_stream,
			char *caption,
			LIST *heading_list );

void latex_output_table_row_list(
			FILE *output_stream,
			LIST *row_list,
			int heading_list_length );

void latex_output_document_footer(
			FILE *output_stream );

void latex_output_longtable_footer(
			FILE *output_stream );

void latex_output_table_footer(
			FILE *output_stream );

void latex_longtable_output(
			FILE *output_stream,
			boolean landscape_flag,
			LIST *table_list,
			char *logo_filename,
			boolean omit_page_numbers,
			char *footer );

void latex_output_documentclass_letter(
			FILE *output_stream,
			boolean omit_page_numbers );

void latex_output_letterhead_document_heading(
			FILE *output_stream,
			char *logo_filename,
			char *organization_name,
			char *street_address,
			char *city_state_zip,
			char *date_string );

void latex_output_return_envelope_document_heading(
			FILE *output_stream,
			char *logo_filename,
			char *organization_name,
			char *street_address,
			char *city_state_zip,
			char *date_string );

void latex_output_indented_address(
			FILE *output_stream,
			char *full_name,
			char *street_address,
			char *city_state_zip );

void latex_output_table_heading(
			FILE *output_stream,
			char *caption,
			LIST *heading_list );

void latex_output_table_vertical_padding(
			FILE *output_stream,
			int length_heading_list,
			int empty_vertical_rows );

char *latex_escape_data(char *destination,
			char *source,
			int buffer_size );

void latex_append_column_data_list(
			LIST *column_data_list,
			char *column_data,
			boolean large_bold );

LIST *latex_table_right_heading_list(
			LIST *heading_list );

#endif
