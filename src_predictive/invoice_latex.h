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
#include "statement_link.h"
#include "entity_self.h"
#include "customer.h"

typedef struct
{
	LATEX_TABLE *latex_table;
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

typedef struct
{
	INVOICE *invoice;
	STATEMENT_LINK *statement_link;
	LATEX *latex;
	INVOICE_LATEX_TABLE *invoice_latex_table;
	char *statement_caption_logo_filename;
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
void latex_invoice_output_document_header(
		FILE *appaserver_output_file );

/* Usage */
/* ----- */
void latex_invoice_output_self(
		ENTITY_SELF *entity_self,
		FILE *appaserver_output_file );

/* Driver */
/* ------ */
void invoice_latex_output(
		INVOICE_LATEX *invoice_latex );

void latex_invoice_output_footer(
			FILE *output_stream,
			boolean with_customer_signature );

void latex_invoice_output_invoice_header(
			FILE *output_stream,
			char *invoice_key,
			char *invoice_date,
			char *line_item_key_heading,
			LATEX_INVOICE_SELF *
				latex_invoice_self,
	       		LATEX_INVOICE_CUSTOMER *
				latex_invoice_customer,
			char *customer_service_key,
			boolean exists_discount_amount,
			char *title,
			boolean omit_money,
			char *logo_filename,
			char *instructions,
			LIST *extra_label_list,
			char *first_column_label,
			char *last_column_label,
			char *customer_label );

void latex_invoice_output_line_item_list(
			FILE *output_stream,
			LIST *invoice_line_item_list,
			boolean exists_discount_amount,
			boolean omit_money,
			int quantity_decimal_places );

void latex_invoice_output_invoice_footer(
			FILE *output_stream,
			double extended_price_total,
			double sales_tax,
			double shipping_charge,
			double total_payment,
			char *line_item_key_heading,
			boolean exists_discount_amount,
			boolean is_estimate );

boolean latex_invoice_exists_discount_amount(
			LIST *invoice_line_item_list );

boolean latex_invoice_each_quantity_integer(
			LIST *invoice_line_item_list );

boolean latex_invoice_exists_extended_price(
			LIST *invoice_line_item_list );

int latex_invoice_quantity_decimal_places(
			boolean each_quantity_integer,
			int default_quantity_decimal_places );

/* Returns heap memory */
/* ------------------- */
char *latex_invoice_header_text_line(
			boolean exists_discount_amount,
			boolean omit_money,
			char *first_column_label,
			char *last_column_label );

/* Returns heap memory */
/* ------------------- */
char *latex_invoice_header_format_line(
			boolean exists_discount_amount );

void latex_invoice_education_invoice_header(
			FILE *output_stream,
			char *invoice_date,
			LATEX_INVOICE_SELF *invoice_self,
			LATEX_INVOICE_CUSTOMER *invoice_customer,
			char *title,
			char *logo_filename );

char *latex_invoice_header_education_format_line(
			void );


char *latex_invoice_header_education_text_line(
			char *first_column_label,
			char *last_column_label );

void latex_invoice_education_line_item_list(
			FILE *output_stream,
			LIST *invoice_line_item_list );

void latex_invoice_education_invoice_footer(
			FILE *output_stream,
			double extended_price_total,
			double total_payment );

#endif

