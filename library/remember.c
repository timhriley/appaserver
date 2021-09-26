/* --------------------------------------------- */
/* $APPASERVER_HOME/library/remember.c		 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "remember.h"

REMEMBER *remember_calloc( void )
{
	REMEMBER *remember;

	if ( ! ( remember = calloc( 1, sizeof ( REMEMBER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return remember;
}

char *remember_button( char *control_string )
{
	char button[ 128 ];

	if ( !control_string || !*control_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: control_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(button,
"<td><input type=\"button\" value=\"Recall\" onClick=\"%s\">\n",
		control_string );

	return strdup( button );
}

