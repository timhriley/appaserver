/* --------------------------------------------- */
/* $APPASERVER_HOME/library/cookie.c		 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cookie.h"

char *cookie_key(	char *form_name,
			char *folder_name )
{
	static char key[ 128 ];

	if ( !form_name || !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"<%s_%s>",
		form_name,
		folder_name );

	return key;
}
