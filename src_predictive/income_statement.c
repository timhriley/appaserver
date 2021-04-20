/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/income_statement.c			*/
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
#include "String.h"
#include "piece.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "date.h"
#include "appaserver_link_file.h"
#include "transaction.h"
#include "subclassification.h"
#include "element.h"
#include "predictive.h"

/* Constants */
/* --------- */
#define PROMPT				"Press here to view statement."

/* Prototypes */
/* ---------- */
void income_statement_net_income_html(
			HTML_TABLE *html_table,
			double net_income,
			boolean is_statement_of_activities,
			double percent_denominator,
			int skip_columns );

void income_statement_subclassification_omit_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			boolean is_statement_of_activities,
			char *process_name,
			char *logo_filename );

void income_statement_net_income_only(
			char *fund_name,
			char *as_of_date );

LIST *build_subclassification_display_PDF_heading_list(
			void );

LIST *build_subclassification_omit_PDF_heading_list(
			void );

LIST *build_subclassification_aggregate_PDF_heading_list(
			void );

LIST *build_subclassification_omit_PDF_row_list(
			LIST *element_list,
			boolean is_statement_of_activities );

LIST *build_subclassification_display_PDF_row_list(
			LIST *element_list,
			boolean is_statement_of_activities );

LIST *build_subclassification_aggregate_PDF_row_list(
			LIST *element_list,
			boolean is_statement_of_activities );

void income_statement_subclassification_aggregate_html_table(
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			boolean is_statement_of_activities );

void income_statement_subclassification_display_html_table(
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					boolean is_statement_of_activities );

void income_statement_subclassification_omit_html_table(
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					boolean is_statement_of_activities );

void income_statement_subclassification_aggregate_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			boolean is_statement_of_activities,
			char *process_name,
			char *logo_filename );

void income_statement_subclassification_display_PDF(
					char *application_name,
					char *title,
					char *sub_title,
					char *fund_name,
					char *as_of_date,
					char *document_root_directory,
					boolean is_statement_of_activities,
					char *process_name,
					char *logo_filename );

double get_shares_outstanding(		char *appaserver_mount_point,
					char *application_name,
					char *as_of_date );

void output_earnings_per_share(		HTML_TABLE *html_table,
					double net_income,
					double shares_outstanding );

double get_income_tax_expense(		LIST *account_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fund_name;
	char *as_of_date;
	char *subclassification_option;
	boolean net_income_only = 0;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *output_medium;
	char title[ 256 ];
	char sub_title[ 256 ];
	boolean is_statement_of_activities;
	char *logo_filename;
	char *program_name = {0};
/*
	double shares_outstanding;
*/

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc < 6 )
	{
		fprintf( stderr,
"Usage: %s process fund as_of_date subclassification_option output_medium [net_income_only_yn] [program_name]\n",
			 argv[ 0 ] );

		fprintf( stderr,
"Note: subclassification_option={omit,aggregate,display}\n" );

		exit ( 1 );
	}

	is_statement_of_activities =
		( strcmp( argv[ 0 ], "statement_of_activities" ) == 0 );

	process_name = argv[ 1 ];
	fund_name = argv[ 2 ];
	as_of_date = argv[ 3 ];
	subclassification_option = argv[ 4 ];

	if ( strcmp( subclassification_option, "aggregate" ) != 0
	&&   strcmp( subclassification_option, "display" ) != 0
	&&   strcmp( subclassification_option, "omit" ) != 0 )
	{
		subclassification_option = "display";
	}

	output_medium = argv[ 5 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	if ( argc >= 7 )
	{
		net_income_only = (*argv[ 6 ] == 'y');
	}

	if ( argc >= 8 ) program_name = argv[ 7 ];

	if ( program_name ){};

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*as_of_date
	||   strcmp(	as_of_date,
			"as_of_date" ) == 0 )
	{
		as_of_date =
			/* -------------------- */
			/* Returns heap memory. */
			/* -------------------- */
			transaction_date_maximum();
	}

	if ( net_income_only )
	{
		income_statement_net_income_only(
			fund_name,
			as_of_date );

		exit( 0 );
	}

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	logo_filename =
		application_constants_quick_fetch(
			application_name,
			"logo_filename" /* key */ );

	if ( !transaction_report_title_sub_title(
		title,
		sub_title,
		process_name,
		fund_name,
		as_of_date,
		list_length(
			transaction_fund_name_list() ),
		logo_filename ) )
	{
		printf( "<h3>Error. No transactions.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		if ( strcmp( subclassification_option, "aggregate" ) == 0 )
		{
			income_statement_subclassification_aggregate_html_table(
				title,
				sub_title,
				fund_name,
				as_of_date,
				is_statement_of_activities );
		}
		else
		if ( strcmp( subclassification_option, "display" ) == 0 )
		{
			income_statement_subclassification_display_html_table(
				title,
				sub_title,
				fund_name,
				as_of_date,
				is_statement_of_activities );
		}
		else
		/* ------------ */
		/* Must be omit */
		/* ------------ */
		{
			income_statement_subclassification_omit_html_table(
				title,
				sub_title,
				fund_name,
				as_of_date,
				is_statement_of_activities );
		}
	}
	else
	/* ----------- */
	/* Must be PDF */
	/* ----------- */
	{
		if ( strcmp( subclassification_option, "aggregate" ) == 0 )
		{
			income_statement_subclassification_aggregate_PDF(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				appaserver_parameter_file->
					document_root,
				is_statement_of_activities,
				process_name,
				logo_filename );
		}
		else
		if ( strcmp( subclassification_option, "display" ) == 0 )
		{
			income_statement_subclassification_display_PDF(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				appaserver_parameter_file->
					document_root,
				is_statement_of_activities,
				process_name,
				logo_filename );
		}
		else
		/* ------------ */
		/* Must be omit */
		/* ------------ */
		{
			income_statement_subclassification_omit_PDF(
				application_name,
				title,
				sub_title,
				fund_name,
				as_of_date,
				appaserver_parameter_file->
					document_root,
				is_statement_of_activities,
				process_name,
				logo_filename );
		}
	}

	return 0;

} /* main() */

