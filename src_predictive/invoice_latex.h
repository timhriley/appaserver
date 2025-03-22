/* $APPASERVER_HOME/library/invoice_latex.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INVOICE_LATEX_H
#define INVOICE_LATEX_H

#include "list.h"
#include "boolean.h"
#include "latex.h"
#include "invoice.h"
#include "statement.h"
#include "entity_self.h"
#include "customer.h"

#define INVOICE_LATEX_ESCAPE_ARRAY	"$'#;"

typedef struct
{
	char *extended_display;
	char *discount_display;
	char *amount_display;
	char *payable_display;
	char *due_display;
	char *display;
} INVOICE_LATEX_SUMMARY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVOICE_LATEX_SUMMARY *invoice_latex_summary_new(
		char *amount_due_label,
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		double invoice_line_item_extended_price_total,
		double invoice_line_item_discount_total,
		double invoice_latex_summary_invoice_amount,
		double customer_payable_balance,
		double amount_due );

/* Process */
/* ------- */
INVOICE_LATEX_SUMMARY *invoice_latex_summary_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_summary_extended_display(
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		double extended_price_total );

/* Returns heap memory or null */
/* --------------------------- */
char *invoice_latex_summary_discount_display(
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		double discount_total );

/* Returns heap memory or null */
/* --------------------------- */
char *invoice_latex_summary_amount_display(
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		double invoice_amount );

/* Returns heap memory or null */
/* --------------------------- */
char *invoice_latex_summary_payable_display(
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		double customer_payable_balance );

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_summary_due_display(
		char *amount_due_label,
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		double invoice_summary_amount_due );


/* Returns heap memory */
/* ------------------- */
char *invoice_latex_summary_display(
		char *invoice_latex_summary_extended_display,
		char *invoice_latex_summary_discount_display,
		char *invoice_latex_summary_amount_display,
		char *invoice_latex_summary_payable_display,
		char *invoice_latex_summary_due_display );

/* Usage */
/* ----- */

/* Returns same static memory each time */
/* ------------------------------------ */
char *invoice_latex_summary_empty_display(
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean );

typedef struct
{
	char *heading;
	char *escape_street_address;
	char *city_state_zip;
	char *display;
} INVOICE_LATEX_ENTITY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVOICE_LATEX_ENTITY *invoice_latex_entity_new(
		ENTITY *entity );

/* Process */
/* ------- */
INVOICE_LATEX_ENTITY *invoice_latex_entity_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_entity_heading(
		char *full_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_entity_city_state_zip(
		char *city,
		char *state_code,
		char *zip_code );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_entity_display(
		char *entity_full_name,
		char *invoice_latex_entity_heading,
		char *escape_street_address,
		char *invoice_latex_entity_city_state_zip );

typedef struct
{
	INVOICE_LATEX_ENTITY *self_invoice_latex_entity;
	INVOICE_LATEX_ENTITY *customer_invoice_latex_entity;
	LIST *column_list;
	LIST *row_list;
	LATEX_TABLE *latex_table;
	INVOICE_LATEX_SUMMARY *invoice_latex_summary;
	char *display;
} INVOICE_LATEX_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVOICE_LATEX_TABLE *invoice_latex_table_new(
		enum invoice_enum invoice_enum,
		char *invoice_title,
		ENTITY_SELF *entity_self,
		CUSTOMER *customer,
		LIST *invoice_line_item_list,
		char *amount_due_label,
		INVOICE_SUMMARY *invoice_summary );

/* Process */
/* ------- */
INVOICE_LATEX_TABLE *invoice_latex_table_calloc(
		void );

/* Usage */
/* ----- */
LIST *invoice_latex_table_column_list(
		enum invoice_enum invoice_enum,
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		int invoice_line_item_decimal_count );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *invoice_latex_table_description_size(
		boolean invoice_line_item_discount_boolean );

/* Usage */
/* ----- */
LIST *invoice_latex_table_row_list(
		LIST *invoice_line_item_list,
		LIST *latex_table_column_list );

/* Usage */
/* ----- */
LIST *invoice_latex_table_cell_list(
		char *item_key,
		char *description,
		double quantity,
		double retail_price,
		double discount_amount,
		double extended_price,
		LIST *invoice_latex_table_column_list,
		boolean first_row_boolean );

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_table_display(
		char *latex_table_caption_display,
		char *self_invoice_latex_entity_display,
		char *customer_invoice_latex_entity_display,
		char *latex_table_head_display,
		LIST *latex_table_latex_row_list /* freed */,
		char *latex_table_end_longtable,
		char *invoice_latex_summary_display );

typedef struct
{
	INVOICE *invoice;
	char *document_header;
	STATEMENT_LINK *statement_link;
	char *statement_caption_logo_filename;
	LATEX *latex;
	char *title;
	INVOICE_LATEX_TABLE *invoice_latex_table;
	char *footer;
} INVOICE_LATEX;

/* Usage */
/* ----- */
INVOICE_LATEX *invoice_latex_new(
		char *application_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		INVOICE *invoice );

/* Process */
/* ------- */
INVOICE_LATEX *invoice_latex_calloc(
		void );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *invoice_latex_document_header(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_title(
		char *invoice_caption,
		char *statement_caption_logo_filename,
		char *invoice_date_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_footer(
		enum invoice_enum invoice_enum );

/* Driver */
/* ------ */
void invoice_latex_output(
		INVOICE_LATEX *invoice_latex );

#endif

