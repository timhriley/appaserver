/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/balance_sheet.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
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
#include "element.h"
#include "account.h"
#include "transaction.h"
#include "subclassification.h"
#include "predictive.h"
#include "statement.h"

/* Constants */
/* --------- */
#define PROMPT				"Press here to view statement."

/* Prototypes */
/* ---------- */
SUBCLASSIFICATION *balance_sheet_net_income_subclassification(
			char *fund_name,
			char *as_of_date );

void html_output(	LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_financial_positiion,
			enum subclassification_option );

void html_output_fund(	STATEMENT_FUND *statement_fund,
			char *title,
			char *subtitle,
			boolean is_financial_position,
			enum subclassification_option );

void html_display_subclassification_element_list(
			HTML_TABLE *html_table,
			LIST *preclose_element_list,
			LIST *prior_year_list,
			char *as_of_date,
			char *fund_name,
			boolean is_financial_position );

void html_display_subclassification_regular_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list );

void html_display_subclassification_equity_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list,
			char *as_of_date,
			char *fund_name,
			boolean is_financial_position );

int main( int argc, char **argv )
{
	boolean is_financial_position;
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

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s session login_name role process fund as_of_date prior_year_count fund_aggregation subclassification_option output_medium\n",
			 argv[ 0 ] );

		fprintf( stderr,
"Note: subclassification_option={display,omit,aggregate}\n" );

		fprintf( stderr,
"Note: fund_aggregation={sequential,consolidated}\n" );

		fprintf( stderr,
"Note: output_medium={table,PDF,stdout}\n" );

		exit ( 1 );
	}

	is_financial_position =
		( strcmp( argv[ 0 ], "financial_position" ) == 0 );

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

	appaserver_parameter_file = appaserver_parameter_file_new();

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
			(LIST *)0 /* filter_element_name_list */,
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
			statement );

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
			is_financial_position,
			statement->subclassification_option );
	}
	else
	if ( statement->statement_output_medium == output_PDF )
	{
/*
		PDF_output(
			application_name,
			appaserver_parameter_file->document_root,
			process_name,
			logo_filename,
			statement->statement_fund_list,
			statement->title,
			statement->subtitle,
			statement->statement_subclassification_option,
			statement->statement_fund_aggregation );
*/
	}
	else
	{
/*
		stdout_output(
			statement->statement_fund_list );
*/
	}

	if ( statement->statement_output_medium != output_stdout )
		document_close();

	return 0;
}

void html_output(	LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_financial_position,
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
			is_financial_position,
			subclassification_option );

	} while ( list_next( statement_fund_list ) );
}

void html_output_fund(	STATEMENT_FUND *statement_fund,
			char *title,
			char *subtitle,
			boolean is_financial_position,
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
		html_display_subclassification_element_list(
			html_table,
			statement_fund->preclose_element_list,
			statement_fund->prior_year_list,
			statement_fund->as_of_date,
			statement_fund->fund_name,
			is_financial_position );
	}
	else
	if ( subclassification_option == subclassification_omit )
	{
/*
		html_omit_subclassification_element_list(
			html_table,
			statement_fund->preclose_element_list,
			statement_fund->prior_year_list );
*/
	}
	else
	if ( subclassification_option == subclassification_aggregate )
	{
/*
		html_aggregate_subclassification_element_list(
			html_table,
			statement_fund->preclose_element_list,
			statement_fund->prior_year_list );
*/
	}

	html_table_close();
}

void html_display_subclassification_element_list(
			HTML_TABLE *html_table,
			LIST *preclose_element_list,
			LIST *prior_year_list,
			char *as_of_date,
			char *fund_name,
			boolean is_financial_position )
{
	ELEMENT *element;

	if ( !list_rewind( preclose_element_list ) ) return;

	do {
		element = list_get( preclose_element_list );

		if ( !element->element_balance_total ) continue;

		if ( strcmp( element->element_name, ELEMENT_EQUITY ) == 0 )
		{
			html_display_subclassification_equity_element(
				html_table,
				element,
				prior_year_list,
				as_of_date,
				fund_name,
				is_financial_position );
		}
		else
		{
			html_display_subclassification_regular_element(
				html_table,
				element,
				prior_year_list );
		}

	} while ( list_next( preclose_element_list ) );
}

