/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/self_trigger.c				*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "sql.h"
#include "preupdate_change.h"
#include "credit_provider.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *state;
	char *full_name;
	char *street_address;
	char *preupdate_credit_card_number;
	char *credit_card_number;
	enum preupdate_change_state preupdate_change_state;

	application_name = environment_exit_application_name( argv[ 0 ] );

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s state full_name street_address preupdate_credit_card_number credit_card_number\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	state = argv[ 1 ];
	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	preupdate_credit_card_number = argv[ 4 ];
	credit_card_number = argv[ 5 ];

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	preupdate_change_state =
		preupdate_change_state_evaluate(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_credit_card_number
				/* preupdate_datum */,
			credit_card_number
				/* postupdate_datum */,
			"preupdate_credit_card_number"
				/* preupdate_placeholder_name */ );

	if ( preupdate_change_state == no_change_null
	||   preupdate_change_state == no_change_something )
	{
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		CREDIT_PROVIDER *credit_provider;
		char *error_string;

		credit_provider =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			credit_provider_new(
				full_name,
				street_address,
				credit_card_number );

		error_string =
			/* ---------------------------- */
			/* Returns error_string or null */
			/* ---------------------------- */
			sql_execute(
				SQL_EXECUTABLE,
				appaserver_error_filename(
					application_name ),
				(LIST *)0 /* sql_list */,
				credit_provider->sql_statement );

		if ( error_string )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"sql_execute() returned %s.",
				error_string );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	return 0;
}

