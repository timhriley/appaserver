/* $APPASERVER_HOME/src_appaserver/output_vertical_frameset.c		*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frameset.h"

void output_vertical_frameset(
			char *application_name,
			char *session_key );

int main( int argc, char **argv )
{
	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s application_name session_key\n",
			argv[ 0 ] );
		exit( 1 );
	}

	output_vertical_frameset(
		argv[ 1 ],
		argv[ 2 ] );

	return 0;
}

void output_vertical_frameset(
			char *application_name,
			char *session_key )
{
	FRAMESET *frameset;

	if ( ! ( frameset =
			frameset_new(
				application_name,
				session_key,
				0 /* not frameset_menu_horizontal */ ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: frameset_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
}
