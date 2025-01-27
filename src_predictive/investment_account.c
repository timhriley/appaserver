/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment_account.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "spool.h"
#include "account_balance.h"
#include "investment_account.h"

INVESTMENT_ACCOUNT *investment_account_calloc( void )
{
	INVESTMENT_ACCOUNT *investment_account;

	if ( ! ( investment_account =
			calloc( 1, sizeof ( INVESTMENT_ACCOUNT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return investment_account;
}

INVESTMENT_ACCOUNT *investment_account_parse(
		char *full_name,
		char *street_address,
		char *account_number,
		char *primary_where,
		char *input )
{
	char buffer[ 128 ];
	INVESTMENT_ACCOUNT *investment_account;

	if ( !input || !*input ) return (INVESTMENT_ACCOUNT *)0;

	investment_account =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		investment_account_new(
			full_name,
			street_address,
			account_number,
			primary_where );

	/* See INVESTMENT_ACCOUNT_SELECT */
	/* ----------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer )
		investment_account->investment_classification =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer )
		investment_account->investment_purpose =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		investment_account->certificate_maturity_months =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		investment_account->certificate_maturity_date =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		investment_account->interest_rate =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
		investment_account->balance_latest =
			atof( buffer );

	return investment_account;
}

char *investment_account_primary_where(
		char *full_name,
		char *street_address,
		char *account_number )
{
	static char where[ 128 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "account_number = '%s'		",
		 full_name,
		 street_address,
		 account_number );

	return where;
}

INVESTMENT_ACCOUNT *investment_account_fetch(
		char *full_name,
		char *street_address,
		char *account_number,
		boolean fetch_account_balance_list )
{
	char *primary_where;
	char *system_string;
	char *input;
	INVESTMENT_ACCOUNT *investment_account;

	if ( !full_name
	||   !street_address
	||   !account_number )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		investment_account_primary_where(
			full_name,
			street_address,
			account_number );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			INVESTMENT_ACCOUNT_SELECT,
			INVESTMENT_ACCOUNT_TABLE,
			primary_where );

	if ( ! ( input =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_input(
				system_string ) ) )
	{
		return NULL;
	}

	investment_account =
		investment_account_parse(
			full_name,
			street_address,
			account_number,
			primary_where,
			input );

	if ( !investment_account )
	{
		char message[ 128 ];

		sprintf(message,
			"investment_account_parse(%s,%s,%s) returned empty.",
			full_name,
			street_address,
			account_number );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( fetch_account_balance_list )
	{
		investment_account->account_balance_list =
			account_balance_list(
				full_name,
				street_address,
				account_number,
				primary_where );

		investment_account->update_balance_latest =
			investment_account_update_balance_latest(
				account_balance_update_set(
				investment_account->
					account_balance_list /* in/out */ ) );
	}

	return investment_account;
}

INVESTMENT_ACCOUNT *investment_account_new(
		char *full_name,
		char *street_address,
		char *account_number,
		char *primary_where )
{
	INVESTMENT_ACCOUNT *investment_account;

	if ( !full_name
	||   !street_address
	||   !account_number
	||   !primary_where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	investment_account = investment_account_calloc();

	investment_account->full_name = full_name;
	investment_account->street_address = street_address;
	investment_account->account_number = account_number;
	investment_account->primary_where = primary_where;

	return investment_account;
}

char *investment_account_update(
		const char *investment_account_table,
		char *where,
		double update_balance_latest )
{
	char *update_statement;
	SPOOL *spool;
	LIST *list;
	char *error_string;

	if  ( !where )
	{
		char message[ 128 ];

		sprintf(message, "where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		investment_account_update_statement(
			investment_account_table,
			where,
			update_balance_latest );


	/* Safely returns */
	/* -------------- */
	spool =
		spool_new(
			"sql.e 2>&1",
			0 /* not capture_stderr_boolean */ );

	fprintf(spool->output_pipe,
		"%s\n",
		update_statement );

	pclose( spool->output_pipe );
	list = spool_list( spool->output_filename );
	error_string = list_first( list );

	return error_string;
}

char *investment_account_update_statement(
		const char *investment_account_table,
		char *investment_account_primary_where,
		double update_balance_latest )
{
	char statement[ 1024 ];

	sprintf(statement,
		"update %s set balance_latest = %.2lf where %s;",
		investment_account_table,
		update_balance_latest,
		investment_account_primary_where );

	return strdup( statement );
}

double investment_account_update_balance_latest(
		LIST *account_balance_update_set )
{
	ACCOUNT_BALANCE *account_balance;

	if ( ! ( account_balance =
			list_last(
				account_balance_update_set ) ) )
	{
		char message[ 128 ];

		sprintf(message, "account_balance_update_set is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return account_balance->balance;
}

