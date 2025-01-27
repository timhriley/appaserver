/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/appaserver_folder_list.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "folder.h"

int main( void )
{
	LIST *system_name_list;

	system_name_list = folder_system_name_list();

	if ( list_rewind( system_name_list ) )
	{
		do {
			printf( "%s\n", (char *)list_get( system_name_list ) );

		} while ( list_next( system_name_list ) );
	}

	return 0;
}
