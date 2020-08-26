/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/account_balance.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "list.h"
#include "entity.h"
#include "investment_account.h"
#include "account_balance.h"

ACCOUNT_BALANCE *account_balance_new(
			char *full_name,
			char *street_address,
			char *account_number,
			char *date )
{
	ACCOUNT_BALANCE *account_balance;

	if ( ! ( account_balance =
			calloc( 1, sizeof( ACCOUNT_BALANCE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	account_balance->financial_entity =
		entity_new(
			full_name,
			street_address );

	account_balance->account_number = account_number;
	account_balance->date = date;

	return account_balance;
}

char *account_balance_select( void )
{
	return
	"full_name,"
	"street_address,"
	"account_number,"
	"date,"
	"balance,"
	"balance_change,"
	"balance_change_percent";
}

ACCOUNT_BALANCE *account_balance_parse( char *input )
{
	ACCOUNT_BALANCE *account_balance;
	char full_name[ 128 ];
	char street_address[ 128 ];
	char account_number[ 128 ];
	char date[ 128 ];
	char balance[ 128 ];
	char balance_change[ 128 ];
	char balance_change_percent[ 128 ];

	if ( !input || !*input ) return (ACCOUNT_BALANCE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( account_number, SQL_DELIMITER, input, 2 );
	piece( date, SQL_DELIMITER, input, 3 );

	account_balance =
		account_balance_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( account_number ),
			strdup( date ) );

	piece( balance, SQL_DELIMITER, input, 4 );
	account_balance->balance = atof( balance );

	piece( balance_change, SQL_DELIMITER, input, 5 );
	account_balance->balance_change_database =
		atof( balance_change );

	piece( balance_change_percent, SQL_DELIMITER, input, 6 );
	account_balance->balance_change_percent_database =
		atof( balance_change_percent );

	return account_balance;
}

void account_balance_update(
			double account_balance_change,
			int account_balance_change_percent,
			char *full_name,
			char *street_address,
			char *account_number,
			char *date )
{
	FILE *update_pipe;

	update_pipe = account_balance_update_open();

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^balance_change^%.2lf\n",
		full_name,
		street_address,
		account_number,
		date,
		account_balance_change );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^balance_change_percent^%d\n",
		full_name,
		street_address,
		account_number,
		date,
		account_balance_change_percent );

	pclose( update_pipe );
}

FILE *account_balance_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"account_number,"
		"date";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql						 ",
		 ACCOUNT_BALANCE_TABLE,
		 key );

	return popen( sys_string, "w" );
}

ACCOUNT_BALANCE *account_balance_steady_state(
			char *full_name,
			char *street_address,
			char *account_number,
			char *date,
			double balance,
			double balance_change,
			int balance_change_percent,
			LIST *investment_account_balance_list )
{
	ACCOUNT_BALANCE *account_balance;

	account_balance =
		account_balance_new(
			full_name,
			street_address,
			account_number,
			date );

	account_balance->balance = balance;

	account_balance->balance_change_database = balance_change;

	account_balance->balance_change_percent_database =
		balance_change_percent;

	account_balance->account_balance_change =
		account_balance_change(
			account_balance->balance,
			investment_account_balance_list );

	account_balance->account_balance_change_percent =
		account_balance_change_percent(
			account_balance->balance,
			investment_account_balance_list );

	return account_balance;
}

char *account_balance_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 account_balance_select(),
		 ACCOUNT_BALANCE_TABLE,
		 where,
		 "date" );

	return strdup( sys_string );
}

LIST *account_balance_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *account_balance_list;

	if ( !sys_string ) return (LIST *)0;

	account_balance_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			account_balance_list, 
			account_balance_parse( input ) );
	}

	pclose( input_pipe );
	return account_balance_list;
}

double account_balance_change(
			double balance,
			LIST *investment_account_balance_list )
{
	ACCOUNT_BALANCE *prior_account_balance;
	int length;

	length = list_length( investment_account_balance_list );

	if ( !length )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: empty account_balance_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0.0;
	}

	if ( length == 1 )
	{
		return balance;
	}

	list_last( investment_account_balance_list );
	list_previous( investment_account_balance_list );
	prior_account_balance = list_get( investment_account_balance_list );

	return	balance -
		prior_account_balance->balance;
}

int account_balance_change_percent(
			double balance,
			LIST *investment_account_balance_list )
{
	int length;
	double change;
	double change_rate;

	length = list_length( investment_account_balance_list );

	if ( !length )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: empty investment_account_balance_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0.0;
	}

	if ( timlib_dollar_virtually_same( balance, 0.0 ) )
		return 100;

	change =
		account_balance_change(
			balance,
			investment_account_balance_list );

	change_rate = change / balance;
	return timlib_round_int( change_rate * 100.0 );
}

double account_balance_latest(
			LIST *investment_account_balance_list )
{
	ACCOUNT_BALANCE *account_balance;

	if ( !list_tail( investment_account_balance_list ) ) return 0.0;

	account_balance = list_get( investment_account_balance_list );
	return account_balance->balance;
}

ACCOUNT_BALANCE *account_balance_seek(
			LIST *investment_account_balance_list,
			char *date )
{
	ACCOUNT_BALANCE *account_balance;

	if ( !list_rewind( investment_account_balance_list ) )
		return (ACCOUNT_BALANCE *)0;

	do {
		account_balance = list_get( investment_account_balance_list );

		if ( timlib_strcmp(
			account_balance->date,
			date ) == 0 )
		{
			return account_balance;
		}

	} while( list_next( investment_account_balance_list ) );

	return (ACCOUNT_BALANCE *)0;
}

LIST *account_balance_list_steady_state(
			LIST *account_balance_list )
{
	ACCOUNT_BALANCE *account_balance;
	LIST *steady_balance_list;

	if ( !list_rewind( account_balance_list ) ) return (LIST *)0;

	steady_balance_list = list_new();

	do {
		account_balance = list_get( account_balance_list );

		list_push( account_balance_list );

		list_set(
			steady_balance_list,
			account_balance_steady_state(
				account_balance->
					financial_entity->
					full_name,
				account_balance->
					financial_entity->
					street_address,
				account_balance->account_number,
				account_balance->date,
				account_balance->balance,
				account_balance->balance_change_database,
				account_balance->
					balance_change_percent_database,
				account_balance_list ) );

		list_pop( account_balance_list );

	} while ( list_next( account_balance_list ) );

	return steady_balance_list;
}

