/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/investment_transaction_driver.c	*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "account.h"
#include "investment_transaction.h"
#include "account_delta.h"

#define IRS_RETIREMENT_PERCENT		0.08
#define LIABILITY_MEMO			"Liability mark to market"
#define TAXABLE_INVESTMENT_PURPOSE	"taxable_retirement"
#define IRA_TAX_PAYABLE_KEY		"IRA_tax_payable"
#define EQUITY_KEY			"equity_key"
#define LIABILITY_TRANSACTION_NO_CHANGE	"<p>No change in liability account."

void output_investment_transaction(
		double investment_account_sum,
		boolean execute_boolean );

void output_liability_transaction(
		double retirement_account_sum,
		boolean execute_boolean );

int main( int argc, char **argv )
{
	char *application_name;
	double investment_account_sum;
	double retirement_account_sum;
	boolean execute_boolean;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
	"Usage: %s investment_account_sum retirement_account_sum execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	investment_account_sum = atof( argv[ 1 ] );
	retirement_account_sum = atof( argv[ 2 ] );
	execute_boolean = (*argv[ 3 ] == 'y');

	output_investment_transaction(
		investment_account_sum,
		execute_boolean );

	output_liability_transaction(
		retirement_account_sum,
		execute_boolean );

	return 0;
}

void output_investment_transaction(
		double investment_account_sum,
		boolean execute_boolean )
{
	INVESTMENT_TRANSACTION *investment_transaction;

	investment_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		investment_transaction_new(
			(char *)0 /* fund_name */,
			investment_account_sum );

	if ( !investment_transaction->transaction )
	{
		printf(
			"%s\n",
			INVESTMENT_TRANSACTION_NO_CHANGE );
	
		return;
	}

	if ( execute_boolean )
	{
		investment_transaction->
			transaction->
			transaction_date_time =
				/* -------------------------------------- */
				/* Returns inserted transaction_date_time */
				/* -------------------------------------- */
				transaction_insert(
					investment_transaction->
						transaction->
						fund_name,
					investment_transaction->
						transaction->
						full_name,
					investment_transaction->
						transaction->
						street_address,
					investment_transaction->
						transaction->
						transaction_date_time,
					investment_transaction->
						transaction->
						transaction_amount,
					0 /* check_number */,
					investment_transaction->
						transaction->
						memo,
					investment_transaction->
						transaction->
						journal_list,
					1 /* insert_journal_list_boolean */ );
	}

	transaction_html_display( investment_transaction->transaction );
}

void output_liability_transaction(
		double retirement_account_sum,
		boolean execute_boolean )
{
	ACCOUNT *equity_account;
	ACCOUNT *liability_account;
	ACCOUNT_DELTA *account_delta;
	double liability_ending_balance;

	/* Debit account */
	/* ------------- */
	equity_account =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_key_fetch(
			EQUITY_KEY /* hard_coded_account_key */,
			__FUNCTION__ /* calling_function_name */ );

	/* Credit account */
	/* -------------- */
	liability_account =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_key_fetch(
			IRA_TAX_PAYABLE_KEY /* hard_coded_account_key */,
			__FUNCTION__ /* calling_function_name */ );

	liability_ending_balance =
		retirement_account_sum *
		IRS_RETIREMENT_PERCENT;

	account_delta =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_delta_new(
			(char *)0 /* fund_name */,
			equity_account->account_name
				/* debit_account_name */,
			liability_account->account_name
				/* credit_account_name */,
			0 /* liability not accumulate_debit_boolean */,
			liability_ending_balance,
			LIABILITY_MEMO );

	if ( !account_delta->transaction_binary )
	{
		printf(
			"%s\n",
			LIABILITY_TRANSACTION_NO_CHANGE );
	
		return;
	}

	if ( execute_boolean )
	{
		account_delta->
			transaction_binary->
			transaction_date_time =
				/* -------------------------------------- */
				/* Returns inserted transaction_date_time */
				/* -------------------------------------- */
				transaction_insert(
					account_delta->
						transaction_binary->
						fund_name,
					account_delta->
						transaction_binary->
						full_name,
					account_delta->
						transaction_binary->
						street_address,
					account_delta->
						transaction_binary->
						transaction_date_time,
					account_delta->
						transaction_binary->
						transaction_amount,
					0 /* check_number */,
					account_delta->
						transaction_binary->
						memo,
					account_delta->
						transaction_binary->
						journal_list,
					1 /* insert_journal_list_boolean */ );
	}

	transaction_html_display( account_delta->transaction_binary );
}

