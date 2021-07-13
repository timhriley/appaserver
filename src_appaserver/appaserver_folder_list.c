/* $APPASERVER_HOME/src_appaserver/appaserver_folder_list.c	*/
/* --------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------------	*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"

int main( void )
{
	char **system_folder_list;

	for ( 	system_folder_list =
			appaserver_library_system_folder_list();
		*system_folder_list; 
		system_folder_list++ )
	{
		printf( "%s\n", *system_folder_list );
	}
	return 0;
} /* main() */