void html_display_subclassification_regular_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	char element_title[ 128 ];

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( element->subclassification_list ) ) return;

	/* Set the element as centered */
	/* --------------------------- */
	sprintf(element_title,
		"<h2>%s</h2>",
		format_initial_capital(
			format_buffer,
			element->element_name ) );

	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );

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

	do {
		subclassification = list_get( element->subclassification_list );

		if ( !list_length( subclassification->account_list ) )
			continue;

		/* ------------------------------------ */
		/* If more than one account,		*/
		/* then set the subclassification name.	*/
		/* ------------------------------------ */
		if ( list_length( subclassification->account_list ) > 1 )
		{
			sprintf(buffer,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
	
			/* Output the subclassification name */
			/* --------------------------------- */
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();
		}

		list_rewind( subclassification->account_list );

		do {
			account =
				list_get(
					subclassification->account_list );

			if ( !account->account_total ) continue;

			html_table_set_data(
				html_table->data_list,
				strdup(
					format_initial_capital(
						format_buffer,
						account->account_name ) ) );
	
			html_table_set_data(
				html_table->data_list,
				strdup(
					place_commas_in_money(
						account->account_total ) ) );

			/* Skip the subclassification column */
			/* --------------------------------- */
			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			/* Skip the element column */
			/* ----------------------- */
			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			/* Set the account percent of revenue */
			/* ---------------------------------- */
			sprintf( buffer,
				 "%d%c",
				 account->percent_of_revenue,
				 '%' );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

			if ( list_length( prior_year_list ) )
			{
				list_append_list(
					html_table->data_list,
					statement_html_account_delta_list(
						account->account_name,
						prior_year_list ) );
			}

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

		} while( list_next( subclassification->account_list ) );

		/* Set the subclassification label */
		/* ------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				statement_html_subclassification_label(
					subclassification->
						subclassification_name ) ) );

		/* Skip the account column */
		/* ----------------------- */
		html_table_set_data( html_table->data_list, strdup( "" ) );

		/* Set the subclassification total */
		/* ------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				place_commas_in_money(
					subclassification->
						subclassification_total ) ) );

		/* Skip the element column */
		/* ----------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		sprintf( buffer,
			 "%d%c",
			 subclassification->percent_of_revenue,
			 '%' );

		/* Set the subclassification percent of revenue */
		/* -------------------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		if ( list_length( prior_year_list ) )
		{
			list_append_list(
				html_table->data_list,
				statement_html_subclassification_delta_list(
					subclassification->
						subclassification_name,
					prior_year_list ) );
		}

		/* Output the row */
		/* -------------- */
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

	} while( list_next( element->subclassification_list ) );

	sprintf(element_title,
		"<h2>Total %s</h2>",
		format_initial_capital(
			format_buffer,
			element->element_name ) );

	/* Set the element total title */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );
	
	/* Skip the account column */
	/* ----------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Skip the subclassification column */
	/* --------------------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the element total column */
	/* ---------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				element->element_balance_total ) ) );

	sprintf( buffer,
		 "%d%c",
		 element->percent_of_revenue,
		 '%' );

	/* Set the percent of revenue column */
	/* --------------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_element_delta_list(
				element->element_name,
				prior_year_list ) );
	}

	/* Output the element row */
	/* ---------------------- */
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

