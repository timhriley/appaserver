/* ---------------------------------------------------------------	*/
/* src_predictive/propagate_purchase_order_accounts.c			*/
/* ---------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name = {0};
	char *fund_name;
	char *transaction_date_time;
	LIST *inventory_account_list;
	char *sales_tax_expense_account = {0};
	char *freight_in_expense_account = {0};
	char *account_payable_account = {0};
	char *specific_inventory_account = {0};
	char *cost_of_goods_sold_account = {0};

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s ignored fund transaction_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 2 ];
	transaction_date_time = argv[ 3 ];

	inventory_account_list =
		ledger_get_inventory_account_name_list(
			application_name );

	ledger_get_purchase_order_account_names(
		&sales_tax_expense_account,
		&freight_in_expense_account,
		&account_payable_account,
		application_name,
		fund_name );

	if ( !list_length( inventory_account_list ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get purchase order account names.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_propagate_account_name_list(
				application_name,
				transaction_date_time,
				inventory_account_list );

	if ( sales_tax_expense_account )
	{
		ledger_propagate(	application_name,
					transaction_date_time,
					sales_tax_expense_account );
	}

	if ( freight_in_expense_account )
	{
		ledger_propagate(	application_name,
					transaction_date_time,
					freight_in_expense_account );
	}

	ledger_propagate(	application_name,
				transaction_date_time,
				account_payable_account );

	return 0;

} /* main() */

