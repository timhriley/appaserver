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
#include "html_table.h"
#include "entity.h"
#include "entity_self.h"
#include "subclassification.h"
#include "element.h"
#include "journal.h"
#include "transaction.h"
#include "statement.h"
#include "predictive.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
double insert_drawing(
			FILE *output_pipe,
			LIST *subclassification_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time_string,
			char *drawing_account );

double output_drawing_subclassification_list(
			HTML_TABLE *html_table,
			LIST *subclassification_list,
			char *drawing_account );

boolean close_nominal_accounts_execute(
			char *as_of_date );

boolean close_nominal_accounts_fund_execute(
			char *fund_name,
			char *transaction_date_time_string,
			char *as_of_date );

double insert_journal(	FILE *output_pipe,
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
			char *as_of_date );

void close_nominal_accounts_fund_display(
			char *fund_name,
			char *transaction_date_time_string,
			char *as_of_date,
			char *drawing_account );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *as_of_date;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 128 ];
	boolean execute;

	application_name = environ_exit_application_name( argv[ 0 ] );

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
		close_nominal_accounts_display( as_of_date );
	}

	document_close();

	return 0;
}

boolean close_nominal_accounts_execute( char *as_of_date )
{
	char *transaction_date_time;
	char *fund_name;
	LIST *fund_name_list;
	char sys_string[ 1024 ];

	if ( transaction_closing_entry_exists( as_of_date ) )
	{
		return 0;
	}

	transaction_date_time =
		transaction_date_time_closing(
			as_of_date,
			0 /* not preclose_time */,
			0 /* not closing_entry_exists */ );

	sprintf( sys_string,
		 "folder_attribute_exists.sh %s account fund",
		 environment_application_name() );

	/* If no ACCOUNT.fund attribute */
	/* ---------------------------- */
	if ( system( sys_string ) != 0 )
	{
		return close_nominal_accounts_fund_execute(
				(char *)0 /* fund_name */,
				transaction_date_time,
				as_of_date );
	}

	fund_name_list = statement_fund_name_list();

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
		fund_name = list_get( fund_name_list );

		if ( !close_nominal_accounts_fund_execute(
			fund_name,
			transaction_date_time,
			as_of_date ) )
		{
			return 0;
		}

	} while( list_next( fund_name_list ) );

	return 1;
}

boolean close_nominal_accounts_fund_execute(
			char *fund_name,
			char *transaction_date_time_string,
			char *as_of_date )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	LIST *filter_element_name_list;
	LIST *list;
	char *field_list;
	ELEMENT *element;
	ENTITY_SELF *self;
	TRANSACTION *transaction;
	char *closing_entry_account;
	char *drawing_account;
	double element_total;
	double retained_earnings = {0};
	char *transaction_date_time_closing_string;

	closing_entry_account =
		account_hard_coded_account_name(
				fund_name,
				ACCOUNT_CLOSING_KEY,
				0 /* not warning_only */,
				__FUNCTION__ );

	drawing_account =
		account_hard_coded_account_name(
				fund_name,
				ACCOUNT_DRAWING_KEY,
				1 /* warning_only */,
				__FUNCTION__ );

	if ( ! ( self = entity_self_load() ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot fetch from SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0;
	}

	transaction =
		transaction_new(
			self->entity->full_name,
			self->entity->street_address,
			transaction_date_time_string );

	transaction->memo = TRANSACTION_CLOSING_ENTRY_MEMO;

	field_list =
		"full_name,"
		"street_address,"
		"transaction_date_time,"
		"account,"
		"debit_amount,"
		"credit_amount";

	/* Get the element list. */
	/* --------------------- */
	filter_element_name_list = list_new();

	list_append_pointer(	filter_element_name_list,
				ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LOSS );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	transaction_date_time_closing_string =
		transaction_date_time_closing(
			as_of_date,
			1 /* preclose_time */,
			transaction_closing_entry_exists(
				as_of_date ) );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing_string,
			transaction_date_time_closing_string,
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^' 	|"
		 "sql							 ",
		 JOURNAL_TABLE,
		 field_list );

	output_pipe = popen( sys_string, "w" );

	/* Revenues */
	/* -------- */
	if ( ( element =
			element_seek(
				ELEMENT_REVENUE,
				list ) ) )
	{
		element_total =
			insert_journal(
				output_pipe,
				element->subclassification_list,
				element->accumulate_debit,
				self->entity->full_name,
				self->entity->street_address,
				transaction_date_time_string );

		retained_earnings = element_total;
	}

	/* Expenses */
	/* -------- */
	if ( ( element =
			element_seek(
				ELEMENT_EXPENSE,
				list ) ) )
	{
		element_total =
			insert_journal(
				output_pipe,
				element->subclassification_list,
				element->accumulate_debit,
				self->entity->full_name,
				self->entity->street_address,
				transaction_date_time_string );

		retained_earnings -= element_total;
	}

	/* Gains */
	/* ----- */
	if ( ( element =
			element_seek(
				ELEMENT_GAIN,
				list ) ) )
	{
		element_total =
			insert_journal(
				output_pipe,
				element->subclassification_list,
				element->accumulate_debit,
				self->entity->full_name,
				self->entity->street_address,
				transaction_date_time_string );

		retained_earnings += element_total;
	}

	/* Losses */
	/* ------ */
	if ( ( element =
			element_seek(
				ELEMENT_LOSS,
				list ) ) )
	{
		element_total =
			insert_journal(
				output_pipe,
				element->subclassification_list,
				element->accumulate_debit,
				self->entity->full_name,
				self->entity->street_address,
				transaction_date_time_string );

		retained_earnings -= element_total;
	}

	/* Drawing */
	/* ------- */
	if ( drawing_account )
	{
		if ( ( element =
				element_seek(
					ELEMENT_EQUITY,
					list ) ) )
		{
			element_total =
				insert_drawing(
					output_pipe,
					element->subclassification_list,
					self->entity->full_name,
					self->entity->street_address,
					transaction_date_time_string,
					drawing_account );

			retained_earnings -= element_total;
		}
	}

	/* Insert retained_earnings */
	/* ------------------------ */
	if ( retained_earnings > 0.0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^%s^^%.2lf\n",
		 	self->entity->full_name,
		 	self->entity->street_address,
		 	transaction_date_time_string,
		 	closing_entry_account,
		 	retained_earnings );
	}
	else
	if ( retained_earnings < 0.0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^%s^%.2lf^\n",
		 	self->entity->full_name,
		 	self->entity->street_address,
		 	transaction_date_time_string,
		 	closing_entry_account,
		 	-retained_earnings );
	}

	pclose( output_pipe );

	transaction_insert(
		transaction->full_name,
		transaction->street_address,
		transaction_date_time_string,
		timlib_abs_double( retained_earnings )
			/* transaction_amount */,
		transaction->memo,
		0 /* check_number */,
		0 /* not lock_transaction */,
		0 /* not replace */ );

	element_list_propagate(
		list,
		transaction_date_time_string );

	return 1;
}

