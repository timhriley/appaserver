/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/program.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "boolean.h"
#include "program.h"

char *program_escape_name( char *program_name )
{
	static char name[ 128 ];

	return string_escape( name, program_name, "'" );
}

PROGRAM *program_new( char *program_name )
{
	PROGRAM *program;

	if ( ! ( program = calloc( 1, sizeof( PROGRAM ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	program->program_name = program_name;
	return program;
}