void html_display_subclassification_equity_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list,
			char *as_of_date,
			char *fund_name,
			boolean is_financial_position )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	char element_title[ 128 ];

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( element->subclassification_list ) ) return;

	list_set(
		element->subclassification_list,
		balance_sheet_net_income_subclassification(
			fund_name,
			as_of_date ) );

	/* Set the element as centered */
	/* --------------------------- */
	sprintf(element_title,
		"<h2>%s</h2>",
		format_initial_capital(
			format_buffer,
			element->element_name ) );

	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );

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

	list_rewind( element->subclassification_list );

	do {
		subclassification = list_get( element->subclassification_list );

		if ( !list_length( subclassification->account_list ) )
			continue;

		/* ------------------------------------ */
		/* If more than one account,		*/
		/* then set the subclassification name.	*/
		/* ------------------------------------ */
		if ( list_length( subclassification->account_list ) > 1 )
		{
			sprintf(buffer,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
	
			/* Output the subclassification name */
			/* --------------------------------- */
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();
		}

		list_rewind( subclassification->account_list );

		do {
			account =
				list_get(
					subclassification->account_list );

			if ( !account->account_total ) continue;

			html_table_set_data(
				html_table->data_list,
				strdup(
					format_initial_capital(
						format_buffer,
						account->account_name ) ) );
	
			html_table_set_data(
				html_table->data_list,
				strdup(
					place_commas_in_money(
						account->account_total ) ) );

			/* Skip the subclassification column */
			/* --------------------------------- */
			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			/* Skip the element column */
			/* ----------------------- */
			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			/* Set the account percent of revenue */
			/* ---------------------------------- */
			sprintf( buffer,
				 "%d%c",
				 account->percent_of_revenue,
				 '%' );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

			if ( list_length( prior_year_list ) )
			{
				list_append_list(
					html_table->data_list,
					statement_html_account_delta_list(
						account->account_name,
						prior_year_list ) );
			}

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

		} while( list_next( subclassification->account_list ) );

		/* Set the subclassification label */
		/* ------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				statement_html_subclassification_label(
					subclassification->
						subclassification_name ) ) );

		/* Skip the account column */
		/* ----------------------- */
		html_table_set_data( html_table->data_list, strdup( "" ) );

		/* Set the subclassification total */
		/* ------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				place_commas_in_money(
					subclassification->
						subclassification_total ) ) );

		/* Skip the element column */
		/* ----------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		sprintf( buffer,
			 "%d%c",
			 subclassification->percent_of_revenue,
			 '%' );

		/* Set the subclassification percent of revenue */
		/* -------------------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		if ( list_length( prior_year_list ) )
		{
			list_append_list(
				html_table->data_list,
				statement_html_subclassification_delta_list(
					subclassification->
						subclassification_name,
					prior_year_list ) );
		}

		/* Output the row */
		/* -------------- */
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

	} while( list_next( element->subclassification_list ) );

	sprintf(element_title,
		"<h2>Total %s</h2>",
		format_initial_capital(
			format_buffer,
			element->element_name ) );

	/* Set the element total title */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );
	
	/* Skip the account column */
	/* ----------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Skip the subclassification column */
	/* --------------------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the element total column */
	/* ---------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				element->element_balance_total ) ) );

	sprintf( buffer,
		 "%d%c",
		 element->percent_of_revenue,
		 '%' );

	/* Set the percent of revenue column */
	/* --------------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_element_delta_list(
				element->element_name,
				prior_year_list ) );
	}

	/* Output the element row */
	/* ---------------------- */
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

