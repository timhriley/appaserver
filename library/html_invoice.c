/* library/html_invoice.c						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver html_invoice ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "html_invoice.h"
#include "timlib.h"

HTML_INVOICE *html_invoice_new(		char *invoice_date,
					char *company_name,
					char *company_street_address,
					char *company_suite_number,
					char *company_city,
					char *company_state,
					char *company_zip_code,
					char *line_item_key_heading )
{
	HTML_INVOICE *h = (HTML_INVOICE *)calloc( 1, sizeof( HTML_INVOICE ) );

	h->invoice_date = invoice_date;
	h->line_item_key_heading = line_item_key_heading;

	h->invoice_company.name = company_name;
	h->invoice_company.street_address = company_street_address;
	h->invoice_company.suite_number = company_suite_number;
	h->invoice_company.city = company_city;
	h->invoice_company.state = company_state;
	h->invoice_company.zip_code = company_zip_code;
	return h;
} /* html_invoice_new() */

HTML_INVOICE_CUSTOMER *html_invoice_customer_new(
					char *invoice_key,
					char *customer_name,
					char *customer_street_address,
					char *customer_suite_number,
					char *customer_city,
					char *customer_state,
					char *customer_zip_code,
					char *customer_service_key,
					double sales_tax,
					double shipping_charge )
{
	HTML_INVOICE_CUSTOMER *h =
		(HTML_INVOICE_CUSTOMER *)
			calloc( 1, sizeof( HTML_INVOICE_CUSTOMER ) );

	h->invoice_key = invoice_key;
	h->name = customer_name;
	h->street_address = customer_street_address;
	h->suite_number = customer_suite_number;
	h->city = customer_city;
	h->state = customer_state;
	h->zip_code = customer_zip_code;
	h->customer_service_key = customer_service_key;
	h->sales_tax = sales_tax;
	h->shipping_charge = shipping_charge;
	h->invoice_line_item_list = list_new();
	return h;
} /* html_invoice_customer_new() */

double html_invoice_append_line_item(	LIST *invoice_line_item_list,
					char *item_key,
					char *item,
					int quantity,
					double retail_price,
					double discount_amount )
{
	HTML_INVOICE_LINE_ITEM *line_item;

	line_item = html_invoice_line_item_new(
					item_key,
					item,
					quantity,
					retail_price,
					discount_amount );

	list_append_pointer( invoice_line_item_list, line_item );
	return HTML_INVOICE_EXTENSION;

} /* html_invoice_append_line_item() */

