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
#include "account.h"
#include "predictive.h"
#include "latex.h"
#include "statement.h"

/* Constants */
/* --------- */
#define PROMPT				"Press here to view statement."

/* Prototypes */
/* ---------- */
void html_output(	LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities,
			enum subclassification_option );

void html_output_fund(
			STATEMENT_FUND *statement_fund,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities,
			enum subclassification_option );

void html_display_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			LIST *prior_year_list,
			boolean is_statement_of_activities );

void html_omit_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			LIST *prior_year_list,
			boolean is_statement_of_activities );

void html_aggregate_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			LIST *prior_year_list,
			boolean is_statement_of_activities );

void PDF_output(	char *application_name,
			char *process_name,
			char *logo_filename,
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities,
			char *document_root_directory,
			enum subclassification_option );

void PDF_output_fund(	char *application_name,
			char *process_name,
			char *logo_filename,
			STATEMENT_FUND *statement_fund,
			boolean is_statement_of_activities,
			char *document_root_directory,
			enum subclassification_option );

LATEX_ROW *PDF_net_income_latex_row(
			double net_income,
			int net_income_percent,
			LIST *prior_year_list,
			boolean is_statement_of_activities,
			boolean include_account,
			boolean include_subclassification );

int main( int argc, char **argv )
{
	boolean is_statement_of_activities;
	boolean net_income_only;
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *process_name;
	char *fund_name;
	LIST *fund_name_list;
	char *subclassification_option_string;
	char *fund_aggregation_string;
	char *output_medium_string;
	char *as_of_date;
	int prior_year_count;
	char *logo_filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	STATEMENT *statement;
	LIST *filter_element_name_list;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s session login_name role process fund as_of_date prior_year_count fund_aggregation subclassification_option output_medium net_income_only_yn\n",
			 argv[ 0 ] );

		fprintf( stderr,
"Note: subclassification_option={display,omit,aggregate}\n" );

		fprintf( stderr,
"Note: fund_aggregation={sequential,consolidated}\n" );

		fprintf( stderr,
"Note: output_medium={table,PDF,stdout}\n" );

		exit ( 1 );
	}

	/* Input */
	/* ----- */
	is_statement_of_activities =
		( strcmp( argv[ 0 ],
			  "statement_of_activities" ) == 0 );

	session = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];

	fund_name = argv[ 5 ];

	if ( *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		fund_name_list = list_delimited_string_to_list( fund_name );
	}
	else
	{
		fund_name_list = (LIST *)0;
	}

	as_of_date = argv[ 6 ];
	prior_year_count = atoi( argv[ 7 ] );
	fund_aggregation_string = argv[ 8 ];
	subclassification_option_string = argv[ 9 ];
	output_medium_string = argv[ 10 ];

	net_income_only = (*argv[ 11 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	/* Process */
	/* ------- */
	filter_element_name_list = list_new();

	list_set( filter_element_name_list, ELEMENT_REVENUE );
	list_set( filter_element_name_list, ELEMENT_EXPENSE );
	list_set( filter_element_name_list, ELEMENT_GAIN );
	list_set( filter_element_name_list, ELEMENT_LOSS );

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

	logo_filename =
		application_constants_quick_fetch(
			application_name,
			"logo_filename" /* key */ );

	statement =
		statement_new(
			application_name,
			session,
			login_name,
			role_name,
			process_name,
			(*logo_filename != 0) /* exists_logo_filename */,
			filter_element_name_list,
			as_of_date,
			prior_year_count,
			fund_name_list,
			subclassification_option_string,
			fund_aggregation_string,
			output_medium_string );

	statement =
		statement_steady_state(
			statement->application_name,
			statement->session,
			statement->login_name,
			statement->role_name,
			statement->process_name,
			statement->exists_logo_filename,
			statement->filter_element_name_list,
			statement->as_of_date,
			statement->prior_year_count,
			statement->fund_name_list,
			statement->subclassification_option_string,
			statement->fund_aggregation_string,
			statement->output_medium_string,
			0 /* not with_postclose */,
			statement );

	if ( !list_length( statement->statement_fund_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: statement_fund_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	statement_fund_list_net_income_set( statement->statement_fund_list );

	statement_fund_list_prior_year_list_set(
		statement->statement_fund_list,
		statement->prior_year_count );

	/* Output */
	/* ------ */
	if ( net_income_only )
	{
		STATEMENT_FUND *statement_fund;

		if ( ! ( statement_fund =
				statement_fund_seek(
					fund_name,
					statement->statement_fund_list ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: statement_fund_seek() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		printf( "%.2lf\n", statement_fund->net_income );

		exit( 0 );
	}

	if ( statement->statement_output_medium != output_stdout )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );
	}

	if ( statement->statement_output_medium == output_table )
	{
		html_output(
			statement->statement_fund_list,
			statement->title,
			statement->subtitle,
			is_statement_of_activities,
			statement->statement_subclassification_option );
	}
	else
	if ( statement->statement_output_medium == output_PDF )
	{
		PDF_output(
			application_name,
			process_name,
			logo_filename,
			statement->statement_fund_list,
			statement->title,
			statement->subtitle,
			is_statement_of_activities,
			appaserver_parameter_file->
				document_root,
			statement->statement_subclassification_option );
	}

	if ( statement->statement_output_medium != output_stdout )
		document_close();

	return 0;
}

void PDF_output(
			char *application_name,
			char *process_name,
			char *logo_filename,
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities,
			char *document_root_directory,
			enum subclassification_option subclassification_option )
{
	STATEMENT_FUND *statement_fund;

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", subtitle );

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		PDF_output_fund(
			application_name,
			process_name,
			logo_filename,
			statement_fund,
			is_statement_of_activities,
			document_root_directory,
			subclassification_option );

	} while ( list_next( statement_fund_list ) );
}

void PDF_output_fund(
			char *application_name,
			char *process_name,
			char *logo_filename,
			STATEMENT_FUND *statement_fund,
			boolean is_statement_of_activities,
			char *document_root_directory,
			enum subclassification_option subclassification_option )
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	int pid = getpid();
	APPASERVER_LINK_FILE *appaserver_link_file;
	boolean include_account = 0;
	boolean include_subclassification = 0;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
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
		appaserver_link_working_directory(
			document_root_directory,
			application_name );

	latex =
		latex_new(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	latex_table =
		latex_new_latex_table(
			statement_fund->
				statement_fund_caption );

	list_set( latex->table_list, latex_table );

	latex_table->row_list =
		statement_PDF_row_list(
			statement_fund->preclose_element_list,
			statement_fund->prior_year_list,
			subclassification_option,
			1 /* is_percent_of_revenue */ );

	if ( !list_length( latex_table->row_list ) )
	{
		printf(
		"<p>ERROR: there are no elements for this statement.\n" );
		document_close();
		exit( 1 );
	}

	if ( subclassification_option == subclassification_display )
	{
		include_account = 1;
		include_subclassification = 1;

		list_set(
			latex_table->row_list,
			PDF_net_income_latex_row(
				statement_fund->net_income,
				statement_fund->net_income_percent,
				statement_fund->prior_year_list,
				is_statement_of_activities,
				include_account,
				include_subclassification ) );
	}
	else
	if ( subclassification_option == subclassification_omit )
	{
		include_account = 1;

		list_set(
			latex_table->row_list,
			PDF_net_income_latex_row(
				statement_fund->net_income,
				statement_fund->net_income_percent,
				statement_fund->prior_year_list,
				is_statement_of_activities,
				include_account,
				include_subclassification ) );
	}
	else
	if ( subclassification_option == subclassification_aggregate )
	{
		include_subclassification = 1;

		list_set(
			latex_table->row_list,
			PDF_net_income_latex_row(
				statement_fund->net_income,
				statement_fund->net_income_percent,
				statement_fund->prior_year_list,
				is_statement_of_activities,
				include_account,
				include_subclassification ) );
	}

	latex_table->heading_list =
		statement_PDF_heading_list(
			statement_fund->prior_year_list,
			include_account,
			include_subclassification );

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

	latex_tex2pdf(
		latex->tex_filename,
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

void html_output(	LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities,
			enum subclassification_option subclassification_option )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		html_output_fund(
			statement_fund,
			title,
			subtitle,
			is_statement_of_activities,
			subclassification_option );

	} while ( list_next( statement_fund_list ) );
}

void html_output_fund(
			STATEMENT_FUND *statement_fund,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities,
			enum subclassification_option subclassification_option )
{
	HTML_TABLE *html_table;
	boolean include_account = 0;
	boolean include_subclassification = 0;

	html_table =
		html_table_new(
			title,
			subtitle,
			statement_fund->fund_name );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 99;

	html_table_heading(
		html_table->title,
		html_table->sub_title,
		html_table->sub_sub_title );

	if ( subclassification_option == subclassification_display )
	{
		include_account = 1;
		include_subclassification = 1;
	}
	if ( subclassification_option == subclassification_omit )
	{
		include_account = 1;
	}
	if ( subclassification_option == subclassification_aggregate )
	{
		include_subclassification = 1;
	}

	html_table_output_data_heading(
		statement_html_heading_list(
			statement_fund->prior_year_list,
			include_account,
			include_subclassification ),
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	if ( subclassification_option == subclassification_display )
	{
		statement_html_display_element_list(
			html_table,
			statement_fund->preclose_element_list,
			statement_fund->prior_year_list,
			1 /* is_percent_of_revenue */ );

		html_display_subclassification_net_income(
			html_table,
			statement_fund->net_income,
			statement_fund->net_income_percent,
			statement_fund->prior_year_list,
			is_statement_of_activities );
	}
	else
	if ( subclassification_option == subclassification_omit )
	{
		statement_html_omit_element_list(
			html_table,
			statement_fund->preclose_element_list,
			statement_fund->prior_year_list,
			1 /* is_percent_of_revenue */ );

		html_omit_subclassification_net_income(
			html_table,
			statement_fund->net_income,
			statement_fund->net_income_percent,
			statement_fund->prior_year_list,
			is_statement_of_activities );
	}
	else
	if ( subclassification_option == subclassification_aggregate )
	{
		statement_html_aggregate_element_list(
			html_table,
			statement_fund->preclose_element_list,
			statement_fund->prior_year_list,
			1 /* is_percent_of_revenue */ );

		html_aggregate_subclassification_net_income(
			html_table,
			statement_fund->net_income,
			statement_fund->net_income_percent,
			statement_fund->prior_year_list,
			is_statement_of_activities );
	}

	html_table_close();
}

void html_display_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			LIST *prior_year_list,
			boolean is_statement_of_activities )
{
	char buffer[ 128 ];

	html_table_set_data(
		html_table->data_list,
		strdup(
			statement_html_net_income_label(
				is_statement_of_activities ) ) );

	/* Skip the account */
	/* ---------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Skip the subclassification */
	/* -------------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the net income */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				net_income ) ) );

	sprintf( buffer,
		 "%d%c",
		 net_income_percent,
		 '%' );

	/* Set the percent of revenue */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_net_income_delta_list(
				prior_year_list ) );
	}

	/* Output the net income row */
	/* ------------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

void html_omit_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			LIST *prior_year_list,
			boolean is_statement_of_activities )
{
	char buffer[ 128 ];

	/* Set the element as centered */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			statement_html_net_income_label(
				is_statement_of_activities )  ) );

	/* Skip the account */
	/* ---------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the net income */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				net_income ) ) );

	sprintf( buffer,
		 "%d%c",
		 net_income_percent,
		 '%' );

	/* Percent of revenue */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_net_income_delta_list(
				prior_year_list ) );
	}

	/* Output the net income row */
	/* ------------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

void html_aggregate_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			LIST *prior_year_list,
			boolean is_statement_of_activities )
{
	char buffer[ 128 ];

	/* Set the element as centered */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			statement_html_net_income_label(
				is_statement_of_activities )  ) );

	/* Skip the subclassification */
	/* -------------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the net income */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				net_income ) ) );

	sprintf( buffer,
		 "%d%c",
		 net_income_percent,
		 '%' );

	/* Percent of revenue */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_net_income_delta_list(
				prior_year_list ) );
	}

	/* Output the net income row */
	/* ------------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

LATEX_ROW *PDF_net_income_latex_row(
			double net_income,
			int net_income_percent,
			LIST *prior_year_list,
			boolean is_statement_of_activities,
			boolean include_account,
			boolean include_subclassification )
{
	LATEX_ROW *latex_row;
	char buffer[ 128 ];

	latex_row = latex_new_latex_row();

	latex_column_data_set(
		latex_row->column_data_list,
		statement_PDF_net_income_label(
			is_statement_of_activities ),
		1 /* large_bold */ );

	if ( include_account )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_bold */ );
	}

	if ( include_subclassification )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_bold */ );
	}

	latex_column_data_set(
		latex_row->column_data_list,
		strdup(
			place_commas_in_money(
				net_income ) ),
		0 /* not large_bold */ );

	sprintf(buffer,
		"%d%c",
	 	net_income_percent,
	 	'%' );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( buffer ),
		0 /* not large_bold */ );

	if ( list_length( prior_year_list ) )
	{
		latex_column_data_set_list(
			latex_row->column_data_list,
			statement_PDF_net_income_delta_list(
					prior_year_list ) );
	}

	return latex_row;
}

