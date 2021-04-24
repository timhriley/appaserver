/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/trial_balance.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "float.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "latex.h"
#include "date.h"
#include "date_convert.h"
#include "boolean.h"
#include "appaserver_link_file.h"
#include "transaction.h"
#include "subclassification.h"
#include "journal.h"
#include "account.h"
#include "element.h"
#include "predictive.h"
#include "statement.h"

/* Constants */
/* --------- */
#define DAYS_FOR_EMPHASIS		35
#define ROWS_BETWEEN_HEADING		10
#define PROMPT				"Press here to view statement."

/* Prototypes */
/* ---------- */

/* Returns static memory */
/* --------------------- */
char *html_table_account_title(
			char *account_name,
			char *full_name,
			double debit_amount,
			double credit_amount,
			char *transaction_date,
			char *memo );

void trial_balance_html_table(
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean postclose );

void trial_balance_html_table_subclassification(
			LIST *element_list,
			LIST *prior_year_list,
			char *title,
			char *subtitle,
			char *fund_name,
			double debit_total,
			double credit_total );

void trial_balance_html_table_subclassification_list(
			HTML_TABLE *html_table,
			LIST *prior_year_list,
			char *element_name,
			LIST *subclassification_list );

void trial_balance_html_table_account(
			LIST *element_list,
			LIST *prior_year_list,
			char *title,
			char *subtitle,
			char *fund_name,
			double debit_total,
			double credit_total );

void trial_balance_html_table_subclassification_account(
			HTML_TABLE *html_table,
			ACCOUNT *account,
			LIST *prior_year_list,
			char *element_name,
			char *subclassification_name );

void trial_balance_html_table_subclassification_account_name(
			LIST *data_list,
			char *element_name,
			char *subclassification_name,
			char *account_name,
			char *full_name,
			int transaction_count,
			double balance,
			char *transaction_date_time,
			char *memo,
			int number_left_justified_columns,
			int number_right_justified_columns,
			boolean background_shaded,
			LIST *justify_list,
			boolean accumulate_debit,
			double debit_amount,
			double credit_amount,
			int percent_of_total,
			char *action_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *fund_name;
	LIST *fund_name_list;
	char *subclassification_option_string;
	char *fund_aggregation_string;
	char *output_medium_string;
	char *as_of_date;
	int prior_year_count;
	char *logo_filename;
	STATEMENT *statement;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s session login_name role process fund as_of_date prior_year_count aggregation output_medium subclassification_option\n",
			 argv[ 0 ] );

		fprintf( stderr,
"Note: subclassification_option={aggregate,display,omit}\n" );

		fprintf( stderr,
"Note: aggregation={single_fund,sequential,consolidated}\n" );

		fprintf( stderr,
"Note: output_medium={table,PDF,stdout}\n" );

		exit ( 1 );
	}

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
	output_medium_string = argv[ 9 ];
	subclassification_option_string = argv[ 10 ];

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
		trial_balance_html_table(
			statement->statement_fund_list,
			statement->title,
			statement->subtitle,
			0 /* not postclose */ );

		if ( statement_exists_postclose(
			statement->statement_fund_list ) )
		{
			trial_balance_html_table(
				statement->statement_fund_list,
				statement->title,
				statement->subtitle,
				1 /* postclose */ );
		}
	}
	else
	if ( statement->statement_output_medium == output_PDF )
	{
/*
		trial_balance_PDF(
			application_name,
			fund_name,
			as_of_date,
			appaserver_parameter_file->
				document_root,
			process_name,
			fund_aggregation,
			logo_filename,
			fetch_subclassification_list,
			fetch_account_list );
*/
	}
	else
	{
/*
		trial_balance_stdout(
			fund_name,
			as_of_date,
			fetch_subclassification_list,
			fetch_account_list );
*/
	}

	if ( statement->statement_output_medium != output_stdout )
		document_close();

	return 0;
}

void trial_balance_html_table(
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean postclose )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		if ( postclose )
		{
			if (	statement_fund->subclassification_option ==
				subclassification_display )
			{
				trial_balance_html_table_subclassification(
					statement_fund->postclose_element_list,
					statement_fund->prior_year_list,
					title,
					subtitle,
					statement_fund->fund_name,
					statement_fund->
						postclose_debit_total,
					statement_fund->
						postclose_credit_total );
			}
			else
			if (	statement_fund->subclassification_option ==
				subclassification_omit )
			{
/*
				trial_balance_html_table_account(
					statement_fund->postclose_element_list,
					statement_fund->prior_year_list,
					title,
					subtitle,
					statement_fund->fund_name,
					statement_fund->
						postclose_debit_total,
					statement_fund->
						postclose_credit_total );
*/
			}
		}
		else
		{
			if (	statement_fund->subclassification_option ==
				subclassification_display )
			{
				trial_balance_html_table_subclassification(
					statement_fund->preclose_element_list,
					statement_fund->prior_year_list,
					title,
					subtitle,
					statement_fund->fund_name,
					statement_fund->
						preclose_debit_total,
					statement_fund->
						preclose_credit_total );
			}
			else
			if (	statement_fund->subclassification_option ==
				subclassification_omit )
			{
/*
				trial_balance_html_table_account(
					statement_fund->preclose_element_list,
					statement_fund->prior_year_list,
					title,
					subtitle,
					statement_fund->fund_name,
					statement_fund->
						preclose_debit_total,
					statement_fund->
						preclose_credit_total );
*/
			}
		}

	} while ( list_next( statement_fund_list ) );
}

