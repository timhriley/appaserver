/* --------------------------------------------------------------------	 */
/* $APPASERVER_HOME/src_predictive/post_change_account_balance.c   	 */
/* --------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	 */
/* --------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "inventory.h"
#include "appaserver_error.h"
#include "account_balance.h"

void post_change_account_balance_insert_update_delete(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *account_number;
	char *date;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s fund_name full_name contact_key account_number date state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	account_number = argv[ 4 ];
	date = argv[ 5 ];
	state = argv[ 6 ];

	/* If Changed the financial institution's name or address */
	/* ------------------------------------------------------ */
	if ( strcmp( account_number, "account_number" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0
	||   strcmp( state, "delete" ) == 0 )
	{
		post_change_account_balance_insert_update_delete(
			fund_name,
			full_name,
			contact_key,
			account_number,
			date );
	}

	return 0;
}

void post_change_account_balance_insert_update_delete(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *account_number,
		char *date )
{
	ACCOUNT_BALANCE_TRIGGER *account_balance_trigger;

	account_balance_trigger =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_balance_trigger_new(
			fund_name,
			full_name,
			contact_key,
			account_number,
			date );

	account_balance_update(
		account_balance_trigger->predictive_fund_boolean,
		account_balance_trigger->entity_contact_key_boolean,
		account_balance_trigger->account_balance_fetch,
		account_balance_trigger->account_balance_next );
}