SUBCLASSIFICATION *balance_sheet_net_income_subclassification(
			char *fund_name,
			char *as_of_date )
{
	double net_income;
	ACCOUNT *net_income_account;
	SUBCLASSIFICATION *net_income_subclassification;

	net_income = 
		transaction_net_income_fetch(
			fund_name,
			as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			account_new(
				ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			account_new(
				ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			net_income_account->account_name );

	net_income_account->account_total = 
	net_income_account->latest_journal->balance = net_income;

	net_income_subclassification =
		subclassification_new(
			net_income_account->account_name );

	net_income_subclassification->account_list = list_new();

	list_set(
		net_income_subclassification->account_list,
		net_income_account );

	return net_income_subclassification;
}

#ifdef NOT_DEFINED

	HTML_TABLE *html_table;
	LIST *filter_element_name_list;
	LIST *list;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;
	double total_assets = {0};
	double total_liabilities = {0};
	double total_equity = {0};
	double net_income = {0};

	html_table = html_table_new( title, sub_title, "" );

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Account" );
	html_table_set_heading( html_table, "Subclassification" );
	html_table_set_heading( html_table, "Element" );
	html_table_set_heading( html_table, "Percent" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 4;

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
				ELEMENT_ASSET );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LIABILITY );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time */,
				transaction_closing_entry_exists(
					as_of_date ) ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	net_income = 
		transaction_net_income_fetch(
			fund_name,
			as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			account_new(
				ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			account_new(
				ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_journal =
			journal_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_journal->balance = net_income;

	subclassification =
		subclassification_new(
			net_income_account->account_name );

	subclassification->account_list = list_new();

	list_append_pointer(	subclassification->account_list,
				net_income_account );

	list_append_pointer(	element->subclassification_list,
				subclassification );

	total_equity =
		subclassification_html_display(
			html_table,
			element->subclassification_list,
			element->element_name,
			element->accumulate_debit,
			total_assets /* percent_denominator */ );

	liabilities_plus_equity_html(
		html_table,
		total_liabilities + total_equity,
		0 /* not aggregate_subclassification */ );

	html_table_close();



void balance_sheet_subclassification_aggregate_PDF(
				char *application_name,
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				char *document_root_directory,
				char *process_name,
				boolean is_financial_position,
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
				ELEMENT_ASSET );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LIABILITY );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time */,
				transaction_closing_entry_exists(
					as_of_date ) ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	latex_table->row_list =
		build_subclassification_aggregate_PDF_row_list(
			list /* element_list */,
			fund_name,
			as_of_date,
			is_financial_position );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

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
}

void balance_sheet_subclassification_display_PDF(
				char *application_name,
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				char *document_root_directory,
				char *process_name,
				boolean is_financial_position,
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
				ELEMENT_ASSET );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LIABILITY );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time */,
				transaction_closing_entry_exists(
					as_of_date ) ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	latex_table->row_list =
		build_subclassification_display_PDF_row_list(
			list,
			fund_name,
			as_of_date,
			is_financial_position );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

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
}

