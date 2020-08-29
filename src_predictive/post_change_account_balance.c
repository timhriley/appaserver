/* --------------------------------------------------------------------	 */
/* $APPASERVER_HOME/src_predictive/post_change_account_balance.c   	 */
/* --------------------------------------------------------------------- */
/* 									 */
/* Freely available software: see Appaserver.org			 */
/* --------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "account_balance.h"
#include "investment_account.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_account_balance_insert_update_delete(
			char *full_name,
			char *street_address,
			char *account_number,
			char *date );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *account_number;
	char *date;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
	"Usage: %s full_name street_address account_number date state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	account_number = argv[ 3 ];
	date = argv[ 4 ];
	state = argv[ 5 ];

	/* If Changed the financial institution's name or address */
	/* ------------------------------------------------------ */
	if ( strcmp( account_number, "account_number" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0
	||   strcmp( state, "delete" ) == 0 )
	{
		post_change_account_balance_insert_update_delete(
			full_name,
			street_address,
			account_number,
			date );
	}

	return 0;

}

void post_change_account_balance_insert_update_delete(
			char *full_name,
			char *street_address,
			char *account_number,
			char *date )
{
	INVESTMENT_ACCOUNT *investment_account;
	ACCOUNT_BALANCE *account_balance;

	if ( ! ( investment_account =
			investment_account_fetch(
				full_name,
				street_address,
				account_number ) ) )
	{
		return;
	}

	if ( ( account_balance =
			account_balance_seek(
				investment_account->
					investment_account_balance_list,
				date ) ) )
	{
		account_balance_update(
			account_balance->account_balance_change,
			account_balance->account_balance_change_percent,
			account_balance->
				financial_entity->
					full_name,
			account_balance->
				financial_entity->
					street_address,
			account_balance->account_number,
			account_balance->date );
	}

	investment_account_update(
		investment_account->account_balance_latest,
		investment_account->
			financial_entity->
				full_name,
		investment_account->
			financial_entity->
				street_address,
		investment_account->account_number );
}

