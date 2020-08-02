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

char *course_select( void )
{
	return "course_name,course_price";
}

COURSE *course_parse( char *input_buffer )
{
	char course_name[ 128 ];
	char piece_buffer[ 128 ];
	COURSE *course;

	if ( !input_buffer ) return (COURSE *)0;

	piece( course_name, SQL_DELIMITER, input_buffer, 0 );

	course = course_new( strdup( course_name  ) );

	piece( piece_buffer, SQL_DELIMITER, input_buffer, 1 );
	course->course_price = atof( piece_buffer );

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
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 course_select(),
		 "course",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 course_primary_where(
			course_name ),
		 course_select() );

	return course_parse( pipe2string( sys_string ) );
}

/* Safely returns heap memory */
/* -------------------------- */
char *course_primary_where( char *course_name )
{
	char where[ 1024 ];

	sprintf( where,
		 "course_name = '%s'",
		 course_name );

	return strdup( where );
}

