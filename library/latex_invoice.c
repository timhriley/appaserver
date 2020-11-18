/* $APPASERVER_HOME/library/latex_invoice.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "latex_invoice.h"
#include "timlib.h"
#include "String.h"
#include "appaserver.h"

LATEX_INVOICE *latex_invoice_new(
			char *invoice_date,
			char *self_full_name,
			char *self_street_address,
			char *self_city,
			char *self_state_code,
			char *self_zip_code,
			char *self_phone_number,
			char *self_email_address )
{
	LATEX_INVOICE *h =
		(LATEX_INVOICE *)
			calloc( 1, sizeof( LATEX_INVOICE ) );

	h->invoice_date = invoice_date;

	h->invoice_self =
		latex_invoice_self_new(
			self_full_name,
			self_street_address,
			self_city,
			self_state_code,
			self_zip_code,
			self_phone_number,
			self_email_address );

	h->invoice_line_item_list = list_new();

	return h;
}

LATEX_INVOICE_SELF *
	latex_invoice_self_new(
			char *full_name,
			char *street_address,
			char *city,
			char *state_code,
			char *zip_code,
			char *phone_number,
			char *email_address )
{
	LATEX_INVOICE_SELF *h =
		(LATEX_INVOICE_SELF *)
			calloc( 1, sizeof( LATEX_INVOICE_SELF ) );

	h->full_name = full_name;
	h->street_address = street_address;
	h->city = city;
	h->state_code = state_code;
	h->zip_code = zip_code;
	h->phone_number = phone_number;
	h->email_address = email_address;

	return h;
}

LATEX_INVOICE_CUSTOMER *
	latex_invoice_customer_new(
			char *full_name,
			char *street_address,
			char *city,
			char *state_code,
			char *zip_code,
			char *phone_number,
			char *email_address )
{
	LATEX_INVOICE_CUSTOMER *h =
		(LATEX_INVOICE_CUSTOMER *)
			calloc( 1, sizeof( LATEX_INVOICE_CUSTOMER ) );

	h->full_name = full_name;
	h->street_address = street_address;
	h->city = city;
	h->state_code = state_code;
	h->zip_code = zip_code;
	h->phone_number = phone_number;
	h->email_address = email_address;
	return h;
}

LIST *latex_invoice_line_item_set(
			LIST *invoice_line_item_list,
			char *item_key,
			char *item_name,
			double quantity,
			double retail_price,
			double discount_amount,
			double extended_price )
{
	LATEX_INVOICE_LINE_ITEM *latex_invoice_line_item;

	latex_invoice_line_item =
		latex_invoice_line_item_new(
			item_key,
			item_name,
			quantity,
			retail_price,
			discount_amount,
			extended_price );

	list_set( invoice_line_item_list, latex_invoice_line_item );
	return invoice_line_item_list;
}

LATEX_INVOICE_LINE_ITEM *latex_invoice_line_item_new(
			char *item_key,
			char *item_name,
			double quantity,
			double retail_price,
			double discount_amount,
			double extended_price )
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
	h->item_name = item_name;
	h->quantity = quantity;
	h->retail_price = retail_price;
	h->discount_amount = discount_amount;
	h->extended_price = extended_price;
	return h;
}

void latex_invoice_output_header( FILE *output_stream )
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
			LIST *extra_label_list )
{
	if ( title && *title )
	{
		fprintf(output_stream,
"\\begin{center}{\\Large \\bf %s} \\end{center}\n",
	 	 	title );
	}

	if ( invoice_key && *invoice_key )
	{
		fprintf(output_stream,
"\\begin{center}{\\large For %s} \\end{center}\n",
		 	invoice_key );
	}

	if ( logo_filename
	&&   *logo_filename
	&&   timlib_file_exists( logo_filename ) )
	{
		fprintf(output_stream,
"\\begin{center}\n"
"\\includegraphics{%s}\n"
"\\end{center}\n",
		 	logo_filename );
	}

	if ( invoice_date && *invoice_date )
	{
		fprintf(output_stream,
"\\begin{center}{%s} \\end{center}\n",
	 	 	invoice_date );
	}

	if ( latex_invoice_self->city && *latex_invoice_self->city )
	{
		fprintf( output_stream,
"\\begin{tabular}[t]{p{5.0in}r}\n"
"%s & %s \\\\\n"
"%s, %s %s & %s \\\\\n"
"\\end{tabular}\n\n",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
	 		appaserver_escape_street_address(
				latex_invoice_self->street_address ),
			(latex_invoice_self->phone_number)
				? latex_invoice_self->phone_number
				: "",
	 		latex_invoice_self->city,
	 		latex_invoice_self->state_code,
	 		latex_invoice_self->zip_code,
	 		(latex_invoice_self->email_address)
	 			? latex_invoice_self->email_address
				: "" );
	}
	else
	{
		fprintf( output_stream,
"\\begin{tabular}[t]{l}\n"
"%s\\\\\n"
"\\end{tabular}\n\n",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
	 		appaserver_escape_street_address(
				latex_invoice_self->street_address ) );
	}

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
		 	latex_invoice_customer->full_name,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
	 		appaserver_escape_street_address(
				latex_invoice_customer->street_address ),
		 	latex_invoice_customer->city,
		 	latex_invoice_customer->state_code,
		 	latex_invoice_customer->zip_code );
	}
	else
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& %s\n"
"\\end{tabular}\n\n",
		 	latex_invoice_customer->full_name );
	}

	if ( customer_service_key
	&&   *customer_service_key )
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& \\bf %s\n"
"\\end{tabular}\n\n",
		 	customer_service_key );
	}

	if ( list_rewind( extra_label_list ) )
	{
		do {
			fprintf( output_stream,
"\\begin{tabular}{l}\n"
"%s\n"
"\\end{tabular}\n\n",
		 		(char *)list_get( extra_label_list ) );
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

	fprintf( output_stream,
		 "%s\n",
		 /* Returns heap memory */
		 /* ------------------- */
		 latex_invoice_header_format_line(
			exists_discount_amount ) );

	if ( line_item_key_heading
	&&   *line_item_key_heading )
	{
		fprintf( output_stream, "\\bf %s & ", line_item_key_heading );
	}

	fprintf( output_stream,
		 "%s\n",
		 /* Returns heap memory */
		 /* ------------------- */
		 /* Returns heap memory */
		 /* ------------------- */
		 latex_invoice_header_text_line(
			exists_discount_amount,
			omit_money ) );

	fprintf( output_stream,
"\\hline \\hline\n" );

}

