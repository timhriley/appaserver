/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/investment_account.c		*/
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
#include "entity.h"
#include "list.h"
#include "account_balance.h"
#include "investment_account.h"

INVESTMENT_ACCOUNT *investment_account_new(
			char *full_name,
			char *street_address,
			char *account_number )
{
	INVESTMENT_ACCOUNT *investment_account;

	if ( ! ( investment_account =
			calloc( 1, sizeof( INVESTMENT_ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	investment_account->financial_entity =
		entity_new(
			full_name,
			street_address );

	investment_account->account_number = account_number;

	return investment_account;
}

INVESTMENT_ACCOUNT *investment_account_fetch(
			char *full_name,
			char *street_address,
			char *account_number )
{
	return investment_account_parse(
		pipe2string(
			investment_account_sys_string(
		 		/* -------------------------- */
		 		/* Safely returns heap memory */
		 		/* -------------------------- */
		 		investment_account_primary_where(
					full_name,
					street_address,
					account_number ) ) ) );
}

char *investment_account_select( void )
{
	return
	"full_name,"
	"street_address,"
	"account_number,"
	"investment_classification,"
	"investment_purpose,"
	"certificate_maturity_months,"
	"certificate_maturity_date,"
	"interest_rate,"
	"balance_latest";
}

INVESTMENT_ACCOUNT *investment_account_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char account_number[ 128 ];
	char investment_classification[ 128 ];
	char investment_purpose[ 128 ];
	char certificate_maturity_months[ 128 ];
	char certificate_maturity_date[ 128 ];
	char interest_rate[ 128 ];
	char balance_latest[ 128 ];

	if ( !input || !*input ) return (INVESTMENT_ACCOUNT *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( account_number, SQL_DELIMITER, input, 2 );
	piece( investment_classification, SQL_DELIMITER, input, 3 );
	piece( investment_purpose, SQL_DELIMITER, input, 4 );
	piece( certificate_maturity_months, SQL_DELIMITER, input, 5 );
	piece( certificate_maturity_date, SQL_DELIMITER, input, 6 );
	piece( interest_rate, SQL_DELIMITER, input, 7 );
	piece( balance_latest, SQL_DELIMITER, input, 8 );

	return investment_account_steady_state(
			strdup( full_name ),
			strdup( street_address ),
			strdup( account_number ),
			strdup( investment_classification ),
			strdup( investment_purpose ),
			atoi( certificate_maturity_months ),
			strdup( certificate_maturity_date ),
			atoi( interest_rate ),
			atof( balance_latest ),
			account_balance_list(
				full_name,
				street_address,
				account_number ) );
}

void investment_account_update(
			double balance_latest,
			char *full_name,
			char *street_address,
			char *account_number )
{
	FILE *update_pipe;

	update_pipe = investment_account_update_open();

	fprintf(update_pipe,
	 	"%s^%s^%s^balance_latest^%.2lf\n",
		full_name,
		street_address,
		account_number,
		balance_latest );

	pclose( update_pipe );
}

FILE *investment_account_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"account_number";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 INVESTMENT_ACCOUNT_TABLE,
		 key );

	return fopen( sys_string, "w" );
}

INVESTMENT_ACCOUNT *investment_account_steady_state(
			char *full_name,
			char *street_address,
			char *account_number,
			char *investment_classification,
			char *investment_purpose,
			int certificate_maturity_months,
			char *certificate_maturity_date,
			double interest_rate,
			double balance_latest_database,
			LIST *account_balance_list )
{
	INVESTMENT_ACCOUNT *investment_account;

	investment_account =
		investment_account_new(
			full_name,
			street_address,
			account_number );

	investment_account->investment_classification =
		investment_classification;

	investment_account->investment_purpose = investment_purpose;

	investment_account->certificate_maturity_months =
		certificate_maturity_months;

	investment_account->certificate_maturity_date =
		certificate_maturity_date;

	investment_account->interest_rate = interest_rate;

	investment_account->balance_latest_database =
		balance_latest_database;

	investment_account->account_balance_latest =
		account_balance_latest(
			account_balance_list );

	return investment_account;
}

char *investment_account_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 investment_account_select(),
		 INVESTMENT_ACCOUNT_TABLE,
		 where,
		 investment_account_select() );

	return strdup( sys_string );
}

LIST *investment_account_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *investment_account_list;

	if ( !sys_string ) return (LIST *)0;

	investment_account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			investment_account_list, 
			investment_account_parse( input ) );
	}

	pclose( input_pipe );
	return investment_account_list;
}

char *investment_account_primary_where(
			char *full_name,
			char *street_address,
			char *account_number )
{
	char where[ 1024 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "account_number = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address,
		 account_number );

	return strdup( where );
}