void trial_balance_html_table_subclassification(
			LIST *element_list,
			LIST *prior_year_list,
			char *title,
			char *subtitle,
			char *fund_name,
			double debit_total,
			double credit_total )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	char *debit_total_string;
	char *credit_total_string;
	ELEMENT *element;
	int number_left_justified_columns = 3;

	/* Create the table heading */
	/* ------------------------ */
	heading_list = list_new();

	list_set( heading_list, "Element" );
	list_set( heading_list, "Subclassification" );
	list_set( heading_list, "Account" );
	list_set( heading_list, "Count" );
	list_set( heading_list, "Debit" );
	list_set( heading_list, "Credit" );
	list_set( heading_list, "Percent of Assets" );

	html_table =
		html_table_new(
			title,
			subtitle,
			fund_name );

	html_table->number_left_justified_columns =
		number_left_justified_columns;

	html_table->number_right_justified_columns = 4;
	html_table_set_heading_list( html_table, heading_list );

	html_table_output_table_heading(
		html_table->title,
		html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->justify_list );

	if ( !list_rewind( element_list ) )
	{
		html_table_close();
		return;
	}

	do {
		element = list_get( element_list );

		if ( !list_length( element->subclassification_list ) )
			continue;

		trial_balance_html_table_subclassification_list(
			html_table,
			prior_year_list,
			element->element_name,
			element->subclassification_list );

	} while( list_next( element_list ) );

	html_table_set_data( html_table->data_list, "Total" );
	html_table_set_data( html_table->data_list, "" );
	html_table_set_data( html_table->data_list, "" );
	html_table_set_data( html_table->data_list, "" );

	/* Returns static memory */
	/* --------------------- */
	debit_total_string = timlib_place_commas_in_money( debit_total );

	html_table_set_data(
		html_table->data_list,
		strdup( debit_total_string ) );

	/* Returns static memory */
	/* --------------------- */
	credit_total_string = timlib_place_commas_in_money( credit_total );

	html_table_set_data(
		html_table->data_list,
		strdup( credit_total_string ) );

	html_table_output_data(
		html_table->data_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	html_table_close();
}

