/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction_html_display.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "appaserver_error.h"
#include "transaction.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *transaction_date_time;
	TRANSACTION *transaction;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s transaction_date_time\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	transaction_date_time = argv[ 1 ];

	if ( ! ( transaction =
			transaction_date_time_fetch(
				transaction_date_time,
				1 /* fetch_journal_list */ ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: transaction_date_time_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			transaction_date_time );
		exit( 1 );
	}

	transaction_html_display( transaction );

	return 0;
}

