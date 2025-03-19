/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/invoice.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INVOICE_H
#define INVOICE_H

#include "list.h"
#include "boolean.h"
#include "entity_self.h"
#include "customer.h"

#define INVOICE_LINE_ITEM_QUANTITY_DECIMAL_PLACES	4

typedef struct
{
	char *item_key;
	char *description;
	double quantity;
	double retail_price;
	double discount_amount;
	double extended_price;
} INVOICE_LINE_ITEM;

/* Usage */
/* ----- */
LIST *invoice_line_item_list(
		char *customer_full_name,
		char *customer_street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
INVOICE_LINE_ITEM *invoice_line_item_parse(
		char *string_input );

/* Usage */
/* ----- */
INVOICE_LINE_ITEM *invoice_line_item_new(
		char *item,
		char *description /* stack memory */,
		double quantity,
		double retail_price,
		double discount_amount );

/* Process */
/* ------- */
INVOICE_LINE_ITEM *invoice_line_item_calloc(
		void );

double invoice_line_item_extended_price(
		double quantity,
		double retail_price,
		double discount_amount );

/* Usage */
/* ----- */
double invoice_line_item_extended_total(
		LIST *invoice_line_item_list );

/* Usage */
/* ----- */
boolean invoice_line_item_description_exists(
		LIST *invoice_line_item_list );

/* Usage */
/* ----- */
boolean invoice_line_item_discount_amount_exists(
		LIST *invoice_line_item_list );

/* Usage */
/* ----- */
int invoice_line_item_quantity_decimal_places(
		LIST *invoice_line_item_list );

/* Usage */
/* ----- */
double invoice_line_item_discount_amount(
		LIST *invoice_line_item_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *invoice_line_item_system_string(
		char *customer_full_name,
		char *customer_street_address,
		char *sale_date_time );

typedef struct
{
	LIST *invoice_line_item_list;
	double invoice_line_item_extended_total;
	double invoice_line_item_discount_amount;
	double discount_amount;
	double taxable_amount;
	double sales_tax;
	double total;
	boolean invoice_line_item_description_boolean;
	boolean invoice_line_item_discount_boolean;
	int invoice_line_item_quantity_decimal_places;
} INVOICE_CALCULATE;

/* Usage */
/* ----- */
INVOICE_CALCULATE *invoice_calculate_new(
		LIST *invoice_line_item_list,
		double discount_amount,
		double sales_tax_rate );

/* Process */
/* ------- */
INVOICE_CALCULATE *invoice_calculate_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
double invoice_calculate_discount_amount(
		double discount_amount,
		double invoice_line_item_discount_amount );

double invoice_calculate_taxable_amount(
		double invoice_line_item_extended_total,
		double invoice_calculate_discount_amount );

double invoice_calculate_sales_tax(
		double sale_tax_rate,
		double invoice_calculate_taxable_amount );

double invoice_calculate_total(
		double invoice_line_item_extended_total,
		double invoice_calculate_sales_tax );

typedef struct
{
	INVOICE_CALCULATE *invoice_calculate;
	double shipping_amount;
	double total_amount;
} INVOICE_DATA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVOICE_DATA *invoice_data_new(
		INVOICE_CALCULATE *invoice_calculate,
		double shipping_amount );

/* Process */
/* ------- */
INVOICE_DATA *invoice_data_calloc(
		void );

double invoice_data_total_amount(
		double invoice_calculate_total,
		double shipping_amount );

typedef struct
{
	INVOICE_DATA *invoice_data;
	ENTITY_SELF *entity_self;
	CUSTOMER *customer;
	double customer_payment;
	LIST *extra_label_list;
	char *instructions;
	boolean workorder_boolean;
	char *use_key;
	char *caption;
	char *date_string;
	double amount_due;
	char *amount_due_label;
} INVOICE;

/* Usage */
/* ----- */
INVOICE *invoice_new(
		char *invoice_key,
		char *transaction_date_time_string,
		char *invoice_date_time_string,
		char *customer_name,
		char *customer_street_address,
		INVOICE_DATA *invoice_data,
		double customer_payment,
		LIST *extra_label_list,
		char *instructions,
		boolean workorder_boolean,
		boolean estimate_boolean );

/* Process */
/* ------- */
INVOICE *invoice_calloc(
		void );

/* Returns first non-empty parameter */
/* --------------------------------- */
char *invoice_use_key(
		char *invoice_key,
		char *transaction_date_time_string,
		char *invoice_date_time_string );

/* Returns static memory */
/* --------------------- */
char *invoice_caption(
		char *customer_name,
		boolean workorder_boolean,
		char *invoice_use_key );

/* Returns static memory */
/* --------------------- */
char *invoice_date_string(
		char *invoice_date_time_string );

double invoice_amount_due(
		double invoice_data_total_amount,
		double customer_payment );

/* Returns program memory */
/* ---------------------- */
char *invoice_amount_due_label(
		boolean estimate_boolean );

/* Driver */
/* ------ */
void invoice_html_output(
		FILE *output_stream,
		INVOICE *invoice );

/* Process */
/* ------- */
void invoice_html_output_style(
		FILE *output_stream );

void invoice_html_output_table_open(
		FILE *output_stream,
		char *invoice_caption );

void invoice_html_output_self(
		FILE *output_stream,
		char *invoice_date_string,
		ENTITY_SELF *entity_self,
		boolean invoice_line_item_description_exists,
		boolean invoice_line_item_discount_amount_exists );

void invoice_html_output_customer(
		FILE *output_stream,
	       	CUSTOMER *customer,
		boolean invoice_line_item_description_exists,
		boolean invoice_line_item_discount_amount_exists );

void invoice_html_output_extra(
		FILE *output_stream,
		LIST *extra_label_list,
		char *instructions );

void invoice_html_output_table_header(
		FILE *output_stream,
		boolean workorder_boolean,
		boolean invoice_line_item_description_exists,
		boolean invoice_line_item_discount_amount_exists );

void invoice_html_output_line_item_list(
		FILE *output_stream,
		LIST *invoice_line_item_list,
		boolean workorder_boolean,
		boolean invoice_line_item_description_exists,
		boolean invoice_line_item_discount_amount_exists,
		int invoice_line_item_quantity_decimal_places );

void invoice_html_output_footer(
		FILE *output_stream,
		char *invoice_amount_due_label,
		double sales_tax_amount,
		double shipping_amount,
		boolean invoice_line_item_description_exists,
		boolean invoice_line_item_discount_amount_exists,
		double invoice_data_total_amount,
		double customer_payment,
		double invoice_amount_due );

void invoice_html_output_table_close(
		FILE *output_stream );

#endif