void balance_sheet_subclassification_aggregate_html(
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				boolean is_financial_position )
{
	HTML_TABLE *html_table;
	LIST *filter_element_name_list;
	LIST *list;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;
	double total_assets = {0};
	double total_liabilities = {0};
	double total_equity = {0};
	double net_income = {0};

	html_table = html_table_new( title, sub_title, "" );

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Subclassification" );
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
				ELEMENT_ASSET );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LIABILITY );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time */,
				transaction_closing_entry_exists(
					as_of_date ) ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	/* Output total_assets */
	/* ------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_ASSET,
				list ) ) )
	{
		total_assets =
			subclassification_aggregate_html(
				html_table,
				element->subclassification_list,
				element->element_name,
				element->element_total
					/* percent_denominator */ );
	}

	/* Calculate total_liabilities */
	/* --------------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_LIABILITY,
				list ) ) )
	{
		total_liabilities =
			subclassification_aggregate_html(
				html_table,
				element->subclassification_list,
				element->element_name,
				total_assets /* percent_denominator */ );
	}

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			element_seek(
				ELEMENT_EQUITY,
				list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot element_seek(%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_EQUITY );
		exit( 1 );
	}

	/* Add net income to equity */
	/* ------------------------- */
	net_income =
		transaction_net_income_fetch(
			fund_name,
			as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			account_new(
				ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			account_new(
				ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_journal =
			journal_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_journal->balance = net_income;

	subclassification =
		subclassification_new(
			net_income_account->account_name );

	subclassification->subclassification_total = net_income;

	list_set(	element->subclassification_list,
			subclassification );

	total_equity =
		subclassification_aggregate_html(
			html_table,
			element->subclassification_list,
			element->element_name,
			total_assets /* percent_denominator */ );

	liabilities_plus_equity_html(
			html_table,
			total_liabilities + total_equity,
			1 /* aggregate_subclassification */ );

	html_table_close();
}

void balance_sheet_subclassification_display_html(
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				boolean is_financial_position )
{
	HTML_TABLE *html_table;
	LIST *filter_element_name_list;
	LIST *list;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;
	double total_assets = {0};
	double total_liabilities = {0};
	double total_equity = {0};
	double net_income = {0};

	html_table = html_table_new( title, sub_title, "" );

	html_table_set_heading( html_table, "" );
	html_table_set_heading( html_table, "Account" );
	html_table_set_heading( html_table, "Subclassification" );
	html_table_set_heading( html_table, "Element" );
	html_table_set_heading( html_table, "Percent" );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 4;

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
				ELEMENT_ASSET );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LIABILITY );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time */,
				transaction_closing_entry_exists(
					as_of_date ) ),
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	/* Output total_assets */
	/* ------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_ASSET,
				list ) ) )
	{
		total_assets = element->element_total;

		subclassification_html_display(
			html_table,
			element->subclassification_list,
			element->element_name,
			element->accumulate_debit,
			total_assets /* percent_denominator */ );
	}

	/* Calculate total_liabilities */
	/* --------------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_LIABILITY,
				list ) ) )
	{
		total_liabilities =
			subclassification_html_display(
				html_table,
				element->subclassification_list,
				element->element_name,
				element->accumulate_debit,
				total_assets /* percent_denominator */ );
	}

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			element_seek(
				ELEMENT_EQUITY,
				list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_EQUITY );
	}

	/* Add net income to equity */
	/* ------------------------- */
	net_income = 
		transaction_net_income_fetch(
			fund_name,
			as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			account_new(
				ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			account_new(
				ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_journal =
			journal_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_journal->balance = net_income;

	subclassification =
		subclassification_new(
			net_income_account->account_name );

	subclassification->account_list = list_new();

	list_append_pointer(	subclassification->account_list,
				net_income_account );

	list_append_pointer(	element->subclassification_list,
				subclassification );

	total_equity =
		subclassification_html_display(
			html_table,
			element->subclassification_list,
			element->element_name,
			element->accumulate_debit,
			total_assets /* percent_denominator */ );

	liabilities_plus_equity_html(
		html_table,
		total_liabilities + total_equity,
		0 /* not aggregate_subclassification */ );

	html_table_close();
}

void liabilities_plus_equity_html(
			HTML_TABLE *html_table,
			double liabilities_plus_equity,
			boolean aggregate_subclassification )
{
	char element_title[ 128 ];

	sprintf(	element_title,
			"<h2>Liabilities + Equity</h2>" );

	html_table_set_data( html_table->data_list, element_title );

	html_table_set_data(	html_table->data_list, "" );

	if ( !aggregate_subclassification )
		html_table_set_data( html_table->data_list, "" );

	html_table_set_data(	html_table->data_list,
				strdup( place_commas_in_money(
					liabilities_plus_equity ) ) );

	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	html_table->data_list = list_new();
}

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

LIST *build_account_omit_PDF_heading_list( void )
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
}

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
}

