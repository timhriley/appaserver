/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_rentalproperty/schedule_e_report.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application_constants.h"
#include "document.h"
#include "application.h"
#include "ledger.h"
#include "appaserver_parameter_file.h"
#include "appaserver_link_file.h"
#include "html_table.h"
#include "date.h"
#include "boolean.h"
#include "latex.h"
#include "tax.h"

/* Constants */
/* --------- */
#define PROMPT			"Press here to view statement."
#define CURRENT_CENTURY		2000
#define SCHEDULE_E		"Schedule E"
#define ROWS_BETWEEN_HEADING	10

/* Prototypes */
/* ---------- */
LIST *build_PDF_row_list(
			LIST *tax_form_line_rental_list );

LIST *build_PDF_heading_list(
			LIST *input_rental_property_list );

LATEX_TABLE *tax_form_report_PDF_table(
			char *sub_title,
			LIST *tax_form_line_rental_list,
			LIST *input_rental_property_list );

void tax_form_report_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *document_root_directory,
			char *process_name,
			LIST *tax_form_line_rental_list,
			LIST *input_rental_property_list,
			char *logo_filename );

HTML_TABLE *tax_form_report_get_html_table(
			char *title,
			char *sub_title,
			LIST *rental_property_list );

void tax_form_report_html_table(
			char *title,
			char *sub_title,
			LIST *tax_form_line_rental_list,
			LIST *rental_property_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	int tax_year;
	char *output_medium;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 128 ];
	char sub_title[ 128 ];
	char buffer[ 128 ];
	TAX *tax;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s process tax_year output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	tax_year = atoi( argv[ 2 ] );
	output_medium = argv[ 3 ];

	if ( !tax_year )
	{
		tax_year = atoi( pipe2string( "now.sh ymd | piece.e '-' 0" ) );
	}
	else
	if ( tax_year < 99 )
	{
		tax_year += CURRENT_CENTURY;
	}

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	appaserver_parameter_file = appaserver_parameter_file_new();

	sprintf(title,
		"%s",
		application_title(
			application_name ) );

	sprintf(sub_title,
	 	"%s Tax Year: %d",
	 	format_initial_capital( buffer, process_name ),
	 	tax_year );

	document = document_new( process_name, application_name );
	document->output_content_type = 1;

	document_output_heading(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	tax = tax_new(	application_name,
			SCHEDULE_E,
			tax_year );

	if ( !tax->tax_input.tax_form->tax_form )
	{
		printf(
		"<h3>An internal error occurred. Check log.</h3>\n" );
		document_close();
		exit( 0 );
	}

	tax->tax_input.rental_property_list =
		tax_fetch_rental_property_list(
			application_name,
			tax->tax_input.end_date_string,
			tax->tax_input.tax_year );

	if ( !list_length( tax->tax_input.rental_property_list ) )
	{
		printf(
		"<h3>No rental properties owned during this period.</h3>\n" );
		document_close();
		exit( 0 );
	}

	tax->tax_output_rental.tax_form_line_rental_list =
		tax_get_tax_form_line_rental_list(
			tax->tax_process.tax_form_line_list,
			tax->tax_input.rental_property_list );

fprintf( stderr, "%s/%s()/%d\n",
__FILE__,
__FUNCTION__,
__LINE__ );
	if ( !list_length(
		tax->tax_output_rental.tax_form_line_rental_list ) )
	{
		printf(
	     "<h3>No rental property transactions during this period.</h3>\n" );
		document_close();
		exit( 0 );
	}

	tax_line_rental_list_accumulate_line_total(
		tax->tax_output_rental.tax_form_line_rental_list,
		tax->tax_input.rental_property_list );

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		tax_form_report_html_table(
			title,
			sub_title,
			tax->tax_output_rental.tax_form_line_rental_list,
			tax->tax_input.rental_property_list );
	}
	else
	{
		tax_form_report_PDF(
			application_name,
			title,
			sub_title,
			appaserver_parameter_file->
				document_root,
			process_name,
			tax->tax_output_rental.tax_form_line_rental_list,
			tax->tax_input.rental_property_list,
			application_constants_quick_fetch(
				application_name,
				"logo_filename" /* key */ ) );
	}

	document_close();

	return 0;

} /* main() */

void tax_form_report_html_table(
			char *title,
			char *sub_title,
			LIST *tax_form_line_rental_list,
			LIST *rental_property_list )
{
	TAX_FORM_LINE_RENTAL *tax_form_line_rental;
	TAX_OUTPUT_RENTAL_PROPERTY *rental_property;
	int count = 0;
	HTML_TABLE *html_table;

	html_table =
		tax_form_report_get_html_table(
			title,
			sub_title,
			rental_property_list );

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->justify_list );

	if ( !list_rewind( tax_form_line_rental_list ) )
	{
		printf(
"<h3>ERROR: there are no tax form lines for this tax form.</h3>\n" );
		html_table_close();
		document_close();
		exit( 1 );
	}

	do {
		tax_form_line_rental = list_get( tax_form_line_rental_list );

		if ( !list_length(
			tax_form_line_rental->
				rental_property_list ) )
		{
			continue;
		}

		if ( ++count == ROWS_BETWEEN_HEADING )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->
				number_left_justified_columns,
				html_table->
				number_right_justified_columns,
				html_table->justify_list );
			count = 0;
		}

		html_table_set_data(
			html_table->data_list,
			strdup( tax_form_line_rental->tax_form_line ) );

		html_table_set_data(
			html_table->data_list,
			strdup( tax_form_line_rental->tax_form_description ) );

		list_rewind( tax_form_line_rental->rental_property_list );

		do {
			rental_property =
				list_get_pointer(
					tax_form_line_rental->
						rental_property_list );

			html_table_set_data(
				html_table->data_list,
				strdup( timlib_place_commas_in_money(
					     rental_property->
						tax_form_line_total ) ) );

		} while( list_next(
				tax_form_line_rental->
					rental_property_list ) );

		html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

			list_free( html_table->data_list );
			html_table->data_list = list_new();

	} while( list_next( tax_form_line_rental_list ) );

	html_table_close();

} /* tax_form_report_html_table() */

