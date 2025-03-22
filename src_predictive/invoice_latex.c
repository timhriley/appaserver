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
		INVOICE *invoice )
{
	INVOICE_LATEX *invoice_latex;

	if ( !application_name
	||   !process_name
	||   !data_directory
	||   !invoice
	||   !invoice->entity_self
	||   !invoice->customer )
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

	invoice_latex->statement_caption_logo_filename =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		statement_caption_logo_filename(
			STATEMENT_LOGO_FILENAME_KEY );

	invoice_latex->latex =
		latex_new(
			invoice_latex->
				statement_link->
				tex_filename,
			invoice_latex->
				statement_link->
				appaserver_link_working_directory,
			0 /* not statement_pdf_landscape_boolean */,
			invoice_latex->statement_caption_logo_filename );

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
			invoice->invoice_enum,
			invoice->title,
			invoice->entity_self,
			invoice->customer,
			invoice->invoice_line_item_list,
			invoice->amount_due_label,
			invoice->invoice_summary );

	invoice_latex->footer =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_latex_footer(
			invoice->invoice_enum );

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
		enum invoice_enum invoice_enum,
		char *invoice_title,
		ENTITY_SELF *entity_self,
		CUSTOMER *customer,
		LIST *invoice_line_item_list,
		char *amount_due_label,
		INVOICE_SUMMARY *invoice_summary )
{
	INVOICE_LATEX_TABLE *invoice_latex_table;

	if ( !invoice_title
	||   !entity_self
	||   !customer
	||   !invoice_summary )
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
				entity_self->entity );

	invoice_latex_table->
		customer_invoice_latex_entity =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			invoice_latex_entity_new(
				customer->entity );

	invoice_latex_table->column_list =
		invoice_latex_table_column_list(
			invoice_enum,
			invoice_summary->
				invoice_line_item_description_boolean,
			invoice_summary->
				invoice_line_item_discount_boolean,
			invoice_summary->
				invoice_line_item_quantity_decimal_count );

	invoice_latex_table->row_list =
		invoice_latex_table_row_list(
			invoice_line_item_list,
			invoice_latex_table->column_list );

	invoice_latex_table->latex_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		latex_table_new(
			invoice_title,
			invoice_latex_table->column_list,
			invoice_latex_table->row_list );

	invoice_latex_table->invoice_latex_summary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		invoice_latex_summary_new(
			amount_due_label,
			invoice_summary->
				invoice_line_item_description_boolean,
			invoice_summary->
				invoice_line_item_discount_boolean,
			invoice_summary->
				invoice_line_item_extended_total,
			invoice_summary->
				invoice_line_item_discount_total,
			invoice_summary->invoice_amount,
			customer->journal_payable_balance,
			invoice_summary->amount_due );

	invoice_latex_table->display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_latex_table_display(
			invoice_latex_table->latex_table->caption_display,
			invoice_latex_table->
				self_invoice_latex_entity->
				display,
			invoice_latex_table->
				customer_invoice_latex_entity->
				display,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			latex_table_head_display(
				invoice_latex_table->
					latex_table->
					begin_longtable,
				invoice_latex_table->
					latex_table->
					latex_column_header_format_line,
				invoice_latex_table->
					latex_table->
					caption_display,
				invoice_latex_table->
					latex_table->
					latex_column_header_text_line,
				invoice_latex_table->
					latex_table->
					footer_display ),
			invoice_latex_table->
				latex_table->
				latex_row_list /* freed */,
			invoice_latex_table->
				latex_table->
				end_longtable,
			invoice_latex_table->
				invoice_latex_summary->
				display );

	return invoice_latex_table;
}