LIST *build_subclassification_aggregate_PDF_row_list(
			LIST *element_list,
			char *fund_name,
			char *as_of_date,
			boolean is_financial_position )
{
	ELEMENT *element;
	double total_assets = 0.0;
	double total_liabilities = 0.0;
	double total_equity = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;

	row_list = list_new();

	/* Compute total_assets */
	/* -------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_ASSET,
				element_list ) ) )
	{
		list_append_list(
			row_list,
			subclassification_aggregate_latex_row_list(
				&total_assets,
				element->subclassification_list,
				element->element_name,
				element->element_total
					/* percent_denominator */ ) );
	}

	/* Compute total_liabilities */
	/* ------------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_LIABILITY,
				element_list ) ) )
	{
		list_append_list(
			row_list,
			   subclassification_aggregate_latex_row_list(
			     &total_liabilities,
			     element->subclassification_list,
			     element->element_name,
			     element->element_total
					/* percent_denominator */ ) );
	}

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			element_seek(
				ELEMENT_EQUITY,
				element_list ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: element_seek(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_EQUITY );
		exit( 1 );
	}

	/* Build net income subclassification. */
	/* ----------------------------------- */
	net_income =
		transaction_net_income_fetch(
			fund_name,
			as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			account_new(
				ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			account_new(
				ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_journal =
			journal_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_journal->balance = net_income;

	subclassification =
		subclassification_new(
			net_income_account->account_name );

	subclassification->subclassification_total = net_income;

	subclassification->account_list = list_new();

	list_set(	subclassification->account_list,
			net_income_account );

	list_set(	element->subclassification_list,
			subclassification );

	list_append_list(
		row_list,
		subclassification_aggregate_latex_row_list(
			&total_equity,
			element->subclassification_list,
			element->element_name,
			element->element_total + net_income
				/* percent_denominator */ ) );

	list_set(	row_list,
			element_latex_liabilities_plus_equity_row(
				total_liabilities + total_equity,
				0 /* skip_columns */ ) );

	return row_list;
}

LIST *build_subclassification_display_PDF_row_list(
				LIST *element_list,
				char *fund_name,
				char *as_of_date,
				boolean is_financial_position )
{
	ELEMENT *element;
	double total_assets = 0.0;
	double total_liabilities = 0.0;
	double total_equity = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *net_income_account;

	row_list = list_new();

	/* Compute total_assets */
	/* -------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_ASSET,
				element_list ) ) )
	{
		list_append_list(
			row_list,
			subclassification_display_latex_row_list(
				&total_assets,
				element->subclassification_list,
				element->element_name,
				element->accumulate_debit,
				element->element_total
					/* percent_denominator */ ) );
	}

	/* Compute total_liabilities */
	/* ------------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_LIABILITY,
				element_list ) ) )
	{
		list_append_list(
			row_list,
			subclassification_display_latex_row_list(
				&total_liabilities,
				element->subclassification_list,
				element->element_name,
				element->accumulate_debit,
				total_assets /* percent_denominator */ ) );
	}

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			element_seek(
				ELEMENT_EQUITY,
				element_list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_EQUITY );
		exit( 1 );
	}

	/* Build net income subclassification. */
	/* ----------------------------------- */
	net_income =
		transaction_net_income_fetch(
			fund_name,
			as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			account_new(
				ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			account_new(
				ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_journal =
			journal_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_journal->balance = net_income;

	subclassification =
		subclassification_new(
			net_income_account->account_name );

	subclassification->account_list = list_new();

	list_set(	subclassification->account_list,
			net_income_account );

	list_set(	element->subclassification_list,
			subclassification );

	list_append_list(
		row_list,
		subclassification_display_latex_row_list(
			&total_equity,
			element->subclassification_list,
			element->element_name,
			element->accumulate_debit,
			total_assets /* percent_denominator */ ) );

	list_set(
		row_list,
		element_latex_liabilities_plus_equity_row(
			total_liabilities + total_equity,
			2 /* skip_columns */ ) );

	return row_list;
}

void balance_sheet_account_omit_html(
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				boolean is_financial_position )
{
	HTML_TABLE *html_table;
	LIST *filter_element_name_list;
	LIST *list;
	ELEMENT *element;
	ACCOUNT *net_income_account;
	double total_liabilities = {0};
	double total_equity = {0};
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
				ELEMENT_ASSET );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LIABILITY );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time */,
				transaction_closing_entry_exists(
					as_of_date ) ),
			0 /* not fetch_subclassifiction_list */,
			1 /* fetch_account_list */ );

	/* Output total_assets */
	/* ------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_ASSET,
				list ) ) )
	{
		/* Ignore total_assets */
		/* ------------------- */
		account_list_html_output(
			html_table,
			element->account_list
			element->element_name,
			element->accumulate_debit,
			element->element_total
				/* percent_denominator */ );
	}

	/* Calculate total_liabilities */
	/* --------------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_LIABILITY,
				list ) ) )
	{
		total_liabilities =
			account_list_html_output(
				html_table,
				element->account_list,
				element->element_name,
				element->accumulate_debit,
				element->element_total
					/* percent_denominator */ );
	}

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			element_seek(
				ELEMENT_EQUITY,
				list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_EQUITY );
	}

	/* Add net income to equity */
	/* ------------------------- */
	net_income =
		transaction_net_income_fetch(
			fund_name,
			as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			account_new(
				ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			account_new(
				ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_journal =
			journal_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	element->element_total +=
	net_income_account->latest_journal->balance = net_income;

	list_add_pointer_in_order(
		element->account_list,
		net_income_account,
		account_balance_match_function );

	total_equity =
		account_list_html_output(
			html_table,
			element->account_list,
			element->element_name,
			element->accumulate_debit,
			element->element_total
				/* percent_denominator */ );

	liabilities_plus_equity_html(
			html_table,
			total_liabilities + total_equity,
			1 /* aggregate_subclassification	*/
			  /* This shifts value one column left. */ );

	html_table_close();
}

void balance_sheet_account_omit_PDF(
				char *application_name,
				char *title,
				char *sub_title,
				char *fund_name,
				char *as_of_date,
				char *document_root_directory,
				char *process_name,
				boolean is_financial_position,
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
		build_account_omit_PDF_heading_list();

	filter_element_name_list = list_new();
	list_append_pointer(	filter_element_name_list,
				ELEMENT_ASSET );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LIABILITY );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time */,
				transaction_closing_entry_exists(
					as_of_date ) ),
			0 /* not fetch_subclassifiction_list */,
			1 /* fetch_account_list */ );

	latex_table->row_list =
		build_account_omit_PDF_row_list(
			list /* element_list */,
			fund_name,
			as_of_date,
			is_financial_position );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>%s</h2>\n", sub_title );

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
}