void income_statement_net_income_only(
			char *fund_name,
			char *as_of_date )
{
	LIST *list;
	ELEMENT *element;
	LIST *filter_element_name_list;
	double total_revenues = {0};
	double total_expenses = {0};
	double total_gains = {0};
	double total_losses = {0};
	double net_income = {0};

	filter_element_name_list = list_new();

	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_LOSS );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* prior_closing_time_boolean */ ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ( element =
			element_list_seek(
				list,
				PREDICTIVE_ELEMENT_REVENUE ) ) )
	{
		total_revenues =
			element_value(
				element->subclassification_list,
				element->accumulate_debit );
	}

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_EXPENSE ) ) )
	{
		total_expenses =
			element_value(
				element->subclassification_list,
				element->accumulate_debit );
	}

	/* Compute total gains */
	/* ------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_GAIN ) ) )
	{
		total_gains =
			element_value(
				element->subclassification_list,
				element->accumulate_debit );
	}

	/* Compute total losses */
	/* -------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_LOSS ) ) )
	{
		total_losses =
			element_value(
					element->subclassification_list,
					element->accumulate_debit );
	}

	net_income = transaction_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	printf( "%.2lf\n", net_income );
}

void income_statement_subclassification_aggregate_html_table(
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			boolean is_statement_of_activities )
{
	LIST *list;
	ELEMENT *element;
	LIST *filter_element_name_list;
	HTML_TABLE *html_table;
	double total_revenues = {0};
	double total_expenses = {0};
	double total_gains = {0};
	double total_losses = {0};
	double net_income = {0};

	html_table = html_table_new( title, sub_title, "" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 3;

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Subclassification" );
	html_table_set_heading( html_table, "Element" );
	html_table_set_heading( html_table, "Percent" );

	html_table_output_table_heading(
		html_table->title,
		html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_LOSS );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* prior_closing_time_boolean */ ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_REVENUE ) ) )
	{
		total_revenues =
			subclassification_aggregate_html(
				html_table,
				element->subclassification_list,
				element->element_name,
				element->element_total
					/* percent_denominator */ );
	}

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_EXPENSE ) ) )
	{
		total_expenses =
			subclassification_aggregate_html(
				html_table,
				element->subclassification_list,
				element->element_name,
				total_revenues /* percent_denominator */ );
	}

	/* Compute total gains */
	/* ------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_GAIN ) ) )
	{
		total_gains =
			subclassification_aggregate_html(
				html_table,
				element->subclassification_list,
				element->element_name,
				total_revenues /* percent_denominator */ );
	}

	/* Compute total losses */
	/* -------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_LOSS ) ) )
	{
		total_losses =
			subclassification_aggregate_html(
				html_table,
				element->subclassification_list,
				element->element_name,
				total_revenues
					/* percent_denominator */ );
	}

	net_income =
		transaction_net_income(
			total_revenues,
			total_expenses,
			total_gains,
			total_losses );

	subclassification_aggregate_net_income_output(
		html_table,
		net_income,
		is_statement_of_activities,
		total_revenues /* percent_denominator */ );

	html_table_close();
	document_close();
}

