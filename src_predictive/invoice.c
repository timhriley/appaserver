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
		char *customer_full_name,
		char *customer_street_address,
		enum invoice_enum invoice_enum,
		LIST *invoice_line_item_list )
{
	INVOICE *invoice;

	if ( !invoice_date_time_string
	||   !customer_full_name
	||   !customer_street_address )
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

	invoice->invoice_line_item_list = invoice_line_item_list;

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
				customer_full_name,
				customer_street_address,
				1 /* fetch_entity_boolean */,
				1 /* fetch_payable_balance_boolean */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"customer_fetch(%s,%s) returned empty.",
			customer_full_name,
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

	invoice->title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		invoice_title(
			customer_full_name,
			invoice_enum,
			invoice->use_key );

	invoice->date_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		invoice_date_string(
			invoice_date_time_string );

	invoice->amount_due_label =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		invoice_amount_due_label(
			invoice_enum );

	invoice->invoice_summary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		invoice_summary_new(
			invoice_line_item_list,
			invoice->customer->journal_payable_balance );

	return invoice;
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
		char *item_key,
		char *description /* stack memory */,
		double quantity,
		double retail_price,
		double discount_amount )
{
	INVOICE_LINE_ITEM *invoice_line_item;

	invoice_line_item = invoice_line_item_calloc();

	invoice_line_item->item_key = item_key;

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
		boolean description_boolean,
		boolean discount_boolean,
		double extended_total,
		double customer_payable_balance,
		double invoice_summary_amount_due,
		char *amount_due_label,
		FILE *output_stream )
{
	fprintf( output_stream, "<tr><td>Extended Total" );

	if ( description_boolean )
		fprintf( output_stream, "<td>" );

	/* Placeholder for quantity */
	/* ------------------------ */
	fprintf( output_stream, "<td>" );

	if ( discount_boolean )
		fprintf( output_stream, "<td>" );

	fprintf(output_stream,
"<td align=right>$%s\n"
"</table>\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
		 	extended_total ) );

	if ( !float_virtually_same( customer_payable_balance, 0.0 ) )
	{
		fprintf( output_stream, "<tr><td>Payment (Thank you)" );

		if ( description_boolean )
			fprintf( output_stream, "<td>" );

		/* Placeholder for quantity */
		/* ------------------------ */
		fprintf( output_stream, "<td>" );

		if ( discount_boolean )
			fprintf( output_stream, "<td>" );

		fprintf(output_stream,
"<td align=right>$%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_money(
			 	customer_payable_balance ) );
	}

	fprintf(output_stream,
		"<tr><td><big>%s</big>",
		amount_due_label );

	if ( description_boolean )
		fprintf( output_stream, "<td>" );

	/* Placeholder for quantity */
	/* ------------------------ */
	fprintf( output_stream, "<td>" );

	if ( discount_boolean )
		fprintf( output_stream, "<td>" );

	fprintf(output_stream,
"<td align=right><big>$%s</big>\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money( invoice_summary_amount_due ) );
}

void invoice_html_output_line_item_list(
		enum invoice_enum invoice_enum,
		LIST *invoice_line_item_list,
		boolean description_boolean,
		boolean discount_boolean,
		int quantity_decimal_count,
		FILE *output_stream )
{
	INVOICE_LINE_ITEM *line_item;
	char buffer[ 256 ];
	char dollar_string[ 2 ] = {0};

	*dollar_string = '$';

	if ( list_rewind( invoice_line_item_list ) )
	do {
		line_item = list_get( invoice_line_item_list );

		fprintf( output_stream, "<tr>" );

		fprintf(output_stream,
			"<td>%s",
			string_initial_capital(
				buffer,
				line_item->item_key ) );

		if ( description_boolean )
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
				quantity_decimal_count ) );

		if ( invoice_enum != invoice_workorder )
		{
			fprintf(output_stream,
				"<td align=right>%s%s",
			 	dollar_string,
				/* --------------------- */
			 	/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					line_item->retail_price ) );

			if ( discount_boolean )
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

	} while( list_next( invoice_line_item_list ) );
}

double invoice_line_item_extended_total( LIST *invoice_line_item_list )
{
	INVOICE_LINE_ITEM *line_item;
	double total = 0.0;

	if ( list_rewind( invoice_line_item_list ) )
	do {
		line_item = list_get( invoice_line_item_list );

		total += line_item->extended_price;

	} while( list_next( invoice_line_item_list ) );

	return total;
}

