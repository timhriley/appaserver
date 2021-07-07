/* $APPASERVER_HOME/library/security.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "query.h"
#include "appaserver_error.h"
#include "environ.h"
#include "security.h"

char *security_sql_injection_escape( char *source )
{
	char destination[ QUERY_WHERE_BUFFER ];
	char *return_destination;

	string_escape_character_array(
		destination,
		source,
		"`'$;%" );

	return_destination = strdup( destination );

{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: returning = %x [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
(int)return_destination,
return_destination );
m2( environment_application(), msg );
}
	return return_destination;
}

