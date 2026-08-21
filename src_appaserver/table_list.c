/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/table_list.c				*/
/* ---------------------------------------------------------------	*/
/* This process outputs the Mysql tables.				*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"

int main( void )
{
	LIST *table_name_list;

	/* Retrieve all the Mysql tables for a database */
	/* -------------------------------------------- */
	table_name_list = application_table_name_list();

	if ( list_rewind( table_name_list ) )
	do {
		printf( "%s\n", (char *)list_get( table_name_list ) );

	} while ( list_next( table_name_list ) );

	return 0;
}
