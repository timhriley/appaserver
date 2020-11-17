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
	char *invoice_date;
	char *line_item_key_heading;
	LATEX_INVOICE_SELF *latex_invoice_self;
	LATEX_INVOICE_CUSTOMER *latex_invoice_customer;
	char *customer_service_key;
	LIST *invoice_line_item_list;
	double sales_tax;
	double shipping_charge;
	double total_payment;
	double extended_price_total;
	boolean exists_discount_amount;
	boolean omit_money;
	int quantity_decimal_places;
	char *instructions;
	/* LIST *extra_label_list; */
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
			char *self_email_address,
			char *line_item_key_heading,
			char *instructions );

LATEX_INVOICE_CUSTOMER *latex_invoice_customer_new(
			char *customer_full_name,
			char *customer_street_address,
			char *customer_city,
			char *customer_state,
			char *customer_zip_code,
			char *customer_service_key );

void latex_invoice_line_item_list_free(
			LIST  *line_item_list )

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
			LATEX_INVOICE_SELF *
				latex_invoice_self,
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
			double extended_price_total,
			double sales_tax,
			double shipping_charge,
			double total_payment,
			char *line_item_key_heading,
			boolean exists_discount_amount,
			boolean is_estimate );

boolean latex_invoice_get_exists_discount_amount(
			LIST *invoice_line_item_list );

boolean latex_invoice_each_quantity_integer(
			LIST *invoice_line_item_list );

#endif

