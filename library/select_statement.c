/* $APPASERVER_HOME/library/select_statement.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "select_statement.h"

SELECT_STATEMENT *select_statement_fetch(
		char *select_statement_title,
		char *login_name )
{
	char *system_string;
	char *input;

	if ( !select_statement_title
	||   !login_name )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			SELECT_STATEMENT_SELECT,
			SELECT_STATEMENT_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			select_statement_primary_where(
				select_statement_title,
				login_name ) );

	if ( ! ( input =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_input(
				system_string ) ) )
	{
		return (SELECT_STATEMENT *)0;
	}

	return
	select_statement_parse(
		select_statement_title,
		login_name,
		input );
}

char *select_statement_primary_where(
		char *select_statement_title,
		char *login_name )
{
	static char where[ 256 ];

	if ( !select_statement_title
	||   !login_name )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"select_statement_title = '%s' and "
		"login_name = '%s'",
		select_statement_title,
		login_name );

	return where;
}

SELECT_STATEMENT *select_statement_parse(
		char *select_statement_title,
		char *login_name,
		char *input )
{
	SELECT_STATEMENT *select_statement;

	if ( !select_statement_title
	||   !login_name
	||   !input )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select_statement = select_statement_calloc();

	select_statement->select_statement_title = select_statement_title;
	select_statement->login_name = login_name;
	select_statement->statement = strdup( input );

	return select_statement;
}

SELECT_STATEMENT *select_statement_calloc( void )
{
	SELECT_STATEMENT *select_statement;

	if ( ! ( select_statement =
			calloc( 1,
				sizeof ( SELECT_STATEMENT ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return select_statement;
}

