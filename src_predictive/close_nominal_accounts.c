/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/close_nominal_accounts.c		*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "date.h"
#include "list_usage.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "ledger.h"
#include "entity.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean close_nominal_accounts_execute(
					char *application_name,
					char *as_of_date );

boolean close_nominal_accounts_fund_execute(
					char *application_name,
					char *fund_name,
					char *transaction_date_time_string,
					char *as_of_date );

double insert_journal_ledger(		FILE *output_pipe,
					LIST *subclassification_list,
					boolean accumulate_debit,
					char *full_name,
					char *street_address,
					char *transaction_date_time_string );

double output_subclassification_list(
				HTML_TABLE *html_table,
				LIST *subclassification_list,
				boolean accumulate_debit );

void close_nominal_accounts_display(
				char *application_name,
				char *as_of_date );

void close_nominal_accounts_fund_display(
				char *application_name,
				char *fund_name,
				char *transaction_date_time_string,
				char *as_of_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *as_of_date;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 128 ];
	boolean execute;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
		"Usage: %s process as_of_date execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	as_of_date = argv[ 2 ];
	execute = ( *argv[ 3 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	format_initial_capital( title, process_name ),

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	if ( !*as_of_date
	||   strcmp( as_of_date, "as_of_date" ) == 0 )
	{
		printf( "<h3>Please enter an as of date.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( execute )
	{
		if ( close_nominal_accounts_execute(
			application_name,
			as_of_date ) )
		{
			printf( "<h3>Process complete.</h3>\n" );
		}
		else
		{
			printf( "<h3>Cannot close accounts. See log.</h3>\n" );
		}
	}
	else
	{
		close_nominal_accounts_display(
			application_name,
			as_of_date );
	}

	document_close();

	return 0;

} /* main() */

boolean close_nominal_accounts_execute(
				char *application_name,
				char *as_of_date )
{
	char *transaction_date_time;
	char *fund_name;
	LIST *fund_name_list;
	char sys_string[ 1024 ];

	if ( ( transaction_date_time =
			ledger_get_existing_closing_transaction_date_time(
				application_name,
				as_of_date ) ) )
	{
		char msg[ 1024 ];

		sprintf( msg,
			 "Error in %s/%s()/%d: accounts were closed on %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 transaction_date_time );
		m2( application_name, msg );
		return 0;
	}

	transaction_date_time =
		ledger_get_closing_transaction_date_time(
			as_of_date );

	sprintf( sys_string,
		 "folder_attribute_exists.sh %s account fund",
		 application_name );

	/* If no ACCOUNT.fund attribute */
	/* ---------------------------- */
	if ( system( sys_string ) != 0 )
	{
		return close_nominal_accounts_fund_execute(
				application_name,
				(char *)0 /* fund_name */,
				transaction_date_time,
				as_of_date );
	}

	fund_name_list = ledger_get_fund_name_list( application_name );

	if ( !list_rewind( fund_name_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty fund_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		fund_name = list_get_pointer( fund_name_list );

		if ( !close_nominal_accounts_fund_execute(
			application_name,
			fund_name,
			transaction_date_time,
			as_of_date ) )
		{
			return 0;
		}

	} while( list_next( fund_name_list ) );

	return 1;

} /* close_nominal_accounts_execute() */

boolean close_nominal_accounts_fund_execute(
				char *application_name,
				char *fund_name,
				char *transaction_date_time_string,
				char *as_of_date )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	LIST *filter_element_name_list;
	LIST *element_list;
	double retained_earnings;
	char *field_list;
	LEDGER_ELEMENT *element;
	ENTITY_SELF *self;
	TRANSACTION *transaction;
	char *table_name;
	char *closing_entry_account;

	closing_entry_account =
		ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				"closing_key",
				0 /* not warning_only */,
				__FUNCTION__ );


	table_name = get_table_name( application_name, LEDGER_FOLDER_NAME );

	if ( ! ( self = entity_self_load( application_name ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot fetch from SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0;
	}

	transaction = ledger_transaction_new(
				self->entity->full_name,
				self->entity->street_address,
				transaction_date_time_string,
				LEDGER_CLOSING_ENTRY_MEMO );

	field_list =
"full_name,street_address,transaction_date_time,account,debit_amount,credit_amount";

	/* Get the element list. */
	/* --------------------- */
	filter_element_name_list = list_new();

	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );
/*
	list_append_pointer(	filter_element_name_list,
				LEDGER_EQUITY_ELEMENT );
*/

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			0 /* not omit_subclassification */ );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s delimiter='^' 	|"
		 "sql.e							 ",
		 table_name,
		 field_list );

	output_pipe = popen( sys_string, "w" );

	/* Revenues */
	/* -------- */
	if ( ! ( element = ledger_element_seek(	element_list,
						LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
		exit( 1 );
	}

	retained_earnings =
		insert_journal_ledger(
			output_pipe,
			element->subclassification_list,
			element->accumulate_debit,
			self->entity->full_name,
			self->entity->street_address,
			transaction_date_time_string );

	/* Expenses */
	/* -------- */
	if ( ! ( element = ledger_element_seek(	element_list,
						LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
		exit( 1 );
	}

	retained_earnings -=
		insert_journal_ledger(
			output_pipe,
			element->subclassification_list,
			element->accumulate_debit,
			self->entity->full_name,
			self->entity->street_address,
			transaction_date_time_string );

	/* Gains */
	/* ----- */
	if ( ! ( element = ledger_element_seek(	element_list,
						LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
		exit( 1 );
	}

	retained_earnings +=
		insert_journal_ledger(
			output_pipe,
			element->subclassification_list,
			element->accumulate_debit,
			self->entity->full_name,
			self->entity->street_address,
			transaction_date_time_string );

	/* Losses */
	/* ------ */
	if ( ! ( element = ledger_element_seek(	element_list,
						LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
		exit( 1 );
	}

	retained_earnings -=
		insert_journal_ledger(
			output_pipe,
			element->subclassification_list,
			element->accumulate_debit,
			self->entity->full_name,
			self->entity->street_address,
			transaction_date_time_string );

	/* Insert retained_earnings */
	/* ------------------------ */
	if ( retained_earnings > 0.0 )
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^%s^^%.2lf\n",
		 	self->entity->full_name,
		 	self->entity->street_address,
		 	transaction_date_time_string,
		 	closing_entry_account,
		 	retained_earnings );
	}
	else
	{
		fprintf( output_pipe,
		 	"%s^%s^%s^%s^%.2lf^\n",
		 	self->entity->full_name,
		 	self->entity->street_address,
		 	transaction_date_time_string,
		 	closing_entry_account,
		 	-retained_earnings );
	}

	pclose( output_pipe );

	ledger_propagate_element_list(
				application_name,
				transaction_date_time_string,
				element_list );

	ledger_propagate(
		application_name,
		transaction_date_time_string,
		closing_entry_account );

	transaction_date_time_string =
		ledger_transaction_insert(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction_date_time_string,
			retained_earnings /* transaction_amount */,
			transaction->memo,
			0 /* check_number */,
			0 /* not lock_transaction */ );

	return 1;

} /* close_nominal_accounts_fund_execute() */

void close_nominal_accounts_display(
				char *application_name,
				char *as_of_date )
{
	char *transaction_date_time;
	char *fund_name;
	LIST *fund_name_list;
	char sys_string[ 512 ];

	sprintf( sys_string,
		 "folder_attribute_exists.sh %s account fund",
		 application_name );

	/* If no ACCOUNT.fund attribute */
	/* ---------------------------- */
	if ( system( sys_string ) != 0 )
	{
		transaction_date_time =
			ledger_get_closing_transaction_date_time(
				as_of_date );

		if ( ledger_transaction_date_time_exists(
				application_name,
				transaction_date_time ) )
		{
			printf(
	"<h3>Warning: accounts were closed already on this date.</h3>\n" );
		}
		else
		{
			close_nominal_accounts_fund_display(
				application_name,
				(char *)0 /* fund_name */,
				transaction_date_time,
				as_of_date );
		}

		return;
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=fund	"
		 "			folder=fund	",
		 application_name );

	fund_name_list = pipe2list( sys_string );

	if ( !list_rewind( fund_name_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty fund_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		fund_name = list_get_pointer( fund_name_list );

		transaction_date_time =
			ledger_get_closing_transaction_date_time(
				as_of_date );

		close_nominal_accounts_fund_display(
			application_name,
			fund_name,
			transaction_date_time,
			as_of_date );

	} while( list_next( fund_name_list ) );

} /* close_nominal_accounts_display() */

void close_nominal_accounts_fund_display(
					char *application_name,
					char *fund_name,
					char *transaction_date_time,
					char *as_of_date )
{
	HTML_TABLE *html_table;
	LIST *element_list;
	LIST *heading_list;
	LIST *filter_element_name_list;
	LEDGER_ELEMENT *element;
	char title[ 128 ];
	char sub_title[ 128 ];
	char buffer[ 128 ];
	ENTITY_SELF *self;
	char *closing_entry_account;
	double retained_earnings;
	double debit_sum;
	double credit_sum;
	double element_total;

	closing_entry_account =
		ledger_get_hard_coded_account_name(
				application_name,
				fund_name,
				"closing_key",
				0 /* not warning_only */,
				__FUNCTION__ );

	if ( ! ( self = entity_self_load( application_name ) ) )
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot fetch from ENTITY_SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	/* Get the element list. */
	/* --------------------- */
	filter_element_name_list = list_new();

	list_append_pointer(	filter_element_name_list,
				LEDGER_REVENUE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_EXPENSE_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_GAIN_ELEMENT );
	list_append_pointer(	filter_element_name_list,
				LEDGER_LOSS_ELEMENT );
/*
	list_append_pointer(	filter_element_name_list,
				LEDGER_EQUITY_ELEMENT );
*/

	element_list =
		ledger_get_element_list(
			application_name,
			filter_element_name_list,
			fund_name,
			as_of_date,
			0 /* not omit_subclassification */ );

	/* Output the elements. */
	/* -------------------- */
	heading_list = list_new();
	list_append_pointer( heading_list, "Account" );
	list_append_pointer( heading_list, "Debit" );
	list_append_pointer( heading_list, "Credit" );

	if ( fund_name && *fund_name )
	{
		sprintf(	title,
				"Fund: %s",
				format_initial_capital( buffer, fund_name ) );
	}
	else
	{
		*title = '\0';
	}

	sprintf(	sub_title,
			"Transaction Date Time: %s",
			transaction_date_time );

	html_table = new_html_table(
			title,
			sub_title ); 

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 2;
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );
	html_table_output_data_heading(
			heading_list,
			html_table->number_left_justified_columns,
			html_table->number_right_justified_columns,
			html_table->justify_list );

	/* Revenues */
	/* -------- */
	if ( ! ( element = ledger_element_seek(	element_list,
						LEDGER_REVENUE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_REVENUE_ELEMENT );
		exit( 1 );
	}

	element_total =
		output_subclassification_list(
			html_table,
			element->subclassification_list,
			element->accumulate_debit );

	retained_earnings = element_total;
	debit_sum = element_total;

	/* Expenses */
	/* -------- */
	if ( ! ( element = ledger_element_seek(	element_list,
						LEDGER_EXPENSE_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_EXPENSE_ELEMENT );
		exit( 1 );
	}

	element_total =
		output_subclassification_list(
			html_table,
			element->subclassification_list,
			element->accumulate_debit );

	retained_earnings -= element_total;
	credit_sum = element_total;

	/* Gains */
	/* ----- */
	if ( ! ( element = ledger_element_seek(	element_list,
						LEDGER_GAIN_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_GAIN_ELEMENT );
		exit( 1 );
	}

	element_total =
		output_subclassification_list(
			html_table,
			element->subclassification_list,
			element->accumulate_debit );

	retained_earnings += element_total;
	debit_sum += element_total;

	/* Losses */
	/* ------ */
	if ( ! ( element = ledger_element_seek(	element_list,
						LEDGER_LOSS_ELEMENT ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 LEDGER_LOSS_ELEMENT );
		exit( 1 );
	}

	element_total =
		output_subclassification_list(
			html_table,
			element->subclassification_list,
			element->accumulate_debit );

	retained_earnings -= element_total;
	credit_sum += element_total;
	credit_sum += retained_earnings;

	/* Retained Earnings */
	/* ----------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( format_initial_capital(
			buffer,
			closing_entry_account ) ) );

	if ( retained_earnings > 0.0  )
	{
		html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

		html_table_set_data(
				html_table->data_list,
				strdup( place_commas_in_money(
					retained_earnings ) ) );
	}
	else
	{
		html_table_set_data(
				html_table->data_list,
				strdup( place_commas_in_money(
					-retained_earnings ) ) );

		html_table_set_data(
				html_table->data_list,
				strdup( "" ) );
	}

	/* Output retained earnings */
	/* ------------------------ */
	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table->data_list = list_new();

	/* Total */
	/* ----- */
	html_table_set_data(
		html_table->data_list,
		strdup( "Total" ) );

	html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money( debit_sum ) ) );

	html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money( credit_sum ) ) );

	/* Output total */
	/* ------------ */
	html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

	html_table_close();

} /* close_nominal_accounts_fund_display() */

double output_subclassification_list(
					HTML_TABLE *html_table,
					LIST *subclassification_list,
					boolean accumulate_debit )
{
	double retained_earnings = 0.0;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	char buffer[ 128 ];

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification =
			list_get_pointer(
				subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( strcmp(
			subclassification->subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			LEDGER_SUBCLASSIFICATION_CONTRIBUTED_CAPITAL ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			LEDGER_SUBCLASSIFICATION_RETAINED_EARNINGS ) == 0 )
		{
			continue;
		}

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			html_table_set_data(
				html_table->data_list,
				strdup( format_initial_capital(
					buffer,
					account->account_name ) ) );

			if ( accumulate_debit )
			{
				html_table_set_data(
					html_table->data_list,
					strdup( "" ) );
			}

			html_table_set_data(
				html_table->data_list,
				strdup( place_commas_in_money(
					account->
						latest_ledger->
						balance ) ) );
	
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

			retained_earnings +=
				account->latest_ledger->balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return retained_earnings;

} /* output_subclassification_list() */

double insert_journal_ledger(
					FILE *output_pipe,
					LIST *subclassification_list,
					boolean accumulate_debit,
					char *full_name,
					char *street_address,
					char *transaction_date_time_string )
{
	double retained_earnings = 0.0;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification =
			list_get_pointer(
				subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( strcmp(
			subclassification->subclassification_name,
			LEDGER_SUBCLASSIFICATION_NET_ASSETS ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			LEDGER_SUBCLASSIFICATION_CONTRIBUTED_CAPITAL ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			LEDGER_SUBCLASSIFICATION_RETAINED_EARNINGS ) == 0 )
		{
			continue;
		}

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_ledger
			||   !account->latest_ledger->balance )
				continue;

			if ( accumulate_debit )
			{
				fprintf( output_pipe,
					 "%s^%s^%s^%s^^%.2lf\n",
					 full_name,
					 street_address,
					 transaction_date_time_string,
					 account->account_name,
					 account->latest_ledger->balance );
			}
			else
			{
				fprintf( output_pipe,
					 "%s^%s^%s^%s^%.2lf^\n",
					 full_name,
					 street_address,
					 transaction_date_time_string,
					 account->account_name,
					 account->latest_ledger->balance );
			}

			retained_earnings +=
				account->latest_ledger->balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return retained_earnings;

} /* insert_journal_ledger() */

