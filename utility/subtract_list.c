/* ------------------------------------------------------- */
/* $APPASERVER_HOME/utility/subtract_list.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"

int main( int argc, char **argv )
{
	LIST *results;
	LIST *source;
	LIST *subtract;

	if ( argc != 3 )
	{
		fprintf( stderr, 
			 "Usage: %s source_file subtract_file\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	if ( ! ( source = list_usage_file2list( argv[ 1 ] ) ) )
	{
		fprintf( stderr,
			 "ERROR: %s cannot open %s for read.\n",
			 argv[ 0 ],
			 argv[ 1 ] );
	}

	if ( ! ( subtract = list_usage_file2list( argv[ 2 ] ) ) )
	{
		fprintf( stderr,
			 "ERROR: %s cannot open %s for read.\n",
			 argv[ 0 ],
			 argv[ 2 ] );
	}

	results = subtract_list( source, subtract );
	list_usage_stdout_display( results );

	return 0;
}

