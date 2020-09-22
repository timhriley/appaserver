/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/student.c		*/
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
#include "student.h"

STUDENT *student_new(	char *student_full_name,
			char *street_address )
{
	STUDENT *student;

	if ( ! ( student = calloc( 1, sizeof( STUDENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	student->full_name = student_full_name;
	student->street_address = street_address;
	return student;
}

/* Returns static memory */
/* --------------------- */
char *student_name_escape( char *student_full_name )
{
	static char escape_full_name[ 256 ];

	return string_escape_quote( escape_full_name, student_full_name );
}
char *student_escape_name( char *student_full_name )
{
	return student_name_escape( student_full_name );
}

FILE *student_insert_open(
			char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement table=%s field=\"%s\" delimiter='%c'	|"
		"sql 2>&1						|"
		"grep -vi duplicate					|"
		"cat >%s						 ",
		STUDENT_TABLE,
		STUDENT_INSERT_COLUMNS,
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void student_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address )
{
	fprintf(insert_pipe,
		"%s^%s\n",
		student_escape_name( student_full_name ),
		street_address );
}

