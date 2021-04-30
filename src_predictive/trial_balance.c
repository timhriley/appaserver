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
void pipe_account_total(
			FILE *output_pipe,
			char *element_name,
			ACCOUNT *account,
			LIST *prior_year_list,
			char delimiter );

void pipe_account_list(
			FILE *output_pipe,
			LIST *prior_year_list,
			char *element_name,
			LIST *account_list,
			char delimiter );

void pipe_account(	FILE *output_pipe,
			LIST *element_list,
			LIST *prior_year_list,
			char *fund_name,
			char delimiter );

void pipe_subclassification_account(
			FILE *output_pipe,
			char *element_name,
			char *subclassification_name,
			ACCOUNT *account,
			LIST *prior_year_list,
			char delimiter );

void pipe_subclassification_list(
			FILE *output_pipe,
			LIST *prior_year_list,
			char *element_name,
			LIST *subclassification_list,
			char delimiter );

void pipe_subclassification(
			FILE *output_pipe,
			LIST *element_list,
			LIST *prior_year_list,
			char *fund_name,
			char delimiter );

void trial_balance_stdout(
			LIST *statement_fund_list );

LIST *html_heading_list(
			LIST *prior_year_list,
			boolean include_subclassification );

/* Returns static memory */
/* --------------------- */
char *html_table_account_title(
			char *account_name,
			char *full_name,
			double debit_amount,
			double credit_amount,
			char *transaction_date,
			char *memo );

void trial_balance_html(
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean postclose );

void html_subclassification(
			LIST *element_list,
			LIST *prior_year_list,
			char *title,
			char *subtitle,
			char *fund_name,
			double debit_total,
			double credit_total );

/* Returns count */
/* ------------- */
int html_subclassification_list(
			HTML_TABLE *html_table,
			LIST *prior_year_list,
			char *element_name,
			LIST *subclassification_list );

void html_account(
			LIST *element_list,
			LIST *prior_year_list,
			char *title,
			char *subtitle,
			char *fund_name,
			double debit_total,
			double credit_total );

void html_subclassification_account(
			HTML_TABLE *html_table,
			char *element_name,
			char *subclassification_name,
			ACCOUNT *account,
			LIST *prior_year_list );

/* Returns count */
/* ------------- */
int html_account_list(
			HTML_TABLE *html_table,
			LIST *prior_year_list,
			char *element_name,
			LIST *account_list );

void html_account_total(
			HTML_TABLE *html_table,
			char *element_name,
			ACCOUNT *account,
			LIST *prior_year_list );

void trial_balance_PDF(
			char *application_name,
			char *document_root_directory,
			char *process_name,
			char *logo_filename,
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean postclose,
			enum subclassification_option,
			enum fund_aggregation );

LATEX_TABLE *PDF_subclassification_latex_table(
			char *subtitle,
			STATEMENT_FUND *statement_fund,
			boolean postclose,
			double debit_total,
			double credit_total,
			char *today_date_string );

LATEX_TABLE *PDF_account_latex_table(
			char *subtitle,
			STATEMENT_FUND *statement_fund,
			boolean postclose,
			double debit_total,
			double credit_total,
			char *today_date_string );

LIST *PDF_subclassification_row_list(
			LIST *element_list,
			LIST *prior_year_list,
			double debit_total,
			double credit_total,
			char *today_date_string );

LIST *PDF_account_row_list(
			LIST *element_list,
			LIST *prior_year_list,
			double debit_total,
			double credit_total,
			char *today_date_string );

LIST *PDF_subclassification_element_row_list(
			char *element_name,
			LIST *subclassification_list,
			LIST *prior_year_list,
			char *today_date_string );

LIST *PDF_account_element_row_list(
			char *element_name,
			LIST *account_list,
			LIST *prior_year_list,
			char *today_date_string );

LIST *PDF_subclassification_account_list_row_list(
			char *element_name,
			char *subclassification_name,
			LIST *account_list,
			LIST *prior_year_list,
			char *today_date_string );

LATEX_ROW *PDF_account_latex_row(
			char *element_name,
			char *subclassification_name,
			ACCOUNT *account,
			LIST *prior_year_list,
			char *today_date_string,
			boolean include_subclassification );

LIST *PDF_heading_list( LIST *prior_year_list,
			boolean include_subclassification );

/* Returns static memory */
/* --------------------- */
char *PDF_account_title(
			char *account_name,
			char *full_name,
			double debit_amount,
			double credit_amount,
			char *transaction_date,
			char *memo );