LIST *invoice_latex_table_column_list(
		enum invoice_enum invoice_enum,
		boolean description_boolean,
		boolean discount_boolean,
		int quantity_decimal_count )
{
	LIST *column_list = list_new();
	LATEX_COLUMN *latex_column;

	list_set(
		column_list,
		latex_column_new(
			"item_key" /* heading_string */,
			latex_column_text /* enum latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	if ( description_boolean )
	{
		list_set(
			column_list,
			latex_column_new(
				"description" /* heading_string */,
				latex_column_text /* enum latex_column_enum */,
				0 /* float_decimal_count */,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				invoice_latex_table_description_size(
					discount_boolean )
						/* paragraph_size */,
				0 /* not first_column_boolean */ ) );
	}

	list_set(
		column_list,
		latex_column_new(
			"quantity" /* heading_string */,
			latex_column_float /* enum latex_column_enum */,
			quantity_decimal_count
				/* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	if ( invoice_enum != invoice_workorder )
	{
		list_set(
			column_list,
			latex_column_new(
				"retail_price" /* heading_string */,
				latex_column_float /* enum latex_column_enum */,
				2 /* float_decimal_count */,
				(char *)0 /* paragraph_size */,
				0 /* not first_column_boolean */ ) );


		if ( discount_boolean )
		{
			latex_column =
				latex_column_new(
					"discount_amount" /* heading_string */,
					latex_column_float,
					2 /* float_decimal_count */,
					(char *)0 /* paragraph_size */,
					0 /* not first_column_boolean */ );

			latex_column->dollar_sign_boolean = 1;
			list_set( column_list, latex_column );
		}

		latex_column =
			latex_column_new(
				"extended_price" /* heading_string */,
				latex_column_float /* enum latex_column_enum */,
				2 /* float_decimal_count */,
				(char *)0 /* paragraph_size */,
				0 /* not first_column_boolean */ );

		latex_column->dollar_sign_boolean = 1;
		list_set( column_list, latex_column );
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
		LIST *invoice_line_item_list,
		LIST *invoice_latex_table_column_list )
{
	LIST *latex_row_list = list_new();
	int first_row_boolean = 1;
	INVOICE_LINE_ITEM *invoice_line_item;
	LIST *cell_list;
	LATEX_ROW *latex_row;

	if ( list_rewind( invoice_line_item_list ) )
	do {
		invoice_line_item =
			list_get(
				invoice_line_item_list );

		cell_list =
			invoice_latex_table_cell_list(
				invoice_line_item->item_key,
				invoice_line_item->description,
				invoice_line_item->quantity,
				invoice_line_item->retail_price,
				invoice_line_item->discount_amount,
				invoice_line_item->extended_price,
				invoice_latex_table_column_list,
				first_row_boolean );

		latex_row = latex_row_new( cell_list );
		list_set( latex_row_list, latex_row );
		first_row_boolean = 0;

	} while ( list_next( invoice_line_item_list ) );

	if ( !list_length( latex_row_list ) )
	{
		list_free( latex_row_list );
		latex_row_list = NULL;
	}

	return latex_row_list;
}