void latex_invoice_output_invoice_footer(
			FILE *output_stream,
			double extended_price_total,
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
		 	string_commas_dollar( sales_tax ) );

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
		 	string_commas_dollar( shipping_charge ) );

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
		 	string_commas_dollar( total_payment ) );

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
		 string_commas_dollar(	extended_price_total +
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

void latex_invoice_output_line_item_list(
			FILE *output_stream,
			LIST *invoice_line_item_list,
			boolean exists_discount_amount,
			boolean omit_money,
			int quantity_decimal_places )
{
	LATEX_INVOICE_LINE_ITEM *line_item;
	char buffer[ 256 ];
	char dollar_string[ 3 ];

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

		if ( !omit_money )
		{
			fprintf( output_stream,
"%s & %.*lf & %s%.2lf",
			 	format_initial_capital(
					buffer, line_item->item_name ),
				quantity_decimal_places,
			 	line_item->quantity,
			 	dollar_string,
			 	line_item->retail_price );

			if ( exists_discount_amount )
			{
				fprintf( output_stream,
"& %s%.2lf",
			 		 dollar_string,
			 		 line_item->discount_amount );
			}

			fprintf( output_stream,
"& %s%s \\\\\n",
			 	 dollar_string,
			 	 timlib_place_commas_in_money(
					line_item->extended_price ) );

			if ( *dollar_string ) *dollar_string = '\0';
		}
		else
		{
			fprintf( output_stream,
"%s & %.*lf \\\\\n",
			 	 format_initial_capital(
					buffer, line_item->item_name ),
				 quantity_decimal_places,
			 	 line_item->quantity );
		}

	} while( list_next( invoice_line_item_list ) );
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

boolean latex_invoice_exists_discount_amount(
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

int latex_invoice_quantity_decimal_places(
			boolean each_quantity_integer,
			int default_quantity_decimal_places )
{
	if ( each_quantity_integer )
		return 0;
	else
		return default_quantity_decimal_places;
}

char *latex_invoice_header_text_line(
			boolean exists_discount_amount,
			boolean omit_money )
{
	char text_line[ 1024 ];

	if ( !omit_money )
	{
		if ( exists_discount_amount )
		{
			sprintf( text_line,
"\\bf Description & \\bf Quantity & \\bf Retail Price & \\bf Discount & \\bf Extended \\\\[0.5ex]\n" );
		}
		else
		{
			sprintf( text_line,
"\\bf Description & \\bf Quantity & \\bf Retail Price & \\bf Extended \\\\[0.5ex]\n" );
		}
	}
	else
	{
		sprintf( text_line,
"\\bf Description & \\bf Quantity \\\\[0.5ex]\n" );
	}

	return strdup( text_line );
}

char *latex_invoice_header_format_line(
			boolean exists_discount_amount )
{
	char format_line[ 1024 ];

	if ( exists_discount_amount )
	{
		sprintf( format_line,
"\\begin{longtable}[t]{|p{3.2in}|r|r|r|r|} \\hline\n"
"\\hline\n" );
	}
	else
	{
		sprintf( format_line,
"\\begin{longtable}[t]{|p{4.2in}|r|r|r|} \\hline\n"
"\\hline\n" );
	}

	return strdup( format_line );
}

