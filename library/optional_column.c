/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/optional_column.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "security.h"
#include "optional_column.h"

OPTIONAL_COLUMN *optional_column_new(
		const char delimiter,
		char *base_string,
		char *component,
		boolean escape_boolean,
		boolean set_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	if ( !base_string
	||   !component )
	{
		char message[ 1024 ];

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

	optional_column = optional_column_calloc();

	if ( !set_boolean )
	{
		optional_column->return_string = strdup( base_string );
	}
	else
	{
		if ( escape_boolean )
		{
			component =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				security_sql_injection_escape(
					SECURITY_ESCAPE_CHARACTER_STRING,
					component );
		}

		optional_column->return_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			optional_column_return_string(
				delimiter,
				base_string,
				component );

		if ( escape_boolean ) free( component );
	}

	return optional_column;
}

OPTIONAL_COLUMN *optional_column_calloc( void )
{
	OPTIONAL_COLUMN *optional_column;

	if ( ! ( optional_column = calloc( 1, sizeof ( OPTIONAL_COLUMN ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return optional_column;
}

char *optional_column_return_string(
		const char delimiter,
		char *base_string,
		char *component )
{
	char return_string[ 1024 ];

	snprintf(
		return_string,
		sizeof ( return_string ),
		"%s%c%s",
		base_string,
		delimiter,
		component );

	return strdup( return_string );
}