int main( int argc, char **argv )
{
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
"Note: subclassification_option={display,omit}\n" );

		fprintf( stderr,
"Note: fund_aggregation={sequential,consolidated}\n" );

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
		trial_balance_html(
			statement->statement_fund_list,
			statement->title,
			statement->subtitle,
			0 /* not postclose */ );

		if ( statement_fund_exists_postclose(
			statement->statement_fund_list ) )
		{
			trial_balance_html(
				statement->statement_fund_list,
				statement->title,
				statement->subtitle,
				1 /* postclose */ );
		}
	}
	else
	if ( statement->statement_output_medium == output_PDF )
	{
		trial_balance_PDF(
			application_name,
			appaserver_parameter_file->document_root,
			process_name,
			logo_filename,
			statement->statement_fund_list,
			statement->title,
			statement->subtitle,
			0 /* not postclose */,
			statement->statement_subclassification_option,
			statement->statement_fund_aggregation );

		if ( statement_fund_exists_postclose(
			statement->statement_fund_list ) )
		{
			trial_balance_PDF(
				application_name,
				appaserver_parameter_file->document_root,
				process_name,
				logo_filename,
				statement->statement_fund_list,
				statement->title,
				statement->subtitle,
				1 /* postclose */,
				statement->statement_subclassification_option,
				statement->statement_fund_aggregation );
		}
	}
	else
	{
		trial_balance_stdout(
			statement->statement_fund_list );
	}

	if ( statement->statement_output_medium != output_stdout )
		document_close();

	return 0;
}

void trial_balance_stdout(
			LIST *statement_fund_list )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		if (	statement_fund->subclassification_option ==
			subclassification_omit )
		{
			pipe_account(
				stdout,
				statement_fund->preclose_element_list,
				statement_fund->prior_year_list,
				statement_fund->fund_name,
				'^' );
		}
		else
		if (	statement_fund->subclassification_option ==
			subclassification_display )
		{
			pipe_subclassification(
				stdout,
				statement_fund->preclose_element_list,
				statement_fund->prior_year_list,
				statement_fund->fund_name,
				'^' );
		}
		else
		if (	statement_fund->subclassification_option ==
			subclassification_omit )
		{
			pipe_subclassification(
				stdout,
				statement_fund->preclose_element_list,
				statement_fund->prior_year_list,
				statement_fund->fund_name,
				'^' );
		}

	} while ( list_next( statement_fund_list ) );
}

