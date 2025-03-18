/* $APPASERVER_HOME/library/invoice_latex.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "file.h"
#include "float.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "statement.h"
#include "invoice_latex.h"

INVOICE_LATEX *invoice_latex_new(
		char *application_name,
		char *process_name,
		char *data_directory,
		char *statement_logo_filename,
		INVOICE *invoice )
{
	INVOICE_LATEX *invoice_latex;

	if ( !application_name
	||   !process_name
	||   !invoice
	||   !invoice->entity_self
	||   !invoice->customer
	||   !invoice->invoice_data )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	invoice_latex = invoice_latex_calloc();

	invoice_latex->invoice = invoice;

	invoice_latex->document_header =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		invoice_latex_document_header();

	invoice_latex->statement_link =
		statement_link_new(
			application_name,
			process_name,
			data_directory,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			getpid() /* process_id */ );

	if ( !invoice_latex->statement_link )
	{
		char message[ 256 ];

		sprintf(message,
			"statement_line_new(%s,%s,%s) returned empty.",
			application_name,
			process_name,
			data_directory );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	invoice_latex->latex =
		latex_new(
			invoice_latex->
				statement_link->
				tex_filename,
			invoice_latex->
				statement_link->
				appaserver_link_working_directory,
			0 /* not statement_pdf_landscape_boolean */,
			statement_logo_filename );

	if ( !invoice_latex->latex )
	{
		char message[ 256 ];

		sprintf(message,
			"latex_new(%s,%s) returned empty.",
			invoice_latex->
				statement_link->
				tex_filename,
			invoice_latex->
				statement_link->
				appaserver_link_working_directory );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	invoice_latex->invoice_latex_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		invoice_latex_table_new(
			invoice->caption,
			invoice->entity_self,
			invoice->customer,
			invoice->invoice_data );

	list_set(
		invoice_latex->latex->table_list,
		invoice_latex->invoice_latex_table->latex_table );

	invoice_latex->statement_caption_logo_filename =
		statement_caption_logo_filename(
			STATEMENT_LOGO_FILENAME_KEY );

	return invoice_latex;
}

INVOICE_LATEX *invoice_latex_calloc( void )
{
	INVOICE_LATEX *invoice_latex;

	if ( ! ( invoice_latex = calloc( 1, sizeof ( INVOICE_LATEX ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return invoice_latex;
}

INVOICE_LATEX_TABLE *invoice_latex_table_new(
		char *invoice_caption,
		ENTITY_SELF *entity_self,
		CUSTOMER *customer,
		INVOICE_DATA *invoice_data )
{
	INVOICE_LATEX_TABLE *invoice_latex_table;

	if ( !invoice_caption
	||   !entity_self
	||   !customer
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

	invoice_latex_table = invoice_latex_table_calloc();

	invoice_latex_table->
		self_invoice_latex_entity =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			invoice_latex_entity_new(
				(char *)0 /* label */,
				entity_self->entity );

	invoice_latex_table->
		customer_invoice_latex_entity =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			invoice_latex_entity_new(
				customer_label,
				customer->entity );

	invoice_latex_table->latex_table =
		latex_table_new(
			invoice_caption );

	invoice_latex_table->latex_table->column_list =
		invoice_latex_table_column_list(
			invoice_data );

	return invoice_latex_table;
}

LIST *invoice_latex_table_column_list( INVOICE_DATA *invoice_data )
{
	LIST *column_list;

	if ( !invoice_data )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"invoice_data is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return column_list;
}

INVOICE_LATEX_TABLE *invoice_latex_table_calloc( void )
{
	INVOICE_LATEX_TABLE *invoice_latex_table;

	if ( ! ( invoice_latex_table =
			calloc( 1, sizeof ( INVOICE_LATEX_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return invoice_latex_table;
}

LIST *invoice_latex_table_row_list(
		INVOICE_DATA *invoice_data,
		LIST *latex_table_column_list )
{
}

char *invoice_latex_table_display(
		char *latex_table_title,
		INVOICE_LATEX_ENTITY *
			self_invoice_latex_entity,
		INVOICE_LATEX_ENTITY *
			customer_invoice_latex_entity,
		LIST *latex_table_row_list /* freed */,
		INVOICE_LATEX_SUMMARY *invoice_latex_summary )
{
	char *row_list_display;
	char display[ 4096 ];

	if ( !latex_table_title
	||   !self_invoice_latex_entity
	||   !customer_invoice_latex_entity
	||   !list_length( latex_table_row_list )
	||   !invoice_latex_summary )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		latex_row_list_display(
			latex_table_row_list /* freed */ );

	snprintf(
		display,
		sizeof ( display ),
		"%s\n%s\n%s\n%s\n%s\n",
		latex_table_title,
		self_invoice_latex_entity->display,
		customer_invoice_latex_entity->display,
		row_list_display,
		invoice_latex_summary->display );

	free( row_list_display );

	return strdup( display );
}

void invoice_latex_output( INVOICE_LATEX *invoice_latex )
{
	FILE *output_file;

	if ( !invoice_latex
	||   !invoice_latex->invoice
	||   !invoice_latex->statement_link
	||   !invoice_latex->invoice->entity_self
	||   !invoice_latex->invoice->customer
	||   !invoice_latex->invoice->invoice_data )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			invoice_latex->
				statement_link->
				tex_filename );

	fprintf(output_file,
		"%s\n",
		invoice_latex->document_header );

	invoice_latex_output_title(
		invoice_latex->invoice->caption,
		invoice_latex->statement_caption_logo_filename,
		invoice_latex->invoice->date_string,
		output_file );

	fclose( output_file );

	latex_tex2pdf(
		invoice_latex->
			statement_link->
			tex_filename,
		invoice_latex->
			appaserver_link_working_directory );

	printf( "%s\n",
		invoice_latex->pdf_anchor_html );
}

char *invoice_latex_document_header( void )
{
	return
	"\\documentclass{letter}\n"
	"\\usepackage{longtable}\n"
	"\\usepackage{graphics}\n"
	"\\usepackage[	margin=1in,\n"
	"		nohead]{geometry}\n"
	"\\begin{document}\n";
}

char *invoice_latex_title_string(
		char *invoice_caption,
		char *logo_filename,
		char *date_string )
{
	char title[ 1024 ];
	char *ptr = title;

	if ( !invoice_caption
	||   !date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
"\\begin{center}{\\Large \\bf %s} \\end{center}\n",
	 	invoice_caption );

	if ( file_exists_boolean( logo_filename ) )
	{
		ptr += sprintf(
			ptr,
"\\begin{center}\n"
"\\includegraphics{%s}\n"
"\\end{center}\n",
		 	logo_filename );
	}

	ptr += sprintf(
		ptr,
"\\begin{center}{%s} \\end{center}\n",
		date_string );

	return strdup( title );
}

char *invoice_latex_self_string( ENTITY_SELF *entity_self )
{
	char self_string[ 1024 ];
	char *ptr = self_string;
	char *escape_street_address;

	if ( !entity_self
	||   !entity_self->entity )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity_self is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	escape_street_address =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_escape_array(
			INVOICE_LATEX_ESCAPE_ARRAY,
			entity->street_address );

	if ( entity_self->entity->city
	&&   entity_self->entity->state_code
	&&   entity_self->entity->zip_code )
	{
		ptr += sprintf(
			ptr,
"\\begin{tabular}[t]{p{5.0in}r}\n"
"%s & %s \\\\\n"
"%s, %s %s & %s \\\\\n"
"\\end{tabular}\n\n",
			escape_street_address,
			(entity_self->phone_number)
				? entity_self->phone_number
				: "",
	 		entity_self->city,
	 		entity_self->state_code,
	 		entity_self->zip_code,
	 		(entity_self->email_address)
	 			? entity_self->email_address
				: "" );
	}
	else
	{
		fprintf( output_stream,
"\\begin{tabular}[t]{l}\n"
"%s\\\\\n"
"\\end{tabular}\n\n",
			escape_street_address );
	}

	free( escape_street_address );

	return strdup( self_string );
}

char *invoice_latex_customer_string(
			char *invoice_key,
			char *invoice_date,
			char *line_item_key_heading,
			LATEX_INVOICE_SELF *invoice_self,
      			LATEX_INVOICE_CUSTOMER *invoice_customer,
			char *customer_service_key,
			boolean exists_discount_amount,
			char *title,
			boolean omit_money,
			char *logo_filename,
			char *instructions,
			LIST *extra_label_list,
			char *first_column_label,
			char *last_column_label,
			char *customer_label )
{
	fprintf( output_stream,
"\\begin{tabular}[t]{l}\n"
"\\bf %s:\n"
"\\end{tabular}\n\n",
		 customer_label );

	if ( invoice_customer->city
	&&   *invoice_customer->city )
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& %s \\\\\n"
"& %s \\\\\n"
"& %s, %s %s\n"
"\\end{tabular}\n\n",
		 	invoice_customer->full_name,
	 		string_escape_full(
				escape_street_address,
				invoice_customer->street_address ),
		 	invoice_customer->city,
		 	invoice_customer->state_code,
		 	invoice_customer->zip_code );
	}
	else
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& %s\n"
"\\end{tabular}\n\n",
		 	invoice_customer->full_name );
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
}

char *invoice_latex_extra_string(
			char *invoice_key,
			char *invoice_date,
			char *line_item_key_heading,
			LATEX_INVOICE_SELF *invoice_self,
      			LATEX_INVOICE_CUSTOMER *invoice_customer,
			char *customer_service_key,
			boolean exists_discount_amount,
			char *title,
			boolean omit_money,
			char *logo_filename,
			char *instructions,
			LIST *extra_label_list,
			char *first_column_label,
			char *last_column_label,
			char *customer_label )
{
	if ( list_rewind( extra_label_list ) )
	do {
		fprintf( output_stream,
"\\begin{tabular}{l}\n"
"%s\n"
"\\end{tabular}\n\n",
	 		(char *)list_get( extra_label_list ) );
	} while( list_next( extra_label_list ) );

	if ( instructions && *instructions )
	{
		fprintf( output_stream,
"\\begin{tabular}{p{6.5in}}\n"
"%s\n"
"\\end{tabular}\n\n",
		 	instructions );
	}

}

char *invoice_latex_table_header(
		char *invoice_key,
		char *invoice_date,
		char *line_item_key_heading,
		LATEX_INVOICE_SELF *invoice_self,
      		LATEX_INVOICE_CUSTOMER *invoice_customer,
		char *customer_service_key,
		boolean exists_discount_amount,
		char *title,
		boolean omit_money,
		char *logo_filename,
		char *instructions,
		LIST *extra_label_list,
		char *first_column_label,
		char *last_column_label,
		char *customer_label )
{

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
		 /* ------------------- */
		 /* Returns heap memory */
		 /* ------------------- */
		 latex_invoice_header_text_line(
			exists_discount_amount,
			omit_money,
			first_column_label,
			last_column_label ) );

	fprintf( output_stream,
"\\hline \\hline\n" );

}

char *invoice_latex_table_footer(
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
		fprintf( output_stream, "Payment (thank you) &" );

		/* Quantity column */
		/* --------------- */
		fprintf( output_stream, "&" );

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

	/* Quantity column */
	/* --------------- */
	fprintf( output_stream, "&" );

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

char *invoice_latex_summation_string(
		double extended_price_total,
		double total_payment,
		double amount_due )
{
	if ( total_payment )
	{
		fprintf( output_stream,
"Payment (thank you) & -%s \\\\\n",
		 	string_commas_dollar( total_payment ) );

	}

	fprintf( output_stream, "\\bf Amount Due &" );

	fprintf( output_stream,
"\\bf \\$%s \\\\\n",
		 string_commas_dollar(	extended_price_total -
					total_payment ) );
}

char *invoice_latex_footer(
		boolean customer_signature_boolean )
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

char *invoice_latex_line_item_list_string(
		LIST *invoice_line_item_list )
{
	LATEX_INVOICE_LINE_ITEM *line_item;
	char buffer[ 256 ];
	char dollar_string[ 3 ];

	strcpy( dollar_string, "\\$" );

	if ( !list_rewind( invoice_line_item_list ) ) return;

	do {
		line_item = list_get_pointer( invoice_line_item_list );

		fprintf(output_stream,
"%s & %s%s \\\\\n",
		 	format_initial_capital(
				buffer, line_item->item_name ),
		 	dollar_string,
			string_commas_money(
				line_item->extended_price ) );

		if ( *dollar_string ) *dollar_string = '\0';

	} while( list_next( invoice_line_item_list ) );
}

void latex_invoice_output_line_item_list(
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
			 	 string_commas_money(
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

		if ( !float_integer_boolean(
			line_item->quantity ) )
		{
			return 0;
		}

	} while( list_next( invoice_line_item_list ) );

	return 1;
}

boolean invoice_latex_discount_amount_boolean(
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
		boolean discount_amount_boolean,
		boolean omit_money_boolean,
		char *first_column_label,
		char *last_column_label )
{
	char text_line[ 1024 ];

	if ( !omit_money )
	{
		if ( exists_discount_amount )
		{
			sprintf( text_line,
"\\bf %s & \\bf Quantity & \\bf Retail Price & \\bf Discount & \\bf %s \\\\[0.5ex]\n",
				 first_column_label,
				 last_column_label );
		}
		else
		{
			sprintf( text_line,
"\\bf %s & \\bf Quantity & \\bf Retail Price & \\bf %s \\\\[0.5ex]\n",
				 first_column_label,
				 last_column_label );
		}
	}
	else
	{
		sprintf( text_line,
"\\bf %s & \\bf Quantity \\\\[0.5ex]\n",
			 first_column_label );
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

char *invoice_latex_entity_label( char *label )
{
	char entity_label[ 1024 ];

	if ( !label )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		entity_label,
		sizeof ( label ),
		"\\begin{tablular}[t]{l}\n"
		"\\bf %s:\n"
		"\\end{tabular}\n",
		label );

	return strdup( entity_label );
}

char *invoice_latex_entity_city_state_zip(
		char *city,
		char *state_code,
		char *zip_code )
{
	char city_state_zip[ 1024 ];
	char *ptr = city_state_zip;

	if ( !city )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"city is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	ptr += sprintf(
		ptr,
		"%s",
		city );

	if ( state_code || zip_code )
	{
		ptr += sprintf( ptr, "," );
	}

	if ( state_code )
	{
		ptr += sprintf(
			ptr,
			" %s",
			state_code );
	}

	if ( zip_code )
	{
		ptr += sprintf(
			ptr,
			" %s",
			zip_code );
	}

	return strdup( city_state_zip );
}

char *invoice_latex_entity_string(
		char *entity_full_name,
		char *invoice_latex_entity_label,
		char *escape_street_address,
		char *invoice_latex_entity_city_state_zip )
{
	char string[ 2048 ];
	char *ptr = string;

	if ( !entity_full_name
	||   !escape_street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( latex_entity_label )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			latex_entity_label );
	}

	ptr += sprintf( ptr, "\\begin{tabular}{p{0.5in}l}\n" );

	ptr += sprintf(
		ptr,
		"& %s \\\\\n"
		"& %s \\\\\n",
		entity_full_name,
		escape_street_address );

	if ( latex_entity_city_state_zip )
	{
		ptr += sprintf(
			ptr,
			"& %s\n",
			latex_entity_city_state_zip );
	}

	sprintf( ptr, "\\end{tabular}\n" );

	return strdup( string );
}

INVOICE_LATEX_ENTITY *invoice_latex_entity_new(
		char *label,
		ENTITY *entity )
{
	INVOICE_LATEX_ENTITY *invoice_latex_entity;

	if ( !entity )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	invoice_latex_entity = invoice_latex_entity_calloc();

	if ( label )
	{
		invoice_latex_entity->label =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			invoice_latex_entity_label(
				label );
	}

	invoice_latex_entity->escape_street_address =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_escape_array(
			INVOICE_LATEX_ESCAPE_ARRAY,
			entity->street_address );

	if ( entity->city )
	{
		invoice_latex_entity->city_state_zip =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			invoice_latex_entity_city_state_zip(
				entity->city,
				entity->state_code,
				entity->zip_code );
	}

	invoice_latex_entity->string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_latex_entity_string(
			entity->full_name,
			latex_entity->label,
			invoice_latex_entity->escape_street_address,
			invoice_latex_entity->city_state_zip );

	return invoice_latex_entity;
}

INVOICE_LATEX_ENTITY *invoice_latex_entity_calloc( void )
{
	INVOICE_LATEX_ENTITY *invoice_latex_entity;

	if ( ! ( invoice_latex_entity =
			calloc( 1,
				sizeof ( INVOICE_LATEX_ENTITY ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return invoice_latex_entity;
}

#ifdef NOT_DEFINED
char *latex_invoice_header_school_format_line( void )
{
	char format_line[ 1024 ];

	sprintf( format_line,
"\\begin{longtable}[t]{|p{4.2in}|r|} \\hline\n"
"\\hline\n" );

	return strdup( format_line );
}

char *invoice_latex_school_header(
		char *invoice_date,
		LATEX_INVOICE_SELF *invoice_self,
		LATEX_INVOICE_CUSTOMER *invoice_customer,
		char *title,
		char *logo_filename )
{
	char escape_street_address[ 128 ];

	if ( title && *title )
	{
		fprintf(output_stream,
"\\begin{center}{\\Large \\bf %s} \\end{center}\n",
	 	 	title );
	}

	if ( logo_filename
	&&   *logo_filename
	&&   file_exists_boolean( logo_filename ) )
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

	if ( invoice_self->city && *invoice_self->city )
	{
		fprintf( output_stream,
"\\begin{tabular}[t]{p{5.0in}r}\n"
"%s & %s \\\\\n"
"%s, %s %s & %s \\\\\n"
"\\end{tabular}\n\n",
	 		string_escape_full(
				escape_street_address,
				invoice_self->street_address ),
			(invoice_self->phone_number)
				? invoice_self->phone_number
				: "",
	 		invoice_self->city,
	 		invoice_self->state_code,
	 		invoice_self->zip_code,
	 		(invoice_self->email_address)
	 			? invoice_self->email_address
				: "" );
	}
	else
	{
		fprintf( output_stream,
"\\begin{tabular}[t]{l}\n"
"%s\\\\\n"
"\\end{tabular}\n\n",
	 		string_escape_full(
				escape_street_address,
				invoice_self->street_address ) );
	}

	fprintf( output_stream,
"\\begin{tabular}[t]{l}\n"
"\\bf %s:\n"
"\\end{tabular}\n\n",
		 "Student" );

	if ( invoice_customer->city
	&&   *invoice_customer->city )
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& %s \\\\\n"
"& %s \\\\\n"
"& %s, %s %s\n"
"\\end{tabular}\n\n",
		 	invoice_customer->full_name,
	 		string_escape_full(
				escape_street_address,
				invoice_customer->street_address ),
		 	invoice_customer->city,
		 	invoice_customer->state_code,
		 	invoice_customer->zip_code );
	}
	else
	{
		fprintf( output_stream,
"\\begin{tabular}{p{0.5in}l}\n"
"& %s\n"
"\\end{tabular}\n\n",
		 	invoice_customer->full_name );
	}

	fprintf( output_stream,
		 "%s\n",
		 /* Returns heap memory */
		 /* ------------------- */
		 latex_invoice_header_school_format_line() );

	fprintf( output_stream,
		 "%s\n",
		 /* ------------------- */
		 /* Returns heap memory */
		 /* ------------------- */
		 latex_invoice_header_school_text_line(
			"Course",
			"Tuition" ) );

	fprintf( output_stream,
"\\hline \\hline\n" );

}

char *latex_invoice_header_school_text_line(
		char *first_column_label,
		char *last_column_label )
{
	char text_line[ 1024 ];

	sprintf( text_line,
"\\bf %s & \\bf %s \\\\[0.5ex]\n",
		 first_column_label,
		 last_column_label );

	return strdup( text_line );
}

#endif
