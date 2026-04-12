#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account_delta.h"

int main( void )
{
	ACCOUNT_DELTA *account_delta;

	account_delta =
		account_delta_new(
			(char *)0 /* fund_name */,
			"investment" /* debit_account_name */,
			"investment_increase" /* credit_account_name */,
			1 /* accumulate_debit_boolean */,
			1000000 /* ending_balance */,
			"Mark to market" /* memo */ );

	if ( !account_delta->transaction_binary )
	{
		printf(
			"%s\n",
			ACCOUNT_DELTA_NO_CHANGE );

		exit( 0 );
	}

	transaction_html_display(
		account_delta->
			transaction_binary );

	return 0;
}

