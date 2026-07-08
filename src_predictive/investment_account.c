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
#include "entity.h"
#include "predictive.h"
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
		char *fund_name,
		char *as_of_date,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *input )
{
	char full_name[ 128 ];
	char account_number[ 128 ];
	char buffer[ 128 ];
	char *contact_key = {0};
	INVESTMENT_ACCOUNT *investment_account;

	if ( !input || !*input ) return NULL;

	/* See entity_select_string() */
	/* -------------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( account_number, SQL_DELIMITER, input, 1 );

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 8 );
		contact_key = strdup( buffer );
	}

	investment_account =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		investment_account_new(
			fund_name,
			strdup( full_name ),
			contact_key,
			strdup( account_number ) );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		investment_account->investment_classification =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		investment_account->investment_purpose =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		investment_account->certificate_maturity_months =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
		investment_account->certificate_maturity_date =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer )
		investment_account->interest_rate =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer )
		investment_account->balance_latest =
			atof( buffer );

	investment_account->account_balance_latest =
		account_balance_latest(
			fund_name,
			as_of_date,
			fund_boolean,
			contact_key_boolean,
			full_name,
			contact_key,
			account_number );

	if ( !investment_account->account_balance_latest ) return NULL;

	investment_account->primary_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		investment_account_primary_where(
			fund_name,
			full_name,
			contact_key,
			account_number,
			fund_boolean,
			contact_key_boolean );

	return investment_account;
}

LIST *investment_account_list(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *as_of_date,
		char *investment_purpose,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *select_string;
	char *where;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	INVESTMENT_ACCOUNT *investment_account;
	LIST *list = list_new();

	if ( !as_of_date )
	{
		char message[ 128 ];

		sprintf(message, "as_of_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_select_string(
			INVESTMENT_ACCOUNT_SELECT /* ENTITY_SELECT */,
			ENTITY_CONTACT_KEY_COLUMN,
			contact_key_boolean );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		investment_account_where(
			fund_name,
			full_name,
			contact_key,
			investment_purpose,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select_string,
			INVESTMENT_ACCOUNT_TABLE,
			where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	while( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		investment_account =
			investment_account_parse(
				fund_name,
				as_of_date,
				fund_boolean,
				contact_key_boolean,
				input );

		list_set( list, investment_account );
	}

	pclose ( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

INVESTMENT_ACCOUNT *investment_account_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number )
{
	INVESTMENT_ACCOUNT *investment_account;

	if ( !full_name
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

	investment_account = investment_account_calloc();

	investment_account->fund_name = fund_name;
	investment_account->full_name = full_name;
	investment_account->contact_key = contact_key;
	investment_account->account_number = account_number;

	return investment_account;
}

char *investment_account_update(
		const char *investment_account_table,
		char *primary_where,
		double balance )
{
	char *update_statement;
	SPOOL *spool;
	LIST *list;
	char *error_string;

	if  ( !primary_where )
	{
		char message[ 128 ];

		sprintf(message, "primary_where is empty." );

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
			primary_where,
			balance );

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
		double balance )
{
	char statement[ 1024 ];

	snprintf(
		statement,
		sizeof ( statement ),
		"update %s set balance_latest = %.2lf where %s;",
		investment_account_table,
		balance,
		investment_account_primary_where );

	return strdup( statement );
}

double investment_account_liability_sum(
		const char *investment_purpose_taxable_liability,
		LIST *investment_account_list )
{
	INVESTMENT_ACCOUNT *investment_account;
	double liability_sum = {0};

	if ( list_rewind( investment_account_list ) )
	do {
		investment_account = list_get( investment_account_list );

		if ( !investment_account->account_balance_latest )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
		"investment_account->account_balanced_latest is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( string_strcmp(
			investment_account->investment_purpose,
			(char *)investment_purpose_taxable_liability ) == 0 )
		{
			liability_sum +=
				investment_account->
					account_balance_latest->
					balance;
		}

	} while ( list_next( investment_account_list ) );

	return liability_sum;
}

