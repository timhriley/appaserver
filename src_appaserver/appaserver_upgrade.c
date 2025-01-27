/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/appaserver_upgrade.c			*/
/* --------------------------------------------------------------------	*/
/* 						       			*/
/* Run this program after upgrading to a new version			*/
/* of Appaserver.							*/
/* 									*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "environ.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "application_log.h"
#include "appaserver_upgrade.h"

int main( int argc, char **argv )
{
	char *application_name = {0};
	boolean step_boolean = 0;
	APPASERVER_PARAMETER *appaserver_parameter;
	APPASERVER_UPGRADE *appaserver_upgrade;
	APPASERVER_UPGRADE_EXECUTE *appaserver_upgrade_execute;

	if ( argc == 2 )
	{
		step_boolean = ( strcmp( argv[ 1 ], "step" ) == 0 );

		if ( step_boolean )
		{
			application_name =
				environment_application_name();
		}
	}
	else
	if ( argc == 3 )
	{
		step_boolean = ( strcmp( argv[ 2 ], "step" ) == 0 );

		if ( step_boolean ) application_name = argv[ 1 ];
	}

	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	appaserver_upgrade =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_upgrade_fetch(
			APPLICATION_LOG_EXTENSION,
			argc,
			argv,
			application_name,
			step_boolean,
			appaserver_parameter->log_directory,
			appaserver_parameter->mount_point );

	if ( list_rewind( 
		appaserver_upgrade->
			appaserver_upgrade_execute_list ) )
	do {
		appaserver_upgrade_execute =
		     list_get(
			appaserver_upgrade->
				appaserver_upgrade_execute_list );

		if ( !list_length(
			appaserver_upgrade_execute->
				system_string_list ) )
		{
			continue;
		}

		environment_database_set(
			appaserver_upgrade_execute->
				application_name );

		appaserver_upgrade_execute_list_system(
			appaserver_upgrade_execute->
				application_name,
			appaserver_upgrade_execute->
				system_string_list,
			appaserver_upgrade_execute->
				insert_statement_list );

	} while ( list_next(
			appaserver_upgrade->
				appaserver_upgrade_execute_list ) );

	return 0;
}