void trial_balance_html_table_subclassification_list(
			HTML_TABLE *html_table,
			LIST *prior_year_list,
			char *element_name,
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	char *subclassification_name;

	if ( !list_rewind( subclassification_list ) ) return;

	do {
		subclassification =
			list_get(
				subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		subclassification_name =
			subclassification->
				subclassification_name;

		do {
			account = 
				list_get(
					subclassification->
						account_list );

			if ( !account->latest_journal
			||   !account->latest_journal->balance )
				continue;

			trial_balance_html_table_subclassification_account(
				html_table,
				account,
				prior_year_list,
				element_name,
				subclassification_name );

			list_free( html_table->data_list );
			html_table->data_list = list_new();

			subclassification_name = (char *)0;
			element_name = (char *)0;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );
}

void trial_balance_html_table_subclassification_account(
			HTML_TABLE *html_table,
			ACCOUNT *account,
			LIST *prior_year_list,
			char *element_name,
			char *subclassification_name )
{
	double balance;
	boolean accumulate_debit;

/* Stub */
/* ---- */
if ( prior_year_list ) {};

	if ( !account->latest_journal || !account->latest_journal->balance )
		return;

	balance = account->latest_journal->balance;
	accumulate_debit = account->accumulate_debit;

	/* See if negative balance. */
	/* ------------------------ */
	if ( balance < 0.0 )
	{
		balance = float_abs( balance );
		accumulate_debit = 1 - accumulate_debit;
	}

	trial_balance_html_table_subclassification_account_name(
		html_table->data_list,
		element_name,
		subclassification_name,
		account->account_name,
		account->latest_journal->full_name,
		account->latest_journal->transaction_count,
		balance,
		account->latest_journal->transaction_date_time,
		account->latest_journal->memo,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list,
		accumulate_debit,
		account->latest_journal->debit_amount,
		account->latest_journal->credit_amount,
		account->percent_of_total,
		account->account_action_string );
}

void trial_balance_html_table_subclassification_account_name(
			LIST *data_list,
			char *element_name,
			char *subclassification_name,
			char *account_name,
			char *full_name,
			int transaction_count,
			double balance,
			char *transaction_date_time,
			char *memo,
			int number_left_justified_columns,
			int number_right_justified_columns,
			boolean background_shaded,
			LIST *justify_list,
			boolean accumulate_debit,
			double debit_amount,
			double credit_amount,
			int percent_of_total,
			char *action_string )
{
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char *account_title;
	char transaction_count_string[ 16 ];
	char debit_string[ 4096 ];
	char credit_string[ 4096 ];
	char percent_of_total_string[ 16 ];
	char transaction_date_string[ 16 ];

	if ( element_name && *element_name )
	{
		format_initial_capital(
			element_title,
			element_name );

		html_table_set_data(
			data_list,
			strdup( element_title ) );
	}
	else
		html_table_set_data( data_list, strdup( "" ) );

	if ( subclassification_name && *subclassification_name )
	{
		format_initial_capital(
			subclassification_title,
			subclassification_name );

		html_table_set_data(
			data_list,
			strdup( subclassification_title )  );
	}

	account_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		html_table_account_title(
			account_name,
			full_name,
			debit_amount,
			credit_amount,
			column( transaction_date_string,
				0,
				transaction_date_time ),
			memo );

	html_table_set_data(
		data_list,
		strdup( account_title ) );

	sprintf( transaction_count_string, "%d", transaction_count );

	html_table_set_data(
		data_list,
		strdup( transaction_count_string ) );

	/* Set the debit account. */
	/* ---------------------- */
	if ( accumulate_debit )
	{
		sprintf( debit_string,
			 "<a href=\"%s\">%s</a>",
			 action_string,
			 /* --------------------- */
			 /* Returns static memory */
			 /* --------------------- */
			 timlib_place_commas_in_money( balance ) );
	}
	else
	{
		*debit_string = '\0';
	}

	html_table_set_data(
		data_list,
		strdup( debit_string ) );

	/* Set the credit account. */
	/* ----------------------- */
	if ( !accumulate_debit )
	{
		sprintf( credit_string,
			 "<a href=\"%s\">%s</a>",
			 action_string,
			 /* --------------------- */
			 /* Returns static memory */
			 /* --------------------- */
			 timlib_place_commas_in_money( balance ) );
	}
	else
	{
		*credit_string = '\0';
	}

	html_table_set_data(
		data_list,
		strdup( credit_string ) );

	/* Set percent of total */
	/* -------------------- */
	sprintf(percent_of_total_string,
		"%d%c",
		percent_of_total,
		'%' );

	html_table_set_data(
		data_list,
		strdup( percent_of_total_string ) );

	/* Output the row */
	/* -------------- */
	html_table_output_data(
		data_list,
		number_left_justified_columns,
		number_right_justified_columns,
		background_shaded,
		justify_list );
}

char *html_table_account_title(
			char *account_name,
			char *full_name,
			double debit_amount,
			double credit_amount,
			char *transaction_date,
			char *memo )
{
	static char account_title[ 128 ];
	char *ptr = account_title;
	char account_name_formatted[ 128 ];
	char full_name_formatted[ 128 ];
	char *transaction_amount_string;

	if ( !transaction_date || !*transaction_date )
		return "Can't get account title";

	if ( !full_name || !*full_name )
		return "Can't get full name";

	*ptr = '\0';

	format_initial_capital( account_name_formatted, account_name );
	ptr += sprintf( ptr, "%s", account_name_formatted );

	ptr += sprintf( ptr, " <br>(%s:", transaction_date );

	format_initial_capital( full_name_formatted, full_name );
	ptr += sprintf( ptr, " %s", full_name_formatted );

	if ( memo && *memo )
	{
		ptr += sprintf( ptr, "; %s", memo );
	}

	if ( debit_amount )
	{
		transaction_amount_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_money(
				debit_amount );
	}
	else
	if ( credit_amount )
	{
		transaction_amount_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_money(
				credit_amount );
	}
	else
	{
		transaction_amount_string = "Unknown";
	}

	ptr += sprintf( ptr, " $%s)", transaction_amount_string );

	return account_title;

}

#ifdef NOT_DEFINED
/* Returns count */
/* ------------- */
int trial_balance_html_table_account_list(
				double *debit_sum,
				double *credit_sum,
				HTML_TABLE *html_table,
				char *application_name,
				LIST *prior_element_list,
				char *element_name,
				LIST *account_list,
				double element_total,
				char *beginning_date,
				char *as_of_date,
				char *session,
				char *login_name,
				char *role_name,
				int count,
				boolean fetch_subclassification_list )
{
	ACCOUNT *account;
	double balance;
	boolean accumulate_debit;

	if ( !list_rewind( account_list ) ) return 0;

	do {
		account = list_get_pointer( account_list );

		if ( !account->latest_journal
		||   !account->latest_journal->balance )
			continue;

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

		trial_balance_account_html_table(
			&balance,
			&accumulate_debit,
			html_table,
			application_name,
			account,
			prior_element_list,
			element_name,
			(char *)0 /* subclassification_name */,
			element_total /* ratio_denominator */,
			beginning_date,
			as_of_date,
			session,
			login_name,
			role_name,
			fetch_subclassification_list );

		list_free( html_table->data_list );
		html_table->data_list = list_new();

		if ( accumulate_debit )
		{
			*debit_sum += balance;
		}
		else
		{
			*credit_sum += balance;
		}

		element_name = (char *)0;

	} while( list_next( account_list ) );

	return count;

}

/* Returns count */
/* ------------- */
int trial_balance_html_table_subclassification_list(
				double *debit_sum,
				double *credit_sum,
				HTML_TABLE *html_table,
				char *application_name,
				LIST *prior_element_list,
				char *element_name,
				boolean is_period_element,
				LIST *subclassification_list,
				char *beginning_date,
				char *as_of_date,
				char *session,
				char *login_name,
				char *role_name,
				int count,
				boolean fetch_subclassification_list )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	double balance;
	boolean accumulate_debit;
	double ratio_denominator;

	if ( !list_rewind( subclassification_list ) ) return 0;

	do {
		subclassification =
			list_get_pointer(
				subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		do {
			account = 
				list_get_pointer(
					subclassification->
						account_list );

			if ( !account->latest_journal
			||   !account->latest_journal->balance )
				continue;

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

			if ( is_period_element )
			{
				ratio_denominator =
					subclassification->
						subclassification_total;
			}
			else
			{
				ratio_denominator = 0.0;
			}

			trial_balance_account_html_table(
				&balance,
				&accumulate_debit,
				html_table,
				application_name,
				account,
				prior_element_list,
				element_name,
				subclassification->
					subclassification_name,
				ratio_denominator
					/* ratio_denominator */,
				beginning_date,
				as_of_date,
				session,
				login_name,
				role_name,
				fetch_subclassification_list );

			list_free( html_table->data_list );
			html_table->data_list = list_new();

			if ( accumulate_debit )
			{
				*debit_sum += balance;
			}
			else
			{
				*credit_sum += balance;
			}

			subclassification->
				subclassification_name =
					(char *)0;

			element_name = (char *)0;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return count;

}

void trial_balance_account_html_table(
					double *balance,
					boolean *accumulate_debit,
					HTML_TABLE *html_table,
					char *application_name,
					ACCOUNT *account,
					LIST *prior_element_list,
					char *element_name,
					char *subclassification_name,
					double ratio_denominator,
					char *beginning_date,
					char *as_of_date,
					char *session,
					char *login_name,
					char *role_name,
					boolean fetch_subclassification_list )
{
	double prior_balance_change;

	*accumulate_debit =
		account_accumulate_debit(
			account->subclassification_name );

	*balance = account->latest_journal->balance;

	prior_balance_change =
		trial_balance_get_prior_balance_change(
			prior_element_list,
			account->account_name,
			*balance /* current_balance */ );

	/* See if negative balance. */
	/* ------------------------ */
	if ( *balance < 0.0 )
	{
		*balance = float_abs( *balance );
		*accumulate_debit = 1 - *accumulate_debit;
	}

	output_html_table(
		html_table->data_list,
		element_name,
		subclassification_name,
		account->account_name,
		account->
			latest_journal->
			full_name,
		account->
			latest_journal->
			transaction_count,
		*balance,
		account->
			latest_journal->
			transaction_date_time,
		account->
			latest_journal->
			memo,
		html_table->
		 number_left_justified_columns,
		html_table->
		 number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list,
		*accumulate_debit,
		account->
			latest_journal->
			debit_amount,
		account->
			latest_journal->
			credit_amount,
		prior_balance_change,
		ratio_denominator,
		application_name,
		beginning_date,
		as_of_date,
		session,
		login_name,
		role_name,
		fetch_subclassification_list );
}

void trial_balance_PDF(
			char *application_name,
			char *fund_name,
			char *as_of_date,
			char *document_root_directory,
			char *process_name,
			char *fund_aggregation,
			char *logo_filename,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
{
	LATEX *latex;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	int pid = getpid();
	char title[ 256 ];
	char sub_title[ 256 ];
	LIST *fund_name_list;
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

	fund_name_list = transaction_fund_name_list();

	transaction_report_title_sub_title(
		title,
		sub_title,
		process_name,
		fund_name_list,
		transaction_beginning_date_string(
			as_of_date ),
		as_of_date,
		logo_filename );

	printf( "<h1>%s</h1>\n", title );

	latex = latex_new_latex(
			latex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_flag */,
			logo_filename );

	if ( list_rewind( fund_name_list )
	&&   strcmp( fund_aggregation, "sequential" ) == 0 )
	{
		do {
			fund_name = list_get_pointer( fund_name_list );

			transaction_report_title_sub_title(
				title,
				sub_title,
				process_name,
				(LIST *)0 /* fund_name_list */,
				transaction_beginning_date_string(
				    	as_of_date ),
				as_of_date,
				logo_filename );

			trial_balance_PDF_fund(
					latex,
					sub_title,
					fund_name,
					as_of_date,
					fetch_subclassification_list,
					fetch_account_list );

		} while( list_next( fund_name_list ) );
	}
	else
	{
		/* Either single fund or consolidated. */
		/* ----------------------------------- */
		trial_balance_PDF_fund(
				latex,
				sub_title,
				fund_name,
				as_of_date,
				fetch_subclassification_list,
				fetch_account_list );
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

void trial_balance_PDF_fund(
			LATEX *latex,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
{
	LATEX_TABLE *latex_table;
	LIST *current_element_list;
	LIST *prior_element_list;
	LIST *prior_filter_element_name_list;
	DATE *prior_closing_transaction_date;
	char *prior_closing_transaction_date_string = {0};

	printf( "<h2>%s</h2>\n", sub_title );

	latex_table =
		latex_new_latex_table(
			strdup( sub_title ) /* caption */ );

	list_append_pointer( latex->table_list, latex_table );

	latex_table->heading_list =
		build_PDF_heading_list(
			fetch_subclassification_list );

	/* Populate the current_element_list */
	/* --------------------------------- */
	current_element_list =
		element_list(
			(LIST *)0 /* filter_element_name_list */,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				0 /* not prior_closing_time_boolean */ ),
			fetch_subclassification_list,
			fetch_account_list );

	/* Populate the prior_element_list */
	/* ------------------------------- */
	prior_closing_transaction_date =
		transaction_prior_closing_transaction_date(
			as_of_date /* ending_transaction_date */ );

	if ( prior_closing_transaction_date )
	{
		prior_closing_transaction_date_string =
			date_get_yyyy_mm_dd_string(
				prior_closing_transaction_date );
	}
	else
	{
		prior_closing_transaction_date_string =
			transaction_date_prior_closing_beginning(
				as_of_date );
	}

	prior_filter_element_name_list = list_new();

	list_append_pointer(
		prior_filter_element_name_list, 
		ELEMENT_ASSET );

	list_append_pointer(
		prior_filter_element_name_list, 
		ELEMENT_LIABILITY );

	prior_element_list =
		element_list(
			prior_filter_element_name_list,
			fund_name,
			prior_closing_transaction_date_string,
			fetch_subclassification_list,
			fetch_account_list );

	latex_table->row_list =
		build_PDF_row_list(
			current_element_list,
			prior_element_list,
			fetch_subclassification_list );
}

LIST *build_PDF_row_list(	LIST *current_element_list,
				LIST *prior_element_list,
				boolean fetch_subclassification_list )
{
	LATEX_ROW *latex_row;
	LIST *row_list;
	char *debit_amount;
	char *credit_amount;
	double debit_sum = 0.0;
	double credit_sum = 0.0;
	ELEMENT *element;

	if ( !list_rewind( current_element_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		element = list_get_pointer( current_element_list );

		if ( list_length( element->subclassification_list ) )
		{
			list_append_list(
				row_list,
				build_subclassification_PDF_row_list(
					element->subclassification_list,
					&debit_sum,
					&credit_sum,
					element->element_name,
					prior_element_list ) );
		}
		else
		{
			list_append_list(
				row_list,
				build_account_PDF_row_list(
					element->account_list,
					&debit_sum,
					&credit_sum,
					element->element_name,
					element->element_total,
					prior_element_list ) );
		}
			
	} while( list_next( current_element_list ) );

	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	latex_append_column_data_list(
		latex_row->column_data_list,
		"Total",
		0 /* not large_bold */ );

	if ( fetch_subclassification_list )
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			(char *)0,
			0 /* not large_bold */ );
	}

	latex_append_column_data_list(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	latex_append_column_data_list(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	debit_amount = timlib_place_commas_in_dollars( debit_sum );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( debit_amount ),
		0 /* not large_bold */ );

	credit_amount = timlib_place_commas_in_dollars( credit_sum );

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( credit_amount ),
		0 /* not large_bold */ );

	return row_list;
}

LIST *build_account_PDF_row_list(
				LIST *account_list,
				double *debit_sum,
				double *credit_sum,
				char *element_name,
				double element_total,
				LIST *prior_element_list )
{
	ACCOUNT *account;
	LIST *row_list;
	LATEX_ROW *latex_row;
	double ratio_denominator;
	char element_title[ 128 ];
	boolean accumulate_debit;
	double balance;

	if ( element_is_period( element_name ) )
	{
		ratio_denominator = element_total;
	}
	else
	{
		ratio_denominator = 0.0;
	}

	if ( !list_rewind( account_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		account =
			list_get_pointer(
				account_list );

		if ( !account->latest_journal
		||   !account->latest_journal->balance )
			continue;

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		if ( element_name )
		{
			format_initial_capital(
				element_title,
				element_name );

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( element_title ),
				0 /* not large_bold */ );

			element_name = (char *)0;
		}
		else
		{
			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( "" ),
				0 /* not large_bold */ );
		}

		balance = 0.0;
		accumulate_debit = 0;

		build_PDF_account_row(
			latex_row->column_data_list,
			&accumulate_debit,
			&balance,
			account,
			prior_element_list,
			ratio_denominator );

		if ( accumulate_debit )
			*debit_sum += balance;
		else
			*credit_sum += balance;

	} while( list_next( account_list ) );

	return row_list;
}

LIST *build_subclassification_PDF_row_list(
				LIST *subclassification_list,
				double *debit_sum,
				double *credit_sum,
				char *element_name,
				LIST *prior_element_list )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	LIST *row_list;
	LATEX_ROW *latex_row;
	double ratio_denominator;
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	boolean accumulate_debit;
	double balance;
	boolean is_period_element;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	is_period_element = element_is_period( element_name );
	row_list = list_new();

	do {
		subclassification =
			list_get_pointer(
				subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		do {
			account = 
				list_get_pointer(
					subclassification->
						account_list );

			if ( !account->latest_journal
			||   !account->latest_journal->balance )
				continue;

			if ( is_period_element )
			{
				ratio_denominator =
					subclassification->
						subclassification_total;
			}
			else
			{
				ratio_denominator = 0.0;
			}

			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			if ( element_name )
			{
				format_initial_capital(
					element_title,
					element_name );

				element_name = (char *)0;
			}
			else
			{
				*element_title = '\0';
			}

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( element_title ),
				0 /* not large_bold */ );

			if ( subclassification->subclassification_name )
			{
				format_initial_capital(
					subclassification_title,
					subclassification->
					     subclassification_name );
			}
			else
			{
				*subclassification_title = '\0';
			}

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( subclassification_title ),
				0 /* not large_bold */ );

			accumulate_debit = 0;
			balance = 0.0;

			build_PDF_account_row(
				latex_row->column_data_list,
				&accumulate_debit,
				&balance,
				account,
				prior_element_list,
				ratio_denominator );

			if ( accumulate_debit )
				*debit_sum += balance;
			else
				*credit_sum += balance;

			subclassification->
				subclassification_name =
					(char *)0;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return row_list;
}

LIST *build_PDF_heading_list(
			boolean fetch_subclassification_list )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Element";
	table_heading->right_justified_flag = 0;
	list_append_pointer( heading_list, table_heading );

	if ( fetch_subclassification_list )
	{
		table_heading = latex_new_latex_table_heading();
		table_heading->right_justified_flag = 0;
		table_heading->heading = "Subclassification";
		list_append_pointer( heading_list, table_heading );
	}

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Account";
	table_heading->paragraph_size = "6.0cm";
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Count";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Debit";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "Credit";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	table_heading = latex_new_latex_table_heading();
	table_heading->heading = "$\\Delta$ or \\%";
	table_heading->right_justified_flag = 1;
	list_append_pointer( heading_list, table_heading );

	return heading_list;

}

void output_html_table(	LIST *data_list,
			char *element_name,
			char *subclassification_name,
			char *account_name,
			char *full_name,
			int transaction_count,
			double balance,
			char *transaction_date_time,
			char *memo,
			int number_left_justified_columns,
			int number_right_justified_columns,
			boolean background_shaded,
			LIST *justify_list,
			boolean accumulate_debit,
			double debit_amount,
			double credit_amount,
			double prior_balance_change,
			double ratio_denominator,
			char *application_name,
			char *beginning_date,
			char *as_of_date,
			char *session,
			char *login_name,
			char *role_name,
			boolean fetch_subclassification_list )
{
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char *account_title;
	char transaction_count_string[ 16 ];
	char debit_string[ 4096 ];
	char credit_string[ 4096 ];
	char *prior_balance_change_string;
	char account_ratio_string[ 16 ];
	char transaction_date_string[ 16 ];
	char *action_string;

	action_string =
		account_action_string(
			application_name,
			session,
			login_name,
			role_name,
			beginning_date,
			as_of_date,
			account_name );

	if ( element_name && *element_name )
	{
		format_initial_capital(
			element_title,
			element_name );

		html_table_set_data(
			data_list,
			strdup( element_title ) );
	}
	else
		html_table_set_data( data_list, strdup( "" ) );

	if ( subclassification_name && *subclassification_name )
	{
		format_initial_capital(
			subclassification_title,
			subclassification_name );

		html_table_set_data(
			data_list,
			strdup( subclassification_title )  );
	}
	else
	{
		if ( fetch_subclassification_list )
		{
			html_table_set_data( data_list, strdup( "" ) );
		}
	}

	account_title =
		html_table_account_title(
			account_name,
			full_name,
			debit_amount,
			credit_amount,
			column( transaction_date_string,
				0,
				transaction_date_time ),
			memo );

	html_table_set_data(
		data_list,
		strdup( account_title ) );

	sprintf( transaction_count_string, "%d", transaction_count );

	html_table_set_data(	data_list,
				strdup( transaction_count_string ) );

	/* Set the debit account. */
	/* ---------------------- */
	if ( accumulate_debit )
	{
		sprintf( debit_string,
			 "<a href=\"%s\">%s</a>",
			 action_string,
			 /* --------------------- */
			 /* Returns static memory */
			 /* --------------------- */
			 timlib_place_commas_in_money( balance ) );
	}
	else
	{
		*debit_string = '\0';
	}

	html_table_set_data(	data_list,
				strdup( debit_string ) );

	/* Set the credit account. */
	/* ----------------------- */
	if ( !accumulate_debit )
	{
		sprintf( credit_string,
			 "<a href=\"%s\">%s</a>",
			 action_string,
			 /* --------------------- */
			 /* Returns static memory */
			 /* --------------------- */
			 timlib_place_commas_in_money( balance ) );
	}
	else
	{
		*credit_string = '\0';
	}

	html_table_set_data(	data_list,
				strdup( credit_string ) );

	/* Set prior_balance_change (maybe) */
	/* -------------------------------- */
	if ( !timlib_dollar_virtually_same(
			prior_balance_change,
			0.0 ) )
	{
		char buffer[ 32 ];

		prior_balance_change_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_money(
				prior_balance_change );

		sprintf( buffer, "&Delta;%s", prior_balance_change_string );

		html_table_set_data(
			data_list,
			strdup( buffer ) );
	}
	else
	/* Set account ratio ratio (maybe) */
	/* ----------------------------------------- */
	if ( !timlib_dollar_virtually_same(
			ratio_denominator,
			0.0 ) )
	{
		sprintf( account_ratio_string,
			 "%.0lf%c",
			 (balance / ratio_denominator) * 100.0,
			 '%' );

		html_table_set_data(
			data_list,
			strdup( account_ratio_string ) );
	}

	/* Output the row */
	/* -------------- */
	html_table_output_data(
		data_list,
		number_left_justified_columns,
		number_right_justified_columns,
		background_shaded,
		justify_list );

}

char *get_latex_account_title(	char *account_name,
				char *full_name,
				double debit_amount,
				double credit_amount,
				char *transaction_date,
				char *memo )
{
	static char account_title[ 1024 ];
	char *ptr = account_title;
	char account_name_formatted[ 128 ];
	char full_name_formatted[ 128 ];
	char *transaction_amount_string;

	if ( !transaction_date || !*transaction_date )
		return "Can't get account title";

	*ptr = '\0';

	format_initial_capital( account_name_formatted, account_name );
	ptr += sprintf( ptr, "\\textbf{%s}", account_name_formatted );

	ptr += sprintf( ptr, " (\\scriptsize{%s:", transaction_date );

	format_initial_capital( full_name_formatted, full_name );
	ptr += sprintf( ptr, " %s", full_name_formatted );

	if ( memo && *memo )
	{
		ptr += sprintf( ptr, "; %s", memo );
	}

	if ( debit_amount )
	{
		transaction_amount_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_money(
				debit_amount );
	}
	else
	if ( credit_amount )
	{
		transaction_amount_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_money(
				credit_amount );
	}
	else
	{
		transaction_amount_string = "Unknown";
	}

	ptr += sprintf( ptr, " \\$%s})", transaction_amount_string );

	return account_title;

}

double trial_balance_get_prior_balance_change(
			LIST *prior_element_list,
			char *account_name,
			double current_balance )
{
	ACCOUNT *account;
	double balance;
	double balance_change;

	if ( ( account =
		element_account_seek(
			account_name,
			prior_element_list ) ) )
	{
		balance = account->latest_journal->balance;
		balance_change = current_balance - balance;

		if ( balance < 0.0 && balance_change < 0.0 )
			return 0.0 - balance_change;
		else
			return balance_change;
	}
	else
	{
		return 0.0;
	}

}

void build_PDF_account_row(	LIST *column_data_list,
				boolean *accumulate_debit,
				double *balance,
				ACCOUNT *account,
				LIST *prior_element_list,
				double ratio_denominator )
{
	char transaction_date_american[ 16 ];
	char transaction_date_string[ 16 ];
	char *account_title;
	char transaction_count_string[ 32 ];
	char *debit_string;
	char *credit_string;
	char *prior_balance_change_string;
	double prior_balance_change;
	char ratio_denominator_string[ 16 ];
	char *today_date_string;
	int days_between;

	today_date_string = date_get_now_yyyy_mm_dd( date_get_utc_offset() );

	*accumulate_debit =
		account_accumulate_debit(
			account->subclassification_name );

	date_convert_source_international(
		transaction_date_american,
		american,
 		column( transaction_date_string,
 			0,
 			account->
			latest_journal->
			transaction_date_time ) );

	account_title =
		get_latex_account_title(
			account->account_name,
			account->
				latest_journal->
				full_name,
			account->
				latest_journal->
				debit_amount,
			account->
				latest_journal->
				credit_amount,
			transaction_date_american,
			account->
				latest_journal->
				memo );

	latex_append_column_data_list(
		column_data_list,
		strdup( account_title ),
		0 /* not large_bold */ );

	sprintf( transaction_count_string,
		 "%d",
		 account->
			latest_journal->
			transaction_count );

	latex_append_column_data_list(
		column_data_list,
		strdup( transaction_count_string ),
		0 /* not large_bold */ );

	*balance = account->latest_journal->balance;

	prior_balance_change =
		trial_balance_get_prior_balance_change(
			prior_element_list,
			account->account_name,
			*balance /* current_balance */ );

	/* See if negative balance. */
	/* ------------------------ */
	if ( *balance < 0.0 )
	{
		*balance = float_abs( *balance );
		*accumulate_debit = 1 - *accumulate_debit;
	}

	days_between =
		date_days_between(
			transaction_date_string,
			today_date_string );

	if ( *accumulate_debit )
		debit_string = timlib_place_commas_in_dollars( *balance );
	else
		debit_string = "";

	latex_append_column_data_list(
		column_data_list,
		strdup( debit_string ),
		(days_between <= DAYS_FOR_EMPHASIS) /* large_bold */ );

	if ( !*accumulate_debit )
		credit_string = timlib_place_commas_in_dollars( *balance );
	else
		credit_string = "";

	latex_append_column_data_list(
		column_data_list,
		strdup( credit_string ),
		(days_between <= DAYS_FOR_EMPHASIS) /* large_bold */ );

	/* Set prior_balance_change (maybe) */
	/* -------------------------------- */
	if ( !timlib_dollar_virtually_same(
		prior_balance_change,
		0.0 ) )
	{
		char buffer[ 32 ];

		prior_balance_change_string =
			timlib_place_commas_in_dollars(
				prior_balance_change );

		sprintf( buffer, "$\\Delta$%s", prior_balance_change_string );

		latex_append_column_data_list(
			column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );
	}

	/* Set ratio denominator (maybe) */
	/* ----------------------------- */
	if ( !timlib_dollar_virtually_same(
			ratio_denominator,
			0.0 ) )
	{
		sprintf( ratio_denominator_string,
			 "%.0lf%c",
			 (*balance / ratio_denominator) * 100.0,
			 '%' );

		latex_append_column_data_list(
			column_data_list,
			strdup( ratio_denominator_string ),
			0 /* not large_bold */ );
	}
}

void trial_balance_stdout(
			char *fund_name,
			char *as_of_date,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
{
	LIST *heading_list;
	char *debit_string;
	char *credit_string;
	double debit_sum = 0.0;
	double credit_sum = 0.0;
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	boolean accumulate_debit;
	double balance;
	LIST *current_element_list;
	LIST *prior_element_list;
	LIST *prior_filter_element_name_list;
	DATE *prior_closing_transaction_date;
	char *prior_closing_transaction_date_string = {0};
	double ratio_denominator;
	char *element_name = {0};
	LIST *data_list = list_new();

	/* Populate the current_element_list */
	/* --------------------------------- */
	current_element_list =
		element_list(
			(LIST *)0 /* filter_element_name_list */,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				0 /* not prior_closing_time_boolean */ ),
			fetch_subclassification_list,
			fetch_account_list );

	/* Populate the prior_element_list */
	/* ------------------------------- */
	prior_closing_transaction_date =
		transaction_prior_closing_transaction_date(
			as_of_date /* ending_transaction_date */ );

	if ( prior_closing_transaction_date )
	{
		prior_closing_transaction_date_string =
			date_get_yyyy_mm_dd_string(
				prior_closing_transaction_date );
	}
	else
	{
		prior_closing_transaction_date_string =
			transaction_date_prior_closing_beginning(
				as_of_date );
	}

	prior_filter_element_name_list = list_new();

	list_append_pointer(
		prior_filter_element_name_list,
		ELEMENT_ASSET );

	list_append_pointer(
		prior_filter_element_name_list,
		ELEMENT_LIABILITY );

	prior_element_list =
		element_list(
			prior_filter_element_name_list,
			fund_name,
			prior_closing_transaction_date_string,
			fetch_subclassification_list,
			fetch_account_list );

	/* Create the table heading */
	/* ------------------------ */
	heading_list = list_new();
	list_append_string( heading_list, "Element" );
	list_append_string( heading_list, "Subclassification" );
	list_append_string( heading_list, "Account" );
	list_append_string( heading_list, "Count" );
	list_append_string( heading_list, "Debit" );
	list_append_string( heading_list, "Credit" );
	list_append_string( heading_list, "change_or_percent" );
	
	printf( "%s\n", list_display_delimited( heading_list, '^' ) );

	if ( !list_rewind( current_element_list ) )
	{
		printf(
	"ERROR: there are no elements for this statement.\n" );
		exit( 1 );
	}

	do {
		element = list_get_pointer( current_element_list );

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		element_name = element->element_name;

		do {
			subclassification =
				list_get_pointer(
					element->
					   subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;

			do {
				account = 
					list_get_pointer(
						subclassification->
							account_list );

				if ( !account->latest_journal
				||   !account->latest_journal->balance )
					continue;

				if ( element_is_period(
					element->element_name ) )
				{
					ratio_denominator =
						subclassification->
							subclassification_total;
				}
				else
				{
					ratio_denominator = 0.0;
				}

				trial_balance_account_stdout(
					&balance,
					&accumulate_debit,
					account,
					prior_element_list,
					element_name,
					subclassification->
						subclassification_name,
					ratio_denominator );

				if ( accumulate_debit )
				{
					debit_sum += balance;
				}
				else
				{
					credit_sum += balance;
				}

				subclassification->
					subclassification_name =
						(char *)0;

				element_name = (char *)0;

			} while( list_next( subclassification->account_list ) );

		} while( list_next( element->subclassification_list ) );

	} while( list_next( current_element_list ) );

	list_append_pointer( data_list, "Total" );
	list_append_pointer( data_list, "" );
	list_append_pointer( data_list, "" );
	list_append_pointer( data_list, "" );

	/* Returns static memory */
	/* --------------------- */
	debit_string = timlib_place_commas_in_money( debit_sum );
	list_set( data_list, strdup( debit_string ) );

	/* Returns static memory */
	/* --------------------- */
	credit_string = timlib_place_commas_in_money( credit_sum );
	list_set( data_list, strdup( credit_string ) );

	printf( "%s\n", list_display_delimited( data_list, '^' ) );
}

void trial_balance_account_stdout(
					double *balance,
					boolean *accumulate_debit,
					ACCOUNT *account,
					LIST *prior_element_list,
					char *element_name,
					char *subclassification_name,
					double ratio_denominator )
{
	double prior_balance_change;

	*accumulate_debit =
		account_accumulate_debit(
			account->subclassification_name );

	*balance = account->latest_journal->balance;

	prior_balance_change =
		trial_balance_get_prior_balance_change(
			prior_element_list,
			account->account_name,
			*balance /* current_balance */ );

	/* See if negative balance. */
	/* ------------------------ */
	if ( *balance < 0.0 )
	{
		*balance = float_abs( *balance );
		*accumulate_debit = 1 - *accumulate_debit;
	}

	output_stdout(
		element_name,
		subclassification_name,
		account->account_name,
		account->
			latest_journal->
			full_name,
		account->
			latest_journal->
			transaction_count,
		*balance,
		account->
			latest_journal->
			transaction_date_time,
		account->
			latest_journal->
			memo,
		*accumulate_debit,
		account->
			latest_journal->
			debit_amount,
		account->
			latest_journal->
			credit_amount,
		prior_balance_change,
		ratio_denominator );
}

void output_stdout(	char *element_name,
			char *subclassification_name,
			char *account_name,
			char *full_name,
			int transaction_count,
			double balance,
			char *transaction_date_time,
			char *memo,
			boolean accumulate_debit,
			double debit_amount,
			double credit_amount,
			double prior_balance_change,
			double ratio_denominator )
{
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char *account_title;
	char transaction_count_string[ 16 ];
	char *debit_string;
	char *credit_string;
	char *prior_balance_change_string;
	char ratio_denominator_string[ 16 ];
	char transaction_date_string[ 16 ];
	LIST *data_list = list_new();

	if ( element_name && *element_name )
	{
		format_initial_capital(
			element_title,
			element_name );

		list_append_pointer(
			data_list,
			strdup( element_title ) );
	}
	else
		list_append_pointer( data_list, strdup( "" ) );

	if ( subclassification_name && *subclassification_name )
	{
		format_initial_capital(
			subclassification_title,
			subclassification_name );

		list_append_pointer(
			data_list,
			strdup( subclassification_title )  );
	}
	else
		list_append_pointer( data_list, strdup( "" ) );

	account_title =
		html_table_account_title(
			account_name,
			full_name,
			debit_amount,
			credit_amount,
			column( transaction_date_string,
				0,
				transaction_date_time ),
			memo );

	list_append_pointer(
		data_list,
		strdup( account_title ) );

	sprintf( transaction_count_string, "%d", transaction_count );

	list_append_pointer(	data_list,
				strdup( transaction_count_string ) );

	/* Set the debit account. */
	/* ---------------------- */
	if ( accumulate_debit )
	{
		/* Returns static memory */
		/* --------------------- */
		debit_string = timlib_place_commas_in_money( balance );
	}
	else
		debit_string = "";

	list_append_pointer(	data_list,
				strdup( debit_string ) );

	/* Set the credit account. */
	/* ----------------------- */
	if ( !accumulate_debit )
	{
		/* Returns static memory */
		/* --------------------- */
		credit_string = timlib_place_commas_in_money( balance );
	}
	else
		credit_string = "";

	list_append_pointer(	data_list,
				strdup( credit_string ) );

	/* Set prior_balance_change (maybe) */
	/* -------------------------------- */
	if ( !timlib_dollar_virtually_same(
			prior_balance_change,
			0.0 ) )
	{
		char buffer[ 32 ];

		prior_balance_change_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_money(
				prior_balance_change );

		sprintf( buffer, "&Delta;%s", prior_balance_change_string );

		list_append_pointer(
			data_list,
			strdup( buffer ) );
	}
	else
	/* Set ratio denominator (maybe) */
	/* ----------------------------- */
	if ( !timlib_dollar_virtually_same(
			ratio_denominator,
			0.0 ) )
	{
		sprintf( ratio_denominator_string,
			 "%.0lf%c",
			 (balance / ratio_denominator) * 100.0,
			 '%' );

		list_append_pointer(
			data_list,
			strdup( ratio_denominator_string ) );
	}

	printf( "%s\n", list_display_delimited( data_list, '^' ) );

}

void trial_balance_html_table(
			char *application_name,
			char *title,
			char *sub_title,
			char *fund_name,
			char *as_of_date,
			char *session,
			char *login_name,
			char *role_name,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
{
	HTML_TABLE *html_table;
	LIST *heading_list;
	char *debit_string;
	char *credit_string;
	double debit_sum = 0.0;
	double credit_sum = 0.0;
	ELEMENT *element;
	LIST *current_element_list;
	LIST *prior_element_list;
	LIST *prior_filter_element_name_list;
	DATE *prior_closing_transaction_date;
	char *prior_closing_transaction_date_string = {0};
	int count = 0;
	char *element_name = {0};
	char *beginning_date;
	int number_left_justified_columns = 3;
	double element_total;

	if ( timlib_strlen( as_of_date ) == 10 )
	{
		char buffer[ 32 ];

		sprintf( buffer,
			 "%s %s",
			 as_of_date,
			 TRANSACTION_CLOSING_TRANSACTION_TIME );

		as_of_date = strdup( buffer );
	}

	if ( ! ( beginning_date = 
			transaction_date_prior_closing_beginning(
				as_of_date ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get beginning_date.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Populate the current_element_list */
	/* --------------------------------- */
	current_element_list =
		element_list(
			(LIST *)0 /* filter_element_name_list */,
			fund_name,
			transaction_date_time_closing(
				as_of_date,
				0 /* not prior_closing_time_boolean */ ),
			fetch_subclassification_list,
			fetch_account_list );

	/* Populate the prior_element_list */
	/* ------------------------------- */
	prior_closing_transaction_date =
		transaction_prior_closing_transaction_date(
			as_of_date /* ending_transaction_date */ );

	if ( prior_closing_transaction_date )
	{
		prior_closing_transaction_date_string =
			date_get_yyyy_mm_dd_string(
				prior_closing_transaction_date );
	}
	else
	{
		prior_closing_transaction_date_string =
			transaction_date_prior_closing_beginning(
				as_of_date );
	}

	prior_filter_element_name_list = list_new();

	list_append_pointer(
		prior_filter_element_name_list, 
		ELEMENT_ASSET );

	list_append_pointer(
		prior_filter_element_name_list, 
		ELEMENT_LIABILITY );

	prior_element_list =
		element_list(
			prior_filter_element_name_list,
			fund_name,
			prior_closing_transaction_date_string,
			fetch_subclassification_list,
			fetch_account_list );

	/* Create the table heading */
	/* ------------------------ */
	heading_list = list_new();

	list_append_string( heading_list, "Element" );

	if ( fetch_subclassification_list )
	{
		list_append_string(
			heading_list,
			"Subclassification" );
	}
	else
	{
		number_left_justified_columns--;
	}
	
	list_append_string( heading_list, "Account" );
	list_append_string( heading_list, "Count" );
	list_append_string( heading_list, "Debit" );
	list_append_string( heading_list, "Credit" );

	if ( fetch_subclassification_list )
	{
		list_append_string(
			heading_list,
			"change_or_percent_of_subclassification" );
	}
	else
	{
		list_append_string(
			heading_list,
			"change_or_percent_of_element" );
	}

	html_table = new_html_table(
			title,
			sub_title );

	html_table->number_left_justified_columns =
		number_left_justified_columns;

	html_table->number_right_justified_columns = 4;
	html_table_set_heading_list( html_table, heading_list );

	html_table_output_table_heading(
		html_table->title,
		html_table->sub_title );

	html_table_output_data_heading(
		html_table->heading_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->justify_list );

	if ( !list_rewind( current_element_list ) )
	{
		printf(
	"<h3>ERROR: there are no elements for this statement.</h3>\n" );
		document_close();
		exit( 1 );
	}

	do {
		element = list_get_pointer( current_element_list );

		element_name = element->element_name;

		if ( fetch_account_list )
		{
			if ( !list_length( element->account_list ) )
				continue;

			if ( element_is_period(
					element->element_name ) )
			{
				element_total = element->element_total;
			}
			else
			{
				element_total = 0.0;
			}

			count =
			   trial_balance_html_table_account_list(
				&debit_sum,
				&credit_sum,
				html_table,
				application_name,
				prior_element_list,
				element_name,
				element->account_list,
				element_total,
				beginning_date,
				as_of_date,
				session,
				login_name,
				role_name,
				count,
				fetch_subclassification_list );
		}
		else
		{
			if ( !list_length( element->subclassification_list ) )
				continue;

			count =
			   trial_balance_html_table_subclassification_list(
				&debit_sum,
				&credit_sum,
				html_table,
				application_name,
				prior_element_list,
				element_name,
				element_is_period(
					element_name ),
				element->subclassification_list,
				beginning_date,
				as_of_date,
				session,
				login_name,
				role_name,
				count,
				fetch_subclassification_list );
		}

	} while( list_next( current_element_list ) );

	html_table_set_data( html_table->data_list, "Total" );
	html_table_set_data( html_table->data_list, "" );

	if ( fetch_subclassification_list )
		html_table_set_data( html_table->data_list, "" );

	html_table_set_data( html_table->data_list, "" );

	/* Returns static memory */
	/* --------------------- */
	debit_string = timlib_place_commas_in_money( debit_sum );
	html_table_set_data( html_table->data_list, strdup( debit_string ) );

	/* Returns static memory */
	/* --------------------- */
	credit_string = timlib_place_commas_in_money( credit_sum );
	html_table_set_data( html_table->data_list, strdup( credit_string ) );

	html_table_output_data(
		html_table->data_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	html_table_close();

}
#endif
