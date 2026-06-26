/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account_balance.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "sql.h"
#include "float.h"
#include "entity.h"
#include "spool.h"
#include "investment_account.h"
#include "account_balance.h"

ACCOUNT_BALANCE *account_balance_new(
		char *full_name,
		char *street_address,
		char *account_number,
		char *date_string )
{
	ACCOUNT_BALANCE *account_balance;

	if ( !full_name
	||   !street_address
	||   !account_number
	||   !date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	account_balance = account_balance_calloc();

	account_balance->full_name = full_name;
	account_balance->street_address = street_address;
	account_balance->account_number = account_number;
	account_balance->date_string = date_string;

	return account_balance;
}

ACCOUNT_BALANCE *account_balance_calloc( void )
{
	ACCOUNT_BALANCE *account_balance;

	if ( ! ( account_balance =
			calloc( 1, sizeof ( ACCOUNT_BALANCE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return account_balance;
}

ACCOUNT_BALANCE *account_balance_parse(
		char *full_name,
		char *street_address,
		char *account_number,
		char *input )
{
	ACCOUNT_BALANCE *account_balance;
	char date_string[ 128 ];
	char balance[ 128 ];
	char balance_change[ 128 ];
	char balance_change_percent[ 128 ];

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

	if ( !input || !*input ) return NULL;

	/* See ACCOUNT_BALANCE_SELECT */
	/* -------------------------- */
	piece( date_string, SQL_DELIMITER, input, 0 );

	account_balance =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_balance_new(
			full_name,
			street_address,
			account_number,
			strdup( date_string ) );

	piece( balance, SQL_DELIMITER, input, 1 );
	account_balance->balance = atof( balance );

	piece( balance_change, SQL_DELIMITER, input, 2 );
	account_balance->balance_change =
		atof( balance_change );

	piece( balance_change_percent, SQL_DELIMITER, input, 3 );
	account_balance->balance_change_percent =
		atoi( balance_change_percent );

	return account_balance;
}

char *account_balance_update_change_string(
		char *full_name,
		char *street_address,
		char *account_number,
		char *date_string,
		double balance_change )
{
	static char string[ 256 ];

	if ( !full_name
	||   !street_address
	||   !account_number
	||   !date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(string,
	 	"%s^%s^%s^%s^balance_change^%.2lf\n",
		full_name,
		street_address,
		account_number,
		date_string,
		balance_change );

	return string;
}

char *account_balance_update_percent_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date_string,
		int update_balance_change_percent,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean )
{
	char string[ 1024 ];

	if ( !full_name
	||   !street_address
	||   !account_number
	||   !date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(string,
	 	"%s^%s^%s^%s^balance_change_percent^%d\n",
		full_name,
		street_address,
		account_number,
		date_string,
		balance_change_percent );

	return strdup( string );
}

LIST *account_balance_list(
		char *full_name,
		char *street_address,
		char *account_number,
		char *investment_account_primary_where )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *list;
	char *system_string;

	if ( !full_name
	||   !street_address
	||   !account_number
	||   !investment_account_primary_where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_system_string(
			ACCOUNT_BALANCE_SELECT,
			ACCOUNT_BALANCE_TABLE,
			investment_account_primary_where );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list, 
			account_balance_parse(
				full_name,
				street_address,
				account_number,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

char *account_balance_system_string(
		const char *select,
		const char *account_balance_table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !where )
	{
		char message[ 128 ];

		sprintf(message, "where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" date",
		select,
		account_balance_table,
		where ); 

	return strdup( system_string );
}

LIST *account_balance_update_set( LIST *account_balance_list )
{
	ACCOUNT_BALANCE *account_balance;
	ACCOUNT_BALANCE *account_balance_prior = {0};
	double change;
	int change_percent;

	if ( list_rewind( account_balance_list ) )
	do {
		account_balance = list_get( account_balance_list );

		if ( list_first_boolean( account_balance_list ) )
		{
			account_balance->
				update_balance_change =
					account_balance->
						balance;

			account_balance->
				update_balance_change_percent = 100;
		}
		else
		{
			change =
				account_balance_update_change(
					account_balance_prior->balance,
					account_balance->balance );

			account_balance->
				update_balance_change =
					change;

			change_percent =
				account_balance_update_change_percent(
					account_balance_prior->balance,
					change );

			account_balance->
				update_balance_change_percent =
					change_percent;
		}

		account_balance_prior = account_balance;

	} while ( list_next( account_balance_list ) );

	return account_balance_list;
}

double account_balance_update_change(
		double prior_balance,
		double balance )
{
	return balance - prior_balance;
}

int account_balance_update_change_percent(
		double prior_balance,
		double balance_change )
{
	int change_percent;
	double change_ratio;

	if ( !prior_balance && !balance_change )
		return 0;
	else
	if ( !prior_balance && balance_change < 0.0 )
		return -100;
	else
	if ( !prior_balance && balance_change > 0.0 )
		return 100;

	change_ratio = balance_change / prior_balance;

	change_percent =
		float_round_int(
			change_ratio * 100.0 );

	if ( change_percent == -100
	&&   !float_virtually_same(
			prior_balance,
			-balance_change ) )
	{
		change_percent = -99;
	}

	return change_percent;
}

char *account_balance_list_update( LIST *account_balance_update_set )
{
	char *system_string;
	SPOOL *spool;
	ACCOUNT_BALANCE *account_balance;
	char *string;
	LIST *list;
	char *error_string;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_update_system_string(
			ACCOUNT_BALANCE_TABLE,
			ACCOUNT_BALANCE_PRIMARY_KEY );

	spool =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		spool_new(
			system_string,
			0 /* not capture_stderr_boolean */ );

	if ( list_rewind( account_balance_update_set ) )
	do {
		account_balance = list_get( account_balance_update_set );

		string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			account_balance_update_change_string(
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_string,
				account_balance->
					update_balance_change );

		fprintf(spool->output_pipe,
			"%s\n",
			string );

		string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			account_balance_update_percent_string(
				account_balance->full_name,
				account_balance->street_address,
				account_balance->account_number,
				account_balance->date_string,
				account_balance->
					update_balance_change_percent );

		fprintf(spool->output_pipe,
			"%s\n",
			string );
	} while ( list_next( account_balance_update_set ) );

	pclose( spool->output_pipe );
	list = spool_list( spool->output_filename );

	error_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_string_delimited(
			list, "<br>" );

	if ( *error_string )
		return error_string;
	else
		return (char *)0;
}

char *account_balance_update_system_string(
		const char *account_balance_table,
		const char *account_balance_primary_key,
		const char *predictive_fund_column,
		const char *entity_contact_key_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean )
{
	char *key_string;
	char system_string[ 1024 ];

	key_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_key_string(
			account_balance_primary_key,
			predictive_fund_column,
			entity_contact_key_column,
			predictive_fund_boolean,
			entity_contact_key_boolean );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y |"
		"sql.e 2>&1",
		account_balance_table,
		key_string );

	free( key_string );

	return strdup( system_string );
}

char *account_balance_key_string(
		const char *account_balance_primary_key,
		const char *predictive_fund_column,
		const char *entity_contact_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			account_balance_primary_key /* base */,
			predictive_fund_column /* component */,
			0 /* not escape_boolean */,
			fund_boolean /* set_boolean */ );

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base */,
			entity_contact_key_column /* component */,
			0 /* not escape_boolean */,
			contact_key_boolean /* set_boolean */ );

	return optional_column->return_string /* heap memory */;
}

void account_balance_update_spool(
		char *update_change_string,
		char *update_percent_string,
		SPOOL *spool )
{
	if ( !update_change_string
	||   !update_percent_string
	||   !spool )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(
		spool->output_pipe,
		"%s\n",
		update_change_string );

	fprintf(
		spool->output_pipe,
		"%s\n",
		update_percent_stringl );
}

void account_balance_update(
		char *fund_name,
		boolean fund_boolean,
		boolean contact_key_boolean,
		ACCOUNT_BALANCE *account_balance_fetch,
		ACCOUNT_BALANCE *account_balance_next )
{
	char *update_system_string;
	SPOOL *spool;
	LIST *list;
	char *error_string;

	if ( !account_balance_fetch ) return;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_update_system_string(
			account_balance_table,
			account_balance_primary_key,
			PREDICTIVE_FUND_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	spool =
		/* -------------- */
		/* Safely returns */
		/* -------------- */

		/* -------------------------------------------- */
		/* The output is in:				*/
		/* spool_list( spool_new()->output_filename )   */
		/* -------------------------------------------- */
		spool_new(
			update_system_string,
			0 /* not capture_stderr_boolean */ );

	account_balance_update_spool(
		account_balance_fetch->update_change_string,
		account_balance_fetch->update_percent_string,
		spool );

	if ( account_balance_next )
	{
		account_balance_update_spool(
			account_balance_next->update_change_string,
			account_balance_next->update_percent_string,
			spool );
	}

	pclose( spool->output_pipe );
	list = spool_list( spool->output_filename );
	error_string = list_string_delimited( list, "<br>" );
	if ( error_string ) printf( "%s\n", error_string );
}

char *account_balance_fetch_date(
		const char *account_balance_table,
		const *account_balance_min_function,
		const *account_balance_max_function,
		char *account_balance_where,
		boolean fetch_min_boolean )
{
	char *system_string;
	char *date;

	if ( !account_balance_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_balance_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(fetch_min_boolean)
				? account_balance_min_function
				: account_balance_max_function,
			account_balance_table,
			account_balance_where );

	date = string_system_input( system_string );
	free( system_string );

	return date;
}

char *account_balance_where(
		char *fund_name,
		char *as_of_date,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number,
		const char *relational_operator )
{
	char *account_where;
	char where[ 1024 ];

	if ( !as_of_date
	||   !full_name
	||   !account_number )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	account_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		investment_account_where(
			fund_name,
			full_name,
			contact_key,
			investment_purpose,
			predictive_fund_boolean,
			entity_contact_key_boolean );

	snprintf(
		where,
		sizeof ( where ),
		"%s and date %s '%s'",
		account_where,
		relational_operator,
		as_of_date );

	return strdup( where );
}


