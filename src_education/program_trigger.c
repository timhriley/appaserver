/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/program_trigger.c	*/
/* ---------------------------------------------------- */
/* 							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "enrollment.h"
#include "semester.h"
#include "program.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *program_name;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s program_name state\n",
			 argv[ 0 ] );
		fprintf( stderr,
			 "state in {insert,update}\n" );
		exit ( 1 );
	}

	program_name = argv[ 1 ];
	state = argv[ 2 ];

	if ( !*program_name
	||   strcmp( program_name, "program_name" ) == 0 )
	{
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		program_fetch_update( program_name );
	}
	return 0;
}

