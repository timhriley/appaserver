/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment_transaction.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "date.h"
#include "float.h"
#include "entity.h"
#include "predictive.h"
#include "journal.h"
#include "investment.h"
#include "investment_account.h"
#include "investment_transaction.h"

void investment_transaction_output(
		const char *memo,
		boolean execute_boolean,
		INVESTMENT_TRANSACTION *investment_transaction )
{
	if ( !investment_transaction )
	{
		printf( "<h3>%s</h3>\n%s\n",
			memo,
			INVESTMENT_TRANSACTION_NO_CHANGE );

		return;
	}

	if ( !investment_transaction->transaction_binary )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
		"investment_transaction->transaction_binary is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( execute_boolean )
	{
		investment_transaction->
			transaction_binary->
			transaction_date_time =
				/* -------------------------------------- */
				/* Returns inserted transaction_date_time */
				/* -------------------------------------- */
				transaction_insert(
					investment_transaction->
						transaction_binary->
						fund_name,
					investment_transaction->
						transaction_binary->
						full_name,
					investment_transaction->
						transaction_binary->
						contact_key,
					investment_transaction->
						transaction_binary->
						transaction_date_time,
					investment_transaction->
						transaction_binary->
						transaction_amount,
					0 /* check_number */,
					investment_transaction->
						transaction_binary->
						memo,
					investment_transaction->
						transaction_binary->
						journal_list,
					investment_transaction->
						predictive_fund_boolean,
					investment_transaction->
						entity_contact_key_boolean,
					1 /* insert_journal_list_boolean */ );
	}

	printf( "<h3>%s</h3>",
		investment_transaction->
			transaction_binary->
			memo );

	transaction_html_display( investment_transaction->transaction_binary );

	if ( execute_boolean )
	{
		printf( "<h3>Transaction inserted.</h3>\n" );
	}
}

INVESTMENT_TRANSACTION *investment_transaction_new(
		const char *memo,
		char *fund_name,
		double account_sum,
		char *debit_account_name,
		char *credit_account_name,
		char *negative_account_name,
		boolean accumulate_debit_boolean )
{
	INVESTMENT_TRANSACTION *investment_transaction;

	investment_transaction = investment_transaction_calloc();

	investment_transaction->fetch_account_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		investment_transaction_fetch_account_name(
			debit_account_name,
			credit_account_name,
			accumulate_debit_boolean );

	investment_transaction->date_now19 =
		date_now19( date_utc_offset() );

	investment_transaction->predictive_fund_boolean =
		predictive_fund_boolean(
			PREDICTIVE_FUND_TABLE,
			PREDICTIVE_FUND_COLUMN );

	investment_transaction->entity_contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	investment_transaction->account_journal_latest =
		account_journal_latest(
			JOURNAL_TABLE,
			fund_name,
			investment_transaction->fetch_account_name,
			investment_transaction->date_now19
				/* end_date_time_string */,
			investment_transaction->predictive_fund_boolean,
			investment_transaction->entity_contact_key_boolean,
			0 /* not fetch_transaction */,
			0 /* not latest_zero_balance_boolean */ );

	investment_transaction->balance =
		investment_transaction_balance(
			investment_transaction->account_journal_latest );

	investment_transaction->delta =
		investment_transaction_delta(
			account_sum,
			investment_transaction->balance );

	if ( float_money_virtually_same(
		investment_transaction->delta,
		0.0 ) )
	{
		return NULL;
	}

	investment_transaction->entity_self_fetch =
		/* ------------------------------------- */
		/* Returns heap memory of static pointer */
		/* ------------------------------------- */
		entity_self_fetch(
			investment_transaction->entity_contact_key_boolean,
			0 /* not fetch_entity_boolean */ );

	if ( investment_transaction->delta > 0.0 )
	{
		investment_transaction->transaction_binary =
			transaction_binary(
				fund_name,
				investment_transaction->
					entity_self_fetch->
					full_name,
				investment_transaction->
					entity_self_fetch->
					contact_key,
				investment_transaction->date_now19
					/* transaction_date_time */,
				investment_transaction->delta
					/* transaction_amount */,
				(char *)memo,
				debit_account_name,
				credit_account_name );
	}
	else
	/* Must be < 0.0 */
	{
		investment_transaction->negative_debit_account =
			/* ------------------------ */
			/* Returns either parameter */
			/* ------------------------ */
			investment_transaction_negative_debit_account(
				credit_account_name,
				negative_account_name );

		investment_transaction->transaction_binary =
			transaction_binary(
				fund_name,
				investment_transaction->
					entity_self_fetch->
					full_name,
				investment_transaction->
					entity_self_fetch->
					contact_key,
				investment_transaction->date_now19
					/* transaction_date_time */,
				-investment_transaction->delta
					/* transaction_amount */,
				(char *)memo,
				investment_transaction->
					negative_debit_account,
				debit_account_name /* credit_account */ );
	}

	return investment_transaction;
}

