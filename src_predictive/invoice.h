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

#define INVOICE_LINE_ITEM_QUANTITY_DECIMAL_COUNT	4

enum invoice_enum {
	invoice_due,
	invoice_workorder,
	invoice_estimate };

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

/* Safely returns */
/* -------------- */
INVOICE_LINE_ITEM *invoice_line_item_new(
		char *item_key,
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
boolean invoice_line_item_description_boolean(
		LIST *invoice_line_item_list );

/* Usage */
/* ----- */
boolean invoice_line_item_discount_boolean(
		LIST *invoice_line_item_list );

/* Usage */
/* ----- */
int invoice_line_item_quantity_decimal_count(
		LIST *invoice_line_item_list );

/* Usage */
/* ----- */
double invoice_line_item_discount_total(
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
	boolean invoice_line_item_description_boolean;
	boolean invoice_line_item_discount_boolean;
	int invoice_line_item_quantity_decimal_count;
	double invoice_line_item_extended_total;
	double invoice_line_item_discount_total;
	double invoice_amount;
	double amount_due;
} INVOICE_SUMMARY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVOICE_SUMMARY *invoice_summary_new(
		LIST *invoice_line_item_list,
		double customer_payable_balance );

/* Process */
/* ------- */
INVOICE_SUMMARY *invoice_summary_calloc(
		void );

double invoice_summary_invoice_amount(
		double invoice_line_item_extended_total,
		double invoice_line_item_discount_total );

double invoice_summary_amount_due(
		double customer_payable_balance,
		double invoice_summary_invoice_amount );

typedef struct
{
	LIST *invoice_line_item_list;
	enum invoice_enum invoice_enum;
	ENTITY_SELF *entity_self;
	CUSTOMER *customer;
	char *use_key;
	char *title;
	char *date_string;
	char *amount_due_label;
	INVOICE_SUMMARY *invoice_summary;
} INVOICE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVOICE *invoice_new(
		char *invoice_key,
		char *transaction_date_time_string,
		char *invoice_date_time_string,
		char *customer_full_name,
		char *customer_street_address,
		enum invoice_enum invoice_enum,
		LIST *invoice_line_item_list );

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
char *invoice_title(
		char *customer_full_name,
		enum invoice_enum invoice_enum,
		char *invoice_use_key );

/* Returns static memory */
/* --------------------- */
char *invoice_date_string(
		char *invoice_date_time_string );

/* Returns program memory */
/* ---------------------- */
char *invoice_amount_due_label(
		enum invoice_enum invoice_enum );

/* Driver */
/* ------ */
void invoice_html_output(
		INVOICE *invoice,
		FILE *output_stream );

/* Process */
/* ------- */
void invoice_html_output_style(
		FILE *output_stream );

void invoice_html_output_table_open(
		char *invoice_title,
		FILE *output_stream );

void invoice_html_output_self(
		char *invoice_date_string,
		ENTITY_SELF *entity_self,
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		FILE *output_stream );

void invoice_html_output_customer(
	       	CUSTOMER *customer,
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		FILE *output_stream );

void invoice_html_output_table_header(
		enum invoice_enum invoice_enum,
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		FILE *output_stream );

void invoice_html_output_line_item_list(
		enum invoice_enum invoice_enum,
		LIST *invoice_line_item_list,
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		int invoice_line_item_quantity_decimal_count,
		FILE *output_stream );

void invoice_html_output_footer(
		boolean invoice_line_item_description_boolean,
		boolean invoice_line_item_discount_boolean,
		double invoice_line_item_extended_total,
		double customer_payable_balance,
		double invoice_summary_amount_due,
		char *invoice_amount_due_label,
		FILE *output_stream );

void invoice_html_output_table_close(
		FILE *output_stream );

#endif

