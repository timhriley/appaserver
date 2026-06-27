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
#include "predictive.h"
#include "optional_column.h"
#include "investment_account.h"
#include "account_balance.h"

ACCOUNT_BALANCE *account_balance_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date_string )
{
	ACCOUNT_BALANCE *account_balance;

	if ( !full_name
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

	account_balance->fund_name = fund_name;
	account_balance->full_name = full_name;
	account_balance->contact_key = contact_key;
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
		char *fund_name,
		boolean contact_key_boolean,
		char *input )
{
	ACCOUNT_BALANCE *account_balance;
	char full_name[ 128 ];
	char account_number[ 128 ];
	char date_string[ 128 ];
	char buffer[ 128 ];
	char *contact_key = {0};

	if ( !input || !*input ) return NULL;

	/* See entity_select_string() */
	/* -------------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( account_number, SQL_DELIMITER, input, 1 );
	piece( date_string, SQL_DELIMITER, input, 2 );

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 6 );
		contact_key = strdup( buffer );
	}

	account_balance =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_balance_new(
			fund_name,
			strdup( full_name ),
			contact_key,
			strdup( account_number ),
			strdup( date_string ) );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )account_balance->balance = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) account_balance->balance_change = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) account_balance->balance_change_percent = atoi( buffer );

	return account_balance;
}

char *account_balance_update_change_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date_string,
		double update_balance_change,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char string[ 1024 ];
	OPTIONAL_COLUMN *optional_column;
	char update_string[ 128 ];

	if ( !full_name
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

	snprintf(
		string,
		sizeof ( string ),
	 	"%s^%s^%s\n",
		full_name,
		account_number,
		date_string );

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			sql_delimiter,
			string /* base_string */,
			fund_name /* component */,
			0 /* not escape_boolean*/,
			fund_boolean /* set_boolean */ );

	optional_column =
		optional_column_new(
			sql_delimiter,
			optional_column->return_string /* base_string */,
			contact_key /* component */,
			0 /* not escape_boolean*/,
			contact_key_boolean /* set_boolean */ );

	snprintf(
		update_string,
		sizeof ( update_string ),
	 	"balance_change%c%.2lf",
		sql_delimiter,
		update_balance_change );

	optional_column =
		optional_column_new(
			sql_delimiter,
			optional_column->return_string /* base_string */,
			update_string /* component */,
			0 /* not escape_boolean*/,
			1 /* set_boolean */ );

	return optional_column->return_string;
}

char *account_balance_update_percent_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date_string,
		int update_balance_change_percent,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char string[ 1024 ];
	OPTIONAL_COLUMN *optional_column;
	char update_string[ 128 ];

	if ( !full_name
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

	snprintf(
		string,
		sizeof ( string ),
	 	"%s^%s^%s\n",
		full_name,
		account_number,
		date_string );

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			sql_delimiter,
			string /* base_string */,
			fund_name /* component */,
			0 /* not escape_boolean*/,
			fund_boolean /* set_boolean */ );

	optional_column =
		optional_column_new(
			sql_delimiter,
			optional_column->return_string /* base_string */,
			contact_key /* component */,
			0 /* not escape_boolean*/,
			contact_key_boolean /* set_boolean */ );

	snprintf(
		update_string,
		sizeof ( update_string ),
	 	"balance_change_percent%c%d",
		sql_delimiter,
		update_balance_change_percent );

	optional_column =
		optional_column_new(
			sql_delimiter,
			optional_column->return_string /* base_string */,
			update_string /* component */,
			0 /* not escape_boolean*/,
			1 /* set_boolean */ );

	return optional_column->return_string;
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

char *account_balance_update_system_string(
		const char *account_balance_table,
		const char *account_balance_primary_key,
		const char *predictive_fund_column,
		const char *entity_contact_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
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
			fund_boolean,
			contact_key_boolean );

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
			(char *)account_balance_primary_key /* base */,
			(char *)predictive_fund_column /* component */,
			0 /* not escape_boolean */,
			fund_boolean /* set_boolean */ );

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base */,
			(char *)entity_contact_key_column /* component */,
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
		update_percent_string );
}

void account_balance_update(
		boolean fund_boolean,
		boolean contact_key_boolean,
		ACCOUNT_BALANCE *account_balance_fetch,
		ACCOUNT_BALANCE *account_balance_next )
{
	char *system_string;
	SPOOL *spool;
	LIST *list;
	char *error_string;

	if ( !account_balance_fetch ) return;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_update_system_string(
			ACCOUNT_BALANCE_TABLE,
			ACCOUNT_BALANCE_PRIMARY_KEY,
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
			system_string,
			0 /* not capture_stderr_boolean */ );

	free( system_string );

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
		const char *account_balance_min_function,
		const char *account_balance_max_function,
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
				? (char *)account_balance_min_function
				: (char *)account_balance_max_function,
			(char *)account_balance_table,
			account_balance_where );

	/* Returns heap memory or null */
	/* --------------------------- */
	date = string_system_input( system_string );

	free( system_string );

	return date;
}

