/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_program/program.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "list.h"
#include "program.h"

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

FILE *program_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement table=%s field=\"%s\" delimiter='%c'	|"
		"sql 2>&1						|"
		"grep -vi duplicate					|"
		"cat >%s 2>&1						 ",
		PROGRAM_TABLE,
		PROGRAM_INSERT_COLUMNS,
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void program_insert_pipe(
			FILE *insert_pipe,
			char *program_name )
{
	fprintf(insert_pipe,
		"%s\n",
		program_name );
}