HTML_TABLE *tax_form_report_get_html_table(
				char *title,
				char *sub_title,
				LIST *rental_property_list )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	TAX_INPUT_RENTAL_PROPERTY *rental_property;

	html_table = new_html_table(
			title,
			strdup( sub_title ) );

	if ( !list_rewind( rental_property_list ) )
		return html_table;

	heading_list = list_new();
	list_append_string( heading_list, "tax_form_line" );
	list_append_string( heading_list, "tax_form_description" );

	do {
		rental_property =
			list_get_pointer(
				rental_property_list );

		list_append_string(
			heading_list,
			rental_property->property_street_address );

	} while( list_next( rental_property_list ) );

	html_table_set_heading_list( html_table, heading_list );
	html_table->number_left_justified_columns = 2;

	html_table->number_right_justified_columns =
		list_length( rental_property_list );

	return html_table;

} /* tax_form_report_get_html_table() */

void tax_form_report_PDF(	char *application_name,
				char *title,
				char *sub_title,
				char *document_root_directory,
				char *process_name,
				LIST *tax_form_line_rental_list,
				LIST *input_rental_property_list,
				char *logo_filename )
{
	LATEX *latex;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	int pid = getpid();

	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			process_name /* filename_stem */,
			application_name,
			pid /* process_id */,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->extension = "tex";

	latex_filename =
		strdup( appaserver_link_get_tail_half(
				(char *)0 /* application_name */,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension ) );

	appaserver_link_file->extension = "dvi";

	dvi_filename =
		strdup( appaserver_link_get_tail_half(
				(char *)0 /* application_name */,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension ) );

	working_directory =
		appaserver_link_source_directory(
			document_root_directory,
			application_name );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	list_append_pointer(
		latex->table_list,
		tax_form_report_PDF_table(
			sub_title,
			tax_form_line_rental_list,
			input_rental_property_list ) );

	latex_longtable_output(
		latex->output_stream,
		latex->landscape_flag,
		latex->table_list,
		latex->logo_filename,
		0 /* not omit_page_numbers */,
		date_get_now_yyyy_mm_dd_hh_mm(
			date_get_utc_offset() )
				/* footline */ );

	fclose( latex->output_stream );

	appaserver_link_file->extension = "pdf";

	ftp_output_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );

} /* tax_form_report_PDF() */

LIST *build_PDF_row_list( LIST *tax_form_line_rental_list )
{
	LATEX_ROW *latex_row;
	LIST *row_list;
	TAX_FORM_LINE_RENTAL *tax_form_line_rental;
	TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property;

	if ( !list_rewind( tax_form_line_rental_list ) )
	{
		printf(
"<h3>ERROR: there are no tax form lines for this tax form.</h3>\n" );
		document_close();
		exit( 1 );
	}

	row_list = list_new();

	do {
		tax_form_line_rental =
			list_get(
				tax_form_line_rental_list );

		if ( !list_rewind(
			tax_form_line_rental->
				rental_property_list ) )
		{
			continue;
		}

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_column_data_set(
			latex_row->column_data_list,
			tax_form_line_rental->tax_form_line,
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( tax_form_line_rental->tax_form_description ),
			0 /* not large_bold */ );

		do {
			tax_output_rental_property =
				list_get_pointer(
					tax_form_line_rental->
						rental_property_list );

			latex_column_data_set(
				latex_row->column_data_list,
				strdup( timlib_place_commas_in_money(
					   tax_output_rental_property->
						tax_form_line_total ) ),
				0 /* not large_bold */ );

		} while( list_next(
				tax_form_line_rental->	
					rental_property_list ) );

	} while( list_next( tax_form_line_rental_list ) );

	return row_list;

} /* build_PDF_row_list() */

LIST *build_PDF_heading_list( LIST *input_rental_property_list )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;
	TAX_INPUT_RENTAL_PROPERTY *rental_property;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "tax_form_line";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "tax_form_description";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	if ( !list_rewind( input_rental_property_list ) )
		return heading_list;

	do {
		rental_property =
			list_get_pointer(
				input_rental_property_list );

		table_heading = latex_new_latex_table_heading();
		table_heading->heading =
			rental_property->property_street_address;
		table_heading->right_justified_flag = 1;
		list_append_pointer( heading_list, table_heading );

	} while( list_next( input_rental_property_list ) );

	return heading_list;

} /* build_PDF_heading_list() */

LATEX_TABLE *tax_form_report_PDF_table(
			char *sub_title,
			LIST *tax_form_line_rental_list,
			LIST *input_rental_property_list )
{
	LATEX_TABLE *latex_table;
	char caption[ 128 ];

	strcpy( caption, sub_title );

	latex_table =
		latex_new_latex_table(
			strdup( caption ) );

	latex_table->heading_list =
		build_PDF_heading_list(
			input_rental_property_list );

	latex_table->row_list =
		build_PDF_row_list(
			tax_form_line_rental_list );

	return latex_table;

} /* tax_form_report_PDF_table() */