char *account_balance_where(
		char *fund_name,
		char *as_of_date,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number,
		const char *relational_operator )
{
	char *primary_where;
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

	primary_where =
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

	snprintf(
		where,
		sizeof ( where ),
		"%s and date %s '%s'",
		primary_where,
		relational_operator,
		as_of_date );

	return strdup( where );
}

void account_balance_delta_set(
		char *fund_name,
		boolean fund_boolean,
		boolean contact_key_boolean,
		ACCOUNT_BALANCE *account_balance_prior,
		ACCOUNT_BALANCE *account_balance_current /* in/out */ )
{
	if ( account_balance_current )
	{
		if ( !account_balance_prior )
		{
			account_balance_current->
				update_balance_change =
					account_balance_current->balance;

			account_balance_current->
				update_balance_change_percent = 100;
		}
		else
		{
			double update_change;
			int change_percent;

			update_change =
				account_balance_update_change(
					account_balance_prior->balance,
					account_balance_current->balance );

			account_balance_current->
				update_balance_change =
					update_change;

			change_percent =
				account_balance_update_change_percent(
					account_balance_prior->balance,
					update_change );

			account_balance_current->
				update_balance_change_percent =
					change_percent;
		}

		account_balance_current->update_change_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			account_balance_update_change_string(
				SQL_DELIMITER,
				fund_name,
				account_balance_current->full_name,
				account_balance_current->contact_key,
				account_balance_current->account_number,
				account_balance_current->date_string,
				account_balance_current->update_balance_change,
				fund_boolean,
				contact_key_boolean );

		account_balance_current->update_percent_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			account_balance_update_percent_string(
				SQL_DELIMITER,
				fund_name,
				account_balance_current->full_name,
				account_balance_current->contact_key,
				account_balance_current->account_number,
				account_balance_current->date_string,
				account_balance_current->
					update_balance_change_percent,
				fund_boolean,
				contact_key_boolean );
	}
}

ACCOUNT_BALANCE *account_balance_next(
		char *fund_name,
		char *as_of_date,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number )
{
	char *where;
	char *fetch_date;

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

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_where(
			fund_name,
			as_of_date,
			fund_boolean,
			contact_key_boolean,
			full_name,
			contact_key,
			account_number,
			">" /* relational_operator */ );

	fetch_date =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		account_balance_fetch_date(
			ACCOUNT_BALANCE_TABLE,
			ACCOUNT_BALANCE_MIN_FUNCTION,
			ACCOUNT_BALANCE_MAX_FUNCTION,
			where,
			1 /* fetch_min_boolean */ );

	free( where );

	return
	account_balance_fetch(
		fund_name,
		fetch_date,
		fund_boolean,
		contact_key_boolean,
		full_name,
		contact_key,
		account_number );
}

ACCOUNT_BALANCE *account_balance_prior(
		char *fund_name,
		char *as_of_date,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number )
{
	char *where;
	char *fetch_date;

	if ( !full_name
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

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_where(
			fund_name,
			as_of_date,
			fund_boolean,
			contact_key_boolean,
			full_name,
			contact_key,
			account_number,
			"<" /* relational_operator */ );

	fetch_date =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		account_balance_fetch_date(
			ACCOUNT_BALANCE_TABLE,
			ACCOUNT_BALANCE_MIN_FUNCTION,
			ACCOUNT_BALANCE_MAX_FUNCTION,
			where,
			0 /* not fetch_min_boolean */ );

	return
	account_balance_fetch(
		fund_name,
		fetch_date,
		fund_boolean,
		contact_key_boolean,
		full_name,
		contact_key,
		account_number );
}

ACCOUNT_BALANCE *account_balance_fetch(
		char *fund_name,
		char *date,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number )
{
	char *select_string;
	char *where;
	char *system_string;

	if ( !full_name
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

	if ( !date ) return NULL;

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_select_string(
			ACCOUNT_BALANCE_SELECT /* ENTITY_SELECT */,
			ENTITY_CONTACT_KEY_COLUMN,
			contact_key_boolean );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_where(
			fund_name,
			date,
			fund_boolean,
			contact_key_boolean,
			full_name,
			contact_key,
			account_number,
			"=" /* relational_operator */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select_string,
			ACCOUNT_BALANCE_TABLE,
			where );

	return
	account_balance_parse(
		fund_name,
		contact_key_boolean,
		string_system_input( system_string ) );
}

ACCOUNT_BALANCE *account_balance_latest(
		char *fund_name,
		char *as_of_date,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *account_number )
{
	char *where;
	char *fetch_date;

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

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_balance_where(
			fund_name,
			as_of_date,
			fund_boolean,
			contact_key_boolean,
			full_name,
			contact_key,
			account_number,
			"<=" /* relational_operator */ );

	fetch_date =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		account_balance_fetch_date(
			ACCOUNT_BALANCE_TABLE,
			ACCOUNT_BALANCE_MIN_FUNCTION,
			ACCOUNT_BALANCE_MAX_FUNCTION,
			where,
			0 /* not fetch_min_boolean */ );

	free( where );

	return
	account_balance_fetch(
		fund_name,
		fetch_date,
		fund_boolean,
		contact_key_boolean,
		full_name,
		contact_key,
		account_number );
}