double investment_account_asset_sum( LIST *investment_account_list )
{
	INVESTMENT_ACCOUNT *investment_account;
	double asset_sum = {0};

	if ( list_rewind( investment_account_list ) )
	do {
		investment_account = list_get( investment_account_list );

		if ( !investment_account->account_balance_latest )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
		"investment_account->account_balanced_latest is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		asset_sum +=
			investment_account->
				account_balance_latest->
				balance;

	} while ( list_next( investment_account_list ) );

	return asset_sum;
}

char *investment_account_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *investment_purpose,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *fund_where;
	char *primary_where;
	char *purpose_where;
	char where[ 1024 ];

	fund_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		predictive_fund_where(
			PREDICTIVE_FUND_COLUMN,
			fund_name,
			fund_boolean );

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			full_name,
			contact_key,
			contact_key_boolean );

	purpose_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		investment_account_purpose_where(
			investment_purpose );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s and %s",
		fund_where,
		primary_where,
		purpose_where );

	return strdup( where );
}

char *investment_account_purpose_where( char *investment_purpose )
{
	static char where[ 128 ];

	if ( investment_account_purpose_populated_boolean(
		investment_purpose ) )
	{
		snprintf(
			where,
			sizeof ( where ),
			"investment_purpose = '%s'",
			investment_purpose );
	}
	else
	{
		strcpy( where, "1 = 1" );
	}

	return where;
}

boolean investment_account_purpose_populated_boolean( char *investment_purpose )
{
	boolean populated_boolean = 0;

	if ( investment_purpose
	&&   *investment_purpose
	&&   strcmp( investment_purpose, "investment_purpose" ) != 0 )
	{
		populated_boolean = 1;
	}

	return populated_boolean;
}

char *investment_account_primary_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *fund_where;
	char *primary_where;
	char where[ 1024 ];

	if ( !account_number )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_number is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fund_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		predictive_fund_where(
			PREDICTIVE_FUND_COLUMN,
			fund_name,
			fund_boolean );

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			full_name,
			contact_key,
			contact_key_boolean );
	snprintf(
		where,
		sizeof ( where ),
		"%s and %s and account_number = '%s'",
		fund_where,
		primary_where,
		account_number );

	return strdup( where );
}

void investment_account_list_html_output(
		const char sql_delimiter,
		const char *investment_account_heading,
		const char *investment_account_justification,
		LIST *investment_account_list,
		double asset_sum )
{
	char *system_string;
	FILE *output_pipe;
	INVESTMENT_ACCOUNT *investment_account;
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		investment_account_system_string(
			sql_delimiter,
			investment_account_heading,
			investment_account_justification );

	/* Safely returns */
	/* -------------- */
	output_pipe = appaserver_output_pipe( system_string );

	if ( list_rewind( investment_account_list ) )
	do {
		investment_account = list_get( investment_account_list );

		if ( !investment_account->account_balance_latest )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
		"investment_account->account_balance_latest is empty." );

			pclose( output_pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		fprintf(
			output_pipe,
			"%s^%s^%s^%s^%s^%s\n",
			string_initial_capital(
				buffer1,
				investment_account->full_name ),
			investment_account->account_number,
			investment_account->account_balance_latest->date_string,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_dollar(
				investment_account->
					account_balance_latest->
					balance ),
			(investment_account->certificate_maturity_date)
				? investment_account->certificate_maturity_date
				: "",
			string_initial_capital(
				buffer2,
				investment_account->investment_purpose ) );

	} while ( list_next( investment_account_list ) );

	fprintf(output_pipe,
		"Sum^^^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_dollar( asset_sum ) );

	pclose( output_pipe );
}

char *investment_account_system_string(
		const char sql_delimiter,
		const char *heading,
		const char *justification )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"html_table.e '' '%s' '%c' '%s'",
		heading,
		sql_delimiter,
		justification );

	return strdup( system_string );
}