INVESTMENT_TRANSACTION *investment_transaction_calloc( void )
{
	INVESTMENT_TRANSACTION *investment_transaction;

	if ( ! ( investment_transaction =
			calloc( 1,
				sizeof ( INVESTMENT_TRANSACTION ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return investment_transaction;
}

char *investment_transaction_fetch_account_name(
		char *debit_account_name,
		char *credit_account_name,
		boolean accumulate_debit_boolean )
{
	return
	(accumulate_debit_boolean)
		? debit_account_name
		: credit_account_name;
}

double investment_transaction_balance(
		ACCOUNT_JOURNAL *account_journal_latest )
{
	if ( !account_journal_latest )
		return 0.0;
	else
		return account_journal_latest->balance;
}

double investment_transaction_delta(
		double account_sum,
		double investment_transaction_balance )
{
	return account_sum - investment_transaction_balance;
}

char *investment_transaction_negative_debit_account(
		char *credit_account_name,
		char *negative_account_name )
{

	return
	(negative_account_name)
		? negative_account_name
		: credit_account_name;
}

boolean investment_transaction_boolean(
		char *full_name,
		char *as_of_date,
		char *investment_purpose )
{
	if ( entity_full_name_populated_boolean(
		ENTITY_FULL_NAME_COLUMN,
		full_name ) )
	{
		return 0;
	}

	if ( investment_account_purpose_populated_boolean(
		investment_purpose ) )
	{
		return 0;
	}

	if ( investment_transaction_date_populated_boolean(
		as_of_date ) )
	{
		return 0;
	}

	return 1;
}

boolean investment_transaction_date_populated_boolean( char *as_of_date )
{
	boolean populated_boolean = 0;

	if (	as_of_date
	&&	*as_of_date
	&&	strcmp(
			as_of_date,
			"as_of_date" ) != 0 )
	{
		populated_boolean = 1;
	}

	return populated_boolean;
}

INVESTMENT_TRANSACTION *investment_transaction_asset(
		const char *investment_account_key,
		const char *investment_gain_account_key,
		const char *investment_loss_account_key,
		const char *asset_memo,
		char *fund_name,
		double asset_sum )
{
	char *investment_account_name;
	char *gain_account_name;
	char *loss_account_name;

	investment_account_name =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_key_account_name(
			investment_account_key,
			__FUNCTION__ );

	gain_account_name =
		account_key_account_name(
			investment_gain_account_key,
			__FUNCTION__ );

	loss_account_name =
		account_key_account_name(
			investment_loss_account_key,
			__FUNCTION__ );

	return
	investment_transaction_new(
		asset_memo,
		fund_name,
		asset_sum,
		investment_account_name /* debit_account_name */,
		gain_account_name /* credit_account_name */,
		loss_account_name /* negative_account_name */,
		1 /* accumulate_debit_boolean */ );
}

INVESTMENT_TRANSACTION *investment_transaction_liability(
		const char *equity_key,
		const char *ira_tax_payable_key,
		const char *liability_memo,
		char *fund_name,
		double liability_amount )
{

	char *equity_account_name;
	char *tax_payable_account_name;

	equity_account_name =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_key_account_name(
			equity_key,
			__FUNCTION__ );

	tax_payable_account_name =
		account_key_account_name(
			ira_tax_payable_key,
			__FUNCTION__ );

	return
	investment_transaction_new(
		liability_memo,
		fund_name,
		liability_amount,
		equity_account_name /* debit_account_name */,
		tax_payable_account_name /* credit_account_name */,
		(char *)0 /* negative_account_name */,
		0 /* not accumulate_debit_boolean */ );
}

double investment_transaction_liability_amount(
		const double ira_tax_percent,
		double liability_sum )
{
	return ira_tax_percent * liability_sum;
}