void income_statement_subclassification_display_html_table(
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			boolean is_statement_of_activities )
{
	LIST *list;
	ELEMENT *element;
	LIST *filter_element_name_list;
	HTML_TABLE *html_table;
	double total_revenues = {0};
	double total_expenses = {0};
	double total_gains = {0};
	double total_losses = {0};
	double net_income = {0};

	html_table = html_table_new( title, sub_title, "" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 4;

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Account" );
	html_table_set_heading( html_table, "Subclassification" );
	html_table_set_heading( html_table, "Element" );
	html_table_set_heading( html_table, "Percent" );

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_LOSS );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* prior_closing_time_boolean */ ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_REVENUE ) ) )
	{
		total_revenues =
			subclassification_html_display(
				html_table,
				element->subclassification_list,
				PREDICTIVE_ELEMENT_REVENUE,
				element->accumulate_debit,
				element->element_total
					/* percent_denominator */ );
	}

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_EXPENSE ) ) )
	{
		total_expenses =
			subclassification_html_display(
				html_table,
				element->subclassification_list,
				PREDICTIVE_ELEMENT_EXPENSE,
				element->accumulate_debit,
				total_revenues
					/* percent_denominator */ );
	}

	/* Compute total gains */
	/* ------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_GAIN ) ) )
	{
		total_gains =
			subclassification_html_display(
				html_table,
				element->subclassification_list,
				PREDICTIVE_ELEMENT_GAIN,
				element->accumulate_debit,
				total_revenues
					/* percent_denominator */ );
	}

	/* Compute total losses */
	/* -------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_LOSS ) ) )
	{
		total_losses =
			subclassification_html_display(
				html_table,
				element->subclassification_list,
				PREDICTIVE_ELEMENT_LOSS,
				element->accumulate_debit,
				total_revenues
					/* percent_denominator */ );
	}

	net_income = transaction_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	income_statement_net_income_html(
		html_table,
		net_income,
		is_statement_of_activities,
		total_revenues,
		2 /* skip_columns */);

/*
		shares_outstanding = get_shares_outstanding(
					appaserver_parameter_file->
						appaserver_mount_point,
					application_name,
					as_of_date );
		if( shares_outstanding )
		{
			output_earnings_per_share(
						html_table,
						net_income,
						shares_outstanding );
		}
*/

	html_table_close();
	document_close();
}

void income_statement_subclassification_aggregate_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			boolean is_statement_of_activities,
			char *process_name,
			char *logo_filename )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	LIST *list;
	LIST *filter_element_name_list;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	int pid = getpid();
	APPASERVER_LINK_FILE *appaserver_link_file;

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

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

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	latex_table =
		latex_new_latex_table(
			sub_title /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list =
		build_subclassification_aggregate_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_LOSS );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* prior_closing_time_boolean */ ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	latex_table->row_list =
		build_subclassification_aggregate_PDF_row_list(
			list /* element_list */,
			is_statement_of_activities );

	if ( !list_length( latex_table->row_list ) )
	{
		printf(
		"<p>ERROR: there are no elements for this statement.\n" );
		document_close();
		exit( 1 );
	}

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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

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

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );
}

void income_statement_subclassification_display_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			boolean is_statement_of_activities,
			char *process_name,
			char *logo_filename )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	LIST *list;
	LIST *filter_element_name_list;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	int pid = getpid();
	APPASERVER_LINK_FILE *appaserver_link_file;

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

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

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	latex_table =
		latex_new_latex_table(
			sub_title /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list =
		build_subclassification_display_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_LOSS );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* prior_closing_time_boolean */ ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	latex_table->row_list =
		build_subclassification_display_PDF_row_list(
			list,
			is_statement_of_activities );

	if ( !list_length( latex_table->row_list ) )
	{
		printf(
		"<p>ERROR: there are no elements for this statement.\n" );
		document_close();
		exit( 1 );
	}

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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

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

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );
}

