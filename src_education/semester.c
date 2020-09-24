/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/semester.c		*/
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
#include "semester.h"
#include "offering_fns.h"
#include "registration_fns.h"

SEMESTER *semester_calloc( void )
{
	SEMESTER *semester;

	if ( ! ( semester = calloc( 1, sizeof( SEMESTER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return semester;
}

SEMESTER *semester_new(	char *season_name,
			int year )
{
	SEMESTER *semester = semester_calloc();

	semester->season_name = season_name;
	semester->year = year;
	return semester;
}