void close_nominal_accounts_display( char *as_of_date )
{
	char *transaction_date_time;
	LIST *fund_name_list;
	char *fund_name;
	char sys_string[ 512 ];
	char *drawing_account;

	sprintf( sys_string,
		 "folder_attribute_exists.sh %s account fund",
		 environment_application_name() );

	/* If no ACCOUNT.fund attribute */
	/* ---------------------------- */
	if ( system( sys_string ) != 0 )
	{
		transaction_date_time =
			transaction_date_time_closing(
				as_of_date,
				0 /* not preclose_time */,
				0 /* not closing_entry_exists */ );

		if ( transaction_closing_entry_exists(
				transaction_date_time ) )
		{
			printf(
	"<h3>Warning: accounts were closed already on this date.</h3>\n" );
		}
		else
		{
			drawing_account =
				account_hard_coded_account_name(
					(char *)0 /* fund_name */,
					ACCOUNT_DRAWING_KEY,
					1 /* warning_only */,
					__FUNCTION__ );

			close_nominal_accounts_fund_display(
				(char *)0 /* fund_name */,
				transaction_date_time,
				as_of_date,
				drawing_account );
		}

		return;
	}

	sprintf( sys_string,
		 "echo \"select %s from %s order by %s;\" | sql",
		 "fund",
		 "fund",
		 "fund" );

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
		fund_name = list_get( fund_name_list );

		drawing_account =
			account_hard_coded_account_name(
				fund_name,
				ACCOUNT_DRAWING_KEY,
				1 /* warning_only */,
				__FUNCTION__ );

		transaction_date_time =
			transaction_date_time_closing(
				as_of_date,
				0 /* not preclose_time */,
				0 /* not closing_entry_exists */ );

		close_nominal_accounts_fund_display(
			fund_name,
			transaction_date_time,
			as_of_date,
			drawing_account );

	} while( list_next( fund_name_list ) );
}

