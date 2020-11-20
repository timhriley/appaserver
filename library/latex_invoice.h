/* $APPASERVER_HOME/library/latex_invoice.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef LATEX_INVOICE_H
#define LATEX_INVOICE_H

#include "list.h"

/* Constants */
/* --------- */
#define LATEX_EXTENSION( quantity, retail_price, discount_amount )	\
			( quantity * retail_price - discount_amount )

#define LATEX_INVOICE_QUANTITY_DECIMAL_PLACES	4

/* Structures */
/* ---------- */
typedef struct
{
	char *item_key;
	char *item_name;
	double quantity;
	double retail_price;
	double discount_amount;
	double extended_price;
} LATEX_INVOICE_LINE_ITEM;

typedef struct
{
	char *full_name;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *phone_number;
	char *email_address;
} LATEX_INVOICE_CUSTOMER;

typedef struct
{
	char *full_name;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *phone_number;
	char *email_address;
} LATEX_INVOICE_SELF;

typedef struct
{
	/* Input */
	/* ----- */
	char *invoice_key;
	char *invoice_date;
	char *line_item_key_heading;
	LATEX_INVOICE_SELF *invoice_self;
	LATEX_INVOICE_CUSTOMER *invoice_customer;
	char *customer_service_key;
	double sales_tax;
	double shipping_charge;
	double total_payment;
	double extended_price_total;
	boolean omit_money;
	char *instructions;
	boolean is_estimate;
	LIST *extra_label_list;

	/* Process */
	/* ------- */
	boolean exists_discount_amount;
	boolean each_quantity_integer;
	boolean exists_extended_price;
	boolean quantity_decimal_places;
	LIST *invoice_line_item_list;
} LATEX_INVOICE;

/* Operations */
/* ---------- */
LATEX_INVOICE *latex_invoice_new(
			char *invoice_date,
			char *self_full_name,
			char *self_street_address,
			char *self_city,
			char *self_state_code,
			char *self_zip_code,
			char *self_phone_number,
			char *self_email_address );

LATEX_INVOICE_CUSTOMER *latex_invoice_customer_new(
			char *customer_full_name,
			char *customer_street_address,
			char *customer_city,
			char *customer_state_code,
			char *customer_zip_code,
			char *customer_phone_number,
			char *customer_email_address );

LATEX_INVOICE_SELF *
	latex_invoice_self_new(
			char *full_name,
			char *street_address,
			char *city,
			char *state_code,
			char *zip_code,
			char *phone_number,
			char *email_address );

LIST *latex_invoice_line_item_set(
			LIST *invoice_line_item_list,
			char *item_key,
			char *item_name,
			double quantity,
			double retail_price,
			double discount_amount,
			double extended_price );

LATEX_INVOICE_LINE_ITEM *latex_invoice_line_item_new(
			char *item_key,
			char *item_name,
			double quantity,
			double retail_price,
			double discount_amount,
			double extended_price );

void latex_invoice_output_header(
			FILE *output_stream );

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

