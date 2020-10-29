/* $APPASERVER_HOME/library/latex_invoice.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "latex_invoice.h"
#include "timlib.h"

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
			LIST *extra_label_list )
{
	LATEX_INVOICE *h =
		(LATEX_INVOICE *)
			calloc( 1, sizeof( LATEX_INVOICE ) );

	h->invoice_date = invoice_date;
	h->line_item_key_heading = line_item_key_heading;
	h->instructions = instructions;
	h->extra_label_list = extra_label_list;

	h->invoice_company.name = company_name;
	h->invoice_company.street_address = company_street_address;
	h->invoice_company.suite_number = company_suite_number;
	h->invoice_company.city = company_city;
	h->invoice_company.state = company_state;
	h->invoice_company.zip_code = company_zip_code;
	h->invoice_company.phone_number = company_phone_number;
	h->invoice_company.email_address = company_email_address;
	h->quantity_decimal_places = LATEX_INVOICE_QUANTITY_DECIMAL_PLACES;
	return h;
}

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
					double total_payment )
{
	LATEX_INVOICE_CUSTOMER *h =
		(LATEX_INVOICE_CUSTOMER *)
			calloc( 1, sizeof( LATEX_INVOICE_CUSTOMER ) );

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
	h->total_payment = total_payment;
	h->invoice_line_item_list = list_new();
	return h;
}

double latex_invoice_append_line_item(
			LIST *invoice_line_item_list,
			char *item_key,
			char *item,
			double quantity,
			double retail_price,
			double discount_amount )
{
	LATEX_INVOICE_LINE_ITEM *line_item;

	line_item = latex_invoice_line_item_new(
					item_key,
					item,
					quantity,
					retail_price,
					discount_amount );

	list_append_pointer( invoice_line_item_list, line_item );
	return LATEX_EXTENSION( quantity, retail_price, discount_amount );
}

LATEX_INVOICE_LINE_ITEM *latex_invoice_line_item_new(
			char *item_key,
			char *item,
			double quantity,
			double retail_price,
			double discount_amount )
{
	LATEX_INVOICE_LINE_ITEM *h;

	if ( ! ( h = (LATEX_INVOICE_LINE_ITEM *)
		calloc( 1, sizeof( LATEX_INVOICE_LINE_ITEM ) ) ) )
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
}

void latex_invoice_line_item_free(	LATEX_INVOICE_LINE_ITEM *
						latex_invoice_line_item )
{
	if ( latex_invoice_line_item->item_key )
		free( latex_invoice_line_item->item_key );

	free( latex_invoice_line_item->item );
	free( latex_invoice_line_item );
}

void latex_invoice_output_header(	FILE *output_stream )
{
	fprintf( output_stream,
"\\documentclass{letter}\n"
"\\usepackage{longtable}\n"
"\\usepackage{graphics}\n"
"\\usepackage[	margin=1in,\n"
"		nohead]{geometry}\n"
"\\begin{document}\n" );
}

void latex_invoice_output_invoice_header(
			FILE *output_stream,
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
			LIST *extra_label_list )
{
	char buffer[ 128 ];
	char company_street_address[ 128 ];
	char customer_street_address[ 128 ];

	if ( latex_invoice_company->suite_number
	&&   *latex_invoice_company->suite_number )
	{
		sprintf( company_street_address,
			 "%s, Suite %s",
			 latex_invoice_company->street_address,
			 latex_invoice_company->suite_number );
	}
	else
	{
		strcpy( company_street_address,
			latex_invoice_company->street_address );
	}

	if ( latex_invoice_customer->suite_number
	&&   *latex_invoice_customer->suite_number )
	{
		sprintf( customer_street_address,
			 "%s, Suite %s",
			 latex_invoice_customer->street_address,
			 latex_invoice_customer->suite_number );
	}
	else
	{
		strcpy( customer_street_address,
			latex_invoice_customer->street_address );
	}

	fprintf( output_stream,
"\\begin{center}{\\Large \\bf %s} \\end{center}\n",
	 	 title );

	fprintf( output_stream,
"\\begin{center}{\\large For %s} \\end{center}\n",
		 latex_invoice_customer->invoice_key );

	if ( logo_filename
	&&   *logo_filename
	&&   timlib_file_exists( logo_filename ) )
	{
		fprintf( output_stream,
"\\begin{center}\n"
"\\includegraphics{%s}\n"
"\\end{center}\n",
		 	 logo_filename );
	}

	fprintf( output_stream,
"\\begin{center}{%s} \\end{center}\n",
	 	 invoice_date );

	fprintf( output_stream,
"\\begin{tabular}[t]{p{5.0in}r}\n"
"%s & %s \\\\\n"
"%s, %s %s & %s \\\\\n"
"\\end{tabular}\n\n",
	 	escape_character(
			buffer,
			company_street_address,
			'#' ),
		latex_invoice_company->phone_number,
	 	latex_invoice_company->city,
	 	latex_invoice_company->state,
	 	latex_invoice_company->zip_code,
	 	latex_invoice_company->email_address );

	fprintf( output_stream,
"\\begin{tabular}[t]{l}\n"
"\\bf Customer:\n"
"\\end{tabular}\n\n" );

	if ( latex_invoice_customer->city
	&&   *latex_invoice_customer->city )
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& %s \\\\\n"
"& %s \\\\\n"
"& %s, %s %s\n"
"\\end{tabular}\n\n",
		 	latex_invoice_customer->name,
	 		escape_character(
				buffer,
				customer_street_address,
				'#' ),
		 	latex_invoice_customer->city,
		 	latex_invoice_customer->state,
		 	latex_invoice_customer->zip_code );
	}
	else
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& %s\n"
"\\end{tabular}\n\n",
		 	latex_invoice_customer->name );
	}

	if ( latex_invoice_customer->customer_service_key
	&&   *latex_invoice_customer->customer_service_key )
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& \\bf %s\n"
"\\end{tabular}\n\n",
		 	latex_invoice_customer->customer_service_key );
	}

	if ( list_rewind( extra_label_list ) )
	{
		do {
			fprintf( output_stream,
"\\begin{tabular}{l}\n"
"%s\n"
"\\end{tabular}\n\n",
		 		(char *)list_get_pointer( extra_label_list ) );
		} while( list_next( extra_label_list ) );
	}

	if ( instructions && *instructions )
	{
		fprintf( output_stream,
"\\begin{tabular}{p{6.5in}}\n"
"%s\n"
"\\end{tabular}\n\n",
		 	instructions );
	}

	if ( exists_discount_amount )
	{
		fprintf( output_stream,
"\\begin{longtable}[t]{|p{3.2in}|r|r|r|r|} \\hline\n"
"\\hline\n" );
	}
	else
	{
		fprintf( output_stream,
"\\begin{longtable}[t]{|p{4.2in}|r|r|r|} \\hline\n"
"\\hline\n" );
	}

	if ( line_item_key_heading
	&&   *line_item_key_heading )
	{
		fprintf( output_stream, "\\bf %s & ", line_item_key_heading );
	}

	if ( !omit_money )
	{
		if ( exists_discount_amount )
		{
			fprintf( output_stream,
"\\bf Description & \\bf Quantity & \\bf Retail Price & \\bf Discount & \\bf Extension \\\\[0.5ex]\n" );
		}
		else
		{
			fprintf( output_stream,
"\\bf Description & \\bf Quantity & \\bf Retail Price & \\bf Extension \\\\[0.5ex]\n" );
		}
	}
	else
	{
		fprintf( output_stream,
"\\bf Description & \\bf Quantity \\\\[0.5ex]\n" );
	}

	fprintf( output_stream,
"\\hline \\hline\n" );

}

void latex_invoice_output_invoice_footer(
					FILE *output_stream,
					double extension_total,
					double sales_tax,
					double shipping_charge,
					double total_payment,
					char *line_item_key_heading,
					boolean exists_discount_amount,
					boolean is_estimate )
{
	if ( sales_tax )
	{
		fprintf( output_stream, "Sales tax &" );

		if ( line_item_key_heading )
			fprintf( output_stream, "&" );

		if ( exists_discount_amount )
			fprintf( output_stream, "&" );

		fprintf( output_stream,
"& %s \\\\\n",
		 	timlib_commas_in_dollars( sales_tax ) );

	}

	if ( shipping_charge )
	{
		fprintf( output_stream, "Shipping &" );

		if ( line_item_key_heading )
			fprintf( output_stream, "&" );

		if ( exists_discount_amount )
			fprintf( output_stream, "&" );

		fprintf( output_stream,
"& %s \\\\\n",
		 	timlib_commas_in_dollars( shipping_charge ) );

	}

	if ( total_payment )
	{
		fprintf( output_stream, "Payment (Thank you) &" );

		if ( line_item_key_heading )
			fprintf( output_stream, "&" );

		if ( exists_discount_amount )
			fprintf( output_stream, "&" );

		fprintf( output_stream,
"& -%s \\\\\n",
		 	timlib_commas_in_dollars( total_payment ) );

	}

	if ( is_estimate )
		fprintf( output_stream, "\\bf Estimated Due &" );
	else
		fprintf( output_stream, "\\bf Amount Due &" );

	if ( line_item_key_heading )
		fprintf( output_stream, "&" );

	if ( exists_discount_amount )
		fprintf( output_stream, "&" );

	fprintf( output_stream,
"& \\bf \\$%s \\\\\n",
		 timlib_place_commas_in_money(	extension_total +
						sales_tax +
						shipping_charge -
						total_payment ) );

}

void latex_invoice_output_footer(
				FILE *output_stream,
				boolean with_customer_signature )
{
	fprintf( output_stream,
"\\hline \\hline\n"
"\\end{longtable}\n" );

	if ( with_customer_signature )
	{
		fprintf( output_stream,
"\n\\begin{tabular}{lp{2.5in}lp{2in}}\n");
		fprintf( output_stream,
"\\bf \\large{Customer Signature}  & \\line(1,0){175} &"
"\\bf \\large{Date}  & \\line(1,0){100} \\\\\n" );
		fprintf( output_stream,
"\\end{tabular}\n" );

	}

	fprintf( output_stream,
"\\end{document}\n" );
}

void latex_invoice_company_free(		LATEX_INVOICE_COMPANY *
						invoice_company )
{
	free( invoice_company->name );
	free( invoice_company->street_address );

	if ( invoice_company->suite_number )
		free( invoice_company->suite_number );

	free( invoice_company->city );
	free( invoice_company->state );
	free( invoice_company->zip_code );
	free( invoice_company->phone_number );
	free( invoice_company->email_address );
}

void latex_invoice_customer_free(	LATEX_INVOICE_CUSTOMER *
						latex_invoice_customer )
{
	LATEX_INVOICE_LINE_ITEM *line_item;
	LIST *line_item_list;

	line_item_list = latex_invoice_customer->invoice_line_item_list;

	if ( list_rewind( line_item_list ) )
	{
		do {
			line_item = list_get_pointer( line_item_list );
			latex_invoice_line_item_free( line_item );
		} while( list_next( line_item_list ) );
	}

	free( latex_invoice_customer->invoice_key );
	free( latex_invoice_customer->name );
	free( latex_invoice_customer->street_address );

	if ( latex_invoice_customer->suite_number )
		free( latex_invoice_customer->suite_number );

	free( latex_invoice_customer->city );
	free( latex_invoice_customer->state );
	free( latex_invoice_customer->zip_code );

	if ( latex_invoice_customer->customer_service_key )
		free( latex_invoice_customer->customer_service_key );

	list_free( line_item_list );
	free( latex_invoice_customer );
}

void latex_invoice_output_invoice_line_items(
					FILE *output_stream,
					LIST *invoice_line_item_list,
					boolean exists_discount_amount,
					boolean omit_money,
					int quantity_decimal_places )
{
	LATEX_INVOICE_LINE_ITEM *line_item;
	char buffer[ 256 ];
	char dollar_string[ 3 ];
	double quantity;
	double retail_price;
	double discount_amount;
	double extension;

	strcpy( dollar_string, "\\$" );

	if ( !list_rewind( invoice_line_item_list ) ) return;

	do {
		line_item = list_get_pointer( invoice_line_item_list );

		if ( line_item->item_key
		&&   *line_item->item_key )
		{
			fprintf(	output_stream,
					"%s &",
					line_item->item_key );
		}

		quantity = line_item->quantity;

		if ( !omit_money )
		{
			retail_price = line_item->retail_price;
			discount_amount = line_item->discount_amount;

			fprintf( output_stream,
"%s & %.*lf & %s%.2lf",
			 	format_initial_capital(
					buffer, line_item->item ),
				quantity_decimal_places,
			 	quantity,
			 	dollar_string,
			 	retail_price );

			if ( exists_discount_amount )
			{
				fprintf( output_stream,
"& %s%.2lf",
			 		 dollar_string,
			 		 discount_amount );
			}

			extension = LATEX_EXTENSION(
					quantity,
					retail_price,
					discount_amount );

			fprintf( output_stream,
"& %s%s \\\\\n",
			 	 dollar_string,
			 	 timlib_place_commas_in_money(
					extension ) );

			if ( *dollar_string ) *dollar_string = '\0';
		}
		else
		{
			fprintf( output_stream,
"%s & %.*lf \\\\\n",
			 	 format_initial_capital(
					buffer, line_item->item ),
				 quantity_decimal_places,
			 	 quantity );
		}

	} while( list_next( invoice_line_item_list ) );
}

void latex_invoice_free( LATEX_INVOICE *invoice )
{
	free( invoice->invoice_date );

	if ( invoice->line_item_key_heading )
		free( invoice->line_item_key_heading );

	free( invoice );
}

boolean latex_invoice_each_quantity_integer(
					LIST *invoice_line_item_list )
{
	LATEX_INVOICE_LINE_ITEM *line_item;

	if ( !list_rewind( invoice_line_item_list ) ) return 0;

	do {
		line_item = list_get_pointer( invoice_line_item_list );

		if ( !timlib_double_is_integer(
			line_item->quantity ) )
		{
			return 0;
		}

	} while( list_next( invoice_line_item_list ) );

	return 1;

}

boolean latex_invoice_get_exists_discount_amount(
			LIST *invoice_line_item_list )
{
	LATEX_INVOICE_LINE_ITEM *line_item;

	if ( !list_rewind( invoice_line_item_list ) ) return 0;

	do {
		line_item = list_get_pointer( invoice_line_item_list );

		if ( line_item->discount_amount ) return 1;

	} while( list_next( invoice_line_item_list ) );

	return 0;

}