boolean invoice_line_item_description_boolean( LIST *invoice_line_item_list )
{
	INVOICE_LINE_ITEM *line_item;

	if ( list_rewind( invoice_line_item_list ) )
	do {
		line_item = list_get( invoice_line_item_list );

		if ( line_item->description ) return 1;

	} while( list_next( invoice_line_item_list ) );

	return 0;
}

boolean invoice_line_item_discount_boolean(
		LIST *invoice_line_item_list )
{
	INVOICE_LINE_ITEM *line_item;

	if ( list_rewind( invoice_line_item_list ) )
	do {
		line_item = list_get( invoice_line_item_list );

		if ( line_item->discount_amount ) return 1;

	} while( list_next( invoice_line_item_list ) );

	return 0;
}

void invoice_html_output(
		INVOICE *invoice,
		FILE *output_stream )
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
	||   !invoice->invoice_summary )
	{
		char message[ 128 ];

		sprintf(message, "invoice is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	invoice_html_output_style( output_stream );

	invoice_html_output_table_open(
		invoice->title,
		output_stream );

	invoice_html_output_self(
		invoice->date_string,
		invoice->entity_self,
		invoice->
			invoice_summary->
			invoice_line_item_description_boolean,
		invoice->
			invoice_summary->
			invoice_line_item_discount_boolean,
		output_stream );

	invoice_html_output_customer(
		invoice->customer,
		invoice->
			invoice_summary->
			invoice_line_item_description_boolean,
		invoice->
			invoice_summary->
			invoice_line_item_discount_boolean,
		output_stream );

	invoice_html_output_table_header(
		invoice->invoice_enum,
		invoice->
			invoice_summary->
			invoice_line_item_description_boolean,
		invoice->
			invoice_summary->
			invoice_line_item_discount_boolean,
		output_stream );

	invoice_html_output_line_item_list(
		invoice->invoice_enum,
		invoice->invoice_line_item_list,
		invoice->
			invoice_summary->
			invoice_line_item_description_boolean,
		invoice->
			invoice_summary->
			invoice_line_item_discount_boolean,
		invoice->
			invoice_summary->
			invoice_line_item_quantity_decimal_count,
		output_stream );

	invoice_html_output_footer(
		invoice->
			invoice_summary->
			invoice_line_item_description_boolean,
		invoice->
			invoice_summary->
			invoice_line_item_discount_boolean,
		invoice->
			invoice_summary->
			invoice_line_item_extended_total,
		invoice->customer->journal_payable_balance
			/* customer_payable_balance */,
		invoice->invoice_summary->amount_due,
		invoice->amount_due_label,
		output_stream );

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

int invoice_line_item_quantity_decimal_count(
		LIST *invoice_line_item_list )
{
	INVOICE_LINE_ITEM *invoice_line_item;

	if ( list_rewind( invoice_line_item_list ) )
	do {
		invoice_line_item =
			list_get(
				invoice_line_item_list );

		if ( !float_is_integer( invoice_line_item->quantity ) )
		{
			return
			INVOICE_LINE_ITEM_QUANTITY_DECIMAL_COUNT;
		}

	} while ( list_next( invoice_line_item_list ) );

	return 0;
}

void invoice_html_output_table_open(
		char *invoice_title,
		FILE *output_stream )
{
	char title_buffer[ 128 ];

	if ( !invoice_title )
	{
		char message[ 128 ];

		sprintf(message, "invoice_title is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy( title_buffer, invoice_title );
	string_search_replace( title_buffer, " ", "&nbsp;" );

	fprintf(output_stream,
"<table border=1>						\n"
"<caption align=top><big><bold>%s				\n"
"</bold></big></caption>					\n",
		title_buffer );
}

char *invoice_title(
		char *customer_full_name,
		enum invoice_enum invoice_enum,
		char *invoice_use_key )
{
	static char title[ 256 ];
	char *prompt;

	if ( !customer_full_name
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

	if ( invoice_enum == invoice_workorder )
		prompt = "Workorder";
	else
	if ( invoice_enum == invoice_estimate )
		prompt = "Estimate";
	else
	/* Must be invoice_enum == invoice_due */
		prompt = "Invoice Due";

	snprintf(
		title,
		sizeof ( title ),
		"%s %s for %s",
		prompt,
		invoice_use_key,
		customer_full_name );

	return title;
}

void invoice_html_output_self(
		char *invoice_date_string,
		ENTITY_SELF *entity_self,
		boolean description_boolean,
		boolean discount_boolean,
		FILE *output_stream )
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

	if ( description_boolean ) column_span++;
	if ( discount_boolean ) column_span++;

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
	       	CUSTOMER *customer,
		boolean description_boolean,
		boolean discount_boolean,
		FILE *output_stream )
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

	if ( description_boolean ) column_span++;
	if ( discount_boolean ) column_span++;

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
		 	(customer->entity->state_code)
		 		? customer->entity->state_code
				: "EMPTY",
		 	(customer->entity->zip_code)
		 		? customer->entity->zip_code
				: "EMPTY" );
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
}

void invoice_html_output_table_header(
		enum invoice_enum invoice_enum,
		boolean description_boolean,
		boolean discount_boolean,
		FILE *output_stream )
{
	fprintf( output_stream, "<tr>" );

	fprintf( output_stream, "<th>Item" );

	if ( description_boolean ) fprintf( output_stream, "<th>Description" );

	fprintf( output_stream, "<th>Quantity" );

	if ( invoice_enum != invoice_workorder )
	{
		if ( discount_boolean )
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

char *invoice_amount_due_label( enum invoice_enum invoice_enum )
{
	if ( invoice_enum == invoice_workorder )
		return "Workorder";
	else
	if ( invoice_enum == invoice_estimate )
		return "Estimated Due";
	else
	/* ------------------- */
	/* Must be invoice_due */
	/* ------------------- */
		return "Amount Due";
}

double invoice_summary_invoice_amount(
		double extended_total,
		double discount_total )
{
	return
	extended_total -
	discount_total;
}

double invoice_summary_amount_due(
		double customer_payable_balance,
		double invoice_amount )
{
	return
	invoice_amount -
	customer_payable_balance;
}

INVOICE_SUMMARY *invoice_summary_new(
		LIST *invoice_line_item_list,
		double customer_payable_balance )
{
	INVOICE_SUMMARY *invoice_summary;

	invoice_summary = invoice_summary_calloc();

	invoice_summary->invoice_line_item_description_boolean =
		invoice_line_item_description_boolean(
			invoice_line_item_list );

	invoice_summary->invoice_line_item_discount_boolean =
		invoice_line_item_discount_boolean(
			invoice_line_item_list );

	invoice_summary->invoice_line_item_quantity_decimal_count =
		invoice_line_item_quantity_decimal_count(
			invoice_line_item_list );

	invoice_summary->invoice_line_item_extended_total =
		invoice_line_item_extended_total(
			invoice_line_item_list );

	invoice_summary->invoice_line_item_discount_total =
		invoice_line_item_discount_total(
			invoice_line_item_list );

	invoice_summary->invoice_amount =
		invoice_summary_invoice_amount(
			invoice_summary->invoice_line_item_extended_total,
			invoice_summary->invoice_line_item_discount_total );

	invoice_summary->amount_due =
		invoice_summary_amount_due(
			customer_payable_balance,
			invoice_summary->invoice_amount );

	return invoice_summary;
}

INVOICE_SUMMARY *invoice_summary_calloc( void )
{
	INVOICE_SUMMARY *invoice_summary;

	if ( ! ( invoice_summary =
			calloc( 1, sizeof ( INVOICE_SUMMARY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return invoice_summary;
}

double invoice_line_item_discount_total( LIST *line_item_list )
{
	INVOICE_LINE_ITEM *line_item;
	double discount_total = 0.0;

	if ( list_rewind( line_item_list ) )
	do {
		line_item = list_get( line_item_list );
		discount_total += line_item->discount_amount;

	} while( list_next( line_item_list ) );

	return discount_total;
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

	snprintf(
		system_string,
		sizeof ( system_string ),
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
	char item_key[ 256 ];
	char description[ 1024 ];
	char quantity[ 128 ];
	char retail_price[ 128 ];
	char discount_amount[ 128 ];

	if ( !input ) return NULL;

	/* See invoice_select.sh */
	/* --------------------- */
	piece( item_key, SQL_DELIMITER, input, 0 );
	piece( description, SQL_DELIMITER, input, 1 );
	piece( quantity, SQL_DELIMITER, input, 2 );
	piece( retail_price, SQL_DELIMITER, input, 3 );
	piece( discount_amount, SQL_DELIMITER, input, 4 );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	invoice_line_item_new(
		strdup( item_key ),
		description /* stack memory */,
		atof( quantity ),
		atof( retail_price ),
		atof( discount_amount ) );
}