LIST *build_subclassification_aggregate_PDF_row_list(
			LIST *element_list,
			boolean is_statement_of_activities )
{
	ELEMENT *element;
	double total_revenues = 0.0;
	double total_expenses = 0.0;
	double total_gains = 0.0;
	double total_losses = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	LATEX_ROW *latex_row;

	row_list = list_new();

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_REVENUE ) ) )
	{
		list_append_list(
			row_list,
			subclassification_aggregate_latex_row_list(
				&total_revenues,
				element->subclassification_list,
				element->element_name,
				element->element_total
					 /* percent_denominator */ ) );
	}

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_EXPENSE ) ) )
	{
		list_append_list(
			row_list,
			subclassification_aggregate_latex_row_list(
				&total_expenses,
				element->subclassification_list,
				element->element_name,
				total_revenues /* percent_denominator */ ) );
	}

	/* Compute total gains */
	/* ------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_GAIN ) ) )
	{
		list_append_list(
			row_list,
			subclassification_aggregate_latex_row_list(
				&total_gains,
				element->subclassification_list,
				element->element_name,
				total_revenues /* percent_denominator */ ) );
	}

	/* Compute total losses */
	/* -------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_LOSS ) ) )
	{
		list_append_list(
			row_list,
			subclassification_aggregate_latex_row_list(
				&total_losses,
				element->subclassification_list,
				element->element_name,
				total_revenues /* percent_denominator */ ) );
	}

	net_income =
		transaction_net_income(
			total_revenues,
			total_expenses,
			total_gains,
			total_losses );

	/* Blank line */
	/* ---------- */
	latex_row = latex_new_latex_row();
	list_set( row_list, latex_row );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "" ),
		0 /* not large_bold */ );

	list_set(
		row_list,
		element_latex_subclassification_aggregate_net_income_row(
			net_income,
			is_statement_of_activities,
			total_revenues
				/* percent_denominator */ ) );
	return row_list;
}

LIST *build_subclassification_display_PDF_row_list(
			LIST *element_list,
			boolean is_statement_of_activities )
{
	ELEMENT *element;
	double total_revenues = 0.0;
	double total_expenses = 0.0;
	double total_gains = 0.0;
	double total_losses = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	LATEX_ROW *latex_row;

	row_list = list_new();

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_REVENUE ) ) )
	{
		list_append_list(
			row_list,
			subclassification_display_latex_row_list(
				&total_revenues,
				element->subclassification_list,
				element->element_name,
				element->accumulate_debit,
				element->
					element_total
						/* percent_denominator */ ) );
	}
	
	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_EXPENSE ) ) )
	{
		list_append_list(
			row_list,
			subclassification_display_latex_row_list(
				&total_expenses,
				element->subclassification_list,
				element->element_name,
				element->accumulate_debit,
				total_revenues /* percent_denominator */ ) );
	}

	/* Compute total gains */
	/* ------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_GAIN ) ) )
	{
		list_append_list(
			row_list,
			subclassification_display_latex_row_list(
				&total_gains,
				element->subclassification_list,
				element->element_name,
				element->accumulate_debit,
				total_revenues /* percent_denominator */ ) );
	}

	/* Compute total losses */
	/* -------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_LOSS ) ) )
	{
		list_append_list(
			row_list,
			subclassification_display_latex_row_list(
				&total_losses,
				element->subclassification_list,
				element->element_name,
				element->accumulate_debit,
				total_revenues /* percent_denominator */ ) );
	}

	net_income = transaction_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	/* Blank line */
	/* ---------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "" ),
		0 /* not large_bold */ );

	list_set(	row_list,
			element_latex_net_income_row(
				net_income,
				is_statement_of_activities,
				total_revenues
					/* percent_denominator */,
				0 /* not omit_subclassification */ ) );

	return row_list;
}

