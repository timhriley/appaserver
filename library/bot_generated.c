/* --------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/bot_generated.c				*/
/* --------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environ.h"
#include "appaserver.h"
#include "bot_generated.h"

BOT_GENERATED *bot_generated_new( char *argv_0 )
{
	BOT_GENERATED *bot_generated;

	bot_generated = bot_generated_calloc();

/* Turn off b/c of false positives. */
return bot_generated;

	bot_generated->remote_ip_address =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		bot_generated_remote_ip_address();

	if ( !bot_generated->remote_ip_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			"bot_generated_remote_ip_address" );
		exit( 1 );
	}

	environment_prepend_path( BOT_GENERATED_DIRECTORY );
	environment_utility_path();

	bot_generated->appaserver_executable_exists_boolean =
		appaserver_executable_exists_boolean(
			BOT_GENERATED_DENY_PROCESS );

	if ( !bot_generated->appaserver_executable_exists_boolean )
		return bot_generated;

	bot_generated->system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		bot_generated_system_string(
			BOT_GENERATED_DENY_PROCESS,
			bot_generated->remote_ip_address );

	bot_generated->yes_boolean =
		bot_generated_yes_boolean(
			bot_generated->system_string );

	if ( bot_generated->yes_boolean )
	{
		bot_generated->message =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			bot_generated_message(
				argv_0,
				bot_generated->remote_ip_address );
	}

	return bot_generated;
}

BOT_GENERATED *bot_generated_calloc( void )
{
	BOT_GENERATED *bot_generated;

	if ( ! ( bot_generated =
			calloc( 1,
				sizeof ( BOT_GENERATED ) ) ) )
	{
		fprintf(stderr,
			"%s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return bot_generated;
}

char *bot_generated_remote_ip_address( void )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	environment_get( "REMOTE_ADDR" );
}

boolean bot_generated_yes_boolean( char *system_string )
{
	int result;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
	}

	result = system( system_string );

	/* ------------------------------ */
	/* Why is "exit 1" returning 256? */
	/* ------------------------------ */
	if ( result == 1 || result == 256 )
		return 1;
	else
		return 0;
}

char *bot_generated_system_string(
		const char *bot_generated_deny_process,
		char *remote_ip_address )
{
	static char system_string[ 256 ];

	if ( !remote_ip_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: remote_ip_address is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s",
		bot_generated_deny_process,
		remote_ip_address );

	return system_string;
}

char *bot_generated_message(
		char *argv_0,
		char *remote_ip_address )
{
	static char message[ 128 ];

	if ( !argv_0
	||   !remote_ip_address )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		message,
		sizeof ( message ),
		"%s: Bot generated remote_ip_address=%s",
		argv_0,
		remote_ip_address );

	return message;
}