LIST *build_account_omit_PDF_row_list(
				LIST *element_list,
				char *fund_name,
				char *as_of_date,
				boolean is_financial_position )
{
	ELEMENT *element;
	double total_assets = 0.0;
	double total_liabilities = 0.0;
	double total_equity = 0.0;
	double net_income = 0.0;
	LIST *row_list;
	ACCOUNT *net_income_account;

	row_list = list_new();

	/* Compute total_assets */
	/* -------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_ASSET,
				element_list ) ) )
	{
		list_append_list(
			row_list,
			account_omit_latex_row_list(
				&total_assets,
				element->account_list,
				element->element_name,
				element->accumulate_debit,
				element->element_total
					/* percent_denominator */ ) );
	}

	/* Compute total_liabilities */
	/* ------------------------- */
	if ( ( element =
			element_seek(
				ELEMENT_LIABILITY,
				element_list ) ) )
	{
		list_append_list(
			row_list,
			account_omit_latex_row_list(
				&total_liabilities,
				element->account_list,
				element->element_name,
				element->accumulate_debit,
				element->element_total
					/* percent_denominator */ ) );
	}

	/* Calculate total_equity */
	/* ----------------------- */
	if ( ! ( element =
			element_seek(
				ELEMENT_EQUITY,
				element_list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_EQUITY );
	}

	/* Build net income account. */
	/* ------------------------- */
	net_income =
		transaction_net_income_fetch(
			fund_name,
			as_of_date );

	if ( is_financial_position )
	{
		net_income_account =
			account_new(
				ACCOUNT_CHANGE_IN_NET_ASSETS );
	}
	else
	{
		net_income_account =
			account_new(
				ACCOUNT_NET_INCOME );
	}

	net_income_account->accumulate_debit = 0;

	net_income_account->latest_journal =
			journal_new(
				(char *)0 /* full_name */,
				(char *)0 /* street_address */,
				(char *)0 /* transaction_date_time */,
				net_income_account->account_name );

	net_income_account->latest_journal->balance = net_income;

	list_add_pointer_in_order(
		element->account_list,
		net_income_account,
		account_balance_match_function );

	list_append_list(
		row_list,
		account_omit_latex_row_list(
			&total_equity,
			element->account_list,
			element->element_name,
			element->accumulate_debit,
			element->element_total +
			net_income /* percent_denominator */ ) );

	list_set(
			row_list,
			element_latex_liabilities_plus_equity_row(
				total_liabilities + total_equity,
				1 /* skip_columns */ ) );

	return row_list;
}
#endif