#ifdef NOT_DEFINED
void output_earnings_per_share(	HTML_TABLE *html_table,
				double net_income,
				double shares_outstanding )
{
	char buffer[ 128 ];

	html_table_set_data( html_table->data_list,
			     "<h2>Earnings Per Share</h2>" );

	html_table_set_data(
			html_table->data_list,
			"Net Income" );

	html_table_set_data(
			html_table->data_list,
			"Shares Outstanding" );

	html_table_set_data(
			html_table->data_list,
			"Earnings Per Share" );

	html_table_output_data(
			html_table->data_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );
	html_table->data_list = list_new();

	html_table_set_data( html_table->data_list, "" );

	sprintf(	buffer, 
			"%.2lf",
			net_income );

	html_table_set_data(	html_table->data_list,
				strdup( buffer ) );

	sprintf(	buffer, 
			"%.2lf",
			shares_outstanding );

	html_table_set_data(	html_table->data_list,
				strdup( buffer ) );

	if ( shares_outstanding )
	{
		sprintf(	buffer, 
				"%.2lf",
				net_income / shares_outstanding );
	}
	else
	{
		strcpy( buffer, "0.0" );
	}

	html_table_set_data(	html_table->data_list,
				strdup( buffer ) );

	html_table_output_data(
		html_table->data_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

} /* output_earnings_per_share() */
#endif

#ifdef NOT_DEFINED
double get_shares_outstanding(	char *appaserver_mount_point,
				char *application_name,
				char *as_of_date )
{
	char sys_string[ 1024 ];
	char *results_string;

	sprintf(sys_string,
		"%s/%s/shares_outstanding \"%s\" \"%s\" 2>>%s",
		appaserver_mount_point,
		application_relative_source_directory(
			application_name ),
		application_name,
		as_of_date,
		appaserver_error_get_filename(
			application_name ) );

	results_string = pipe2string( sys_string );
	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );
} /* get_shares_outstanding() */
#endif

LIST *build_subclassification_omit_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Element";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Percent";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_subclassification_omit_PDF_heading_list() */

LIST *build_subclassification_display_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Subclassification";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Element";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Percent";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

} /* build_subclassification_display_PDF_heading_list() */

LIST *build_subclassification_aggregate_PDF_heading_list( void )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;
}

void income_statement_subclassification_omit_html_table(
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			boolean is_statement_of_activities )
{
	LIST *list;
	ELEMENT *element;
	LIST *filter_element_name_list;
	HTML_TABLE *html_table;
	double total_revenues = {0};
	double total_expenses = {0};
	double total_gains = {0};
	double total_losses = {0};
	double net_income = {0};

	html_table = html_table_new( title, sub_title, "" );

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Account" );
	html_table_set_heading( html_table, "Element" );
	html_table_set_heading( html_table, "Percent" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 3;

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_LOSS );

	list = element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* prior_closing_time_boolean */ ),
			0 /* not fetch_subclassifiction_list */,
			1 /* fetch_account_list */ );

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_REVENUE ) ) )
	{
		total_revenues =
			account_list_html_output(
				html_table,
				element->account_list,
				PREDICTIVE_ELEMENT_REVENUE,
				element->accumulate_debit,
				element->element_total
					/* percent_denominator */ );
	}

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_EXPENSE ) ) )
	{
		total_expenses =
			account_list_html_output(
				html_table,
				element->account_list,
				PREDICTIVE_ELEMENT_EXPENSE,
				element->accumulate_debit,
				total_revenues
					/* percent_denominator */ );
	}

	/* Compute total gains */
	/* ------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_GAIN ) ) )
	{
		total_gains =
			account_list_html_output(
				html_table,
				element->account_list,
				PREDICTIVE_ELEMENT_GAIN,
				element->accumulate_debit,
				total_revenues
					/* percent_denominator */ );
	}

	/* Compute total losses */
	/* -------------------- */ 
	if ( ( element =
			element_seek(
				list,
				PREDICTIVE_ELEMENT_LOSS ) ) )
	{
		total_losses =
			account_list_html_output(
				html_table,
				element->account_list,
				PREDICTIVE_ELEMENT_LOSS,
				element->accumulate_debit,
				total_revenues
					/* percent_denominator */ );
	}

	net_income = transaction_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	income_statement_net_income_html(
		html_table,
		net_income,
		is_statement_of_activities,
		total_revenues,
		1 /* skip_columns */ );

/*
		shares_outstanding = get_shares_outstanding(
					appaserver_parameter_file->
						appaserver_mount_point,
					application_name,
					as_of_date );
		if( shares_outstanding )
		{
			output_earnings_per_share(
						html_table,
						net_income,
						shares_outstanding );
		}
*/

	html_table_close();
	document_close();

}

