/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/course.c		*/
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
#include "environ.h"
#include "boolean.h"
#include "list.h"
#include "program.h"
#include "course.h"

COURSE *course_parse(	char *input,
			boolean fetch_program,
			boolean fetch_alias_list )
{
	char course_name[ 128 ];
	char program_name[ 128 ];
	char description[ 1024 ];
	COURSE *course;

	if ( !input || !*input ) return (COURSE *)0;

	/* See: attribute_list course */
	/* -------------------------- */
	piece( course_name, SQL_DELIMITER, input, 0 );

	course = course_new( strdup( course_name ) );

	piece( program_name, SQL_DELIMITER, input, 1 );
	course->program_name = strdup( program_name );

	if ( fetch_program )
	{
		course->program =
			program_fetch(
				program_name,
				fetch_alias_list );
	}

	piece( description, SQL_DELIMITER, input, 2 );
	course->description = strdup( description );

	return course;
}

COURSE *course_new( char *course_name )
{
	COURSE *course;

	if ( ! ( course = calloc( 1, sizeof( COURSE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	course->course_name = course_name;
	return course;
}

COURSE *course_fetch(	char *course_name,
			boolean fetch_program,
			boolean fetch_alias_list )
{
	char sys_string[ 1024 ];
	COURSE *course;

	if ( !course_name || !*course_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty course_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 COURSE_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 course_primary_where( course_name ) );

	course =
		course_parse(
			pipe2string( sys_string ),
			fetch_program,
			fetch_alias_list );

	return course;
}

char *course_primary_where( char *course_name )
{
	char static where[ 128 ];

	sprintf( where,
		 "course_name = '%s'",
		 course_name_escape( course_name ) );

	return where;
}

char *course_escape_name( char *course_name )
{
	return course_name_escape( course_name );
}

char *course_name_escape( char *course_name )
{
	char name[ 256 ];

	return strdup( string_escape_quote_dollar( name, course_name ) );
}

char *course_program_name( COURSE *course )
{
	char *program_name;

	if ( !course )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty course.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( course->program )
	{
		program_name = 
			course->
				program->
				program_name;
	}
	else
	{
		program_name =
			course->
				course_name;
	}
	return program_name;
}
