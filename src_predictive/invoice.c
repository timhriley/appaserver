/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/invoice.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "float.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "column.h"
#include "piece.h"
#include "sql.h"
#include "entity_self.h"
#include "customer.h"
#include "invoice.h"

INVOICE *invoice_calloc( void )
{
	INVOICE *invoice;

	if ( ! ( invoice = calloc( 1, sizeof ( INVOICE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return invoice;
}

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
		boolean estimate_boolean )
{
	INVOICE *invoice;

	if ( !invoice_date_time_string
	||   !customer_name
	||   !customer_street_address
	||   !invoice_data )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	invoice = invoice_calloc();

	invoice->extra_label_list = extra_label_list;
	invoice->instructions = instructions;
	invoice->workorder_boolean = workorder_boolean;

	if ( ! ( invoice->entity_self =
			entity_self_fetch(
				1 /* fetch_entity_boolean */ ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"entity_self_fetch() returned  empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( invoice->customer =
			customer_fetch(
				customer_name,
				customer_street_address,
				1 /* fetch_entity_boolean */ ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"customer_fetch(%s,%s) returned empty.",
			customer_name,
			customer_street_address );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	invoice->use_key =
		/* --------------------------------- */
		/* Returns first non-empty parameter */
		/* --------------------------------- */
		invoice_use_key(
			invoice_key,
			transaction_date_time_string,
			invoice_date_time_string );

	invoice->caption =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		invoice_caption(
			customer_name,
			workorder_boolean,
			invoice->use_key );

	invoice->date_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		invoice_date_string(
			invoice_date_time_string );

	invoice->amount_due =
		invoice_amount_due(
			invoice_data->total_amount,
			customer_payment );

	invoice->amount_due_label =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		invoice_amount_due_label(
			estimate_boolean );

	return invoice;
}

double invoice_amount_due(
		double total_amount,
		double customer_payment )
{
	return
	total_amount - customer_payment;
}

double invoice_line_item_extended_price(
		double quantity,
		double retail_price,
		double discount_amount )
{
	return
	quantity * retail_price - discount_amount;
}

INVOICE_LINE_ITEM *invoice_line_item_calloc( void )
{
	INVOICE_LINE_ITEM *invoice_line_item;

	if ( ! ( invoice_line_item =
			calloc( 1,
				sizeof ( INVOICE_LINE_ITEM ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return invoice_line_item;
}

INVOICE_LINE_ITEM *invoice_line_item_new(
		char *item,
		char *description /* stack memory */,
		double quantity,
		double retail_price,
		double discount_amount )
{
	INVOICE_LINE_ITEM *invoice_line_item;

	if ( !item ) return NULL;

	invoice_line_item = invoice_line_item_calloc();

	invoice_line_item->item = item;

	if ( *description )
		invoice_line_item->description =
			strdup( description );

	invoice_line_item->quantity = quantity;
	invoice_line_item->retail_price = retail_price;
	invoice_line_item->discount_amount = discount_amount;

	invoice_line_item->extended_price =
		invoice_line_item_extended_price(
			quantity,
			retail_price,
			discount_amount );

	return invoice_line_item;
}

void invoice_html_output_style( FILE *output_stream )
{
	fprintf(output_stream,
"<style type=\"text/css\">			\n"
"<!--						\n"
"table {page-break-after: always;		\n"
"	page-break-before: avoid; }		\n"
"-->						\n"
"</style>					\n" );
}

void invoice_html_output_footer(
		FILE *output_stream,
		char *amount_due_label,
		double sales_tax,
		double shipping_amount,
		boolean description_exists,
		boolean discount_amount_exists,
		double total_amount,
		double customer_payment,
		double amount_due )
{
	if ( !float_virtually_same( sales_tax, 0.0 ) )
	{
		fprintf( output_stream, "<tr><td>Sales tax" );

		if ( description_exists )
			fprintf( output_stream, "<td>" );

		/* Placeholder for quantity */
		/* ------------------------ */
		fprintf( output_stream, "<td>" );

		if ( discount_amount_exists )
			fprintf( output_stream, "<td>" );

		fprintf(output_stream,
"<td align=right>%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	string_commas_money( sales_tax ) );
	}

	if ( !float_virtually_same( shipping_amount, 0.0 ) )
	{
		fprintf( output_stream, "<tr><td>Shipping" );

		if ( description_exists )
			fprintf( output_stream, "<td>" );

		/* Placeholder for quantity */
		/* ------------------------ */
		fprintf( output_stream, "<td>" );

		if ( discount_amount_exists )
			fprintf( output_stream, "<td>" );

		fprintf(output_stream,
"<td align=right>%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	string_commas_money(
				shipping_amount ) );
	}

	if ( float_virtually_same( customer_payment, 0.0 ) )
	{
		fprintf(output_stream,
			"<tr><td><big>%s</big>",
			amount_due_label );

		if ( description_exists )
			fprintf( output_stream, "<td>" );

		/* Placeholder for quantity */
		/* ------------------------ */
		fprintf( output_stream, "<td>" );

		if ( discount_amount_exists )
			fprintf( output_stream, "<td>" );

		fprintf(output_stream,
"<td align=right><big>$%s</big>\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_money(
			 	amount_due ) );
	}
	else
	{
		fprintf( output_stream, "<tr><td>Invoice Total" );

		if ( description_exists )
			fprintf( output_stream, "<td>" );

		/* Placeholder for quantity */
		/* ------------------------ */
		fprintf( output_stream, "<td>" );

		if ( discount_amount_exists )
			fprintf( output_stream, "<td>" );

		fprintf(output_stream,
"<td align=right>$%s\n"
"</table>								\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_money(
			 	total_amount ) );

		fprintf( output_stream, "<tr><td>Payment (Thank you)" );

		if ( description_exists )
			fprintf( output_stream, "<td>" );

		/* Placeholder for quantity */
		/* ------------------------ */
		fprintf( output_stream, "<td>" );

		if ( discount_amount_exists )
			fprintf( output_stream, "<td>" );

		fprintf(output_stream,
"<td align=right>$%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_money(
			 	customer_payment ) );

		fprintf(output_stream,
			"<tr><td><big>%s</big>",
			amount_due_label );

		if ( description_exists )
			fprintf( output_stream, "<td>" );

		/* Placeholder for quantity */
		/* ------------------------ */
		fprintf( output_stream, "<td>" );

		if ( discount_amount_exists )
			fprintf( output_stream, "<td>" );

		fprintf(output_stream,
"<td align=right><big>$%s</big>\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_money( amount_due ) );
	}
}

void invoice_html_output_line_item_list(
		FILE *output_stream,
		LIST *line_item_list,
		boolean workorder_boolean,
		boolean description_exists,
		boolean discount_amount_exists,
		int quantity_decimal_places )
{
	INVOICE_LINE_ITEM *line_item;
	char buffer[ 256 ];
	char dollar_string[ 2 ];

	strcpy( dollar_string, "$" );

	if ( list_rewind( line_item_list ) )
	do {
		line_item = list_get( line_item_list );

		fprintf( output_stream, "<tr>" );

		fprintf(output_stream,
			"<td>%s",
			string_initial_capital(
				buffer,
				line_item->item ) );

		if ( description_exists )
		{
			fprintf(output_stream,
"<td>%s",
				(line_item->description)
					? line_item->description
					: "" );
		}

		fprintf(output_stream,
"<td align=right>%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	string_commas_float(
				line_item->quantity,
				quantity_decimal_places ) );

		if ( !workorder_boolean )
		{
			fprintf(output_stream,
"<td align=right>%s%s",
			 	dollar_string,
				/* --------------------- */
			 	/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					line_item->retail_price ) );

			if ( discount_amount_exists )
			{
				fprintf(output_stream,
"<td align=right>%s%s",
			 		dollar_string,
					/* --------------------- */
			 		/* Returns static memory */
					/* --------------------- */
					string_commas_money(
						line_item->discount_amount ) );
			}

			fprintf(output_stream,
"<td align=right>%s%s\n",
			 	dollar_string,
				/* --------------------- */
			 	/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					line_item->extended_price ) );

			if ( *dollar_string ) *dollar_string = '\0';
		}

	} while( list_next( line_item_list ) );
}

double invoice_line_item_extended_total( LIST *line_item_list )
{
	INVOICE_LINE_ITEM *line_item;
	double total = 0.0;

	if ( list_rewind( line_item_list ) )
	do {
		line_item = list_get( line_item_list );

		total += line_item->extended_price;

	} while( list_next( line_item_list ) );

	return total;
}

boolean invoice_line_item_description_exists( LIST *line_item_list )
{
	INVOICE_LINE_ITEM *line_item;

	if ( !list_rewind( line_item_list ) ) return 0;

	do {
		line_item = list_get( line_item_list );

		if ( line_item->description ) return 1;

	} while( list_next( line_item_list ) );

	return 0;
}

boolean invoice_line_item_discount_amount_exists( LIST *line_item_list )
{
	INVOICE_LINE_ITEM *line_item;

	if ( !list_rewind( line_item_list ) ) return 0;

	do {
		line_item = list_get( line_item_list );

		if ( line_item->discount_amount ) return 1;

	} while( list_next( line_item_list ) );

	return 0;
}

void invoice_html_output(
		FILE *output_stream,
		INVOICE *invoice )
{
	if ( !output_stream )
	{
		char message[ 128 ];

		sprintf(message, "output_stream is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !invoice
	||   !invoice->use_key
	||   !invoice->date_string
	||   !invoice->entity_self
	||   !invoice->customer
	||   !invoice->invoice_data
	||   !invoice->invoice_data->invoice_calculate )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	invoice_html_output_style( output_stream );

	invoice_html_output_table_open(
		output_stream,
		invoice->caption );

	invoice_html_output_self(
		output_stream,
		invoice->date_string,
		invoice->entity_self,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_description_exists,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_discount_amount_exists );

	invoice_html_output_customer(
		output_stream,
		invoice->customer,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_description_exists,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_discount_amount_exists );

	invoice_html_output_extra(
		output_stream,
		invoice->extra_label_list,
		invoice->instructions );

	invoice_html_output_table_header(
		output_stream,
		invoice->workorder_boolean,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_description_exists,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_discount_amount_exists );

	invoice_html_output_line_item_list(
		output_stream,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_list,
		invoice->workorder_boolean,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_description_exists,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_discount_amount_exists,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_quantity_decimal_places );

	invoice_html_output_footer(
		output_stream,
		invoice->amount_due_label,
		invoice->
			invoice_data->
			invoice_calculate->
			sales_tax,
		invoice->invoice_data->shipping_amount,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_description_exists,
		invoice->
			invoice_data->
			invoice_calculate->
			invoice_line_item_discount_amount_exists,
		invoice->
			invoice_data->
			total_amount,
		invoice->customer_payment,
		invoice->amount_due );

	invoice_html_output_table_close( output_stream );
}

char *invoice_use_key(
		char *invoice_key,
		char *transaction_date_time_string,
		char *invoice_date_time_string )
{
	char *key;

	if ( invoice_key && *invoice_key )
		key = invoice_key;
	else
	if ( transaction_date_time_string && *transaction_date_time_string )
		key = transaction_date_time_string;
	else
		key = invoice_date_time_string;

	if ( !key )
	{
		char message[ 128 ];

		sprintf(message, "key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return key;
}

char *invoice_date_string( char *invoice_date_time_string )
{
	static char date_string[ 128 ];

	column(	date_string, 0, invoice_date_time_string );

	return date_string;
}

int invoice_line_item_quantity_decimal_places(
		LIST *invoice_line_item_list )
{
	INVOICE_LINE_ITEM *invoice_line_item;

	if ( !list_rewind( invoice_line_item_list ) ) return 0;

	do {
		invoice_line_item =
			list_get(
				invoice_line_item_list );

		if ( !float_is_integer( invoice_line_item->quantity ) )
		{
			return
			INVOICE_LINE_ITEM_QUANTITY_DECIMAL_PLACES;
		}

	} while ( list_next( invoice_line_item_list ) );

	return 0;
}

void invoice_html_output_table_open(
		FILE *output_stream,
		char *invoice_caption )
{
	char caption_buffer[ 128 ];

	if ( !invoice_caption )
	{
		char message[ 128 ];

		sprintf(message, "caption is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy( caption_buffer, invoice_caption );
	string_search_replace( caption_buffer, " ", "&nbsp;" );

	fprintf(output_stream,
"<table border=1>						\n"
"<caption align=top><big><bold>%s				\n"
"</bold></big></caption>					\n",
		caption_buffer );
}

char *invoice_caption(
		char *customer_name,
		boolean workorder_boolean,
		char *invoice_use_key )
{
	static char caption[ 256 ];
	char *title;

	if ( !customer_name
	||   !invoice_use_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( workorder_boolean )
		title = "Workorder";
	else
		title = "Invoice";

	sprintf(caption,
		"%s %s for %s",
		title,
		invoice_use_key,
		customer_name );

	return caption;
}

void invoice_html_output_self(
		FILE *output_stream,
		char *invoice_date_string,
		ENTITY_SELF *entity_self,
		boolean description_exists,
		boolean discount_amount_exists )
{
	int column_span = 3;

	if ( !invoice_date_string
	||   !entity_self
	||   !entity_self->entity )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( description_exists ) column_span++;
	if ( discount_amount_exists ) column_span++;

	fprintf(output_stream,
"<tr><td colspan=%d>\n"
"Date: %s<br>\n"
"%s<br>\n"
"%s<br>\n"
"%s, %s %s\n"
"</td>\n",
	 	column_span,
		invoice_date_string,
	 	entity_self->full_name,
	 	entity_self->street_address,
	 	entity_self->entity->city,
	 	entity_self->entity->state_code,
	 	entity_self->entity->zip_code );
}

void invoice_html_output_customer(
		FILE *output_stream,
	       	CUSTOMER *customer,
		boolean description_exists,
		boolean discount_amount_exists )
{
	int column_span = 3;

	if ( !customer
	||   !customer->entity )
	{
		char message[ 128 ];

		sprintf(message, "customer is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( description_exists ) column_span++;
	if ( discount_amount_exists ) column_span++;

	if ( customer->entity->city )
	{
		fprintf(output_stream,
"<tr><td colspan=%d>\n"
"Customer:\n"
"<br>%s\n"
"<br>%s\n"
"<br>%s, %s %s\n",
		 	column_span + 1,
		 	customer->customer_full_name,
		 	customer->customer_street_address,
		 	customer->entity->city,
		 	customer->entity->state_code,
		 	customer->entity->zip_code );
	}
	else
	{
		fprintf(output_stream,
"<tr><td colspan=%d>\n"
"Customer:\n"
"<br>%s\n"
"<br>%s\n",
		 	column_span + 1,
		 	customer->customer_full_name,
		 	customer->customer_street_address );
	}

	if ( customer->customer_key )
	{
		fprintf(output_stream,
"<br>Label:%s\n",
		 	customer->customer_key );
	}
}

void invoice_html_output_extra(
		FILE *output_stream,
		LIST *extra_label_list,
		char *instructions )
{
	if ( list_rewind( extra_label_list ) )
	do {
		fprintf(output_stream,
			"<tr><td>%s\n",
			(char *)list_get( extra_label_list ) );

	} while ( list_next( extra_label_list ) );

	if ( instructions && *instructions )
	{
		fprintf(output_stream,
			"<tr><td>%s\n",
			instructions );
	}
}

void invoice_html_output_table_header(
		FILE *output_stream,
		boolean workorder_boolean,
		boolean description_exists,
		boolean discount_amount_exists )
{
	fprintf( output_stream, "<tr>" );

	fprintf( output_stream, "<th>Item" );

	if ( description_exists ) fprintf( output_stream, "<th>Description" );

	fprintf( output_stream, "<th>Quantity" );

	if ( !workorder_boolean )
	{
		if ( discount_amount_exists )
		{
			fprintf(output_stream,
"<th>Retail Price<th>Discount<th>Extended\n" );
		}
		else
		{
			fprintf(output_stream,
"<th>Retail Price<th>Extended</tr>\n" );
		}
	}
}

void invoice_html_output_table_close( FILE *output_stream )
{
	fprintf( output_stream, "</table>\n" );
}

char *invoice_amount_due_label( boolean estimate_boolean )
{
	if ( estimate_boolean )
		return "Estimated Due";
	else
		return "Amount Due";
}

INVOICE_DATA *invoice_data_new(
		INVOICE_CALCULATE *invoice_calculate,
		double shipping_amount )
{
	INVOICE_DATA *invoice_data;

	if ( !invoice_calculate )
	{
		char message[ 128 ];

		sprintf(message, "invoice_calculate is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	invoice_data = invoice_data_calloc();

	invoice_data->invoice_calculate = invoice_calculate;

	invoice_data->total_amount =
		invoice_data_total_amount(
			invoice_calculate->total,
			shipping_amount );

	return invoice_data;
}

INVOICE_DATA *invoice_data_calloc( void )
{
	INVOICE_DATA *invoice_data;

	if ( ! ( invoice_data = calloc( 1, sizeof ( INVOICE_DATA ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return invoice_data;
}

double invoice_calculate_total_amount(
		double invoice_calculate_total,
		double shipping_amount )
{
	return invoice_calculate_total + shipping_amount;
}

INVOICE_CALCULATE *invoice_calculate_new(
		LIST *invoice_line_item_list,
		double discount_amount,
		double sales_tax_rate )
{
	INVOICE_CALCULATE *invoice_calculate;

	if ( !list_length( invoice_line_item_list ) ) return NULL;

	invoice_calculate = invoice_calculate_calloc();

	invoice_calculate->invoice_line_item_list = invoice_line_item_list;

	invoice_calculate->invoice_line_item_extended_total =
		invoice_line_item_extended_total(
			invoice_line_item_list );

	invoice_calculate->invoice_line_item_discount_amount =
		invoice_line_item_discount_amount(
			invoice_line_item_list );

	invoice_calculate->discount_amount =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		invoice_calculate_discount_amount(
			discount_amount,
			invoice_calculate->
				invoice_line_item_discount_amount );

	invoice_calculate->taxable_amount =
		invoice_calculate_taxable_amount(
			invoice_calculate->invoice_line_item_extended_total,
			invoice_calculate->discount_amount );

	invoice_calculate->sales_tax =
		invoice_calculate_sales_tax(
			sales_tax_rate,
			invoice_calculate->taxable_amount );
	
	invoice_calculate->total =
		invoice_calculate_total(
			invoice_calculate->invoice_line_item_extended_total,
			invoice_calculate->sales_tax );

	invoice_calculate->invoice_line_item_description_exists =
		invoice_line_item_description_exists(
			invoice_line_item_list );

	invoice_calculate->invoice_line_item_discount_amount_exists =
		invoice_line_item_discount_amount_exists(
			invoice_line_item_list );

	invoice_calculate->invoice_line_item_quantity_decimal_places =
		invoice_line_item_quantity_decimal_places(
			invoice_line_item_list );

	return invoice_calculate;
}

INVOICE_CALCULATE *invoice_calculate_calloc( void )
{
	INVOICE_CALCULATE *invoice_calculate;

	if ( ! ( invoice_calculate =
			calloc( 1, sizeof ( INVOICE_CALCULATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return invoice_calculate;
}

double invoice_calculate_discount_amount(
		double discount_amount,
		double invoice_line_item_discount_amount )
{
	if ( !float_virtually_same( discount_amount, 0.0 ) )
		return discount_amount;
	else
		return invoice_line_item_discount_amount;
}

double invoice_calculate_taxable_amount(
		double invoice_line_item_extended_total,
		double invoice_calculate_discount_amount )
{
	return
	invoice_line_item_extended_total -
	invoice_calculate_discount_amount;
}

double invoice_calculate_sales_tax(
		double sales_tax_rate,
		double invoice_calculate_taxable_amount )
{
	if ( float_virtually_same( sales_tax_rate, 0.0 ) ) return 0.0;

	return
	sales_tax_rate *
	invoice_calculate_taxable_amount;
}

double invoice_calculate_total(
		double invoice_line_item_extended_total,
		double invoice_calculate_sales_tax )
{
	return
	invoice_line_item_extended_total +
	invoice_calculate_sales_tax;
}

double invoice_line_item_discount_amount( LIST *line_item_list )
{
	INVOICE_LINE_ITEM *line_item;
	double discount_amount = 0.0;

	if ( list_rewind( line_item_list ) )
	do {
		line_item = list_get( line_item_list );

		discount_amount += line_item->discount_amount;

	} while( list_next( line_item_list ) );

	return discount_amount;
}

char *invoice_line_item_system_string(
		char *customer_full_name,
		char *customer_street_address,
		char *sale_date_time )
{
	char system_string[ 1024 ];

	if ( !customer_full_name
	||   !customer_street_address
	||   !sale_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"invoice_select.sh \"%s\" \"%s\" \"%s\"",
		customer_full_name,
		customer_street_address,
		sale_date_time );

	return strdup( system_string );
}

LIST *invoice_line_item_list(
		char *customer_full_name,
		char *customer_street_address,
		char *sale_date_time )
{
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	INVOICE_LINE_ITEM *invoice_line_item;
	LIST *list;

	if ( !customer_full_name
	||   !customer_street_address
	||   !sale_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_line_item_system_string(
			customer_full_name,
			customer_street_address,
			sale_date_time );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe( system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		invoice_line_item =
			invoice_line_item_parse(
				input );

		if ( invoice_line_item ) list_set( list, invoice_line_item );
	}

	pclose( input_pipe );

	return list;
}

INVOICE_LINE_ITEM *invoice_line_item_parse( char *input )
{
	char item[ 256 ];
	char description[ 1024 ];
	char quantity[ 128 ];
	char retail_price[ 128 ];
	char discount_amount[ 128 ];

	if ( !input ) return NULL;

	/* See invoice_select.sh */
	/* --------------------- */
	piece( item, SQL_DELIMITER, input, 0 );
	piece( description, SQL_DELIMITER, input, 1 );
	piece( quantity, SQL_DELIMITER, input, 2 );
	piece( retail_price, SQL_DELIMITER, input, 3 );
	piece( discount_amount, SQL_DELIMITER, input, 4 );

	return
	invoice_line_item_new(
		strdup( item ),
		description /* stack memory */,
		atof( quantity ),
		atof( retail_price ),
		atof( discount_amount ) );
}