LIST *invoice_latex_table_cell_list(
		char *item_key,
		char *description,
		double quantity,
		double retail_price,
		double discount_amount,
		double extended_price,
		LIST *column_list,
		boolean first_row_boolean )
{
	LIST *cell_list = list_new();
	LATEX_COLUMN *latex_column;
	LATEX_CELL *latex_cell;

	if ( !item_key )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"item_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( ! ( latex_column =
			latex_column_seek(
				column_list,
				"item_key" ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"latex_column_seek(%s) returned empty.",
			"item_key" );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_cell =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		latex_cell_new(
			latex_column,
			first_row_boolean,
			item_key /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );

	list_set( cell_list, latex_cell );

	if ( ( latex_column = latex_column_seek(
		column_list,
		"description" ) ) )
	{
		latex_cell =
			latex_cell_new(
				latex_column,
				first_row_boolean,
				description /* datum */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ );

		list_set( cell_list, latex_cell );
	}

	if ( ! ( latex_column =
			latex_column_seek(
				column_list,
				"quantity" ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"latex_column_seek(%s) returned empty.",
			"quantity" );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_cell =
		/* Safely returns */
		/* -------------- */
		latex_cell_float_new(
			latex_column,
			first_row_boolean,
			quantity /* value */ );

	list_set( cell_list, latex_cell );

	if ( ! ( latex_column_seek(
			column_list,
			"retail_price" ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"latex_column_seek(%s) returned empty.",
			"retail_price" );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_cell =
		latex_cell_float_new(
			latex_column,
			first_row_boolean,
			retail_price /* value */ );

	list_set( cell_list, latex_cell );

	if ( ( latex_column =
			latex_column_seek(
				column_list,
				"discount_amount" ) ) )
	{
		latex_cell =
			latex_cell_float_new(
				latex_column,
				first_row_boolean,
				discount_amount /* value */ );

		list_set( cell_list, latex_cell );
	}

	if ( ! ( latex_column =
			latex_column_seek(
				column_list,
				"extended_price" ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"latex_column_seek(%s) returned empty.",
			"extended_price" );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	latex_cell =
		latex_cell_float_new(
			latex_column,
			first_row_boolean,
			extended_price /* value */ );

	list_set( cell_list, latex_cell );

	return cell_list;
}

char *invoice_latex_table_display(
		char *latex_table_caption_display,
		char *self_invoice_latex_entity_display,
		char *customer_invoice_latex_entity_display,
		char *latex_table_head_display,
		LIST *latex_row_list /* freed */,
		char *latex_table_end_longtable,
		char *invoice_latex_summary_display )
{
	char *row_list_display;

	/* Synchronize with latex_row_list_display() */
	/* ----------------------------------------- */
	char display[ STRING_770K ];

	if ( !latex_table_caption_display
	||   !self_invoice_latex_entity_display
	||   !customer_invoice_latex_entity_display
	||   !latex_table_head_display
	||   !latex_table_end_longtable
	||   !invoice_latex_summary_display )
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
			latex_row_list /* freed */ );

	snprintf(
		display,
		sizeof ( display ),
		"%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
		latex_table_caption_display,
		self_invoice_latex_entity_display,
		customer_invoice_latex_entity_display,
		latex_table_head_display,
		row_list_display,
		latex_table_end_longtable,
		invoice_latex_summary_display );

	free( row_list_display );

	return strdup( display );
}

void invoice_latex_output( INVOICE_LATEX *invoice_latex )
{
	FILE *output_file;

	if ( !invoice_latex
	||   !invoice_latex->statement_link
	||   !invoice_latex->invoice->customer )
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

	fprintf(output_file,
		"%s\n",
		invoice_latex->title );

	fprintf(output_file,
		"%s\n",
		invoice_latex->
			invoice_latex_table->
			display );

	fprintf(output_file,
		"%s\n",
		invoice_latex->footer );

	fclose( output_file );

	latex_tex2pdf(
		invoice_latex->
			statement_link->
			tex_filename,
		invoice_latex->
			statement_link->
			appaserver_link_working_directory );

	printf( "%s<br>%s\n",
		invoice_latex->
			statement_link->
			pdf_anchor_html,
		invoice_latex->
			statement_link->
			tex_anchor_html );
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

char *invoice_latex_title(
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

char *invoice_latex_footer( enum invoice_enum invoice_enum )
{
	char footer[ 1024 ];
	char *ptr = footer;

	if ( invoice_enum == invoice_workorder )
	{
		ptr += sprintf( ptr,
"\n\\begin{tabular}{lp{2.5in}lp{2in}}\n");

		ptr += sprintf( ptr,
"\\bf \\large{Customer Signature}  & \\line(1,0){175} &"
"\\bf \\large{Date}  & \\line(1,0){100} \\\\\n" );

		ptr += sprintf( ptr,
"\\end{tabular}\n" );
	}

	sprintf( ptr,
"\\end{document}\n" );

	return strdup( footer );
}

char *invoice_latex_entity_heading( char *full_name )
{
	char heading[ 1024 ];

	if ( !full_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		heading,
		sizeof ( heading ),
		"\\begin{tablular}[t]{l}\n"
		"\\bf %s:\n"
		"\\end{tabular}\n",
		full_name );

	return strdup( heading );
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

char *invoice_latex_entity_display(
		char *entity_full_name,
		char *invoice_latex_entity_heading,
		char *escape_street_address,
		char *invoice_latex_entity_city_state_zip )
{
	char display[ STRING_64K ];
	char *ptr = display;

	if ( !entity_full_name
	||   !invoice_latex_entity_heading
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

	ptr += sprintf(
		ptr,
		"%s\n",
		invoice_latex_entity_heading );

	ptr += sprintf( ptr, "\\begin{tabular}{p{0.5in}l}\n" );

	ptr += sprintf(
		ptr,
		"& %s \\\\\n"
		"& %s \\\\\n",
		entity_full_name,
		escape_street_address );

	if ( invoice_latex_entity_city_state_zip )
	{
		ptr += sprintf(
			ptr,
			"& %s\n",
			invoice_latex_entity_city_state_zip );
	}

	sprintf( ptr, "\\end{tabular}\n" );

	return strdup( display );
}

INVOICE_LATEX_ENTITY *invoice_latex_entity_new(
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

	invoice_latex_entity->heading =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_latex_entity_heading(
			entity->full_name );

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

	invoice_latex_entity->display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_latex_entity_display(
			entity->full_name,
			invoice_latex_entity->heading,
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

INVOICE_LATEX_SUMMARY *invoice_latex_summary_new(
		char *amount_due_label,
		boolean description_boolean,
		boolean discount_boolean,
		double extended_price_total,
		double discount_total,
		double invoice_amount,
		double customer_payable_balance,
		double amount_due )
{
	INVOICE_LATEX_SUMMARY *invoice_latex_summary;

	invoice_latex_summary = invoice_latex_summary_calloc();

	invoice_latex_summary->extended_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_latex_summary_extended_display(
			description_boolean,
			discount_boolean,
			extended_price_total );

	invoice_latex_summary->discount_display =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		invoice_latex_summary_discount_display(
			description_boolean,
			discount_boolean,
			discount_total );

	invoice_latex_summary->amount_display =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		invoice_latex_summary_amount_display(
			description_boolean,
			discount_boolean,
			invoice_amount );

	invoice_latex_summary->payable_display =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		invoice_latex_summary_payable_display(
			description_boolean,
			discount_boolean,
			customer_payable_balance );

	invoice_latex_summary->due_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_latex_summary_due_display(
			amount_due_label,
			description_boolean,
			discount_boolean,
			amount_due );

	invoice_latex_summary->display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		invoice_latex_summary_display(
			invoice_latex_summary->extended_display,
			invoice_latex_summary->discount_display,
			invoice_latex_summary->amount_display,
			invoice_latex_summary->payable_display,
			invoice_latex_summary->due_display );

	return invoice_latex_summary;
}

INVOICE_LATEX_SUMMARY *invoice_latex_summary_calloc( void )
{
	INVOICE_LATEX_SUMMARY *invoice_latex_summary;

	if ( ! ( invoice_latex_summary =
			calloc( 1,
				sizeof ( INVOICE_LATEX_SUMMARY ) ) ) )
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

	return invoice_latex_summary;
}

char *invoice_latex_summary_extended_display(
		boolean description_boolean,
		boolean discount_boolean,
		double extended_price_total )
{
	char display[ 1024 ];

	snprintf(
		display,
		sizeof ( display ),
		"\\bf Extended total &%s \\bf %s \\\\",
		/* ------------------------------------ */
		/* Returns same static memory each time */
		/* ------------------------------------ */
		invoice_latex_summary_empty_display(
			description_boolean,
			discount_boolean ),
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			extended_price_total ) );

	return strdup( display );
}

char *invoice_latex_summary_discount_display(
		boolean description_boolean,
		boolean discount_boolean,
		double discount_total )
{
	char display[ 1024 ];

	if ( float_virtually_same( discount_total, 0.0 ) ) return NULL;

	snprintf(
		display,
		sizeof ( display ),
		"\\bf Discount total &%s \\bf %s \\\\",
		/* ------------------------------------ */
		/* Returns same static memory each time */
		/* ------------------------------------ */
		invoice_latex_summary_empty_display(
			description_boolean,
			discount_boolean ),
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			discount_total ) );

	return strdup( display );
}

char *invoice_latex_summary_amount_display(
		boolean description_boolean,
		boolean discount_boolean,
		double invoice_amount )
{
	char display[ 1024 ];

	if ( float_virtually_same( invoice_amount, 0.0 ) ) return NULL;

	snprintf(
		display,
		sizeof ( display ),
		"\\bf Invoice amount &%s \\bf %s \\\\",
		/* ------------------------------------ */
		/* Returns same static memory each time */
		/* ------------------------------------ */
		invoice_latex_summary_empty_display(
			description_boolean,
			discount_boolean ),
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			invoice_amount ) );

	return strdup( display );
}

char *invoice_latex_summary_payable_display(
		boolean description_boolean,
		boolean discount_boolean,
		double customer_payable_balance )
{
	char display[ 1024 ];

	if ( float_virtually_same( customer_payable_balance, 0.0 ) )
		return NULL;

	snprintf(
		display,
		sizeof ( display ),
		"\\bf Payment (Thank you) &%s \\bf -%s \\\\",
		/* ------------------------------------ */
		/* Returns same static memory each time */
		/* ------------------------------------ */
		invoice_latex_summary_empty_display(
			description_boolean,
			discount_boolean ),
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			customer_payable_balance ) );

	return strdup( display );
}

char *invoice_latex_summary_due_display(
		char *amount_due_label,
		boolean description_boolean,
		boolean discount_boolean,
		double amount_due )
{
	char display[ 1024 ];

	if ( !amount_due_label )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"amount_due_label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		display,
		sizeof ( display ),
		"\\bf %s &%s \\bf \\$%s \\\\",
		amount_due_label,
		/* ------------------------------------ */
		/* Returns same static memory each time */
		/* ------------------------------------ */
		invoice_latex_summary_empty_display(
			description_boolean,
			discount_boolean ),
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_dollar(
			amount_due ) );

	return strdup( display );
}


char *invoice_latex_summary_display(
		char *extended_display,
		char *discount_display,
		char *amount_display,
		char *payable_display,
		char *due_display )
{
	char display[ STRING_64K ];

	if ( !extended_display
	||   !due_display )
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

	snprintf(
		display,
		sizeof ( display ),
		"%s\n%s\n%s\n%s\n%s",
		extended_display,
		(discount_display) ? discount_display : "",
		(amount_display) ? amount_display : "",
		(payable_display) ? payable_display : "",
		due_display );

	return strdup( display );
}

char *invoice_latex_summary_empty_display(
		boolean description_boolean,
		boolean discount_boolean )
{
	static char empty_display[ 128 ];
	char *ptr = empty_display;

	if ( *empty_display ) return empty_display;

	ptr += sprintf( ptr, "&&" );

	if ( description_boolean ) ptr += sprintf( ptr, "&" );
	if ( discount_boolean ) ptr += sprintf( ptr, "&" );

	return empty_display;
}

char *invoice_latex_table_description_size( boolean discount_boolean )
{
	if ( discount_boolean )
		return "p{3.2in}";
	else
		return "p{4.2in}";
}

