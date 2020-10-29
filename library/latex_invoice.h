/* library/latex_invoice.h						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver latex_invoice ADT.				*/
/*									*/
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
	char *item;
	double quantity;
	double retail_price;
	double discount_amount;
} LATEX_INVOICE_LINE_ITEM;

typedef struct
{
	char *invoice_key;
	char *name;
	char *street_address;
	char *suite_number;
	char *city;
	char *state;
	char *zip_code;
	char *customer_service_key;
	LIST *invoice_line_item_list;
	double sales_tax;
	double shipping_charge;
	double total_payment;
	double extension_total;
	boolean exists_discount_amount;
} LATEX_INVOICE_CUSTOMER;

typedef struct
{
	char *name;
	char *street_address;
	char *suite_number;
	char *city;
	char *state;
	char *zip_code;
	char *phone_number;
	char *email_address;
} LATEX_INVOICE_COMPANY;

typedef struct
{
	char *invoice_date;
	char *line_item_key_heading;
	LATEX_INVOICE_COMPANY invoice_company;
	LATEX_INVOICE_CUSTOMER *invoice_customer;
	boolean omit_money;
	int quantity_decimal_places;
	char *instructions;
	LIST *extra_label_list;
} LATEX_INVOICE;

/* Operations */
/* ---------- */
LATEX_INVOICE *latex_invoice_new(
			char *invoice_date,
			char *company_name,
			char *company_street_address,
			char *company_suite_number,
			char *company_city,
			char *company_state,
			char *company_zip_code,
			char *company_phone_number,
			char *company_email_address,
			char *line_item_key_heading,
			char *instructions,
			LIST *extra_label_list );

LATEX_INVOICE_CUSTOMER *latex_invoice_customer_new(
			char *invoice_key,
			char *customer_name,
			char *customer_street_address,
			char *customer_suite_number,
			char *customer_city,
			char *customer_state,
			char *customer_zip_code,
			char *customer_service_key,
			double sales_tax,
			double shipping_charge,
			double total_payment );

void latex_invoice_customer_free(
			LATEX_INVOICE_CUSTOMER *
				latex_invoice_customer );

/* Returns (quantity * retail_price ) - discount_amount */
/* ---------------------------------------------------- */
double latex_invoice_append_line_item(
			LIST *invoice_line_item_list,
			char *item_key,
			char *item,
			double quantity,
			double retail_price,
			double discount_amount );

LATEX_INVOICE_LINE_ITEM *latex_invoice_line_item_new(
			char *item_key,
			char *item,
			double quantity,
			double retail_price,
			double discount_amount );

void latex_invoice_line_item_free(
			LATEX_INVOICE_LINE_ITEM *
				latex_invoice_line_item );

void latex_invoice_output_header(
			FILE *output_stream );

void latex_invoice_output_footer(
			FILE *output_stream,
			boolean with_customer_signature );

void latex_invoice_output_invoice_header(FILE *output_stream,
			char *invoice_date,
			char *line_item_key_heading,
			LATEX_INVOICE_COMPANY *
				latex_invoice_company,
	       		LATEX_INVOICE_CUSTOMER *
				latex_invoice_customer,
			boolean exists_discount_amount,
			char *title,
			boolean omit_money,
			char *logo_filename,
			char *instructions,
			LIST *extra_label_list );

void latex_invoice_output_invoice_line_items(
			FILE *output_stream,
			LIST *invoice_line_item_list,
			boolean exists_discount_amount,
			boolean omit_money,
			int quantity_decimal_places );

void latex_invoice_output_invoice_footer(
			FILE *output_stream,
			double extension_total,
			double sales_tax,
			double shipping_charge,
			double total_payment,
			char *line_item_key_heading,
			boolean exists_discount_amount,
			boolean is_estimate );

void latex_invoice_customer_free(
			LATEX_INVOICE_CUSTOMER *
				invoice_customer );

void latex_invoice_company_free(
			LATEX_INVOICE_COMPANY *
				invoice_company );

void latex_invoice_free(
			LATEX_INVOICE *invoice );

boolean latex_invoice_get_exists_discount_amount(
			LIST *invoice_line_item_list );

boolean latex_invoice_each_quantity_integer(
			LIST *invoice_line_item_list );

#endif

