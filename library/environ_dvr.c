/* library/environ_dvr.c */
/* --------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_library.h"

int main()
{
	char *str;
	char buffer[ 1024 ];

 	str = "this_file.sh $root/invlink/load/trading";

	printf( "resolved: %s\n",
		 resolve_environment_variables( buffer, str ) );
	return 0;
}
