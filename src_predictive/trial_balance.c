/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/trial_balance.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "float.h"
#include "date.h"
#include "appaserver_error.h"
#include "transaction.h"
#include "statement.h"
#include "element.h"
#include "subclassification.h"
#include "account.h"
#include "trial_balance.h"

TRIAL_BALANCE *trial_balance_calloc( void )
{
	TRIAL_BALANCE *trial_balance;

	if ( ! ( trial_balance = calloc( 1, sizeof ( TRIAL_BALANCE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: () returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance;
}

TRIAL_BALANCE *trial_balance_fetch(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *data_directory,
		char *as_of_date_string,
		int prior_year_count,
		char *subclassification_option_string,
		char *output_medium_string )
{
	TRIAL_BALANCE *trial_balance;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
	||   !as_of_date_string
	||   !data_directory
	||   !subclassification_option_string
	||   !output_medium_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	trial_balance = trial_balance_calloc();

	trial_balance->statement_subclassification_option =
		statement_resolve_subclassification_option(
			subclassification_option_string );

	trial_balance->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	trial_balance->transaction_date_trial_balance =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		transaction_date_trial_balance_new(
			as_of_date_string );

	if ( ! ( trial_balance->
			transaction_date_trial_balance->
			transaction_date_as_of ) )
	{
		free( trial_balance );
		return NULL;
	}

	trial_balance->element_name_list =
		trial_balance_element_name_list(
			ELEMENT_ASSET,
			ELEMENT_LIABILITY,
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE,
			ELEMENT_GAIN,
			ELEMENT_LOSS,
			ELEMENT_EQUITY );

	if ( trial_balance->
		transaction_date_trial_balance->
		transaction_date_close_boolean )
	{
		trial_balance->preclose_statement =
			statement_fetch(
				application_name,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				trial_balance_preclose_process_name(
					process_name ),
				prior_year_count,
				trial_balance->element_name_list,
				trial_balance->
					transaction_date_trial_balance->
					transaction_date_begin_date_string,
				trial_balance->
					transaction_date_trial_balance->
					preclose_end_date_time_string,
				1 /* fetch_transaction */ );

		if ( !trial_balance->preclose_statement )
		{
			char message[ 128 ];

			sprintf(message,
				"statement_fetch(%s) returned empty.",
				trial_balance->
					transaction_date_trial_balance->
					preclose_end_date_time_string );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		element_account_transaction_count_set(
			trial_balance->
				preclose_statement->
				element_statement_list,
			trial_balance->
				transaction_date_trial_balance->
				transaction_date_begin_date_string,
			trial_balance->
				transaction_date_trial_balance->
				preclose_end_date_time_string );

		if ( trial_balance->statement_output_medium ==
			statement_output_table )
		{
			element_account_action_string_set(
				trial_balance->
					preclose_statement->
					element_statement_list,
				application_name,
				session_key,
				login_name,
				role_name,
				trial_balance->
					transaction_date_trial_balance->
					transaction_date_begin_date_string,
				trial_balance->
					transaction_date_trial_balance->
					preclose_end_date_time_string );
		}

		if ( prior_year_count )
		{
			trial_balance->preclose_statement_prior_year_list =
				statement_prior_year_list(
					trial_balance->
						element_name_list,
					trial_balance->
						transaction_date_trial_balance->
						preclose_end_date_time_string,
					prior_year_count,
					trial_balance->preclose_statement
						/* statement */ );
		}
	}

	trial_balance->statement =
		statement_fetch(
			application_name,
			process_name,
			prior_year_count,
			trial_balance->element_name_list,
			trial_balance->
				transaction_date_trial_balance->
				transaction_date_begin_date_string,
			trial_balance->
				transaction_date_trial_balance->
				end_date_time_string,
			1 /* fetch_transaction */ );

	if ( !trial_balance->statement )
	{
		char message[ 128 ];

		sprintf(message,
			"statement_fetch(%s) returned empty.",
			trial_balance->
				transaction_date_trial_balance->
				end_date_time_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	element_account_transaction_count_set(
		trial_balance->statement->element_statement_list,
		trial_balance->
			transaction_date_trial_balance->
			transaction_date_begin_date_string,
		trial_balance->
			transaction_date_trial_balance->
			end_date_time_string );

	if ( trial_balance->statement_output_medium ==
		statement_output_table )
	{
		element_account_action_string_set(
			trial_balance->statement->element_statement_list,
			application_name,
			session_key,
			login_name,
			role_name,
			trial_balance->
				transaction_date_trial_balance->
				transaction_date_begin_date_string,
			trial_balance->
				transaction_date_trial_balance->
				end_date_time_string );
	}

	if ( prior_year_count )
	{
		trial_balance->statement_prior_year_list =
			statement_prior_year_list(
				trial_balance->
					element_name_list,
				trial_balance->
					transaction_date_trial_balance->
					end_date_time_string,
				prior_year_count,
				trial_balance->statement );
	}

	if ( trial_balance->
		transaction_date_trial_balance->
		transaction_date_close_boolean )
	{
		trial_balance->preclose_debit_sum =
			element_list_debit_sum(
				trial_balance->
					preclose_statement->
					element_statement_list );

		trial_balance->preclose_credit_sum =
			element_list_credit_sum(
				trial_balance->
					preclose_statement->
					element_statement_list );
	}

	trial_balance->debit_sum =
		element_list_debit_sum(
			trial_balance->
				statement->
				element_statement_list );

	trial_balance->credit_sum =
		element_list_credit_sum(
			trial_balance->
				statement->
				element_statement_list );

	if ( trial_balance->
		transaction_date_trial_balance->
		transaction_date_close_boolean
	&&   trial_balance->statement_subclassification_option ==
			statement_subclassification_omit )
	{
		element_list_account_statement_list_set(
			trial_balance->
				preclose_statement->
				element_statement_list );
	}

	if (	trial_balance->statement_subclassification_option ==
			statement_subclassification_omit )
	{
		element_list_account_statement_list_set(
			trial_balance->
				statement->
				element_statement_list );
	}

	if ( trial_balance->statement_output_medium == statement_output_PDF )
	{
		trial_balance->trial_balance_pdf =
			trial_balance_pdf_new(
				application_name,
				data_directory,
				trial_balance->
					statement_subclassification_option,
				trial_balance->preclose_statement,
				trial_balance->
					preclose_statement_prior_year_list,
				trial_balance->preclose_debit_sum,
				trial_balance->preclose_credit_sum,
				trial_balance->statement,
				trial_balance->statement_prior_year_list,
				trial_balance->debit_sum,
				trial_balance->credit_sum,
				getpid() /* process_id */ );
	}
	else
	if ( trial_balance->statement_output_medium == statement_output_table )
	{
		trial_balance->trial_balance_table =
			trial_balance_table_new(
				trial_balance->
					statement_subclassification_option,
				trial_balance->preclose_statement,
				trial_balance->
					preclose_statement_prior_year_list,
				trial_balance->preclose_debit_sum,
				trial_balance->preclose_credit_sum,
				trial_balance->statement,
				trial_balance->statement_prior_year_list,
				trial_balance->debit_sum,
				trial_balance->credit_sum );
	}

	return trial_balance;
}

LIST *trial_balance_element_name_list(
		char *element_asset,
		char *element_liability,
		char *element_revenue,
		char *element_expense,
		char *element_gain,
		char *element_loss,
		char *element_equity )
{
	LIST *element_name_list = list_new();

	list_set( element_name_list, element_asset );
	list_set( element_name_list, element_liability );
	list_set( element_name_list, element_revenue );
	list_set( element_name_list, element_expense );
	list_set( element_name_list, element_gain );
	list_set( element_name_list, element_loss );
	list_set( element_name_list, element_equity );

	return element_name_list;
}

TRIAL_BALANCE_PDF *trial_balance_pdf_new(
		char *application_name,
		char *data_directory,
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *preclose_statement,
		LIST *preclose_statement_prior_year_list,
		double preclose_debit_sum,
		double preclose_credit_sum,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum,
		pid_t process_id )
{
	TRIAL_BALANCE_PDF *trial_balance_pdf;

	if ( !application_name
	||   !data_directory
	||   !statement
	||   !statement->statement_caption
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	trial_balance_pdf = trial_balance_pdf_calloc();

	trial_balance_pdf->statement_pdf_landscape_boolean =
		statement_pdf_landscape_boolean(
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */ );

	if ( preclose_statement )
	{
		trial_balance_pdf->preclose_statement_link =
			statement_link_new(
				application_name,
				preclose_statement->process_name,
				data_directory,
				preclose_statement->
					transaction_date_begin_date_string,
				preclose_statement->
					end_date_time_string,
				process_id );

		trial_balance_pdf->preclose_trial_balance_latex =
			trial_balance_latex_new(
				statement_subclassification_option,
				trial_balance_pdf->
					preclose_statement_link->
					tex_filename,
				trial_balance_pdf->
					preclose_statement_link->
					appaserver_link_working_directory,
				trial_balance_pdf->
					statement_pdf_landscape_boolean,
				preclose_statement->
					statement_caption->
					logo_filename,
				preclose_statement,
				preclose_statement_prior_year_list,
				preclose_debit_sum,
				preclose_credit_sum );
	}

	trial_balance_pdf->statement_link =
		statement_link_new(
			application_name,
			statement->process_name,
			data_directory,
			statement->transaction_date_begin_date_string,
			statement->end_date_time_string,
			process_id );

	trial_balance_pdf->trial_balance_latex =
		trial_balance_latex_new(
			statement_subclassification_option,
			trial_balance_pdf->
				statement_link->
				tex_filename,
			trial_balance_pdf->
				statement_link->
				appaserver_link_working_directory,
			trial_balance_pdf->
				statement_pdf_landscape_boolean,
			statement->statement_caption->logo_filename,
			statement,
			statement_prior_year_list,
			debit_sum,
			credit_sum );

	return trial_balance_pdf;
}

TRIAL_BALANCE_PDF *trial_balance_pdf_calloc( void )
{
	TRIAL_BALANCE_PDF *trial_balance_pdf;

	if ( ! ( trial_balance_pdf =
			calloc( 1, sizeof ( TRIAL_BALANCE_PDF ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_pdf;
}

TRIAL_BALANCE_LATEX *trial_balance_latex_new(
		enum statement_subclassification_option
			statement_subclassification_option,
		char *tex_filename,
		char *working_directory,
		boolean statement_pdf_landscape_boolean,
		char *statement_caption_logo_filename,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum )
{
	TRIAL_BALANCE_LATEX *trial_balance_latex;

	if ( !tex_filename
	||   !working_directory
	||   !statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	trial_balance_latex = trial_balance_latex_calloc();

	trial_balance_latex->latex =
		latex_new(
			tex_filename,
			working_directory,
			statement_pdf_landscape_boolean,
			statement_caption_logo_filename );

	if (	statement_subclassification_option ==
			statement_subclassification_display )
	{
		trial_balance_latex->trial_balance_subclass_display_latex =
			trial_balance_subclass_display_latex_new(
				statement,
				statement_prior_year_list,
				debit_sum,
				credit_sum );

		trial_balance_latex->latex_table =
			latex_table_new(
				statement->statement_caption->combined,
				trial_balance_latex->
					trial_balance_subclass_display_latex->
					column_list,
				trial_balance_latex->
					trial_balance_subclass_display_latex->
					row_list );
	}
	else
	{
		trial_balance_latex->trial_balance_subclass_omit_latex =
			trial_balance_subclass_omit_latex_new(
				statement,
				statement_prior_year_list,
				debit_sum,
				credit_sum );

		trial_balance_latex->latex_table =
			latex_table_new(
				statement->statement_caption->combined,
				trial_balance_latex->
					trial_balance_subclass_omit_latex->
					column_list,
				trial_balance_latex->
					trial_balance_subclass_omit_latex->
					row_list );
	}

	return trial_balance_latex;
}

TRIAL_BALANCE_LATEX *trial_balance_latex_calloc( void )
{
	TRIAL_BALANCE_LATEX *trial_balance_latex;

	if ( ! ( trial_balance_latex =
			calloc( 1, sizeof ( TRIAL_BALANCE_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_latex;
}

HTML_TABLE *trial_balance_html_table(
		char *statement_caption_sub_title,
		LIST *column_list,
		LIST *row_list )
{
	HTML_TABLE *html_table;

	if ( !statement_caption_sub_title
	||   !list_length( column_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_table =
		html_table_new(
			(char *)0 /* title */,
			statement_caption_sub_title,
			(char *)0 /* sub_sub_title */ );

	html_table->column_list = column_list;
	html_table->row_list = row_list;

	return html_table;
}

char *trial_balance_html_account_datum(
		char *account_name,
		char *full_name,
		double debit_amount,
		double credit_amount,
		char *transaction_date_american,
		char *memo )
{
	char account_datum[ 2048 ];
	char *ptr = account_datum;
	char account_name_formatted[ 128 ];
	char full_name_formatted[ 128 ];
	char *transaction_amount_string;

	if ( !account_name
	||   !full_name
	||   !transaction_date_american )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*ptr = '\0';

	string_initial_capital( account_name_formatted, account_name );
	ptr += sprintf( ptr, "<big><b>%s</b></big>", account_name_formatted );

	ptr += sprintf( ptr, " (%s:", transaction_date_american );

	string_initial_capital( full_name_formatted, full_name );
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
			string_commas_money(
				debit_amount );
	}
	else
	if ( credit_amount )
	{
		transaction_amount_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_money(
				credit_amount );
	}
	else
	{
		transaction_amount_string = "Unknown";
	}

	ptr += sprintf( ptr, " $%s)", transaction_amount_string );

	return strdup( account_datum );
}

char *trial_balance_latex_account_datum(
		char *account_name,
		char *full_name,
		double debit_amount,
		double credit_amount,
		char *transaction_date_american,
		char *memo )
{
	char account_datum[ 2048 ];
	char *ptr = account_datum;
	char account_name_formatted[ 128 ];
	char full_name_formatted[ 128 ];
	char *transaction_amount_string;

	if ( !account_name
	||   !full_name
	||   !transaction_date_american )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*ptr = '\0';

	string_initial_capital( account_name_formatted, account_name );
	ptr += sprintf( ptr, "\\textbf{%s}", account_name_formatted );

	ptr += sprintf( ptr, " \\scriptsize{(%s:", transaction_date_american );

	string_initial_capital( full_name_formatted, full_name );
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
			string_commas_money(
				debit_amount );
	}
	else
	if ( credit_amount )
	{
		transaction_amount_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_money(
				credit_amount );
	}
	else
	{
		transaction_amount_string = "Unknown";
	}

	ptr += sprintf( ptr, " $%s)}", transaction_amount_string );

	return strdup( account_datum );
}

char *trial_balance_transaction_count_string(
		int transaction_count )
{
	char count_string[ 16 ];

	sprintf(count_string,
		"%d",
		transaction_count );

	return strdup( count_string );
}

TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
	trial_balance_subclass_display_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum )
{
	TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
		trial_balance_subclass_display_latex;
	ELEMENT *element;
	char *element_name;
	SUBCLASSIFICATION *subclassification;
	LIST *row_list;
	LATEX_ROW *latex_row;

	if ( !statement
	||   !statement->end_date_time_string
	||   !list_rewind( statement->element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "statement is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	trial_balance_subclass_display_latex =
		trial_balance_subclass_display_latex_calloc();

	trial_balance_subclass_display_latex->column_list =
		trial_balance_subclass_display_latex_column_list(
			statement_prior_year_list );

	trial_balance_subclass_display_latex->row_list = list_new();

	do {
		element = list_get( statement->element_statement_list );

		if ( !list_rewind( element->subclassification_statement_list ) )
		{
			continue;
		}

		element_name = element->element_name;

		do {
			subclassification =
				list_get(
				   element->
					subclassification_statement_list );

			if ( !list_length(
				subclassification->
					account_statement_list ) )
			{
				continue;
			}

			row_list =
				trial_balance_subclass_display_latex_row_list(
					statement->
						end_date_time_string,
					element_name,
					element->accumulate_debit,
					subclassification->
						subclassification_name,
					subclassification->
						account_statement_list,
					statement_prior_year_list,
					trial_balance_subclass_display_latex->
						column_list );

			list_set_list(
				trial_balance_subclass_display_latex->row_list,
				row_list );

			element_name = (char *)0;

		} while ( list_next(
				element->
					subclassification_statement_list ) );

	} while ( list_next( statement->element_statement_list ) );

	latex_row =
		trial_balance_latex_total_row(
			debit_sum,
			credit_sum,
			trial_balance_subclass_display_latex->column_list,
			3 /* column_skip_count */ );

	list_set(
		trial_balance_subclass_display_latex->row_list,
		latex_row );

	return trial_balance_subclass_display_latex;
}

TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
	trial_balance_subclass_display_latex_calloc(
			void )
{
	TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
		trial_balance_subclass_display_latex;

	if ( ! ( trial_balance_subclass_display_latex =
		   calloc( 1,
			   sizeof ( TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	return trial_balance_subclass_display_latex;
}

LIST *trial_balance_subclass_display_latex_row_list(
		char *end_date_time_string,
		char *element_name,
		boolean element_accumulate_debit,
		char *subclassification_name,
		LIST *account_statement_list,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *row_list;
	ACCOUNT *account;
	STATEMENT_ACCOUNT *statement_account;

	if ( !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "end_date_time_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( account_statement_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
				"account_journal_latest is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		statement_account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statement_account_new(
				end_date_time_string,
				element_accumulate_debit,
				account->account_journal_latest,
				(char *)0 /* account_action_string */,
				1 /* round_dollar_boolean */,
				account );

		list_set(
			row_list,
			trial_balance_subclass_display_latex_row(
				element_name,
				subclassification_name,
				statement_account,
				statement_prior_year_list,
				latex_column_list ) );

		element_name = (char *)0;
		subclassification_name = (char *)0;

	} while ( list_next( account_statement_list ) );

	return row_list;
}

LIST *trial_balance_subclass_display_html_row_list(
		char *end_date_time_string,
		char *element_name,
		boolean element_accumulate_debit,
		char *subclassification_name,
		LIST *account_statement_list,
		LIST *statement_prior_year_list )
{
	LIST *row_list;
	ACCOUNT *account;
	STATEMENT_ACCOUNT *statement_account;

	if ( !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "end_date_time_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( account_statement_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
			"account_name=[%s], account_journal_latest is empty.",
				account->account_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		statement_account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statement_account_new(
				end_date_time_string,
				element_accumulate_debit,
				account->account_journal_latest,
				account->action_string,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			row_list,
			trial_balance_subclass_display_html_row(
				element_name,
				subclassification_name,
				statement_account,
				statement_prior_year_list ) );

		element_name = (char *)0;
		subclassification_name = (char *)0;

	} while ( list_next( account_statement_list ) );

	return row_list;
}

HTML_ROW *trial_balance_subclass_display_html_row(
		char *element_name,
		char *subclassification_name,
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;
	char *cell_label_datum;
	char *transaction_date_american;
	char *account_datum;
	char *transaction_count_string;

	if ( !statement_account
	||   !statement_account->account
	||   !statement_account->account->account_journal_latest
	||   !statement_account->account->account_journal_latest->transaction )
	{
		char message[ 128 ];

		sprintf(message, "statement_account is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			element_name );

	list_set(
		cell_list,
		html_cell_new(
			cell_label_datum,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			subclassification_name );

	list_set(
		cell_list,
		html_cell_new(
			cell_label_datum,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( ! ( transaction_date_american =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				statement_account->
					account->
					account_journal_latest->
					transaction_date_time ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"statement_date_american(%s) returned empty.",
			statement_account->
				account->
				account_journal_latest->
				transaction_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	account_datum =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_html_account_datum(
			statement_account->
				account->
				account_name,
			statement_account->
				account->
				account_journal_latest->
				full_name,
			statement_account->
				account->
				account_journal_latest->
				debit_amount,
			statement_account->
				account->
				account_journal_latest->
				credit_amount,
			transaction_date_american,
			statement_account->
				account->
				account_journal_latest->
				transaction->memo );

	list_set(
		cell_list,
		html_cell_new(
			account_datum,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	transaction_count_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_transaction_count_string(
			statement_account->account->transaction_count );

	list_set(
		cell_list,
		html_cell_new(
			transaction_count_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->debit_string,
			0 /* not large_boolean */,
			statement_account->
				within_days_between_boolean
				/* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->credit_string,
		 	0 /* not large_boolean */,
			statement_account->
				within_days_between_boolean
				/* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->asset_percent_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->revenue_percent_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_account_data_list =
			statement_prior_year_account_data_list(
				statement_account->account->account_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			html_cell_list(
				prior_year_account_data_list ) );
	}

	return
	html_row_new( cell_list );
}

LIST *trial_balance_subclass_display_html_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			"Element",
			0 /* not right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Subclassification",
			0 /* not right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Account",
			0 /* not right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Count",
			1 /* right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Debit",
			1 /* right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Credit",
			1 /* right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"percent_of_asset",
			1 /* right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"percent_of_income",
			1 /* right_justify_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_heading_list =
			statement_prior_year_heading_list(
				statement_prior_year_list );

		list_set_list(
			column_list,
			html_column_right_list(
				prior_year_heading_list ) );
	}

	return column_list;
}

TRIAL_BALANCE_TABLE *trial_balance_table_new(
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *preclose_statement,
		LIST *preclose_statement_prior_year_list,
		double preclose_debit_sum,
		double preclose_credit_sum,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum )
{
	TRIAL_BALANCE_TABLE *trial_balance_table;

	if ( !statement )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: statement is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	trial_balance_table = trial_balance_table_calloc();

	if ( preclose_statement )
	{
		trial_balance_table->
			preclose_trial_balance_html =
				trial_balance_html_new(
					statement_subclassification_option,
					preclose_statement,
					preclose_statement_prior_year_list,
					preclose_debit_sum,
					preclose_credit_sum );
	}

	trial_balance_table->
		trial_balance_html =
			trial_balance_html_new(
				statement_subclassification_option,
				statement,
				statement_prior_year_list,
				debit_sum,
				credit_sum );

	return trial_balance_table;
}

TRIAL_BALANCE_TABLE *trial_balance_table_calloc( void )
{
	TRIAL_BALANCE_TABLE *trial_balance_table;

	if ( ! ( trial_balance_table =
			calloc( 1, sizeof ( TRIAL_BALANCE_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_table;
}

TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML *
	trial_balance_subclass_display_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum )
{
	TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML *
		trial_balance_subclass_display_html;
	ELEMENT *element;
	char *element_name;
	SUBCLASSIFICATION *subclassification;
	LIST *row_list;
	HTML_ROW *total_row;

	if ( !statement
	||   !statement->end_date_time_string
	||   !list_rewind( statement->element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "statement is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	trial_balance_subclass_display_html =
		trial_balance_subclass_display_html_calloc();

	trial_balance_subclass_display_html->column_list =
		trial_balance_subclass_display_html_column_list(
			statement_prior_year_list );

	trial_balance_subclass_display_html->row_list = list_new();

	do {
		element = list_get( statement->element_statement_list );

		if ( !list_rewind(
			element->
				subclassification_statement_list ) )
		{
			continue;
		}

		element_name = element->element_name;

		do {
			subclassification =
			    list_get(
				element->
					subclassification_statement_list );

			if ( !list_length(
				subclassification->
					account_statement_list ) )
			{
				continue;
			}

			row_list =
				trial_balance_subclass_display_html_row_list(
					statement->
						end_date_time_string,
					element_name,
					element->accumulate_debit,
					subclassification->
						subclassification_name,
					subclassification->
						account_statement_list,
					statement_prior_year_list );

			list_set_list(
				trial_balance_subclass_display_html->row_list,
				row_list );

			element_name = (char *)0;

		} while ( list_next(
				element->
					subclassification_statement_list ) );

	} while ( list_next( statement->element_statement_list ) );

	total_row =
		trial_balance_html_total_row(
			debit_sum,
			credit_sum,
			3 /* column_skip_count */ );

	list_set(
		trial_balance_subclass_display_html->row_list,
		total_row );

	return trial_balance_subclass_display_html;
}

TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML *
	trial_balance_subclass_display_html_calloc(
		void )
{
	TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML *
		trial_balance_subclass_display_html;

	if ( ! ( trial_balance_subclass_display_html =
			calloc(	1,
				sizeof (
				   TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_subclass_display_html;
}

TRIAL_BALANCE_HTML *trial_balance_html_new(
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum )
{
	TRIAL_BALANCE_HTML *trial_balance_html;

	if ( !statement
	||   !statement->end_date_time_string
	||   !statement->statement_caption )
	{
		char message[ 128 ];

		sprintf(message, "statement is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	trial_balance_html = trial_balance_html_calloc();

	if (	statement_subclassification_option ==
		statement_subclassification_display )
	{
		trial_balance_html->
			trial_balance_subclass_display_html =
				trial_balance_subclass_display_html_new(
					statement,
					statement_prior_year_list,
					debit_sum,
					credit_sum );

		trial_balance_html->html_table =
			trial_balance_html_table(
				statement->statement_caption->sub_title,
				trial_balance_html->
					trial_balance_subclass_display_html->
					column_list,
				trial_balance_html->
					trial_balance_subclass_display_html->
					row_list );
	}
	else
	{
		trial_balance_html->
			trial_balance_subclass_omit_html =
				trial_balance_subclass_omit_html_new(
					statement,
					statement_prior_year_list,
					debit_sum,
					credit_sum );

		trial_balance_html->html_table =
			trial_balance_html_table(
				statement->statement_caption->sub_title,
				trial_balance_html->
					trial_balance_subclass_omit_html->
					column_list,
				trial_balance_html->
					trial_balance_subclass_omit_html->
					row_list );
	}

	return trial_balance_html;
}

TRIAL_BALANCE_HTML *trial_balance_html_calloc( void )
{
	TRIAL_BALANCE_HTML *trial_balance_html;

	if ( ! ( trial_balance_html =
			calloc( 1, sizeof ( TRIAL_BALANCE_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_html;
}

TRIAL_BALANCE_SUBCLASS_OMIT_HTML *
	trial_balance_subclass_omit_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum )
{
	TRIAL_BALANCE_SUBCLASS_OMIT_HTML *trial_balance_subclass_omit_html;
	ELEMENT *element;
	HTML_ROW *total_row;

	if ( !statement
	||   !statement->end_date_time_string
	||   !list_rewind( statement->element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "statement is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	trial_balance_subclass_omit_html =
		trial_balance_subclass_omit_html_calloc();

	trial_balance_subclass_omit_html->column_list =
		trial_balance_subclass_omit_html_column_list(
			statement_prior_year_list );

	trial_balance_subclass_omit_html->row_list = list_new();

	do {
		element = list_get( statement->element_statement_list );

		if ( !list_length( element->account_statement_list ) )
		{
			continue;
		}

		list_set_list(
			trial_balance_subclass_omit_html->row_list,
			trial_balance_subclass_omit_html_row_list(
				statement->end_date_time_string,
				element->element_name,
				element->accumulate_debit,
				element->account_statement_list,
				statement_prior_year_list ) );

	} while ( list_next( statement->element_statement_list ) );

	total_row =
		trial_balance_html_total_row(
			debit_sum,
			credit_sum,
			2 /* column_skip_count */ );

	list_set( trial_balance_subclass_omit_html->row_list, total_row );

	return trial_balance_subclass_omit_html;
}

TRIAL_BALANCE_SUBCLASS_OMIT_HTML *
	trial_balance_subclass_omit_html_calloc(
			void )
{
	TRIAL_BALANCE_SUBCLASS_OMIT_HTML *trial_balance_subclass_omit_html;

	if ( ! ( trial_balance_subclass_omit_html =
			calloc(
			   1,
			   sizeof ( TRIAL_BALANCE_SUBCLASS_OMIT_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_subclass_omit_html;
}

LIST *trial_balance_subclass_omit_html_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			"Element",
			0 /* not right_Justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Account",
			0 /* not right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Count",
			1 /* right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Debit",
			1 /* right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Credit",
			1 /* right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"percent_of_asset",
			1 /* right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"percent_of_income",
			1 /* right_justify_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_heading_list =
			statement_prior_year_heading_list(
				statement_prior_year_list );

		list_set_list(
			column_list,
			html_column_right_list(
				prior_year_heading_list ) );
	}

	return column_list;
}

LIST *trial_balance_subclass_omit_html_row_list(
		char *end_date_time_string,
		char *element_name,
		boolean element_accumulate_debit,
		LIST *account_statement_list,
		LIST *statement_prior_year_list )
{
	LIST *row_list;
	ACCOUNT *account;
	STATEMENT_ACCOUNT *statement_account;

	if ( !end_date_time_string
	||   !list_rewind( account_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
		"for account_name=[%s], account_journal_latest is empty.",
				account->account_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		statement_account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statement_account_new(
				end_date_time_string,
				element_accumulate_debit,
				account->account_journal_latest,
				account->action_string,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			row_list,
			trial_balance_subclass_omit_html_row(
				element_name,
				statement_account,
				statement_prior_year_list ) );

		element_name = (char *)0;

	} while ( list_next( account_statement_list ) );

	return row_list;
}

HTML_ROW *trial_balance_subclass_omit_html_row(
		char *element_name,
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;
	char *cell_label_datum;
	char *transaction_date_american;
	char *account_datum;

	if ( !statement_account
	||   !statement_account->account
	||   !statement_account->account->account_journal_latest
	||   !statement_account->account->account_journal_latest->transaction )
	{
		char message[ 128 ];

		sprintf(message, "statement_account is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			element_name );

	list_set(
		cell_list,
		html_cell_new(
		cell_label_datum,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ ) );

	if ( ! ( transaction_date_american =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				statement_account->
					account->
					account_journal_latest->
					transaction_date_time ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"statement_date_american(%s) returned empty.",
			statement_account->
				account->
				account_journal_latest->
				transaction_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	account_datum =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_html_account_datum(
			statement_account->account->account_name,
			statement_account->
				account->
				account_journal_latest->
				full_name,
			statement_account->
				account->
				account_journal_latest->
				debit_amount,
			statement_account->
				account->
				account_journal_latest->
				credit_amount,
			transaction_date_american,
			statement_account->
				account->
				account_journal_latest->
				transaction->memo );

	list_set(
		cell_list,
		html_cell_new(
			account_datum,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			trial_balance_transaction_count_string(
				statement_account->
					account->
					transaction_count ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->debit_string,
			0 /* not large_boolean */,
			statement_account->
				within_days_between_boolean
					/* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->credit_string,
			0 /* not large_boolean */,
			statement_account->
				within_days_between_boolean
					/* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->asset_percent_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->revenue_percent_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_account_data_list =
			statement_prior_year_account_data_list(
				statement_account->account->account_name,
				statement_prior_year_list );

		list_set(
			cell_list,
			html_cell_list(
				prior_year_account_data_list ) );
	}

	return
	html_row_new( cell_list );
}

TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *
	trial_balance_subclass_omit_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum )
{
	TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *trial_balance_subclass_omit_latex;
	ELEMENT *element;
	LATEX_ROW *latex_row;

	if ( !statement
	||   !statement->end_date_time_string
	||   !list_rewind( statement->element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "statement is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	trial_balance_subclass_omit_latex =
		trial_balance_subclass_omit_latex_calloc();

	trial_balance_subclass_omit_latex->column_list =
		trial_balance_subclass_omit_latex_column_list(
			statement_prior_year_list );

	trial_balance_subclass_omit_latex->row_list = list_new();

	do {
		element = list_get( statement->element_statement_list );

		if ( !list_length( element->account_statement_list ) )
			continue;

		list_set_list(
			trial_balance_subclass_omit_latex->row_list,
		   	trial_balance_subclass_omit_latex_row_list(
				statement->end_date_time_string,
				element->element_name,
				element->accumulate_debit,
				element->account_statement_list,
				statement_prior_year_list,
				trial_balance_subclass_omit_latex->
					column_list
					/* latex_column_list */ ) );

	} while ( list_next( statement->element_statement_list ) );

	latex_row =
		trial_balance_latex_total_row(
			debit_sum,
			credit_sum,
			trial_balance_subclass_omit_latex->column_list,
			2 /* column_skip_count */ );

	list_set( trial_balance_subclass_omit_latex->row_list, latex_row );

	return trial_balance_subclass_omit_latex;
}

TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *
	trial_balance_subclass_omit_latex_calloc(
			void )
{
	TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *trial_balance_subclass_omit_latex;

	if ( ! ( trial_balance_subclass_omit_latex =
			calloc(
			   1,
			   sizeof ( TRIAL_BALANCE_SUBCLASS_OMIT_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_subclass_omit_latex;
}

LIST *trial_balance_subclass_display_latex_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		latex_column_new(
			"element" /* heading_string */,
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );
 
	list_set(
		column_list,
		latex_column_new(
			"subclassification" /* heading_string */,
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"account" /* heading_string */,
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			"5.5cm" /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"count" /* heading_string */,
			latex_column_integer /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"debit" /* heading_string */,
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"credit" /* heading_string */,
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	LATEX_COLUMN *latex_column =
		latex_column_new(
			"standardized" /* heading_string */,
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ );

	latex_column->right_justify_boolean  = 1;

	list_set( column_list, latex_column );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_heading_list =
			statement_prior_year_heading_list(
				statement_prior_year_list );

		list_set_list(
			column_list,
			latex_column_text_list(
				prior_year_heading_list,
				0 /* not first_column_boolean */,
				1 /* right_justify_boolean */ ) );
	}

	return column_list;
}

char *trial_balance_preclose_process_name( char *process_name )
{
	static char preclose_process_name[ 128 ];

	sprintf(preclose_process_name,
		"%s (preclose)",
		process_name );

	return preclose_process_name;
}

LATEX_ROW *trial_balance_subclass_omit_latex_row(
		char *element_name,
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	char *label_datum;
	LATEX_COLUMN *latex_column;
	LATEX_CELL *latex_cell;
	char *date_american;
	char *account_datum;
	char *transaction_count_string;

	if ( !statement_account
	||   !statement_account->account
	||   !statement_account->account->account_journal_latest
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			element_name );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			label_datum );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	if ( ! ( date_american =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				statement_account->
					account->
					account_journal_latest->
					transaction_date_time ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"statement_date_american(%s) returned empty.",
			statement_account->
				account->
				account_journal_latest->
				transaction_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	account_datum =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_latex_account_datum(
			statement_account->account->account_name,
			statement_account->
				account->
				account_journal_latest->
				full_name,
			statement_account->
				account->
				account_journal_latest->
				debit_amount,
			statement_account->
				account->
				account_journal_latest->
				credit_amount,
			date_american,
			statement_account->
				account->
				account_journal_latest->
				transaction->
				memo );

	free( date_american );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			account_datum );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	transaction_count_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_transaction_count_string(
			statement_account->account->transaction_count );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			transaction_count_string );

	list_set( cell_list, latex_cell );

	list_next( latex_column_list );
	latex_column = list_get( latex_column_list );

	latex_cell =
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			statement_account->debit_string /* datum */,
			0 /* not large_boolean */,
			statement_account->within_days_between_boolean
				/* bold_boolean */ );

	list_set( cell_list, latex_cell );

	list_next( latex_column_list );
	latex_column = list_get( latex_column_list );

	latex_cell =
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			statement_account->credit_string /* datum */,
			0 /* not large_boolean */,
			statement_account->within_days_between_boolean
				/* bold_boolean */ );

	list_set( cell_list, latex_cell );

	list_next( latex_column_list );
	latex_column = list_get( latex_column_list );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			statement_account->percent_string /* datum */ );

	list_set( cell_list, latex_cell );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_account_data_list =
			statement_prior_year_account_data_list(
				statement_account->account->account_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_year_account_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

LATEX_ROW *trial_balance_latex_total_row(
		double debit_sum,
		double credit_sum,
		LIST *latex_column_list,
		int column_skip_count )
{
	LATEX_COLUMN *latex_column;
	LIST *cell_list;
	int i;

	if ( !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "latex_column_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			"Total" /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for ( i = 0; i < column_skip_count; i++ )
	{

		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_double(
					debit_sum,
					0 /* decimal_place_count */ ) )
					/* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_double(
					credit_sum,
					0 /* decimal_place_count */ ) )
					/* datum */ ) );

	latex_column = list_get( latex_column_list );

	while ( list_still_more( latex_column_list ) )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	latex_row_new( cell_list );
}

LIST *trial_balance_subclass_omit_latex_row_list(
		char *end_date_time_string,
		char *element_name,
		boolean element_accumulate_debit,
		LIST *account_statement_list,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *row_list;
	LATEX_ROW *latex_row;
	ACCOUNT *account;
	STATEMENT_ACCOUNT *statement_account;

	if ( !end_date_time_string
	||   !list_length( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( account_statement_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
			"for account=[%s], account_journal_latest is empty.",
				account->account_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		statement_account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statement_account_new(
				end_date_time_string,
				element_accumulate_debit,
				account->account_journal_latest,
				(char *)0 /* account_action_string */,
				1 /* round_dollar_boolean */,
				account );

		latex_row =
			trial_balance_subclass_omit_latex_row(
				element_name,
				statement_account,
				statement_prior_year_list,
				latex_column_list );

		list_set( row_list, latex_row );

		element_name = (char *)0;

	} while ( list_next( account_statement_list ) );

	return row_list;
}

LIST *trial_balance_subclass_omit_latex_column_list(
		LIST *statement_prior_year_list )
{
	LIST *latex_column_list;
	LATEX_COLUMN *latex_column;

	latex_column_list = list_new();

	list_set(
		latex_column_list,
		latex_column_new(
			"element" /* heading_string */,
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		latex_column_list,
		latex_column_new(
			"account" /* heading_string */,
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			"8.0cm" /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		latex_column_list,
		latex_column_new(
			"count" /* heading_string */,
			latex_column_integer /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		latex_column_list,
		latex_column_new(
			"debit" /* heading_string */,
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		latex_column_list,
		latex_column_new(
			"credit" /* heading_string */,
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	latex_column =
		latex_column_new(
			"standardized" /* heading_string */,
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ );

	latex_column->right_justify_boolean = 1;

	list_set( latex_column_list, latex_column );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *column_text_list =
			latex_column_text_list(
				statement_prior_year_heading_list(
					statement_prior_year_list ),
				0 /* not first_column_boolean */,
				1 /* right_justify_boolean */ );

		list_set_list( latex_column_list, column_text_list );
	}

	return latex_column_list;
}

LATEX_ROW *trial_balance_subclass_display_latex_row(
		char *element_name,
		char *subclassification_name,
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	char *cell_label_datum;
	LATEX_CELL *latex_cell;
	char *date_american;
	char *account_datum;
	char *transaction_count_string;

	if ( !statement_account
	||   !statement_account->account
	||   !statement_account->account->account_journal_latest
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			element_name );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			cell_label_datum );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum( 
			subclassification_name );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			cell_label_datum );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	if ( ! ( date_american =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				statement_account->
					account->
					account_journal_latest->
					transaction_date_time ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"statement_date_american(%s) returned empty.",
			statement_account->
				account->
				account_journal_latest->
				transaction_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	account_datum =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_latex_account_datum(
			statement_account->account->account_name,
			statement_account->
				account->
				account_journal_latest->
				full_name,
			statement_account->
				account->
				account_journal_latest->
				debit_amount,
			statement_account->
				account->
				account_journal_latest->
				credit_amount,
			date_american,
			statement_account->
				account->
				account_journal_latest->
				transaction->
				memo );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			account_datum );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	transaction_count_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_transaction_count_string(
			statement_account->account->transaction_count );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			transaction_count_string );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	latex_cell =
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			statement_account->debit_string /* datum */,
			0 /* not large_boolean */,
			statement_account->within_days_between_boolean
				/* bold_boolean */ );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	latex_cell =
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			statement_account->credit_string /* datum */,
			0 /* not large_boolean */,
			statement_account->within_days_between_boolean
				/* bold_boolean */ );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			statement_account->percent_string /* datum */ );

	list_set( cell_list, latex_cell );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_account_data_list =
			statement_prior_year_account_data_list(
				statement_account->account->account_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_year_account_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

HTML_ROW *trial_balance_html_total_row(
		double debit_sum,
		double credit_sum,
		int column_skip_count )
{
	int i;
	LIST *cell_list;

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			"Total",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	for ( i = 0; i < column_skip_count; i++ )
	{
		list_set(
			cell_list,
			html_cell_new(
				(char *)0 /* datum */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				string_commas_double(
					debit_sum,
					2 /* decimal_place_count */ ) )
				/* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				string_commas_double(
					credit_sum,
					2 /* decimal_place_count */ ) )
				/* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return
	html_row_new( cell_list );
}
