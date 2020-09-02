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
#include "course.h"
#include "course.h"

COURSE *course_parse( char *input )
{
	char course_name[ 128 ];
	char piece_buffer[ 128 ];
	COURSE *course;

	if ( !input || !*input ) return (COURSE *)0;

	/* See: attribute_list course */
	/* -------------------------- */
	piece( course_name, SQL_DELIMITER, input, 0 );

	course = course_new( strdup( course_name  ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	course->course_price = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	course->program_name = strdup( piece_buffer );

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

COURSE *course_fetch( char *course_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 COURSE_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 course_primary_where( course_name ) );

	return course_parse( pipe2string( sys_string ) );
}

char *course_primary_where( char *course_name )
{
	char static where[ 128 ];

	sprintf( where,
		 "course_name = '%s'",
		 course_name );

	return where;
}

char *course_name_escape( char *course_name )
{
	static char name[ 256 ];

	return string_escape_quote( name, course_name );
}
