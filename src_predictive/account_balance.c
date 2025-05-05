/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account_balance.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
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
		char *full_name,
		char *street_address,
		char *account_number,
		char *date_string,
		int balance_change_percent )
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
	 	"%s^%s^%s^%s^balance_change_percent^%d\n",
		full_name,
		street_address,
		account_number,
		date_string,
		balance_change_percent );

	return string;
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
		const char *account_balance_primary_key )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"update_statement.e table=%s key=%s carrot=y |"
		"sql.e 2>&1",
		account_balance_table,
		account_balance_primary_key );

	return strdup( system_string );
}
