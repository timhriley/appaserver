/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/table_list.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "folder.h"

int main( int argc, char **argv )
{
	LIST *table_name_list;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s role_name\n",
			argv[ 0 ] );

		exit( 1 );
	}

	table_name_list = folder_table_name_list( argv[ 1 ] /* role_name */ );

	if ( list_rewind( table_name_list ) )
	do {
		printf( "%s\n", (char *)list_get( table_name_list ) );

	} while ( list_next( table_name_list ) );

	return 0;
}