void close_nominal_accounts_fund_display(
			char *fund_name,
			char *transaction_date_time,
			char *as_of_date,
			char *drawing_account )
{
	HTML_TABLE *html_table;
	LIST *list;
	LIST *heading_list;
	LIST *filter_element_name_list;
	ELEMENT *element;
	char title[ 128 ];
	char sub_title[ 128 ];
	char buffer[ 128 ];
	ENTITY_SELF *self;
	char *closing_entry_account;
	double retained_earnings = 0.0;
	double debit_sum = 0.0;
	double credit_sum = 0.0;
	double element_total;
	char *transaction_date_time_closing_string;

	closing_entry_account =
		account_hard_coded_account_name(
			fund_name,
			ACCOUNT_CLOSING_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	if ( ! ( self = entity_self_load() ) )
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
				ELEMENT_REVENUE );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EXPENSE );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_GAIN );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_LOSS );
	list_append_pointer(	filter_element_name_list,
				ELEMENT_EQUITY );

	transaction_date_time_closing_string =
		transaction_date_time_closing(
			as_of_date,
			1 /* preclose_time */,
			transaction_closing_entry_exists(
				as_of_date ) );

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing_string,
			transaction_date_time_closing_string,
			1 /* fetch_subclassifiction_list */,
			0 /* not fetch_account_list */ );

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

	sprintf(sub_title,
		"Transaction Date Time: %s",
		transaction_date_time );

	html_table =
		html_table_new(
			title,
			sub_title,
			(char *)0 /* subsub_title */ ); 

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
	if ( ! ( element =
			element_seek(
				ELEMENT_REVENUE,
				list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_REVENUE );
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
	if ( ! ( element =
			element_seek(
				ELEMENT_EXPENSE,
				list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_EXPENSE );
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
	if ( ! ( element =
			element_seek(
				ELEMENT_GAIN,
				list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_GAIN );
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
	if ( ! ( element =
			element_seek(
				ELEMENT_LOSS,
				list ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 ELEMENT_LOSS );
		exit( 1 );
	}

	element_total =
		output_subclassification_list(
			html_table,
			element->subclassification_list,
			element->accumulate_debit );

	retained_earnings -= element_total;
	credit_sum += element_total;

	if ( drawing_account )
	{
		/* Drawing */
		/* ------- */
		if ( ! ( element =
				element_seek(
					ELEMENT_EQUITY,
					list ) ) )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek element_name = %s\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	ELEMENT_EQUITY );
			exit( 1 );
		}

		element_total =
			output_drawing_subclassification_list(
				html_table,
				element->subclassification_list,
				drawing_account );

		retained_earnings -= element_total;
		credit_sum += element_total;
	}

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

		credit_sum += retained_earnings;
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

		debit_sum += -retained_earnings;
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
}

double output_drawing_subclassification_list(
			HTML_TABLE *html_table,
			LIST *subclassification_list,
			char *drawing_account )
{
	double balance_total = 0.0;
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

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( strcmp(	account->account_name,
					drawing_account ) != 0 )
			{
				continue;
			}

			if ( !account->latest_journal
			||   !account->latest_journal->balance )
				continue;

			html_table_set_data(
				html_table->data_list,
				strdup( format_initial_capital(
					buffer,
					account->account_name ) ) );

			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			html_table_set_data(
				html_table->data_list,
				strdup( place_commas_in_money(
					/* ------------------------------ */
					/* Drawing has a negative balance */
					/* ------------------------------ */
					-account->
						latest_journal->
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

			balance_total +=
				account->latest_journal->balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	/* ------------------------------ */
	/* Drawing has a negative balance */
	/* ------------------------------ */
	return -balance_total;
}

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
			SUBCLASSIFICATION_NET_ASSETS ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			SUBCLASSIFICATION_CONTRIBUTED_CAPITAL ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			SUBCLASSIFICATION_RETAINED_EARNINGS ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			SUBCLASSIFICATION_DRAWING ) == 0 )
		{
			continue;
		}

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_journal
			||   !account->latest_journal->balance )
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
						latest_journal->
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
				account->latest_journal->balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return retained_earnings;
}

double insert_journal(
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
			SUBCLASSIFICATION_NET_ASSETS ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			SUBCLASSIFICATION_CONTRIBUTED_CAPITAL ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			SUBCLASSIFICATION_RETAINED_EARNINGS ) == 0
		||   strcmp(
			subclassification->subclassification_name,
			SUBCLASSIFICATION_DRAWING ) == 0 )
		{
			continue;
		}

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( !account->latest_journal
			||   !account->latest_journal->balance )
				continue;

			if ( accumulate_debit )
			{
				fprintf( output_pipe,
					 "%s^%s^%s^%s^^%.2lf\n",
					 full_name,
					 street_address,
					 transaction_date_time_string,
					 account->account_name,
					 account->latest_journal->balance );
			}
			else
			{
				fprintf( output_pipe,
					 "%s^%s^%s^%s^%.2lf^\n",
					 full_name,
					 street_address,
					 transaction_date_time_string,
					 account->account_name,
					 account->latest_journal->balance );
			}

			retained_earnings +=
				account->latest_journal->balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return retained_earnings;
}

double insert_drawing(
			FILE *output_pipe,
			LIST *subclassification_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time_string,
			char *drawing_account )
{
	double balance_total = 0.0;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification =
			list_get_pointer(
				subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		do {
			account =
				list_get_pointer(
					subclassification->account_list );

			if ( strcmp(	account->account_name,
					drawing_account ) != 0 )
			{
				continue;
			}

			if ( !account->latest_journal
			||   !account->latest_journal->balance )
				continue;

			fprintf( output_pipe,
				 "%s^%s^%s^%s^^%.2lf\n",
				 full_name,
				 street_address,
				 transaction_date_time_string,
				 account->account_name,
				/* ------------------------------ */
				/* Drawing has a negative balance */
				/* ------------------------------ */
				 0.0 - account->latest_journal->balance );

			balance_total +=
				account->latest_journal->balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	/* ------------------------------ */
	/* Drawing has a negative balance */
	/* ------------------------------ */
	return 0.0 - balance_total;
}