HTML_INVOICE_LINE_ITEM *html_invoice_line_item_new(
					char *item_key,
					char *item,
					int quantity,
					double retail_price,
					double discount_amount )
{
	HTML_INVOICE_LINE_ITEM *h;

	if ( ! ( h = (HTML_INVOICE_LINE_ITEM *)
		calloc( 1, sizeof( HTML_INVOICE_LINE_ITEM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	h->item_key = item_key;
	h->item = item;
	h->quantity = quantity;
	h->retail_price = retail_price;
	h->discount_amount = discount_amount;
	return h;

} /* html_invoice_line_item_new() */

void html_invoice_line_item_free(	HTML_INVOICE_LINE_ITEM *
						html_invoice_line_item )
{
	if ( html_invoice_line_item->item_key )
		free( html_invoice_line_item->item_key );

	free( html_invoice_line_item->item );
	free( html_invoice_line_item );
}

void html_invoice_output_header(	FILE *output_stream )
{
	fprintf( output_stream,
"<style type=\"text/css\">			\n"
"<!--						\n"
"table {page-break-after: always;		\n"
"	page-break-before: avoid; }		\n"
"-->						\n"
"</style>					\n" );
}

void html_invoice_output_invoice_header(FILE *output_stream,
					char *invoice_date,
					char *line_item_key_heading,
					HTML_INVOICE_COMPANY *
						html_invoice_company,
	       				HTML_INVOICE_CUSTOMER *
						html_invoice_customer,
					boolean exists_discount_amount,
					char *title,
					boolean omit_money )
{
	int column_span = 3;
	char company_street_address[ 128 ];
	char customer_street_address[ 128 ];
	char invoice_date_buffer[ 128 ];

	strcpy( invoice_date_buffer, invoice_date );
	search_replace_string( invoice_date_buffer, " ", "&nbsp;" );

	if ( *html_invoice_company->suite_number )
	{
		sprintf( company_street_address,
			 "%s, Suite %s",
			 html_invoice_company->street_address,
			 html_invoice_company->suite_number );
	}
	else
	{
		strcpy( company_street_address,
			html_invoice_company->street_address );
	}

	if ( *html_invoice_customer->suite_number )
	{
		sprintf( customer_street_address,
			 "%s, Suite %s",
			 html_invoice_customer->street_address,
			 html_invoice_customer->suite_number );
	}
	else
	{
		strcpy( customer_street_address,
			html_invoice_customer->street_address );
	}

	if ( line_item_key_heading
	&&   *line_item_key_heading )
	{
		column_span++;
	}

	if ( exists_discount_amount ) column_span++;

	fprintf( output_stream,
"<table border=1>						\n"
"<caption align=top><big><bold>%s for %s</bold></big></caption>\n",
		 title,
		 html_invoice_customer->invoice_key );

	fflush( output_stream );

	fprintf( output_stream,
"<tr><td colspan=%d>						\n"
"%s<br>								\n"
"%s<br>								\n"
"%s, %s %s							\n"
"</td>								\n"
"<td align=right valign=top>%s</td>				\n",
	 	column_span,
	 	html_invoice_company->name,
	 	company_street_address,
	 	html_invoice_company->city,
	 	html_invoice_company->state,
	 	html_invoice_company->zip_code,
	 	invoice_date_buffer );

	if ( html_invoice_customer->city
	&&   *html_invoice_customer->city )
	{
		fprintf( output_stream,
"<tr><td colspan=%d>						\n"
"Customer:							\n"
"<br>%s								\n"
"<br>%s								\n"
"<br>%s, %s %s							\n",
		 	column_span + 1,
		 	html_invoice_customer->name,
		 	customer_street_address,
		 	html_invoice_customer->city,
		 	html_invoice_customer->state,
		 	html_invoice_customer->zip_code );
	}
	else
	{
		fprintf( output_stream,
"<tr><td colspan=%d>						\n"
"Customer:							\n"
"<br>%s								\n"
"<br>%s								\n",
		 	column_span + 1,
		 	html_invoice_customer->name,
		 	customer_street_address );
	}

	if ( html_invoice_customer->customer_service_key
	&&   *html_invoice_customer->customer_service_key )
	{
		fprintf( output_stream,
"<br>%s								\n",
		 	html_invoice_customer->customer_service_key );
	}

	fprintf( output_stream, "<tr>" );

	if ( line_item_key_heading
	&&   *line_item_key_heading )
	{
		fprintf( output_stream, "<th>%s", line_item_key_heading );
	}

	if ( !omit_money )
	{
		if ( exists_discount_amount )
		{
			fprintf( output_stream,
"<th>Item<th>Quantity<th>Retail Price<th>Discount<th>Extension</tr>\n" );
		}
		else
		{
			fprintf( output_stream,
"<th>Item<th>Quantity<th>Retail Price<th>Extension</tr>\n" );
		}
	}
	else
	{
		fprintf( output_stream,
"<th>Item<th>Quantity</tr>\n" );
	}
}

void html_invoice_output_invoice_footer(
					FILE *output_stream,
					double extension_total,
					double sales_tax,
					double shipping_charge,
					char *line_item_key_heading,
					boolean exists_discount_amount )
{
	char buffer[ 128 ];

	if ( sales_tax )
	{
		fprintf( output_stream, "<tr><td>Sales tax" );

		if ( line_item_key_heading )
			fprintf( output_stream, "<td>" );

		if ( exists_discount_amount )
			fprintf( output_stream, "<td>" );

		fprintf( output_stream,
"<td><td width=100 align=right>%s\n",
		 	commas_in_double( buffer, sales_tax ) );

	}

	if ( shipping_charge )
	{
		fprintf( output_stream, "<tr><td>Shipping" );

		if ( line_item_key_heading )
			fprintf( output_stream, "<td>" );

		if ( exists_discount_amount )
			fprintf( output_stream, "<td>" );

		fprintf( output_stream,
"<td><td width=100 align=right>%s\n",
		 	commas_in_double( buffer, shipping_charge ) );

	}

	fprintf( output_stream, "<tr><td><big>Amount Due</big>" );

	if ( line_item_key_heading )
		fprintf( output_stream, "<td>" );

	if ( exists_discount_amount )
		fprintf( output_stream, "<td>" );

	fprintf( output_stream,
"<td><td width=100 align=right><big>$%s</big>\n"
"</table>								\n",
		 commas_in_double(	buffer,
					extension_total +
					sales_tax +
					shipping_charge ) );

} /* html_invoice_output_invoice_footer() */

void html_invoice_company_free(		HTML_INVOICE_COMPANY *
						invoice_company )
{
	free( invoice_company->name );
	free( invoice_company->street_address );
	free( invoice_company->suite_number );
	free( invoice_company->city );
	free( invoice_company->state );
	free( invoice_company->zip_code );
	/* Not dynamically allocated: free( invoice_company ); */
} /* html_invoice_company_free() */

void html_invoice_customer_free(	HTML_INVOICE_CUSTOMER *
						html_invoice_customer )
{
	HTML_INVOICE_LINE_ITEM *line_item;
	LIST *line_item_list;

	line_item_list = html_invoice_customer->invoice_line_item_list;

	if ( list_rewind( line_item_list ) )
	{
		do {
			line_item = list_get_pointer( line_item_list );
			html_invoice_line_item_free( line_item );
		} while( list_next( line_item_list ) );
	}

	free( html_invoice_customer->invoice_key );
	free( html_invoice_customer->name );
	free( html_invoice_customer->street_address );
	free( html_invoice_customer->suite_number );
	free( html_invoice_customer->city );
	free( html_invoice_customer->state );
	free( html_invoice_customer->zip_code );

	if ( html_invoice_customer->customer_service_key )
		free( html_invoice_customer->customer_service_key );

	list_free( line_item_list );
	free( html_invoice_customer );

} /* html_invoice_customer_free() */

void html_invoice_output_invoice_line_items(
					FILE *output_stream,
					LIST *invoice_line_item_list,
					boolean exists_discount_amount,
					boolean omit_money )
{
	HTML_INVOICE_LINE_ITEM *line_item;
	char buffer[ 256 ];
	char dollar_string[ 2 ];
	int quantity;
	double retail_price;
	double discount_amount;

	strcpy( dollar_string, "$" );

	if ( !list_rewind( invoice_line_item_list ) ) return;

	do {
		line_item = list_get_pointer( invoice_line_item_list );

		fprintf( output_stream, "<tr>" );

		if ( line_item->item_key
		&&   *line_item->item_key )
		{
			fprintf(	output_stream,
					"<td>%s",
					line_item->item_key );
		}

		quantity = line_item->quantity;

		if ( !omit_money )
		{
			retail_price = line_item->retail_price;
			discount_amount = line_item->discount_amount;

			fprintf( output_stream,
"<td>%s<td width=100 align=right>%s<td width=100 align=right>%s%.2lf",
			 	format_initial_capital(
					buffer, line_item->item ),
			 	place_commas_in_long( quantity ),
			 	dollar_string,
			 	retail_price );

			if ( exists_discount_amount )
			{
				fprintf( output_stream,
"<td width=100 align=right>%s%.2lf",
			 		 dollar_string,
			 		 discount_amount );
			}

			fprintf( output_stream,
"<td width=100 align=right>%s%.2lf\n",
			 	 dollar_string,
			 	 HTML_INVOICE_EXTENSION );

			if ( *dollar_string ) *dollar_string = '\0';
		}
		else
		{
			fprintf( output_stream,
"<td>%s<td width=100 align=right>%s\n",
			 	 format_initial_capital(
					buffer, line_item->item ),
			 	 place_commas_in_long( quantity ) );
		}

	} while( list_next( invoice_line_item_list ) );

} /* html_invoice_output_invoice_line_items() */

void html_invoice_free( HTML_INVOICE *invoice )
{
	free( invoice->invoice_date );

	if ( invoice->line_item_key_heading )
		free( invoice->line_item_key_heading );

	free( invoice );

} /* html_invoice_free() */

boolean html_invoice_get_exists_discount_amount(
			LIST *invoice_line_item_list )
{
	HTML_INVOICE_LINE_ITEM *line_item;

	if ( !list_rewind( invoice_line_item_list ) ) return 0;

	do {
		line_item = list_get_pointer( invoice_line_item_list );

		if ( line_item->discount_amount ) return 1;

	} while( list_next( invoice_line_item_list ) );

	return 0;

} /* html_invoice_get_exists_discount_amount() */
