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
	char *label;
	char *escape_street_address;
	char *city_state_zip;
	char *string;
} INVOICE_LATEX_ENTITY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVOICE_LATEX_ENTITY *invoice_latex_entity_new(
		char *label,
		ENTITY *entity );

/* Process */
/* ------- */
INVOICE_LATEX_ENTITY *invoice_latex_entity_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_entity_label(
		char *label );

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
char *invoice_latex_entity_string(
		char *entity_full_name,
		char *invoice_latex_entity_label,
		char *escape_street_address,
		char *invoice_latex_entity_city_state_zip );

typedef struct
{
	INVOICE_LATEX_ENTITY *self_invoice_latex_entity;
	INVOICE_LATEX_ENTITY *customer_invoice_latex_entity;
	LATEX_TABLE *latex_table;
	INVOICE_LATEX_SUMMARY *invoice_latex_summary;
	char *display;
} INVOICE_LATEX_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVOICE_LATEX_TABLE *invoice_latex_table_new(
		char *invoice_caption,
		ENTITY_SELF *entity_self,
		CUSTOMER *customer,
		INVOICE_DATA *invoice_data );

/* Process */
/* ------- */
INVOICE_LATEX_TABLE *invoice_latex_table_calloc(
		void );

/* Usage */
/* ----- */
LIST *invoice_latex_table_column_list(
		INVOICE_DATA *invoice_data );

/* Usage */
/* ----- */
LIST *invoice_latex_table_row_list(
		INVOICE_DATA *invoice_data,
		LIST *latex_table_column_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *invoice_latex_table_display(
		char *latex_table_title,
		INVOICE_LATEX_ENTITY *
			self_invoice_latex_entity,
		INVOICE_LATEX_ENTITY *
			customer_invoice_latex_entity,
		LIST *latex_table_row_list /* freed */,
		INVOICE_LATEX_SUMMARY *invoice_latex_summary );

typedef struct
{
	INVOICE *invoice;
	char *document_header;
	STATEMENT_LINK *statement_link;
	LATEX *latex;
	INVOICE_LATEX_TABLE *invoice_latex_table;
	char *statement_caption_logo_filename;
	char *document_header;
	char *title_string;
	char *self_string;
	char *customer_string;
	char *extra_string;
} INVOICE_LATEX;

/* Usage */
/* ----- */
INVOICE_LATEX *invoice_latex_new(
		char *application_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *statement_logo_filename,
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
char *invoice_latex_title_string(
		char *invoice_caption,
		char *statement_caption_logo_filename,
		char *invoice_date_string );

/* Usage */
/* ----- */
char *invoice_latex_self_string(
		ENTITY_SELF *entity_self );

void latex_invoice_output_footer(
			FILE *output_stream,
			boolean with_customer_signature );

char *invoice_latex_header(
		char *invoice_key,
		char *invoice_date,
		char *line_item_key_heading,
		INVOICE_LATEX_ENTITY *
			self_invoice_latex_entity,
	       	INVOICE_LATEX_ENTITY *
			customer_invoice_latex_entity,
		char *customer_service_key,
		boolean discount_amount_boolean,
		char *title_string,
		boolean omit_money_boolean,
		char *logo_filename,
		char *instructions,
		LIST *extra_label_list,
		char *first_column_label,
		char *last_column_label,
		char *customer_label );

char *invoice_latex_line_item_list_string(
		LIST *invoice_line_item_list,
		boolean _discount_amount_boolean,
		boolean omit_money_boolean,
		int quantity_decimal_places );

char *invoice_latex_invoice_footer(
		double extended_price_total,
		double sales_tax,
		double shipping_charge,
		double total_payment,
		double amount_due,
		char *line_item_key_heading,
		boolean discount_amount_boolean,
		boolean estimate_boolean );

boolean latex_invoice_discount_amount_boolean(
		LIST *invoice_line_item_list );

boolean latex_invoice_each_quantity_integer_boolean(
		LIST *invoice_line_item_list );

boolean latex_invoice_extended_price_boolean(
		LIST *invoice_line_item_list );

int latex_invoice_quantity_decimal_places(
		boolean each_quantity_integer_boolean,
		int default_quantity_decimal_places );

/* Returns heap memory */
/* ------------------- */
char *latex_invoice_header_text_line(
		boolean discount_amount_boolean,
		boolean omit_money_boolean,
		char *first_column_label,
		char *last_column_label );

/* Returns heap memory */
/* ------------------- */
char *latex_invoice_header_format_line(
		boolean discount_amount_boolean );

/* Driver */
/* ------ */
void invoice_latex_output(
		INVOICE_LATEX *invoice_latex );

typedef struct
{
	/* Stub */
} INVOICE_LATEX_SCHOOL;

char *invoice_latex_school_header(
		char *invoice_date,
		INVOICE_LATEX_ENTITY *self_invoice_latex_entity;
		INVOICE_LATEX_ENTITY *customer_invoice_latex_entity;
		char *title,
		char *logo_filename );

char *invoice_latex_school_header_format_line(
		void );

char *invoice_latex_school_header_text_line(
		char *first_column_label,
		char *last_column_label );

char *invoice_latex_school_line_item_list(
		LIST *invoice_line_item_list );

char *invoice_latex_school_footer(
		double extended_price_total,
		double total_payment,
		double amount_due );

#endif

