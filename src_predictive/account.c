/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/account.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "boolean.h"
#include "element.h"
#include "account.h"

char *account_name_escape(
			char *account_name )
{
	return account_escape_name( account_name );
}

char *account_escape_name(
			char *account_name )
{
	static char escape_name[ 256 ];

	string_escape_quote( escape_name, account_name );
	return escape_name;
}

boolean account_accumulate_debit(
			char *account_name )
{
	ELEMENT *element;

	if ( ! ( element =
			element_account_name_fetch(
				account_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot fetch element for account = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name );
		return 0;
	}

	return element->accumulate_debit;
}

char *account_receivable( void )
{
	return "";
}

char *account_cash( void )
{
	return "";
}

char *account_fees_expense( void )
{
	return "";
}

char *account_name_display( char *account_name )
{
	if ( !account_name )
		return LEDGER_NOT_SET_ACCOUNT;
	else
		return account_name;
}

