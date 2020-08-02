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
#include "account.h"

char *account_escape_name( char *account_name )
{
	static char escape_name[ 256 ];

	string_escape_quote( escape_name, account_name );
	return escape_name;
}
