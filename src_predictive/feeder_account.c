/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_account.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "security.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "sql.h"
#include "feeder_account.h"

FEEDER_ACCOUNT *feeder_account_fetch(
		const char *feeder_account_table,
		char *feeder_account_name )
{
	char *system_string;
	char *pipe_fetch;


	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			FEEDER_ACCOUNT_SELECT,
			(char *)feeder_account_table,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_account_primary_where(
				FEEDER_ACCOUNT_PRIMARY_KEY,
				feeder_account_name ) );

	if ( ! ( pipe_fetch =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_fetch(
				system_string ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_pipe_fetch(%s) returned empty.",
			feeder_account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	feeder_account_parse(
		feeder_account_name,
		pipe_fetch );
}

char *feeder_account_primary_where(
		const char *feeder_account_primary_key,
		char *feeder_account_name )
{
	static char primary_where[ 128 ];
	char *tmp;

	if ( !feeder_account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		primary_where,
		sizeof ( primary_where ),
		"%s = '%s'",
		feeder_account_primary_key,
		( tmp =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				feeder_account_name /* datum */ ) ) );

	free( tmp );

	return primary_where;
}

FEEDER_ACCOUNT *feeder_account_parse(
		char *feeder_account_name,
		char *string_pipe_fetch )
{
	FEEDER_ACCOUNT *feeder_account;
	char buffer[ 128 ];

	if ( !feeder_account_name
	||   !string_pipe_fetch )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_account =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_account_new(
			feeder_account_name );


	/* See FEEDER_ACCOUNT_SELECT */
	/* ------------------------- */
	piece( buffer, SQL_DELIMITER, string_pipe_fetch, 0 );

	if ( *buffer )
	{
		feeder_account->financial_institution_full_name =
			strdup( buffer );
	}
	else
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"financial_institution_full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece( buffer, SQL_DELIMITER, string_pipe_fetch, 1 );
	if ( *buffer )
		feeder_account->financial_institution_street_address =
			strdup( buffer );

	return feeder_account;
}

FEEDER_ACCOUNT *feeder_account_new( char *feeder_account_name )
{
	FEEDER_ACCOUNT *feeder_account;

	if ( !feeder_account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	feeder_account = feeder_account_calloc();
	feeder_account->feeder_account_name = feeder_account_name;

	return feeder_account;
}

FEEDER_ACCOUNT *feeder_account_calloc( void )
{
	FEEDER_ACCOUNT *feeder_account;

	if ( ! ( feeder_account = calloc( 1, sizeof ( FEEDER_ACCOUNT ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_account;
}