void trial_balance_html(
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
			/* ------------------------------------ */
			/* Warning: altering static memory.	*/
			/* Must be the last to display.		*/
			/* ------------------------------------ */
			sprintf(title + strlen( title ),
				"(Post Close)" );

			if (	statement_fund->subclassification_option ==
				subclassification_display )
			{
				html_subclassification(
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
				html_account(
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
		}
		else
		{
			if (	statement_fund->subclassification_option ==
				subclassification_display )
			{
				html_subclassification(
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
				html_account(
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
		}

	} while ( list_next( statement_fund_list ) );
}

void pipe_subclassification(
			FILE *output_pipe,
			LIST *element_list,
			LIST *prior_year_list,
			char *fund_name,
			char delimiter )
{
	ELEMENT *element;

	if ( fund_name && *fund_name )
	{
		fprintf( output_pipe, "%s\n", fund_name );
	}

	fprintf(output_pipe,
		"%s\n",
		list_display_delimited(
			html_heading_list(
				prior_year_list,
				1 /* include_subclassification */ ),
			delimiter ) );

	if ( !list_rewind( element_list ) )
	{
		return;
	}

	do {
		element = list_get( element_list );

		if ( !list_length( element->subclassification_list ) )
			continue;

		pipe_subclassification_list(
			output_pipe,
			prior_year_list,
			element->element_name,
			element->subclassification_list,
			delimiter );

	} while( list_next( element_list ) );
}

void html_subclassification(
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
	int count;

	heading_list =
		html_heading_list(
			prior_year_list,
			1 /* include_subclassification */ );

	html_table =
		html_table_new(
			title,
			subtitle,
			fund_name );

	html_table->number_left_justified_columns = 3;
	html_table->number_right_justified_columns = 99;
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

	count = 0;

	do {
		element = list_get( element_list );

		if ( !list_length( element->subclassification_list ) )
			continue;

		if ( count >= ROWS_BETWEEN_HEADING )
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

		count +=
			html_subclassification_list(
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

void pipe_subclassification_list(
			FILE *output_pipe,
			LIST *prior_year_list,
			char *element_name,
			LIST *subclassification_list,
			char delimiter )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( subclassification_list ) ) return;

	do {
		subclassification =
			list_get(
				subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		do {
			account = 
				list_get(
					subclassification->
						account_list );

			if ( !account->account_total ) continue;

			pipe_subclassification_account(
				output_pipe,
				element_name,
				subclassification->subclassification_name,
				account,
				prior_year_list,
				delimiter );

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );
}

int html_subclassification_list(
			HTML_TABLE *html_table,
			LIST *prior_year_list,
			char *element_name,
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	char *subclassification_name;
	int count;

	if ( !list_rewind( subclassification_list ) ) return 0;

	count = 0;

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

			if ( !account->account_total ) continue;

			html_subclassification_account(
				html_table,
				element_name,
				subclassification_name,
				account,
				prior_year_list );

			list_free( html_table->data_list );
			html_table->data_list = list_new();

			subclassification_name = (char *)0;
			element_name = (char *)0;
			count++;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return count;
}

void html_subclassification_account(
			HTML_TABLE *html_table,
			char *element_name,
			char *subclassification_name,
			ACCOUNT *account,
			LIST *prior_year_list )
{
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char *account_title;
	char transaction_count_string[ 16 ];
	char debit_string[ 4096 ];
	char credit_string[ 4096 ];
	char percent_of_assets_string[ 16 ];
	char percent_of_revenues_string[ 16 ];
	char transaction_date_string[ 16 ];
	double balance;
	boolean accumulate_debit;
	LIST *data_list;

	if ( !account->account_total ) return;
	if ( !account->latest_journal ) return;

	data_list = list_new();

	balance = account->account_total;
	accumulate_debit = account->accumulate_debit;

	/* See if negative balance. */
	/* ------------------------ */
	if ( balance < 0.0 )
	{
		balance = float_abs( balance );
		accumulate_debit = 1 - accumulate_debit;
	}

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
	{
		html_table_set_data( data_list, strdup( "" ) );
	}

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
		html_table_set_data( data_list, strdup( "" ) );
	}

	account_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		html_table_account_title(
			account->account_name,
			account->latest_journal->full_name,
			account->latest_journal->debit_amount,
			account->latest_journal->credit_amount,
			column( transaction_date_string,
				0,
				account->
					latest_journal->
					transaction_date_time ),
			account->latest_journal->memo );

	html_table_set_data(
		data_list,
		strdup( account_title ) );

	sprintf(transaction_count_string,
		"%d",
		account->latest_journal->transaction_count );

	html_table_set_data(
		data_list,
		strdup( transaction_count_string ) );

	/* Set the debit account. */
	/* ---------------------- */
	if ( accumulate_debit )
	{
		sprintf( debit_string,
			 "<a href=\"%s\">%s</a>",
			 account->account_action_string,
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
			 account->account_action_string,
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

	/* Set percent of assets */
	/* -------------------- */
	sprintf(percent_of_assets_string,
		"%d%c",
		account->percent_of_assets,
		'%' );

	html_table_set_data(
		data_list,
		strdup( percent_of_assets_string ) );

	/* Set percent of revenues */
	/* ----------------------- */
	sprintf(percent_of_revenues_string,
		"%d%c",
		account->percent_of_revenues,
		'%' );

	html_table_set_data(
		data_list,
		strdup( percent_of_revenues_string ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			data_list,
			statement_html_prior_year_delta_list(
				account->account_name,
				prior_year_list ) );
	}

	/* Output the row */
	/* -------------- */
	html_table_output_data(
		data_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );
}

void pipe_subclassification_account(
			FILE *output_pipe,
			char *element_name,
			char *subclassification_name,
			ACCOUNT *account,
			LIST *prior_year_list,
			char delimiter )
{
	double balance;
	boolean accumulate_debit;

	if ( !account->account_total ) return;
	if ( !account->latest_journal ) return;

	balance = account->account_total;
	accumulate_debit = account->accumulate_debit;

	/* See if negative balance. */
	/* ------------------------ */
	if ( balance < 0.0 )
	{
		balance = float_abs( balance );
		accumulate_debit = 1 - accumulate_debit;
	}

	printf( "%s^%s^%s^%d",
		element_name,
		subclassification_name,
		account->account_name,
		account->latest_journal->transaction_count );

	/* Set the debit account. */
	/* ---------------------- */
	if ( accumulate_debit )
	{
		printf( "^%s",
			timlib_place_commas_in_money(
				balance ) );
	}
	else
	{
		printf( "^" );
	}

	if ( !accumulate_debit )
	{
		printf( "^%s",
			timlib_place_commas_in_money(
				balance ) );
	}
	else
	{
		printf( "^" );
	}

	fprintf(output_pipe,
		"%c%d%c%d",
		delimiter,
		account->percent_of_assets,
		delimiter,
		account->percent_of_revenues );

	if ( list_length( prior_year_list ) )
	{
		fprintf(output_pipe,
			"%c%s\n",
			delimiter,
			list_display_delimited(
				statement_prior_year_delta_list(
					account->account_name,
					prior_year_list ),
				delimiter ) );
	}
	else
	{
		fprintf( output_pipe, "\n" );
	}
}

char *html_table_account_title(
			char *account_name,
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

void pipe_account(	FILE *output_pipe,
			LIST *element_list,
			LIST *prior_year_list,
			char *fund_name,
			char delimiter )
{
	ELEMENT *element;

	if ( fund_name && *fund_name )
	{
		fprintf( output_pipe, "%s\n", fund_name );
	}

	fprintf(output_pipe,
		"%s\n",
		list_display_delimited(
			html_heading_list(
				prior_year_list,
				0 /* not include_subclassification */ ),
			delimiter ) );

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		if ( !list_length( element->account_list ) )
			continue;

		pipe_account_list(
			output_pipe,
			prior_year_list,
			element->element_name,
			element->account_list,
			delimiter );

	} while( list_next( element_list ) );
}

void html_account(
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
	int count;

	heading_list =
		html_heading_list(
			prior_year_list,
			0 /* not include_subclassification */ );

	html_table =
		html_table_new(
			title,
			subtitle,
			fund_name );

	html_table->number_left_justified_columns = 2;
	html_table->number_right_justified_columns = 99;
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

	count = 0;

	do {
		element = list_get( element_list );

		if ( !list_length( element->account_list ) )
			continue;

		if ( count >= ROWS_BETWEEN_HEADING )
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

		count +=
			html_account_list(
				html_table,
				prior_year_list,
				element->element_name,
				element->account_list );

	} while( list_next( element_list ) );

	html_table_set_data( html_table->data_list, "Total" );
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

void pipe_account_list(
			FILE *output_pipe,
			LIST *prior_year_list,
			char *element_name,
			LIST *account_list,
			char delimiter )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return;

	do {
		account =
			list_get(
				account_list );

		if ( !account->account_total ) continue;

		pipe_account_total(
			output_pipe,
			element_name,
			account,
			prior_year_list,
			delimiter );

	} while( list_next( account_list ) );
}

int html_account_list(
			HTML_TABLE *html_table,
			LIST *prior_year_list,
			char *element_name,
			LIST *account_list )
{
	ACCOUNT *account;
	int count;

	if ( !list_rewind( account_list ) ) return 0;

	count = 0;

	do {
		account =
			list_get(
				account_list );

		if ( !account->account_total ) continue;

		html_account_total(
			html_table,
			element_name,
			account,
			prior_year_list );

		list_free( html_table->data_list );
		html_table->data_list = list_new();

		element_name = (char *)0;
		count++;

	} while( list_next( account_list ) );

	return count;
}

void pipe_account_total(
			FILE *output_pipe,
			char *element_name,
			ACCOUNT *account,
			LIST *prior_year_list,
			char delimiter )
{
	double balance;
	boolean accumulate_debit;

	if ( !account->account_total ) return;
	if ( !account->latest_journal ) return;

	balance = account->account_total;
	accumulate_debit = account->accumulate_debit;

	if ( balance < 0.0 )
	{
		balance = float_abs( balance );
		accumulate_debit = 1 - accumulate_debit;
	}

	fprintf(output_pipe,
		"%s%c%s%c%d",
		element_name,
		delimiter,
		account->account_name,
		delimiter,
		account->latest_journal->transaction_count );

	if ( accumulate_debit )
	{
		fprintf(output_pipe,
			"%c%s",
			delimiter,
			timlib_place_commas_in_money(
				balance ) );
	}
	else
	{
		fprintf( output_pipe, "%c", delimiter );
	}

	if ( !accumulate_debit )
	{
		fprintf(output_pipe,
			"%c%s",
			delimiter,
			timlib_place_commas_in_money(
				balance ) );
	}
	else
	{
		fprintf( output_pipe, "%c", delimiter );
	}

	fprintf(output_pipe,
		"%c%d%c%d",
		delimiter,
		account->percent_of_assets,
		delimiter,
		account->percent_of_revenues );

	if ( list_length( prior_year_list ) )
	{
		fprintf(output_pipe,
			"%s\n",
			list_display_delimited(
				statement_prior_year_delta_list(
					account->account_name,
					prior_year_list ),
				delimiter ) );
	}
	else
	{
		fprintf( output_pipe, "\n" );
	}
}

void html_account_total(
			HTML_TABLE *html_table,
			char *element_name,
			ACCOUNT *account,
			LIST *prior_year_list )
{
	char element_title[ 128 ];
	char *account_title;
	char transaction_count_string[ 16 ];
	char debit_string[ 4096 ];
	char credit_string[ 4096 ];
	char percent_of_assets_string[ 16 ];
	char percent_of_revenues_string[ 16 ];
	char transaction_date_string[ 16 ];
	double balance;
	boolean accumulate_debit;

	if ( !account->account_total ) return;
	if ( !account->latest_journal ) return;

	balance = account->account_total;
	accumulate_debit = account->accumulate_debit;

	/* See if negative balance. */
	/* ------------------------ */
	if ( balance < 0.0 )
	{
		balance = float_abs( balance );
		accumulate_debit = 1 - accumulate_debit;
	}

	if ( element_name && *element_name )
	{
		format_initial_capital(
			element_title,
			element_name );

		html_table_set_data(
			html_table->data_list,
			strdup( element_title ) );
	}
	else
	{
		html_table_set_data( html_table->data_list, strdup( "" ) );
	}

	account_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		html_table_account_title(
			account->account_name,
			account->latest_journal->full_name,
			account->latest_journal->debit_amount,
			account->latest_journal->credit_amount,
			column( transaction_date_string,
				0,
				account->
					latest_journal->
					transaction_date_time ),
			account->latest_journal->memo );

	html_table_set_data(
		html_table->data_list,
		strdup( account_title ) );

	sprintf(transaction_count_string,
		"%d",
		account->latest_journal->transaction_count );

	html_table_set_data(
		html_table->data_list,
		strdup( transaction_count_string ) );

	/* Set the debit account. */
	/* ---------------------- */
	if ( accumulate_debit )
	{
		sprintf( debit_string,
			 "<a href=\"%s\">%s</a>",
			 account->account_action_string,
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
		html_table->data_list,
		strdup( debit_string ) );

	/* Set the credit account. */
	/* ----------------------- */
	if ( !accumulate_debit )
	{
		sprintf( credit_string,
			 "<a href=\"%s\">%s</a>",
			 account->account_action_string,
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
		html_table->data_list,
		strdup( credit_string ) );

	/* Set percent of assets */
	/* -------------------- */
	sprintf(percent_of_assets_string,
		"%d%c",
		account->percent_of_assets,
		'%' );

	html_table_set_data(
		html_table->data_list,
		strdup( percent_of_assets_string ) );

	/* Set percent of revenues */
	/* ----------------------- */
	sprintf(percent_of_revenues_string,
		"%d%c",
		account->percent_of_revenues,
		'%' );

	html_table_set_data(
		html_table->data_list,
		strdup( percent_of_revenues_string ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_prior_year_delta_list(
				account->account_name,
				prior_year_list ) );
	}

	/* Output the row */
	/* -------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );
}

void trial_balance_PDF(
			char *application_name,
			char *document_root_directory,
			char *process_name,
			char *logo_filename,
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean postclose,
			enum subclassification_option subclassification_option,
			enum fund_aggregation fund_aggregation )
{
	STATEMENT_FUND *statement_fund;
	LATEX *latex;
	char *latex_filename;
	char *dvi_filename;
	char *working_directory;
	char *ftp_output_filename;
	pid_t pid = getpid();
	APPASERVER_LINK_FILE *appaserver_link_file;
	double debit_total;
	double credit_total;
	char prompt[ 128 ];
	boolean landscape_flag = 0;
	static boolean first_time = 1;

	if ( postclose ) pid++;

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

	if ( first_time )
	{
		printf( "<h1>%s</h1>\n", title );
		printf( "<h2>%s</h2>\n", subtitle );

		first_time = 0;
	}
	else
	{
		printf( "<br>\n" );
	}

	if ( statement_fund_exists_prior_year(
		statement_fund_list ) )
	{
		landscape_flag = 1;
	}

	if ( !list_length( statement_fund_list ) ) return;

	latex =
		latex_new(
			latex_filename,
			dvi_filename,
			working_directory,
			landscape_flag,
			logo_filename );

	if ( fund_aggregation == sequential )
	{
		list_rewind( statement_fund_list );

		do {
			statement_fund =
				list_get(
					statement_fund_list );

			if ( postclose )
			{
				debit_total =
					statement_fund->
						postclose_debit_total;

				credit_total =
					statement_fund->
						postclose_credit_total;
			}
			else
			{
				debit_total =
					statement_fund->
						preclose_debit_total;

				credit_total =
					statement_fund->
						preclose_credit_total;
			}

			if (	subclassification_option ==
				subclassification_display )
			{
				list_set(
					latex->table_list,
					PDF_subclassification_latex_table(
						subtitle,
						statement_fund,
						postclose,
						debit_total,
						credit_total,
						date_get_now_yyyy_mm_dd(
						    date_utc_offset() )
						    /* today_date_string */ ) );
			}
			else
			{
				list_set(
					latex->table_list,
					PDF_account_latex_table(
						subtitle,
						statement_fund,
						postclose,
						debit_total,
						credit_total,
						date_get_now_yyyy_mm_dd(
						    date_utc_offset() )
						    /* today_date_string */ ) );
			}

		} while( list_next( statement_fund_list ) );
	}
	else
	/* Either single fund or consolidated. */
	/* ----------------------------------- */
	{
		statement_fund =
			list_first(
				statement_fund_list );

		if ( postclose )
		{
			debit_total =
				statement_fund->
					postclose_debit_total;

			credit_total =
				statement_fund->
					postclose_credit_total;
		}
		else
		{
			debit_total =
				statement_fund->
					preclose_debit_total;

			credit_total =
				statement_fund->
					preclose_credit_total;
		}

		if (	subclassification_option ==
			subclassification_display )
		{
			list_set(
				latex->table_list,
				PDF_subclassification_latex_table(
					subtitle,
					statement_fund,
					postclose,
					debit_total,
					credit_total,
					date_get_now_yyyy_mm_dd(
						date_utc_offset() )
						/* today_date_string */ ) );
		}
		else
		{
			list_set(
				latex->table_list,
				PDF_account_latex_table(
					subtitle,
					statement_fund,
					postclose,
					debit_total,
					credit_total,
					date_get_now_yyyy_mm_dd(
						date_utc_offset() )
						/* today_date_string */ ) );
		}
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

	if ( postclose )
	{
		sprintf(prompt,
			"%s (Post Close)",
			PROMPT );
	}
	else
	{
		strcpy( prompt, PROMPT );
	}

	appaserver_library_output_ftp_prompt(
		ftp_output_filename, 
		prompt,
		process_name /* target */,
		(char *)0 /* mime_type */ );
}

LATEX_TABLE *PDF_subclassification_latex_table(
			char *subtitle,
			STATEMENT_FUND *statement_fund,
			boolean postclose,
			double debit_total,
			double credit_total,
			char *today_date_string )
{
	LATEX_TABLE *latex_table;

	latex_table =
		latex_table_new(
			strdup( subtitle )
				/* caption */ );

	latex_table->heading_list =
		PDF_heading_list(
			statement_fund->prior_year_list,
			1 /* include_subclassification */ );

	if ( postclose
	&&   list_length( statement_fund->postclose_element_list ) )
	{
		latex_table->row_list =
			PDF_subclassification_row_list(
				statement_fund->postclose_element_list,
				statement_fund->prior_year_list,
				debit_total,
				credit_total,
				today_date_string );
	}
	else
	if ( list_length( statement_fund->preclose_element_list ) )
	{
		latex_table->row_list =
			PDF_subclassification_row_list(
				statement_fund->preclose_element_list,
				statement_fund->prior_year_list,
				debit_total,
				credit_total,
				today_date_string );
	}
	return latex_table;
}

LATEX_TABLE *PDF_account_latex_table(
			char *subtitle,
			STATEMENT_FUND *statement_fund,
			boolean postclose,
			double debit_total,
			double credit_total,
			char *today_date_string )
{
	LATEX_TABLE *latex_table;

	latex_table =
		latex_table_new(
			strdup( subtitle )
				/* caption */ );

	latex_table->heading_list =
		PDF_heading_list(
			statement_fund->prior_year_list,
			0 /* not include_subclassification */ );

	if ( postclose
	&&   list_length( statement_fund->postclose_element_list ) )
	{
		latex_table->row_list =
			PDF_account_row_list(
				statement_fund->postclose_element_list,
				statement_fund->prior_year_list,
				debit_total,
				credit_total,
				today_date_string );
	}
	else
	if ( list_length( statement_fund->preclose_element_list ) )
	{
		latex_table->row_list =
			PDF_account_row_list(
				statement_fund->preclose_element_list,
				statement_fund->prior_year_list,
				debit_total,
				credit_total,
				today_date_string );
	}

	return latex_table;
}

LIST *html_heading_list(
			LIST *prior_year_list,
			boolean include_subclassification )
{
	LIST *heading_list = list_new();

	list_set( heading_list, "Element" );

	if ( include_subclassification )
	{
		list_set( heading_list, "Subclassification" );
	}

	list_set( heading_list, "Account" );
	list_set( heading_list, "Count" );
	list_set( heading_list, "Debit" );
	list_set( heading_list, "Credit" );
	list_set( heading_list, "Percent of Asset" );
	list_set( heading_list, "Percent of Revenue" );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			heading_list,
			statement_prior_year_heading_list(
				prior_year_list ) );
	}

	return heading_list;
}

LIST *PDF_subclassification_row_list(
			LIST *element_list,
			LIST *prior_year_list,
			double debit_total,
			double credit_total,
			char *today_date_string )
{
	LATEX_ROW *latex_row;
	LIST *row_list;
	ELEMENT *element;
	char *debit_total_string;
	char *credit_total_string;

	if ( !list_rewind( element_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		element = list_get( element_list );

		if ( !list_length( element->subclassification_list ) )
			continue;

		list_set_list(
			row_list,
			PDF_subclassification_element_row_list(
				element->element_name,
				element->subclassification_list,
				prior_year_list,
				today_date_string ) );

	} while( list_next( element_list ) );

	latex_row = latex_row_new();
	list_set( row_list, latex_row );

	latex_column_data_set(
		latex_row->column_data_list,
		"Total",
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	debit_total_string = timlib_place_commas_in_dollars( debit_total );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( debit_total_string ),
		0 /* not large_bold */ );

	credit_total_string = timlib_place_commas_in_dollars( credit_total );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( credit_total_string ),
		0 /* not large_bold */ );

	return row_list;
}

LIST *PDF_account_row_list(
			LIST *element_list,
			LIST *prior_year_list,
			double debit_total,
			double credit_total,
			char *today_date_string )
{
	LATEX_ROW *latex_row;
	LIST *row_list;
	ELEMENT *element;
	char *debit_total_string;
	char *credit_total_string;

	if ( !list_rewind( element_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		element = list_get( element_list );

		if ( !list_length( element->account_list ) )
			continue;

		list_set_list(
			row_list,
			PDF_account_element_row_list(
				element->element_name,
				element->account_list,
				prior_year_list,
				today_date_string ) );

	} while( list_next( element_list ) );

	latex_row = latex_row_new();
	list_set( row_list, latex_row );

	latex_column_data_set(
		latex_row->column_data_list,
		"Total",
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	debit_total_string = timlib_place_commas_in_dollars( debit_total );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( debit_total_string ),
		0 /* not large_bold */ );

	credit_total_string = timlib_place_commas_in_dollars( credit_total );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( credit_total_string ),
		0 /* not large_bold */ );

	return row_list;
}

LIST *PDF_subclassification_element_row_list(
			char *element_name,
			LIST *subclassification_list,
			LIST *prior_year_list,
			char *today_date_string )
{
	SUBCLASSIFICATION *subclassification;
	LIST *row_list;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		subclassification =
			list_get(
				subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		list_set_list(
			row_list,
			PDF_subclassification_account_list_row_list(
				element_name,
				subclassification->subclassification_name,
				subclassification->account_list,
				prior_year_list,
				today_date_string ) );

		element_name = (char *)0;

	} while( list_next( subclassification_list ) );

	return row_list;
}

LIST *PDF_account_element_row_list(
			char *element_name,
			LIST *account_list,
			LIST *prior_year_list,
			char *today_date_string )
{
	ACCOUNT *account;
	LIST *row_list;

	if ( !list_rewind( account_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		account =
			list_get(
				account_list );

		if ( !account->account_total ) continue;

		list_set(
			row_list,
				PDF_account_latex_row(
				element_name,
				(char *)0 /* subclassification_name */,
				account,
				prior_year_list,
				today_date_string,
				0 /* not include_subclassification */ ) );

		element_name = (char *)0;

	} while( list_next( account_list ) );

	return row_list;
}

LIST *PDF_subclassification_account_list_row_list(
			char *element_name,
			char *subclassification_name,
			LIST *account_list,
			LIST *prior_year_list,
			char *today_date_string )
{
	ACCOUNT *account;
	LIST *row_list;

	if ( !list_rewind( account_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		account = 
			list_get(
				account_list );

		if ( !account->account_total ) continue;

		list_set(
			row_list,
			PDF_account_latex_row(
				element_name,
				subclassification_name,
				account,
				prior_year_list,
				today_date_string,
				1 /* include_subclassification */ ) );

		subclassification_name = (char *)0;
		element_name = (char *)0;

	} while ( list_next( account_list ) );

	return row_list;
}

LATEX_ROW *PDF_account_latex_row(
			char *element_name,
			char *subclassification_name,
			ACCOUNT *account,
			LIST *prior_year_list,
			char *today_date_string,
			boolean include_subclassification )
{
	char element_title[ 128 ];
	char subclassification_title[ 128 ];
	char transaction_date_american[ 16 ];
	char transaction_date_string[ 16 ];
	char *account_title;
	char transaction_count_string[ 32 ];
	double balance;
	boolean accumulate_debit;
	char *debit_string;
	char *credit_string;
	int days_between;
	char percent_string[ 16 ];
	LATEX_ROW *latex_row;

	if ( !account->account_total ) return (LATEX_ROW *)0;
	if ( !account->latest_journal ) return (LATEX_ROW *)0;

	latex_row = latex_row_new();

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

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( element_title ),
		0 /* not large_bold */ );

	if ( include_subclassification )
	{
		if ( subclassification_name )
		{
			format_initial_capital(
				subclassification_title,
				subclassification_name );
	
			subclassification_name = (char *)0;
		}
		else
		{
			*subclassification_title = '\0';
		}
	
		latex_column_data_set(
			latex_row->column_data_list,
			strdup( subclassification_title ),
			0 /* not large_bold */ );
	}

	date_convert_source_international(
		transaction_date_american,
		american,
 		column( transaction_date_string,
 			0,
 			account->
				latest_journal->
				transaction_date_time ) );

	account_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		PDF_account_title(
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

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( account_title ),
		0 /* not large_bold */ );

	sprintf( transaction_count_string,
		 "%d",
		 account->
			latest_journal->
			transaction_count );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( transaction_count_string ),
		0 /* not large_bold */ );

	balance = account->latest_journal->balance;
	accumulate_debit = account->accumulate_debit;

	/* See if negative balance. */
	/* ------------------------ */
	if ( balance < 0.0 )
	{
		balance = float_abs( balance );
		accumulate_debit = 1 - accumulate_debit;
	}

	days_between =
		date_days_between(
			transaction_date_string,
			today_date_string );

	if ( accumulate_debit )
		debit_string = timlib_place_commas_in_dollars( balance );
	else
		debit_string = "";

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( debit_string ),
		(days_between <= DAYS_FOR_EMPHASIS) /* large_bold */ );

	if ( !accumulate_debit )
		credit_string = timlib_place_commas_in_dollars( balance );
	else
		credit_string = "";

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( credit_string ),
		(days_between <= DAYS_FOR_EMPHASIS) /* large_bold */ );

	if ( element_is_nominal( account->element_name ) )
	{
		sprintf(percent_string,
			"%d%c",
			account->percent_of_revenues,
			'%' );
	}
	else
	{
		sprintf(percent_string,
			"%d%c",
			account->percent_of_assets,
			'%' );
	}

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( percent_string ),
		0 /* not large_bold */ );

	if ( list_length( prior_year_list ) )
	{
		latex_column_data_set_list(
			latex_row->column_data_list,
			statement_PDF_prior_year_delta_list(
				account->account_name,
				prior_year_list ) );
	}

	return latex_row;
}

char *PDF_account_title(
			char *account_name,
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

LIST *PDF_heading_list(
			LIST *prior_year_list,
			boolean include_subclassification )
{
	LATEX_TABLE_HEADING *table_heading;
	LIST *heading_list;

	heading_list = list_new();

	table_heading = latex_table_heading_new();
	table_heading->heading = "Element";
	table_heading->right_justified_flag = 0;
	list_set( heading_list, table_heading );

	if ( include_subclassification )
	{
		table_heading = latex_table_heading_new();
		table_heading->right_justified_flag = 0;
		table_heading->heading = "Subclassification";
		list_set( heading_list, table_heading );
	}

	table_heading = latex_table_heading_new();
	table_heading->heading = "Account";
	table_heading->paragraph_size = "6.0cm";
	list_set( heading_list, table_heading );

	table_heading = latex_table_heading_new();
	table_heading->heading = "Count";
	table_heading->right_justified_flag = 1;
	list_set( heading_list, table_heading );

	table_heading = latex_table_heading_new();
	table_heading->heading = "Debit";
	table_heading->right_justified_flag = 1;
	list_set( heading_list, table_heading );

	table_heading = latex_table_heading_new();
	table_heading->heading = "Credit";
	table_heading->right_justified_flag = 1;
	list_set( heading_list, table_heading );

	table_heading = latex_table_heading_new();
	table_heading->heading = "Percent";
	table_heading->right_justified_flag = 1;
	list_set( heading_list, table_heading );

	if ( list_length( prior_year_list ) )
	{
		list_set_list(
			heading_list,
			latex_table_right_heading_list(
				statement_prior_year_heading_list(
					prior_year_list ) ) );
	}

	return heading_list;
}

