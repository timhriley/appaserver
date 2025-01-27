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
#include "investment_account.h"

/* Returns error_string or null */
/* ---------------------------- */
char *post_change_account_balance_insert_update_delete(
		char *full_name,
		char *street_address,
		char *account_number );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *account_number;
	char *date_string;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
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

	/* Stub */
	/* ---- */
	if ( ( date_string = argv[ 4 ] ) ){}

	state = argv[ 5 ];

	/* If Changed the financial institution's name or address */
	/* ------------------------------------------------------ */
	if ( strcmp( account_number, "account_number" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0
	||   strcmp( state, "delete" ) == 0 )
	{
		char *error_string =
			post_change_account_balance_insert_update_delete(
				full_name,
				street_address,
				account_number );

		if ( error_string )
		{
			appaserver_error_message_file(
				application_name,
				(char *)0 /* login_name */,
				error_string /* message */ );
		}
	}

	return 0;
}

char *post_change_account_balance_insert_update_delete(
		char *full_name,
		char *street_address,
		char *account_number )
{
	INVESTMENT_ACCOUNT *investment_account;
	char *error_string;

	if ( ! ( investment_account =
			investment_account_fetch(
				full_name,
				street_address,
				account_number,
				1 /* fetch_account_balance_list */ ) ) )
	{
		/* Might be deleted */
		/* ---------------- */
		return (char *)0;
	}

	error_string =
		account_balance_list_update(
			investment_account->
				account_balance_list );

	if ( error_string ) return error_string;

	return
	/* ---------------------------- */
	/* Returns error_string or null */
	/* ---------------------------- */
	investment_account_update(
		INVESTMENT_ACCOUNT_TABLE,
		investment_account->primary_where,
		investment_account->update_balance_latest );
}

