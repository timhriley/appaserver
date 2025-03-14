/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/escape_security.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "security.h"
 
int main( void )
{
	char input_buffer[ STRING_64K ];
	char *sql_injection_escape;

	while( string_input( input_buffer, stdin, sizeof ( input_buffer ) ) )
	{
		sql_injection_escape =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				input_buffer /* datum */ );

		printf( "%s\n", sql_injection_escape );
		free( sql_injection_escape );
	}

	return 0;
}
 
