/* $APPASERVER_HOME/library/security.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "query.h"
#include "security.h"

char *security_sql_injection_escape( char *source )
{
	char destination[ QUERY_WHERE_BUFFER ];

	string_escape_character_array(
		destination,
		source,
		"`'$;%" );

	return strdup( destination );
}

