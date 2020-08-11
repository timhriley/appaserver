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
#include "investment.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_account_balance_update(
				INVESTMENT_EQUITY *t,
				char *application_name );

void post_change_account_balance_insert(
				INVESTMENT_EQUITY *investment_equity,
				char *application_name );

void post_change_account_balance_delete(
				INVESTMENT_EQUITY *investment_equity,
				char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *street_address;
	char *account_number;
	char *date_time;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_account_number;
	char *preupdate_date_time;
	INVESTMENT_EQUITY *investment_equity;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s ignored fund full_name street_address account_number date_time state preupdate_full_name preupdate_street_address preupdate_account_number preupdate_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	account_number = argv[ 5 ];
	date_time = argv[ 6 ];
	state = argv[ 7 ];
	preupdate_full_name = argv[ 8 ];
	preupdate_street_address = argv[ 9 ];
	preupdate_account_number = argv[ 10 ];
	preupdate_date_time = argv[ 11 ];

fprintf( stderr, "%s/%s()/%d: Turned off.\n",
__FILE__,
__FUNCTION__,
__LINE__ );

exit( 0 );

	/* If Changed the financial institution's name or address */
	/* ------------------------------------------------------ */
	if ( strcmp( account_number, "account_number" ) == 0 ) exit( 0 );

	/* ---------------------------------------------------- */
	/* Need to execute on predelete to get			*/
	/* EQUITY_ACCOUNT_BALANCE.transaction_date_time. 	*/
	/* ---------------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( ! ( investment_equity =
			investment_equity_new(
					application_name,
					full_name,
					street_address,
					fund_name,
					account_number,
					date_time,
					state,
					preupdate_full_name,
					preupdate_street_address,
					preupdate_account_number,
					preupdate_date_time ) ) )
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot investment_equity_new()\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_account_balance_insert(
			investment_equity,
			application_name );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		boolean do_again = 0;

		post_change_account_balance_update(
			investment_equity,
			application_name );

		if ( investment_equity->
				process->
				full_name_change_state ==
		     from_something_to_something_else )
		{
			full_name = preupdate_full_name;
			do_again = 1;
		}

		if ( investment_equity->
				process->
				street_address_change_state ==
		     from_something_to_something_else )
		{
			street_address = preupdate_street_address;
			do_again = 1;
		}

		if ( investment_equity->
				process->
				account_number_change_state ==
		     from_something_to_something_else )
		{
			account_number = account_number;
			do_again = 1;
		}

		if ( do_again )
		{
			investment_equity =
				investment_equity_new(
					application_name,
					full_name,
					street_address,
					fund_name,
					account_number,
					date_time,
					state,
					(char *)0 /* preupdate_full_name */,
					(char *)0 /* preupdate_st..._address */,
					(char *)0 /* preupdate_acc..._number */,
					preupdate_date_time );

			if ( !investment_equity )
			{
				fprintf( stderr,
			 "ERROR in %s/%s()/%d: INVESTMENT_EQUITY is null.\n",
			 		__FILE__,
			 		__FUNCTION__,
			 		__LINE__ );
				exit( 1 );
			}

			post_change_account_balance_update(
				investment_equity,
				application_name );
		}
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_account_balance_delete(
			investment_equity,
			application_name );
	}

	return 0;

} /* main() */

void post_change_account_balance_insert(
				INVESTMENT_EQUITY *t,
				char *application_name )
{
	if ( !t )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: INVESTMENT_EQUITY is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( t->output_account_balance_list =
			investment_calculate_account_balance_list(
				application_name,
				t->investment_account.full_name,
				t->investment_account.street_address,
				t->input.fund_name,
				t->investment_account.investment_account,
				t->investment_account.
					fair_value_adjustment_account,
				t->input_account_balance_list ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot calculate account balance list (%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 t->investment_account.full_name,
			 t->investment_account.street_address,
			 t->investment_account.account_number,
			 t->input.date_time );
		exit( 1 );
	}

	investment_account_balance_list_update(
		t->output_account_balance_list,
		application_name,
		t->input_account_balance_list,
		t->investment_account.full_name,
		t->investment_account.street_address,
		t->investment_account.account_number );

} /* post_change_account_balance_insert() */

void post_change_account_balance_update(
				INVESTMENT_EQUITY *t,
				char *application_name )
{
	if ( !t )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: INVESTMENT_EQUITY is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( t->output_account_balance_list =
			investment_calculate_account_balance_list(
				application_name,
				t->investment_account.full_name,
				t->investment_account.street_address,
				t->input.fund_name,
				t->investment_account.investment_account,
				t->investment_account.
					fair_value_adjustment_account,
				t->input_account_balance_list ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot calculate account balance list (%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 t->investment_account.full_name,
			 t->investment_account.street_address,
			 t->investment_account.account_number,
			 t->input.date_time );
		exit( 1 );
	}

	investment_account_balance_list_update(
		t->output_account_balance_list,
		application_name,
		t->input_account_balance_list,
		t->investment_account.full_name,
		t->investment_account.street_address,
		t->investment_account.account_number );

} /* post_change_account_balance_update() */

void post_change_account_balance_delete(
				INVESTMENT_EQUITY *t,
				char *application_name )
{
	ACCOUNT_BALANCE *account_balance;

	if ( !t )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: INVESTMENT_EQUITY is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( account_balance =
			investment_account_balance_seek(
				t->input_account_balance_list,
				t->input.date_time ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek account balance list (%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 t->investment_account.full_name,
			 t->investment_account.street_address,
			 t->investment_account.account_number,
			 t->input.date_time );
		exit( 1 );
	}

	list_delete_current( t->input_account_balance_list );

	if ( ! ( t->output_account_balance_list =
			investment_calculate_account_balance_list(
				application_name,
				t->investment_account.full_name,
				t->investment_account.street_address,
				t->input.fund_name,
				t->investment_account.investment_account,
				t->investment_account.
					fair_value_adjustment_account,
				t->input_account_balance_list ) ) )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: investment_calculate_account_balance_list (%s,%s,%s,%s) returned NULL. TRANSACTION will still be deleted.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 t->investment_account.full_name,
			 t->investment_account.street_address,
			 t->investment_account.account_number,
			 t->input.date_time );
	}
	else
	{
		investment_account_balance_list_update(
			t->output_account_balance_list,
			application_name,
			t->input_account_balance_list,
			t->investment_account.full_name,
			t->investment_account.street_address,
			t->investment_account.account_number );
	}

	account_balance->transaction =
		ledger_transaction_fetch(
			application_name,
			t->investment_account.full_name,
			t->investment_account.street_address,
			account_balance->transaction_date_time );

	if ( !account_balance->transaction )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_transaction_delete_propagate(
				application_name,
				account_balance->
					transaction->
					full_name,
				account_balance->
					transaction->
					street_address,
				account_balance->
					transaction->
					transaction_date_time,
				account_balance->
					transaction->
					journal_ledger_list );

} /* post_change_account_balance_delete() */