void income_statement_subclassification_omit_PDF(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			boolean is_statement_of_activities,
			char *process_name,
			char *logo_filename )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	LIST *list;
	LIST *filter_element_name_list;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	int pid = getpid();
	APPASERVER_LINK_FILE *appaserver_link_file;

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

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

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	latex_table =
		latex_new_latex_table(
			sub_title /* caption */ );

	list_set( latex->table_list, latex_table );

	latex_table->heading_list =
		build_subclassification_omit_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				PREDICTIVE_ELEMENT_LOSS );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* prior_closing_time_boolean */ ),
			0 /* not fetch_subclassifiction_list */,
			1 /* fetch_account_list */ );

	latex_table->row_list =
		build_subclassification_omit_PDF_row_list(
			list /* element_list */,
			is_statement_of_activities );

	if ( !list_length( latex_table->row_list ) )
	{
		printf(
		"<p>ERROR: there are no elements for this statement.\n" );
		document_close();
		exit( 1 );
	}

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

	latex_tex2pdf(	latex->tex_filename,
			latex->working_directory );

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

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		PROMPT,
		process_name /* target */,
		(char *)0 /* mime_type */ );
}

LIST *build_subclassification_omit_PDF_row_list(
			LIST *element_list,
			boolean is_statement_of_activities )
{
	ELEMENT *element;
	double total_revenues = 0.0;
	double total_expenses = 0.0;
	double total_gains = 0.0;
	double total_losses = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	LATEX_ROW *latex_row;

	row_list = list_new();

	/* Compute total revenues */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_REVENUE ) ) )
	{
		list_append_list(
			row_list,
			account_omit_latex_row_list(
				&total_revenues,
				element->account_list,
				element->element_name,
				element->accumulate_debit,
				element->element_total
					/* percent_denominator */ ) );
	}

	/* Compute total expenses */
	/* ---------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_EXPENSE ) ) )
	{
		list_append_list(
			row_list,
			account_omit_latex_row_list(
				&total_expenses,
				element->account_list,
				element->element_name,
				element->accumulate_debit,
				total_revenues /* percent_denominator */ ) );
	}

	/* Compute total gains */
	/* ------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_GAIN ) ) )
	{
		list_append_list(
			row_list,
			account_omit_latex_row_list(
				&total_gains,
				element->account_list,
				element->element_name,
				element->accumulate_debit,
				total_revenues /* percent_denominator */ ) );
	}

	/* Compute total losses */
	/* -------------------- */ 
	if ( ( element =
			element_seek(
				element_list,
				PREDICTIVE_ELEMENT_LOSS ) ) )
	{
		list_append_list(
			row_list,
			account_omit_latex_row_list(
				&total_losses,
				element->account_list,
				element->element_name,
				element->accumulate_debit,
				total_revenues /* percent_denominator */ ) );
	}

	net_income = transaction_net_income(
				total_revenues,
				total_expenses,
				total_gains,
				total_losses );

	/* Blank line */
	/* ---------- */
	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "" ),
		0 /* not large_bold */ );

	list_set(	row_list,
			element_latex_net_income_row(
				net_income,
				is_statement_of_activities,
				total_revenues
					/* percent_denominator */,
				1 /* omit_subclassification */ ) );
	return row_list;
}

void income_statement_net_income_html(
			HTML_TABLE *html_table,
			double net_income,
			boolean is_statement_of_activities,
			double percent_denominator,
			int skip_columns )
{
	double percent_of_total;

	html_table->data_list = list_new();

	if ( is_statement_of_activities )
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Change in Net Assets</h2>" );
	}
	else
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Net Income</h2>" );
	}

	while( skip_columns-- )
	{
		html_table_set_data( html_table->data_list, strdup( "" ) );
	}

	html_table_set_data(	html_table->data_list,
				strdup( place_commas_in_money( net_income ) ) );

	if ( percent_denominator )
	{
		char buffer[ 128 ];

		percent_of_total =
			( net_income /
			  percent_denominator ) * 100.0;

		sprintf( buffer,
			 "%.1lf%c",
			 percent_of_total,
			 '%' );

		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );
	}

	html_table_output_data( html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
	html_table->data_list = list_new();
}

