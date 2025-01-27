/* database_management_system.c */
/* ---------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_parameter_file.h"

int main( void )
{
	printf( "%s\n", appaserver_parameter_file_get_dbms() );
	return 0;

} /* main() */

